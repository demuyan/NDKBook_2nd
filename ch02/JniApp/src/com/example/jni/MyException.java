package com.example.jni;


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
