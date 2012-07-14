//////begin ch_multi_samplecode_2
package com.example.nativemediaplayer;

import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.view.Menu;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;

public class MainActivity extends Activity {

  static final String TAG = "NativeMediaPlayer";
  private SurfaceView mSurfaceView1;
  private SurfaceHolder mSurfaceHolder1;
  protected String mSourceString;
  private SurfaceHolderVideoSink mSurfaceHolder1VideoSink;

  protected boolean mIsPlayingStreaming;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    // ネイティブメディアプレイヤーを初期化
    createEngine();

    // サーフェイスを生成
    mSurfaceView1 = (SurfaceView) findViewById(R.id.surfaceview1);
    mSurfaceHolder1 = mSurfaceView1.getHolder();
    mSurfaceHolder1.addCallback(new SurfaceHolder.Callback() {

      public void surfaceChanged(SurfaceHolder holder, int format, int width,
                                 int height) {
        Log.v(TAG, "surfaceChanged format=" + format + ", width=" + width
            + ", height=" + height);
      }

      public void surfaceCreated(SurfaceHolder holder) {
        Log.v(TAG, "surfaceCreated");
        setSurface(holder.getSurface());
      }

      public void surfaceDestroyed(SurfaceHolder holder) {
        Log.v(TAG, "surfaceDestroyed");
      }

    });

    // ストリーミングデータ選択
    Spinner sourceSpinner = (Spinner) findViewById(R.id.source_spinner);
    ArrayAdapter<CharSequence> sourceAdapter = ArrayAdapter.createFromResource(
        this, R.array.source_array, android.R.layout.simple_spinner_item);
    sourceAdapter
        .setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
    sourceSpinner.setAdapter(sourceAdapter);
    sourceSpinner
        .setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {

          @Override
          public void onItemSelected(AdapterView<?> parent, View view, int pos,
                                     long id) {
            mSourceString = parent.getItemAtPosition(pos).toString();
            Log.v(TAG, "onItemSelected " + mSourceString);
          }

          @Override
          public void onNothingSelected(AdapterView parent) {
            Log.v(TAG, "onNothingSelected");
            mSourceString = null;
          }

        });

    // ビデオシンクの生成
    mSurfaceHolder1VideoSink = new SurfaceHolderVideoSink(mSurfaceHolder1);
    
    // Start/Pauseボタン
    ((Button) findViewById(R.id.start_btn))
        .setOnClickListener(new View.OnClickListener() {

          boolean created = false;

          public void onClick(View view) {
            if (!created) {
              mSurfaceHolder1VideoSink.useAsSinkForNative();
              if (mSourceString != null) {
                created = createStreamingMediaPlayer(mSourceString);
              }
            }
            if (created) {
              mIsPlayingStreaming = !mIsPlayingStreaming;
              setPlayingStreamingMediaPlayer(mIsPlayingStreaming);
            }
          }

        });

    // native MediaPlayer rewind
    ((Button) findViewById(R.id.rewind_btn))
        .setOnClickListener(new View.OnClickListener() {

          public void onClick(View view) {
            if (mSurfaceHolder1VideoSink != null) {
              rewindStreamingMediaPlayer();
            }
          }
        });

  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.activity_main, menu);
    return true;
  }

  public static native void createEngine();
  public static native boolean createStreamingMediaPlayer(String filename);
  public static native void setPlayingStreamingMediaPlayer(boolean isPlaying);
  public static native void shutdown();
  public static native void setSurface(Surface surface);
  public static native void rewindStreamingMediaPlayer();

  static {
    System.loadLibrary("movieplayer-jni");
  }

  // ビデオシンク用サーフェイスを設定する
  static class SurfaceHolderVideoSink{
    private final SurfaceHolder mSurfaceHolder;
    SurfaceHolderVideoSink(SurfaceHolder surfaceHolder) {
      mSurfaceHolder = surfaceHolder;
    }
    void setFixedSize(int width, int height) {
      mSurfaceHolder.setFixedSize(width, height);
    }
    void useAsSinkForNative() {
      Surface s = mSurfaceHolder.getSurface(); /////-----(1)
      setSurface(s);
      s.release();
    }
  }
}
/////end