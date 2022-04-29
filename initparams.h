#ifndef _CO_PARAMS_H_
#define _CO_PARAMS_H_
#include<jsoncpp/json/json.h>
#include<string>
#include<fstream>
#include<memory>
#include<iostream>
using namespace std;

struct mysqlParams
{
    std::string HostIp;
    int HostPort;
    std::string DataBaseName;
    std::string UserName;
    std::string PassWord;
    std::string DBPoolName;
    int DBPoolMaxNum;
//    "HostIp":"127.0.0.1",
//    "HostPort":3306,
//    "DataBaseName":"face_detect",
//    "UserName":"root",
//    "PassWord":"991214",
//    "DBPoolName":"wzx",
//    "DBPoolMaxNum":4
};

class initParams
{
public:
    static initParams& GetInstance();
    shared_ptr<mysqlParams> getMysqlParams();
private:
    initParams();
    int init(string jsonPath);
private:
    shared_ptr<mysqlParams> m_mysqlParams;
    std::ifstream ifs;
};


#endif
