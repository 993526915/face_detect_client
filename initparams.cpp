#include "initparams.h"

initParams::initParams()
{
    m_mysqlParams = make_shared<mysqlParams>();
    m_cameraParams = make_shared<cameraParams2>();
    m_baiduAPIParams = make_shared<baiduAPIParams>();
}

int initParams::init(string jsonPath)
{
    Json::Value root;
    Json::Reader reader;
    ifs.open(jsonPath.c_str());
    if(!ifs.is_open())
    {
        qDebug() << "Json文件打开错误--------initParams::init()" << endl;
        return 0;
    }
    if(!reader.parse(ifs, root)){
       qDebug() << "Json解析错误--------initParams::init()" << endl;
       return 0;
    }
    //百度相关参数
    m_baiduAPIParams->app_id = root["baiduAPI"]["app_id"].asString();
    m_baiduAPIParams->api_key = root["baiduAPI"]["api_key"].asString();
    m_baiduAPIParams->secret_key = root["baiduAPI"]["secret_key"].asString();
    m_baiduAPIParams->group_id_list = root["baiduAPI"]["group_id_list"].asString();
    m_baiduAPIParams->image_type = root["baiduAPI"]["image_type"].asString();
    m_baiduAPIParams->access_token = root["baiduAPI"]["access_token"].asString();
    m_baiduAPIParams->request_url_add = root["baiduAPI"]["request_url_add"].asString();
    m_baiduAPIParams->request_url_serach_group_id = root["baiduAPI"]["request_url_search_group_id"].asString();
    m_baiduAPIParams->request_url_serach_user_id = root["baiduAPI"]["request_url_serach_user_id"].asString();
    m_baiduAPIParams->request_url_delete_group = root["baiduAPI"]["request_url_delete_group"].asString();
    m_baiduAPIParams->request_url_delete_user  = root["baiduAPI"]["request_url_delete_user"].asString();

    //相机相关参数
    m_cameraParams->width = root["cameraParams"]["width"].asString();
    m_cameraParams->height = root["cameraParams"]["height"].asString();
    m_cameraParams->aoiX = root["cameraParams"]["aoiX"].asString();
    m_cameraParams->aoiY = root["cameraParams"]["aoiY"].asString();
    m_cameraParams->MyAcquisitionFrameRate = root["cameraParams"]["MyAcquisitionFrameRate"].asString();
    m_cameraParams->MyTriggerMode = root["cameraParams"]["MyTriggerMode"].asString();
    m_cameraParams->MyExposureTime = root["cameraParams"]["MyExposureTime"].asString();
    m_cameraParams->MyGainAuto = root["cameraParams"]["MyGainAuto"].asString();
    m_cameraParams->MyEnumeration = root["cameraParams"]["MyEnumeration"].asString();
    m_cameraParams->MyGammaEnable = root["cameraParams"]["MyGammaEnable"].asString();
    m_cameraParams->MyGamma = root["cameraParams"]["MyGamma"].asString();
    m_cameraParams->MySharpnessEnable = root["cameraParams"]["MySharpnessEnable"].asString();
    m_cameraParams->MySharpness = root["cameraParams"]["MySharpness"].asString();
    m_cameraParams->MyHueEnable = root["cameraParams"]["MyHueEnable"].asString();
    m_cameraParams->MyHue = root["cameraParams"]["MyHue"].asString();
    m_cameraParams->MySaturationEnable = root["cameraParams"]["MySaturationEnable"].asString();
    m_cameraParams->MySaturation = root["cameraParams"]["MySaturation"].asString();
    m_cameraParams->fx = root["cameraParams"]["fx"].asString();
    m_cameraParams->fy = root["cameraParams"]["fy"].asString();
    m_cameraParams->u0 = root["cameraParams"]["u0"].asString();
    m_cameraParams->v0 = root["cameraParams"]["v0"].asString();
    m_cameraParams->k1 = root["cameraParams"]["k1"].asString();
    m_cameraParams->k2 = root["cameraParams"]["k2"].asString();
    m_cameraParams->p1 = root["cameraParams"]["p1"].asString();
    m_cameraParams->p2 = root["cameraParams"]["p2"].asString();
    m_cameraParams->k3 = root["cameraParams"]["k3"].asString();

    //mysql相关参数
    m_mysqlParams->HostIp = root["mysql"]["HostIp"].asString();
    m_mysqlParams->HostPort = root["mysql"]["HostPort"].asString();
    m_mysqlParams->DataBaseName = root["mysql"]["DataBaseName"].asString();
    m_mysqlParams->UserName = root["mysql"]["UserName"].asString();
    m_mysqlParams->PassWord = root["mysql"]["PassWord"].asString();
    m_mysqlParams->DBPoolName = root["mysql"]["DBPoolName"].asString();
    m_mysqlParams->DBPoolMaxNum = root["mysql"]["DBPoolMaxNum"].asString();

    return 1;
}


shared_ptr<mysqlParams> initParams::getMysqlParams()
{
    return this->m_mysqlParams;
}
shared_ptr<baiduAPIParams> initParams::getBaiduAPIParams()
{
    return this->m_baiduAPIParams;
}
shared_ptr<cameraParams2> initParams::getCameraParams()
{
    return this->m_cameraParams;
}

initParams::~initParams()
{
    ifs.close();
}
