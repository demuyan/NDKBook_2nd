package com.example.jni;

import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.app.Activity;
import android.app.AlertDialog;


public class ExceptionActivity extends Activity {

  private Button mThrowExceptionBtn;
  private TextView mValueTextView;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.exception);

    mThrowExceptionBtn = (Button) findViewById(R.id.JvThrowException);
    mValueTextView = (TextView) findViewById(R.id.JvTextView1);
    mThrowExceptionBtn.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {        

        // protected native int getValue(int index) throws MyException;
        try {
          int value = getValueOfData(9999);
          mValueTextView.setText(String.valueOf(value));
        } catch (Exception exp) {

          AlertDialog.Builder alert = new AlertDialog.Builder(v.getContext());
          alert.setTitle("Exception");
          alert.setMessage(exp.getMessage());
          alert.show();
        }

      }
    });
  }


  // 配列の値を取得する
  int getValue(int index) throws IndexOutOfBoundsException {
    int list[] = { 1, 2 };
    // 範囲外にアクセス（ワザと例外を発生させる）
    return list[index];
  }

  // 値を取得（例外が発生するサンプル）
  protected native int getValueOfData(int index) throws MyException;

  // あらかじめロードするモジュールを指定
  static {
    System.loadLibrary("exception-jni");
  }
}
