#include <srt.h>

#include <string>
#include <thread>
#include <array>

#include "srt_wrap.hpp"

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x602
#endif
#include <WS2tcpip.h>
#pragma comment(lib,"Ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

class SrtWrapImpl {
    static constexpr int POLLING_TIME = 1; /// Time in milliseconds between interrupt check
    static constexpr int TS_UDP_LEN = 188*7;
public:
    explicit SrtWrapImpl(const char *ip,int port);

    ~SrtWrapImpl();

    bool open();

    void close();

    bool write(const uint8_t *buf, int size);

private:
    bool init();

    bool initConnect(const char *streamid);

    bool connectServer();

    bool receive(uint8_t *&buf,int &size);

    bool publish(const uint8_t *buf, int size);

private:
    std::string ip;
    int port;
    SRTSOCKET sock;
    int eid;
};

SrtWrapImpl::SrtWrapImpl(const char *ip,int port):ip(ip),port(port)
{
}

SrtWrapImpl::~SrtWrapImpl()
{
    close();
    srt_cleanup();
}

#ifdef _WIN32
static bool inetPton(int32_t af, const char *src, void *dst) {
    struct sockaddr_storage ss;
    int32_t ssSize = sizeof(ss);
    ZeroMemory(&ss, sizeof(ss));
    std::array<char,INET6_ADDRSTRLEN + 1> srcCopy;
    // work around non-const API
    strncpy(srcCopy.data(), src, srcCopy.size());
    srcCopy[INET6_ADDRSTRLEN] = '\0';
    if (WSAStringToAddress(srcCopy.data(), af, nullptr, (struct sockaddr*) &ss, &ssSize) != 0) {
        return false;
    }

    switch (af) {
    case AF_INET: {
        *(struct in_addr*) dst = ((struct sockaddr_in*) &ss)->sin_addr;
        return true;
    }
    case AF_INET6: {
        *(struct in6_addr*) dst = ((struct sockaddr_in6*) &ss)->sin6_addr;
        return true;
    }
    default: {
        // No-Op
    }
    }

    return 0;
}
#endif

bool SrtWrapImpl::open()
{


    return false;
}

void SrtWrapImpl::close()
{
    int32_t st = 0;
    if (eid > 0) {
        st = srt_epoll_remove_usock(eid, sock);
        srt_epoll_release(eid);

    }

    //srt_close
    if (sock > 0) {
        st = srt_close(sock);
    }

    if (st == SRT_ERROR) {
        fprintf(stderr,"srt_close: %s \n", srt_getlasterror_str());
        return;
    }

    sock = 0;
}

bool SrtWrapImpl::write(const uint8_t *buf, int size)
{

    return true;
}

bool SrtWrapImpl::init()
{
#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2); //create 16bit data
    if (WSAStartup(wVersionRequested, &wsaData) != 0) {
        fprintf(stderr,"Load WinSock Failed!\n");
        return false;
    }
#endif
    srt_startup();
    srt_setloglevel(srt_logging::LogLevel::debug);

    return true;
}

bool SrtWrapImpl::initConnect(const char *streamid)
{
    sock = srt_create_socket();
    if(sock == SRT_ERROR){
        fprintf(stderr,"srt_socket :%s\n",srt_getlasterror_str());
        return false;
    }

    const int32_t no = 0;
    srt_setsockopt(sock, 0, SRTO_SNDSYN, &no, sizeof no); // for async write
    srt_setsockopt(sock, 0, SRTO_RCVSYN, &no, sizeof no);
    if (srt_setsockopt(sock, 0, SRTO_STREAMID, streamid, strlen(streamid)) < 0) {
        fprintf(stderr,"[%p]CSLSRelay::open, srt_setsockopt SRTO_STREAMID failure. err=%s.\n",this, srt_getlasterror_str());
        return false;
    }

    const int32_t file_mode = SRTT_LIVE;   //SRTT_FILE;
    srt_setsockflag(sock, SRTO_TRANSTYPE, &file_mode, sizeof(file_mode));

    addrinfo hints, *res;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_socktype = SOCK_DGRAM; //SOCK_STREAM;
    hints.ai_family = AF_INET;
    struct sockaddr_in sa;
    sa.sin_port = htons(port);
    sa.sin_family = AF_INET; //AF_UNSPEC;
    getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &res);
#ifdef _WIN32
    if (inetPton(AF_INET, ip.c_str(), &sa.sin_addr) != 1) {
        return false;
    }
#else
    if (inet_pton(AF_INET,ip.c_str(), &sa.sin_addr) != 1) {
        return false;
    }
#endif

       //srt_connect
    int32_t st = srt_connect(sock, (struct sockaddr*) &sa, sizeof sa);
    SRT_SOCKSTATUS status = srt_getsockstate(sock);
    fprintf(stdout,"srt connect status===%d\n", status);
    if (st == SRT_ERROR) {
        fprintf(stderr,"srt_connect: %s\n", srt_getlasterror_str());
        return false;
    }

    eid = srt_epoll_create();
    if (eid < 0) {
        printf("work, srt_epoll_create failed.\n");
        return false;
    }

       //compatible with srt v1.4.0 when container is empty.
    srt_epoll_set(eid, SRT_EPOLL_ENABLE_EMPTY);
    const int32_t modes = SRT_EPOLL_IN | SRT_EPOLL_OUT | SRT_EPOLL_ERR;
    const int32_t ret = srt_epoll_add_usock(eid, sock, &modes);
    if (ret < 0) {
        fprintf(stderr,"srt_add_to_epoll, srt_epoll_add_usock failed, m_eid=%d, fd=%d, modes=%d\n",eid, sock, modes);
        return false;	//libsrt_neterrno();
    }

    return true;
}

bool SrtWrapImpl::connectServer()
{
    int32_t srtRet = 0;
    for (int32_t i = 0; i < 500; i++) {
        srtRet = srt_getsockstate(sock);
        if (srtRet == SRTS_CONNECTED) {
            return true;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(1000));
    }

    return false;
}

bool SrtWrapImpl::receive(uint8_t *&buf, int &size)
{
    if (srt_getsockstate(sock) != SRTS_CONNECTED) {
        return false;
    }

    buf = nullptr;
    size = 0;
    SRTSOCKET read_socks[1];
    SRTSOCKET write_socks[1];
    int32_t read_len = 1;
    int32_t write_len = 0;

    int32_t ret = srt_epoll_wait(eid, read_socks, &read_len, write_socks, &write_len, POLLING_TIME, 0, 0, 0, 0);
    if (ret < 2) {
        //fprintf(stderr,"srt_epoll failure, n=%s.\n",srt_getlasterror_str());
        return true;	//ERROR_SRT_EpollSelectFailure;
    }

    if (0 >= read_socks[0]) {
        //        fprintf(stderr,"srt_reader failure, n=%s.\n", srt_getlasterror_str());
        return true;	//ERROR_SRT_ReadSocket;
    }

       //read data
    const int32_t recv_result = srt_recvmsg(sock, (char*)(buf), TS_UDP_LEN);
    if (ret == SRT_ERROR) {
        if (srt_getsockstate(sock) == SRTS_CONNECTED) {
            return true;
        }

           //        fprintf(stderr,"read_data_handler, srt_read failure, errno=%d...err=%s",
           //            srt_getlasterror(nullptr),srt_getlasterror_str());
        return false;
    }

    return true;
}

bool SrtWrapImpl::publish(const uint8_t *buf, int size)
{
    if (srt_getsockstate(sock) != SRTS_CONNECTED) {
        return false;
    }

    buf = nullptr;
    size = 0;
    SRTSOCKET read_socks[1];
    SRTSOCKET write_socks[1];
    int32_t read_len = 0;
    int32_t write_len = 1;

    int32_t ret = srt_epoll_wait(eid, read_socks, &read_len, write_socks,&write_len, POLLING_TIME, 0, 0, 0, 0);
    if (0 > ret) {
        return true;
    }
    if (0 >= write_socks[0]) {
        //fprintf(stderr,"srt_write failure, n=%s.\n", yang_srt_getlasterror_str());
        return true;	//ERROR_SRT_WriteSocket;
    }

       //write data
    const int32_t send_result = srt_sendmsg(sock, (char*)(buf), size, -1, 0);
    if (send_result == SRT_ERROR) {
        //        fprintf(stderr,"srt_write failure, n=%d.\n", send_result);
        return false;
    }

    return true;
}

SrtWrap::SrtWrap(const char *ip,int port)
{
    impl = new SrtWrapImpl(ip,port);
}

SrtWrap::~SrtWrap()
{
    delete impl;
}

