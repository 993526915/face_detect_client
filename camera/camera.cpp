#include "camera.h"
#include<iostream>
using namespace std;
unsigned char           * g_pRgbBuffer;     //处理后数据缓存区
mindCamera::mindCamera()
{
    matrix = Mat(3, 3, CV_64F);
    matrix.at<double>(0, 0) = m_params.fx;
    matrix.at<double>(0, 1) = 0;
    matrix.at<double>(0, 2) = m_params.cx;
    matrix.at<double>(1, 0) = 0;
    matrix.at<double>(1, 1) = m_params.fy;
    matrix.at<double>(1, 2) = m_params.cy;
    matrix.at<double>(2, 0) = 0;
    matrix.at<double>(2, 1) = 0;
    matrix.at<double>(2, 2) = 1;

    coeff = Mat(1, 4, CV_64F);
    coeff.at<double>(0, 0) = m_params.k1k2Distortion[0];
    coeff.at<double>(0, 1) = m_params.k1k2Distortion[1];
    coeff.at<double>(0, 2) = m_params.p1p2p3Distortion[0];
    coeff.at<double>(0, 3) = m_params.p1p2p3Distortion[1];
}

int mindCamera::initCameraSDK()
{
    int                     iCameraCounts = 4;
    int                     iStatus=-1;
    tSdkCameraDevInfo       tCameraEnumList[4];

    //sdk初始化  0 English 1中文
    CameraSdkInit(1);

    //枚举设备，并建立设备列表
    CameraEnumerateDevice(tCameraEnumList,&iCameraCounts);

    //没有连接设备
    if(iCameraCounts==0){
        return -1;
    }

    //相机初始化。初始化成功后，才能调用任何其他相机相关的操作接口
    iStatus = CameraInit(tCameraEnumList,-1,-1,&g_hCamera);

    //初始化失败
    if(iStatus!=CAMERA_STATUS_SUCCESS){
        return -1;
    }
    //获得相机的特性描述结构体。该结构体中包含了相机可设置的各种参数的范围信息。决定了相关函数的参数
    CameraGetCapability(g_hCamera,&g_tCapability);

    g_pRgbBuffer = (unsigned char*)malloc(g_tCapability.sResolutionRange.iHeightMax*g_tCapability.sResolutionRange.iWidthMax*3);

    // 设置分辨率
    //SetCameraResolution(0, 0, 640, 480);

    /*让SDK进入工作模式，开始接收来自相机发送的图像
    数据。如果当前相机是触发模式，则需要接收到
    触发帧以后才会更新图像。    */
    CameraPlay(g_hCamera);


    /*
        设置图像处理的输出格式，彩色黑白都支持RGB24位
    */
    if(g_tCapability.sIspCapacity.bMonoSensor){

        CameraSetIspOutFormat(g_hCamera,CAMERA_MEDIA_TYPE_MONO8);
    }else{

        CameraSetIspOutFormat(g_hCamera,CAMERA_MEDIA_TYPE_BGR8);
    }
    CameraSetOnceWB(g_hCamera);

    return 0;
}


int mindCamera::SetCameraResolution(int offsetx, int offsety, int width, int height)
{
    tSdkImageResolution sRoiResolution = { 0 };

    // 设置成0xff表示自定义分辨率，设置成0到N表示选择预设分辨率
    // Set to 0xff for custom resolution, set to 0 to N for select preset resolution
    sRoiResolution.iIndex = 0xff;

    // iWidthFOV表示相机的视场宽度，iWidth表示相机实际输出宽度
    // 大部分情况下iWidthFOV=iWidth。有些特殊的分辨率模式如BIN2X2：iWidthFOV=2*iWidth，表示视场是实际输出宽度的2倍
    // iWidthFOV represents the camera's field of view width, iWidth represents the camera's actual output width
    // In most cases iWidthFOV=iWidth. Some special resolution modes such as BIN2X2:iWidthFOV=2*iWidth indicate that the field of view is twice the actual output width
    sRoiResolution.iWidth = width;
    sRoiResolution.iWidthFOV = width;

    // 高度，参考上面宽度的说明
    // height, refer to the description of the width above
    sRoiResolution.iHeight = height;
    sRoiResolution.iHeightFOV = height;

    // 视场偏移
    // Field of view offset
    sRoiResolution.iHOffsetFOV = offsetx;
    sRoiResolution.iVOffsetFOV = offsety;

    // ISP软件缩放宽高，都为0则表示不缩放
    // ISP software zoom width and height, all 0 means not zoom
    sRoiResolution.iWidthZoomSw = 0;
    sRoiResolution.iHeightZoomSw = 0;

    // BIN SKIP 模式设置（需要相机硬件支持）
    // BIN SKIP mode setting (requires camera hardware support)
    sRoiResolution.uBinAverageMode = 0;
    sRoiResolution.uBinSumMode = 0;
    sRoiResolution.uResampleMask = 0;
    sRoiResolution.uSkipMode = 0;

    return CameraSetImageResolution(g_hCamera, &sRoiResolution);
}
int mindCamera::SetExposureTime(int time)
{
    /*
        设置曝光时间。单位为微秒。对于CMOS传感器，其曝光
        的单位是按照行来计算的，因此，曝光时间并不能在微秒
        级别连续可调。而是会按照整行来取舍。在调用
        本函数设定曝光时间后，建议再调用CameraGetExposureTime
        来获得实际设定的值。
    */
    return CameraSetExposureTime(g_hCamera,time);
}
int mindCamera::getImage(Mat &src,bool distortion)
{
    BYTE*			        pbyBuffer;
    if(CameraGetImageBuffer(g_hCamera,&g_tFrameHead,&pbyBuffer,1000) == CAMERA_STATUS_SUCCESS)
    {
        CameraImageProcess(g_hCamera, pbyBuffer, g_pRgbBuffer,&g_tFrameHead);
        cv::Mat matImage(
                Size(g_tFrameHead.iWidth,g_tFrameHead.iHeight),
                g_tFrameHead.uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? CV_8UC1 : CV_8UC3,
                g_pRgbBuffer
                );
        if(distortion)
        {
            Mat new_matrix;
            undistort(matImage, src, matrix, coeff, new_matrix);
        }
        else {
            src = matImage;
        }
        //在成功调用CameraGetImageBuffer后，必须调用CameraReleaseImageBuffer来释放获得的buffer。
        //否则再次调用CameraGetImageBuffer时，程序将被挂起一直阻塞，直到其他线程中调用CameraReleaseImageBuffer来释放了buffer
        CameraReleaseImageBuffer(g_hCamera,pbyBuffer);
        return CAMERA_STATUS_SUCCESS;
    }
    return CAMERA_STATUS_FAILED;

}
mindCamera::~mindCamera()
{
    CameraUnInit(g_hCamera);
    //注意，现反初始化后再free
    free(g_pRgbBuffer);
}
