package com.example.cubedroid2;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

// OpenGL|ES 2.0向けSurfaceView
class GL2JNIView extends GLSurfaceView {
  private static String TAG = "GL2JNIView";
  private static final boolean DEBUG = false;

  public GL2JNIView(Context context) {
    super(context);
    init(false, 24, 0);
  }

  public GL2JNIView(Context context, boolean translucent, int depth, int stencil) {
    super(context);
    init(translucent, depth, stencil);
  }

  private void init(boolean translucent, int depth, int stencil) {

    // GLSurfaceView関数は初期設定でRGB_565形式を使用する。
    // 半透明表示を行う場合は、PixelFormat.TRANSLUCENTをセットする
    if (translucent) {
      this.getHolder().setFormat(PixelFormat.TRANSLUCENT);
    }

    // レンダリングコンテキストを設定する
    setEGLContextFactory(new ContextFactory());

    // レンダリングコンテキストをセットする
    // 詳細は、同ファイルにあるConfigChooserクラスを参照
    setEGLConfigChooser( translucent ?
                         new ConfigChooser(8, 8, 8, 8, depth, stencil) :
                         new ConfigChooser(5, 6, 5, 0, depth, stencil) );

    // フレームごとの描画を行うレンダラーをセットする
    setRenderer(new Renderer());
  }

  // OpenGL|ES 2.0向けEGLレンダリングコンテキストを作成
  private static class ContextFactory implements GLSurfaceView.EGLContextFactory {
    private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

    public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig) {
      checkEglError("Before eglCreateContext", egl);

      // OpenGL ES 2.0向けレンダリングコンテキストを指定
      int[] attrib_list = {
        EGL_CONTEXT_CLIENT_VERSION, 2, 
        EGL10.EGL_NONE };
      // OpenGL ES 2.0向けレンダリングコンテキスト作成
      EGLContext context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list);
      // エラーが発生していないかチェックする
      checkEglError("After eglCreateContext", egl);
      return context;
    }

    // EGLレンダリングコンテキストを破棄する
    public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) {
      egl.eglDestroyContext(display, context);
    }
  }

  // EGLの処理でエラーが発生していないか確認する
  private static void checkEglError(String prompt, EGL10 egl) {
    int error;
    while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) {
      Log.e(TAG, String.format("%s: EGL error: 0x%x", prompt, error));
    }
  }

  // 指定したディスプレイ条件一致したEGLConfigを返す
  private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser {

    public ConfigChooser(int r, int g, int b, int a, int depth, int stencil) {
      mRedSize = r;
      mGreenSize = g;
      mBlueSize = b;
      mAlphaSize = a;
      mDepthSize = depth;
      mStencilSize = stencil;
    }

    // 表示条件のEGLパラメータ
    //  EGL10.EGL_RED_SIZE : 
    //  EGL10.EGL_GREEN_SIZE : 
    //  EGL10.EGL_BLUE_SIZE : 
    //  EGL10.EGL_RENDERBLE_TYPE : OpenGL ES2.0がレンダリング可能
    //  EGL10.EGL_NONE : 終端
    private static int EGL_OPENGL_ES2_BIT = 4;
    private static int[] s_configAttribs2 =
    {
      EGL10.EGL_RED_SIZE, 4,
      EGL10.EGL_GREEN_SIZE, 4,
      EGL10.EGL_BLUE_SIZE, 4,
      EGL10.EGL_DEPTH_SIZE, 24, 
      EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
      EGL10.EGL_NONE
    };

    public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {

      // 条件に合ったEGLフレームバッファ設定の数を取得する
      int[] num_config = new int[1];
      egl.eglChooseConfig(display, s_configAttribs2, null, 0, num_config);

      int numConfigs = num_config[0];

      // 条件に該当する設定が無ければ例外を返す
      if (numConfigs <= 0) {
        throw new IllegalArgumentException("No configs match configSpec");
      }

      // 与えた条件に合ったEGLフレームバッファ設定のリストを取得する
      EGLConfig[] configs = new EGLConfig[numConfigs];
      egl.eglChooseConfig(display, s_configAttribs2, configs, numConfigs, num_config);

      return chooseConfig(egl, display, configs);
    }

    // 条件にあった設定を取得する
    public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display,
                                  EGLConfig[] configs) {
      for(EGLConfig config : configs) {
        // デプスバッファのサイズを取得する
        int d = findConfigAttrib(egl, display, config,
                                 EGL10.EGL_DEPTH_SIZE, 0);
        // ステンシルバッファのサイズを取得する
        int s = findConfigAttrib(egl, display, config,
                                 EGL10.EGL_STENCIL_SIZE, 0);

        if (d < mDepthSize || s < mStencilSize)
          continue;

        // 赤のバッファサイズを取得する
        int r = findConfigAttrib(egl, display, config,
                                 EGL10.EGL_RED_SIZE, 0);
        // 緑のバッファサイズを取得する
        int g = findConfigAttrib(egl, display, config,
                                 EGL10.EGL_GREEN_SIZE, 0);
        // 青のバッファサイズを取得する
        int b = findConfigAttrib(egl, display, config,
                                 EGL10.EGL_BLUE_SIZE, 0);
        // アルファのバッファサイズを取得する
        int a = findConfigAttrib(egl, display, config,
                                 EGL10.EGL_ALPHA_SIZE, 0);
        
        // バッファサイズが同一であるものを返す
        if (r == mRedSize && g == mGreenSize && b == mBlueSize && a == mAlphaSize)
          return config;
      }
      return null;
    }

    // 設定属性を検索する
    private int findConfigAttrib(EGL10 egl, EGLDisplay display,
                                 EGLConfig config, int attribute, int defaultValue) {

      if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
        return mValue[0];
      }
      return defaultValue;
    }

    protected int mRedSize;
    protected int mGreenSize;
    protected int mBlueSize;
    protected int mAlphaSize;
    protected int mDepthSize;
    protected int mStencilSize;
    private int[] mValue = new int[1];
  }

  // レンダラー
  private static class Renderer implements GLSurfaceView.Renderer {
    public void onDrawFrame(GL10 gl) {
      GL2JNILib.step();
    }

    // サーフェイスが変化した時に行う
    public void onSurfaceChanged(GL10 gl, int width, int height) {
      GL2JNILib.init(width, height);
    }

    // サーフェイスが生成された時に行う
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
      gl.glEnable(GL10.GL_DEPTH_TEST);
    }
  }
}
