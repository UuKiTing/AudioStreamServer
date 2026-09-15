#include "EventLoop.hpp"
#include "EpollPoller.hpp"
#include <iostream>

namespace net{
    EventLoop::EventLoop() : poller_(std::make_unique<EpollPoller>()){
        std::cout << "[EventLoop] 已创建\n";
    }

    EventLoop::~EventLoop() {
        std::cout << "[EventLoop] 已销毁\n";
    }

    void EventLoop::loop() {
        quit_ = false;

        std::cout << "[EventLoop] 开始循环中...\n";

        std::vector<struct epoll_event> activeEvents;

        while(!quit_){
            int n = poller_->poll(2000, activeEvents); // 拿到就绪的fds
            
            if(n > 0){
                for(const auto& event: activeEvents){
                    int fd = event.data.fd;
                    uint32_t revents = event.events;
                    
                    auto it = callbacks_.find(fd);
                    if(it != callbacks_.end()){
                        EventCallback callBack = it->second;
                        
                        try{
                            callBack(revents);
                            
                        }
                        catch(const std::exception& e){
                            std::cerr << "[EventLoop] 回调函数调用失败, fd: " << fd
                                      << ", what: " << e.what() << std::endl;
                        }
                        catch(...){
                            std::cerr << "[EventLoop] 未知的回调执行, fd: "
                                      << fd << std::endl;
                        }
                    }
                }
            }
        }
    }

    void EventLoop::quit() {
        quit_ = true;
    }

    bool EventLoop::addFd(int fd, uint32_t events, EventCallback callBack) {
        callbacks_[fd] = callBack;
        return poller_->addFd(fd, events);
    }

    bool EventLoop::modFd(int fd, uint32_t events) {
        return poller_->modFd(fd, events);
    }

    bool EventLoop::delFd(int fd) {
        callbacks_.erase(fd);
        return poller_->delFd(fd);
    }


} // namespace net