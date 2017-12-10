#include <stdlib.h>
#include <pthread.h>
#include "binary_tree.h"

int insert(struct binary_tree *bt, unsigned long target);
struct binary_tree_node *search(struct binary_tree *bt, unsigned long target);
struct binary_tree_node *delete(struct binary_tree *bt, unsigned long target);
struct binary_tree_node *rebalance(struct binary_tree *bt, struct binary_tree_node *ptr);
struct binary_tree_node *rotate_right(struct binary_tree_node *ptr);
struct binary_tree_node *rotate_left(struct binary_tree_node *ptr);
void recursive_free_node(binary_tree_node *ptr);

struct binary_tree initialize_binary_tree()
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

struct binary_tree_node *search(struct binary_tree *bt, unsigned long target)
{
	struct binary_tree_node *ptr;
	ptr = root;
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

struct binary_tree_node *rebalance(struct binary_tree *bt, struct binary_tree_node *ptr)
{
	struct binary_tree_node *big;
	for (; ptr != bt->root; ptr = ptr->parent) {
		if (unbalanced(ptr->left->weight, ptr->right->weight)) {
			if (double_rotate(ptr->left->right->weight, ptr->left->left->weight))
				ptr->left = rotate_left(ptr->left);
			ptr = rotate_right(ptr);
		} else if (unbalanced(ptr->right->weight, ptr->left->weight)) {
			if (double_rotate(ptr->right->left->weight, ptr->right->right->weight))
				ptr->right = rotate_right(ptr->right);
			ptr = rotate_left(ptr);
		}
	}
struct binary_tree_node *rotate_right(struct binary_tree_node *ptr);
struct binary_tree_node *rotate_left(struct binary_tree_node *ptr);
void recursive_free_node(binary_tree_node *ptr);
