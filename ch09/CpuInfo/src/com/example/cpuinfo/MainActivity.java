package com.example.cpuinfo;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import android.support.v4.app.NavUtils;

public class MainActivity extends Activity {

    private TextView mTextView;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        mTextView = (TextView)findViewById(R.id.textView2);
        mTextView.setText(getCpuInfo());
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_main, menu);
        return true;
    }

    // CPU情報を取得する
    protected native String getCpuInfo();

    // あらかじめロードするモジュールを指定
    static {
      System.loadLibrary("cpuinfo-jni");
    }
   
}
