#ifndef MINDCAMERA_H
#define MINDCAMERA_H
#include<opencv2/opencv.hpp>
#include"CameraApi.h"
#include"CameraDefine.h"
#include"CameraStatus.h"
using namespace  cv;
struct cameraParams
{
    double fx = 531.0938;
    double fy = 527.6194;
    double cx = 338.8036;
    double cy = 252.0414;
    double k1k2Distortion[2] = { -0.4203  ,  0.2817 };
    double p1p2p3Distortion[3] = { 0,0,0 };
};

class mindCamera
{
public:
    mindCamera();
    //初始化相机
    int initCameraSDK();
    //设置相机分辨率
    int SetCameraResolution(int offsetx, int offsety, int width, int height);
    // 设置曝光时间
    int SetExposureTime(int time);
    // 获取一帧图片
    int getImage(Mat &src,bool distortion);

    ~mindCamera();
private:
    int                     g_hCamera = -1;     //设备句柄
    unsigned char           * g_pRgbBuffer=NULL;     //处理后数据缓存区
    tSdkFrameHead           g_tFrameHead;       //图像帧头信息
    tSdkCameraCapbility     g_tCapability;      //设备描述信息

    int                     g_SaveParameter_num=0;    //保存参数组
    int                     g_SaveImage_type=0;         //保存图像格式

    int                     g_read_fps=0;       //统计读取帧率
    int                     g_disply_fps=0;     //统计显示帧率

    cameraParams m_params;
    Mat matrix;
    Mat coeff;
};

#endif // MINDCAMERA_H
