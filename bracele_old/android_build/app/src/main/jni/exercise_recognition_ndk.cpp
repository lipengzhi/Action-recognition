#include <jni.h>
#include <string>
#include "utils.hpp"
#include "../../../../../smart_bracelet/callback.hpp"
#include "../../../../../smart_bracelet/exercise_recognition_api.hpp"
#include "classholder.hpp"

static ExerciseRecognitionAPI api;

static JavaVM *globalJavaVm;
static jobject globalCallback;

// ******************** 非 JNI 方法 ***************************

/**
 * 释放 callback 引用
 * 应用场景
 * 1App主动释放
 * 2JNI_OnUnload
 * 3设置一个空callback
 * @param env
 */
void remove_callback(JNIEnv *env) {
    env->DeleteGlobalRef(globalCallback);
    globalCallback = NULL;
}

/**
 * 回调方法的实现
 * @param recResult
 */
void sendGestureRecognitionResult(RecResult recResult) {
    if (globalCallback == NULL) {
        return;
    }
    JNIEnv *env = NULL;
    globalJavaVm->AttachCurrentThread(&env, NULL);
    jobject result = recResult2jobject(env, recResult);
    jmethodID mid = env->GetMethodID(ClassHolder::Callback, "onResult",
                                     "(Lcom/gotokeep/keep/kitbit/gesturerecognition/RecResult;)V");
    env->CallVoidMethod(globalCallback, mid, result);
    globalJavaVm->DetachCurrentThread();
}


// **************************** JNI 方法 ********************************
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    result = JNI_VERSION_1_6;
    /**
     * 初始化
     */
    globalJavaVm = vm;
    ClassHolder::init(env);
    return result;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    vm->AttachCurrentThread(&env, NULL);
    ClassHolder::release(env);
    if (globalCallback != NULL) {
        remove_callback(env);
    }
    vm->DetachCurrentThread();
    globalJavaVm = NULL;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_gotokeep_keep_kitbit_gesturerecognition_GestureRecognitionUtils_setTemplate(JNIEnv *env,
                                                                                     jclass type,
                                                                                     jstring tmpFilePath_) {
    const char *filePath = env->GetStringUTFChars(tmpFilePath_, 0);

    // 设置模板
    int result = api.setTemplate(filePath);

    //释放内存
    env->ReleaseStringUTFChars(tmpFilePath_, filePath);
    return result;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gotokeep_keep_kitbit_gesturerecognition_GestureRecognitionUtils_setCallback(JNIEnv *env,
                                                                                     jclass type,
                                                                                     jobject callback_) {
    if (callback_ == NULL && globalCallback != NULL) {
        remove_callback(env);
        return;
    }
    globalCallback = env->NewGlobalRef(callback_);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gotokeep_keep_kitbit_gesturerecognition_GestureRecognitionUtils_removeCallback(JNIEnv *env,
                                                                                        jclass type) {
    if (globalCallback != NULL) {
        remove_callback(env);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gotokeep_keep_kitbit_gesturerecognition_GestureRecognitionUtils_detecting(
        JNIEnv *env, jclass type, jobjectArray newPoints_) {
    // 类型转换
    if (newPoints_ == NULL) {
        return;
    }
    std::vector<TrackPoint> newPoints = points2vector(env, newPoints_);

    api.addData(newPoints);
}
