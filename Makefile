CC = gcc
server_sources = buf_store.c mergesort.c readbuf.c server.c 
server_headers = buf_store.h mergesort.h readbuf.h 

.PHONY: all
all: server client

server: $(server_sources) $(server_headers)
	gcc -pthread -g -Wall -o server $(server_sources)

client: sorter_client.c sorter_client.h
	gcc -pthread -g -Wall -o client sorter_client.c

.PHONY: clean
clean:
	rm -vf client server 
