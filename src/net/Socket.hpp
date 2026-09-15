#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "c_socket.h"
#include <string>
#include <stdexcept>
#include <unistd.h>

namespace net{

class Socket{

public:
    // 自动创建一个全新的 TCP 套接字
    Socket();

    // 包装一个现有的 原始fd
    explicit Socket(int fd);

    // RAII核心：析构时自动关闭 socket句柄
    ~Socket(); 

    // 禁用拷贝构造和拷贝赋值
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    // 允许移动构造和移动赋值
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    // 获取裸 fd
    int fd() const;

    // 设置非阻塞
    bool setNonBlocking();

    // 绑定和监听端口
    bool bindAndListen(std::string& client_ip, int port);

    // 接受连接
    int accept(std::string& client_ip, int& client_port);

    // 判断 fd 是否有效
    bool isVaild();


private:
    int fd_{-1};

};



} // namespace net

#endif // SOCKET_HPP