
all:
	gcc -o tcp_server tcp_server.c -lpthread
	gcc -o tcp_client tcp_client.c

clean:
	rm -rf tcp_server tcp_client
	
