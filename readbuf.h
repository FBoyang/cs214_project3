#ifndef _CSV_H
#define _CSV_H

struct csv {
	char ***matrix;
	pthread_mutex_t mutex;
	int num_rows;
	int row_capacity;
	int t_length;
};

struct csv* initialize_csv();
struct csv readbuf(char *buffer);
void append_file(char *file, int len, int sid, struct bufarg* ba);
void append_csv(struct csv *table, char ***new_entries, int num_new);
char *print_csv(struct bufarg,  int length);
void free_csv(struct csv *table);

#endif
