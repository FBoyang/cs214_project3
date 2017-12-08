#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "readbuf.h"

#define NUM_COLS 28

char *header = "color,director_name,num_critic_for_reviews,duration,director_facebook_likes,actor_3_facebook_likes,actor_2_name,actor_1_facebook_likes,gross,genres,actor_1_name,movie_title,num_voted_users,cast_total_facebook_likes,actor_3_name,facenumber_in_poster,plot_keywords,movie_imdb_link,num_user_for_reviews,language,country,content_rating,budget,title_year,actor_2_facebook_likes,imdb_score,aspect_ratio,movie_facebook_likes\r\n";

char *field_list[NUM_COLS] = {
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

void initialize_csv(struct csv *table)
{
	table->matrix = NULL;
	pthread_mutex_init(&table->mutex, NULL);
	table->num_rows = 0;
	table->row_capacity = 0;
}


//csvread would take a buffer,a matrix as an argument
void readbuf(char *buffer, struct bufarg* node)
{
	
	char ***matrix;
	int row;
	int row_capacity;
	char *line, *linep;
	char *str;
	char *strex;
	int i;
	
/*
	if ((infile = fopen(args->file, "r")) == NULL) {
		fprintf(stderr, "failed to open file \"%s\" for reading\n", args->file);
		return NULL;
	}
	*/

	line = strtok(buffer, "\r\n");
	
	if (line == NULL || strcmp(line, header)) {
		//fputs("invalid header\n", stderr);
		free(buffer);
		return;
	}
	row_capacity = 64;
	matrix = malloc(row_capacity * sizeof(*matrix));
	row = 0;
	while ((line = strtok(NULL, "\r\n")) != NULL) {
		/*
		if (strlen(line) >= 2 && strcmp(line + strlen(line) - 2, "\r\n") == 0)
			line[strlen(line) - 2] = '\0';
		*/
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
	free(buffer);
	if(node -> table == NULL)	
		node -> table = calloc(1, sizeof(struct csv));
	
	append_csv(node->table, matrix, row);
	free(matrix);
	return;
}

void append_csv(struct csv *table, char ***new_entries, int num_new)
{
	pthread_mutex_lock(&table->mutex);
	if (table->num_rows + num_new > table->row_capacity) {
		table->row_capacity = 2 * (table->num_rows + num_new);
		table->matrix = realloc(table->matrix, table->row_capacity * sizeof(*table->matrix));
	}
	memcpy(table->matrix + table->num_rows, new_entries, num_new * sizeof(*table->matrix));
	table->num_rows += num_new;
	pthread_mutex_unlock(&table->mutex);
}

void print_csv(struct csv *table, char *buffer)
{
	int i, j;
	int length = table -> num_rows;
	buffer = malloc(length);
	sprintf(buffer, "%s", header);
	for (i = 0; i < table->num_rows; i++) {
		for (j = 0; j < NUM_COLS; j++) {
			if (table->matrix[i][j]) {
				if (index(table->matrix[i][j], ','))
					sprintf(buffer, "\"%s\"", table->matrix[i][j]);
				else
					sprintf(buffer, table->matrix[i][j]);
			}
			if (j < NUM_COLS - 1)
				sprintf(buffer, ",");
		}
		sprintf(buffer, "\r\n");
	}
}

void free_csv(struct csv *table)
{
	int i, j;
	for (i = 0; i < table->num_rows; i++) {
		for (j = 0; j < NUM_COLS; j++)
			free(table->matrix[i][j]);
		free(table->matrix[i]);
	}
	free(table->matrix);
	pthread_mutex_destroy(&table->mutex);
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
