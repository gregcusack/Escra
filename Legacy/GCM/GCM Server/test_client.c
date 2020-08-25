#ifndef __TESTCLIENT__
#define __TESTCLIENT__


// TCP Client program 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include "server.h"

#define PORT 4444
#define MAXLINE 30 
int main() 
{
    int sockfd; 
    char buffer[MAXLINE]; 
    char* message = "Hello Server"; 
    struct sockaddr_in servaddr; 
  
    int n, len; 
    // Creating socket file descriptor 
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        printf("socket creation failed"); 
        exit(0); 
    } 
  
    memset(&servaddr, 0, sizeof(servaddr)); 
  
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
  
    if (connect(sockfd, (struct sockaddr*)&servaddr,  
                             sizeof(servaddr)) < 0) { 
        printf("\n Error : Connect Failed \n"); 
    }
    //printf("[dbg] Are we even here?\n");
    ec_message_t* ec_message = (ec_message_t*) malloc( sizeof(ec_message) );
    ec_message -> is_mem = 1;
    ec_message -> cgroup_id = 2;
    ec_message -> mem_limit = 500;

    memset(buffer, 0, sizeof(buffer)); 
    //strcpy(buffer, "Hello Server"); 
    write(sockfd, (char*)ec_message , sizeof(ec_message_t)); 
    printf("Message from server: ");  
    read(sockfd, buffer, sizeof(buffer));
    //puts(buffer); 
    printf("return value is: %d\n", *((int*)buffer) );
    close(sockfd);
} 


#endif