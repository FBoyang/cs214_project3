#ifndef _BINARY_TREE_H
#define _BINARY_TREE_H

#include "csv.h"
#include "mergesort.h"

struct binary_tree_node {
    unsigned int key;
    struct csv *value;
    struct binary_tree_node *left;
    struct binary_tree_node *right;
    unsigned long weight;
};

struct binary_tree {
    struct binary_tree_node *root;
    pthread_mutex_t mutex;
};

struct binary_tree *initialize_binary_tree();
unsigned int new_session(struct binary_tree *bt, char *field_name);
void append_file(struct binary_tree *bt, char *new_file, unsigned int sid);
char *get_output(struct binary_tree *bt, unsigned int sid);
void free_binary_tree(struct binary_tree *bt);

#endif
