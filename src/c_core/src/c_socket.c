#include "c_socket.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>

int c_socket_create_tcp()
{
    // AF_INET: IPv4, SOCK_STREAM: TCP协议
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    return fd;
}

int c_socket_set_nonblocking(int fd)
{
    // 获取 fd 的文件状态标志
    int flags = fcntl(fd, F_GETFL, NULL);
    if(flags == -1){
        return -1;
    }

    // 为 fd 加上非阻塞标志
    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1){
        return -1;
    }
    return 0;
}

int c_socket_bind_and_listen(int fd, char* client_ip ,int port)
{
    // 1. 开启端口复用
    int optval = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));


    // 2. 配置服务器地址结构体
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, client_ip, &server_addr.sin_addr); // server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    

    // 3. 绑定地址到 socket
    if(bind(fd, (struct sockaddr*)(&server_addr), sizeof(server_addr)) == -1){
        return - 1;
    }

    // 4. 开始监听，SOMAXCONN 是内核允许的未决连接队列最大长度 (通常为 128 或 4096)
    if(listen(fd, SOMAXCONN) == -1){
        return -1;
    }

    return 0;
}

int c_socket_accept(int listen_fd, char* client_ip, int *client_port)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    // 接受连接
    int client_fd = accept(listen_fd, (struct sockaddr*)(&client_addr), &client_len);
    if(client_fd < 0){
        return -1;
    }

    // 提取客户端 IP 和 端口信息
    if(client_ip != NULL){
        // 将网络字节序的IP转换为字符串
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    }

    if(client_port != NULL){
        // 将网络字节序的端口号转换为主机字符串
        *client_port = ntohs(client_addr.sin_port);
    }

    return client_fd;
}
