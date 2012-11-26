
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
  jclass toast = (*env)->FindClass(env, "android/widget/Toast");       /////-----(1)

  // ToastクラスのmakeTextスタティックメソッドのメソッドIDを取得する
  jmethodID methodMakeText =                                           /////-----(2)
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
  jobject toastobj = (*env)->CallStaticObjectMethod(env, toast, methodMakeText,      /////-----(3)
                                                    thiz, charseq, 0);

  // (Java)toastobj.show();に相当
  jmethodID methodShow = (*env)->GetMethodID(env, toast, "show", "()V");       /////-----(4)
  if (methodShow == NULL) {
    LOGE("toast.show not Found");
    return;
  }
  (*env)->CallVoidMethod(env, toastobj, methodShow); /////-----(5)

  return;
}

