LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := nativebitmap
LOCAL_SRC_FILES := nativebitmap.c
LOCAL_LDLIBS    := -llog \
                   -ljnigraphics #AndroidBitmapに必要

include $(BUILD_SHARED_LIBRARY)
