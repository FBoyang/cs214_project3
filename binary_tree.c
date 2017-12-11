#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "binary_tree.h"

struct csv *search_and_lock(struct binary_tree *bt, unsigned long target);
struct csv *delete_and_lock(struct binary_tree *bt, unsigned long target);
int insert(struct binary_tree *bt, unsigned long target);
struct binary_tree_node *search(struct binary_tree *bt, unsigned long target);
struct csv *delete(struct binary_tree *bt, unsigned long target);
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

unsigned long new_session(struct binary_tree *bt)
{
	unsigned long sid;
	pthread_mutex_lock(&bt->mutex);
	do {
		sid = random();
	} while (insert(bt, sid));
	pthread_mutex_unlock(&bt->mutex);
	return sid;
}

void append_file(struct binary_tree *bt, char *new_file, unsigned long sid)
{
	struct file_node *fn;
	struct csv *table;
	fn = read_csv(new_file);
	table = search_and_lock(bt, sid);
	append_csv(table, fn, strlen(new_file));
	pthread_mutex_unlock(&table->mutex);
}

char *get_output(struct binary_tree *bt, unsigned long sid)
{
	struct csv *table;
	struct file_node *fn;
	char *output;
	table = delete_and_lock(bt, sid);
	fn = mergesort(table);
	output = print_csv(fn);
	free_csv(table);
	return output;
}

void free_binary_tree(struct binary_tree *bt)
{
	recursive_free_node(bt->root);
	pthread_mutex_destroy(&bt->mutex);
	free(bt);
}

int insert(struct binary_tree *bt, unsigned long target)
{
	int ret;
	struct binary_tree_node *ptr, *par;
	ptr = bt->root;
	par = NULL;
	while (ret == 0 && ptr) {
		par = ptr;
		if (target == ptr->key)
			ret = 1;
		else if (target < ptr->key)
			ptr = ptr->left;
		else
			ptr = ptr->right;
	}
	if (ret == 0) {
		ptr = malloc(sizeof(*ptr));
		ptr->key = target;
		ptr->value = initialize_csv();
		ptr->parent = par;
		ptr->left = NULL;
		ptr->right = NULL;
		ptr->weight = 2L;
		if (target < par->key)
			par->left = ptr;
		else
			par->right = ptr;
		rebalance(bt, par);
	}
	return ret;
}

struct csv *search_and_lock(struct binary_tree *bt, unsigned long target)
{
	struct binary_tree_node *ptr;
	pthread_mutex_lock(&bt->mutex);
	ptr = search(bt, target);
	pthread_mutex_lock(&ptr->value->mutex);
	pthread_mutex_unlock(&bt->mutex);
	return ptr->value;
}

struct csv *delete_and_lock(struct binary_tree *bt, unsigned long target)
{
	struct csv *ret;
	pthread_mutex_lock(&bt->mutex);
	ret = delete(bt, target);
	pthread_mutex_lock(&ret->mutex);
	pthread_mutex_unlock(&bt->mutex);
	return ret;
}

struct binary_tree_node *search(struct binary_tree *bt, unsigned long target)
{
	struct binary_tree_node *ptr;
	ptr = bt->root;
	while (ptr && target != ptr->key) {
		if (target < ptr->key)
			ptr = ptr->left;
		else
			ptr = ptr->right;
	}
	return ptr;
}

struct csv *delete(struct binary_tree *bt, unsigned long target)
{
	struct binary_tree_node *ptr, *par, *tmp;
	struct csv *ret;
	ptr = search(bt, target);
	if (ptr == NULL)
		return NULL;
	par = ptr->parent;
	if (ptr->left == NULL && ptr->right == NULL) {
		if (ptr == par->left)
			par->left = NULL;
		else
			par->right = NULL;
	} else if (ptr->left == NULL) {
		tmp = ptr->right;
		if (ptr == par->left) {
			par->left = tmp;
			tmp->parent = par;
		} else {
			par->right = tmp;
			tmp->parent = par;
		}
	} else if (ptr->right == NULL) {
		tmp = ptr->left;
		if (ptr == par->left) {
			par->left = tmp;
			tmp->parent = par;
		} else {
			par->right = tmp;
			tmp->parent = par;
		}
	} else {
		tmp = ptr->left;
		while (tmp->right)
			tmp = tmp->right;
		par = tmp->parent;
		if (par == ptr)
			par->left = tmp->left;
		else
			par->right = tmp->left;
		ptr->key = tmp->key;
		ptr->value = tmp->value;
	}
	rebalance(bt, par);
	ret = ptr->value;
	free(ptr);
	return ret;
}

void rebalance(struct binary_tree *bt, struct binary_tree_node *ptr)
{
	struct binary_tree_node *child;
	for (; ptr != bt->root; ptr = ptr->parent) {
		if (unbalanced(get_weight(ptr->left), get_weight(ptr->right))) {
			if (double_rotate(get_weight(ptr->left->right), get_weight(ptr->left->left)))
				ptr->left = rotate_left(ptr->left);
			ptr = rotate_right(ptr);
		} else if (unbalanced(get_weight(ptr->right), get_weight(ptr->left))) {
			if (double_rotate(get_weight(ptr->right->left), get_weight(ptr->right->right)))
				ptr->right = rotate_right(ptr->right);
			ptr = rotate_left(ptr);
		}
		child = ptr;
	}
	bt->root = child;
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
