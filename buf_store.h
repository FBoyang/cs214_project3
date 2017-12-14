#ifndef _BUF_STORE_H
#define _BUF_STORE_H
#include <pthread.h>
struct bufarg{
	struct csv *table;
	int id_size;
	int isFree;
	int field_num;
	pthread_mutex_t id_locker;
};


struct bufNode{
	struct csv *table;
	int field_num;
	int sess_id;
	pthread_mutex_t id_locker;
	int append_num;
	struct bufNode *next;
};


struct bufNode *init_node();
void add_node(struct bufNode *list, struct bufNode *nNode);
int delete(struct bufNode *list, int id);
struct bufNode *search(struct bufNode *list, int sid);
#include "readbuf.h"
void enlarge(struct bufarg**);
void free_bufarg(struct bufarg*);
struct bufarg* init_array();
int get_id(char *, struct bufarg**);
void free_id(struct bufarg*, int);
#endif 
