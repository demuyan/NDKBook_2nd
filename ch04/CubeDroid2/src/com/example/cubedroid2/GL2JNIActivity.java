package com.example.cubedroid2;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;

import java.io.File;

public class GL2JNIActivity extends Activity {

  GL2JNIView mView;

  @Override 
  protected void onCreate(Bundle icicle) {
    super.onCreate(icicle);
    mView = new GL2JNIView(getApplication());
    setContentView(mView);
  }

  @Override 
  protected void onPause() {
    super.onPause();
    mView.onPause();
  }

  @Override 
  protected void onResume() {
    super.onResume();
    mView.onResume();
  }
}
