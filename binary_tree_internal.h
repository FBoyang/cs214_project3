#ifndef _BINARY_TREE_INTERNAL_H
#define _BINARY_TREE_INTERNAL_H

int insert(struct binary_tree *bt, unsigned int target, char *field_name);
struct binary_tree_node *search(struct binary_tree_node *root, unsigned int target);
struct csv *delete(struct binary_tree *bt, unsigned int target);
int unbalanced(unsigned long w1, unsigned long w2);
unsigned long get_weight(struct binary_tree_node *ptr);

#endif
