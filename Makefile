CC = gcc
CFLAGS = -Wall -Wextra -pthread

all: server client

server: server.c
	$(CC) $(CFLAGS) -o server server.c

client: client.c
	$(CC) $(CFLAGS) -o client client.c

run_server: server
	./server 5500

run_client: client
	./client 127.0.0.1 5500

clean:
	rm -f server client

.PHONY: all run_server run_client clean
