#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "buf_store.h"
#include "readbuf.h"
#include "mergesort.h"

/*socket stuff
 */
#include <sys/socket.h>
#include <netinet/in.h>

/*Pthread Library*/
#include <pthread.h>

/*Macros (define your macros below */
#define MAX_NUM_THREAD 10

/*******start coding here *******/
//you need a port number here
#define PORT 9011

/*******coding End *******/

int status = 1;
int num_of_thread = 0;

char* itoa(int val, int base){
    static char buf[32] = {0};
    int i = 30;
    for(; val && i; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];
    return &buf[i+1];
}

void *service(void *args)
{
    //this is the socket for our server
    //to talk to the client
    //define two buffers, receive and send
    struct sarg *arg = (struct sarg*) args;
    int client_socket = arg -> socketfd;
    int size;
    int sess_id = 0;
    int field = 0;
    /*step 5: receive data */
    //use read system call to read data
    
    struct bufarg *buf_list = arg -> id_list;
    //create a buffer list and get an available id
    
    char rebeg[128];
    int a = read(client_socket, rebeg, 128);
    if (a <= 0){
        perror("read");
    }
    char *readrec=strtok_r(rebeg,"-_-",&op);
    
    
    
    
    if(strcmp(readrec,"Get_Id")==0){
        //here should write to me
        printf("now sending the session id\n\n\n\n\n\n");
        //send session id to client
        
        
        
        //By Chijun Sha start
        char *field_index = strtok_r(NULL,"-_-",&op);
        field = get_field_index(sortedBy);
        if(field == -1){
            return;
        }
        //By Chijun Sha end
        
        
        
        sess_id = get_id(buf_list);
        if(buf_list[sess_id].table == NULL)
            buf_list[sess_id].table = malloc(sizeof(struct csv));
        initialize_csv(buf_list[sess_id].table);
        char *send_id = itoa(sess_id, 10);
        if(write(client_socket, send_id, 4) <= 0){
            perror("write");
        }
    }
    else if(strcmp(readrec, "QUIT_SERVER") == 0){
        
        
        //By Chijun Sha start
        //do you need get id first?
        
         char *field_index = strtok_r(NULL,"-_-",&op);
         sess_id = atoi(field_index);
        //by Chijun Sha end
        
        
        
        //Quit server
        /*STEP 6: send data */
        // prepare your sending data
        // use write system call to send data
        char ***matrix = buf_list[sess_id].table -> matrix;
        int high = (buf_list[sess_id].table) -> num_rows;
        mergesort(0, high, field, matrix);
        int t_size = buf_list[sess_id].size;
        char *sort_buffer = malloc(t_size + 10);
        print_csv(buf_list[sess_id].table, sort_buffer);
        write(client_socket, sort_buffer, t_size + 10);
        printf("[s] Data sent \n");
    }else{
        //else, do normal works
        int a;
        /*
         FILE *fptr;
         remove("output.csv");
         fptr = fopen("output.csv", "w");
         */
        
        
        
        //By Chijun Sha start
        //        char *recv_buf = malloc(10);
        //        char * op;
        //        char *id=strtok_r(rebeg,"-_-",&op);
        //        char *length=strtok_r(NULL,"-_-",&op);
        //        size=atoi(readrec);
        sess_id = atoi(readrec);
        char *length=strtok_r(NULL,"-_-",&op);
        size=atoi(readrec);
        buf_list[sess_id].size += size;
        printf("now reciving%d,path%s", size, id);
        char *recv_buf = realloc(recv_buf, 1024);
        //by Chijun Sha end
        
        
        
        a = 0;
        do{
            a += read(client_socket, recv_buf, 1024);
            memset(recv_buf, 0, 1024);
        }while(a < size - 1024);
        a = read (client_socket, recv_buf, size - a);
        //now append the current buffer into the corresponding matrix
        readbuf(recv_buf, &buf_list[sess_id]);
        
        
        printf("buffer size is %d\n", size);
        //printf("last 10 char of buffer is %s\n", recv_buf[size - 10]);
        printf("[r] Reading from  client: %s\n", recv_buf);
    }
    /*step 7: close socket */
    close(client_socket);
    
    return NULL;
}


int main(int argc, char **argv)
{
    /*optional: You can add args checking
     * 	here. That means you allow user to
     * 	enter some information such as port
     * 	number
     */
    
    /***********start code here *****************/
    
    int server_sock, client_sock;
    struct sockaddr_in server_address;
    
    /*	step1: create socket and setup sockaddr_in */
    //you can call socket(AF_INET, SOCK_STREAM, 0)
    //to create a socket
    //REMEMBER: ALWAYS CHECK FAILURE WHEN YOU DO I/O
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    //setup the sockaddr_in struct you defined above
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
    
    if (bind(server_sock, (struct sockaddr*)&server_address, sizeof(server_address))<0)
    {
        perror("bind");
        //close socket
        close(server_sock);
        //exit your program
        exit(EXIT_FAILURE);
    }
    
    //step *3 listen
    //Now we have a binded socket, we can use this socket
    //to listen on a port
    if(listen(server_sock, 0) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    printf("Waiting for connections ...\n");
    struct bufarg *buf_list = NULL;
    
    //we use a while loop to keep waiting for the connections
    while(status)
    {
        /*STEP 4: create connnection/accept connection request */
        //use client socket to accept client request
        client_sock = accept(server_sock, NULL, NULL);
        struct sarg *arg = malloc(sizeof(struct sarg*));
        arg -> id_list = buf_list;
        if(client_sock < 0){
            perror("accept");
            close(server_sock);
            exit(EXIT_FAILURE);
        }
        
        
        printf("[+] Connect to client %d\n", client_sock);
        pthread_t tid;
        arg -> socketfd = client_sock;
        //tid_pool[i].socketfd  = client_sock;
        //service((void *)i);
        
        pthread_create(&tid, NULL, service, (void*)arg);
        // num_of_thread ++;
        pthread_detach(tid);
        
        
    }
    return 0;
}	

