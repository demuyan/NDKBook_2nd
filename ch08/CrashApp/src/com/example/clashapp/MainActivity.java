package com.example.clashapp;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class MainActivity extends Activity {
    private Button m_button;

	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        
        
		// ヴィジットの設定
		m_button = (Button) findViewById(R.id.button1);

		// 加算ボタンのクリックイベント
		m_button.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				crashProcess();
			}
		});
	}

	// 加算する（nativeをつけることでJNIで処理することを宣言する)
	protected native void crashProcess();
	
	// あらかじめロードするモジュールを指定
	static {
        System.loadLibrary("process");
    }
}