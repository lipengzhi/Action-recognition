//
// Created by HowieWang on 2018/7/18.
//

#ifndef ANDROID_BUILD_UTILS_HPP
#define ANDROID_BUILD_UTILS_HPP

#include <jni.h>
#include "../../../../../smart_bracelet/exercise_recognition.hpp"

jint getIntProp(JNIEnv *env, jclass clazz, jobject instance, const char *propName);

jdouble getDoubleProp(JNIEnv *env, jclass clazz, jobject instance, const char *propName);

std::vector<TrackPoint> points2vector(JNIEnv *env, jobjectArray aPoints);

jobject recResult2jobject(JNIEnv *env, RecResult recResult);

#endif //ANDROID_BUILD_UTILS_HPP
