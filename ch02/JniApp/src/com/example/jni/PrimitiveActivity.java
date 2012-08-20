package com.example.jni;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class PrimitiveActivity extends Activity {

  protected static final String TAG = "PrimitiveActivity1";

  private Button mAddBtn, mConcatBtn;
  private EditText mEditText1, mEditText2, mEditText3;
  private Button mAddFieldBtn;
  private float mValueFoo;
  private static float mValueBar;
  private Button mAddStaticFieldBtn;

  private Button mAddValStrBtn;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.primitive_activity1);

    mEditText1 = (EditText) findViewById(R.id.primCalcEditText1);
    mEditText2 = (EditText) findViewById(R.id.primCalcEditText2);
    mEditText3 = (EditText) findViewById(R.id.primCalcEditText3);

    mAddBtn = (Button) findViewById(R.id.add_btn);
    mAddBtn.setOnClickListener(new OnClickListener() {
      @Override
      public void onClick(View v) {

        float val1 = Float.parseFloat(mEditText1.getText().toString());
        float val2 = Float.parseFloat(mEditText2.getText().toString());
        float answer = addValues(val1, val2);
        mEditText3.setText(String.valueOf(answer));
      }
    });

    mAddFieldBtn = (Button) findViewById(R.id.add_field_btn);
    mAddFieldBtn.setOnClickListener(new OnClickListener() {
      @Override
      public void onClick(View v) {
        mValueFoo = Float.parseFloat(mEditText1.getText().toString());
        mValueBar = Float.parseFloat(mEditText2.getText().toString());
        float answer = addFieldValues();
        mEditText3.setText(String.valueOf(answer));
      }
    });

    mAddValStrBtn = (Button) findViewById(R.id.add_valstr_btn);
    mAddValStrBtn.setOnClickListener(new OnClickListener() {
      @Override
      public void onClick(View v) {
        float answer = addValuesStr(mEditText1.getText().toString(),mEditText2.getText().toString()); 
        mEditText3.setText(String.valueOf(answer));
      }
    });
  }

  // 加算する
  protected native float addValues(float value1, float value2);
  // フィールド変数の値を加算する
  protected native float addFieldValues();
  // 文字列中となっている数値を加算する
  protected native float addValuesStr(String str1, String str2);
  
  // あらかじめロードするモジュールを指定
  static {
    System.loadLibrary("primitive-jni");
  }
}
