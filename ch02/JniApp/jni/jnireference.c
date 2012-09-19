#include <jni.h>
#include <stdio.h>
#include <android/log.h>

#define TAG "JniReference"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

/////begin jni_locref_01
static jobject localString,localFrameString;

void Java_com_example_jni_JniReferenceActivity_setStringNG(JNIEnv* env, jobject thiz, jobject obj){

  localString = obj; // ローカル参照扱い
}
/////end
/////begin jni_locref_02
void Java_com_example_jni_JniReferenceActivity_setStringOK(JNIEnv* env, jobject thiz, jobject obj){
  // グローバル参照にする
  localString = (*env)->NewGlobalRef(env,obj);
}
/////end
/////begin jni_locref_03
void Java_com_example_jni_JniReferenceActivity_getString(JNIEnv* env, jobject thiz){

  const char* str = (*env)->GetStringUTFChars(env, localString,JNI_FALSE);
  LOGD("%s\n",str);
}
/////end
/////begin jni_locref_04
jobject Java_com_example_jni_JniReferenceActivity_getFrameStringNG(JNIEnv* env, jobject thiz){

  // ローカルフレームを確保する
  (*env)->PushLocalFrame(env,10);

  // 新規作成したJava文字列に格納する
  jobject jstr = (*env)->NewStringUTF(env, "LocalFrame(NG)");

  // ローカルフレームを解放する
  (*env)->PopLocalFrame(env,NULL);

  localFrameString = (*env)->NewGlobalRef(env,jstr); // この時点でjstrは無効となる

  return localFrameString;
}
/////end
/////begin jni_locref_05
jobject Java_com_example_jni_JniReferenceActivity_getFrameStringOK(JNIEnv* env, jobject thiz){

  // ローカルフレームを確保する
  (*env)->PushLocalFrame(env,10);

  // 新規作成したJava文字列に格納する
  jstring jstr = (*env)->NewStringUTF(env, "LocalFrame(OK)");

  // ローカルフレームを解放する
  // jstr2はローカル参照として確保
  jstring jstr2 = (*env)->PopLocalFrame(env,jstr);

  // jstr2をグローバル参照にする
  localFrameString = (*env)->NewGlobalRef(env,jstr2);

  return localFrameString;
}
/////end
