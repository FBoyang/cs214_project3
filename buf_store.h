#ifndef _BUF_STORE_H
#define _BUF_STORE_H

#include "readbuf.h"
struct sarg{
	struct bufarg *id_list;
	int socketfd;
};	

struct bufarg{
	struct csv *table;
	int sess_id;
	int id_size;
	int isFree;
	int *field_num;
};

void enlarge(struct bufarg*);
void init_array(struct bufarg*);
int get_id(char *, struct bufarg**);
void free_id(struct bufarg*, int);
#endif 
