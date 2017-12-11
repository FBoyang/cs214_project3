#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include "binary_tree.h"
#include "protocol.h"

const int BUF_LEN = 256;
const int HDR_LEN = 128;
const int PAD_LEN = 128;
const int BACKLOG = 16;

struct service_args {
    int fd;
    struct binary_tree *bt;
};

void *service(void *arg);

int main(int argc, char **argv)
{
    int c, port, sock, fd;
    struct sockaddr_in addr;
    struct binary_tree *bt;
    struct service_args *sa;
    pthread_t tid;
    port = 0;
    fprintf(stdout, "Received connections from: ");
    while ((c = getopt(argc, argv, "p:")) != -1) {
        switch (c) {
        case 'p':
            port = atoi(optarg);
        }
    }
    if (port == 0) {
        fputs("please provide a port number\n", stderr);
        return 1;
    }
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        fputs("failed to create socket\n", stderr);
        return 1;
    }
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        fprintf(stderr, "failed to bind socket %d to port %d\n", sock, port);
        return 1;
    }
    if (listen(sock, BACKLOG) == -1) {
        fprintf(stderr, "failed to listen on socket %d with backlog %d\n", sock, BACKLOG);
        return 1;
    }
    bt = initialize_binary_tree();
    while (1) {
        if ((fd = accept(sock, NULL, NULL)) == -1) {
            fprintf(stderr, "failed to accept connection on socket %d\n", sock);
            return 1;
        }
        sa = malloc(sizeof(*sa));
        sa->fd = fd;
        sa->bt = bt;
        if (pthread_create(&tid, NULL, &service, sa)) {
            fputs("failed to create thread\n", stderr);
            free(sa);
            return 1;
        }
    }
    free_binary_tree(bt);
    return 0;
}

void *service(void *arg)
{
    struct service_args *args;
    struct binary_tree *bt;
    int fd, sid, len, i;
    char buffer[BUF_LEN], field_name[BUF_LEN], *file, *fptr;
    args = (struct service_args *) arg;
    fd = args->fd;
    bt = args->bt;
    if (read(fd, buffer, HDR_LEN) == -1) {
	    perror("failed to read header");
	    free(arg);
	    return NULL;
    }
    if (strncmp(buffer, DUMP_PREFIX, DUMP_PREFIX_LEN) == 0) {
        if (sscanf(buffer, DUMP_FMT, &sid) == 1) {
            file = get_output(bt, sid);
            len = strlen(file);
            sprintf(buffer, LENGTH_FMT, strlen(file));
            if (write(fd, buffer, HDR_LEN) != -1) {
		    perror("failed to write output file header");
	    } else {
		    for (fptr = file, i = 0; i < len; fptr += BUF_LEN, i += BUF_LEN) {
			strncpy(buffer, fptr, BUF_LEN);
			if (write(fd, buffer, BUF_LEN) == -1) {
				perror("failed to write output file");
				break;
			}
		    }
	    }
            free(file);
        }
    } else if (strncmp(buffer, NEW_SESSION_PREFIX, NEW_SESSION_PREFIX_LEN) == 0) {
        if (sscanf(buffer, NEW_SESSION_FMT, field_name) == 1) {   
	    sid = new_session(bt);
            sprintf(buffer, SESSION_ID_FMT, sid);
            if (write(fd, buffer, HDR_LEN) == -1) {
		    perror("failed to create session");
	    }
        }
    } else if (sscanf(buffer, SORT_FMT, &sid, &len) == 2) {
        read(fd, buffer, PAD_LEN);
        file = malloc((len + 1) * sizeof(*file));
	fptr = file;
	i = 0;
        for (fptr = file, i = 0; i <= len - BUF_LEN; fptr += BUF_LEN, i += BUF_LEN) {
            if (read(fd, buffer, BUF_LEN) == -1) {
		    perror("failed to read input file");
		    break;
	    }
            strncpy(fptr, buffer, BUF_LEN);
        }
        if (read(fd, buffer, len - i) == -1) {
		perror("failed to read input file");
	} else {
		strncpy(fptr, buffer, len - i);
		file[len] = '\0';
		append_file(bt, file, sid);
		strcpy(buffer, FILE_DONE_FMT);
		write(fd, buffer, BUF_LEN);
	}
	free(file);
    }
    close(fd);
    free(arg);
    return NULL;
}
