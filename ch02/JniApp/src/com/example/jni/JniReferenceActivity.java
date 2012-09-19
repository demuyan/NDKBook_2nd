package com.example.jni;

import android.os.Bundle;
import android.app.Activity;
import android.app.AlertDialog;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class JniReferenceActivity extends Activity {

  protected static final String TAG = "JniReferneceActivity";
  private Button mRefBtn1,mRefBtn2,mRefBtn3, mRefBtn4, mRefBtn5;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_jni_reference);

    mRefBtn1 = (Button) findViewById(R.id.JvRefBtn1);
    mRefBtn1.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {        
       
        String str = "LocalRef(NG)";
        setStringNG(str);
        getString();
      }
    });

    mRefBtn2 = (Button) findViewById(R.id.JvRefBtn2);
    mRefBtn2.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {        
       
        String str = "LocalRef(OK)";
        setStringOK(str);
        getString();
      }
    });

    mRefBtn3 = (Button) findViewById(R.id.JvRefBtn3);
    mRefBtn3.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {        
       
        String str = getFrameStringNG();
        Log.d(TAG, str);
      }
    });

    mRefBtn4 = (Button) findViewById(R.id.JvRefBtn4);
    mRefBtn4.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {        
       
        String str = getFrameStringOK();
        Log.d(TAG, str);
      }
    });

    mRefBtn5 = (Button) findViewById(R.id.JvRefBtn5);
    mRefBtn5.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {        
       
        String str = getFrameStringOK();
        Log.d(TAG, str);
      }
    });
    
    
  }

  protected native void setStringNG(String str);
  protected native void setStringOK(String str);
  protected native void getString();
  protected native String getFrameStringNG();
  protected native String getFrameStringOK();

  protected native void weakGlobalRef();

  // あらかじめロードするモジュールを指定
  static {
    System.loadLibrary("jnireference-jni");
  }
}
