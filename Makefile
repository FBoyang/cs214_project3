CC = gcc
server_sources = binary_tree.c mergesort.c csv.c server.c
server_headers = binary_tree.h mergesort.h csv.h binary_tree_internal.h

.PHONY: all
all: server client binary_tree_test

server: $(server_sources) $(server_headers)
	gcc -pthread -g -Wall -o server $(server_sources)

client: sorter_client.c sorter_client.h
	gcc -pthread -g -Wall -o client sorter_client.c

binary_tree_test: binary_tree_test.c binary_tree.c csv.c mergesort.c binary_tree.h binary_tree_internal.h csv.h
	gcc -g -Wall -o binary_tree_test binary_tree_test.c binary_tree.c csv.c mergesort.c

.PHONY: clean
clean:
	rm -vf client server binary_tree_test
