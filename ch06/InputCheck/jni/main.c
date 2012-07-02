#include <jni.h>
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
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__))

typedef struct _TouchPoint {
  int32_t x;
  int32_t y;
} TouchPoint;

/**
 * 再開に必要な保存すべきデータ
 */
struct saved_state {
  float angle;
};

/**
 * アプリケーション内で共通して利用する情報
 */
struct engine {
  struct android_app* app;

  ASensorManager* sensorManager;
  const ASensor* accelerometerSensor;
  ASensorEventQueue* sensorEventQueue;

  int animating;
  EGLDisplay display;
  EGLSurface surface;
  EGLContext context;
  int32_t width;
  int32_t height;

  int point_count;
  TouchPoint point[POINT_MAX];

  ASensorVector sensor_accel;

  int volumeup_keydown;
  int volumedown_keydown;

  struct saved_state state;
};

void gluPerspective(double fovy, double aspect, double zNear, double zFar) {
  GLfloat xmin, xmax, ymin, ymax;
  ymax = zNear * tan(fovy * M_PI / 360.0);
  ymin = -ymax;
  xmin = ymin * aspect;
  xmax = ymax * aspect;
  glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
}

GLuint texName[1];

void RenderGlyph(const glyph* glyphdat, int x, int y) {
  int box[] = { glyphdat->x + glyphdat->x_bearing, glyphdat->y, glyphdat->width
      + 2, glyphdat->height + 2 };

  glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, box);
  glDrawTexiOES(x, y, 0, glyphdat->width + 1, glyphdat->height);
  glFinish();
}

void RenderString(int x, int y, const char* message) {
  glPushMatrix();

  glLoadIdentity();

  glEnable(GL_BLEND);
  glBindTexture(GL_TEXTURE_2D, texName[0]);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(1, 1, 1, 1);

  const char* digit;
  for (digit = &message[0]; *digit; ++digit) {
    const glyph* glyphdat = &glyphlist[digit[0]];
    if (digit[0] == ' ') {
      x += glyphdat->x_advance;
      continue;
    }

    if (glyphdat->width != 0) {
      RenderGlyph(glyphdat, x, y);
      x += glyphdat->x_advance;
    }
  }
  glDisable(GL_BLEND);

  glPopMatrix();
}

/**
 * 表示の初期化
 */
void initBox(struct engine* engine) {

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(2, &texName[0]);
  glBindTexture(GL_TEXTURE_2D, texName[0]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  u_int width = 256, height = 128;
  GLint type = GL_RGBA;
  GLubyte *textureImage;

  glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type,
      GL_UNSIGNED_SHORT_4_4_4_4, &fonttexture[63]);

  glDisable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  glClearColor(.0f, .0f, .0f, 1.f);
  glShadeModel(GL_SMOOTH);

  float ratio = (float) engine->width / (float) engine->height;
  glViewport(0, 0, (int) engine->width, (int) engine->height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, ratio, 0.1, 100);
}

//void drawBox(void) {
//  // 表示の初期化(毎フレーム)
//  glMatrixMode(GL_MODELVIEW);
//  glLoadIdentity();
//
//// 描画する
//  glClear(GL_COLOR_BUFFER_BIT);
//  glEnable(GL_TEXTURE_2D);
//}

/**
 * デバイスに対してのEGLコンテキストの初期化
 */
static int engine_init_display(struct engine* engine) {
  // OepGL ES と EGLの初期化

  const EGLint attribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_BLUE_SIZE, 8,
      EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_NONE };
  EGLint w, h, dummy, format;
  EGLint numConfigs;
  EGLConfig config;
  EGLSurface surface;
  EGLContext context;

  EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

  eglInitialize(display, 0, 0);

  eglChooseConfig(display, attribs, &config, 1, &numConfigs);

  eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

  ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

  surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
  context = eglCreateContext(display, config, NULL, NULL);

  if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
    LOGW("Unable to eglMakeCurrent");
    return -1;
  }

  eglQuerySurface(display, surface, EGL_WIDTH, &w);
  eglQuerySurface(display, surface, EGL_HEIGHT, &h);

  engine->display = display;
  engine->context = context;
  engine->surface = surface;
  engine->width = w;
  engine->height = h;
  engine->state.angle = 0;
  engine->volumedown_keydown = FALSE;
  engine->volumeup_keydown = FALSE;

  // ボックス表示の初期化
  initBox(engine);

  return 0;
}

void drawCircle(int x, int y) {

  glEnable(GL_BLEND);
  glBindTexture(GL_TEXTURE_2D, texName[0]);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(1, 1, 1, 1);

  int box[] = { 256 - 32, 128 - 32, 32, 32 };

  glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, box);
  glDrawTexiOES(x - 16, y - 16, 0, 64, 64);
}

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

void displaySensors(struct engine* engine) {

  int i, y = 0;
  char buf[1024];

  sprintf(buf, "SENSOR ACCEL:(%2.3f,%2.3f,%2.3f)", engine->sensor_accel.x,
      engine->sensor_accel.y, engine->sensor_accel.z);
  RenderString(0, engine->height - 32 - 200, buf);
}

void displayKeys(struct engine* engine) {

  int i, y = 0;
  char buf[1024];

  sprintf(buf, "VOLUMEUP = %s", engine->volumeup_keydown ? "DOWN" : "UP");
  RenderString(engine->width / 2 + 64, engine->height - 32 - y, buf);
  y += 32;
  sprintf(buf, "VOLUMEDOWN = %s", engine->volumedown_keydown ? "DOWN" : "UP");
  RenderString(engine->width / 2 + 64, engine->height - 32 - y, buf);
}


/*
 * 毎フレーム描画
 */
static void engine_draw_frame(struct engine* engine) {

  if (engine->display == NULL) {
    // displayが無い
    return;
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

// 描画する
  glClear(GL_COLOR_BUFFER_BIT);
  glEnable(GL_TEXTURE_2D);

  // Touch Point
  displayTouchPoint(engine);
  displaySensors(engine);
  displayKeys(engine);

  eglSwapBuffers(engine->display, engine->surface);
}

/**
 * EGLコンテキストを破棄する
 */
static void engine_term_display(struct engine* engine) {
  if (engine->display != EGL_NO_DISPLAY) {
    eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE,
        EGL_NO_CONTEXT);
    if (engine->context != EGL_NO_CONTEXT) {
      eglDestroyContext(engine->display, engine->context);
    }
    if (engine->surface != EGL_NO_SURFACE) {
      eglDestroySurface(engine->display, engine->surface);
    }
    eglTerminate(engine->display);
  }
  engine->animating = 0;
  engine->display = EGL_NO_DISPLAY;
  engine->context = EGL_NO_CONTEXT;
  engine->surface = EGL_NO_SURFACE;
}

/**
 * 入力イベントを処理する
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
  struct engine* engine = (struct engine*) app->userData;

  switch (AInputEvent_getType(event)) {
  case AINPUT_EVENT_TYPE_MOTION: {
    engine->animating = 1;

    int32_t action = AMotionEvent_getAction(event);

    // １点以上押している
    if (action != AKEY_EVENT_ACTION_UP) {

      size_t count = AMotionEvent_getPointerCount(event);
      if (count > POINT_MAX)
        count = POINT_MAX;

      int i;
      engine->point_count = count;
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

  case AINPUT_EVENT_TYPE_KEY: {

    int keycode, keyaction;
    keycode = AKeyEvent_getKeyCode(event);
    if (keycode == AKEYCODE_VOLUME_UP) {
      keyaction = AKeyEvent_getAction(event);
      if (keyaction == AKEY_EVENT_ACTION_DOWN) {
        engine->volumeup_keydown = TRUE;
      } else {
        engine->volumeup_keydown = FALSE;
      }
    }
    if (keycode == AKEYCODE_VOLUME_DOWN) {
      keyaction = AKeyEvent_getAction(event);
      if (keyaction == AKEY_EVENT_ACTION_DOWN) {
        engine->volumedown_keydown = TRUE;
      } else {
        engine->volumedown_keydown = FALSE;
      }
    }
    break;

  }

  }

  return 0;
}

/**
 * メインコマンドの処理
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
  struct engine* engine = (struct engine*) app->userData;
  switch (cmd) {
  case APP_CMD_SAVE_STATE:
    engine->app->savedState = malloc(sizeof(struct saved_state));
    *((struct saved_state*) engine->app->savedState) = engine->state;
    engine->app->savedStateSize = sizeof(struct saved_state);
    break;
  case APP_CMD_INIT_WINDOW:
    if (engine->app->window != NULL) {
      engine_init_display(engine);
      engine_draw_frame(engine);
    }
    break;
  case APP_CMD_TERM_WINDOW:
    engine_term_display(engine);
    break;
  case APP_CMD_GAINED_FOCUS:
    if (engine->accelerometerSensor != NULL) {
      ASensorEventQueue_enableSensor(engine->sensorEventQueue,
          engine->accelerometerSensor);
      ASensorEventQueue_setEventRate(engine->sensorEventQueue,
          engine->accelerometerSensor, (1000L / 60) * 1000);
    }
    break;
  case APP_CMD_LOST_FOCUS:
    if (engine->accelerometerSensor != NULL) {
      ASensorEventQueue_disableSensor(engine->sensorEventQueue,
          engine->accelerometerSensor);
    }
    engine->animating = 0;
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
  state->userData = &engine;
  state->onAppCmd = engine_handle_cmd;
  state->onInputEvent = engine_handle_input;
  engine.app = state;

  // センサーからのデータ取得に必要な初期化
  engine.sensorManager = ASensorManager_getInstance();
  engine.accelerometerSensor = ASensorManager_getDefaultSensor(
      engine.sensorManager, ASENSOR_TYPE_ACCELEROMETER);
  engine.sensorEventQueue = ASensorManager_createEventQueue(
      engine.sensorManager, state->looper, LOOPER_ID_USER, NULL, NULL);

  if (state->savedState != NULL) {
    // 以前の状態に戻す
    engine.state = *(struct saved_state*) state->savedState;
  }

  engine.animating = 1;
  while (1) {

    int ident;
    int events;
    struct android_poll_source* source;

    // アプリケーションが動作することになれば、これらセンサーの制御を行う
    while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
        (void**) &source)) >= 0) {

      // イベントを処理する
      if (source != NULL) {
        source->process(state, source);
      }

      // センサーに何かしらのデータが存在したら処理する
      if (ident == LOOPER_ID_USER) {
        if (engine.accelerometerSensor != NULL) {
          ASensorEvent event;
          while (ASensorEventQueue_getEvents(engine.sensorEventQueue, &event, 1)
              > 0) {
            engine.sensor_accel = event.acceleration;
          }
        }
      }

      // 破棄要求があったか
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
    // 画面描画
    engine_draw_frame(&engine);

  }
}
