#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#include "debug.h"
#include "tcp_server.h"
#include "myEpoll.h" 

static USER_LIST *userList = NULL; 

void setUserError(int pid){
    USER_LIST *now = NULL;

    if(pid <= 0)return;
    for(now = userList;now != NULL;now = now->next){
	if(now->pid == pid)
	    now->alive = 0;
    }
}

void userEcho(int userPid){
    char data[MAX_BUFF];
    int dataLength = 0;

    dataLength = read(userPid, data, sizeof(data));
    if(dataLength > 0){
	_DEBUG_MSG("(%d) > %s",userPid,data);    
        if(!socket_check(userPid)){
	    setUserError(userPid);
	    return;
        }
	write(userPid,data,dataLength); 
    }
}

void delUser(void){
    USER_LIST *now = NULL;
    USER_LIST *previous = NULL;

    now = userList;
    while(now != NULL){
	if(now->alive == 0){
	    if(previous != NULL ){
	        previous->next = now->next;
		freeUser(now);
	        now = previous->next;
	    }else{
		userList = now->next;
		freeUser(now);
		now = userList;
	    } 
	}else{
	    previous = now;
	    now = now->next;
	}
    }
}

void checkUserAlive(void){
    USER_LIST *now = NULL;

    if(userList == NULL)return;

    for(now = userList;now != NULL;now = now->next){
	if(now->pid <= 0 ||  now->alive == 0 || !socket_check(now->pid))
	    now->alive = 0;
    }
}

int main( int argc, char *argv[] ){

    int serverPid = 0,dataLength = 0;
    struct epoll_event *events;
    int efd = 0;
    int maxEventCount = 0, eventIndex = 0;

    if(argc < 2){
        _DEBUG_MSG("please,input the port number !!");
	goto end;
    }
    if(atoi(argv[1]) < 1 || atoi(argv[1]) > 65535){
        _DEBUG_MSG("please,input the correct port number(1~65535) !!");
	goto end;
    }

    if( (serverPid = open_server(argv[1],MAX_USER)) < 0 ){
        _DEBUG_MSG("Socket open error!!");
	goto end;
    }

    if ( (efd = epoll_create(MAX_USER)) < 0){
	_DEBUG_MSG("epoll_create fail");
	goto end;
    }

    if(setEpollPid(efd) < 0){
	_DEBUG_MSG("setEpollPid fail");
	goto end;
    }

    if(addEpollEvent(serverPid) < 0){
	_DEBUG_MSG("addEpollEvent fail");
	goto end;
    }

    events = (struct epoll_event *) calloc(MAX_USER, sizeof(struct epoll_event));

    _DEBUG_MSG("Main Loop Start...");

    while(1){

	checkUserAlive();
	
	delUser();

 	//maxEventCount = epoll_wait(efd, events, MAX_USER, EPOLLTIMEOUT);
 	maxEventCount = epoll_wait(efd, events, MAX_USER, -1);

	for(eventIndex = 0; eventIndex < maxEventCount;eventIndex++){
	   if( (events[eventIndex].events & EPOLLERR) || 
	       (events[eventIndex].events & EPOLLHUP) ||
	       (!(events[eventIndex].events & EPOLLIN)) ){
		_DEBUG_MSG("epoll event(%d pid : %d) fail",eventIndex,events[eventIndex].data.fd);
		setUserError(events[eventIndex].data.fd);
		continue;	
	    }else if(events[eventIndex].data.fd == serverPid)
	    	userList = userConnect(serverPid,userList);
	    else
	    	userEcho(events[eventIndex].data.fd);
	}
    }
end:
    if(events)free(events);
    if(serverPid > 0)close(serverPid);

    return 0;
}
