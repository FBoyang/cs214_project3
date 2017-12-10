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
	pthread_mutex_t mutex;
};

struct csv* initialize_csv();
void read_csv(char *buffer, struct csv *table, int len);
void append_file(char *file, int len, int sid, struct bufarg* ba);
void append_csv(struct csv *table, char ***new_entries, int num_new, int length);
char *print_csv(struct bufarg args);
void free_csv(struct csv *table);
int get_field_index(char *);
#endif
