#include <jni.h>
#include "jnihelper.h"

#include <stdio.h>

#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

/////begin ch06_samplecode_9
// 指定したディレクトリ内のファイル一覧を取得する
jobject getAssetsFilelistInDir(JNIEnv* env, jclass thiz, jobject assetManager,
                               jstring dirName) {

  // AssetManagerを取得
  AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);  /////-----(1)
  // 指定したディレクトリを開く
  const jbyte *dname = (*env)->GetStringUTFChars(env, dirName, NULL); /////-----(2) ここから
  AAssetDir* assetDir = AAssetManager_openDir(mgr, dname); 
  (*env)->ReleaseStringUTFChars(env, dirName, dname); /////-----(2)ここまで

  // 先頭のポインタに移動する
  AAssetDir_rewind(assetDir);  /////-----(3)

  // ファイル名を保存する領域を確保
  char* filename;
  int count = 0;
  while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) { 
    count++;
  };
  jobjectArray objary = (jobjectArray) (*env)->NewObjectArray(
      env, count, (*env)->FindClass(env, "java/lang/String"),
      (*env)->NewStringUTF(env, ""));

  // 先頭のポインタに移動する
  AAssetDir_rewind(assetDir);
  // String配列にファイル名を格納する
  int i = 0;
  while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {  /////-----(4) ここから
    (*env)->SetObjectArrayElement(env, objary, i,
                                  (*env)->NewStringUTF(env, filename));
    i++;
  } /////-----(4) ここまで

  // ディレクトリを閉じる
  AAssetDir_close(assetDir); /////-----(5)

  return objary;
}
/////end
/////begin ch06_samplecode_10
// 指定したファイルを読み込んで返す
jobject getAssetsReadTextfile(JNIEnv* env, jclass thiz, jobject assetManager,
                          jstring filename) {

  // AssetManagerを取得
  AAssetManager* mgr = AAssetManager_fromJava(env, assetManager); 

  // 指定したファイルをオープンする
  const jbyte *fname = (*env)->GetStringUTFChars(env, filename, NULL); /////-----(1) ここから
  AAsset* assetFile = AAssetManager_open(mgr, fname, AASSET_MODE_RANDOM);  
  (*env)->ReleaseStringUTFChars(env, filename, fname); /////-----(1) ここまで

  // ファイルサイズを取得
  int length = AAsset_getLength(assetFile); /////-----(2)

  // バッファを取得
  u_char *buf = (u_char*) malloc(length + 1);
  bzero(buf, length + 1);

  // ファイルを読み込み
  AAsset_read(assetFile, buf, length); /////-----(3)

  // UTF-8に変換
  jobject jstr = (*env)->NewStringUTF(env, buf);

  // バッファを解放
  free(buf);

  // ファイルを閉じる
  AAsset_close(assetFile);  /////-----(4)

  return jstr;
}
/////end
static JNINativeMethod sMethods[] = {
/* name, signature, funcPtr */
{ "getAssetsFilelistInDir",
    "(Landroid/content/res/AssetManager;Ljava/lang/String;)[Ljava/lang/String;",
    (void*) getAssetsFilelistInDir },
{ "getAssetsReadTextfile",
    "(Landroid/content/res/AssetManager;Ljava/lang/String;)Ljava/lang/String;",
    (void*) getAssetsReadTextfile }, };

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
