#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "binary_tree.h"

struct csv *search_and_lock(struct binary_tree *bt, unsigned int target);
struct csv *delete_and_lock(struct binary_tree *bt, unsigned int target);
int insert(struct binary_tree *bt, unsigned int target, char *field_name);
struct binary_tree_node *recursive_insert(struct binary_tree_node *root, struct binary_tree_node *ptr)
struct binary_tree_node *search(struct binary_tree_node *root, unsigned int target);
struct csv *delete(struct binary_tree *bt, unsigned int target);
void rebalance(struct binary_tree *bt, struct binary_tree_node *ptr);
struct binary_tree_node *rotate_right(struct binary_tree_node *ptr);
struct binary_tree_node *rotate_left(struct binary_tree_node *ptr);
void recursive_free_node(struct binary_tree_node *ptr);
int unbalanced(unsigned long w1, unsigned long w2);
int double_rotate(unsigned long w1, unsigned long w2);
unsigned long get_weight(struct binary_tree_node *ptr);

struct binary_tree *initialize_binary_tree()
{
    struct binary_tree *bt;
    bt = malloc(sizeof(*bt));
    bt->root = NULL;
    pthread_mutex_init(&bt->mutex, NULL);
    return bt;
}

unsigned int new_session(struct binary_tree *bt, char *field_name)
{
    unsigned int sid;
    pthread_mutex_lock(&bt->mutex);
    do {
        sid = random();
    } while (insert(bt, sid, field_name));
    pthread_mutex_unlock(&bt->mutex);
    return sid;
}

void append_file(struct binary_tree *bt, char *new_file, unsigned int sid)
{
    struct file_node *fn;
    struct csv *table;
    fn = read_csv(new_file);
    table = search_and_lock(bt, sid);
    append_csv(table, fn, strlen(new_file));
    pthread_mutex_unlock(&table->mutex);
}

char *get_output(struct binary_tree *bt, unsigned int sid)
{
    struct csv *table;
    char *output;
    table = delete_and_lock(bt, sid);
    mergesort(table);
    output = print_csv(table);
    free_csv(table);
    return output;
}

void free_binary_tree(struct binary_tree *bt)
{
    recursive_free_node(bt->root);
    pthread_mutex_destroy(&bt->mutex);
    free(bt);
}

int insert(struct binary_tree *bt, unsigned int target, char *field_name)
{
    struct binary_tree_node *ptr, *newptr;
    ptr = malloc(sizeof(*ptr));
    ptr->key = target;
    ptr->value = initialize_csv(field_name);
    ptr->left = NULL;
    ptr->right = NULL;
    ptr->weight = 2L;
    newptr = recursive_insert(bt->root, ptr);
    if (newptr)
        bt->root = newptr;
    else
        free(ptr);
    return (ptr == NULL);
}

struct binary_tree_node *recursive_insert(struct binary_tree_node *root, struct binary_tree_node *ptr)
{
    struct binary_tree_node *newptr;
    if (root == NULL) {
        newptr = ptr;
    } else if (ptr->key == root->key) {
        newptr = NULL;
    } else if (ptr->key < root->key) {
        if ((newptr = recursive_insert(root->left, ptr)))
            root->left = newptr;
    } else {
        if ((newptr = recursive_insert(root->right, ptr)))
            root->right = newptr;
    }
    rebalance(newptr);
    return newptr;
}

struct csv *search_and_lock(struct binary_tree *bt, unsigned int target)
{
    struct binary_tree_node *ptr;
    pthread_mutex_lock(&bt->mutex);
    ptr = search(bt->root, target);
    if (ptr)
        pthread_mutex_lock(&ptr->value->mutex);
    pthread_mutex_unlock(&bt->mutex);
    return ptr ? ptr->value : NULL;
}

struct csv *delete_and_lock(struct binary_tree *bt, unsigned int target)
{
    struct csv *ptr;
    pthread_mutex_lock(&bt->mutex);
    ptr = delete(bt, target);
    if (ptr)
        pthread_mutex_lock(&ptr->mutex);
    pthread_mutex_unlock(&bt->mutex);
    return ptr;
}

struct binary_tree_node *search(struct binary_tree_node *root, unsigned int target)
{
    struct binary_tree_node *ret;
    if (root == NULL)
        ret = NULL;
    else if (target == root->key)
        ret = root;
    else if (target < root->key)
        ret = search(root->left, target);
    else
        ret = search(root->right, target);
    return NULL;
}

struct csv *delete(struct binary_tree *bt, unsigned int target)
{
    struct binary_tree_node *ptr, *par, *tmp;
    struct csv *ret;
}

void rebalance(struct binary_tree_node *ptr)
{
    struct binary_tree_node *ret;
    if (ptr == NULL) {
        ret = NULL;
    } else if (unbalanced(get_weight(ptr->left), get_weight(ptr->right))) {
        if (double_rotate(get_weight(ptr->left->right), get_weight(ptr->left->left)))
            ptr->left = rotate_left(ptr->left);
        ret = rotate_right(ptr);
    } else if (unbalanced(get_weight(ptr->right), get_weight(ptr->left))) {
        if (double_rotate(get_weight(ptr->right->left), get_weight(ptr->right->right)))
            ptr->right = rotate_right(ptr->right);
        ret = rotate_left(ptr);
    } else {
        ret = ptr;
    }
    return ret;
}

struct binary_tree_node *rotate_right(struct binary_tree_node *ptr)
{
    struct binary_tree_node *child;
    child = ptr->left;
    ptr->left = child->right;
    ptr->left->parent = ptr;
    child->right = ptr;
    ptr->parent = child;
    ptr->weight = get_weight(ptr->left) + get_weight(ptr->right);
    child->weight = get_weight(child->left) + get_weight(child->right);
    return child;
}

struct binary_tree_node *rotate_left(struct binary_tree_node *ptr)
{
    struct binary_tree_node *child;
    child = ptr->right;
    ptr->right = child->left;
    ptr->right->parent = ptr;
    child->left = ptr;
    ptr->parent = child;
    ptr->weight = get_weight(ptr->left) + get_weight(ptr->right);
    child->weight = get_weight(child->left) + get_weight(child->right);
    return child;
}

void recursive_free_node(struct binary_tree_node *ptr)
{
    if (ptr) {
        recursive_free_node(ptr->left);
        recursive_free_node(ptr->right);
        free_csv(ptr->value);
        free(ptr);
    }
}

int unbalanced(unsigned long a, unsigned long b)
{
    return (a * a > b * (b + 2*a));
}

int double_rotate(unsigned long a, unsigned long b)
{
    return (a * a > 2 * b * b);
}

unsigned long get_weight(struct binary_tree_node *ptr)
{
    return (ptr ? ptr->weight : 1);
}
