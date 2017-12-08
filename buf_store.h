#ifndef _BUF_STORE_H
#define _BUF_STORE_H

#include "readbuf.h"
struct sarg{
	struct bufarg *id_list;
	int socketfd;
};	

void enlarge(struct bufarg*);
void init_array(struct bufarg*);
int get_id(struct bufarg*);
void free_id(struct bufarg*, int);
#endif 
