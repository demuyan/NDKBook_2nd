LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := neon-jni
LOCAL_SRC_FILES := neon.c

include $(BUILD_SHARED_LIBRARY)
