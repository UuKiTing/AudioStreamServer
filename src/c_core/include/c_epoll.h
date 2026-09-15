#ifndef C_EPOLL_H
#define C_EPOLL_H

#include <sys/epoll.h>

#ifdef __cplusplus
extern "C"{
#endif

    // 创建 epoll 句柄
    // 返回值：成功返回 epoll fd > 0，失败返回 -1
    int c_epoll_create();

    // 添加监听的 socket 和 事件
    int c_epoll_add(int epfd, int fd, uint32_t events);

    // 修改已监听 fd 的事件标记
    int c_epoll_mod(int epfd, int fd, uint32_t events);

    // 移除指定的 fd
    int c_epoll_del(int epfd, int fd);

    // 等待就绪的 IO 事件
    // events: 用于接收就绪事件的数组指针
    // maxevents: 数组的最大容量
    // timeout_ms: 超时时间 (毫秒)，-1 表示无限期等待
    int c_epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout_ms);


#ifdef __cplusplus
}
#endif

#endif // C_EPOLL_H