#include"socktool.h"
std::string SockUtil::inet_ntoa(struct in_addr &addr)
{
    char buf[20];
    unsigned char *p = (unsigned char *) &(addr);
    snprintf(buf, sizeof(buf), "%u.%u.%u.%u", p[0], p[1], p[2], p[3]);
    return string(buf);
}
int SockUtil::co_connect(int &fd,const char *host, uint16_t port)
{
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    int nIP = 0;
    if( !host || '\0' == *host
            || 0 == strcmp(host,"0") || 0 == strcmp(host,"0.0.0.0")
            || 0 == strcmp(host,"*")
      )
    {
        nIP = htonl(INADDR_ANY);
    }
    else
    {
        nIP = inet_addr(host);
    }
    addr.sin_addr.s_addr = nIP;
    return connect(fd,(struct sockaddr*)&addr,sizeof(addr));
}
int SockUtil::co_createSocket()
{
    return socket(PF_INET, SOCK_STREAM, 0);
}
int SockUtil::co_listen(const uint16_t port, const char *local_ip , int back_log )
{
    int fd = -1;
    if ((fd = (int) socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        cout << "创建套接字失败:" << fd << endl;
        return -1;
    }

    setReuseable(fd, true, false);
    setNoBlocked(fd);
    if (bindSock(fd, local_ip, port) == -1) {
        close(fd);
        return -1;
    }

    //开始监听
    if (::listen(fd, back_log) == -1) {
        cout << "开始监听失败:port " << port << " used " << endl;;
        close(fd);
        return -1;
    }

    return fd;
}
int SockUtil::co_write(int fd, const void *buf, size_t nbyte)
{
    int totalLen = 0;
    int writeLen = write(fd, buf, nbyte);
    if(writeLen < 0)
    {
        cout << writeLen<<endl;
        cout << "write error!" << endl;
        return writeLen;
    }
    if( writeLen > 0 )
    {
        totalLen += writeLen;
    }
    while( totalLen < (int)nbyte )
    {

        writeLen = write( fd,(const char *)buf + writeLen,nbyte - writeLen );

        if( writeLen <= 0 )
        {
            break;
        }
        totalLen += writeLen ;
    }
    return totalLen;
}
int SockUtil::co_read( int fd, void *buf, size_t nbyte )
{
    ssize_t readret = read( fd,(char*)buf ,nbyte );

    if( readret < 0 )
    {
        cout << "read error!" << endl;
    }

    return readret;
}
int SockUtil::co_close(int fd)
{
    return close(fd);
}
int SockUtil::co_offline_close(int fd,string classnum,string account)
{
    while(1)
    {
        offlineData offline;
        memcpy(offline.m_classnum,classnum.c_str(),classnum.length());
        memcpy(offline.m_account,account.c_str(),account.length());
        SockUtil::co_write(fd,&offline,offline.m_dataLength);

        offlineResData offlineRes;
        SockUtil::co_read(fd,&offlineRes,offlineRes.m_dataLength);
        if(offlineRes.res==OFFLINE_SUCCESS) break;
    }
    return close(fd);
}
int SockUtil::bindSock(int fd, const char *local_op, uint16_t port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(local_op);
    bzero(&(addr.sin_zero), sizeof addr.sin_zero);
    //绑定监听
    if (::bind(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        cout << "绑定套接字失败:" << fd << endl;
        return -1;
    }
    return 0;
}

int SockUtil::setNoDelay(int fd, bool on)
{
    int opt = on ? 1 : 0;
    int ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *) &opt, static_cast<socklen_t>(sizeof(opt)));
    if (ret == -1) {
        cout << "设置 NoDelay 失败!" << endl;
    }
    return ret;
}
int SockUtil::setNoBlocked(int fd, bool noblock )
{
    int iFlags;
    int ret = -1;
    int opt = noblock ? 1 : 0;
    if(opt)
    {
        iFlags = fcntl(fd, F_GETFL, 0);
        iFlags |= O_NONBLOCK;
    //    iFlags |= O_NDELAY;
        ret = fcntl(fd, F_SETFL, iFlags);
    }
    else
    {
        iFlags = fcntl(fd, F_GETFL, 0);
        iFlags &= ~O_NONBLOCK;
        ret = fcntl(fd, F_SETFL, iFlags);
    }
    if (ret == -1) {
        cout << "设置 NoBlocked 失败!" << endl;
    }
    return ret;
}
int SockUtil::setRecvBuf(int fd, int size )
{
    int ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *) &size, sizeof(size));
    if (ret == -1) {
        cout << "设置接收缓冲区失败!" << endl;
    }
    return ret;
}

int SockUtil::setSendBuf(int fd, int size)
{
    int ret = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *) &size, sizeof(size));
    if (ret == -1) {
        cout << "设置发送缓冲区失败!" << endl;
    }
    return ret;
}
int SockUtil::setReuseable(int fd, bool on , bool reuse_port)
{
    int opt = on ? 1 : 0;
    int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, static_cast<socklen_t>(sizeof(opt)));
    if (ret == -1) {
        cout << "设置 SO_REUSEADDR 失败!"<< endl;
        return ret;
    }
#if defined(SO_REUSEPORT)
    if (reuse_port) {
        ret = setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (char *) &opt, static_cast<socklen_t>(sizeof(opt)));
        if (ret == -1) {
            cout << "设置 SO_REUSEPORT 失败!"<<endl;
        }
    }
#endif
    return ret;
}
int SockUtil::setKeepAlive(int fd, bool on)
{
    int opt = on ? 1 : 0;
    int ret = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *) &opt, static_cast<socklen_t>(sizeof(opt)));
    if (ret == -1) {
        cout << "设置 SO_KEEPALIVE 失败!" << endl;
    }
    return ret;
}

int SockUtil::setCloseWait(int sock, int second )
{
    linger m_sLinger;
    //在调用closesocket()时还有数据未发送完，允许等待
    // 若m_sLinger.l_onoff=0;则调用closesocket()后强制关闭
    m_sLinger.l_onoff = (second > 0);
    m_sLinger.l_linger = second; //设置等待时间为x秒
    int ret = setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *) &m_sLinger, sizeof(linger));
    if (ret == -1) {
#ifndef _WIN32
        cout << "设置 SO_LINGER 失败!" << endl;
#endif
    }
    return ret;
}

bool getSystemDomainIP(const char *host, sockaddr &item) {
    struct addrinfo *answer = nullptr;
    //阻塞式dns解析，可能被打断
    int ret = -1;
    do {
        ret = getaddrinfo(host, nullptr, nullptr, &answer);
    } while (ret == -1);

    if (!answer) {
        cout << "域名解析失败:" << host;
        return false;
    }
    item = *(answer->ai_addr);
    freeaddrinfo(answer);
    return true;
}

bool SockUtil::getDomainIP(const char *host, uint16_t port, struct sockaddr &addr)
{
    bool flag = getSystemDomainIP(host, addr);
    if (flag) {
        ((sockaddr_in *) &addr)->sin_port = htons(port);
    }
    return flag;
}

std::string SockUtil::get_local_ip(int sock)
{
    struct sockaddr addr;
    struct sockaddr_in *addr_v4;
    socklen_t addr_len = sizeof(addr);
    //获取local ip and port
    memset(&addr, 0, sizeof(addr));
    if (0 == getsockname(sock, &addr, &addr_len)) {
        if (addr.sa_family == AF_INET) {
            addr_v4 = (sockaddr_in *) &addr;
            return SockUtil::inet_ntoa(addr_v4->sin_addr);
        }
    }
    return "";
}


uint16_t SockUtil::get_local_port(int sock)
{
    struct sockaddr addr;
    struct sockaddr_in *addr_v4;
    socklen_t addr_len = sizeof(addr);
    //获取remote ip and port
    if (0 == getsockname(sock, &addr, &addr_len)) {
        if (addr.sa_family == AF_INET) {
            addr_v4 = (sockaddr_in *) &addr;
            return ntohs(addr_v4->sin_port);
        }
    }
    return 0;
}


std::string SockUtil::get_peer_ip(int sock)
{
    struct sockaddr addr;
    struct sockaddr_in *addr_v4;
    socklen_t addr_len = sizeof(addr);
    //获取remote ip and port
    if (0 == getpeername(sock, &addr, &addr_len)) {
        if (addr.sa_family == AF_INET) {
            addr_v4 = (sockaddr_in *) &addr;
            return SockUtil::inet_ntoa(addr_v4->sin_addr);
        }
    }
    return "";
}


uint16_t SockUtil::get_peer_port(int sock)
{
    struct sockaddr addr;
    struct sockaddr_in *addr_v4;
    socklen_t addr_len = sizeof(addr);
    //获取remote ip and port
    if (0 == getpeername(sock, &addr, &addr_len)) {
        if (addr.sa_family == AF_INET) {
            addr_v4 = (sockaddr_in *) &addr;
            return ntohs(addr_v4->sin_port);
        }
    }
    return 0;
}

getGroupData::getGroupData()//初始化包头
{
    this->m_cmd = GETGROUP;
    this->m_dataLength = sizeof(getGroupData);
}
getUserData::getUserData()//初始化包头
{
    this->m_cmd = GETUSER;
    this->m_dataLength = sizeof(getUserData);
    memset(m_group_id,0,sizeof(m_group_id));
}
delUserData::delUserData()//初始化包头
{
    this->m_cmd = DELUSER;
    this->m_dataLength = sizeof(delUserData);
    memset(m_group_id,0,sizeof(m_group_id));
    memset(m_user_id,0,sizeof(m_user_id));
}
delGroupData::delGroupData()
{
    this->m_cmd = DELGROUP;
    this->m_dataLength = sizeof(delGroupData);
    memset(m_group_id,0,sizeof(m_group_id));
}
loginData::loginData()
{
    this->m_cmd = LOGIN;
    this->m_dataLength = sizeof(loginData);
    memset(m_username,0,sizeof (m_username));
    memset(m_classnum,0,sizeof (m_classnum));
    memset(m_password,0,sizeof (m_password));
}
loginResData::loginResData()
{
    this->m_cmd = LOGIN_RES;
    this->m_dataLength = sizeof(loginData);
}
registerResData::registerResData()
{
    this->m_cmd = REGISTER_RES;
    this->m_dataLength = sizeof(registerResData);
}
detectFaceData::detectFaceData()
{
    this->m_cmd = DETECT;
    this->m_dataLength = sizeof(detectFaceData);
    memset(m_facedata,0,sizeof(m_facedata));
    memset(m_classnum,0,sizeof(m_classnum));
    memset(m_account,0,sizeof(m_account));
}
detectFaceResData::detectFaceResData()
{
    this->m_cmd = DETECT_RES;
    this->m_dataLength = sizeof(detectFaceResData);
}
messageCreateData::messageCreateData()
{
    this->m_cmd = MESSAGE_CREATE;
    this->m_dataLength = sizeof(messageCreateData);
    memset(m_fromclassnum,0,sizeof(m_fromclassnum));
    memset(m_fromaccount,0,sizeof(m_fromaccount));
    memset(m_toclassnum,0,sizeof(m_toclassnum));
    memset(m_toaccount,0,sizeof(m_toaccount));
    memset(m_peertouxiang,0,sizeof(m_peertouxiang));
}
messageCreateResData::messageCreateResData()
{
    this->m_cmd = MESSAGE_CREATE_RES;
    this->m_dataLength = sizeof(messageCreateResData);
}

onlineData::onlineData()
{
    this->m_cmd = ONLINE;
    this->m_dataLength = sizeof(onlineData);
    memset(m_classnum,0,sizeof(m_classnum));
    memset(m_account,0,sizeof(m_account));
}
onlineResData::onlineResData()
{
    this->m_cmd = ONLINE_RES;
    this->m_dataLength = sizeof(onlineResData);
}
offlineData::offlineData()
{
    this->m_cmd = OFFLINE;
    this->m_dataLength = sizeof(offlineData);
    memset(m_classnum,0,sizeof(m_classnum));
    memset(m_account,0,sizeof(m_account));
}
offlineResData::offlineResData()
{
    this->m_cmd = OFFLINE_RES;
    this->m_dataLength = sizeof(offlineResData);
}
messageSendData::messageSendData()
{
    this->m_cmd = MESSAGE_SEND;
    this->m_dataLength = sizeof(messageSendData);
    memset(fromClassNum,0,sizeof(fromClassNum));
    memset(toClassNum,0,sizeof(toClassNum));
    memset(fromAccount,0,sizeof(fromAccount));
    memset(toAccount,0,sizeof(toAccount));
    updatetime = (int)time(NULL);
    memset(messageContent,0,sizeof(messageContent));
}
messageSendRes::messageSendRes()
{
    this->m_cmd = MESSAGE_SEND_RES;
    this->m_dataLength = sizeof(messageSendRes);
    res = 0;
}
messageHistoryData::messageHistoryData()
{
    this->m_cmd = MESSAGE_HISTORY;
    this->m_dataLength = sizeof(messageHistoryData);
    memset(fromClassNum,0,sizeof(fromClassNum));
    memset(toClassNum,0,sizeof(toClassNum));
    memset(fromAccount,0,sizeof(fromAccount));
    memset(toAccount,0,sizeof(toAccount));
    lastupdatetime = 0;
}
messageHistoryCountResData::messageHistoryCountResData()
{
    this->m_cmd = MESSAGE_HISTORY_COUNT_RES;
    this->m_dataLength = sizeof(messageHistoryCountResData);
    m_count = 0;
}
messageHistoryResData::messageHistoryResData()
{
    this->m_cmd = MESSAGE_HISTORY_RES;
    this->m_dataLength = sizeof(messageHistoryCountResData);
}
messageRecentSessionData::messageRecentSessionData()
{
    this->m_cmd = MESSAGR_RECENT_SESSION;
    this->m_dataLength = sizeof(messageRecentSessionData);
    memset(classNum,0,sizeof(classNum));
    memset(account,0,sizeof(account));
    memset(touxiang,0,sizeof(touxiang));
    memset(lastMessage,0,sizeof(lastMessage));
    lastupdatetime = 0;
}

messageRecentSessionResData::messageRecentSessionResData()
{
    this->m_cmd = MESSAGR_RECENT_SESSION_RES;
    this->m_dataLength = sizeof(messageRecentSessionResData);
}
loginTouXiang::loginTouXiang()
{
    this->m_cmd = LOGIN_TOUXIANG;
    this->m_dataLength = sizeof(loginTouXiang);
    memset(m_classNum,0,sizeof(m_classNum));
    memset(m_account,0,sizeof(m_account));
}

loginTouXiangRes::loginTouXiangRes()
{
    this->m_cmd = LOGIN_TOUXIANG_RES;
    this->m_dataLength = sizeof(loginTouXiangRes);
    memset(m_touxiang,0,sizeof(m_touxiang));
}

changeTouXiangData::changeTouXiangData()
{
    this->m_cmd = CHANGE_TOUXIANG;
    this->m_dataLength = sizeof(changeTouXiangData);
    memset(m_classNum,0,sizeof(m_classNum));
    memset(m_account,0,sizeof(m_account));
    memset(m_touxiang,0,sizeof(m_touxiang));
}

changeTouXiangRes::changeTouXiangRes()
{
    this->m_cmd = CHANGE_TOUXIANG_RES;
    this->m_dataLength = sizeof(changeTouXiangRes);
    m_res = 0;
}

getTouXiangData::getTouXiangData()
{
    this->m_cmd = GET_TOUXIANG;
    this->m_dataLength = sizeof(getTouXiangData);
    memset(m_classNum,0,sizeof(m_classNum));
    memset(m_account,0,sizeof(m_account));
    
}

getTouXiangRes::getTouXiangRes()
{
    this->m_cmd = GET_TOUXIANG_RES;
    this->m_dataLength = sizeof(getTouXiangRes);
    memset(m_touxiang,0,sizeof(m_touxiang));
    m_res = 0;
}
