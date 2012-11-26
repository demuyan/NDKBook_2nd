
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

#生成するモジュール名
LOCAL_MODULE    := native-activity
#ソースコード
LOCAL_SRC_FILES := main.c
#リンクするライブラリ
LOCAL_LDLIBS    := -llog \
                   -landroid \        #=====(1)
                   -lEGL -lGLESv1_CM
#静的リンクする外部モジュール
LOCAL_STATIC_LIBRARIES := android_native_app_glue #=====(2)
#共有モジュールとしてビルドする
include $(BUILD_SHARED_LIBRARY)
#外部モジュールのパス指定
$(call import-module,android/native_app_glue) #=====(3)
