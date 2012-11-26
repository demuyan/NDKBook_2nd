
#ifndef _jnihelp_h
#define _jnihelp_h

#include <jni.h>
#include <stdio.h>
#include <android/log.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TAG "JniHelper"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

#ifndef NELEM
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

#ifdef __cplusplus
}
#endif
#endif

