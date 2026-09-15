#include "c_epoll.h"
#include <unistd.h>

int c_epoll_create() {
    return epoll_create1(EPOLL_CLOEXEC);
}

int c_epoll_add(int epfd, int fd, uint32_t events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    
    return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
}

int c_epoll_mod(int epfd, int fd, uint32_t events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
}

int c_epoll_del(int epfd, int fd) {
    return epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
}

int c_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout_ms) {
    return epoll_wait(epfd, events, maxevents, timeout_ms);
}
