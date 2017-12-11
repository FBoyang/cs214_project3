CC = gcc
server_sources = binary_tree.c mergesort.c csv.c server.c 
server_headers = binary_tree.h mergesort.h csv.h 

.PHONY: all
all: server client

server: $(server_sources) $(server_headers)
	gcc -pthread -g -Wall -o server $(server_sources)

client: sorter_client.c sorter_client.h
	gcc -pthread -g -Wall -o client sorter_client.c

.PHONY: clean
clean:
	rm -vf client server 
