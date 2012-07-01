package com.example.accessassets;

import android.os.Bundle;
import android.app.Activity;
import android.content.res.AssetManager;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import android.support.v4.app.NavUtils;

public class MainActivity extends Activity {

    private TextView mTextView;

	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mTextView = (TextView)findViewById(R.id.textView1);
    
       AssetManager mgr = this.getAssets();
		String[] filelist = getAssetsFilelistInDir(mgr,"image");

		String text = "";
		for(String str : filelist){
			text += str + "\n";
		}
		
		text = text + getAssetsReadFile(mgr,"foo.txt") + "\n";
		text = text + getAssetsReadFile(mgr,"bar.txt") + "\n";
		mTextView.setText(text);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_main, menu);
        return true;
    }

    protected native String[] getAssetsFilelistInDir(AssetManager mgr, String dirname);
    protected native String getAssetsReadFile(AssetManager mgr, String filename);
    
    /** Load jni .so on initialization */
    static {
         System.loadLibrary("accessassets");
    }
    
}
