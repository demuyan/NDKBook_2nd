#include <jni.h>
#include <android/log.h>
#include <string.h>
#include <math.h>

#define TAG "Primitive"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

// 引数の数値を加算して返す
jfloat Java_com_example_jni_PrimitiveActivity_addValues(JNIEnv* env, jobject thiz, jfloat val1, jfloat val2){
  LOGD("val1=%f",val1);
  LOGD("val2=%f",val2);
  return val1+val2;
}

/////begin field_samplecode_01
// フィールド変数を２つ取得、加算する
jfloat Java_com_example_jni_PrimitiveActivity_addFieldValues(JNIEnv* env, jobject thiz){

  // クラスを取得する
  jclass klass = (*env)->GetObjectClass(env, thiz); /////-----(1)

  // フィールド変数(mValueFoo)の値を取得する
  jfieldID jfield1 = (*env)->GetFieldID(env, klass, "mValueFoo", "F"); /////-----(2)
  if (jfield1 == NULL)
    return INFINITY;
  jfloat val1 = (*env)->GetFloatField(env, thiz, jfield1); /////-----(3)

  // スタティックフィールド変数(mValueBar)の値を取得する
  jfieldID jfield2 = (*env)->GetStaticFieldID(env, klass, "mValueBar", "F"); /////-----(4)
  if (jfield2 == NULL)
    return INFINITY;
  jfloat val2 = (*env)->GetStaticFloatField(env, klass, jfield2); /////-----(5)

  // 加算する
  float answer = val1 + val2;

  // 加算結果をフィールド変数(mValueFoo)にセットする（例として）
  (*env)->SetFloatField(env, thiz, jfield1,answer);   /////-----(6)

  return answer;
}
/////end
/////begin klass_samplecode_01
// 文字列を数値に変換し加算して返す
jfloat Java_com_example_jni_PrimitiveActivity_addValuesStr(JNIEnv* env,
                                                            jobject thiz,
                                                            jstring str1,
                                                            jstring str2) {
  // クラスを取得する
  jclass jklass = (*env)->FindClass(env, "java/lang/Float"); /////-----(1)

  // メソッドIDを取得する
  jmethodID jmethod = (*env)->GetStaticMethodID(env, jklass, "parseFloat", /////-----(2)
                                                "(Ljava/lang/String;)F");
  if (jmethod == NULL)  /////-----(3)
    return INFINITY;

  // クラスとメソッドIDを指定してスタティックメソッドを呼び出す
  jfloat value1 = (*env)->CallStaticFloatMethod(env, jklass, jmethod, str1); /////-----(4)
  jfloat value2 = (*env)->CallStaticFloatMethod(env, jklass, jmethod, str2);

  return value1 + value2;
}
/////end
