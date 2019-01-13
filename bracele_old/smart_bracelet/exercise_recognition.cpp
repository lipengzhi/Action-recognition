//
//  exercise_recognition.cpp
//  smart_bracelet
//
//  Created by bo guo on 2018/4/27.
//  Copyright © 2018 bo guo. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <numeric>
#include "exercise_recognition.hpp"
#include "pca.hpp"

using namespace std;

ExerciseRecognition::ExerciseRecognition(){
    m_act_max_cnts = 0;
    m_act_min_cnts = 0;
}

RecStatus ExerciseRecognition::loadTemplateData(string file_path, const bool flag, SampleTemplate &template_data){
    RecStatus status = Success;
    ifstream in;
    string line, field;
    in.open(file_path);
    int row = 0;
    template_data.action.clear();
    template_data.action_u.clear();
    if(in.is_open())
    {
        getline(in, line);    //give up first line.(header info)
        while(getline(in, line))
        {
            row++;
            istringstream stream(line);
            vector<double> data;
            while(getline(stream, field, ','))
            {
                double tmp = 0.0;
                istringstream iss(field);
                iss >> tmp;
                data.push_back(tmp);
            }
            TrackPoint pt(0, 0, 0);
            pt.x = 0;
            pt.y = 0;
            pt.z = 0;
            pt.acc_dist = 0;
            template_data.action.push_back(pt);
            pt.x = data.at(0);
            pt.y = data.at(1);
            pt.z = data.at(2);
            pt.acc_dist = 0;
            template_data.action_u.push_back(pt);
            if (1 == row && flag == true) {
                template_data.act_min_thv = data.at(3);
                template_data.act_max_thv = data.at(4);
                template_data.act_min_scale = data.at(5);
                template_data.act_max_scale = data.at(6);
                template_data.act_valid_gap = data.at(7);
                template_data.act_dist_sum = data.at(8);
                template_data.dtw_thv = data.at(9);
                
                m_act_min_cnts = data.at(3);
                m_act_max_cnts = data.at(4);
                m_act_min_acc_dist = data.at(5) * data.at(8);
                m_act_max_acc_dist = data.at(6) * data.at(8);
                m_act_valid_gap = data.at(7);
            }
        }
    }
    else
    {
        status = Failure;
    }
    return status;
}

int ExerciseRecognition::preProcessing(vector<TrackPoint> &tracks)
{
    int rs_flag = 0;
    //rotation to zero.
    vector<TrackPoint> vecPt;
    vecPt.clear();
    vecPt.assign(tracks.begin(), tracks.end());
    rs_flag = PCA::pca(vecPt);
    
    //z-scale.
    zScale(vecPt);
    tracks.assign(vecPt.begin(), vecPt.end());
    return rs_flag;
}

void ExerciseRecognition::zScale(vector<TrackPoint> &data){
    vector<double> x;
    vector<double> y;
    vector<double> z;
    vector<TrackPoint>::iterator it = data.begin();
    for(; it != data.end(); ++it)
    {
        x.push_back((*it).x);
        y.push_back((*it).y);
        z.push_back((*it).z);
    }
    
    double sum = accumulate(begin(x), end(x), 0.0);
    double mean_x = sum / x.size();
    double accum = 0.0;
//    for_each(begin(x), end(x), [&](const double dx){
    for(int i = 0; i < x.size(); i++){
        accum += (x.at(i) - mean_x) * (x.at(i) - mean_x);
    };
    double stdev_x = sqrt(accum / (x.size() - 1));
    
    sum = accumulate(begin(y), end(y), 0.0);
    double mean_y = sum / y.size();
    accum = 0.0;
//    for_each(begin(y), end(y), [&](const double dy){
    for(int i = 0; i < y.size(); i++){
        accum += (y.at(i) - mean_y) * (y.at(i) - mean_y);
    };
    double stdev_y = sqrt(accum / (y.size() - 1));
    
    sum = accumulate(begin(z), end(z), 0.0);
    double mean_z = sum / z.size();
    accum = 0.0;
//    for_each(begin(z), end(z), [&](const double dz){
    for(int i = 0; i < z.size(); i++){
        accum += (z.at(i) - mean_z) * (z.at(i) - mean_z);
    };
    double stdev_z = sqrt(accum / (z.size() - 1));
    
    if (stdev_x > 0.0001) {
        for(unsigned int i = 0; i < x.size(); ++i){
            data.at(i).x = (x.at(i) - mean_x) / stdev_x;
        }
    }
    else{
        for(unsigned int i = 0; i < x.size(); ++i){
            data.at(i).x = 0;
        }
    }
    if (stdev_y > 0.0001) {
        for(unsigned int i = 0; i < y.size(); ++i){
            data.at(i).y = (y.at(i) - mean_y) / stdev_x;
        }
    }
    else{
        for(unsigned int i = 0; i < y.size(); ++i){
            data.at(i).y = 0;
        }
    }
    if (stdev_z > 0.0001) {
        for(unsigned int i = 0; i < z.size(); ++i){
            data.at(i).z = (z.at(i) - mean_z) / stdev_x;
        }
    }
    else{
        for(unsigned int i = 0; i < z.size(); ++i){
            data.at(i).z = 0;
        }
    }
//    tracks.assign(data.begin(), vecPt.end());
}

void ExerciseRecognition::resample(vector<TrackPoint> src, const unsigned int sample_cnts, vector<TrackPoint> &det)
{
    unsigned int interval_len = (unsigned int)(sample_cnts - 1);
    if (interval_len < 1)
    {
        cout << "src.size < 1." <<endl;
    }
    double interval = (src.back().acc_dist - src.front().acc_dist) / (sample_cnts - 1);
    double D = 0.0;
    //vector<TrackPoint> tmp_Pts(src.begin(), src.begin() + src.size());
    
    //--- Store first TrackPoint since we'll never resample it out of existence.
    vector<TrackPoint> rst;
    rst.clear();
    det.clear();
    rst.push_back(src.front());
    for (unsigned int i = 1; i < src.size(); i++) {
        TrackPoint currentPt = src.at(i);
        TrackPoint previousPt = src.at(i - 1);
        double d = currentPt.acc_dist - previousPt.acc_dist;
        if ((D + d) > interval) {
            double qx = previousPt.x + ((interval - D) / d) * (currentPt.x - previousPt.x);
            double qy = previousPt.y + ((interval - D) / d) * (currentPt.y - previousPt.y);
            double qz = previousPt.z + ((interval - D) / d) * (currentPt.z - previousPt.z);
            TrackPoint pt(qx, qy, qz);
            pt.acc_dist = sqrt((qx - previousPt.x)*(qx - previousPt.x) + (qy - previousPt.y)*(qy - previousPt.y) + (qz - previousPt.z)*(qz - previousPt.z)) + previousPt.acc_dist;
            rst.push_back(pt);
            src.insert(src.begin() + i, pt);
            D = 0.0;
        }
        else
        {
            D += d;
        }
    }
    
    //sometimes we fall a rounding-error short of adding the last TrackPoint, so add it if so.
    if(rst.size() == (sample_cnts - 1))
    {
        rst.push_back(src.back());
    }
    det.assign(rst.begin(), rst.end());
}

void ExerciseRecognition::get_max_min_v(const std::vector<TrackPoint> &condidate_data, const int flag, double &max_amplitude) {
    double max_v = -1;
    double min_v = 1;
    max_amplitude = 0;
    switch(flag){
        case 0:    //X axis
            for (int i = 0; i < condidate_data.size(); ++i) {
                if(condidate_data.at(i).x > max_v){
                    max_v = condidate_data.at(i).x;
                }
                if(condidate_data.at(i).x < min_v){
                    min_v = condidate_data.at(i).x;
                }
            }
            break;
        case 1:    //Y axis
            for (int i = 0; i < condidate_data.size(); ++i) {
                if(condidate_data.at(i).y > max_v){
                    max_v = condidate_data.at(i).y;
                }
                if(condidate_data.at(i).y < min_v){
                    min_v = condidate_data.at(i).y;
                }
            }
            break;
        case 2:    //Z axis
            for (int i = 0; i < condidate_data.size(); ++i) {
                if (condidate_data.at(i).z > max_v) {
                    max_v = condidate_data.at(i).z;
                }
                if (condidate_data.at(i).z < min_v) {
                    min_v = condidate_data.at(i).z;
                }
            }
            break;
        default:
            break;
    }
    max_amplitude = max_v - min_v;
}

RecStatus ExerciseRecognition::adjust_results(const RecResult &result, const vector<TrackPoint> &user_data, double &template_dist_thv,
                    const SampleTemplate &action_template, unsigned int &start_idx, vector<RecResult> &rst){
    RecStatus status = Success;
    const double c_dtw = action_template.dtw_thv;
    const double c_dtw_thv = 5 * c_dtw;
    double k = (action_template.action_u.size() / 100);
    if (k < 2) {
        k = 2;
    }
    if (template_dist_thv < c_dtw_thv) {
        rst.push_back(result);
        
        //find the best match path and update start_idx.
        double user_acc_dist = rst.back().g_scale * action_template.act_dist_sum;
        double best_dist_min = user_acc_dist * (1 - DISTANCE_DELTA);
        double best_dist_max = user_acc_dist * (1 + DISTANCE_DELTA);
        double optimal_offset = user_acc_dist * BEST_OFFSET_RATE;
        vector<unsigned int> opt_start_idxs;
        opt_start_idxs.clear();
        double st = user_data.at(rst.back().start_idx).acc_dist;
        double ed = 0;
        opt_start_idxs.push_back(rst.back().start_idx);
        for (unsigned int i = rst.back().start_idx + 1; i < user_data.size(); i++) {
            ed = user_data.at(i).acc_dist;
            if (ed - st > (k * action_template.act_valid_gap) && ed - user_data.at(rst.back().start_idx).acc_dist < optimal_offset) {
                opt_start_idxs.push_back(i);
                st = ed;
            }
            else if ((ed - user_data.at(rst.back().start_idx).acc_dist) >= optimal_offset){
                break;
            }
            else{
                continue;
            }
        }
        
        vector<unsigned int> opt_st_idxs;
        opt_st_idxs.clear();
        vector<unsigned int> opt_ed_idxs;
        opt_ed_idxs.clear();
        for (unsigned int i = 0; i < opt_start_idxs.size(); i++) {
            unsigned int ls_idx = opt_start_idxs.at(i) + m_act_min_cnts;
            if (ls_idx > (unsigned int)user_data.size() - 1) {
                break;
            }
            unsigned int ld_idx = opt_start_idxs.at(i) + m_act_max_cnts;
            if (ld_idx > (unsigned int)user_data.size() - 1) {
                ld_idx = (unsigned int)user_data.size() - 1;
            }
            double thv = 0;
            for (unsigned int j = ls_idx; j <= ld_idx; j++) {
                double tmp_dist = user_data.at(j).acc_dist - user_data.at(opt_start_idxs.at(i)).acc_dist;
                if (tmp_dist > best_dist_min && tmp_dist < best_dist_max && user_data.at(j).acc_dist - thv > (k * action_template.act_valid_gap)) {
                    opt_st_idxs.push_back(opt_start_idxs.at(i));
                    opt_ed_idxs.push_back(j);
                    thv = user_data.at(j).acc_dist;
                }
            }
        }
        
        vector<TrackPoint> act_src;   //org action
        vector<TrackPoint> act_dst;   //resample action
        for (unsigned int i = 0; i < opt_st_idxs.size(); i++) {
            //resample
            act_src.clear();
            act_dst.clear();
            for (unsigned int j = opt_st_idxs.at(i); j < opt_ed_idxs.at(i); j++) {
                act_src.push_back(user_data.at(j));
            }
            resample(act_src, (unsigned int)action_template.action_u.size(), act_dst);

            //rotation & z-scale.
            int rs_flag = preProcessing(act_dst);
            if(rs_flag == -1){
                continue;
            }

            //calculate distance fastDTW.
            VectorDTW dtw1((uint)action_template.action_u.size(), 10.0);
            vector<TrackPoint> tem(action_template.action_u.begin(), action_template.action_u.begin() + action_template.action_u.size());
            double similarity = dtw1.fastdynamic(tem, act_dst) / action_template.action_u.size();
            if (similarity < template_dist_thv) {
                RecResult tmp;
                template_dist_thv = similarity;
                tmp.start_idx = opt_st_idxs.at(i);
                tmp.end_idx = opt_ed_idxs.at(i);
                tmp.similarity = similarity;
                tmp.g_scale = (user_data.at(result.end_idx).acc_dist - user_data.at(result.start_idx).acc_dist) / action_template.act_dist_sum;
                rst.back() = tmp;
            }
        }
        
        //update result.score.
        if (rst.back().similarity < c_dtw) {
            rst.back().score = 1;    //perfect
        }
        else if(rst.back().similarity < 1.5 * c_dtw){
            rst.back().score = 0.9;    //good
        }
        else if(rst.back().similarity < 2 * c_dtw){
            rst.back().score = 0.8;    //good
        }
        else if(rst.back().similarity < 2.5 * c_dtw){
            rst.back().score = 0.7;    //good
        }
        else if(rst.back().similarity >= 2.5 * c_dtw){
            rst.back().score = 0.6;    //poor
        }
        rst.back().similarity = rst.back().score;
        
        start_idx = rst.back().end_idx - (unsigned int)((rst.back().end_idx - rst.back().start_idx) * DISTANCE_DELTA * 0.5);
        if (start_idx > user_data.size() - 1) {
            start_idx = (unsigned int)user_data.size() - 1;
        }
        m_act_min_cnts = (m_act_min_cnts * 0.3 + (rst.back().end_idx - rst.back().start_idx + 1) * 0.5 * 0.7);
        m_act_max_cnts = (m_act_max_cnts * 0.3 + (rst.back().end_idx - rst.back().start_idx + 1) * 1.5 * 0.7);
        double sum_dist = user_data.at(rst.back().end_idx).acc_dist - user_data.at(rst.back().start_idx).acc_dist;
        m_act_min_acc_dist = (m_act_min_acc_dist * 0.2 + sum_dist * 0.9 * 0.8);
        m_act_max_acc_dist = (m_act_max_acc_dist * 0.2 + sum_dist * 1.1 * 0.8);
        m_act_valid_gap = sum_dist / (rst.back().end_idx - rst.back().start_idx + 1);
        rst.back().act_count = (unsigned int)rst.size();
    }
    else{
        //update start_idx.
        for (unsigned long i = start_idx + 1; i < user_data.size(); i++) {
            double tmp = user_data.at(i).acc_dist - user_data.at(start_idx).acc_dist;
            double thv = m_act_valid_gap;
            double tmp_next = 0;
            if(i < (user_data.size()-1)){
                tmp_next = user_data.at(i+1).acc_dist - user_data.at(i).acc_dist;
            }
            
            if (tmp > thv && tmp_next > thv) {
                start_idx = (unsigned int)i;
                break;
            }
        }
    }
    return status;
}

/*********************************************************************
 This is algorithm API, you could free to call it.
 Input:
         action_templates: (xm,ym,zm) && (m < n), if an action contain multiple templates, please add them to vectors;
         tracks:len =n user training trajectory, singal infinite series;
 Output:
        start_idx: inital value is 0, and then it will been auto update, you could delete 0~start_idx value to avoid stack overflow.
        rst: a struction of result.
 Return:
        function execution status.
 Algorithm steps:
        1. resample
        2. rotation
        3. z-scale
        4. calculate distance(DTW)
        5. output score
 *********************************************************************/
RecStatus ExerciseRecognition::DetectAction(const vector<TrackPoint> &user_data, const SampleTemplate &action_template, unsigned int &start_idx, vector<RecResult> &rst)
{
    RecStatus status = Success;
    if ((user_data.back().acc_dist - user_data.at(start_idx).acc_dist) < m_act_min_acc_dist || (user_data.size() - start_idx) < m_act_min_cnts) {
        return status;
    }

    //generate end_idxs.
    unsigned int ls_idx = start_idx + m_act_min_cnts;
    if (ls_idx > (unsigned int)user_data.size() - 1) {
        return status;
    }
    unsigned int ld_idx = start_idx + m_act_max_cnts;
    double tmp_dist = user_data.back().acc_dist - user_data.at(start_idx).acc_dist;
    if ((ld_idx > (unsigned int)user_data.size() - 1) && (tmp_dist < m_act_max_acc_dist)) {
        return status;
    }
    else if(ld_idx > (unsigned int)user_data.size() - 1) {
        ld_idx = (unsigned int)user_data.size() - 1;
    }

    double scale_thv = m_act_min_acc_dist;
    vector<unsigned int> end_idxs;
    end_idxs.clear();
    for (unsigned int i = ls_idx; i <= ld_idx; i++) {
        double test_acc_dist = user_data.at(i).acc_dist - user_data.at(start_idx).acc_dist;
        if (test_acc_dist < scale_thv) {
            continue;
        }
        else if (test_acc_dist >= scale_thv && test_acc_dist < m_act_max_acc_dist){
            end_idxs.push_back(i);
            scale_thv = test_acc_dist + OFFSET_DELTA * action_template.act_dist_sum;
        }
        else if (test_acc_dist >= m_act_max_acc_dist){
            break;
        }
    }
    
    //find match path.
    vector<TrackPoint> act_src;   //org action
    vector<TrackPoint> act_dst;   //resample action
    vector<TrackPoint> act_sudu;
    vector<TrackPoint> react_sudu;
    vector<double> sudu_modle;
    vector<TrackPoint> sudu_po;
    const double c_dtw = action_template.dtw_thv;
    const double c_dtw_thv = 5 * c_dtw;
    double template_dist_thv = c_dtw_thv;
    RecResult result;
    vector<RecResult> arr_rst;
    arr_rst.clear();
    for (unsigned int i = 0; i < end_idxs.size(); i++) {
        //resample
        act_src.clear();
        act_dst.clear();
        for (unsigned int j = start_idx; j < end_idxs.at(i); j++) {
            act_src.push_back(user_data.at(j));
        }
//        double max_amplitude = 0;
//        get_max_min_v(act_src, 2, max_amplitude);
//        if(max_amplitude < 0.7){
//            result.start_idx = 0;
//            result.end_idx = 0;
//            result.similarity = 999;
//            result.g_scale = 0;
//            arr_rst.push_back(result);
//            continue;
//        }


        
//速度相似度

        string data_path2 = "/Users/lipengzhi/Desktop/braceletgesturerecognition/tracking/109_template_501.csv";
        //            read_crown(data_path1);
        
        
        ifstream myfile2(data_path2);
        
        if (!myfile2.is_open()){
            cout << "Unable to open myfile2";
            system("pause");
            exit(1);
            
        }
        
        vector<string> vec2;
        string temp2;
        
        while (getline(myfile2,temp2))                    //利用getline（）读取每一行，并按照行为单位放入到vector
        {
            
            vec2.push_back(temp2);
        }
        cout << "读入的模板数据为 " << endl;
        for (int i = 1; i <vec2.size();i++)
        {
            cout <<i<< endl;
            cout <<vec2[i]<< endl;
            vector<string> data;
            string tmp;
            istringstream input(vec2[i]);
            while (getline(input, tmp, ','))
            { data.push_back(tmp);
            }
            for (int i =0; i< data.size();i++) {
                cout<<"www"<<data[i]<<endl;
                
            }
            for (int j=0;j<data.size();j++) {
                if (j == 10 ) {
                    string aa = data[j];
                    if(aa == "\r") break;
                    double r = stod(aa.c_str());
                    sudu_modle.push_back(r);
                }
            }
        }
        
        for (int i =0; i< sudu_modle.size();i++) {
            TrackPoint pe(sudu_modle[i], 0, 0);
            sudu_po.push_back(pe);
            
        }

        
        for(int i= 0;i<act_src.size();i++){
            TrackPoint currentPt = act_src.at(i+1);
            TrackPoint previousPt = act_src.at(i);
            
            double x = currentPt.x - previousPt.x;
            double y = currentPt.y - previousPt.y;
            double z = currentPt.z - previousPt.z;
            double lens = sqrt(x*x + y*y + z*z);
            TrackPoint pt(lens, 0, 0);
            act_sudu.push_back(pt);
        }
        
        resample(act_sudu, (uint)sudu_modle.size(), react_sudu);
        
        //calculate distance fastDTW.
        VectorDTW dtw2((uint)sudu_modle.size(), 10);
        double dist = dtw2.fastdynamic(act_sudu, sudu_po);
        
        cout << "Distance: " << dist << endl;
        
        
        
        
        
//轨迹的相似度
        resample(act_src, (unsigned int)action_template.action_u.size(), act_dst);
        
        int rs_flag = preProcessing(act_dst);
        if(rs_flag == -1){
            result.start_idx = 0;
            result.end_idx = 0;
            result.similarity = 999;
            result.g_scale = 0;
            arr_rst.push_back(result);
            continue;
        }

        //calculate distance fastDTW.
        VectorDTW dtw1((uint)action_template.action_u.size(), 10);
        vector<TrackPoint> tem(action_template.action_u.begin(), action_template.action_u.begin() + action_template.action_u.size());
        double similarity = dtw1.fastdynamic(tem, act_dst) / action_template.action_u.size();
        cout <<"similarity return is:"<<similarity<<endl;
//        template_dist_thv = similarity;
//        result.start_idx = start_idx;
//        result.end_idx = end_idxs.at(i);
//        result.similarity = similarity;
//        result.g_scale = (user_data.at(result.end_idx).acc_dist - user_data.at(result.start_idx).acc_dist) / action_template.act_dist_sum;
//        arr_rst.push_back(result);
    }
//    for (unsigned int i = 0; i < end_idxs.size(); i++) {
//        if (arr_rst.at(i).similarity < template_dist_thv) {
//            template_dist_thv = arr_rst.at(i).similarity;
//            result = arr_rst.at(i);
//        }
//    }
//
//    adjust_results(result, user_data, template_dist_thv, action_template, start_idx, rst);
     return status;
}
