#ifndef INITPARAMS_H
#define INITPARAMS_H
#include<jsoncpp/json/json.h>
#include<QDebug>
#include<string>
#include<fstream>
#include<memory>
using namespace std;

struct cameraParams2
{
    string width;    // 图片宽
    string height;   //图片高
    string aoiX;
    string aoiY;
    string MyAcquisitionFrameRate;    //帧率
    string MyTriggerMode;      //0是关闭模式1是开启模式
    string MyExposureTime;                                  //设置曝光度
    string MyGainAuto;                                             // 关闭
    string MyEnumeration;                                       //关闭自动增益
    string MyGain;                                            //自动增益设定值
    string MyGammaEnable;                                       //关闭伽马矫正
    string MyGamma;                                           //伽马矫正设定值
    string MySharpnessEnable;                                  //清晰度使能
    string MySharpness;                                   //清晰度设定值
    string MyHueEnable;                                         //设置色相
    string MyHue;                                          //色相值设定
    string MySaturationEnable;                                  //饱和度使能
    string MySaturation;                                  //饱和度设定
    //相机内参
    string fx;
    string fy;
    string u0;
    string v0;
    //镜头畸变参数
    string k1;
    string k2;
    string p1;
    string p2;
    string k3;
};

struct baiduAPIParams
{
    std::string app_id;
    std::string api_key;
    std::string secret_key;
    std::string group_id_list;
    std::string image_type;
    std::string access_token;
    std::string request_url_add;
    std::string request_url_serach_group_id;
    std::string request_url_serach_user_id;
    std::string request_url_delete_group;
    std::string request_url_delete_user;
};
struct mysqlParams
{
    string HostIp;
    string HostPort;
    string DataBaseName;
    string UserName;
    string PassWord;
    string DBPoolName;
    string DBPoolMaxNum;
};


class initParams
{
public:
    initParams();
    int init(string jsonPath);
    shared_ptr<mysqlParams> getMysqlParams();
    shared_ptr<baiduAPIParams> getBaiduAPIParams();
    shared_ptr<cameraParams2> getCameraParams();
    ~initParams();
private:
    shared_ptr<mysqlParams> m_mysqlParams;
    shared_ptr<baiduAPIParams> m_baiduAPIParams;
    shared_ptr<cameraParams2> m_cameraParams;
    std::ifstream ifs;
};

#endif // INITPARAMS_H
