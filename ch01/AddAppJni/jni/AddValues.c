#include <jni.h>

// 加算を行う
jint Java_com_example_addapp_MainActivity_addValues(JNIEnv* env, jobject thiz,
    jint value1, jint value2) {

  return value1 + value2;
}

