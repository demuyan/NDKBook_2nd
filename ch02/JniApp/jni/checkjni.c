#include <jni.h>
#include <android/log.h>
#include <string.h>
#include <math.h>

#define TAG "CheckJni"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

/////begin jnicheck_samplecode_01
// 引数の数値を加算して返す
jfloat Java_com_example_jni_CheckJniActivity_addFieldValues(JNIEnv* env, jobject thiz){

  // クラスを取得する
  jclass klass = (*env)->GetObjectClass(env, thiz);
  // フィールド変数(mValueFoo)の値を取得する
  jfieldID jfield1 = (*env)->GetFieldID(env, klass, "mValueFoo", "F");
  if (jfield1 == NULL)
    return INFINITY;
  jfloat val1 = (*env)->GetFloatField(env, thiz, jfield1);

  // スタティックフィールド変数(mValueBar)の値を取得する
  jfieldID jfield2 = (*env)->GetStaticFieldID(env, klass, "mValueBar", "F");
  if (jfield2 == NULL)
    return INFINITY;
  jfloat val2 = (*env)->GetStaticFloatField(env, thiz, jfield2); // ここでエラーが発生する

  // 加算する
  float answer = val1 + val2;
  // フィールド変数(mValueFoo)の値を格納する
  (*env)->SetFloatField(env, thiz, jfield1,answer);

  return val1+val2;
}
/////end
