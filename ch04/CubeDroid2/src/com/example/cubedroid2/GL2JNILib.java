package com.example.cubedroid2;

// ネイティブコードのラッパーライブラリ
public class GL2JNILib {

  static {
    System.loadLibrary("cubedroid2");
  }

  public static native void init(int width, int height);
  public static native void step();
}
