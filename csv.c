#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "csv.h"

const int NUM_COLS = 28;

char *header = "color,director_name,num_critic_for_reviews,duration,director_facebook_likes,actor_3_facebook_likes,actor_2_name,actor_1_facebook_likes,gross,genres,actor_1_name,movie_title,num_voted_users,cast_total_facebook_likes,actor_3_name,facenumber_in_poster,plot_keywords,movie_imdb_link,num_user_for_reviews,language,country,content_rating,budget,title_year,actor_2_facebook_likes,imdb_score,aspect_ratio,movie_facebook_likes";

char *field_list[] = {
	"color",
	"director_name",
	"num_critic_for_reviews",
	"duration",
	"director_facebook_likes",
	"actor_3_facebook_likes",
	"actor_2_name",
	"actor_1_facebook_likes",
	"gross",
	"genres",
	"actor_1_name",
	"movie_title",
	"num_voted_users",
	"cast_total_facebook_likes",
	"actor_3_name",
	"facenumber_in_poster",
	"plot_keywords",
	"movie_imdb_link",
	"num_user_for_reviews",
	"language",
	"country",
	"content_rating",
	"budget",
	"title_year",
	"actor_2_facebook_likes",
	"imdb_score",
	"aspect_ratio",
	"movie_facebook_likes",
};

struct csv *initialize_csv()
{
	struct csv *table;
	table = malloc(sizeof(*table));
	table->front = NULL;
	table->total_rows = 0;
	table->total_length = 0;
	pthread_mutex_init(&table->mutex, NULL);
	return table;
}


//csvread would take a buffer,a matrix as an argument
struct file_node *read_csv(char *buffer, struct csv *table, int len)
{
	struct file_node *fn;
	char ***matrix;
	int row;
	int row_capacity;
	char *line, *linep;
	char *str;
	char *strex;
	int i;
	//printf("first line is %.*s\n", 10, buffer);
	line = strtok(buffer, "\r\n");
	//printf("header %s\n", line);
	if (line == NULL || strcmp(line, header)) {
		printf("header length %d, line length is %d\n", strlen(header), strlen(line));
		fputs("invalid header\n", stderr);
		free(buffer);
		return NULL;
	}
	row_capacity = 4;
	matrix = malloc(row_capacity * sizeof(*matrix));
	row = 0;
	while ((line = strtok(NULL, "\r\n")) != NULL) {
		if (row >= row_capacity) {
			row_capacity *= 2;
			matrix = realloc(matrix, row_capacity * sizeof(*matrix));
		}
		matrix[row] = malloc(NUM_COLS * sizeof(**matrix));
		linep = line;
		for (i = 0, str = strsep(&linep, ","); str; i++, str = strsep(&linep, ",")) {
			if (strlen(str) == 0) {
				matrix[row][i] = NULL;
			} else if (str[0] == '"') {
				strex = strsep(&linep, "\"");
				matrix[row][i] = malloc((strlen(str) + strlen(strex) + 1) * sizeof(char));
				sprintf(matrix[row][i], "%s,%s", str + 1, strex);
				strsep(&linep, ",");
			} else {
				matrix[row][i] = malloc((strlen(str) + 1) * sizeof(char));
				strcpy(matrix[row][i], str);
			}
		}
		row++;
	}
	fn = malloc(sizeof(*fn));
	fn->matrix = matrix;
	fn->num_rows = row;
	return fn;
}

void append_csv(struct csv *table, struct file_node *fn, int len)
{
	fn->next = table->front;
	table->front = fn;
	table->total_len += len;
	table->total_rows += fn->num_rows;
}

char *print_csv(file_node *ptr)
{

	int i, j;
	char *buffer = malloc(ptr->total_length + 1);
	char *ptr;
	sprintf(buffer, "%s", header);
	ptr = buffer + strlen(buffer);
	for (i = 0; i < ptr->num_rows; i++) {
		for (j = 0; j < NUM_COLS; j++) {
			if (ptr->matrix[i][j]) {
				if (index(ptr->matrix[i][j], ',')){
					sprintf(ptr, "\"%s\"", ptr->matrix[i][j]);
					ptr += strlen(ptr);
				}
				else{
					sprintf(ptr, ptr->matrix[i][j]);
					ptr += strlen(ptr);
				}
			}		
			if (j < NUM_COLS - 1){
				sprintf(ptr, ",");
				ptr += strlen(ptr);
			}
		}
		sprintf(ptr, "\r\n");
		ptr += strlen(ptr);
	}
	return buffer;

}

void free_csv(struct csv *table)
{
	struct file_node *ptr, *next;
	int i, j;
	for (ptr = table->front; ptr; ptr = next) {
		for (i = 0; i < ptr->num_rows; i++) {
			for (j = 0; j < NUM_COLS; j++)
				free(ptr->matrix[i][j]);
			free(ptr->matrix[i]);
		}
		free(ptr->matrix);
		next = ptr->next;
		free(ptr);
	}
	pthread_mutex_destroy(&table->mutex);
	free(table);
}



int get_field_index(char *field_name)
{
	int i;
	for (i = 0; i < NUM_COLS; i++) {
		if (strcmp(field_list[i], field_name) == 0) {
			return i;
		}
	}
	fprintf(stderr, "error: invalid field \"%s\"\n", field_name);
	return -1;
}
