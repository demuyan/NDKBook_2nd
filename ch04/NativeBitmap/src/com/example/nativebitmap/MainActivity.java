package com.example.nativebitmap;

import android.app.Activity;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;

public class MainActivity extends Activity {
  private ImageView imageview1;
  private Bitmap bitmap;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    // ImageViewに画像を表示
    imageview1 = (ImageView) findViewById(R.id.imageview1);
    Resources r = getResources();
    bitmap = BitmapFactory.decodeResource(r, R.drawable.lena);
    imageview1.setImageBitmap(bitmap);

    Button btn = (Button) findViewById(R.id.button1);
    btn.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View arg0) {
         // セピア調に変化させる
        sepiaImage(bitmap);
        // 変化後の画像をセットする
        imageview1.setImageBitmap(bitmap);
      }
    });
  }

  protected native int sepiaImage(Bitmap bitmap);

  static {
    System.loadLibrary("nativebitmap");
  }

}