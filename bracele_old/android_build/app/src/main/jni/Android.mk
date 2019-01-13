LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_LDLIBS := -lm -llog
LOCAL_MODULE := kitbit-gesture-recognition
LOCAL_SRC_FILES := ${LOCAL_PATH}/../../../../../smart_bracelet/exercise_recognition_api.cpp \
                   ${LOCAL_PATH}/../../../../../smart_bracelet/exercise_recognition.cpp \
                   ${LOCAL_PATH}/exercise_recognition_ndk.cpp \
                   ${LOCAL_PATH}/utils.cpp \
                   ${LOCAL_PATH}/classholder.cpp

include $(BUILD_SHARED_LIBRARY)