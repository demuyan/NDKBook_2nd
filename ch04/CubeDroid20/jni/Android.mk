LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libcubedroid2
LOCAL_CFLAGS    := -Werror
LOCAL_SRC_FILES := main.cpp gl_code.cpp
LOCAL_LDLIBS    := -llog -lGLESv2 -landroid -lEGL 
LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
