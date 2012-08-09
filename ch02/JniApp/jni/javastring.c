#include <jni.h>
#include <android/log.h>
#include <string.h>

#define TAG "Primitive"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
/////begin jnistring_samplecode_01
// 文字列を結合して返す
jobject Java_com_example_jni_JavaStringActivity_concatStrings(JNIEnv* env, jobject thiz, jstring str1, jstring str2){ /////-----(1)

  // Javaの文字列をCで扱えるように変換する
  // 文字列バッファの確保も行われる
  const char* c_str1 = (*env)->GetStringUTFChars(env, str1,JNI_FALSE); /////-----(2)
  const char* c_str2 = (*env)->GetStringUTFChars(env, str2,JNI_FALSE);

  // 文字列を結合する
  char buf[100] = {0};
  strcpy(buf, c_str1); /////-----(3) ここから
  strcat(buf, c_str2); /////-----(3)　ここまで

  // 新規作成したJava文字列に格納する
  jstring jstr = (*env)->NewStringUTF(env, buf); /////-----(4)

  // 文字のバイト数を取得する
  LOGD("GetStringUTFLength=%d",(*env)->GetStringUTFLength(env, jstr)); /////-----(5)
  // 文字数を取得する
  LOGD("GetStringLength=%d",(*env)->GetStringLength(env, jstr)); /////-----(6)

  // 確保した文字列バッファを解放する
  (*env)->ReleaseStringUTFChars(env, str1, c_str1); /////-----(7)
  (*env)->ReleaseStringUTFChars(env, str2, c_str2);

  return jstr;
}
/////end
/////begin jnistring_samplecode_02
// フィールドの文字列を結合して返す
jobject Java_com_example_jni_JavaStringActivity_concatFieldStrings(JNIEnv* env, jobject thiz){

  // オブジェクトのクラスを取得する
  jclass klass = (*env)->GetObjectClass(env, thiz);
  // フィールドIDを取得する
  jfieldID jfield1 = (*env)->GetFieldID(env, klass, "mStringFoo", "Ljava/lang/String;");  /////-----(1)
  if (jfield1 == NULL)
    return 0;
  // フィールド変数の値を取得する
  jstring jstr1 = (*env)->GetObjectField(env, thiz, jfield1);  /////-----(2)
  // C言語の文字列に変換する
  const char* c_str1 = (*env)->GetStringUTFChars(env, jstr1,JNI_FALSE);

  // フィールドIDを取得する
  jfieldID jfield2 = (*env)->GetStaticFieldID(env, klass, "mStringBar", "Ljava/lang/String;");
  if (jfield2 == NULL)
    return 0;
  // フィールド変数の値を取得する
  jstring jstr2 = (*env)->GetStaticObjectField(env, klass, jfield2);
  // C言語の文字列に変換する
  const char* c_str2 = (*env)->GetStringUTFChars(env, jstr2,JNI_FALSE);

  // 文字列を結合する
  char buf[100] = {0};
  strcpy(buf, c_str1);
  strcat(buf, c_str2);

  // 新しい文字列に返す
  jstring jstr = (*env)->NewStringUTF(env, buf);

  // 確保した文字列バッファを解放する
  (*env)->ReleaseStringUTFChars(env, jstr1, c_str1);
  (*env)->ReleaseStringUTFChars(env, jstr2, c_str2);

  // 文字列をセットする
  (*env)->SetObjectField(env, thiz, jfield1, jstr); /////-----(3)

  return jstr;
}
/////end
/////begin field_samplecode_01
// フィールド変数を２つ取得、加算する
jfloat Java_com_example_jni_PrimitiveActivity1_addFieldValues(JNIEnv* env, jobject thiz){

  // クラスを取得する
  jclass klass = (*env)->GetObjectClass(env, thiz); /////-----(1)
  // フィールドIDを取得する
  jfieldID jfield1 = (*env)->GetFieldID(env, klass, "mValueFoo", "F"); /////-----(2)
  if (jfield1 == NULL)
    return 0;
  // フィールド変数(mValueFoo)の値を取得する
  jfloat val1 = (*env)->GetFloatField(env, thiz, jfield1); /////-----(3)
  LOGD("mValueFoo=%f",val1);

  // フィールドID(static)を取得する
  jfieldID jfield2 = (*env)->GetStaticFieldID(env, klass, "mValueBar", "F"); /////-----(4)
  if (jfield2 == NULL)
    return 0;
  // フィールド変数(mValueBar)の値を取得する
  jfloat val2 = (*env)->GetStaticFloatField(env, thiz, jfield2); /////-----(5)
  LOGD("mValueBar=%f",val2);
  // 加算する
  float answer = val1 + val2;
  // 加算結果をフィールド変数(mValueFoo)にセットする（サンプルとして）
  (*env)->SetFloatField(env, thiz, jfield1,answer);   /////-----(6)

  return answer;
}
/////end
