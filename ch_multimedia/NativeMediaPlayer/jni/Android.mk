#//////begin ch_multi_samplecode_1
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := movieplayer-jni
LOCAL_SRC_FILES := main.c jnihelper.c
LOCAL_LDLIBS    += -llog \
                   -lOpenMAXAL #/////-----(1)
                   -landroid   #/////-----(2)

LOCAL_CFLAGS    += -UNDEBUG

include $(BUILD_SHARED_LIBRARY)
#//////end
