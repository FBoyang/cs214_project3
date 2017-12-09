#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "buf_store.h"

#define BUF_LEN 256
#define HDR_LEN 128
#define PAD_LEN 128
#define BACKLOG 16

struct service_args {
    int fd;
    struct bufarg *ba;
};

void *service(void *arg);

int main(int argc, char **argv)
{
    int c, port, sock, fd;
    struct sockaddr_in addr;
    struct bufarg *ba;
    struct service_args *sa;
    port = 0;
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
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fputs("failed to create socket\n", stderr);
        return 1;
    }
    bzero(addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = INADDR_ANY;
    if (bind(sock, &addr, sizeof(addr)) == -1) {
        fprintf(stderr, "failed to bind socket %d to port %d\n", sock, port);
        return 1;
    }
    if (listen(sock, BACKLOG) == -1) {
        fprintf(stderr, "failed to listen on socket %d with backlog %d\n", sock, BACKLOG);
        return 1;
    }
    ba = init_array();
    while (1) {
        if ((fd = accept(sock, NULL, NULL)) == -1) {
            fprintf(stderr, "failed to accept connection on socket %d\n", sock);
            return 1;
        }
        sa = malloc(sizeof(*sa));
        sa->fd = fd;
        sa->ba = &ba;
        if (pthread_create(NULL, NULL, &service, sa)) {
            fputs("failed to create thread\n", stderr);
            free(sa);
            return 1;
        }
    }
    return 0;
}

void *service(void *arg)
{
    struct service_args *args;
    struct bufarg *ba;
    int fd, sid, len, i;
    char buffer[BUF_LEN], field_name[BUF_LEN], *file, *fptr;
    args = (struct service_args *) arg;
    fd = args->fd;
    ba = args->ba;
    read(fd, buffer, HDR_LEN);
    if (strncmp(buffer, "QUIT_SERVER", 11) == 0) {
        if (sscanf(buffer, "QUIT_SERVER-_-%d", &sid) == 1 && sid < ba[0].size && ba[sid].isFree == 0) {
            file = print_csv(ba[sid]);
            len = strlen(file);
            sprintf(buffer, "length %d", strlen(file));
            write(fd, buffer, HDR_LEN);
            for (fptr = file, i = 0; i < len; fptr += BUF_LEN, i += BUF_LEN) {
                strncpy(buffer, fptr, BUF_LEN);
                write(fd, buffer, BUF_LEN);
            }
            free(file);
        }
    } else if (strncmp(buffer, "Get_Id", 6) == 0) {
        if (sscanf(buffer, "Get_Id-_-%s", &field_name) == 1) {
            sid = get_id(field_name, &ba);
            sprintf(buffer, "%d", sid);
            write(fd, buffer, HDR_LEN);
        }
    } else if (sscanf(buffer, "%d-_-%d", &sid, &len) == 2) {
        read(fb, buffer, PAD_LEN);
        file = malloc((len + 1) * sizeof(*file));
        for (fptr = file, i = 0; i <= len - BUF_LEN; fptr += BUF_LEN, i += BUF_LEN) {
            read(fd, buffer, BUF_LEN);
            strncpy(fptr, buffer, BUF_LEN);
        }
        read(fb, buffer, len - i);
        strncpy(fptr, buffer, len - i);
        fptr[len - i] = '\0';
        append_file(file, len, sid, ba);
    }
    close(fd);
    free(arg);
    return NULL;
}
