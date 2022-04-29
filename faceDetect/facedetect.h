#ifndef FACEDETECT_H
#define FACEDETECT_H
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include<stdlib.h>
#include<string.h>
#include"face.h"
#include"initparams.h"
#include <curl/curl.h>
#include<jsoncpp/json/json.h>
#include<vector>
#include<QSqlDatabase>
using namespace std;
using namespace cv;

//struct baiduAPI
//{
//    std::string app_id;
//    std::string api_key;
//    std::string secret_key;
//    std::string group_id_list;
//    std::string image_type;
//    std::string access_token;
//    std::string request_url_add;
//    std::string request_url_serach_group_id;
//    std::string request_url_serach_user_id;
//    std::string request_url_delete_group;
//    std::string request_url_delete_user;
//};

class faceDetect
{

public:
    faceDetect(shared_ptr<initParams> params);
    void readJson();
    std::string Mat2Base64(const cv::Mat img, std::string imgType);
    vector<Rect> findFace(Mat src);
    tuple<QString,QString> detectOneFace(Mat detectSrc);
    static void Pic2Gray(Mat camerFrame,Mat &gray);

    int add(Json::Value &json_result,Mat image,string group_id,string user_id ,string user_info,string quality_control);
    int groupGetlist(Json::Value &json_result,string start="0",string length="100");
    int groupGetusers(Json::Value &json_result, string group_id,string start="0",string length="100");
    int groupDelete(Json::Value &json_result,string group_id) ;
    int userDelete(Json::Value &json_result,string group_id,string user_id);
private:
    string faceCascadeFilename;
    shared_ptr<CascadeClassifier> m_faceDetector;
    shared_ptr<aip::Face> m_client;
    shared_ptr<initParams> m_params;

    shared_ptr<QSqlDatabase> m_db;


};

#endif // FACEDETECT_H
