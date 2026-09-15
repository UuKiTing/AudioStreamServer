#ifndef ACCEPTOR_HPP
#define ACCEPTOR_HPP

#include "Socket.hpp"
#include <functional>

namespace net{

class EventLoop;

class Acceptor{
public:
    using NewConnectionCallback = std::function<void(Socket client_soc, const std::string& ip, int port)>;

    Acceptor(EventLoop *loop, std::string ip, int port);
    ~Acceptor();

    // 设置accept成功后的回调函数
    void setNewConnectionCallback(const NewConnectionCallback& callBack);

    // 监听新连接的到来
    void listen();

    // 处理新连接
    void handle();


private:
    Socket server_soc;
    EventLoop* loop_;
    NewConnectionCallback newConnectionCallback_;
    bool listening_ {false};
};

}; // namespace net


#endif // ACCEPTOR_HPP