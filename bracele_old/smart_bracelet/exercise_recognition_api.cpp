//
//  exercise_recognition_api.cpp
//  TrajectoryMatch
//
//  Created by bo guo on 2018/7/17.
//  Copyright © 2018 bo guo. All rights reserved.
//

#include <thread>
#include "exercise_recognition_api.hpp"
#include "exercise_recognition.hpp"
#include "callback.hpp"

ExerciseRecognitionAPI::ExerciseRecognitionAPI() {
    m_start_idx = 0;
    m_pre_idx = 9999;
    m_user_data.clear();
    is_detecting = false;
    is_load_template_success = false;
    nextResultIndex = 0;
}

ExerciseRecognitionAPI::~ExerciseRecognitionAPI() {

}

RecStatus ExerciseRecognitionAPI::setTemplate(const std::string file_path) {
    is_load_template_success = false;
    RecStatus status = m_act.loadTemplateData(file_path, true, m_action_template);
    if(status == Success){
        if(!is_detecting){
            clearResult();
        }
        is_load_template_success = true;
    }
    return status;
}

void ExerciseRecognitionAPI::detecting() {
    m_pre_idx = -1;
    /**
     * TODO：这里会有一些问题：假如正在检测动作，且数据中还有一些当前动作未检测的数据。
     *  此时切换动作模板，就会停止检测，上个动作最后那段数据就会丢失。假设那段数据中有一个完整的动作，就会造成动作丢失
     */
    while (is_load_template_success) {
        if (m_pre_idx == m_start_idx) {
            // 上一轮未检测出动作，且不再进行检测，退出
            is_detecting = false;
            return;
        }
        m_pre_idx = m_start_idx;
        RecStatus status = m_act.DetectAction(m_user_data, m_action_template, m_start_idx, recResult);

        if(status == Success && recResult.size() > nextResultIndex){
            // 通知检测结果
            RecResult result = recResult.at(nextResultIndex++);
            sendGestureRecognitionResult(result);
        }
    }
    // 由于切换动作，停止本次检测，新动作计次从0开始，需要清空结果集
    clearResult();
    is_detecting = false;
}

void ExerciseRecognitionAPI::awake() {
    if (is_load_template_success && !is_detecting) {
        // 执行检测线程
        is_detecting = true;
        std::thread t{&ExerciseRecognitionAPI::detecting, this};
        t.detach();
    }
}

void ExerciseRecognitionAPI::addData(std::vector<TrackPoint> &new_data) {
    if (new_data.size() <= 0) {
        return;
    }
    TrackPoint pre_pt(0, 0, 0);
    double distance = 0;
    if (m_user_data.size() <= 0) {
        pre_pt = new_data.at(0);
    } else {
        pre_pt = m_user_data.back();
        distance = m_user_data.back().acc_dist;
    }
    for (int i = 0; i < new_data.size(); i++) {
        distance += new_data.at(i).euclid_distance(pre_pt);
        new_data.at(i).acc_dist = distance;
        pre_pt = new_data.at(i);
    }
    m_user_data.insert(m_user_data.end(), new_data.begin(), new_data.end());
    awake();
}

void ExerciseRecognitionAPI::clearResult() {
    nextResultIndex = 0;
    recResult.clear();
}
