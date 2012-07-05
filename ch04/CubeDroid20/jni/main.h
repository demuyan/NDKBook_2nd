#ifndef __MAIN_H
#define __MAIN_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include "Matrix.hpp"

#include <jni.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

// アプリ動作再開に必要なデータ
struct saved_state
{
  int dummy;
};

// アプリケーション内で共通して利用する情報
struct engine
{
  struct android_app* app;

  // センサー関連
  ASensorManager* sensorManager;
  const ASensor* accelerometerSensor;
  const ASensor* gyroscopeSensor;
  ASensorEventQueue* sensorEventQueue;

  // アニメーションフラグ
  int animating;

  // EGL
  EGLDisplay display;
  EGLSurface surface;
  EGLContext context;

  // 画面解像度
  int32_t width;
  int32_t height;

  float angle[3];
  // 保存データ
  struct saved_state state;
};


#endif

