#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "main.h"
#include "Matrix.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define LOG_TAG    "cubedroid20"
// デバッグ用メッセージ(Infomation)
#define LOGI(...)  ((void)__android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__))
// デバッグ用メッセージ(Warning)
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
// デバッグ用メッセージ(Error)
#define LOGE(...)  ((void)__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__))

// インデックス
enum {
  ATTRIB_VERTEX,
  ATTRIB_COLOR,
  NUM_ATTRIBUTES
};

// ログ出力
static void printGLString(const char *name, GLenum s) {
  const char *v = (const char *) glGetString(s);
  LOGI("GL %s = %s\n", name, v);
}

// エラーチェックを行う
static void checkGlError(const char* op) {
  for (GLint error = glGetError(); error; error = glGetError()) {
    LOGI("after %s() glError (0x%x)\n", op, error);
  }
}

// 立方体の頂点リスト
const GLfloat cubeVertices[] = {
/*   x     y     z  */
  -1.0, -1.0,  1.0,
   1.0, -1.0,  1.0,
  -1.0,  1.0,  1.0,
   1.0,  1.0,  1.0,
  -1.0, -1.0, -1.0,
   1.0, -1.0, -1.0,
  -1.0,  1.0, -1.0,
   1.0,  1.0, -1.0,
};

// 描画する頂点リスト
const GLushort cubeIndices[] = {
  0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
};

// 立方体の頂点色リスト
static const GLfloat cubeColors[] = {
  1.0, 1.0,   0, 1.0,
    0, 1.0, 1.0, 1.0,
    0,   0, 1.0, 1.0,
  1.0,   0, 1.0, 1.0,
  1.0, 1.0,   0, 1.0,
    0, 1.0, 1.0, 1.0,
    0,   0, 1.0, 1.0,
  1.0,   0, 1.0, 1.0,
};

GLuint gProgram;

// 透視変換を行う（頂点シェーダ）
static const char gVertexShader[] = 
  "attribute vec4 Position;      \n"
  "attribute vec4 SourceColor;   \n"
  "varying vec4 DestinationColor;\n"
  "uniform mat4 Projection;      \n"
  "uniform mat4 Modelview;       \n"
  "void main() {\n"
  "   gl_Position = Projection * Modelview * Position; \n"
  "   DestinationColor = SourceColor; \n"
  "}\n";

// 頂点色を設定する（フラグメントシェーダ）
static const char gFragmentShader[] = 
  "varying lowp vec4 DestinationColor; \n"
  "void main() {                       \n"
  "  gl_FragColor = DestinationColor;  \n"
  "}\n";

// シェーダープログラムを設定する
GLuint loadShader(GLenum shaderType, const char* pSource) {
  GLuint shader = glCreateShader(shaderType);
  if (shader) {
    glShaderSource(shader, 1, &pSource, NULL);
    glCompileShader(shader);
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
      GLint infoLen = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
      if (infoLen) {
        char* buf = (char*) malloc(infoLen);
        if (buf) {
          glGetShaderInfoLog(shader, infoLen, NULL, buf);
          LOGE("Could not compile shader %d:\n%s\n", shaderType, buf);
          free(buf);
        }
        glDeleteShader(shader);
        shader = 0;
      }
    }
  }
  return shader;
}

// シェーダープログラムをビルド、設定する
GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {

  // 頂点シェーダプログラムをロードする
  GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
  if (!vertexShader)
    return 0;

  // フラグメントシェーダプログラムをロードする
  GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
  if (!pixelShader)
    return 0;

  GLuint program = glCreateProgram();
  if (program) {
    glAttachShader(program, vertexShader);
    checkGlError("glAttachShader");
    glAttachShader(program, pixelShader);
    checkGlError("glAttachShader");
    glLinkProgram(program);

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
      GLint bufLength = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
      if (bufLength) {
        char* buf = (char*) malloc(bufLength);
        if (buf) {
          glGetProgramInfoLog(program, bufLength, NULL, buf);
          LOGE("Could not link program:\n%s\n", buf);
          free(buf);
        }
      }
      glDeleteProgram(program);
      program = 0;
    }
  }
  return program;
}

// 立方体表示の初期化
void initCube(struct engine* engine) {
  // 頂点シェーダー、フラグメントシェーダーを設定する
  gProgram = createProgram(gVertexShader, gFragmentShader);   /////----- (1)
  if (!gProgram) {
    LOGE("Could not create program.");
    return;
  }

  // 頂点シェーダーに頂点座標リストを結びつける
  glBindAttribLocation(gProgram, ATTRIB_VERTEX, "Position");   /////----- (2)

  // フラグメントシェーダーに頂点カラーリストを結びつける
  glBindAttribLocation(gProgram, ATTRIB_COLOR, "SourceColor");

  // 利用するシェーダープログラムを指定する
  glUseProgram(gProgram);                                      /////----- (3)

  // デプステストを有効化する
  glEnable(GL_DEPTH_TEST);
}

#undef PI
#define PI 3.1415926535897932f

mat4 getPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar) {
  GLfloat xmin, xmax, ymin, ymax;

  ymax = zNear * (GLfloat)tan(fovy * PI / 360);
  ymin = -ymax;
  xmin = ymin * aspect;
  xmax = ymax * aspect;

  return mat4::Frustum( xmin, xmax,
      ymin, ymax,
      zNear ,zFar * 65536);
}

// フレーム前初期化
void prepareFrame(struct engine* engine) {

  // ViewPortを指定
  glViewport(0, 0, engine->width, engine->height);
  // 塗りつぶし色設定
  glClearColor(.7f, .7f, .9f, 1.f);
  // カラーバッファ、デプスバッファをクリアー
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  // 背面は表示しない
  glCullFace(GL_BACK);

  // 透視変換行列の設定
  GLint projectionUniform = glGetUniformLocation(gProgram, "Projection");                          /////----- (1) ここから
  mat4 projectionMatrix = getPerspective(45, (float) engine->width / engine->height, 0.5f, 500);
  glUniformMatrix4fv(projectionUniform, 1, 0, projectionMatrix.Pointer());                         /////----- (1) ここまで
}


// レンダリングを行う
void drawCube(struct engine* engine) {

  // 立方体の姿勢特定、行列取得
  mat4 rotationX = mat4::RotateX(engine->angle[0]);                         /////----- (1) ここから
  mat4 rotationY = mat4::RotateY(engine->angle[1]);
  mat4 rotationZ = mat4::RotateZ(engine->angle[2]);

  mat4 scale = mat4::Scale(1.0);
  mat4 translation = mat4::Translate(0, 0, -10  );
  mat4 modelviewMatrix =  scale * rotationX * rotationY * translation;
    
  // 表示物の姿勢設定
  int modelviewUniform = glGetUniformLocation(gProgram, "Modelview");
  glUniformMatrix4fv(modelviewUniform, 1, 0, modelviewMatrix.Pointer());      /////----- (1) ここまで

  //頂点の設定
  glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, cubeVertices); /////----- (2)
  // 色の指定
  glVertexAttribPointer(ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, cubeColors);     /////----- (3)

  glEnableVertexAttribArray(ATTRIB_VERTEX);
  glEnableVertexAttribArray(ATTRIB_COLOR);

  // 描画する
  glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);   /////----- (4)

  glDisableVertexAttribArray(ATTRIB_VERTEX);
  glDisableVertexAttribArray(ATTRIB_COLOR);
}

