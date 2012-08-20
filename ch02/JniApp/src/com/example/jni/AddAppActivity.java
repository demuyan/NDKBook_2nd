package com.example.jni;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class AddAppActivity extends Activity {

  private EditText m_editText1;
  private EditText m_editText2;
  private EditText m_editText3;
  private Button m_button;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.add_app);

    // ヴィジットの設定
    m_button = (Button) findViewById(R.id.addAppButton);
    m_editText1 = (EditText) findViewById(R.id.addAppEditText1);
    m_editText2 = (EditText) findViewById(R.id.addAppEditText2);
    m_editText3 = (EditText) findViewById(R.id.addAppEditText3);

    // 加算ボタンのクリックイベント
    m_button.setOnClickListener(new OnClickListener() {
      public void onClick(View v) {
        // テキストボックスから整数値を取得する
        int value1 = Integer.parseInt(m_editText1.getText().toString());
        int value2 = Integer.parseInt(m_editText2.getText().toString());
        // 加算する
        int answer = addValues(value1, value2);
        // 答えの欄に表示する
        m_editText3.setText(String.valueOf(answer));
      }
    });
  }

  // 加算する（nativeをつけることでJNIで処理することを宣言する)
  protected native int addValues(int value1, int value2);

  // あらかじめロードするモジュールを指定
  static {
    System.loadLibrary("addapp-jni");
  }
}
