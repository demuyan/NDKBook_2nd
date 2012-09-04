package com.example.neon;

import com.example.neonapp.R;

import android.os.AsyncTask;
import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.support.v4.app.NavUtils;

public class MainActivity extends Activity {

  private Button mButton1, mButton2;
  private TextView mTextView1;
  private TextView mTextView2;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    mTextView1 = (TextView) findViewById(R.id.textView1);
    mTextView2 = (TextView) findViewById(R.id.textView2);

    mButton1 = (Button) findViewById(R.id.button1);
    mButton1.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {
        String msg = "";
        mTextView1.setText("計測中");
        new TaskMulti().execute();
      }
    });

    mButton2 = (Button) findViewById(R.id.button2);
    mButton2.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {
        String msg = "";
        mTextView2.setText("計測中");
        new TaskMultiNeon().execute();
      }
    });
  }

  protected native void multi();

  protected native void multineon();

  // あらかじめロードするモジュールを指定
  static {
    System.loadLibrary("neon-jni");
  }

  private class AsyncTaskBase extends AsyncTask<String, Void, String> {

    @Override
    protected String doInBackground(String... params) {
      String msg = "";
      float total_time = 0;
      for (int j = 0; j < 10; j++) {
        long start = System.currentTimeMillis();
        for (int i = 0; i < 1000; i++)
          func();
        long end = System.currentTimeMillis();
        total_time += end - start;
        msg += String.valueOf(end - start) + " ms  ";
      }
      msg += "\n平均 " + String.valueOf(total_time / 10) + "ms";
      return msg;
    }

    protected void func() {
    }
  }

  private class TaskMulti extends AsyncTaskBase {

    @Override
    protected void func() {
      multi();
    }

    @Override
    protected void onPostExecute(String result) {
      mTextView1.setText(result);
      super.onPostExecute(result);
    }
  }

  private class TaskMultiNeon extends AsyncTaskBase {

    @Override
    protected void func() {
      multineon();
    }

    @Override
    protected void onPostExecute(String result) {
      mTextView2.setText(result);
      super.onPostExecute(result);
    }
  }
}
