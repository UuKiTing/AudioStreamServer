#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP

#include "Socket.hpp"
#include "Buffer.hpp"
#include "HttpParser.hpp"
#include <memory>
#include <string>
#include <functional>
#include <any>

namespace net{

class EpollPoller;
class EventLoop;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>{
public:
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
    using RequestCallback = std::function<void(const TcpConnectionPtr&, Buffer*)>;
    using CloseCallback = std::function<void(const TcpConnectionPtr&)>;

    TcpConnection(EventLoop* loop, Socket socket, const std::string& client_ip, int client_port);
    ~TcpConnection();

    int getFd() const; // 获取底层文件描述符
    const std::string& getIp() const; // 获取客户端 ip
    int getPort() const; // 获取客户端 port

    bool isConnected() const; // 判断当前连接是否处于活跃状态

    void connectEstablished(); // 建立连接

    // 设置回调函数
    void setConnectionCallback(const ConnectionCallback& callBack);
    void setRequestCallback(const RequestCallback& callBack);
    void setCloseCallback(const CloseCallback& callBack);
    
    // 向socket_发送数据
    void send(const std::string& msg);
    void send(const char* data, size_t len);

    // 事件分发函数
    void handleEvent(uint32_t events);

    // 关闭
    void shutdown();

    void setHttpParser(http::HttpParser *parser);

    http::HttpParser* getHttpParser();


private:
    enum class State{Connecting, Connected, Disconnecting, Disconnected};
    void setState(State s); // 设置连接状态

    void handleRead(); // 读处理
    void handleWrite(); // 写处理
    void handleClose(); // 关闭处理

    void shutdownWriteEnd(); // 关闭写端

    Socket socket_;
    EventLoop* loop_;
    std::string client_ip_;
    int client_port_;

    State state_{State::Connecting};

    Buffer inputBuffer_;
    Buffer outputBuffer_;

    bool writeClosed_{false};

    ConnectionCallback connectionCallback_;
    RequestCallback requestCallback_;
    CloseCallback closeCallback_; 

    http::HttpParser *parser_;    
};

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    
} // namespace net


#endif // TCP_CONNECTION_HPP