CC = gcc
server_sources = buf_store.c mergesort.c readbuf.c server.c 
server_headers = buf_store.h mergesort.h readbuf.h 
client_sources = sorter_client.c
client_headers = sorter_client.h

.PHONY: all
all: server client sorter.tar

server: $(server_sources) $(server_headers)
	gcc -pthread -g -lm -Wall -o server $(server_sources)

client: $(client_sources) $(client_headers)
	gcc -pthread -g -lm -Wall -o client $(client_sources)

sorter.tar: $(server_sources) $(server_headers) $(client_sources) $(client_headers) Makefile readme.pdf
	tar cvf sorter.tar $(server_sources) $(server_headers) $(client_sources) $(client_headers) Makefile readme.pdf

.PHONY: clean
clean:
	rm -vf client server sorter.tar
