
LOCAL_PATH := $(call my-dir)

# libpngをAndroidで利用できるように
# prebuiltしたライブラリは直接リンクできない
include $(CLEAR_VARS)                            #/////-----(1)ここから
LOCAL_MODULE := libpng15-prebuilt
LOCAL_SRC_FILES := lib/libpng15.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include   
include $(PREBUILT_STATIC_LIBRARY)               #/////-----(1)ここまで

# アプリにリンクするモジュールを生成する
include $(CLEAR_VARS)
LOCAL_MODULE    := cubedroid
LOCAL_SRC_FILES := main.c libpng_android.c glu.c
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv1_CM \
                   -lz  # libpngはlibzをリンクする必要がある
LOCAL_STATIC_LIBRARIES := android_native_app_glue  # NativeActivityGlue
LOCAL_STATIC_LIBRARIES += libpng15-prebuilt        # libpng15  #/////-----(2)
include $(BUILD_SHARED_LIBRARY)

# NativeActivityGlueのモジュール
$(call import-module,android/native_app_glue)
