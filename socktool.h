#ifndef _CO_SOCKTOOL_H_
#define _CO_SOCKTOOL_H_


#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdint>
#include <map>
#include <vector>
#include <string>
#include<iostream>
#include<time.h>
using namespace std;

#define SOCKET_DEFAULT_BUF_SIZE (256 * 1024)
enum
{
    //face
    NOOPERATION = 0,
    ADDFACE ,
    GETGROUP,
    GETUSER,
    DELUSER,
    DELGROUP,
//------------------------------------
    //login
    LOGIN,
    //login_res
    LOGIN_RES,
    //signal
    LOGIN_SUCCESS,
    LOGIN_NOUSER,
    LOGIN_ERRPASSWORD,
//------------------------------------
    //REGISTER
    REGISTER_RES,
    //signal
    REGISTER_SUCCESS,
    REGISTER_USER_ALREADY_EXIST,
//------------------------------------
    DETECT,
    //DETECT
    DETECT_RES,
    //signal
    DETECT_SUCCESS,
    DETECT_NOUSER,
//------------------------------------
    MESSAGE_CREATE,
    MESSAGE_CREATE_RES,
    MESSAGE_CREATE_SUCCESS,
    MESSAGE_CREATE_FAILED,
//------------------------------------
    ONLINE,
    ONLINE_RES,
    ONLINE_SUCCESS,
    ONLINE_FAILED,
//------------------------------------
    OFFLINE,
    OFFLINE_RES,
    OFFLINE_SUCCESS,
    OFFLIBE_FAILED,
//------------------------------------
    CHECK_ISONLINE,
    CHECK_ISONLINE_RES,
    CHECK_ISONLINE_SUCCESS,
    CHECK_ISONLINE_FAILED,
//------------------------------------
    MESSAGE_SEND,
    MESSAGE_SEND_RES,
    MESSAGE_SEND_SUCCESS,
    MESSAGE_SEND_FAILED,
//------------------------------------
    MESSAGE_HISTORY,
    MESSAGE_HISTORY_RES,
    MESSAGE_HISTORY_COUNT_RES,
    MESSAGE_HISTORY_SUCCESS,
    MESSAGE_HISTORY_FAILED,
//------------------------------------
    MESSAGR_RECENT_SESSION,
    MESSAGR_RECENT_SESSION_RES,
    MESSAGR_RECENT_SESSION_SUCCESS,
    MESSAGR_RECENT_SESSION_FAILED,

//------------------------------------
    LOGIN_TOUXIANG,
    LOGIN_TOUXIANG_RES,
    LOGIN_TOUXIANG_SUCCESS,
    LOGIN_TOUXIANG_FAILED
};
//套接字工具类，封装了socket、网络的一些基本操作
class SockUtil {
public:
    /**
     * 创建tcp客户端套接字并连接服务器
     * @param host 服务器ip或域名
     * @param port 服务器端口号
     * @param async 是否异步连接
     * @param local_ip 绑定的本地网卡ip
     * @param local_port 绑定的本地端口号
     * @return -1代表失败，其他为socket fd号
     */
    static int co_connect(int &fd,const char *host, uint16_t port);

    /**
     * 创建tcp监听套接字
     * @param port 监听的本地端口
     * @param local_ip 绑定的本地网卡ip
     * @param back_log accept列队长度
     * @return -1代表失败，其他为socket fd号
     */
    static int co_listen(const uint16_t port, const char *local_ip = "0.0.0.0", int back_log = 1024);

    static int co_createSocket();
    static int co_read( int fd, void *buf, size_t nbyte );
    static int co_write(int fd, const void *buf, size_t nbyte);
    static int co_close(int fd);
    static int co_offline_close(int fd,string classnum,string account);
    /**
     * 绑定socket fd至某个网卡和端口
     * @param fd socket fd号
     * @param local_op 绑定的本地网卡ip
     * @param port 绑定的本地端口
     * @return 0代表成功，-1为失败
     */
    static int bindSock(int fd, const char *local_op, uint16_t port);

    /**
     * 开启TCP_NODELAY，降低TCP交互延时
     * @param fd socket fd号
     * @param on 是否开启
     * @return 0代表成功，-1为失败
     */
    static int setNoDelay(int fd, bool on = true);


    /**
     * 设置读写socket是否阻塞
     * @param fd socket fd号
     * @param noblock 是否阻塞
     * @return 0代表成功，-1为失败
     */
    static int setNoBlocked(int fd, bool noblock = true);

    /**
     * 设置socket接收缓存，默认貌似8K左右，一般有设置上限
     * 可以通过配置内核配置文件调整
     * @param fd socket fd号
     * @param size 接收缓存大小
     * @return 0代表成功，-1为失败
     */
    static int setRecvBuf(int fd, int size = SOCKET_DEFAULT_BUF_SIZE);

    /**
     * 设置socket接收缓存，默认貌似8K左右，一般有设置上限
     * 可以通过配置内核配置文件调整
     * @param fd socket fd号
     * @param size 接收缓存大小
     * @return 0代表成功，-1为失败
     */
    static int setSendBuf(int fd, int size = SOCKET_DEFAULT_BUF_SIZE);

    /**
     * 设置后续可绑定复用端口(处于TIME_WAITE状态)
     * @param fd socket fd号
     * @param on 是否开启该特性
     * @return 0代表成功，-1为失败
     */
    static int setReuseable(int fd, bool on = true, bool reuse_port = true);


    /**
     * 是否开启TCP KeepAlive特性
     * @param fd socket fd号
     * @param on 是否开启该特性
     * @return 0代表成功，-1为失败
     */
    static int setKeepAlive(int fd, bool on = true);


    /**
     * 开启SO_LINGER特性
     * @param sock socket fd号
     * @param second 内核等待关闭socket超时时间，单位秒
     * @return 0代表成功，-1为失败
     */
    static int setCloseWait(int sock, int second = 0);

    /**
     * dns解析
     * @param host 域名或ip
     * @param port 端口号
     * @param addr sockaddr结构体
     * @return 是否成功
     */
    static bool getDomainIP(const char *host, uint16_t port, struct sockaddr &addr);


    /**
     * 获取该socket绑定的本地ip
     * @param sock socket fd号
     */
    static std::string get_local_ip(int sock);

    /**
     * 获取该socket绑定的本地端口
     * @param sock socket fd号
     */
    static uint16_t get_local_port(int sock);

    /**
     * 获取该socket绑定的远端ip
     * @param sock socket fd号
     */
    static std::string get_peer_ip(int sock);

    /**
     * 获取该socket绑定的远端端口
     * @param sock socket fd号
     */
    static uint16_t get_peer_port(int sock);

    /**
     * 线程安全的in_addr转ip字符串
     */
    static std::string inet_ntoa(struct in_addr &addr);

};

class dataHeader
{
public:
    dataHeader(){}
//protected:
    int m_cmd;
    int m_dataLength;
};

class addFaceData : public dataHeader
{
public:
    addFaceData()//初始化包头
    {
        this->m_cmd = ADDFACE;
        this->m_dataLength = sizeof(addFaceData);
    }
    char m_classnum[20];
    char m_account[20];
    char m_password[20];
    char m_facedata[1024*1024];
    int m_cols;
    int m_rows;
    int m_type;
};
class getGroupData : public dataHeader
{
public:
    getGroupData();//初始化包头


};

class getUserData : public dataHeader
{
public:
    getUserData();//初始化包头
    char m_group_id[20];
    //char m_user_id[20];

};

class delUserData : public dataHeader
{
public:
    delUserData();//初始化包头
    char m_group_id[20];
    char m_user_id[20];
};

class delGroupData : public dataHeader
{
public:
    delGroupData();//初始化包头
    char m_group_id[20];
};
class loginData :public dataHeader
{
public:
    loginData();//初始化包头
    char m_classnum[20];
    char m_username[20];
    char m_password[20];

};
class loginResData :public dataHeader
{
public:
    loginResData();//初始化包头
    int m_res;
};
class registerResData :public dataHeader
{
public:
    registerResData();
    int m_res;
};

class detectFaceData :public dataHeader
{
public:
    detectFaceData();
    char m_classnum[20];
    char m_account[20];
    char m_facedata[1024*1024];
    int m_cols;
    int m_rows;
    int m_type;
};
class detectFaceResData :public dataHeader
{
public:
    detectFaceResData();
    int res;
};
class messageCreateData :public dataHeader
{
public:
    messageCreateData();
    char m_fromclassnum[20];
    char m_toclassnum[20];
    char m_fromaccount[20];
    char m_toaccount[20];
};
class messageCreateResData :public dataHeader
{
public:
    messageCreateResData();
    int res;
};


class onlineData :public dataHeader
{
public:
    onlineData();
    char m_classnum[20];
    char m_account[20];
};

class onlineResData :public dataHeader
{
public:
    onlineResData();
    int res;
};

class offlineData :public dataHeader
{
public:
    offlineData();
    char m_classnum[20];
    char m_account[20];
};
class offlineResData :public dataHeader
{
public:
    offlineResData();
    int res;
};
class checkOnlineData :public dataHeader
{
public:
    checkOnlineData()
    {
        this->m_cmd = CHECK_ISONLINE;
        this->m_dataLength = sizeof(checkOnlineData);
    }
};

class messageSendData :public dataHeader
{
public:
    messageSendData();
    char fromClassNum[20];
    char toClassNum[20];
    char fromAccount[20];
    char toAccount[20];
    int updatetime;
    char messageContent[4096];
};

class messageSendRes :public dataHeader
{
public:
    messageSendRes();
    int res;
};


class messageHistoryData :public dataHeader
{
public:
    messageHistoryData();
    char fromClassNum[20];
    char toClassNum[20];
    char fromAccount[20];
    char toAccount[20];
    int lastupdatetime;
};
class messageHistoryCountResData :public dataHeader
{
public:
    messageHistoryCountResData();
    int m_count;
    int res;
};
class messageHistoryResData :public dataHeader
{
public:
    messageHistoryResData();
    int res;
};
class messageRecentSessionData :public dataHeader
{
public:
    messageRecentSessionData();
    char classNum[20];
    char account[20];
    char lastMessage[1024];
    int lastupdatetime;
};


class messageRecentSessionResData :public dataHeader
{
public:
    messageRecentSessionResData();
    int res;
    int m_count;
};
class loginTouXiang :public dataHeader
{
public:
    loginTouXiang();
    char m_classNum[20];
    char m_account[20];
};

class loginTouXiangRes :public dataHeader
{
public:
    loginTouXiangRes();
    char m_touxiang[100];
    int res;
};
#endif
