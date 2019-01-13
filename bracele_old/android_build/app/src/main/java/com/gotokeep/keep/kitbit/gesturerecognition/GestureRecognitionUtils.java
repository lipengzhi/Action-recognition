package com.gotokeep.keep.kitbit.gesturerecognition;

import java.util.List;

public class GestureRecognitionUtils {
    static {
        System.loadLibrary("kitbit-gesture-recognition");
    }

    /**
     * 设置检测结果回调
     * @param callback
     */
    public static native void setCallback(Callback callback);

    /**
     * 移除回调
     */
    public static native void removeCallback();

    /**
     * 设置模板文件
     *
     * @param tmpFilePath
     * @return 返回 int 值，可选择值在 RecStatus 中维护
     */
    public static native int setTemplate(String tmpFilePath);

    /**
     * 当新数据来了，调用detecting函数，进行动作检测
     *
     * @param newPoints，新数据数组，或新数据List
     * @return 返回一个动作检测结果RecResult，如果没有检测到，返回null
     */
    public static native void detecting(Point[] newPoints);

    public static void detecting(List<Point> newPoints) {
        if (newPoints == null) {
            return;
        }
        Point[] pointArray = new Point[newPoints.size()];
        pointArray = newPoints.toArray(pointArray);
        detecting(pointArray);
    }

    public interface Callback {

        void onResult(RecResult result);
    }
}
