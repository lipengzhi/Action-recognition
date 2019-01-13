//
// Created by HowieWang on 2018/7/17.
//

#include <jni.h>
#include "utils.hpp"
#include "classholder.hpp"

std::vector<TrackPoint> points2vector(JNIEnv *env, jobjectArray aPoints) {
    jsize size = env->GetArrayLength(aPoints);
    std::vector<TrackPoint> vPoints;
    jsize i = 0;
    jclass pointClazz = ClassHolder::Point;
    for (; i < size; i++) {
        jobject jPoint = env->GetObjectArrayElement(aPoints, i);
        jdouble x = getDoubleProp(env, pointClazz, jPoint, "x");
        jdouble y = getDoubleProp(env, pointClazz, jPoint, "y");
        jdouble z = getDoubleProp(env, pointClazz, jPoint, "z");
        TrackPoint p = TrackPoint(x, y, z);
        vPoints.push_back(p);
    }
    return vPoints;
}

jint getIntProp(JNIEnv *env, jclass clazz, jobject instance, const char *propName) {
    jfieldID fid = env->GetFieldID(clazz, propName, "I");
    //通过 field id 获取属性的值
    return env->GetIntField(instance, fid);
}

jdouble getDoubleProp(JNIEnv *env, jclass clazz, jobject instance, const char *propName) {
    jfieldID fid = env->GetFieldID(clazz, propName, "D");
    //通过 field id 获取属性的值
    return env->GetDoubleField(instance, fid);
}


jobject recResult2jobject(JNIEnv *env, RecResult recResult) {
    jclass clazz = ClassHolder::RecResult;
    jmethodID constructor = env->GetMethodID(clazz, "<init>", "(IIIDDD)V");
    return env->NewObject(clazz, constructor, recResult.start_idx, recResult.end_idx, recResult.act_count,
                          recResult.similarity, recResult.score, recResult.g_scale);
}




