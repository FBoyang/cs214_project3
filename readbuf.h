#ifndef _CSV_H
#define _CSV_H

struct csv {
	char ***matrix;
	pthread_mutex_t mutex;
	int num_rows;
	int row_capacity;
};

struct bufarg {
	struct csv *table;
 	int sess_id;
	char *buffer;
	int size;
	int isFree;
};

void initialize_csv(struct csv *table);
void readbuf(char *buffer, struct bufarg node);
void append_csv(struct csv *table, char ***new_entries, int num_new);
void print_csv(struct csv *table, char *buffer);
void free_csv(struct csv *table);

#endif
