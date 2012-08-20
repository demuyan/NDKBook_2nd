package com.example.jni;

/////begin exception_samplecode_03
// 自作の例外クラス
public class MyException extends Exception {
  private String mMsg;
  public MyException(String message) {
    mMsg = message;
  }

  public String getMessage() {
    return mMsg;
  }
}
/////end