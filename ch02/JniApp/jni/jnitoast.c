#include <jni.h>
#include <stdio.h>
#include <android/log.h>

#define EXPORT __attribute__((visibility("default")))
#define LOG_TAG  "jnitoast"
#define LOGD(... ) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define LOGE(... ) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))

void Java_com_example_jni_JNIToastActivity_displayToast(JNIEnv* env, jobject thiz, jobject charseq) {

  // (Java)Toast toastobj = Toast.makeText(context, charseq, 0); に相当
  //Toastクラスを取得
  jclass toast = (*env)->FindClass(env, "android/widget/Toast");
  // ToastクラスのmakeTextスタティックメソッドのメソッドIDを取得する
  jmethodID methodMakeText =
      (*env)->GetStaticMethodID(
          env,
          toast,
          "makeText",
          "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;");
  if (methodMakeText == NULL) {
    LOGE("toast.makeText not Found");
    return;
  }

  // (Java)toastobj.makeText(this, charseq, 0);に相当
  jobject toastobj = (*env)->CallStaticObjectMethod(env, toast, methodMakeText,
                                                    thiz, charseq, 0);
  // (Java)toastobj.show();に相当
  jmethodID methodShow = (*env)->GetMethodID(env, toast, "show", "()V");
  (*env)->CallVoidMethod(env, toastobj, methodShow);

  return;
}
