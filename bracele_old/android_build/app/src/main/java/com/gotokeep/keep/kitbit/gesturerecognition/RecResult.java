package com.gotokeep.keep.kitbit.gesturerecognition;

public class RecResult {
    private int startIndex;
    private int endIndex;
    private int count;//动作计次
    private double dist;//相似度
    private double score;//分数
    private double gScale;//与模板动作的路程或累加距离的比值

    public RecResult(int startIndex, int endIndex,int count, double dist, double score, double gScale) {
        this.startIndex = startIndex;
        this.endIndex = endIndex;
        this.count = count;
        this.dist = dist;
        this.score = score;
        this.gScale = gScale;
    }

    @Override
    public String toString() {
        String s = "startIndex=" + startIndex + ",endIndex=" + endIndex +",count=" + count + ",dist=" + dist + ",score=" + score + ",gScale=" + gScale;
        return s;
    }
}
