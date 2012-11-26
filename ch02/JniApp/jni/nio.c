#include <stdio.h>
#include <android/log.h>
#include "nio.h"

#define EXPORT __attribute__((visibility("default")))
#define LOG_TAG "jninio"
#define LOGD(... ) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define LOGE(... ) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))


// メモリに格納されているデータ構造
typedef struct _DataInfo {
  int a;
  short b;
  char c;
} DataInfo __attribute__ ((packed));

void Java_com_example_jni_NioActivity_calcBuffer(JNIEnv* env, jobject thiz,
                                                 jobject buf) {

  DataInfo *dataInfo;

  // メモリから直接値を取得する
  dataInfo = (*env)->GetDirectBufferAddress(env, buf); /////-----(1)

  // 値を加算する（メモリに直接反映させる）
  dataInfo->a += 10; /////-----(2)ここから
  dataInfo->b += 10;
  dataInfo->c += 10; /////-----(2)ここまで

  int capacity = (*env)->GetDirectBufferCapacity(env, buf); /////-----(3)
  LOGD("Capacity=%d", capacity);
}


jobject Java_com_example_jni_NioActivity_getBuffer(JNIEnv* env, jobject thiz,
                                                   jint size) {
  // メモリ確保
  void* buffer = malloc(size); /////-----(1)
  DataInfo *dataInfo = buffer;

  // 値を設定する
  dataInfo->a = 100;
  dataInfo->b = 200;
  dataInfo->c = 0;

  // byteBufferとして割り当てる
  jobject directBuffer = (*env)->NewDirectByteBuffer(env, buffer, size);
  // グローバル参照とする
  jobject globalRef = (*env)->NewGlobalRef(env, directBuffer);

  return globalRef;
}

