#ifndef __TCP_SERVER__
#define __TCP_SERVER__

#define MAX_USER 200
#define MAX_BUFF 1024
#define WAIT_PORT_LOCK 30
#define EPOLLTIMEOUT 30*1000
#define MAXNO(__A__, __B__) ((__A__ > __B__) ? __A__ : __B__)

typedef struct _connect_user_list {
    char *ip;
    in_port_t port;
    int pid;
    int alive;
    struct _connect_user_list *next;
} USER_LIST;

USER_LIST * userConnect (int,USER_LIST *);
int socket_check(int sock);
int open_server (char*,unsigned int);
void freeUser(USER_LIST *);
#endif
