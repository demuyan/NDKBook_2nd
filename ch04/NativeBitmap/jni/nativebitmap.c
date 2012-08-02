#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h> /////-----(1)
#define  LOG_TAG    "nativebitmap"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

// 画像をセピア調に変換する
void convert_to_sepia_rgba8888(AndroidBitmapInfo* info, void* pixels) {

  unsigned long* picbuf = pixels;
  int x, y;
  for (y = 0; y < info->height; y++) {
    for (x = 0; x < info->width; x++) {
      int pixel = picbuf[y * info->width + x];

      int a = (pixel >> 24) & 0xff;
      int b = (pixel >> 16) & 0xff;
      int g = (pixel >> 8) & 0xff;
      int r = (pixel >> 0) & 0xff;

      float gray = (r * 0.2989 + g * 0.5866 + b * 0.1144);
      r = (int) (gray * 0.9) & 0xff;
      g = (int) (gray * 0.7) & 0xff;
      b = (int) (gray * 0.4) & 0xff;
      picbuf[y * info->width + x] = (a << 24) | (b << 16) | (g << 8) | r;
    }
  }
}

// 画像をセピア調に変換する
void convert_to_sepia_rgb565(AndroidBitmapInfo* info, void* pixels) {

  unsigned short* picbuf = pixels;
  int x, y;
  for (y = 0; y < info->height; y++) {
    for (x = 0; x < info->width; x++) {
      int pixel = picbuf[y * info->width + x];

      float r = (pixel >> 11) & 0x1f;
      float g = ((pixel >> 5) & 0x3f)/2;
      float b = (pixel >> 0) & 0x1f;

      float gray = (r * 0.2989 + g * 0.5866 + b * 0.1144);

      int r1 = (int)(gray * 0.9) & 0x1f;
      int g1 = (int)((gray * 0.7) * 2) & 0x3f;
      int b1 = (int)(gray * 0.4) & 0x1f;
      picbuf[y * info->width + x] = (r1 << 11) | (g1 << 5) | b1;
    }
  }
}

char format_msg[][10] = { "NONE", "RGBA_8888", "", "", "RGB_565", "", "",
    "RGBA_4444", "A_8", };

void Java_com_example_nativebitmap_MainActivity_sepiaImage(JNIEnv* env,
                                                           jobject thiz,
                                                           jobject bitmap) {
  // 画像情報を取得
  AndroidBitmapInfo info;
  if (0 > AndroidBitmap_getInfo(env, bitmap, &info)) { /////-----(2)
    LOGE("AndroidBitmap_getInfo() failed !");
    return;
  }
  LOGI("imagesize(%d,%d)\n", info.width, info.height);
  // 指定フォーマット以外は処理を中断
  if (!(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 || info.format == ANDROID_BITMAP_FORMAT_RGB_565)){ /////-----(3)
    LOGE("Can't convert : format=%s",format_msg[info.format]);
    return;
  }
  LOGI("ImageFormat=%s",format_msg[info.format]);

  // 他からのBITMAPへのアクセスを排除する
  void* pixels;
  if (0 > AndroidBitmap_lockPixels(env, bitmap, &pixels)) { /////-----(4)
    LOGE("AndroidBitmap_lockPixels() failed !");
    return;
  }
  // セピア調に変換する
  switch(info.format){   /////-----(5)
  case ANDROID_BITMAP_FORMAT_RGBA_8888:
    convert_to_sepia_rgba8888(&info, pixels);
    break;
  case ANDROID_BITMAP_FORMAT_RGB_565:
    convert_to_sepia_rgb565(&info, pixels);
    break;
  }
  // 他からのBITMAPへのアクセスを許可する
  AndroidBitmap_unlockPixels(env, bitmap); /////-----(6)
}
