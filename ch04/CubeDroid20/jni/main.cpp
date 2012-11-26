#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "main.h"

#define LOG_TAG    "cubedroid20"
// デバッグ用メッセージ(Infomation)
#define LOGI(...)  ((void)__android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__))
// デバッグ用メッセージ(Warning)
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
// デバッグ用メッセージ(Error)
#define LOGE(...)  ((void)__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__))

extern void initCube(struct engine* engine);
extern void prepareFrame(struct engine* engine);
extern void drawCube(struct engine* engine);

// EGL初期化
static int engine_init_display(struct engine* engine) {

  EGLint w, h, dummy, format;
  EGLint numConfigs;
  EGLConfig config;
  EGLSurface surface;
  EGLContext context;

  // 有効にするEGLパラメータ
  const EGLint attribs[] = {
  // レンダリングのタイプにGLES2.0を指定
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, /////-----(1)
      //　サーフェイスのタイプを指定(ダブルバッファを利用するのでEGL_WINDOW_BIT)
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      //　青が利用する最小フレームサイズ(単位はbit)
      EGL_BLUE_SIZE, 8,
      //　緑が利用する最小フレームサイズ(単位はbit)
      EGL_GREEN_SIZE, 8,
      //　赤が利用する最小フレームサイズ(単位はbit)
      EGL_RED_SIZE, 8,
      //　デプスバッファとして確保するサイズ(単位はbit)
      EGL_DEPTH_SIZE, 16,
      //　終端
      EGL_NONE };

  // EGLディスプレイコネクションを取得
  EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  // EGLディスプレイコネクション初期化
  eglInitialize(display, 0, 0);
  // 条件に合ったEGLフレームバッファ設定のリストを返す
  eglChooseConfig(display, attribs, &config, 1, &numConfigs);
  // EGLフレームバッファ設定の情報を取得
  eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
  // NativeActivityへバッファを設定
  ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);
  // EGLウィンドウサーフェイスの取得
  surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
  // EGLレンダリングコンテキストの取得
  const EGLint attrib_list[] = { EGL_CONTEXT_CLIENT_VERSION, 2, // GLES|2.0を指定  /////-----(2) ここから
                                 EGL_NONE }; 
  context = eglCreateContext(display, config, NULL, attrib_list);                  /////-----(2) ここまで
  // EGLレンダリングコンテキストをEGLサーフェイスにアタッチする
  if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
    LOGW("Unable to eglMakeCurrent");
    return -1;
  }

  // 画面解像度取得
  eglQuerySurface(display, surface, EGL_WIDTH, &w);
  eglQuerySurface(display, surface, EGL_HEIGHT, &h);

  // EGL関連データの保存
  engine->display = display;
  engine->context = context;
  engine->surface = surface;

  // 画面解像度の保存
  engine->width = w;
  engine->height = h;

  // 初期値設定
  int j;
  for (j = 0; j < 3; j++) {
    engine->angle[j] = 0;
  }

  // 立方体表示の初期化
  initCube(engine);

  return 0;
}

// 毎フレームの描画処理
static void engine_draw_frame(struct engine* engine) {

  // displayが無い場合は描画しない
  if (engine->display == NULL)
    return;

  // 描画前処理
  prepareFrame(engine);
  // 立方体を描画
  drawCube(engine);
  // ダブルバッファ入替
  eglSwapBuffers(engine->display, engine->surface);
}

// EGL情報を破棄する
static void engine_term_display(struct engine* engine) {
  if (engine->display != EGL_NO_DISPLAY) {
    // EGLレンダリングコンテキストとEGLサーフェイスの関連を外す
    eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                   EGL_NO_CONTEXT);
    // EGLレンダリングコンテキストを破棄する
    if (engine->context != EGL_NO_CONTEXT)
      eglDestroyContext(engine->display, engine->context);
    // EGLサーフェイスを破棄する
    if (engine->surface != EGL_NO_SURFACE)
      eglDestroySurface(engine->display, engine->surface);
    // EGLディスプレイを破棄する
    eglTerminate(engine->display);
  }
  // アニメーション停止
  engine->animating = 0;
  // EGL関連データを破棄
  engine->display = EGL_NO_DISPLAY;
  engine->context = EGL_NO_CONTEXT;
  engine->surface = EGL_NO_SURFACE;
}

// 入力イベントを処理する
static int32_t engine_handle_input(struct android_app* app,
                                   AInputEvent* event) {
  // ユーザデータの取得
  struct engine* engine = (struct engine*) app->userData;
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
    // アニメーション有効化
    engine->animating = 1;
    return 1;
  }
  return 0;
}

// メインコマンドの処理
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
  struct engine* engine = (struct engine*) app->userData;
  switch (cmd) {

  case APP_CMD_SAVE_STATE: // 状態保存を行うとき
    // 状態保存エリア取得
    engine->app->savedState = malloc(sizeof(struct saved_state));
    *((struct saved_state*) engine->app->savedState) = engine->state;
    engine->app->savedStateSize = sizeof(struct saved_state);
    break;

  case APP_CMD_INIT_WINDOW: // ウィンドウを初期化したとき
    if (engine->app->window != NULL) {
      // 画面を初期化する
      engine_init_display(engine);
      // 画面を描画する
      engine_draw_frame(engine);
    }
    break;

  case APP_CMD_TERM_WINDOW: // ウィンドウを破棄するとき
    // EGL情報を破棄する
    engine_term_display(engine);
    break;

  case APP_CMD_GAINED_FOCUS: // アプリがフォーカスを取得したとき
    if (engine->accelerometerSensor != NULL) {
      // 加速度センサーを有効化する
      ASensorEventQueue_enableSensor(engine->sensorEventQueue,
                                     engine->accelerometerSensor);
      // センサー情報取得間隔を設定する
      ASensorEventQueue_setEventRate(engine->sensorEventQueue,
                                     engine->accelerometerSensor,
                                     (1000L / 60) * 1000);
    }
    if (engine->gyroscopeSensor != NULL) {
      // ジャイロスコープを有効化する
      ASensorEventQueue_enableSensor(engine->sensorEventQueue,
                                     engine->gyroscopeSensor);
      // センサー情報取得間隔を設定する
      ASensorEventQueue_setEventRate(engine->sensorEventQueue,
                                     engine->gyroscopeSensor,
                                     (1000L / 60) * 1000);
    }
    break;
  case APP_CMD_LOST_FOCUS: // フォーカスが消失したとき
    if (engine->accelerometerSensor != NULL) {
      // 加速度センサーを無効化する
      ASensorEventQueue_disableSensor(engine->sensorEventQueue,
                                      engine->accelerometerSensor);
    }
    if (engine->gyroscopeSensor != NULL) {
      // ジャイロスコープを無効化する
      ASensorEventQueue_disableSensor(engine->sensorEventQueue,
                                      engine->gyroscopeSensor);
    }
    // アニメーション停止
    engine->animating = 0;
    // 画面を表示
    engine_draw_frame(engine);
    break;
  }
}

// Main関数
void android_main(struct android_app* state) {
  struct engine engine;

  // glueが削除されないように
  app_dummy();
  // アプリ情報保存エリアの確保
  memset(&engine, 0, sizeof(engine));
  // ユーザーデータの配置
  state->userData = &engine;
  // アプリケーションコマンド処理関数の設定
  state->onAppCmd = engine_handle_cmd;
  // 入力イベント処理関数の設定
  state->onInputEvent = engine_handle_input;
  engine.app = state;

  // センサーからのデータ取得に必要な初期化
  engine.sensorManager = ASensorManager_getInstance();
  // 加速度センサーのデータ取得準備
  engine.accelerometerSensor = ASensorManager_getDefaultSensor(
      engine.sensorManager, ASENSOR_TYPE_ACCELEROMETER);
  // ジャイロスコープのデータ取得準備
  engine.gyroscopeSensor = ASensorManager_getDefaultSensor(
      engine.sensorManager, ASENSOR_TYPE_GYROSCOPE);
  // センサー情報取得キューの新規作成
  engine.sensorEventQueue = ASensorManager_createEventQueue(
      engine.sensorManager, state->looper, LOOPER_ID_USER, NULL, NULL);

  if (state->savedState != NULL) {
    // 以前の状態に戻す
    engine.state = *(struct saved_state*) state->savedState;
  }

  while (1) {
    int ident;
    int events;
    struct android_poll_source* source;

    // アプリケーションの状態にあわせてセンサー情報の処理を行う
    while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
                                    (void**) &source)) >= 0) {

      // 内部イベントを処理する
      if (source != NULL) {
        source->process(state, source);
      }

      // センサー情報取得キューのデータを処理する
      if (ident == LOOPER_ID_USER) {
        if (engine.accelerometerSensor != NULL && engine.gyroscopeSensor != NULL) {
          ASensorEvent event[2];
          int count;
          int i;
          while ((count = ASensorEventQueue_getEvents(engine.sensorEventQueue,
                                                      event, 2)) > 0) {

            for (i = 0; i < count; i++) {
              switch (event[i].type) {

              case ASENSOR_TYPE_ACCELEROMETER: // 加速度センサーの値を出力する
//                LOGI("accelerometer: x=%f y=%f z=%f",
//                     event[i].acceleration.x, event[i].acceleration.y,
//                     event[i].acceleration.z);
                break;

              case ASENSOR_TYPE_GYROSCOPE: // ジャイロスコープの値を出力する
//                LOGI("GYROSCOPE: x=%f y=%f z=%f",event[i].vector.azimuth,event[i].vector.pitch,event[i].vector.roll    );
                break;
              }
            }
          }
        }
      }

      // EGL情報を破棄する
      if (state->destroyRequested != 0) {
        engine_term_display(&engine);
        return;
      }
    }

    if (engine.animating) {
      // 次のフレームを描画するのに必要な処理を行う
      int i = 0, j;

      engine.angle[0] += 3;
      engine.angle[1] += 1;
      for (j = 0; j < 3; j++) {
        if (engine.angle[j] > 360)
          engine.angle[j] -= 360;
        if (engine.angle[j] < 0)
          engine.angle[j] += 360;
      }
      // 画面描画
      engine_draw_frame(&engine);
    }
  }
}

