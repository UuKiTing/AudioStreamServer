#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include "EpollPoller.hpp"
#include "Socket.hpp"
#include <unordered_map>
#include <memory>
#include <unistd.h>
#include <functional>

namespace net{

class TcpConnection;

class EventLoop{
public:
    using EventCallback = std::function<void(uint32_t)>; // 回调函数

    EventLoop();
    ~EventLoop();

    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;

    // 开启事件循环
    void loop();

    // 退出事件循环
    void quit();
    
    bool addFd(int fd, uint32_t events, EventCallback callBack);
    bool modFd(int fd, uint32_t events);
    bool delFd(int fd);

private:
    bool quit_{false}; // 是否退出事件循环

    std::unique_ptr<EpollPoller> poller_;
    std::unordered_map<int, EventCallback> callbacks_;
};

}; // namepsace net



#endif //EVENT_LOOP_HPP