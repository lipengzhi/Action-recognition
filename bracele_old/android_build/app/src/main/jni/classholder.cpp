//
// Created by HowieWang on 2018/8/7.
//

#include "classholder.hpp"


jclass ClassHolder::RecResult = NULL;
jclass ClassHolder::Callback = NULL;
jclass ClassHolder::Point = NULL;

void ClassHolder::init(JNIEnv *env) {

    RecResult = (jclass)env->NewGlobalRef(env->FindClass("com/gotokeep/keep/kitbit/gesturerecognition/RecResult"));
    Callback = (jclass) env->NewGlobalRef(env->FindClass(
            "com/gotokeep/keep/kitbit/gesturerecognition/GestureRecognitionUtils$Callback"));
    Point = (jclass)env->NewGlobalRef(env->FindClass("com/gotokeep/keep/kitbit/gesturerecognition/Point"));
}

void ClassHolder::release(JNIEnv *env) {
    env->DeleteGlobalRef(RecResult);
    env->DeleteGlobalRef(Callback);
    env->DeleteGlobalRef(Point);
}