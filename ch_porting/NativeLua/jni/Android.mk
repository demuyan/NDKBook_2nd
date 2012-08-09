#/////begin lua_samplecode_03
LOCAL_PATH := $(call my-dir)

# libpngをAndroidで利用できるように
# prebuiltしたライブラリは直接リンクできない
include $(CLEAR_VARS)
LOCAL_MODULE := liblua-prebuilt
LOCAL_SRC_FILES := lib/liblua.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_STATIC_LIBRARY)

# 共有モジュールを作成する
include $(CLEAR_VARS)
LOCAL_MODULE    := nativelua-jni
LOCAL_SRC_FILES := nativelua.c
LOCAL_STATIC_LIBRARIES := liblua-prebuilt
include $(BUILD_SHARED_LIBRARY)
#/////end