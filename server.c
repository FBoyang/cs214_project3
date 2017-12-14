#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include "binary_tree.h"
#include "protocol.h"

const int BUF_LEN = 256;
const int HDR_LEN = 128;
const int PAD_LEN = 128;
const int BACKLOG = 16;

struct in_addr *client_ips;
int num_ips;
int ip_capacity;

struct service_args {
    int fd;
    struct binary_tree *bt;
};

void *service(void *arg);
void interrupt_handler(int signum);

int main(int argc, char **argv)
{
    int c, port, sock, fd;
    struct sockaddr_in addr;
    socklen_t len;
    struct binary_tree *bt;
    struct service_args *sa;
    pthread_t tid;
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
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("failed to create socket");
        return 1;
    }
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("failed to bind socket to port");
        return 1;
    }
    if (listen(sock, BACKLOG) == -1) {
        perror("failed to listen on socket");
        return 1;
    }
    bt = initialize_binary_tree();
    len = sizeof(addr);
    num_ips = 0;
    ip_capacity = 4;
    client_ips = malloc(ip_capacity * sizeof(*client_ips));
    if (signal(SIGINT, interrupt_handler) == SIG_ERR) {
        perror("failed to set signal handler");
        return 1;
    }
    while ((fd = accept(sock, (struct sockaddr *) &addr, &len)) != -1) {
        if (num_ips > ip_capacity) {
            ip_capacity *= 2;
            client_ips = realloc(client_ips, ip_capacity * sizeof(*client_ips));
        }
        client_ips[num_ips++] = addr.sin_addr;
        sa = malloc(sizeof(*sa));
        sa->fd = fd;
        sa->bt = bt;
        if (pthread_create(&tid, NULL, &service, sa)) {
            perror("failed to create thread");
            free(sa);
            break;
        }
    }
    perror("failed to accept connection");
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
    if (read(fd, buffer, HDR_LEN) != HDR_LEN) {
        perror("failed to read header");
        free(arg);
        return NULL;
    }
    buffer[HDR_LEN - 1] = '\0';
    if (strncmp(buffer, DUMP_PREFIX, DUMP_PREFIX_LEN) == 0) {
        if (sscanf(buffer, DUMP_FMT, &sid) == 1) {
            if ((file = get_output(bt, sid)))
                len = strlen(file);
            else
                len = 0;
            sprintf(buffer, LENGTH_FMT, strlen(file));
            if (write(fd, buffer, HDR_LEN) != HDR_LEN) {
                perror("failed to write output file header");
            } else {
                for (fptr = file, i = 0; i < len; fptr += BUF_LEN, i += BUF_LEN) {
                    strncpy(buffer, fptr, BUF_LEN);
                    if (write(fd, buffer, BUF_LEN) != BUF_LEN) {
                        perror("failed to write output file");
                        break;
                    }
                }
            }
            free(file);
        }
    } else if (strncmp(buffer, NEW_SESSION_PREFIX, NEW_SESSION_PREFIX_LEN) == 0) {
        if (sscanf(buffer, NEW_SESSION_FMT, field_name) == 1) {   
            sid = new_session(bt, field_name);
            sprintf(buffer, SESSION_ID_FMT, sid);
            if (write(fd, buffer, HDR_LEN) != HDR_LEN) {
                perror("failed to create session");
            }
        }
    } else if (sscanf(buffer, SORT_FMT, &sid, &len) == 2) {
        read(fd, buffer, PAD_LEN);
        file = malloc((len + 1) * sizeof(*file));
        fptr = file;
        i = 0;
        for (fptr = file, i = 0; i <= len - BUF_LEN; fptr += BUF_LEN, i += BUF_LEN) {
            if (read(fd, buffer, BUF_LEN) != BUF_LEN) {
                perror("failed to read input file");
                break;
            }
            strncpy(fptr, buffer, BUF_LEN);
        }
        if (read(fd, buffer, len - i) != len - i) {
            perror("failed to read input file");
        } else {
            strncpy(fptr, buffer, len - i);
            file[len] = '\0';
            append_file(bt, file, sid);
            strcpy(buffer, FILE_DONE_FMT);
            if (write(fd, buffer, BUF_LEN) != BUF_LEN)
                perror("failed to send done message");
        }
        free(file);
    }
    close(fd);
    free(arg);
    return NULL;
}

void interrupt_handler(int signum)
{
    int i;
    fputs("Received connections from: ", stdout);
    for (i = 0; i < num_ips; i++) {
        fprintf(stdout, "%s", inet_ntoa(client_ips[i]));
        if (i < num_ips - 1)
            fputc('c', stdout);
    }
    signal(signum, SIG_DFL);
    raise(signum);
}
