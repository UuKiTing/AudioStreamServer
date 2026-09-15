#include "TcpConnection.hpp"
#include "Socket.hpp"
#include "EpollPoller.hpp"
#include "EventLoop.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <cerrno>

namespace net{
    TcpConnection::TcpConnection(EventLoop* loop, Socket socket, const std::string& client_ip, int client_port)
        : loop_(loop),
          socket_(std::move(socket)),
          client_ip_(client_ip),
          client_port_(client_port){

        auto func = [this](uint32_t events){
            handleEvent(events);
        };

        if(!loop_->addFd(socket_.fd(), EPOLLIN | EPOLLRDHUP, func)){
        }
    }

    TcpConnection::~TcpConnection() {
        std::cout << "[TcpConnection] Destroyed | fd: " << socket_.fd() << std::endl;
    }

    int TcpConnection::getFd() const {
        return socket_.fd();
    }

    const std::string& TcpConnection::getIp() const {
        return client_ip_;
    }

    int TcpConnection::getPort() const {
        return client_port_;
    }

    bool TcpConnection::isConnected() const {
        return state_ == State::Connected;
    }

    void TcpConnection::connectEstablished() {
        setState(State::Connected);
    }

    void TcpConnection::setConnectionCallback(const ConnectionCallback& callBack) {
        connectionCallback_ = callBack;
    }

    void TcpConnection::setMessageCallback(const MessageCallback& callBack) {
        messageCallback_ = callBack;
    }

    void TcpConnection::setCloseCallback(const CloseCallback& callBack) {
        closeCallback_ = callBack;
    }


    void TcpConnection::send(const std::string& msg) {
        send(msg.data(), msg.size());
    }

    void TcpConnection::send(const char* data, size_t len) {
        if(state_ == State::Disconnected){
            return;
        }

        ssize_t n = 0;
        size_t remaining = len;

        if(outputBuffer_.readableBytes() == 0){ // 如果outputBuffer_为空，直接send，避免写入outputBuffer_
            n = ::send(socket_.fd(), data, len, MSG_NOSIGNAL);
            if(n >= 0){
                remaining = len - n;
            }
            else{
                n = 0;
                if(errno != EWOULDBLOCK && errno != EAGAIN){ // EWOULDBLOCK/EAGAIN表示：资源暂时不可用，请稍后重试
                    handleClose();
                    return;
                }
            }
        }
        
        if(remaining > 0){
            outputBuffer_.append(data + n, remaining);
            
            loop_->modFd(socket_.fd(), EPOLLIN | EPOLLOUT);
        }
    }


    void TcpConnection::shutdown() {
        if(state_ == State::Connected || state_ == State::Disconnecting){
            setState(State::Disconnecting);

            shutdownWriteEnd();
        }
    }

    void TcpConnection::setHttpParser(const std::any parser) {
        parser_ = parser;
    }

    std::any* TcpConnection::getHttpParser() {
        return &parser_;
    }


    void TcpConnection::setState(State s) {
        state_ = s;
    }

    void TcpConnection::handleRead() {
        int savedErrno = 0;
        ssize_t n = inputBuffer_.readDataInFd(socket_.fd(), &savedErrno);
        
        if(n > 0){
            if(messageCallback_){
                messageCallback_(shared_from_this(), &inputBuffer_);
            }
        }
        else if(n == 0){
            handleClose();
        }
        else{
            std::cerr << "[TcpConnection::handleRead] 读取失败！, errno: " << savedErrno << std::endl;     
        }   
    }

    void TcpConnection::handleWrite() {
        if(outputBuffer_.readableBytes() > 0){
            ssize_t n = ::send(socket_.fd(), outputBuffer_.peek(), outputBuffer_.readableBytes(), MSG_NOSIGNAL);

            if(n > 0){
                outputBuffer_.retrieve(n);
            
                if(outputBuffer_.readableBytes() == 0){ // 数据发送完毕后，输出列表为空
                    if(state_ == State::Disconnecting){ // 如果处于正在断开连接状态，那就关闭写端
                        shutdownWriteEnd();
                    }
                    else{ // 否则不关闭写端，取消写事件
                        loop_->modFd(socket_.fd(), EPOLLIN);
                    }
                }
            }
            else if(errno != EWOULDBLOCK && errno != EAGAIN){
                std::cerr << "TcpConnection::handleWrite error" << std::endl;
            }
        }
    }

    void TcpConnection::handleClose() {
        setState(State::Disconnected);
        loop_->delFd(socket_.fd());

        if(closeCallback_){
            closeCallback_(shared_from_this());
        }
    }

    void TcpConnection::shutdownWriteEnd() {
        if(outputBuffer_.readableBytes() > 0){ // 如果输出队列还有数据，那就设置EPOLLOUT事件，赶紧发送出去
            loop_->modFd(socket_.fd(), EPOLLIN | EPOLLOUT);
            return;
        }

        if(!writeClosed_){
            writeClosed_ = true;

            ::shutdown(socket_.fd(), SHUT_WR);

            loop_->modFd(socket_.fd(), EPOLLIN);
        }
    }


    void TcpConnection::handleEvent(uint32_t events) {
        TcpConnectionPtr guard = shared_from_this();

        // 纯断开事件
        if(events & EPOLLHUP && ( events & EPOLLIN)){
            handleClose();
            return;
        }

        // 错误
        if(events & EPOLLERR){
            handleClose();
            return;
        }

        // 可读
        if(events & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)){
            handleRead();
            if (state_ == State::Disconnected) return;
        }

        // 可写
        if(events & EPOLLOUT){
            handleWrite();
        }
    }
} // namespace net