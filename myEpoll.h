#ifndef __EPOLL__
#define __EPOLL__

int setEpollPid(int);
int getEpollPid(void);
int addEpollEvent(int);
int delEpollEvent(int);
#endif
