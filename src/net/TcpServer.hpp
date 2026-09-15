#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include "EventLoop.hpp"
#include "Socket.hpp"
#include "TcpConnection.hpp"
#include "Acceptor.hpp"
#include <functional>
#include <iostream>

namespace net{

class TcpServer{
public:
    using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
    using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer*)>;


    TcpServer(EventLoop* loop, const std::string& ip, uint16_t port, const std::string& name = "TcpServer");
    ~TcpServer();

    // 启动服务器
    void start();

    // 设置连接成功后的回调函数
    void setConnectionCallback(ConnectionCallback callBack);

    // 设置回复对端的回调函数
    void setMessageCallback(MessageCallback callBack);


private:
    void newConnection(Socket client_soc, const std::string& ip, int port);
    void closeConnection(const TcpConnectionPtr& c); // 关闭客户端连接


    using ConnectionMap = std::unordered_map<int, net::TcpConnectionPtr>;

    EventLoop* loop_;
    std::unique_ptr<Acceptor> acceptor_;  
    ConnectionMap connections_;

    MessageCallback messageCallback_;
    ConnectionCallback connectionCallback_;

};

}; // namespace net





#endif // TCP_SERVER_HPP