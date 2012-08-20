#include <jni.h>
#include <android/log.h>
#include <string.h>
#include <math.h>

#define TAG "exception"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

/////begin exception_samplecode_02
// インデックスの値を取得(例外チェック＆例外発生)
jint Java_com_example_jni_ExceptionActivity_getValueOfData(JNIEnv* env,
                                                           jobject thiz,
                                                           jint index) {
  jthrowable throwObj;

  // IndexOutOfBoundsExceptionクラスを取得する
  jclass ioobe = (*env)->FindClass(env, "java/lang/IndexOutOfBoundsException");

  // getValuesメソッドを呼び出す
  jclass jklass = (*env)->GetObjectClass(env, thiz);                        /////-----(1)ここから
  jmethodID jmethod = (*env)->GetMethodID(env, jklass, "getValue", "(I)I");
  jint value = (*env)->CallIntMethod(env, thiz, jmethod, index);             /////-----(1)ここまで

  // 例外が発生しているか？
  throwObj = (*env)->ExceptionOccurred(env); /////-----(2)
  if (throwObj) {
    jclass throwKlass;

    // 発生した例外をログ表示する
    (*env)->ExceptionDescribe(env);

    // 発生した例外をクリアーする
    (*env)->ExceptionClear(env); /////-----(3)

    // Java側で発生した例外がIndexOutOfBoundsExceptionであるか？
    if ((*env)->IsInstanceOf(env, throwObj, ioobe) == JNI_TRUE) {  /////-----(4)
      LOGD("IndexOutOfBoundsExceptionが発生！");
    }

    // MyExceptionを発生させる
    throwKlass = (*env)->FindClass(env, "com/example/jni/MyException"); /////-----(5) ここから
    (*env)->ThrowNew(env, throwKlass, "throw MyException"); /////-----(5)　ここまで

    // 例外を投げた直後はreturnする
    return INT_MAX;
  }
  return value;
}
/////end
