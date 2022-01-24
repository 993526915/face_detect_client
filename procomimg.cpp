#include "procomimg.h"
#include<QDebug>
#include<QEventLoop>
#include<QTimer>
int BUFFER;
static volatile unsigned int proIdx = 0;    //生产id
static volatile unsigned int consIdx = 0; //消费id

proComImg::proComImg(shared_ptr<faceDetect> faceDetect,shared_ptr<initParams> params)
{
    qRegisterMetaType<Mat>("Mat");
    m_timeStamp = make_shared<CELLTimestamp>();
    m_faceDetect = faceDetect;
    m_isdetect = false;
    m_isFindFace = false;
    m_baiduParams =params->getBaiduAPIParams();
#ifdef COMPUTERCAMERA
    cap.open(0);
#else
    m_camera = make_shared<mindCamera>();
    m_camera->initCameraSDK();
    m_camera->SetCameraResolution(0,0,640,480);
#endif
    readJson();

}
void proComImg::readJson()
{
    Json::Value root;
    Json::Reader reader;
    std::ifstream ifs("/home/wzx/detect/params.json");
    if(!reader.parse(ifs, root)){
       qDebug() << "Json解析错误---------proComImg::readJson()" << endl;
    }
    else{
       // success
        BUFFER = root["imgBuff"].asInt();
    }
}
void proComImg::produce()
{
    cv::Mat src;
    while(1)
    {
       Mat src ;
#ifdef COMPUTERCAMERA
       if(!cap.isOpened())
       {
           cout << "computer cant open"<<endl;
           return ;
       }
       Mat inputSrc;
       cap >> inputSrc;
       resize(inputSrc,src,Size(640,480));
#else
       int c = m_camera->getImage(src,true);
#endif
       if(src.empty()) continue;
       try
       {
           m_imgBuffer[proIdx % BUFFER] = src;
           proIdx++;
       }
       catch (...)
       {
           qDebug() << "照片读如出错" << endl;
           break;
       }
    }

}
void proComImg::comsume()
{
    while(1)
    {
        while(consIdx>=proIdx);
        unsigned int ImageIndex = proIdx-1;              //处理最新图像
        Mat src = m_imgBuffer[ImageIndex % BUFFER];
        m_mutex.lock();
        vector<Rect> res = m_faceDetect->findFace(src);
        if(res.size() != 0) m_isFindFace = true;
        else
        {
            m_isFindFace = false;
            //qDebug() << "find no face" << endl;
        }
        if(res.size()>0)
        {
            //Rect temp(res[0].x/1.2,res[0].y/1.2,res[0].width*1.3,res[0].height*1.3);
            Rect temp(res[0].x/1.1,res[0].y/1.1,res[0].width*1.1,res[0].height*1.1);
            cv::rectangle(src, temp, cv::Scalar(0, 255, 255), 2, 8, 0);
            if(temp.area() > 200)
                 m_detect.push_back(src(temp));
        }
        m_mutex.unlock();
        emit getImage(src);
        consIdx = ImageIndex+1;
        waitkey(1);
    }
}

void proComImg::detect()
{
    aip::Face client(m_baiduParams->app_id, m_baiduParams->api_key, m_baiduParams->secret_key);
    Json::Value result;
    while(1)
    {
        while(m_isdetect)
        {
            if(m_timeStamp->getElapsedSecond() >= 1 && m_isFindFace == true)
            {
                m_mutex.lock();
                Mat detectSrc = m_detect[0];
                m_detect.clear();
                m_mutex.unlock();

                tuple<QString,QString> detectRes = m_faceDetect->detectOneFace(detectSrc);
                if(std::get<0>(detectRes).isEmpty() || std::get<1>(detectRes).isEmpty()) continue;
                qDebug() << "group_id : " <<std::get<0>(detectRes)<< "user_id : " <<std::get<1>(detectRes) << endl;
                emit detectFace(std::get<0>(detectRes),std::get<1>(detectRes));
                m_timeStamp->update();
            }
        }
    }
}



void proComImg::setIsDetect(bool set)
{
    m_isdetect = set;
}

bool proComImg::getIsDetect()
{
    return this->m_isdetect;
}


comsumePro::comsumePro(shared_ptr<proComImg> p)
{
    m_consume = p;
    void (proComImg::*time)(int) = &proComImg::waitkey;
    connect(m_consume.get(),time,
            [&]()
            {
                QEventLoop loop;
                QTimer::singleShot(10, &loop, SLOT(quit()));
                loop.exec();

            }
            );
}

void comsumePro::run()
{
    m_consume->comsume();
}

producePro::producePro(shared_ptr<proComImg> p)
{
    m_produce = p;
}
void producePro::run()
{
    m_produce->produce();
}

detectPro::detectPro(shared_ptr<proComImg> p)
{
    m_detect = p;
}
void detectPro::run()
{
    m_detect->detect();
}

PrintQPixMap::PrintQPixMap(shared_ptr<proComImg> p)
{
    this->m_proComImg =p;
}

QImage PrintQPixMap::cvMat_to_QImage(const cv::Mat &mat)
{
    switch ( mat.type() )
    {
       // 8-bit, 4 channel
       case CV_8UC4:
       {
          QImage image( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB32 );
          return image;
       }

       // 8-bit, 3 channel
       case CV_8UC3:
       {
          QImage image( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888 );
          return image.rgbSwapped();
       }

       // 8-bit, 1 channel
       case CV_8UC1:
       {
          static QVector<QRgb>  sColorTable;
          // only create our color table once
          if ( sColorTable.isEmpty() )
          {
             for ( int i = 0; i < 256; ++i )
                sColorTable.push_back( qRgb( i, i, i ) );
          }
          QImage image( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8 );
          image.setColorTable( sColorTable );
          return image;
       }

       default:
          qDebug("Image format is not supported: depth=%d and %d channels\n", mat.depth(), mat.channels());
          break;
    }
    return QImage();
}

void PrintQPixMap::PrintQPixMap::showImage(Mat src)
{
    if(!src.empty())
    {
        QImage image;
        image = cvMat_to_QImage(src);
        QPixmap pixMap = QPixmap::fromImage(image);
        emit printImg(pixMap);
    }
}


