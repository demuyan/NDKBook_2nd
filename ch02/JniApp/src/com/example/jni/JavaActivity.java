package com.example.jni;

import android.app.Activity;
import android.app.AlertDialog;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class JavaActivity extends Activity {

  protected static final String TAG = "JavaActivity";
  int mValueList[] = { 1, 2, 3, 4, 5 };
  private Button mArrayBtn;
  private EditText mArrayEditText;
  private Button mThrowExceptionBtn;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.java_main);

    mArrayEditText = (EditText) findViewById(R.id.JvSumArrayEditText);
    mArrayBtn = (Button) findViewById(R.id.JvArrayBtn);
    mArrayBtn.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {

        int sum = sumArray(mValueList);
        mArrayEditText.setText(String.valueOf(sum));
        Log.d(TAG, "mValueList[0]="+mValueList[0]);
      }
    });

  }

  // 配列の値を合計する
  protected native int sumArray(int[] list);
  // 配列を取得する
  protected native int[] getArray();

  // あらかじめロードするモジュールを指定
  static {
    System.loadLibrary("java-jni");
  }

}
