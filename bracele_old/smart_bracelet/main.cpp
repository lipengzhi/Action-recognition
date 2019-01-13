//
//  main.cpp
//  smart_bracelet
//
//  Created by bo guo on 2018/4/27.
//  Copyright © 2018 bo guo. All rights reserved.
//

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "exercise_recognition.hpp"
#include "pca.hpp"
#include "vectordtw_02.hpp"

#define MIN_INIT_DIST_RATE 0.8
#define MAX_INIT_DIST_RATE 1.2
#define MIN_INIT_TIME_RATE 0.5
#define MAX_INIT_TIME_RATE 1.5
#define ACTION_CNT 15
#define ACTION_FILE_CNT 1

using namespace std;
using namespace chrono;

typedef struct {
    int st;
    int et;
}labelIdx;

vector<string> getFilesName(string cate_dir){
    vector<string> files;
    DIR *dir;
    struct dirent *ptr;
    if ((dir = opendir(cate_dir.c_str())) == NULL) {
        perror("Open dir error...");
        exit(1);
    }
    while ((ptr=readdir(dir)) != NULL) {
        if (strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0 || strstr(ptr->d_name,".csv")==NULL) {
            continue;
        }
        else if(ptr->d_type == 8){
            string tmp = cate_dir + ptr->d_name;
            files.push_back(tmp);
        }
    }
    closedir(dir);
    return files;
}

void extractDataFromFile(const string file_path, const labelIdx idx, const int col_idx, const char separator, vector<TrackPoint >&origin_data){
    ifstream in;
    string line, field;
    in.open(file_path);
    int row = 0;
    double distance = 0.0;
    TrackPoint pre_pt(0, 0, 0);
    if(in.is_open())
    {
        while(getline(in, line))
        {
            row++;
            if (row < idx.st) {
//                cout << "row:" << row<<endl;
                continue;
            }
            else if (row >= idx.st && row <= idx.et)
            {
                istringstream stream(line);
                vector<double> data;
                while(getline(stream, field, separator))
                {
                    double tmp = 0.0;
                    istringstream iss(field);
                    iss >> tmp;
                    data.push_back(tmp);
                }
                TrackPoint pt(0, 0, 0);
                pt.x = data.at(col_idx);
                pt.y = data.at(col_idx+1);
                pt.z = data.at(col_idx+2);
                if (row==idx.st){
                    pre_pt = pt;
                }
                distance += pt.euclid_distance(pre_pt);
                pt.acc_dist = distance;
                pre_pt = pt;
                origin_data.push_back(pt);
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        cout << " open file failed!" << endl;
    }
    in.close();
}

void loadUserData(const string file_path, const int s_row,  const int s_cloum, const char separator, vector<TrackPoint > &user_data)
{
    ifstream in;
    string line, field;
    int row = s_row;
    double distance = 0.0;
    TrackPoint pre_pt(0, 0, 0);
    in.open(file_path);
    if(in.is_open())
    {
        for (int i = 0; i < s_row; i++) {
            getline(in, line);    //give up first line.(header info)
        }
        while(getline(in, line))
        {
            row++;
            istringstream stream(line);
            vector<double> data;
            while(getline(stream, field, separator))
            {
                double tmp = 0.0;
                istringstream iss(field);
                iss >> tmp;
                data.push_back(tmp);
            }
            if (data.size() <= 0) {
                continue;
            }
            TrackPoint pt(0, 0, 0);
            pt.x = data.at(s_cloum);
            pt.y = data.at(s_cloum+1);
            pt.z = data.at(s_cloum+2);
            if (row==s_row+1){
                pre_pt = pt;
            }
            distance += pt.euclid_distance(pre_pt);
            pt.acc_dist = distance;
            pre_pt = pt;
            user_data.push_back(pt);
        }
    }
    else
    {
        cout << " open file failed!" << endl;
    }
}

void saveToCsv(string path_name, SampleTemplate &data){
    ofstream outFile;
    outFile.open(path_name, ios::out);
    outFile << "X,Y,Z,AccDist,zscale-x,zscale-y,zscale-z,min_n,max_n,min_r,max_r,dtw_thv"<<endl;
    outFile << std::fixed;
    outFile<<data.action[0].x<<","<<data.action[0].y<<","<<data.action[0].z<<","<<data.action[0].acc_dist<<","<<data.action_u[0].x<<","<<data.action_u[0].y<<","<<data.action_u[0].z <<","<< data.act_min_thv << "," << data.act_max_thv << "," << data.act_min_scale << "," << data.act_max_scale << "," << data.dtw_thv <<endl;
    for(int i=1;i<data.action.size();i++){
        outFile<<data.action[i].x<<","<<data.action[i].y<<","<<data.action[i].z<<","<<data.action[i].acc_dist<<","<<data.action_u[i].x<<","<<data.action_u[i].y<<","<<data.action_u[i].z<<endl;
    }
    outFile.close();
}

//process template origin data
void preProcessTemplateOriginData(vector<TrackPoint > &origin_data, SampleTemplate &template_data){
    //resample
    ExerciseRecognition pre_act;
    vector<TrackPoint>resampled_template;
    pre_act.resample(origin_data, (unsigned int)origin_data.size(), resampled_template);
    template_data.action.assign(resampled_template.begin(), resampled_template.end());
    PCA::pca(resampled_template);
    
    //z-scale
    vector<TrackPoint> vecPt;
    vecPt.clear();
    vecPt.assign(resampled_template.begin(), resampled_template.end());
    pre_act.zScale(vecPt);
    for(int i = 0;i<vecPt.size();i++)
        vecPt[i].acc_dist = 0;
    template_data.action_u.assign(vecPt.begin(), vecPt.end());
    template_data.act_min_scale=MIN_INIT_DIST_RATE;
    template_data.act_max_scale=MAX_INIT_DIST_RATE;
    template_data.act_min_thv=(unsigned int)(MIN_INIT_TIME_RATE * vecPt.size());
    template_data.act_max_thv= (unsigned int)(MAX_INIT_TIME_RATE * vecPt.size());
    template_data.dtw_thv = 0;
}

unsigned int generateMultiplesTemplateData(const vector<string> originDataList, const unsigned int start_idxs[][ACTION_CNT], const unsigned int end_idxs[][ACTION_CNT], const string templates_path){
    ExerciseRecognition pre_act;
    assert(originDataList.size() == ACTION_FILE_CNT);
    vector<vector<TrackPoint> >origin_data_arr;
    vector<TrackPoint> origin_data;
    unsigned int sample_cnt = 0;
    for(int i=0;i<originDataList.size();i++)
    {
        for (int j = 0; j < ACTION_CNT; j++) {
            origin_data.clear();
            labelIdx lab_info;
            lab_info.st = (int)start_idxs[i][j];
            lab_info.et = (int)end_idxs[i][j];
            sample_cnt += lab_info.et - lab_info.st + 1;
            extractDataFromFile(originDataList.at(i), lab_info, 0, '\t', origin_data);
            origin_data_arr.push_back(origin_data);
        }
    }
    
    //generate template data.
    for(int i=0;i<(ACTION_CNT*ACTION_FILE_CNT);i++)
    {
        string path_name = templates_path + to_string(i) + ".csv";
        SampleTemplate templateData;
        cout << "template i:" << i << " ."<<endl;
        preProcessTemplateOriginData(origin_data_arr.at(i), templateData);
        saveToCsv(path_name, templateData);
    }
    
    //calculate sample counts.
    sample_cnt = sample_cnt / (ACTION_CNT*ACTION_FILE_CNT);
    cout << "The standard sample count: " <<sample_cnt<< " ."<<endl;
    return sample_cnt;
}

RecStatus run(const vector<TrackPoint> tracks, const string standard_temp_path){
    
    //load action template.
    SampleTemplate action_template;
    ExerciseRecognition act;
    act.loadTemplateData(standard_temp_path, true, action_template);
    
    cout << "Begining search target curve..." << endl;
    RecStatus status = Success;
    
    //recognize. while will break, when main process inform it.
    unsigned int start_idx = 0;
    unsigned int pre_idx = 9999;
    vector<RecResult> rst;
    rst.clear();
    auto start_time = system_clock::now();
    int count = 0;
    while (pre_idx != start_idx) {
        pre_idx = start_idx;
        count++;
        status = act.DetectAction(tracks, action_template, start_idx, rst);
//        cout << "start_idx : " << start_idx <<endl;
    }
    
    cout <<" the count is:" << count<< endl;
    auto end_time = system_clock::now();
    auto duration = duration_cast<microseconds>(end_time - start_time);
    cout << "elapsed time: " << double(duration.count())*microseconds::period::num / microseconds::period::den << "s" <<endl;
    for (unsigned int i = 0; i < rst.size(); i++) {
        cout << " The " << i+1 << "th match is: " << endl;
        cout << "DTW_DIST: " << rst.at(i).similarity << ", score: " << rst.at(i).score << ", g_scal: " << rst.at(i).g_scale << "." <<endl;
    }
    cout << " start_idxs = [";
    for (unsigned int i = 0; i < rst.size(); i++) {
        cout << rst.at(i).start_idx << ", ";
    }
    cout << "]" << endl;
    cout << " end_idxs = [";
    for (unsigned int i = 0; i < rst.size(); i++) {
        cout << rst.at(i).end_idx << ", ";
    }
    cout << "]" << endl;
    cout << "DetectAction Done!" << endl;
    return status;
}


int read_crown(string data_path1){
    ifstream myfile(data_path1);
    
    if (!myfile.is_open()){
        cout << "Unable to open myfile";
        system("pause");
        exit(1);
        
    }
    
    vector<string> vec;
    string temp;
    string aa = temp + ',';
    
    
    while (getline(myfile, aa,','))                    //利用getline（）读取每一行，并按照行为单位放入到vector
    {
        vec.push_back(aa);
    }
    
    vector <float> radius;
    
    
    cout << "读入的数据为 " << endl;
    for (auto it = vec.begin(); it != vec.end(); it++)
    {
        cout << *it << endl;
        istringstream is(*it);                    //用每一行的数据初始化一个字符串输入流；
        string s;
        int pam = 0;
        char ch;
        
        while (is >> s)                          //以空格为界，把istringstream中数据取出放入到依次s中
        {
            is>>ch;
            if (pam == 2)                       //获取第六列的数据
                
            {
                float r = atof(s.c_str());     //做数据类型转换，将string类型转换成float
                radius.push_back(r);
                //cout << r << endl;
            }
            
            pam++;
            
        }
    }
    cout << "读入的第2列数据为 " << endl;
    
    for (auto it = radius.begin(); it != radius.end(); it++)
    {
        cout << *it << endl;
    }
    
    //cout << "successfully to open myfile";
    system("pause");
    return 1;
}




////Vector DTW code
//class TrackPoint
//{
//public:
//    double x, y, z;
//
//    Point(double X, double Y, double Z): x(X), y(Y), z(Z) { }
//
//    //computes the l1 distance with another point
//    double l1_distance(const Point &p)
//    {
//        return fabs(x - p.x) + fabs(y - p.y) + fabs(z - p.z);
//    }
//
//    //euclidean distance
//    double euclid_distance(const Point &p)
//    {
//        return sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y) + (z - p.z) * (z - p.z));
//    }
//};

class VectorDTW_02
{
private:
    vector<vector<double> > mGamma;
    int mN, mConstraint;
public:
    enum { INF = 100000000 }; //some big number
    
    static inline double min (double x, double y ) { return x < y ? x : y; }
    
    /**
     * n is the length of the time series
     *
     * constraint is the maximum warping distance.
     * Typically: constraint = n/10.
     * If you set constraint = n, things will be slower.
     *
     */
    VectorDTW_02(uint n, uint constraint) : mGamma(n, vector<double>(n, INF)), mN(n), mConstraint(constraint) { }
    
    /**
     * This currently uses euclidean distance. You can change it to whatever is needed for your application
     */
    inline double fastdynamic_02(vector<TrackPoint> &v, vector<TrackPoint> &w)
    {
        assert(static_cast<int>(v.size()) == mN);
        assert(static_cast<int>(w.size()) == mN);
        assert(static_cast<int>(mGamma.size()) == mN);
        double Best(INF);
        for (int i = 0; i < mN; ++i)
        {
            assert(static_cast<int>(mGamma[i].size()) == mN);
            for(int j = max(0, i - mConstraint); j < min(mN, i + mConstraint + 1); ++j)
            {
                Best = INF;
                if(i > 0)
                    Best = mGamma[i - 1][j];
                if(j > 0)
                    Best = min(Best, mGamma[i][j - 1]);
                if((i > 0) && (j > 0))
                    Best = min(Best, mGamma[i - 1][j - 1]);
                if((i == 0) && (j == 0))
                    mGamma[i][j] = v[i].euclid_distance(w[j]);
                else
                    mGamma[i][j] = Best + v[i].euclid_distance(w[j]);
            }
        }
        
        return mGamma[mN-1][mN-1];
    }
};






int main(int argc, const char * argv[])
{
    int select_number = 1;
    cout << "1: Generate multiple templates by label info." <<endl;
    cout << "2: Calculate multiple templates DTW with standart template to update template threshold." <<endl;
    cout << "3: Seaching target curve."<<endl;

    string common_path = "/Users/lipengzhi/Desktop/braceletgesturerecognition/tracking/";
    string data_path1;
    string standard_temp_path;
    int sta[15];
    int end[15];
    //传参数if分支
    if(argc>1)
    {
        ifstream in;
        string line, field;
        //接受label文件名
        string fa = argv[1];
        cout << "lable_name ："+ fa<< endl;
        in.open(fa);
        int row = 0;
        vector<int> data;
        if(in.is_open())
        {
            getline(in, line);    //give up first line.(header info)
            while(getline(in, line))
            {
                row++;
                istringstream stream(line);
                while(getline(stream, field, ','))
                {
                    int tmp = 0.0;
                    istringstream iss(field);
                    iss >> tmp;
                    data.push_back(tmp);
                }
            }
            int m =0;
            int n =0;
            for(int i=0; i<data.size(); i++){
                if(i%2 ==0){
                    sta[m] =data[i];
                    m++;
                }
                else{
                    end[n] =data[i];
                    n++;
                }
            }
        }
        else
        {
            cout << " open file failed!" << endl;
        }
        //定义if分支变量
        string aa = argv[2];
        select_number = std::stoi(aa);
        data_path1 =common_path+argv[3];
        cout << "data_path1 ："+ data_path1<< endl;
        string temtemplate_name = argv[4];
        standard_temp_path = common_path + temtemplate_name;

    }
    else{
        //不走传参数if分支定义变量
        select_number = 3;
        data_path1 = common_path + "109coord.txt";
        standard_temp_path = common_path + "109_template_50.csv";

    }
    const int k_hz = 50;//SAMPLE_HZ;
    string templates_path = common_path + "templates/";
    
    //3
    string users_path = "/Users/lipengzhi/Desktop/braceletgesturerecognition/tracking/109coord.txt";
    cout<< " Your choice are：" << select_number<<endl;
    switch (select_number){
        case 1: {
            vector<string> originDataList;
            originDataList.push_back(data_path1);
            if (access(templates_path.c_str(), 0) == -1) {
                cout << templates_path << " is not existing, now creat it."<<endl;
                int flag = mkdir(templates_path.c_str(), 0777);
                if (flag == 0) {
                    cout<<"creat successfully!"<<endl;
                }
                else{
                    cout<< "creat errorly!"<<endl;
                }
            }
//            unsigned int start_idxs[ACTION_FILE_CNT][ACTION_CNT] = {{677, 887, 1340, 1660, 1896, 2108, 2365, 2573, 3433, 3818, 4197, 4500, 4802, 5035, 5267},
//                                                                    {358, 749, 1171, 1471, 1783, 2066, 2351, 2677, 2970, 3276, 3616, 3826, 4044, 4240, 4568},
//                                                                    {177, 472, 632, 819, 1017, 1183, 1361, 1544, 1718, 1891, 2077, 2247, 2433, 2619, 2791}};
//            unsigned int end_idxs[ACTION_FILE_CNT][ACTION_CNT] = {{886, 1102, 1536, 1857, 2105, 2364, 2572, 2815, 3635, 4000, 4379, 4674, 4993, 5214, 5446},
//                                                                  {522, 916, 1338, 1641, 1966, 2230, 2528, 2852, 3130, 3452, 3775, 3991, 4239, 4457, 4726},
//                                                                  {362, 631, 818, 1016, 1182, 1360, 1543, 1717, 1890, 2076, 2246, 2432, 2618, 2790, 2985}};
            unsigned int start_idxs[ACTION_FILE_CNT][ACTION_CNT] = {{135, 351, 559, 769, 975, 1173, 1371, 1565, 1765, 1967, 2167, 2367, 2569, 2769, 2972}};
            unsigned int end_idxs[ACTION_FILE_CNT][ACTION_CNT] = {{325, 535, 744, 956, 1154, 1352, 1564, 1746, 1951, 2151, 2354, 2554, 2751, 2952, 3155}};
            
            //传参数if分支
            if (argc >1)
            {
                //循环给二维数组赋值
                for(int i = 0;i<1;i++)
                {
                    for(int j = 0; j < 15;j++)
                    {
                        int m = j;
                        start_idxs[i][j]= sta[m];
                    }
                }

                for(int l = 0;l<1;l++)
                {
                    for(int j = 0; j < 15;j++)
                    {
                        int m = j;
                        end_idxs[l][j]=end[m];
                    }
                }
            }
            unsigned int tmp_nums = generateMultiplesTemplateData(originDataList, start_idxs, end_idxs, templates_path);
            //   sample_cnt  save in txt
            ofstream sample_count;
            sample_count.open("/Users/lipengzhi/Desktop/braceletgesturerecognition/tracking/sample_count.txt");
            sample_count<<ceil(tmp_nums/2);
            sample_count<<"\n";
            break;
        }
        case 2: {
            //update min_n, max_n, min_r, max_r, valid_gap, dist_sum, dtw_thv;
            int min_n = 0;
            int max_n = 0;
            double min_r = 0;
            double max_r = 0;
            double valid_gap = 0;
            double dist_sum = 0;
            double dtw_thv = 0;
            vector<string> files;
            vector<vector<TrackPoint> > test_temp_datas;
            files = getFilesName(templates_path);
            
            //load action template.
            SampleTemplate action_template;
            ExerciseRecognition act;
            act.loadTemplateData(standard_temp_path, false, action_template);
            
            for (int i = 0; i < files.size(); i++) {
                vector<TrackPoint> tmp_data;
                loadUserData(files.at(i), 1, 0, ',', tmp_data);
                test_temp_datas.push_back(tmp_data);
            }
            double mean_n = 0;
            for (int i = 0; i < test_temp_datas.size(); i++) {
                mean_n += test_temp_datas.at(i).size();
                valid_gap += test_temp_datas.at(i).at(1).acc_dist;
                dist_sum += test_temp_datas.at(i).back().acc_dist;
                
                vector<TrackPoint> act_dst;
                act.resample(test_temp_datas.at(i), (unsigned int)action_template.action_u.size(), act_dst);
                
                //rotation & z-scale.
                act.preProcessing(act_dst);
                
                //calculate distance fastDTW.
                VectorDTW dtw1((uint)action_template.action_u.size(), action_template.action_u.size()/10.0);
                vector<TrackPoint> tem(action_template.action_u.begin(), action_template.action_u.begin() + action_template.action_u.size());
                dtw_thv += dtw1.fastdynamic(tem, act_dst) / action_template.action_u.size();
            }
            min_n = ceil(mean_n * MIN_INIT_TIME_RATE / test_temp_datas.size());
            max_n = ceil(mean_n * MAX_INIT_TIME_RATE / test_temp_datas.size());
            min_r = MIN_INIT_DIST_RATE;
            max_r = MAX_INIT_DIST_RATE;
            valid_gap = valid_gap / test_temp_datas.size();
            if (k_hz==50) {
                min_n = min_n/2;
                max_n = max_n/2;
                valid_gap = valid_gap*2;
            }
            dist_sum = dist_sum / test_temp_datas.size();
            dtw_thv = dtw_thv / test_temp_datas.size();

            //update template and write all theashold to csv.
            ofstream outFile;
            outFile.open(standard_temp_path, ios::out);
            outFile << "zscale-x,zscale-y,zscale-z,min_n,max_n,min_r,max_r,valid_gap,dist_sum,dtw_thv"<<endl;
            outFile << std::fixed;
            outFile<<action_template.action_u[0].x<<","<<action_template.action_u[0].y<<","<<action_template.action_u[0].z <<","
                   << min_n << "," << max_n << "," << min_r << "," << max_r << "," << valid_gap << "," << dist_sum << "," << dtw_thv <<endl;
            for(int i=1;i<action_template.action_u.size();i++){
                outFile<<action_template.action_u[i].x<<","<<action_template.action_u[i].y<<","<<action_template.action_u[i].z<<endl;
            }
            outFile.close();
            break;
        }
        case 3: {
            //load user trajectory.
            std::vector<TrackPoint> tracks;
            tracks.clear();
            loadUserData(users_path, 0, 0, '\t', tracks);
            run(tracks, standard_temp_path);
            break;
        }
            
        case 4: {
            //load user sudu.
            vector<string> originDataList01;
            vector<double> originDataX;
            vector<double> originDataY;
            vector<double> originDataZ;
            vector<double> sudu;
            vector<double> sudu_modle;
            data_path1 = "/Users/lipengzhi/Desktop/caiyang_nan/109.txt";
//            read_crown(data_path1);
            

            ifstream myfile(data_path1);

            if (!myfile.is_open()){
                cout << "Unable to open myfile";
                system("pause");
                exit(1);

            }

            vector<string> vec;
            string temp;

            while (getline(myfile,temp))                    //利用getline（）读取每一行，并按照行为单位放入到vector
            {

                vec.push_back(temp);
            }
            cout << "读入的用户数据为 " << endl;
            for (int i = 0; i <vec.size();i++)
            {
//                cout <<i  << endl;
//                cout <<vec[i]<< endl;
                vector<string> data;
                string tmp;
                istringstream input(vec[i]);
                
                while (getline(input, tmp, ',')){ data.push_back(tmp);
//                    cout<<"232"<<endl;
                }
                
                for (int j=0; j<data.size(); j++) {
//                    cout<<"132"<<data.size()<<endl;
                    if (j == 1) {
                        string aa = data[j];
                        double r = stod(aa.c_str());     //做数据类型转换，将string类型转换成double
                        originDataX.push_back(r);
                     }
                    if (j == 2) {
                        string aa = data[j];
                        double r = stod(aa.c_str());     //做数据类型转换，将string类型转换成double
                        originDataY.push_back(r);
                    }
                    if (j == 3) {
                        string aa = data[j];
                        double r = stod(aa.c_str());     //做数据类型转换，将string类型转换成double
                        originDataZ.push_back(r);
                    }
                    
                        
                }
            
            }
//            for (int i = 0; i <originDataX.size();i++){
//
//                cout <<i  << endl;
//                cout <<"nn"<<originDataY[i]<< endl;
            
//            }
          for(int i=0;i<originDataX.size(); i++)
            {
               double x = originDataX[i+1] - originDataX[i];
               double y = originDataY[i+1] - originDataY[i];
               double z = originDataZ[i+1] - originDataZ[i];
               double lens = sqrt(x*x + y*y + z*z);
               sudu.push_back(lens);
            }
            ofstream outfile;
            outfile.open("/Users/lipengzhi/Desktop/caiyang_nan/109_sudumodle.csv",ios::out);
            for (int i = 0; i <sudu.size();i++){
                
//                                cout <<i  << endl;
//                                cout <<"ss"<<sudu[i]<< endl;
                                outfile<<sudu[i]<< ','<<endl;
                
                            }
            outfile.close();
            
//计算dtw
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
                cout<<"sussu"<<sudu_modle[i]<<endl;

            }
            
            
            //calculate distance DTW.
            cout << "Vector DTW Test" << endl;
            vector<TrackPoint> mainVec_02;
            vector<TrackPoint> testVec_02;
            
            for(int i=0;i<sudu.size();i++){
                TrackPoint p1(sudu[i], 0, 0);
                
                testVec_02.push_back(p1);//用户速度数据
                
            }
            cout <<"testVec_02: "<<testVec_02.size() << endl;
            for(int i=0;i<sudu_modle.size();i++){
                TrackPoint p2(sudu_modle[i], 0, 0);
                mainVec_02.push_back(p2);//速度模版数据
                
            }
            cout <<"mainVec_02: "<<mainVec_02.size()<< endl;
            vector<TrackPoint> act_dst_02;
            ExerciseRecognition act;
            act.resample(mainVec_02, (uint)testVec_02.size(), act_dst_02);
            cout << "act_dst_02: " <<act_dst_02.size() << endl;
            
            
            
            VectorDTW_02 dtw2((uint)testVec_02.size(), 0.3);
            
            double dist = dtw2.fastdynamic_02(mainVec_02, testVec_02);
            
            cout << "Distance: " << dist << endl;
            break;



        }
    default:cout<<"input error"<<endl;
    }
    return 0;
}

