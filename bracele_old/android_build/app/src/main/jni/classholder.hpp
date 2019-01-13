//
// Created by HowieWang on 2018/8/7.
//

#ifndef ANDROID_BUILD_CLASSHOLDER_HPP
#define ANDROID_BUILD_CLASSHOLDER_HPP


#include <jni.h>

class ClassHolder {
public:
    static jclass RecResult;
    static jclass Callback;
    static jclass Point;

    static void init(JNIEnv *env);

    static void release(JNIEnv *env);
};

#endif //ANDROID_BUILD_CLASSHOLDER_HPP
