#include <jni.h>
#include <android/log.h>
#include <string.h>

#define TAG "Primitive"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

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


// フィールドの文字列を結合して返す
jobject Java_com_example_jni_JavaStringActivity_concatFieldStrings(JNIEnv* env, jobject thiz){

  // オブジェクトのクラスを取得する
  jclass klass = (*env)->GetObjectClass(env, thiz);

  // フィールド変数(mStringFoo)の文字列を取得する
  jfieldID jfield1 = (*env)->GetFieldID(env, klass, "mStringFoo", "Ljava/lang/String;");  /////-----(1)
  if (jfield1 == NULL)
    return 0;
  jstring jstr1 = (*env)->GetObjectField(env, thiz, jfield1);  /////-----(2)

  // フィールド変数(mStringBar)の文字列を取得する
  jfieldID jfield2 = (*env)->GetStaticFieldID(env, klass, "mStringBar", "Ljava/lang/String;");
  if (jfield2 == NULL)
    return 0;
  jstring jstr2 = (*env)->GetStaticObjectField(env, klass, jfield2);

  // C言語の文字列に変換する
  const char* c_str1 = (*env)->GetStringUTFChars(env, jstr1,JNI_FALSE);
  const char* c_str2 = (*env)->GetStringUTFChars(env, jstr2,JNI_FALSE);

  // 文字列を結合する
  char buf[100] = {0};
  strcpy(buf, c_str1);
  strcat(buf, c_str2);

  // C言語の文字列からJavaの文字列を新規作成する
  jstring jstr = (*env)->NewStringUTF(env, buf);

  // 確保した文字列バッファを解放する
  (*env)->ReleaseStringUTFChars(env, jstr1, c_str1);
  (*env)->ReleaseStringUTFChars(env, jstr2, c_str2);

  // 文字列をセットする（例として）
  (*env)->SetObjectField(env, thiz, jfield1, jstr); /////-----(3)

  return jstr;
}


