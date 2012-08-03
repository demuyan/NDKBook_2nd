#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES/gl.h>
#include "glu.h"
#include <math.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#include "libpng_android.h"

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

typedef struct _TouchPoint{
    int32_t using;
    float angle[3];
    int32_t x;
    int32_t y;
} TouchPoint;

// アプリ動作再開に必要なデータ
typedef struct _saved_state
{
    TouchPoint point[POINT_MAX];
} saved_state2;

// アプリケーション内で共通して利用する情報
struct engine
{
  struct android_app* app;

  // センサー関連
  ASensorManager* sensorManager; // センサーマネージャ
  const ASensor* accelerometerSensor; // 加速度センサー
  const ASensor* gyroscopeSensor; // ジャイロスコープ
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

  // AssetManager
  AAssetManager* assetManager;

  // 保存データ
  saved_state2 state;
};

#undef PI
#define PI 3.1415926535897932f

GLuint texName[1];
GLenum errCode;

// 頂点リスト
static const GLfloat cubeVertices[] = {
/*  X     Y     Z */
  -1.0, -1.0,  1.0,
   1.0, -1.0,  1.0,
  -1.0,  1.0,  1.0,
   1.0,  1.0,  1.0,
  -1.0, -1.0, -1.0,
   1.0, -1.0, -1.0,
  -1.0,  1.0, -1.0,
   1.0,  1.0, -1.0,
};


// 頂点描画順
static const GLushort cubeIndices[] = {
  0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
};

// 頂点リスト
const GLfloat cubeTexCoords[] = {
        1.0, 0.0,
        0.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
        1.0, 1.0,
        0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
    };

// 表示の初期化
void initBox(struct engine* engine) {

  // 正規化を有効化
  glEnable(GL_NORMALIZE);
  // デプステスト有効化
  glEnable(GL_DEPTH_TEST);

  // 背面処理有効化
//  glEnable(GL_CULL_FACE);
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &texName[0]);
  glBindTexture(GL_TEXTURE_2D, &texName[0]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  png_uint_32 width,height;
  GLint type;
  GLubyte *textureImage;

  int flg = loadPngImage(engine->assetManager,  "texture.png", &width, &height, &type, &textureImage);

  glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, textureImage);
  if ((errCode = glGetError()) != GL_NO_ERROR){
	  LOGI("line %d : GLErrorCode:%x ",__LINE__,errCode);
  }

  // 前面のみ描画（背面は描画しない）
  glCullFace(GL_BACK);
  // 陰影モード設定
//  glShadeModel(GL_SMOOTH);
  glShadeModel(GL_FLAT);
  // 塗りつぶし色設定
  glClearColor(.7f, .7f, .9f, 1.f);

  // 縦横比の設定
  glViewport(0, 0, (int) engine->width, (int) engine->height);
  if ((errCode = glGetError()) != GL_NO_ERROR){
	  LOGI("line %d : GLErrorCode:%x ",__LINE__,errCode);
  }

  // 行列演算のモード設定
  glMatrixMode(GL_PROJECTION);
  // 単位行列のロード
  glLoadIdentity();
  if ((errCode = glGetError()) != GL_NO_ERROR){
	  LOGI("line %d : GLErrorCode:%x ",__LINE__,errCode);
  }

  glOrthof(-2.0, 2.0, -2.0 * engine->height / engine->width, 2.0 * engine->height / engine->width, -10.0, 10.0);

  // 行列演算のモード設定
  glMatrixMode(GL_MODELVIEW);
  // 単位行列のロード
  glLoadIdentity();

}


// 立方体の描画
void drawCube(struct engine* engine) {


  if ((errCode = glGetError()) != GL_NO_ERROR){
	  LOGI("line %d : GLErrorCode:%x ",__LINE__,errCode);
  }

  // 行列演算のモード設定
  glMatrixMode(GL_MODELVIEW);
  // 単位行列のロード
  glLoadIdentity();

  // 行列状態を格納
  glPushMatrix();

  if ((errCode = glGetError()) != GL_NO_ERROR){
	  LOGI("line %d : GLErrorCode:%x ",__LINE__,errCode);
  }

  // 平行移動
  glTranslatef(0, 0, -2);

  // 回転角度の追加
  TouchPoint *tp = &engine->state.point[0];
  glRotatef(tp->angle[0], 1.0f, 0, 0.5f);

  if ((errCode = glGetError()) != GL_NO_ERROR){
	  LOGI("line %d : GLErrorCode:%x ",__LINE__,errCode);
  }

  // バッファをクリアー
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //

  if ((errCode = glGetError()) != GL_NO_ERROR){
	  LOGI("line %d : GLErrorCode:%x ",__LINE__,errCode);
  }

  // 頂点リストの有効化
  glEnableClientState(GL_VERTEX_ARRAY);
  // 頂点リスト指定
  glVertexPointer(3, GL_FLOAT, 0, cubeVertices);

  // 頂点カラーリストの有効化
//  glEnableClientState(GL_COLOR_ARRAY);
  // 頂点カラーリスト指定
//  glColorPointer(4, GL_UNSIGNED_BYTE, 0, cubeColors);

 if ((errCode = glGetError()) != GL_NO_ERROR){
	  LOGI("line %d : GLErrorCode:%x ",__LINE__,errCode);
  }
 glDisableClientState(GL_COLOR_ARRAY);

 glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, cubeTexCoords);
//  glBindTexture(GL_TEXTURE_2D, texName[0]);
    

  if ((errCode = glGetError()) != GL_NO_ERROR){
	  LOGI("line %d : GLErrorCode:%x ",__LINE__,errCode);
  }

  // 頂点インデックスに従って立方体を描画
  glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);

	  if ((errCode = glGetError()) != GL_NO_ERROR){
		  LOGI("line %d : GLErrorCode:%x ",__LINE__,errCode);
	  }

  // 行列状態を取り出し
  glPopMatrix();

}

// EGL初期化
static int engine_init_display(struct engine* engine) {

  LOGI("engine_init_display");

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
      EGL_DEPTH_SIZE, 24, 
      EGL_NONE };

  // EGLディスプレイコネクションを取得

//  EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  EGLDisplay display = eglGetDisplay(0);
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

  // 立方体表示パラメータの初期化
  int i = 0,j;
  for (j = 0; j < 3; j++)
    engine->state.point[i].angle[i] = 0;
  engine->state.point[i].using = TRUE;
  initBox(engine);

  return 0;
}

// 毎フレームの描画処理
static void engine_draw_frame(struct engine* engine) {

  // displayが無い場合は描画しない
  if (engine->display == NULL) 
    return;

  // 立方体を描画
  drawCube(engine);
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
//    engine->animating = 1;

    int i;
    size_t count = AMotionEvent_getPointerCount(event);
    if(count > POINT_MAX)
      count = POINT_MAX;
     
    for (i = 0; i < POINT_MAX; i++){
      engine->state.point[i].using = FALSE;
    }
    // タッチしたポイントを取得する
    for (i = 0; i < count; i++){
      engine->state.point[i].using = TRUE;
      
      engine->state.point[i].x = AMotionEvent_getX(event, i);
      engine->state.point[i].y = AMotionEvent_getY(event, i);
//      LOGI("touch point(%d): x=%d y=%d", i, engine->state.point[i].x, engine->state.point[i].y);
    }

    return 1;
  }
  return 0;
}

// メインコマンドの処理
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
  struct engine* engine = (struct engine*) app->userData;
  switch (cmd) {

  case APP_CMD_SAVE_STATE:  // 状態保存を行うとき
    // 状態保存エリア取得
    engine->app->savedState = malloc(sizeof(saved_state2));
    *((saved_state2*) engine->app->savedState) = engine->state;
    engine->app->savedStateSize = sizeof(saved_state2);
    break;

  case APP_CMD_INIT_WINDOW: // ウィンドウを初期化したとき
    if (engine->app->window != NULL) {
      // 画面を初期化する
      engine_init_display(engine);

      engine->animating = 1;
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
  // ジャイロスコープのデータ取得準備
  engine.gyroscopeSensor = ASensorManager_getDefaultSensor(
    engine.sensorManager, ASENSOR_TYPE_GYROSCOPE );
  // センサー情報取得キューの新規作成
  engine.sensorEventQueue = ASensorManager_createEventQueue(
    engine.sensorManager, state->looper, LOOPER_ID_USER, NULL,
    NULL);

  // AssetManagerの取得
  engine.assetManager = state->activity->assetManager;

  if (state->savedState != NULL) {
    // 以前の状態に戻す
    engine.state = *(saved_state2*) state->savedState;
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
              case ASENSOR_TYPE_ACCELEROMETER:
//                LOGI("accelerometer: x=%f y=%f z=%f",event[i].acceleration.x, event[i].acceleration.y,event[i].acceleration.z);
                break;

              case ASENSOR_TYPE_GYROSCOPE:
//                LOGI("GYROSCOPE: x=%f y=%f z=%f",event[i].vector.azimuth,event[i].vector.pitch,event[i].vector.roll    );
                break;
              }
            }
          }
        }
      }

      if (state->destroyRequested != 0) {
        // EGL情報を破棄する
        engine_term_display(&engine);
        return;
      }
    }

    if (engine.animating) {
      // アニメーション（立方体の回転演算）処理
      int i = 0,j;

      engine.state.point[i].angle[0] -= 3;
      for (j = 0; j < 3; j++){
        if (engine.state.point[i].angle[j] > 360)
          engine.state.point[i].angle[j] -= 360;
        if (engine.state.point[i].angle[j] < 0)
          engine.state.point[i].angle[j] += 360;
      }
      // 画面の描画
      engine_draw_frame(&engine);
    }
  }
}
