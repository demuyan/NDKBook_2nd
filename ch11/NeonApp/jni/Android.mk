
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := neon-jni
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)       
  LOCAL_ARM_NEON  := true
  LOCAL_SRC_FILES := multi_neon.s multi.c.arm.neon
else
  LOCAL_SRC_FILES := multi_neon.c multi.c.arm
endif

include $(BUILD_SHARED_LIBRARY)
