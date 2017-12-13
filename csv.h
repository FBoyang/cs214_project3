#ifndef _CSV_H
#define _CSV_H

struct file_node {
	char ***matrix;
	int num_rows;
	struct file_node *next;
};

struct csv {
	struct file_node *front;
    int field_index;
	int total_rows;
	int total_length;
	pthread_mutex_t mutex;
};

struct csv* initialize_csv(char *field_name);
struct file_node *read_csv(char *buffer);
void append_csv(struct csv *table, struct file_node *fn, int len);
char *print_csv(struct csv *table);
void free_csv(struct csv *table);
#endif
