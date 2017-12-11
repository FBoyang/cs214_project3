#ifndef _BINARY_TREE_H
#define _BINARY_TREE_H

#include "csv.h"
#include "mergesort.h"

struct binary_tree_node {
	unsigned long key;
	struct csv *value;
	struct binary_tree_node *parent;
	struct binary_tree_node *left;
	struct binary_tree_node *right;
	unsigned long weight;
};

struct binary_tree {
	struct binary_tree_node *root;
	pthread_mutex_t mutex;
};

struct binary_tree *initialize_binary_tree();
unsigned long new_session(struct binary_tree *bt);
void append_file(struct binary_tree *bt, char *new_file, unsigned long sid);
char *get_output(struct binary_tree *bt, unsigned long sid);
void free_binary_tree(struct binary_tree *bt);

#endif
