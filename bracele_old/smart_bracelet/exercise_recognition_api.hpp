//
//  exercise_recognition_api.hpp
//  TrajectoryMatch
//
//  Created by bo guo on 2018/7/17.
//  Copyright © 2018 bo guo. All rights reserved.
//

#ifndef exercise_recognition_api_hpp
#define exercise_recognition_api_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include "exercise_recognition.hpp"

typedef struct{
    std::string version;
    bool hand_choice;
}BraceletStatus, *pBraceletStatus;

class ExerciseRecognitionAPI{
    
private:
    unsigned int m_start_idx;
    unsigned int m_pre_idx;
    BraceletStatus m_bs;
    SampleTemplate m_action_template;
    std::vector<TrackPoint> m_user_data;
    ExerciseRecognition m_act;

    bool is_detecting;
    bool is_load_template_success;
    std::vector<RecResult> recResult;
    int nextResultIndex;

    void clearResult();
    
    /*********************************************************************
     This is algorithm API
     Output:
     rst: a struction of result.
     Return:
     function execution status.
     *********************************************************************/
    void detecting();

public:
    ExerciseRecognitionAPI();

    ~ExerciseRecognitionAPI();

    //init bracelet version & hand_chioce.
    inline RecStatus initStatus(const BraceletStatus bra_status){
        RecStatus status = Success;
        m_bs = bra_status;
        return status;
    }

    //init a class templates.
    RecStatus setTemplate(const std::string file_path);

    /*******************************************************
     * 外部调用接口
     * 唤醒算法，去计算
     * 一般情况，无需手动唤醒，只需给算法丢数据，算法主动唤醒
     * 特殊情况，也可手动唤醒
     */
    void awake();

    /*******************************************************
     * 外部调用接口
     * 将新数据丢给算法
     * @param new_data ：新数据构成的容器
     */
    void addData(std::vector<TrackPoint> &new_data);

};

#endif /* exercise_recognition_api_hpp */
