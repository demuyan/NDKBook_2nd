LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := movieplayer-jni
LOCAL_SRC_FILES := main.c jnihelper.c
# for native multimedia
LOCAL_LDLIBS    += -lOpenMAXAL
# for logging
LOCAL_LDLIBS    += -llog
# for native windows
LOCAL_LDLIBS    += -landroid

LOCAL_CFLAGS    += -UNDEBUG

include $(BUILD_SHARED_LIBRARY)