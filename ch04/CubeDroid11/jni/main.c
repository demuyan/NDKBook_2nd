#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <math.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

// デバッグ用メッセージ
#define LOG_TAG "CubeDroid11"
// デバッグ用メッセージ(Infomation)
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
// デバッグ用メッセージ(Warning)
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
// デバッグ用メッセージ(Error)
#define LOGE(...)  ((void)__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__))

#define POINT_MAX (5)
#define TRUE (1)
#define FALSE (0)

// アプリ動作再開に必要なデータ
typedef struct _TouchPoint{
    int32_t using;
    float angle[3];
    int32_t x;
    int32_t y;
} TouchPoint;

struct saved_state
{
    TouchPoint point[POINT_MAX];
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

  // 保存データ
  struct saved_state state;
};

#undef PI
#define PI 3.1415926535897932f

static const GLfloat cubeVertices[] = {
  -1.0, -1.0,  1.0,
  1.0, -1.0,  1.0,
  -1.0,  1.0,  1.0,
  1.0,  1.0,  1.0,
  -1.0, -1.0, -1.0,
  1.0, -1.0, -1.0,
  -1.0,  1.0, -1.0,
  1.0,  1.0, -1.0,
};

static const GLushort cubeIndices[] = {
  0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
};

static const GLubyte cubeColors[] = {
  255, 255,   0, 255,
  0,   255, 255, 255,
  0,     0, 255, 255,
  255,   0, 255, 255,
  255, 255,   0, 255,
  0,   255, 255, 255,
  0,     0, 255, 255,
  255,   0, 255, 255
};

// 表示の初期化
void initBox(struct engine* engine) {

  glEnable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glShadeModel(GL_SMOOTH);

  glClearColor(.7f, .7f, .9f, 1.f);

  glViewport(0, 0, (int) engine->width, (int) engine->height);

  // 表示の初期化(毎フレーム)
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrthof(-2.0, 2.0, -2.0 * engine->height / engine->width, 2.0 * engine->height / engine->width, -10.0, 10.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void drawBox(struct engine* engine) {
//     表示の初期化(毎フレーム)

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    int i = 0;

    TouchPoint *tp = &engine->state.point[i];


    glPushMatrix();
    glTranslatef(0, 0, -2);

    glRotatef(tp->angle[0], 1.0f, 0, 0.5f);

    // 描画する
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, cubeVertices);
    glEnableClientState(GL_VERTEX_ARRAY);

    glColorPointer(4, GL_UNSIGNED_BYTE, 0, cubeColors);
    glEnableClientState(GL_COLOR_ARRAY);

    glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);

    glPopMatrix();
}

/**
 * デバイスに対してのEGLコンテキストの初期化
 */
static int engine_init_display(struct engine* engine) {
    // OepGL ES と EGLの初期化

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

  int i = 0,j;
  for (j = 0; j < 3; j++){
    engine->state.point[i].angle[i] = 0;
  }
  engine->state.point[i].using = TRUE;

  // ボックス表示の初期化
  initBox(engine);

  return 0;
}

// 毎フレームの描画処理
static void engine_draw_frame(struct engine* engine) {

  // displayが無い場合は描画しない
  if (engine->display == NULL) 
    return;
    
  // 四角形を描画
  drawBox(engine);
  // ダブルバッファ入替
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

/**
 * メインコマンドの処理
 */
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
                                       engine->accelerometerSensor, (1000L / 60) * 1000);
      }
      if (engine->gyroscopeSensor != NULL) {
        // ジャイロスコープを有効化する
        ASensorEventQueue_enableSensor(engine->sensorEventQueue,
                                       engine->gyroscopeSensor);
        // センサー情報取得間隔を設定する
        ASensorEventQueue_setEventRate(engine->sensorEventQueue,
                                       engine->gyroscopeSensor, (1000L / 60) * 1000);
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

/**
 * アプリケーション開始
 */
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

  // センサーからのデータ取得に必要な初期化
  engine.sensorManager = ASensorManager_getInstance();
  // 加速度センサーのデータ取得準備
  engine.accelerometerSensor = ASensorManager_getDefaultSensor(
    engine.sensorManager, ASENSOR_TYPE_ACCELEROMETER);
  // ジャイロスコープのデータ取得準備
  engine.gyroscopeSensor = ASensorManager_getDefaultSensor(
    engine.sensorManager, ASENSOR_TYPE_GYROSCOPE );
  // センサー情報取得キューの新規作成
  engine.sensorEventQueue = ASensorManager_createEventQueue(
    engine.sensorManager, state->looper, LOOPER_ID_USER, NULL,
    NULL);

  if (state->savedState != NULL) {
    // 以前の状態に戻す
    engine.state = *(struct saved_state*) state->savedState;
  }

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
        if (engine.accelerometerSensor != NULL && engine.gyroscopeSensor != NULL) {
          ASensorEvent event[2];
          int count;
          int i;
          while ((count = ASensorEventQueue_getEvents(
                    engine.sensorEventQueue, event, 2)) > 0) {

            for (i = 0; i < count; i++){
              switch(event[i].type){
                
              case ASENSOR_TYPE_ACCELEROMETER: // 加速度センサーの値を出力する
                LOGI("accelerometer: x=%f y=%f z=%f",
                     event[i].acceleration.x, event[i].acceleration.y,
                     event[i].acceleration.z);
                break;

              case ASENSOR_TYPE_GYROSCOPE: // ジャイロスコープの値を出力する
                LOGI("GYROSCOPE: x=%f y=%f z=%f",event[i].vector.azimuth,event[i].vector.pitch,event[i].vector.roll    );
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
      int i = 0,j;

      engine.state.point[i].angle[0] -= 3;
      for (j = 0; j < 3; j++){
        if (engine.state.point[i].angle[j] > 360)
          engine.state.point[i].angle[j] -= 360;
        if (engine.state.point[i].angle[j] < 0)
          engine.state.point[i].angle[j] += 360;
      }
      // 画面描画
      engine_draw_frame(&engine);
    }
  }
}
