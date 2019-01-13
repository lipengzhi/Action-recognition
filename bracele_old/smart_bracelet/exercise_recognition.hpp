//
//  exercise_recognition.hpp
//  smart_bracelet
//
//  Created by bo guo on 2018/4/27.
//  Copyright © 2018 bo guo. All rights reserved.
//

#ifndef exercise_recognition_hpp
#define exercise_recognition_hpp

#include <vector>
#include <string>
#include "vectordtw.hpp"

#define DIMENSION 3
#define SAMPLE_HZ 50
#define MIN_G_SCALE_RATE 0.9     
#define MAX_G_SCALE_RATE 1.1
#define MIN_COUNTS_RATE 0.7
#define MAX_COUNTS_RATE 1.3
#define OFFSET_DELTA 0.05
#define DISTANCE_DELTA 0.1     //0.1
#define BEST_OFFSET_RATE 0.1    //0.2

#define TRACE
//#define TRACE (printf("%s(%d)-<%s>: ",__FILE__, __LINE__, __FUNCTION__), printf)

enum RecStatus{
    Success = 0,
    Failure,
    Aborted,
};

typedef struct{
    std::vector<TrackPoint> action;    //template that has been resample, but has not been rotated and z-scale.
    std::vector<TrackPoint> action_u;  //template has benn rotated and z-scaled.
    unsigned int act_min_thv;    //min nums that based on time axis.
    unsigned int act_max_thv;    //max nums that based on time axis.
    double act_min_scale;    //the space min scale rate.
    double act_max_scale;    //the space max scale rate.
    double act_valid_gap;
    double act_dist_sum;
    double dtw_thv;
    
}SampleTemplate, *pSampleTemplate;

typedef struct{
    int start_idx;
    int end_idx;
    int act_count;//动作计次
    double similarity;    //similarity distance
    double score;
    double g_scale;
}RecResult, *pRecResult;

class ExerciseRecognition{

public:
    
public:
    ExerciseRecognition();
//    ~ExerciseRecognition();
    RecStatus loadTemplateData(std::string file_path,  const bool flag, SampleTemplate &template_data);
    int preProcessing(std::vector<TrackPoint> &tracks);
    void resample(std::vector<TrackPoint> src, const unsigned int sample_cnts, std::vector<TrackPoint> &det);
    void zScale(std::vector<TrackPoint> &data);
    RecStatus DetectAction(const std::vector<TrackPoint> &user_data, const SampleTemplate &action_template, unsigned int &start_idx, std::vector<RecResult> &rst);
    
private:
    RecStatus adjust_results(const RecResult &result, const std::vector<TrackPoint> &user_data, double &template_dist_thv,
                        const SampleTemplate &action_template, unsigned int &start_idx, std::vector<RecResult> &rst);
    void get_max_min_v(const std::vector<TrackPoint> &condidate_data, const int flag, double &max_amplitude);
    
private:
    unsigned int m_act_min_cnts;
    unsigned int m_act_max_cnts;
    double m_act_min_acc_dist;
    double m_act_max_acc_dist;
    double m_act_valid_gap;
protected:
    
};

#endif /* exercise_recognition_hpp */
