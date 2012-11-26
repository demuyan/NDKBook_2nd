
package com.example.nativelua;

import com.example.nativelua.R.id;

import android.os.Bundle;
import android.app.Activity;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends Activity {

  private Button runBtn;
  private EditText scriptEditText;
  private TextView valueTextView1,valueTextView2,valueTextView3;
  private EditText valueEditText1,valueEditText2,valueEditText3;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    // Luaを初期化する
    initLua();
    // Luaスクリプトの入力項目
    scriptEditText = (EditText) findViewById(R.id.scriptEditText);
    // 結果を出力する項目
    valueEditText1 = (EditText) findViewById(R.id.valueEditText1);
    valueTextView1 = (TextView) findViewById(R.id.valueTextView1);
    valueEditText2 = (EditText) findViewById(R.id.valueEditText2);
    valueTextView2 = (TextView) findViewById(R.id.valueTextView2);
    valueEditText3 = (EditText) findViewById(R.id.valueEditText3);
    valueTextView3 = (TextView) findViewById(R.id.valueTextView3);
    // サンプルコードを予め登録する
    scriptEditText.setText("value1=2+3");
    
    runBtn = (Button) findViewById(id.runBtn);
    runBtn.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {
        runScript(scriptEditText.getText().toString());
          // テキストボックスで指定した変数の値を取得する
        int value = getInteger(valueEditText1.getText().toString());
        valueTextView1.setText("="+Integer.toString(value));

        value = getInteger(valueEditText2.getText().toString());
        valueTextView2.setText("="+Integer.toString(value));
        
        value = getInteger(valueEditText3.getText().toString());
        valueTextView3.setText("="+Integer.toString(value));
      }
    });
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    // Luaを終了する
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
