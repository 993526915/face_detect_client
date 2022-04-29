#include "procomimg.h"
#include<QDebug>
#include<QEventLoop>
#include<QTimer>
int BUFFER;
static volatile unsigned int proIdx = 0;    //生产id
static volatile unsigned int consIdx = 0; //消费id

proComImg::proComImg()
{
    qRegisterMetaType<Mat>("Mat");
    m_timeStamp = make_shared<CELLTimestamp>();
    m_isdetect = true;
    m_isproduce =true;
    m_iscomsume = true;
    m_isFindFace = false;
    std::string faceCascadeFilename = "/home/wzx/face_detect_client/faceDetect/haarcascade_frontalface_default.xml";
    m_faceDetector = make_shared<CascadeClassifier>();
    //友好错误信息提示
    try{
        //readJson();
        m_faceDetector->load(faceCascadeFilename);
    }
    catch (cv::Exception e){}
    if (m_faceDetector->empty())
    {
        std::cerr << "脸部检测器不能加载 (";
        std::cerr << faceCascadeFilename << ")!" << std::endl;
        exit(1);
    }
#ifdef COMPUTERCAMERA
    cap.open(0);
#else
    m_camera = make_shared<mindCamera>();
    m_camera->initCameraSDK();
    m_camera->SetCameraResolution(0,0,640,480);
#endif
    readJson();

}
vector<Rect> proComImg::findFace(Mat src)
{
    //人脸检测只试用于灰度图像
    Mat gray;
    //Pic2Gray(src, gray);
    //直方图均匀化(改善图像的对比度和亮度)
    //Mat equalizedImg;
    //equalizeHist(gray, equalizedImg);
    int flags = CASCADE_SCALE_IMAGE;	//检测多个人
    Size minFeatureSize(100, 100);
    float searchScaleFactor = 1.3f;
    int minNeighbors = 2;
    std::vector<Rect> faces;
    m_faceDetector->detectMultiScale(src, faces, searchScaleFactor, minNeighbors, flags, minFeatureSize);
    //画矩形框
    cv::Mat face;
    std::vector<Rect> res;
    cv::Point text_lb;
    for (size_t i = 0; i < faces.size(); i++)
    {
        if (faces[i].height > 0 && faces[i].width > 0)
        {
//            /****************画方框***************/
//            face = gray(faces[i]);
//            text_lb = cv::Point(faces[i].x, faces[i].y);
//            cv::rectangle(equalizedImg, faces[i], cv::Scalar(255, 0, 0), 2, 8, 0);
//            cv::rectangle(gray, faces[i], cv::Scalar(255, 0, 0), 2, 8, 0);
//            cv::rectangle(src, faces[i], cv::Scalar(255, 0, 0), 2, 8, 0);
//            /****************画方框完成***************/
            if(faces[i].width%4 ==0 && faces[i].height%4==0)
                res.push_back(faces[i]);
        }
    }
    return res;
}

void proComImg::readJson()
{
    Json::Value root;
    Json::Reader reader;
    std::ifstream ifs("/home/wzx/face_detect_client/params.json");
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
    while(m_isproduce)
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
       if(!inputSrc.empty())
            resize(inputSrc,src,Size(492,369));
       else
           qDebug() << "no picture!" << endl;
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
    if(cap.isOpened())
        cap.release();
}
void proComImg::comsume()
{
    while(m_iscomsume)
    {
        while(consIdx>=proIdx);
        unsigned int ImageIndex = proIdx-1;              //处理最新图像
        Mat src = m_imgBuffer[ImageIndex % BUFFER];
        m_mutex.lock();

        vector<Rect> res = findFace(src);
        if(res.size() != 0) m_isFindFace = true;
        else
        {
            m_isFindFace = false;
            //qDebug() << "find no face" << endl;
        }
        if(res.size()>0)
        {
            //Rect temp(res[0].x/1.2,res[0].y/1.2,res[0].width*1.3,res[0].height*1.3);

            Rect temp(res[0].x,res[0].y,res[0].width,res[0].height);
            if(temp.area() > 200)
                 m_detect.push_back(src(temp));
            cv::rectangle(src, temp, cv::Scalar(0, 255, 255), 2, 8, 0);
            usleep(1);

        }
        m_mutex.unlock();
        emit getImage(src);
        consIdx = ImageIndex+1;
    }
}

void proComImg::detect(string classnum,string account,int fd)
{
    while(m_isdetect)
    {
        if(m_timeStamp->getElapsedSecond() >= 1 && m_isFindFace == true)
        {
            m_mutex.lock();
            Mat detectSrc;
            detectSrc = m_detect[0];
            m_detect.clear();
            m_mutex.unlock();
            imwrite("/home/wzx/b.jpg",detectSrc);

            detectSrc = imread("/home/wzx/b.jpg");
            detectFaceData * detectFace = new detectFaceData;
            detectFace->m_cols = detectSrc.cols;
            detectFace->m_rows = detectSrc.rows;
            detectFace->m_type = detectSrc.type();

            memcpy(detectFace->m_facedata,detectSrc.data, detectSrc.cols * detectSrc.rows *detectSrc.elemSize());
            memcpy(detectFace->m_classnum,classnum.data(),classnum.length());
            memcpy(detectFace->m_account,account.data(),account.length());
            SockUtil::co_write(fd,detectFace,detectFace->m_dataLength);


            detectFaceResData addFaceRes;
            SockUtil::co_read(fd,&addFaceRes,addFaceRes.m_dataLength);
            if(addFaceRes.res==DETECT_SUCCESS)
            {
                qDebug() << "DETECT_SUCCESS";
                emit faceSuccess();
            }
            else
            {
                qDebug() << "DETECT_NOUSER";
                emit faceFailed();
            }
            m_timeStamp->update();
        }
    }
}

void proComImg::setIsDetect(bool set)
{
    m_isdetect = set;
}
void proComImg::setIsProduce(bool set)
{
    m_isproduce = set;
}
void proComImg::setIsComsume(bool set)
{
    m_iscomsume = set;
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
                QTimer::singleShot(50, &loop, SLOT(quit()));
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

detectPro::detectPro(shared_ptr<proComImg> p,shared_ptr<int> fd,string classnum,string account)
{
    m_detect = p;
    m_sockFd = fd;
    m_classnum =classnum;
    m_account = account;
}

void detectPro::run()
{
    m_detect->detect(m_classnum,m_account,*m_sockFd.get());
}
//socketPro::socketPro(shared_ptr<proComImg> p)
//{
//    m_socket = p;
//}
//void socketPro::run()
//{
//    m_socket->request();
//}

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


