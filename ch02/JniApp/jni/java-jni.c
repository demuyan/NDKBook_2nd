#include <jni.h>
#include <android/log.h>
#include <string.h>

#define TAG "javaactivity"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

/////begin jniarray_samplecode_01
// 配列の数値を合計する
jint Java_com_example_jni_JavaActivity_sumArray(JNIEnv* env, jobject thiz,
                                                jintArray arr) {
  jint i, sum = 0;

  // 配列の長さを取得する
  jsize size = (*env)->GetArrayLength(env, arr); /////-----(1)

  // Java配列を元にC言語の配列を生成。先頭ポインタを返す。
  jint *buf = (*env)->GetIntArrayElements(env, arr, 0); /////-----(2)
  for (i = 0; i < size; i++) {
    sum += buf[i];
    // 配列の内容を変更する
    buf[i] += 1;
  }

  // メモリの解放
  (*env)->ReleaseIntArrayElements(env, arr, buf, JNI_ABORT); /////-----(3)

  return sum;
}
/////end
/////begin jniarray_samplecode_02
// 配列を生成する
jintArray Java_com_example_jni_JavaActivity_getArray(JNIEnv* env, jobject thiz,
                                                     jint size) {
  jintArray ary;

  // 配列を新規作成する
  ary = (*env)->NewIntArray(env, size); /////-----(1)
  if (ary == NULL) {
    return NULL;
  }

  // 配列にデータを入れる
  int i;
  jint fill[size];
  for (i = 0; i < size; i++) {
    fill[i] = i;
  }

  // Cの配列をJavaの配列に領域を指定してコピーする
  (*env)->SetIntArrayRegion(env, ary, 0, size, fill); /////-----(2)

  return ary;
}
/////end
