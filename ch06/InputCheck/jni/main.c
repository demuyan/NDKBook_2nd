#include <jni.h>
#include <errno.h>
#include <math.h>

#include <EGL/eglplatform.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <math.h>

#include <fonttexture.h>
#include <glyphpos.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/input.h>

#define POINT_MAX (5)
#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

// デバッグ用メッセージ
#define TAG "InputCheck"
// デバッグ用メッセージ(Infomation)
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
// デバッグ用メッセージ(Warning)
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
// デバッグ用メッセージ(Error)
#define LOGE(...)  ((void)__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__))

typedef struct _TouchPoint {
  int32_t x;
  int32_t y;
} TouchPoint;

// アプリ動作再開に必要なデータ
struct saved_state {
  float angle;
};

// アプリケーション内で共通して利用する情報
struct engine {
  struct android_app* app;

  // センサー関連
  ASensorManager* sensorManager;
  const ASensor* accelerometerSensor;  // 加速度センサー
  ASensorEventQueue* sensorEventQueue; // センサーイベントキュー
  
  // アニメーションフラグ
  int animating;

  // EGL
  EGLDisplay display;
  EGLSurface surface;
  EGLContext context;

  // 画面解像度
  int32_t width;
  int32_t height;

  int point_count;
  TouchPoint point[POINT_MAX];

  // 加速度センサー情報保存
  ASensorVector sensor_accel;

  // キー入力保存
  int volumeup_keydown;
  int volumedown_keydown;

  struct saved_state state;
};

// 透視投影の設定
void gluPerspective(double fovy, double aspect, double zNear, double zFar) {
  GLfloat xmin, xmax, ymin, ymax;
  ymax = zNear * tan(fovy * M_PI / 360.0);
  ymin = -ymax;
  xmin = ymin * aspect;
  xmax = ymax * aspect;
  glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
}

GLuint texName[1];

// １文字を表示
void RenderGlyph(const glyph* glyphdat, int x, int y) {
  int box[] = { glyphdat->x + glyphdat->x_bearing, glyphdat->y, glyphdat->width
      + 2, glyphdat->height + 2 };

  glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, box);
  glDrawTexiOES(x, y, 0, glyphdat->width + 1, glyphdat->height);
  glFinish();
}

// メッセージを表示
void RenderString(int x, int y, const char* message) {

  glPushMatrix();

  glLoadIdentity();

  // 既存の表示物と混合して表示
  glEnable(GL_BLEND);
  glBindTexture(GL_TEXTURE_2D, texName[0]);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // 色指定
  glColor4f(1, 1, 1, 1);

  // messageにある文字列を表示
  const char* digit;
  for (digit = &message[0]; *digit; ++digit) {
    const glyph* glyphdat = &glyphlist[digit[0]];

    // スペースは表示しない
    if (digit[0] == ' ') {
      x += glyphdat->x_advance;
      continue;
    }

    // 登録のある文字は表示
    if (glyphdat->width != 0) {
      RenderGlyph(glyphdat, x, y);
      x += glyphdat->x_advance;
    }
  }
  glDisable(GL_BLEND);

  glPopMatrix();
}

// 表示の初期化
void initBox(struct engine* engine) {

  // 表示の初期化
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // テクスチャの登録
  glGenTextures(2, &texName[0]);
  glBindTexture(GL_TEXTURE_2D, texName[0]);

  // テクスチャの表示条件を設定
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // テクスチャをロードする
  u_int width = 256, height = 128;
  GLint type = GL_RGBA;
  glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type,
               GL_UNSIGNED_SHORT_4_4_4_4, &fonttexture[63]);

  // 光源処理無効化
  glDisable(GL_LIGHTING);
  // 背面処理無効化
  glDisable(GL_CULL_FACE);
  // デプスバッファ無効化
  glDisable(GL_DEPTH_BUFFER_BIT);
  // デプステスト無効化
  glDisable(GL_DEPTH_TEST);
  // 塗りつぶし色設定
  glClearColor(.7f, .7f, .9f, 1.f);
  // 陰影モード設定
  glShadeModel(GL_SMOOTH);

  // 縦横比の設定
  float ratio = (float) engine->width / (float) engine->height;
  glViewport(0, 0, (int) engine->width, (int) engine->height);

  // 行列演算のモード設定
  glMatrixMode(GL_PROJECTION);
  // 単位行列のロード
  glLoadIdentity();
  // 透視投影の設定
  gluPerspective(40.0, ratio, 0.1, 100);
}

// EGL初期化
static int engine_init_display(struct engine* engine) {

  EGLint w, h, dummy, format;
  EGLint numConfigs;
  EGLConfig config;
  EGLSurface surface;
  EGLContext context;

  // 有効にするEGLパラメータ
  const EGLint attribs[] =
    { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, 
      EGL_BLUE_SIZE,  8,
      EGL_GREEN_SIZE, 8, 
      EGL_RED_SIZE,   8,
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
  surface = eglCreateWindowSurface(display, config, engine->app->window,
                                   NULL);
  // EGLレンダリングコンテキストの取得
  context = eglCreateContext(display, config, NULL, NULL);
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

  // 矩形表示の初期化
  engine->state.angle = 0;
  engine->volumedown_keydown = FALSE;
  engine->volumeup_keydown = FALSE;

  // ボックス表示の初期化
  initBox(engine);

  return 0;
}

// 円を描画
void drawCircle(int x, int y) {

  glEnable(GL_BLEND);
  glBindTexture(GL_TEXTURE_2D, texName[0]);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(1, 1, 1, 1);

  int box[] = { 256 - 32, 128 - 32, 32, 32 };

  glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, box);
  glDrawTexiOES(x - 16, y - 16, 0, 64, 64);
}

// タッチ位置情報を表示する
void displayTouchPoint(struct engine* engine) {

  int i, y = 0;
  char buf[1024];
  for (i = 0; i < engine->point_count; i++) {
    sprintf(buf, "TOUCH POINT(%d)=(%d,%d)", i, engine->point[i].x,        engine->point[i].y);
    RenderString(0, engine->height - 32 - y, buf);
    y += 32;

    drawCircle(engine->point[i].x, engine->height - engine->point[i].y);
  }
}

// センサー情報を表示する
void displaySensors(struct engine* engine) {

  int i, y = 0;
  char buf[1024];

  // 加速度センサーの状態を表示
  sprintf(buf, "SENSOR ACCEL:(%2.3f,%2.3f,%2.3f)", engine->sensor_accel.x,
      engine->sensor_accel.y, engine->sensor_accel.z);
  RenderString(0, engine->height - 32 - 200, buf);
}

// キー入力情報を表示する
void displayKeys(struct engine* engine) {

  int i, y = 0;
  char buf[1024];

  // 音量上ボタンの状態を表示
  sprintf(buf, "VOLUMEUP = %s", engine->volumeup_keydown ? "DOWN" : "UP");
  RenderString(engine->width / 2 + 64, engine->height - 32 - y, buf);
  y += 32;
  // 音量下ボタンの状態を表示
 sprintf(buf, "VOLUMEDOWN = %s", engine->volumedown_keydown ? "DOWN" : "UP");
  RenderString(engine->width / 2 + 64, engine->height - 32 - y, buf);
}


//  毎フレーム描画
static void engine_draw_frame(struct engine* engine) {

  // displayが無い場合は描画しない
  if (engine->display == NULL) 
    return;

  // モデル表示モードにする
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // 画面をクリアーする
  glClear(GL_COLOR_BUFFER_BIT);
  // テクスチャー表示を有効化
  glEnable(GL_TEXTURE_2D);

  // タッチ位置を表示
  displayTouchPoint(engine);
  // センサー情報を表示
  displaySensors(engine);
  // キー入力情報を表示
  displayKeys(engine);

  // 表示領域、描画領域を入れ替える
  eglSwapBuffers(engine->display, engine->surface);
}

// EGL情報を破棄する
static void engine_term_display(struct engine* engine) {
  if (engine->display != EGL_NO_DISPLAY) {
    // EGLレンダリングコンテキストとEGLサーフェイスの関連を外す
    eglMakeCurrent(engine->display, 
                   EGL_NO_SURFACE, 
                   EGL_NO_SURFACE,
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
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
  struct engine* engine = (struct engine*) app->userData;

  // ユーザデータの取得
  switch (AInputEvent_getType(event)) {
  case AINPUT_EVENT_TYPE_MOTION: 
  {
    // アニメーション有効化
    engine->animating = 1;

    // タッチの状態を取得する
    int32_t action = AMotionEvent_getAction(event);

    // １点以上押している
    if (action != AKEY_EVENT_ACTION_UP) {

      // 押しているポイントの数を取得
      size_t count = AMotionEvent_getPointerCount(event);
      if (count > POINT_MAX)
        count = POINT_MAX;

      int i;
      engine->point_count = count;
      // 押しているポイントの位置を取得
      for (i = 0; i < count; i++) {
        engine->point[i].x = AMotionEvent_getX(event, i);
        engine->point[i].y = AMotionEvent_getY(event, i);
      }
    } else {
      // 指が全部離れた
      engine->point_count = 0;
    }

    return 1;
  }
  break;

  case AINPUT_EVENT_TYPE_KEY: 
  {

    int keycode, keyaction;
    // キーコードを取得
    keycode = AKeyEvent_getKeyCode(event);
    // 音量キー（上）が変化した
    if (keycode == AKEYCODE_VOLUME_UP) {
      keyaction = AKeyEvent_getAction(event);


      if (keyaction == AKEY_EVENT_ACTION_DOWN) {
        // キーを押した
        engine->volumeup_keydown = TRUE;
      } else {
        // キーを離した
        engine->volumeup_keydown = FALSE;
      }
    }
    // 音量キー（下）が変化した
    if (keycode == AKEYCODE_VOLUME_DOWN) {
      keyaction = AKeyEvent_getAction(event);
      if (keyaction == AKEY_EVENT_ACTION_DOWN) {
        // キーを押した
        engine->volumedown_keydown = TRUE;
      } else {
        // キーを離した
        engine->volumedown_keydown = FALSE;
      }
    }
    
  }
  break;

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

  case APP_CMD_GAINED_FOCUS:  // アプリがフォーカスを取得したとき
    if (engine->accelerometerSensor != NULL) {
      // 加速度センサーを有効化する
      ASensorEventQueue_enableSensor(engine->sensorEventQueue,
          engine->accelerometerSensor);
      // センサー情報取得間隔を設定する
      ASensorEventQueue_setEventRate(engine->sensorEventQueue,
          engine->accelerometerSensor, (1000L / 60) * 1000);
    }
    break;

  case APP_CMD_LOST_FOCUS: // フォーカスが消失したとき
    if (engine->accelerometerSensor != NULL) {
      // 加速度センサーを無効化する
      ASensorEventQueue_disableSensor(engine->sensorEventQueue,
          engine->accelerometerSensor);
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

  memset(&engine, 0, sizeof(engine));
  // ユーザーデータの配置
  state->userData = &engine;
  // アプリケーションコマンド処理関数の設定
  state->onAppCmd = engine_handle_cmd;
  // 入力イベント処理関数の設定
  state->onInputEvent = engine_handle_input;
  engine.app = state;

  // センサーマネージャの取得
  engine.sensorManager = ASensorManager_getInstance();
  // 加速度センサーのデータ取得準備
  engine.accelerometerSensor = ASensorManager_getDefaultSensor(
    engine.sensorManager, ASENSOR_TYPE_ACCELEROMETER);
  // センサー情報取得キューの新規作成
  engine.sensorEventQueue = ASensorManager_createEventQueue(
    engine.sensorManager, state->looper, LOOPER_ID_USER, NULL,
    NULL);

  if (state->savedState != NULL) {
    // 以前の状態に戻す
    engine.state = *(struct saved_state*) state->savedState;
  }

  engine.animating = 1;
  while (1) {

    int ident;
    int events;
    struct android_poll_source* source;

    // アプリケーションの状態にあわせてセンサー情報の処理を行う
    while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL,
                                    &events, (void**) &source)) >= 0) {

      // 内部イベントを処理する
      if (source != NULL) {
        source->process(state, source);
      }

      // センサー情報取得キューのデータを処理する
      if (ident == LOOPER_ID_USER) {
        if (engine.accelerometerSensor != NULL) {
          ASensorEvent event;
          while (ASensorEventQueue_getEvents(engine.sensorEventQueue, &event, 1)
              > 0) {
            engine.sensor_accel = event.acceleration;
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
      engine.state.angle += .01f;
      if (engine.state.angle > 1) {
        engine.state.angle = 0;
      }

    }

    // 画面の描画
    engine_draw_frame(&engine);

  }
}
