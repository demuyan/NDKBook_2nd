
LOCAL_PATH := $(call my-dir)

# prebuildされたモジュールをリンクできるようにする
include $(CLEAR_VARS)                                #/////-----(1)ここから
LOCAL_MODULE := libpng15-prebuilt
LOCAL_SRC_FILES := libpng15.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_STATIC_LIBRARY)                   #/////-----(1)ここまで

# 動的モジュールのビルド
include $(CLEAR_VARS)
LOCAL_MODULE    := cubedroid11tex
LOCAL_SRC_FILES := main.c glu.c libpng_android.c
LOCAL_LDLIBS    := -llog -lz -landroid -lEGL -lGLESv1_CM              #/////-----(2)
LOCAL_STATIC_LIBRARIES := android_native_app_glue libpng15-prebuilt   #/////-----(3)

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
