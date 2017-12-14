#include <stdio.h>
#include <stdlib.h>
#include "readbuf.h"
#include "server.h"

void enlarge(struct bufarg **bufarray)
{
	int size = (*bufarray[0]).id_size;
	*bufarray = realloc(bufarray, size*2);
	(*bufarray)[0].id_size = size*2;
	int i;
	for(i = size; i < size*2; i++){
		(*bufarray)[i].isFree = 1;
	}
}

struct bufNode *init_node()
{
	struct bufNode *NNode = malloc(sizeof(struct bufNode));
	NNode -> table = initialize_csv();
	NNode -> field_num = -1;
	pthread_mutex_init(&(NNode -> id_locker), NULL);
	NNode -> next = NULL;
	NNode -> sess_id = -1;
	NNode -> append_num = 0;
	return NNode;
}

void add_node(struct bufNode *list, struct bufNode *nNode)
{
	struct bufNode *ptr = list;
	while(ptr -> next != NULL){
		ptr = ptr -> next;
	}
	ptr -> next = nNode;
}

int delete(struct bufNode *list, int id)
{
	if(list == NULL){
		return -1;
	}
	struct bufNode *ptr = list -> next;
	struct bufNode *prev = list;
	for (; ptr != NULL; ptr = ptr -> next){
		if(ptr -> sess_id == id){
			struct bufNode *temp = ptr;
			prev -> next = ptr -> next;
			free_csv(temp -> table);
			free(temp);
			return 1;
		}
		prev = prev -> next;
	}
	return -1;
}
			
struct bufNode *search(struct bufNode *list, int sid)
{
	struct bufNode *ptr = list;
	while(ptr != NULL){
		if(ptr -> sess_id == sid){
			return ptr;
		}
		ptr = ptr->next; 
	}
	return NULL;
}

			
struct bufarg *init_array()
{
	struct bufarg *bufarray = malloc(10 * sizeof(struct bufarg));
	bufarray[0].id_size = 10;
	int i;
	for(i = 0; i < 10; i++){
		bufarray[i].isFree = 1;
	}
	return bufarray;
	
}

//return an available session id
int get_id(char *field_name, struct bufarg **bufarray)
{

	printf("start getting id\n");
	
	int size =(*bufarray)[0].id_size;
	int i;
	for(i = 0; i < size - 1; i++){
		if((*bufarray)[i].isFree == 1){
			(*bufarray)[i].isFree = 0;
			(*bufarray)[i].table = initialize_csv();
			(*bufarray)[i].field_num = get_field_index(field_name);
			return i;
		}
	}
	
	enlarge(bufarray);
	(*bufarray)[i].isFree = 0;
	(*bufarray)[i].table = initialize_csv();
	(*bufarray)[i].field_num = get_field_index(field_name);
	pthread_mutex_unlock(&id_locker);
	printf("get id: %d\n", i);
	return i;
}

void free_id(struct bufarg *bufarray, int i)
{
	struct csv *table;
	table = bufarray[i].table;
	bufarray[i].table = NULL;
	free_csv(table);
	bufarray[i].isFree = 1;
}

void free_bufarg(struct bufarg *fi_arg)
{
	free_csv(fi_arg -> table);
	fi_arg -> isFree = 1;
}		
	
		
