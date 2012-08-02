#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>

#define  LOG_TAG    "nativebitmap"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

// 画像をセピア調に変換する
void convert_to_sepia(AndroidBitmapInfo* info, void* pixels) {

  unsigned long* picbuf = pixels;
  int x, y;
  // 高さのpixel数だけループ
  for (y = 0; y < info->height; y++) {
    // 幅のpixel数だけループ
    for (x = 0; x < info->width; x++) {
      int pixel = picbuf[y * info->width + x];
      // RGBAを個別に取得
      int a = (pixel >> 24) & 0xff;
      int b = (pixel >> 16) & 0xff;
      int g = (pixel >> 8) & 0xff;
      int r = (pixel >> 0) & 0xff;
      // セピア調に変換
      float gray = (r * 0.2989 + g * 0.5866 + b * 0.1144);
      r = (int) (gray * 0.9) & 0xff;
      g = (int) (gray * 0.7) & 0xff;
      b = (int) (gray * 0.4) & 0xff;
      // 変換したデータを格納
      picbuf[y * info->width + x] = (a << 24) | (b << 16) | (g << 8) | r;;
    }
  }
}

// 渡したデータをセピア調に変換
void Java_com_example_nativebitmap_MainActivity_sepiaImage(JNIEnv* env,
                                                           jobject thiz,
                                                           jobject bitmap) {
  // 画像情報を取得
  AndroidBitmapInfo info;
  if (0 > AndroidBitmap_getInfo(env, bitmap, &info)) {
    LOGE("AndroidBitmap_getInfo() failed !");
    return;
  }
  LOGI("imagesize(%d,%d)\n", info.width, info.height);
  // 想定していない画像フォーマットなので処理を中断
  if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
    LOGE("Can't convert");
    return;
  }
  // 他からのBITMAPへのアクセスを排除する
  void* pixels;
  if (0 > AndroidBitmap_lockPixels(env, bitmap, &pixels)) {
    LOGE("AndroidBitmap_lockPixels() failed !");
    return;
  }
  // セピア調に変換する
  convert_to_sepia(&info, pixels);
  // 他からのBITMAPへのアクセスを許可する
  AndroidBitmap_unlockPixels(env, bitmap);
}
