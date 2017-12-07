#include <stdio.h>
#include <stdlib.h>
#include "buf_store.h"

void enlarge(struct bufarg *bufarray)
{
	int size = bufarray[0].size;
	bufarray = realloc(bufarray, size*2);
	bufarray[0].size = size*2;
	int i;
	for(i = size; i < size*2; i++){
		bufarray[i].sess_id = i;
		bufarray[i].isFree = 1;
	}
}

void init_array(struct bufarg *bufarray)
{
	bufarray = malloc(10 * sizeof(struct bufarg));
	bufarray[0].size = 10;
	int i;
	for(i = 0; i < 10; i++){
		bufarray[i].sess_id = i;
		bufarray[i].isFree = 1;
	}
	
}

//return an available session id
int get_id(struct bufarg *bufarray)
{
	if(bufarray == NULL){
		init_array(bufarray);
		bufarray[0].isFree = 0;
		return 0;
	}

	int size = bufarray[0].size;
	int i;
	for(i = 0; i < size; i++){
		if(bufarray[i].isFree == 1){
			bufarray[i].isFree = 0;
			return i;
		}
	}
	enlarge(bufarray);
	bufarray[i].isFree = 0;
	return i;
}

void free_id(struct bufarg *bufarray, int i)
{
	struct csv *table;
	table = bufarray[i].table;
	bufarray[i].table = NULL;
	free_csv(table);
	char *del_buf = bufarray[i].buffer;
	bufarray[i].buffer = NULL;
	free(del_buf);
	bufarray[i].isFree = 1;
}
	
	
		
