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

  u_char* p = (u_char*) png_get_io_ptr(pPng);
  memcpy(buf, p + offset, size);
  offset += size;
}

/////begin gl11tex_samplecode_2
int loadPngImage(AAssetManager* mgr, char *filename, png_uint_32* outWidth,
    png_uint_32* outHeight, GLint *type, u_char **outData) {

  png_structp png_ptr;
  png_infop info_ptr;
  int depth;
  int color_type;

  // ファイルをオープンする
  AAsset* assetFile = AAssetManager_open(mgr, filename, AASSET_MODE_RANDOM);

  int size = AAsset_getLength(assetFile);
  u_char* buf = (u_char*) malloc(size);
  AAsset_read(assetFile, buf, size);
  AAsset_close(assetFile);

  if (png_sig_cmp(buf, 0, 8) != 0)
    return FALSE;

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (png_ptr == NULL) {
    if (buf) {
      free(buf);
      buf = NULL;
    }
    return FALSE;
  }

  /* Allocate/initialize the memory
   * for image information.  REQUIRED. */
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    if (buf) {
      free(buf);
      buf = NULL;
    }
    return FALSE;
  }

  // Prepares reading operation by setting-up a read callback.
  png_set_read_fn(png_ptr, buf, callback_read);
  offset = 8;

  if (setjmp(png_jmpbuf(png_ptr))) {
    /* Free all of the memory associated
     * with the png_ptr and info_ptr */
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    /* If we get here, we had a
     * problem reading the file */
    if (buf) {
      free(buf);
      buf = NULL;
    }
    return FALSE;
  }

  /* If we have already
   * read some of the signature */
  png_set_sig_bytes(png_ptr, 8);

  png_read_png(png_ptr, info_ptr,
      PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND,
      NULL);

  *outWidth = info_ptr->width;
  *outHeight = info_ptr->height;
  color_type = info_ptr->color_type;
  depth = info_ptr->bit_depth;

  switch (color_type) {
  case PNG_COLOR_TYPE_PALETTE:
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
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    if (buf) {
      free(buf);
      buf = NULL;
    }
    return FALSE;
  }

  unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
  *outData = (unsigned char*) malloc(row_bytes * *outHeight);

  png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

  int i;
  for (i = 0; i < *outHeight; i++) {
    // note that png is ordered top to
    // bottom, but OpenGL expect it bottom to top
    // so the order or swapped
    memcpy(*outData + (row_bytes * (*outHeight - 1 - i)), row_pointers[i],
        row_bytes);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

  free(buf);

  /* That's it */
  return TRUE;
}
/////end
