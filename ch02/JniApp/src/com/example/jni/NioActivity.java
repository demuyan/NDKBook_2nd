package com.example.jni;

import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class NioActivity extends Activity {
  
  private TextView mTextView;
  private Button mNioBtn1;
  private Button mNioBtn2;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    
    super.onCreate(savedInstanceState);
    setContentView(R.layout.nio_main);

    mTextView = (TextView) findViewById(R.id.textView1);
    mNioBtn1 = (Button) findViewById(R.id.nio_btn_1);
    mNioBtn1.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {
    /////begin nio_samplecode_01
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(10);   /////-----(1)
        byteBuffer.order(ByteOrder.LITTLE_ENDIAN);               /////-----(2)

        int a = 30;
        short b = 20;
        byte c = 10;
        
         // byteBufferに書き込む
        byteBuffer.putInt(0, a);     /////-----(3)ここから
        byteBuffer.putShort(4, b);
        byteBuffer.put(6, c);    /////-----(3)ここまで

        // JNIの関数を呼び出す
        calcBuffer(byteBuffer);  /////-----(4)

        StringBuffer strBuf = new StringBuffer();         /////-----(5)ここから
        strBuf.append("a=" + byteBuffer.getInt(0) + "\n");
        strBuf.append("b=" + byteBuffer.getShort(4) + "\n");
        strBuf.append("c=" + byteBuffer.get(6) + "\n");   /////-----(5)ここまで
        mTextView.setText(strBuf.toString());
        /////end
      }
    });

    mNioBtn2 = (Button) findViewById(R.id.nio_btn_2);
    mNioBtn2.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {
        // JNIの関数を呼び出す
        ByteBuffer byteBuffer = getBuffer(100);
        byteBuffer.order(ByteOrder.LITTLE_ENDIAN);

        StringBuffer strBuf = new StringBuffer();
        strBuf.append("a=" + byteBuffer.getInt(0) + "\n");
        strBuf.append("b=" + byteBuffer.getShort(4) + "\n");
        strBuf.append("c=" + byteBuffer.get(6) + "\n");

        mTextView.setText(strBuf.toString());
      }
    });

  }

  public native int calcBuffer(java.nio.Buffer byteBuffer);
  public native java.nio.ByteBuffer getBuffer(int size);

  static {
    System.loadLibrary("nio-jni");
  }
}
