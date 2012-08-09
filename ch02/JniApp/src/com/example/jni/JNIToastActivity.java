package com.example.jni;

import android.app.Activity;

import android.app.Activity;
import android.app.AlertDialog;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class JNIToastActivity extends Activity {

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    setContentView(R.layout.toast_main);

    Button btn = (Button) findViewById(R.id.btn);
    btn.setOnClickListener(new View.OnClickListener() {

      @Override
      public void onClick(View v) {
        // Toastを表示
        CharSequence text = "JNIからToast表示";
        displayToast(text);
      }
    });

  }

  public native void displayToast(CharSequence charseq);

  static {
    System.loadLibrary("jnitoast-jni");
  }

}
