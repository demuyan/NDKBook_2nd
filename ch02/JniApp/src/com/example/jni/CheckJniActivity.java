package com.example.jni;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class CheckJniActivity extends Activity {

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
    setContentView(R.layout.check_jni);

    mEditText1 = (EditText) findViewById(R.id.primCalcEditText1);
    mEditText2 = (EditText) findViewById(R.id.primCalcEditText2);
    mEditText3 = (EditText) findViewById(R.id.primCalcEditText3);

    mAddBtn = (Button) findViewById(R.id.add_btn);
    mAddBtn.setOnClickListener(new OnClickListener() {
      @Override
      public void onClick(View v) {
        mValueFoo = Float.parseFloat(mEditText1.getText().toString());
        mValueBar = Float.parseFloat(mEditText2.getText().toString());
        float answer = addFieldValues();
        mEditText3.setText(String.valueOf(answer));
      }
    });
  }

  // 加算する
  protected native float addFieldValues();
  
  // あらかじめロードするモジュールを指定
  static {
    System.loadLibrary("checkjni-jni");
  }
}
