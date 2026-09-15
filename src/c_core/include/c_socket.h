#ifndef C_SOCKET_H
#define C_SOCKET_H

#include <arpa/inet.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

    // 创建 TCP 套接字
    // 返回值：成功返回 socket fd (大于0)，失败返回 -1
    int c_socket_create_tcp();

    // 设置文件描述符为非阻塞模式
    // 返回值：成功返回 0，失败返回 -1
    int c_socket_set_nonblocking(int fd);

    // 绑定端口bing监听（同时默认开启端口复用 SO_REUSEADDR）
    // prot：要监听的端口号
    // 返回值：成功返回 0，失败返回 -1
    int c_socket_bind_and_listen(int fd, char* client_ip ,int port);

    // 接受客户端连接
    // listen_fd：监听的 fd
    // client_ip：用于接受客户端 IP 的缓冲区 (建议大小 INET_ADDRSTRLEN)
    // clietn_port：用于接收客户端端口
    // 返回值：成功返回新的 clietn fd，失败返回 -1
    int c_socket_accept(int listen_fd, char* client_ip, int *client_port);



#ifdef __cplusplus
}
#endif


#endif // C_SOCKET_H