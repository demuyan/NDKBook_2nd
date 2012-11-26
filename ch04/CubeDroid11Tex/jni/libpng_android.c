#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

#include <png.h>
#include <pnginfo.h>

#include <jni.h>
#include <android/asset_manager.h>

#include "libpng_android.h"


int offset = 0;
void callback_read(png_structp pPng, png_bytep buf, png_size_t size) {
  // ポインタを取得する
  u_char* p = (u_char*) png_get_io_ptr(pPng);
  // メモリコピー
  memcpy(buf, p + offset, size);
  // 読み込んだバイト数分のポインタをすすめる
  offset += size;
}

// png画像をテクスチャとしてロードする
int loadPngImage(AAssetManager* mgr, char *filename, png_uint_32* outWidth,
                 png_uint_32* outHeight, GLint *type, u_char **outData) {

  png_structp png_ptr;
  png_infop info_ptr;
  int depth;
  int color_type;

  // ファイルをオープンする
  AAsset* assetFile = AAssetManager_open(mgr, filename, AASSET_MODE_RANDOM);
  // ファイルの読み込み
  int size = AAsset_getLength(assetFile);
  u_char* buf = (u_char*) malloc(size);
  AAsset_read(assetFile, buf, size);
  // ファイルを閉じる
  AAsset_close(assetFile);

  // 読み込んだファイルがpng画像であるかチェック
  if (png_sig_cmp(buf, 0, 8) != 0)
    return FALSE;

  // png画像を読み込むためのpng_struct構造体を生成する
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    if (buf) {
      free(buf);
      buf = NULL;
    }
    return FALSE;
  }

  // png画像を読み込むためのpng_info構造体を生成する
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    if (buf) {
      free(buf);
      buf = NULL;
    }
    return FALSE;
  }

  // データ読み込みコールバックの設定を行う
  png_set_read_fn(png_ptr, buf, callback_read);
  offset = 8;

  if (setjmp(png_jmpbuf(png_ptr))) {
    // 何かエラーがあれば、ここに飛んでくる

    // png_ptr、info_ptrのすべてのメモリを解放する
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    if (buf) {
      free(buf);
      buf = NULL;
    }
    return FALSE;
  }

  // png画像のシグネチャを読込済みなので、そのぶんをスキップする
  png_set_sig_bytes(png_ptr, 8);

  // png画像を読み込む
  png_read_png(
      png_ptr, info_ptr,
      PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND,
      NULL);

　// png画像の画像情報を取得する
  *outWidth = info_ptr->width;
  *outHeight = info_ptr->height;
  color_type = info_ptr->color_type;
  depth = info_ptr->bit_depth;

  // カラータイプを判別する
  switch (color_type) {
  case PNG_COLOR_TYPE_PALETTE:
    // パレットベースからRGBベースにデータを変換する
    png_set_palette_to_rgb(png_ptr);
    *type = GL_RGB;
    break;

  case PNG_COLOR_TYPE_RGB:
    *type = GL_RGB;
    break;

  case PNG_COLOR_TYPE_RGBA:
    *type = GL_RGBA;
    break;
  default:
     // 想定外のデータであれば破棄する
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    if (buf) {
      free(buf);
      buf = NULL;
    }
    return FALSE;
  }

  // 1ラインのバイト数を求める
  unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
  *outData = (unsigned char*) malloc(row_bytes * *outHeight);

  // png_info構造体から画像データの先頭アドレスを取得する
  png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

  int i;
  for (i = 0; i < *outHeight; i++) {
    // OpenGLのテクスチャのためにpng画像と上下逆転させる
    memcpy(*outData + (row_bytes * (*outHeight - 1 - i)), row_pointers[i],
           row_bytes);
  }

  // 確保したメモリを解放する
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  free(buf);

  return TRUE;
}

