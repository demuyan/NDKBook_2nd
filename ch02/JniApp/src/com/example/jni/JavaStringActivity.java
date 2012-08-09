package com.example.jni;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class JavaStringActivity extends Activity {

  protected static final String TAG = "PrimitiveActivity1";

  private Button mAddBtn, mConcatBtn;
  private EditText mEditText1, mEditText2, mEditText3;
  private EditText mString1, mString2, mString3;
  private Button mAddFieldBtn;
  private float mValueFoo;
  private static float mValueBar;
  private String mStringFoo;
  private static String mStringBar;
  private Button mAddStaticFieldBtn;
  private Button mConcatFieldBtn;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.javastring);

    mString1 = (EditText) findViewById(R.id.primCalcEditText11);
    mString2 = (EditText) findViewById(R.id.primCalcEditText12);
    mString3 = (EditText) findViewById(R.id.primCalcEditText13);

    mConcatBtn = (Button) findViewById(R.id.concat_btn);
    mConcatBtn.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {
        String str1 = mString1.getText().toString();
        String str2 = mString2.getText().toString();
        String answer = concatStrings(str1, str2);
        mString3.setText(answer);
      }
    });

    mConcatFieldBtn = (Button) findViewById(R.id.concat_field_btn);
    mConcatFieldBtn.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {
        mStringFoo = mString1.getText().toString();
        mStringBar = mString2.getText().toString();
        String answer = concatFieldStrings();
        mString3.setText(answer);

        Log.d(TAG, "mStringFoo=" + mStringFoo);
      }
    });

  }

  // 文字列を結合する
  protected native String concatStrings(String string1, String string2);
  // フィールド変数の文字列を結合する
  protected native String concatFieldStrings();
  // あらかじめロードするモジュールを指定
  static {
    System.loadLibrary("javastring-jni");
  }
}
