#ifndef PROCOMIMG_H
#define PROCOMIMG_H
#include<opencv2/opencv.hpp>
#include<jsoncpp/json/json.h>
#include<QObject>
#include<QMutex>
#include<QThread>
#include <QPixmap>
#include<QImage>
#include<mysql/mysql.h>
#include"faceDetect/facedetect.h"
#include"timeStamp.h"
#include"camera/camera.h"
#include"faceDetect/face.h"
#include"initparams.h"
#include"DBpool.h"
using namespace std;
using namespace  cv;


#define COMPUTERCAMERA 1

extern int BUFFER; //图片缓存区最大数目

class proComImg :public QObject
{
    Q_OBJECT
public:
    proComImg(shared_ptr<faceDetect> faceDetect,shared_ptr<initParams> params);
    void produce();
    void comsume();
    void detect();
    void readJson();
    void setIsDetect(bool set);
    bool getIsDetect();
private:
    shared_ptr<CELLTimestamp> m_timeStamp;
    shared_ptr<faceDetect> m_faceDetect;
    shared_ptr<baiduAPIParams>m_baiduParams;
    QMutex m_mutex;
    Mat m_imgBuffer[10];
    vector<Mat> m_detect;
    bool m_isdetect;
    bool m_isFindFace;
#ifdef COMPUTERCAMERA
    VideoCapture cap;
#else
    shared_ptr<mindCamera> m_camera;
#endif
signals:
    void getImage(Mat src);
    void waitkey(int time);
    void detectFace(QString group_id,QString usr_id);
};

class comsumePro : public QThread
{
    Q_OBJECT
public:
    comsumePro(shared_ptr<proComImg> p);
protected:
    void run() Q_DECL_OVERRIDE;
    void stopThread();

private:
    shared_ptr<proComImg> m_consume;
};

class producePro : public QThread
{
    Q_OBJECT
public:
    producePro(shared_ptr<proComImg> p);
protected:
    void run() Q_DECL_OVERRIDE;
    void stopThread();

private:
    shared_ptr<proComImg> m_produce;
};

class detectPro : public QThread
{
    Q_OBJECT
public:
    detectPro(shared_ptr<proComImg> p);
protected:
    void run() Q_DECL_OVERRIDE;
    void stopThread();

private:
    shared_ptr<proComImg> m_detect;
};

class PrintQPixMap : public QObject
{
    Q_OBJECT
public:
    PrintQPixMap(shared_ptr<proComImg> phoenix);
private:
    QImage cvMat_to_QImage(const cv::Mat &mat);
    shared_ptr<proComImg> m_proComImg;
    QMutex m_mutex;

public slots:
    void showImage(Mat src);

signals:
    void printImg(QPixmap pixMap);
};

#endif // PROCOMIMG_H
