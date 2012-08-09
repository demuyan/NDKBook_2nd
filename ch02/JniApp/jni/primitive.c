#include <jni.h>
#include <android/log.h>
#include <string.h>
#include <math.h>

#define TAG "Primitive"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

// 引数の数値を加算して返す
jfloat Java_com_example_jni_PrimitiveActivity1_addValues(JNIEnv* env, jobject thiz, jfloat val1, jfloat val2){
  LOGD("val1=%f",val1);
  LOGD("val2=%f",val2);
  return val1+val2;
}

// 引数の文字列を結合して返す
jobject Java_com_example_jni_PrimitiveActivity1_concatStrings(JNIEnv* env, jobject thiz, jstring str1, jstring str2){

  char buf[100] = {0};

  // Javaの文字列をCで扱えるように変換する
  // 文字列バッファの確保も行われる
  const jchar* c_str1 = (*env)->GetStringUTFChars(env, str1,JNI_FALSE);
  const jchar* c_str2 = (*env)->GetStringUTFChars(env, str2,JNI_FALSE);

  // ログ出力
  LOGD("c_str1=%s",c_str1);
  LOGD("c_str2=%s",c_str2);

  // 文字列を結合する
  strcpy(buf, c_str1);
  strcat(buf, c_str2);

  // 新しい文字列に返す
  jstring jstr = (*env)->NewStringUTF(env, buf);

  // 確保した文字列バッファを解放する
  (*env)->ReleaseStringChars(env, str1, c_str1);
  (*env)->ReleaseStringChars(env, str2, c_str2);

  return jstr;
}

// フィールドの文字列を結合して返す
jobject Java_com_example_jni_PrimitiveActivity1_concatFieldStrings(JNIEnv* env, jobject thiz){

  jclass klass = (*env)->GetObjectClass(env, thiz);
  jfieldID jfield1 = (*env)->GetFieldID(env, klass, "mStringFoo", "Ljava/lang/String;");
  if (jfield1 == NULL)
    return 0;
  jstring jstr1 = (*env)->GetObjectField(env, thiz, jfield1);

  const jchar* c_str1 = (*env)->GetStringUTFChars(env, jstr1,JNI_FALSE);
  LOGD("c_str1=%s",c_str1);

  jfieldID jfield2 = (*env)->GetStaticFieldID(env, klass, "mStringBar", "Ljava/lang/String;");
  if (jfield2 == NULL)
    return 0;
  jstring jstr2 = (*env)->GetStaticObjectField(env, thiz, jfield2);
  const jchar* c_str2 = (*env)->GetStringUTFChars(env, jstr2,JNI_FALSE);
  // ログ出力
  LOGD("c_str2=%s",c_str2);

  char buf[100] = {0};

  // Javaの文字列をCで扱えるように変換する
  // 文字列バッファの確保も行われる


  // 文字列を結合する
  strcpy(buf, c_str1);
  strcat(buf, c_str2);

  // 新しい文字列に返す
  jstring jstr = (*env)->NewStringUTF(env, buf);

  // 確保した文字列バッファを解放する
  (*env)->ReleaseStringChars(env, jstr1, c_str1);
  (*env)->ReleaseStringChars(env, jstr2, c_str2);

  // 文字列をセットする
  (*env)->SetObjectField(env, thiz, jfield1, jstr);

  return jstr;
}



// フィールド値２つを取得して加算して返す
jfloat Java_com_example_jni_PrimitiveActivity1_addFieldValues(JNIEnv* env, jobject thiz){

  // クラスを取得する
  jclass klass = (*env)->GetObjectClass(env, thiz);
  // フィールド変数(mValueFoo)の値を取得する
  jfieldID jfield1 = (*env)->GetFieldID(env, klass, "mValueFoo", "F");
  if (jfield1 == NULL)
    return INFINITY;
  jfloat val1 = (*env)->GetFloatField(env, thiz, jfield1);
  LOGD("mValueFoo=%f",val1);

  jfieldID jfield2 = (*env)->GetStaticFieldID(env, klass, "mValueBar", "F");
  if (jfield2 == NULL)
    return INFINITY;
  jfloat val2 = (*env)->GetStaticFloatField(env, klass, jfield2);
  LOGD("mValueBar=%f",val2);

  float answer = val1 + val2;
  (*env)->SetFloatField(env, thiz, jfield1,answer);

  return val1+val2;
}
/////begin klass_samplecode_01
// 文字列を数値に変換し加算して返す
jfloat Java_com_example_jni_PrimitiveActivity1_addValuesStr(JNIEnv* env,
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
