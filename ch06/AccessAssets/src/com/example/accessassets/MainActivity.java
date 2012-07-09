package com.example.accessassets;

import android.os.Bundle;
import android.app.Activity;
import android.content.res.AssetManager;
import android.view.Menu;
import android.widget.TextView;

public class MainActivity extends Activity {

  private TextView mTextView;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    mTextView = (TextView) findViewById(R.id.textView1);
    String outoutText = "";

    // AssetManagerを取得する
    AssetManager mgr = this.getAssets();

    outoutText += "--- /assetsのファイル一覧 ---\n";
    String[] filelist = getAssetsFilelistInDir(mgr, "");
    for (String str : filelist) {
      outoutText += str + "\n";
    }

    outoutText += "--- /assets/imageのファイル一覧 ---\n";
    filelist = getAssetsFilelistInDir(mgr, "image");
    for (String str : filelist) {
      outoutText += str + "\n";
    }

    outoutText += "--- /assets/foo.txtの内容表示 ---\n";
    outoutText = outoutText + getAssetsReadTextfile(mgr, "foo.txt") + "\n";

    outoutText += "--- /assets/bar.txtの内容表示 ---\n";
    outoutText = outoutText + getAssetsReadTextfile(mgr, "bar.txt") + "\n";

    // TextViewに結果を出力する
    mTextView.setText(outoutText);
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.activity_main, menu);
    return true;
  }

  // 指定したディレクトリ内のファイル一覧を取得
  protected native String[] getAssetsFilelistInDir(AssetManager mgr,
                                                   String dirname);
  // 指定したテキストファイルの内容を取得する
  protected native String getAssetsReadTextfile(AssetManager mgr,
                                                String filename);

  static {
    System.loadLibrary("accessassets");
  }
}
