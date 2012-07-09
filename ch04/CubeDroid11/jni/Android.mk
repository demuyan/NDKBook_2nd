#/////begin p98_middle_samplecode
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# 出力するモジュール名
LOCAL_MODULE    := cubedroid11
# モジュールを構成するC/C++のソースコード
LOCAL_SRC_FILES := main.c glu.c
# リンクするライブラリ
LOCAL_LDLIBS    := -llog \ 
                   -landroid \  #Android関連（NativeActivityなど）ライブラリ 
                   -lEGL \　     #EGLライブラリ
                   -lGLESv1_CM  #OpenGL|ES1.1ライブラリ
# native_app_glue のリンク指定
LOCAL_STATIC_LIBRARIES := android_native_app_glue 

# 共有ライブラリとして出力する
include $(BUILD_SHARED_LIBRARY)

# native_app_glue の呼び出し指定
$(call import-module,android/native_app_glue)
#/////end