
#include <jni.h>
#include <jnihelp.h>
#include <stdio.h>

int jniRegisterNativeMethods(JNIEnv* env, const char* className,
                             const JNINativeMethod* gMethods, int numMethods);

// 加算を行う
jint addValues(JNIEnv* env, jobject thiz, jint value1, jint value2) { /////-----(1)
  return value1 + value2;
}

// 登録するメソッド一覧
static JNINativeMethod sMethods[] = {            /////-----(2)ここから
/* メソッド名, シグネチャ, 関数ポインタ */
{ "addValues", "(II)I", (void*) addValues }, };  /////-----(2)ここまで

// ロード時に呼ばれる関数
jint JNI_OnLoad(JavaVM* vm, void* reserved) {   /////-----(3)ここから
  JNIEnv* env = NULL;
  jint result = -1;

  // JNIのバージョンチェック
  if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK) {
    return result;
  }
  // リストにある関数をJavaのメソッドとして登録
  jniRegisterNativeMethods(env, "com/example/jni/AddAppActivity", sMethods,
                           NELEM(sMethods));
  return JNI_VERSION_1_6;                           /////-----(3)ここまで
}

// Javaのメソッドとして登録する
int jniRegisterNativeMethods(JNIEnv* env, const char* className,
                             const JNINativeMethod* gMethods, int numMethods) {  /////-----(4)ここから
  jclass clazz;

  // 登録を行うクラスを検索する
  LOGD("Registering %s natives\n", className);
  clazz = (*env)->FindClass(env, className);
  if (clazz == NULL) {
    // 該当するクラスが見つからなかった
    LOGD("Native registration unable to find class '%s'\n", className);
    return -1;
  }

  // メソッドを登録する
  if ((*env)->RegisterNatives(env, clazz, gMethods, numMethods) < 0) {
    LOGD("RegisterNatives failed for '%s'\n", className);
    return -1;
  }
  return 0;
}                                                                                                  /////-----(4)ここまで

