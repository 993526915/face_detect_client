#include "faceDetect/facedetect.h"
#include<fstream>
#include<QDebug>

static std::string callback_result;
/**
 * curl发送http请求调用的回调函数，回调函数中对返回的json格式的body进行了解析，解析结果储存在全局的静态变量当中
 * @param 参数定义见libcurl文档
 * @return 返回值定义见libcurl文档
 */
static size_t callback(void *ptr, size_t size, size_t nmemb, void *stream) {
    // 获取到的body存放在ptr中，先将其转换为string格式
    callback_result = std::string((char *) ptr, size * nmemb);
    return size * nmemb;
}


faceDetect::faceDetect(shared_ptr<QSqlDatabase> db,shared_ptr<initParams> params)
{
    m_db = db;
    m_params = params;
    m_baiduParams = params->getBaiduAPIParams();
    std::string faceCascadeFilename = "/home/wzx/detect/faceDetect/haarcascade_frontalface_default.xml";
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
    m_client = make_shared<aip::Face>(m_baiduParams->app_id, m_baiduParams->api_key, m_baiduParams->secret_key);
}
//void faceDetect::readJson()
//{
//    Json::Value root;
//    Json::Reader reader;
//    std::ifstream ifs("/home/wzx/detect/params.json");
//    if(!reader.parse(ifs, root)){
//       qDebug() << "Json解析错误-------faceDetect::readJson()" << endl;
//    }
//    else{
//       // success
//        m_baiduParams.app_id = root["baiduAPI"]["app_id"].asString();
//        m_baiduParams.api_key = root["baiduAPI"]["api_key"].asString();
//        m_baiduParams.secret_key = root["baiduAPI"]["secret_key"].asString();
//        m_baiduParams.group_id_list = root["baiduAPI"]["group_id_list"].asString();
//        m_baiduParams.image_type = root["baiduAPI"]["image_type"].asString();
//        m_baiduParams.access_token = root["baiduAPI"]["access_token"].asString();
//        m_baiduParams.request_url_add = root["baiduAPI"]["request_url_add"].asString();
//        m_baiduParams.request_url_serach_group_id = root["baiduAPI"]["request_url_search_group_id"].asString();
//        m_baiduParams.request_url_serach_user_id = root["baiduAPI"]["request_url_serach_user_id"].asString();
//        m_baiduParams.request_url_delete_group = root["baiduAPI"]["request_url_delete_group"].asString();
//        m_baiduParams.request_url_delete_user  = root["baiduAPI"]["request_url_delete_user"].asString();
//    }
//}
void faceDetect::Pic2Gray(Mat camerFrame,Mat &gray)
{
    //普通台式机3通道BGR,移动设备为4通道
    if (camerFrame.channels() == 3)
    {
        cvtColor(camerFrame, gray, COLOR_BGR2GRAY);
    }
    else if (camerFrame.channels() == 4)
    {
        cvtColor(camerFrame, gray, COLOR_BGRA2GRAY);
    }
    else
        gray = camerFrame;
}

vector<Rect> faceDetect::findFace(Mat src)
{
    //人脸检测只试用于灰度图像
    Mat gray;
    //Pic2Gray(src, gray);
    //直方图均匀化(改善图像的对比度和亮度)
    //Mat equalizedImg;
    //equalizeHist(gray, equalizedImg);
    int flags = CASCADE_SCALE_IMAGE;	//检测多个人
    Size minFeatureSize(60, 60);
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
            res.push_back(faces[i]);
        }
    }
    return res;
}


tuple<QString,QString> faceDetect::detectOneFace(Mat detectSrc)
{
    Json::Value result;
    std::string imageDecode = Mat2Base64(detectSrc,"jpg");
    result = m_client->search(imageDecode, m_baiduParams->image_type, m_baiduParams->group_id_list, aip::null);
    cout << result << endl;
    Json::Value userLists = result["result"]["user_list"];
    if(userLists.size() == 0) qDebug() << "no face" << endl;
    string user_id = userLists[0]["user_id"].asString();
    string group_id = userLists[0]["group_id"].asString();
    return tuple<QString,QString>(QString(group_id.c_str()),QString(user_id.c_str()));
}

std::string faceDetect::Mat2Base64(const cv::Mat img, std::string imgType)
{
    //Mat转base64
    std::string img_data;
    std::vector<uchar> vecImg;
    std::vector<int> vecCompression_params;
    vecCompression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    vecCompression_params.push_back(90);
    imgType = "." + imgType;
    cv::imencode(imgType, img, vecImg, vecCompression_params);
    img_data = aip::base64_encode((char *)vecImg.data(), vecImg.size());
    return img_data;
}

/**
 * 人脸注册
 * @return 调用成功返回0，发生错误返回其他错误码
 */
int faceDetect::add(Json::Value &json_result,Mat image,string group_id,string user_id ,string user_info,string quality_control)
{
    Json::Value result;
    string imageStrBase64;
    imageStrBase64 = Mat2Base64(image,"jpg");
    std::string image_type = "BASE64";

    // 如果有可选参数
    std::map<std::string, std::string> options;
    options["user_info"] = user_info;
    options["quality_control"] = quality_control;
    options["liveness_control"] = "NORMAL";
    options["action_type"] = "REPLACE";

    // 带参数调用人脸注册
    json_result = m_client->user_add(imageStrBase64, image_type, group_id, user_id, options);

    if(json_result.empty()) return 0;
    return 1;
}
/**
 * 组列表查询
 * @return 调用成功返回0，发生错误返回其他错误码
 */
int faceDetect::groupGetlist(Json::Value &json_result,string start,string length)
{
    // 如果有可选参数
    std::map<std::string, std::string> options;
    options["start"] = start;
    options["length"] = length;

    // 带参数调用组列表查询
    json_result = m_client->group_getlist(options);
    return 1;
}
/**
 * 获取用户列表
 * @return 调用成功返回0，发生错误返回其他错误码
 */
int faceDetect::groupGetusers(Json::Value &json_result, string group_id,string start,string length)
{
//    std::string url = m_baiduParams->request_url_serach_user_id + "?access_token=" + m_baiduParams->access_token;
//    CURL *curl = NULL;
//    CURLcode result_code;
//    int is_success;
//    curl = curl_easy_init();
//    if (curl) {
//        curl_easy_setopt(curl, CURLOPT_URL, url.data());
//        curl_easy_setopt(curl, CURLOPT_POST, 1);
//        curl_slist *headers = NULL;
//        headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
//        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//        string temp = "{\"group_id\":\""+group_id+"\"}";
//        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, temp.c_str());
//        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,callback);
//        result_code = curl_easy_perform(curl);
//        if (result_code != CURLE_OK) {
//            fprintf(stderr, "curl_easy_perform() failed: %s\n",
//                    curl_easy_strerror(result_code));
//            is_success = 1;
//            return is_success;
//        }
//        curl_easy_cleanup(curl);
//        is_success = 0;
//        Json::CharReaderBuilder reader;
//        string err;
//        std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
//        json_read->parse(callback_result.c_str(), callback_result.c_str() + callback_result.length(), &json_result,&err);
//    } else {
//        fprintf(stderr, "curl_easy_init() failed.");
//        is_success = 1;
//    }
//    return is_success;
    // 如果有可选参数
    std::map<std::string, std::string> options;
    options["start"] = start;
    options["length"] = length;

    // 带参数调用获取用户列表
    json_result = m_client->group_getusers(group_id, options);
    return 1;
}

/**
 * 删除用户组
 * @return 调用成功返回0，发生错误返回其他错误码
 */
int faceDetect::groupDelete(Json::Value &json_result,string group_id)
{
    json_result = m_client->group_delete(group_id, aip::null);
    return 1;
}

/**
 * 删除用户
 * @return 调用成功返回0，发生错误返回其他错误码
 */
int faceDetect::userDelete(Json::Value &json_result,string group_id,string user_id)
{
    json_result = m_client->user_delete(group_id, user_id, aip::null);
    return 1;
}
