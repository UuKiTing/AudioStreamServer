#include "EpollPoller.hpp"
#include <iostream>

namespace net{
    
EpollPoller::EpollPoller(int init_event_size): events_(init_event_size) {
    epfd_ = c_epoll_create();
    if(epfd_ < 0){
        throw std::runtime_error("Failed to create epoll instance");
    }
}

EpollPoller::~EpollPoller() {
    if(epfd_ > 0){
        ::close(epfd_);
        epfd_ = -1;
    }
}

EpollPoller::EpollPoller(EpollPoller &&other) noexcept : epfd_(other.epfd_), events_(std::move(other.events_)){
    other.epfd_ = -1;
}

bool EpollPoller::addFd(int fd, uint32_t events) {
    if(c_epoll_add(epfd_, fd, events) < 0){
        std::cerr << "[EpollPoller::addFd] 添加fd=" << fd << "到epoll失败！\n";
        return false;
    }   
    return true;
}

bool EpollPoller::modFd(int fd, uint32_t events){
    if(c_epoll_mod(epfd_, fd, events) < 0){
        std::cerr << "[EpollPoller::modFd] 修改epoll中的fd=" << fd << "失败！\n";
        return false;
    }
    return true;
}

bool EpollPoller::delFd(int fd) {
    
    if(c_epoll_del(epfd_, fd) < 0){
        std::cerr << "[EpollPoller::delFd] 删除epoll中的fd=" << fd << "失败！\n";
        return false;
    }
    return true;
}

int EpollPoller::poll(int timeout_ms, std::vector<struct epoll_event>& active_events) {
    int nfds = c_epoll_wait(epfd_, events_.data(), static_cast<int>(events_.size()), timeout_ms);
    if(nfds < 0){
        return -1;
    }

    active_events.clear();
    
    for (int i = 0; i < nfds; i++){
        active_events.push_back(events_[i]);
    }
    
    // 如果就绪事件的数量等于active_events的大小就扩容为原来的2倍
    if(nfds == static_cast<int>(events_.size())){
        events_.resize(events_.size() * 2);
    }

    return nfds;
}


} // namespace net