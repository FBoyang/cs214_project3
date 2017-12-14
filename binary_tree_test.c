#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "binary_tree.h"
#include "binary_tree_internal.h"

void check_order(struct binary_tree *bt);
void inorder(struct binary_tree_node *ptr, unsigned int *keys, unsigned int *counter);
void check_balance(struct binary_tree *bt);
void recursive_check_balance(struct binary_tree_node *ptr);

int main(int argc, char **argv)
{
    FILE *infile;
    int c;
    char *line;
    size_t length;
    struct binary_tree *bt;
    struct csv *table;
    int target;
    infile = NULL;
    while ((c = getopt(argc, argv, "i:")) != -1) {
        switch (c) {
        case 'i':
            if ((infile = fopen(optarg, "r")) == NULL)
                perror("failed to open file for reading");
        }
    }
    if (infile == NULL) {
        fputs("please provide an input file\n", stderr);
        return 1;
    }
    bt = initialize_binary_tree();
    line = NULL;
    length = 0;
    while (getline(&line, &length, infile) != -1) {
        if (strncmp(line, "insert", 6) == 0 && sscanf(line, "insert %u\n", &target) == 1) {
            insert(bt, target, "movie_title");
        } else if (strncmp(line, "delete", 6) == 0 && sscanf(line, "delete %u\n", &target) == 1) {
            table = delete(bt, target);
            free_csv(table);
        }
        check_order(bt);
        check_balance(bt);
    }
    fclose(infile);
    free(line);
    free_binary_tree(bt);
    return 0;
}

void check_order(struct binary_tree *bt)
{
    unsigned long size;
    unsigned int *keys, counter, i;
    size = get_weight(bt->root) - 1;
    keys = malloc(size * sizeof(*keys));
    counter = 0;
    inorder(bt->root, keys, &counter);
    for (i = 0; i < size; i++) {
        fprintf(stdout, "%u ", keys[i]);
    }
    fputs("\n", stdout);
    free(keys);
}

void inorder(struct binary_tree_node *ptr, unsigned int *keys, unsigned int *counter)
{
    if (ptr == NULL)
        return;
    inorder(ptr->left, keys, counter);
    keys[*counter] = ptr->key;
    *counter += 1;
    inorder(ptr->right, keys, counter);
}

void check_balance(struct binary_tree *bt)
{
    recursive_check_balance(bt->root);
}

void recursive_check_balance(struct binary_tree_node *ptr)
{
    if (ptr == NULL) {
        return;
    } else if (ptr->weight != get_weight(ptr->left) + get_weight(ptr->right)) {
        fprintf(stdout, "node %u has incorrect weight\n", ptr->key);
    } else if (unbalanced(get_weight(ptr->left), get_weight(ptr->right)) ||
            unbalanced(get_weight(ptr->right), get_weight(ptr->left))) {
        fprintf(stdout, "node %u is unbalanced\n", ptr->key);
    } else {
        recursive_check_balance(ptr->left);
        recursive_check_balance(ptr->right);
    }
}
