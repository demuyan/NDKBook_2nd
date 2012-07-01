#include <jni.h>
#include "jnihelper.h"

#include <stdio.h>

#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

jobject getAssetsFilelistInDir(JNIEnv* env, jclass thiz, jobject assetManager,
		jstring dirName) {

	// AssetManagerを取得
	AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);

	// ディレクトリ名をchar型に変換して取得
	const jbyte *dname = (*env)->GetStringUTFChars(env, dirName, NULL);
	// ディレクトリを開く
	AAssetDir* assetDir = AAssetManager_openDir(mgr, dname);
	// UTF文字列を開放する
	(*env)->ReleaseStringUTFChars(env, dirName, dname);
	// 先頭のポインタに移動する
	AAssetDir_rewind(assetDir);

	// ファイルの数をカウントする
	char* filename;
	int count = 0;
	while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
		count++;
	};

	// Stringの配列を新規作成する
	jobjectArray objary = (jobjectArray) (*env)->NewObjectArray(env, count,
			(*env)->FindClass(env, "java/lang/String"),
			(*env)->NewStringUTF(env, ""));

	// 先頭のポインタに移動する
	AAssetDir_rewind(assetDir);
	// String配列にファイル名を格納する
	int i = 0;
	while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
		(*env)->SetObjectArrayElement(env, objary, i,
				(*env)->NewStringUTF(env, filename));
		i++;
	}
	// ディレクトリを閉じる
	AAssetDir_close(assetDir);

	// String配列を返す
	return objary;
}

jobject getAssetsReadFile(JNIEnv* env, jclass thiz, jobject assetManager,
		jstring filename) {

	// AssetManagerを取得
	AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);

	// ファイル名をchar型に変換して取得
	const jbyte *fname = (*env)->GetStringUTFChars(env, filename, NULL);

	// ファイルをオープンする
	AAsset* assetFile = AAssetManager_open(mgr, fname, AASSET_MODE_RANDOM);
	// 文字列バッファを開放する
	(*env)->ReleaseStringUTFChars(env, filename, fname);

	// 長さを取得
	int length = AAsset_getLength(assetFile);
	// 読み込みバッファを取得
	u_char *buf = (u_char*)malloc(length+1);
	bzero(buf, length+1);
	// ファイルを読み込み
	AAsset_read(assetFile, buf, length);

	// UTF-8に変換
	jobject jstr = (*env)->NewStringUTF(env, buf);

	// 読み込みバッファを開放
	free(buf);

	// 文字列を返す
	return jstr;
}

static JNINativeMethod sMethods[] =
		{
				/* name, signature, funcPtr */
				{ "getAssetsFilelistInDir",
						"(Landroid/content/res/AssetManager;Ljava/lang/String;)[Ljava/lang/String;",
						(void*) getAssetsFilelistInDir },
				{ "getAssetsReadFile",
						"(Landroid/content/res/AssetManager;Ljava/lang/String;)Ljava/lang/String;",
						(void*) getAssetsReadFile },
		};

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv* env = NULL;
	jint result = -1;

	if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK) {
		return result;
	}

	jniRegisterNativeMethods(env, "com/example/accessassets/MainActivity",
			sMethods, NELEM(sMethods));
	return JNI_VERSION_1_6;
}
