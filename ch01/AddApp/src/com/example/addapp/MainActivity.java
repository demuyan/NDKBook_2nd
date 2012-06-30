package com.example.addapp;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class MainActivity extends Activity {
  private EditText m_editText1;
  private EditText m_editText2;
  private EditText m_editText3;
  private Button m_button;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    // ヴィジットの設定
    m_button = (Button) findViewById(R.id.button);
    m_editText1 = (EditText) findViewById(R.id.editText1);
    m_editText2 = (EditText) findViewById(R.id.editText2);
    m_editText3 = (EditText) findViewById(R.id.editText3);

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

  // 加算する
  protected int addValues(int value1, int value2) {
    return value1 + value2;
  }
}
