#ifndef _MERGESORT_H
#define _MERGESORT_H

#include "tid_list.h"
#include "main.h"
//pthread_mutex_t locker = PTHREAD_MUTEX_INITIALIZER;

char ***smatrix;
 
struct mergesort_args {
	char ***matrix;
	//struct tid_list *tids;
	int field_index;
	int low;
	int high;
};

void *mergesort(void *arg);

#endif
