package com.gotokeep.keep.kitbit.activity;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;

import com.gotokeep.keep.kitbit.R;
import com.gotokeep.keep.kitbit.gesturerecognition.GestureRecognitionUtils;
import com.gotokeep.keep.kitbit.gesturerecognition.Point;
import com.gotokeep.keep.kitbit.gesturerecognition.RecResult;
import com.gotokeep.keep.kitbit.gesturerecognition.RecStatus;

import java.util.Arrays;

public class MainActivity extends AppCompatActivity {

    private boolean templateLoaded = false;
    private static final String TAG = "GESTURE";

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        requestPermission();

        findViewById(R.id.load_template).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int result = GestureRecognitionUtils.setTemplate(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS).getAbsolutePath() + "/template.csv");
                if (result == RecStatus.SUCCESS) {
                    templateLoaded = true;
                } else {
                    Log.i(TAG, "template load failure，模板文件要求放在手机的download目录下");
                }
            }
        });

        findViewById(R.id.click_run).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!templateLoaded) {
                    Log.i(TAG, "请把模板文件template.csx放在Download目录下");
                    return;
                }
                new Thread() {
                    @Override
                    public void run() {
                        super.run();
                        Point[] points;
                        while ((points = Data.getPoints()) != null) {
//                    Log.i(TAG, "time-before"+System.currentTimeMillis());
                            GestureRecognitionUtils.detecting(Arrays.asList(points));
//                    Log.i("GESTURE", "time-after" + sdf.format(new Date(Long.parseLong(String.valueOf(System.currentTimeMillis())))));
                        }
                    }
                }.start();
            }
        });
    }

    private void requestPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED) {
                return;
            }
            requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 10000);
        }
    }
}
