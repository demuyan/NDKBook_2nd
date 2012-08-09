#include <stdio.h>
#include <android/log.h>
#include "nio.h"

#define EXPORT __attribute__((visibility("default")))
#define LOG_TAG "jninio"
#define LOGD(... ) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define LOGE(... ) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))

/////begin nio_samplecode_02
// メモリに格納されているデータ構造
typedef struct _DataInfo {
  int   a;
  short b;
  char  c;
} DataInfo  __attribute__ ((packed));

void Java_com_example_jni_NioActivity_calcBuffer(JNIEnv* env, jobject thiz, jobject buf) {

  DataInfo *dataInfo;

  // メモリから直接値を取得する
  dataInfo = (*env)->GetDirectBufferAddress(env, buf);

  // 値を加算する（メモリに直接反映させる）
  dataInfo->a += 10;
  dataInfo->b += 10;
  dataInfo->c += 10;

  int capacity = (*env)->GetDirectBufferCapacity(env, buf);
  LOGD("Capacity=%d",capacity);
}
/////end

jobject Java_com_example_jni_NioActivity_getBuffer(JNIEnv* env, jobject thiz, jint size) {

  void* buffer = malloc(size);
  DataInfo *dataInfo = buffer;

  // 値を設定する
  dataInfo->a = 100;
  dataInfo->b = 200;
  dataInfo->c = 0;

  jobject directBuffer = (*env)->NewDirectByteBuffer(env, buffer, size);
  jobject globalRef = (*env)->NewGlobalRef(env,directBuffer);

  return globalRef;
}


//int jniRegisterNativeMethods(JNIEnv* env, const char* className,
//                             const JNINativeMethod* gMethods, int numMethods) {
//  jclass clazz;
//
//  LOGD("Registering %s natives\n", className);
//  clazz = (*env)->FindClass(env, className);
//  if (clazz == NULL) {
//    LOGD("Native registration unable to find class '%s'\n", className);
//    return -1;
//  }
//  if ((*env)->RegisterNatives(env, clazz, gMethods, numMethods) < 0) {
//    LOGD("RegisterNatives failed for '%s'\n", className);
//    return -1;
//  }
//  return 0;
//}
//
//static JNINativeMethod sMethods[] = {
///* name, signature, funcPtr */
//{ "setByteBuffer", "(Ljava/nio/Buffer;)I", (void*) setByteBuffer }, };
//
//EXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
//  JNIEnv* env = NULL;
//  jint result = -1;
//
//  if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK) {
//    return result;
//  }
//
//  jniRegisterNativeMethods(env, "com/example/jni/NioActivity", sMethods,
//                           NELEM(sMethods));
//  return JNI_VERSION_1_6;
//}
