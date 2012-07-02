LOCAL_PATH := $(call my-dir)
# 変数を初期化
include $(CLEAR_VARS)

# 出力するモジュール名
LOCAL_MODULE    := process
# モジュールを構成するC/C++のソースコード
LOCAL_SRC_FILES := main.c
# 共有ライブラリとしてビルドを行う
include $(BUILD_SHARED_LIBRARY)