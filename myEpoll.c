#include <sys/epoll.h>

static int epollPid = 0;

int setEpollPid(int pid){
    if(pid <= 0) return -1;
    epollPid = pid;
    return 0;
}

int getEpollPid(void){
    return epollPid;
}

int addEpollEvent(int fd){
    struct epoll_event event;

    if(epollPid <= 0)return -2;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    return epoll_ctl (epollPid, EPOLL_CTL_ADD, fd, &event);
}

int delEpollEvent(int fd){
    struct epoll_event event;

    if(epollPid <= 0)return -2;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    return epoll_ctl (epollPid, EPOLL_CTL_DEL, fd, &event);
}
