#ifndef PNG_ANDROID_H
#define PNG_ANDROID_H

#include <jni.h>
#include <errno.h>
#include <android/asset_manager.h>
#include <android/log.h>
#include <png.h>

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#define  LOG_TAG    "libpng_android"
// デバッグ用メッセージ(Infomation)
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
// デバッグ用メッセージ(Warning)
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
// デバッグ用メッセージ(Error)
#define LOGE(...)  ((void)__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__))

int loadPngImage(AAssetManager* mgr, char *filename, png_uint_32* outWidth, png_uint_32* outHeight, GLint *type, u_char **outData);

#endif
