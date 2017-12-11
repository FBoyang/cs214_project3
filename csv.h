#ifndef _CSV_H
#define _CSV_H

struct file_node {
	char ***matrix;
	int num_rows;
	struct file_node *next;
};

struct csv {
	struct file_node *front;
	int total_rows;
	int total_length;
	pthread_mutex_t mutex;
};

struct csv* initialize_csv();
struct file_node *read_csv(char *buffer);
void append_csv(struct csv *table, struct file_node *fn, int len);
char *print_csv(struct file_node *ptr);
void free_csv(struct csv *table);
int get_field_index(char *);
#endif
