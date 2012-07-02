LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libpng15-prebuilt
LOCAL_SRC_FILES := libpng15.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := cubedroid
LOCAL_SRC_FILES := main.c libpng_android.c
LOCAL_LDLIBS    := -llog -lz -landroid -lEGL -lGLESv1_CM 
LOCAL_STATIC_LIBRARIES := android_native_app_glue libpng15-prebuilt

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
