#include <jni.h>
#include <stdio.h>
#include <android/log.h>


#ifndef _jnihelper_h
#define _jnihelper_h
#ifdef __cplusplus
extern "C" {
#endif

#ifndef NELEM
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

#ifdef __cplusplus
}
#endif
#endif
