package com.example.nativelua;

import com.example.nativelua.R.id;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends Activity {

  private Button runBtn;
  private TextView outputTextView;
  private EditText scriptEditText;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    // Luaを初期化する
    initLua();
    // 
    scriptEditText = (EditText) findViewById(R.id.scriptEditText);
    outputTextView = (TextView) findViewById(R.id.outputTextView);
    // サンプルコードを予め登録する
    scriptEditText.setText("value=2+3");
    
    runBtn = (Button) findViewById(id.runBtn);
    runBtn.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {
        runScript(scriptEditText.getText().toString());
        // 変数valueを取得する
        int value = getInteger("value");
        outputTextView.setText("value=" + String.valueOf(value));
      }
    });
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    closeLua();
  }

  // Luaを初期化する
  protected native String initLua();
  // Luaを終了する
  protected native void closeLua();
  // スクリプトを実行する
  protected native String runScript(String script);
  // 変数（整数値）を取得する
  protected native int getInteger(String valueName);

  // あらかじめロードするモジュールを指定
  static {
    System.loadLibrary("nativelua-jni");
  }

}
