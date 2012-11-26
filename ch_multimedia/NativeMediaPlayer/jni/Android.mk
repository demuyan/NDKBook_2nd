
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := movieplayer-jni
LOCAL_SRC_FILES := main.c jnihelper.c
LOCAL_LDLIBS    += -llog 
LOCAL_LDLIBS    += -lOpenMAXAL  #/////=====(1)
LOCAL_LDLIBS    += -landroid    #/////======(2)

include $(BUILD_SHARED_LIBRARY)

