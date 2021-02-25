all: server

#%.o: %.c
#	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
server: main.o tcp_server.o myEpoll.o
	$(CC) -o $@ $^ $(LDFLAGS) 

clean:
	rm -f *.o $(all)
