#ifndef EPOLL_POLLER_HPP
#define EPOLL_POLLER_HPP

#include "c_epoll.h"
#include <vector>
#include <unistd.h>
#include <stdexcept>

namespace net{

class EpollPoller{

public:
    explicit EpollPoller(int init_event_size = 16);

    ~EpollPoller();

    EpollPoller(const EpollPoller&) = delete;
    EpollPoller& operator=(const EpollPoller&) = delete;

    EpollPoller(EpollPoller&& other) noexcept;
    EpollPoller& operator=(EpollPoller&& other) noexcept;

    // 添加监听的 fd 和 events
    bool addFd(int fd, uint32_t events);

    // 修改已监听fd的事件标记
    bool modFd(int fd, uint32_t events);

    // 移除指定的 fd
    bool delFd(int fd);

    // 等待I/O事件
    int poll(int timeout_ms, std::vector<struct epoll_event>& active_events);


private:
    int epfd_{-1};
    std::vector<struct epoll_event> events_;
};

} // namespace net



#endif // EPOLL_POLLER_HPP