/*
 * GCM_server.c
 *
 *  Created on: Aug 15, 2019
 *      Author: greg
 */

// Server running in user space
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#define NUM_CORES 4
#define PORT 4444
#define PERIOD 100000000
#define MAX_QUOTA (PERIOD * NUM_CORES)
#define MIN_QUOTA 1000000
#define MAX 32

int main(int argc, char const *argv[])
{
	if(argc < 2) {
		perror("specify port\n");
		exit(EXIT_FAILURE);
	}


    int server_fd, new_socket, valread, port;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[MAX] = {0};
    uint64_t bandwidth_request;
    uint32_t bandwidth_refill;
    char *bandwidth_refill_char;
    uint16_t rx_group_id;
    uint32_t rx_amount;
    uint64_t bandwidth_send_request = 500000;

    port = strtol(argv[1], NULL, 10);
    printf("server port: %d\n", port);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // here, we want to read a one time message from the client - i.e. Registration Process
    int ret;
    int local_pid;
    //ret = read(new_socket, &local_pid, sizeof(int));
    //printf("RET Value: %d\n", ret);
    //printf("Local PID VALUE: %d\n", local_pid);
    //int global_pid;
    //ret = read(new_socket, &global_pid, sizeof(int));
    //printf("RET Value: %d\n", ret);
    //printf("Global PID VALUE: %d\n", global_pid);
    //printf("Sending Confirmation of Container..\n");
    //send(new_socket , "Confirmed" , 10, 0 );

    char buffer_read[1024];

    while(1) {
    	//bzero(buffer, MAX);
    	valread = read(new_socket, buffer, 1024);
//    	payload_ntoh(&rx_pkg);
//    	rx_group_id = rx_pkg.group_id;
//    	rx_amount = rx_pkg.amount;

    	printf("Raw Data Request: %s\n", buffer);

	/*printf("BW request: %lu ns\n", bandwidth_request);
    	if(bandwidth_request > MAX_QUOTA) {
    		printf("bandwidth requested exceeds allowable quota. sending back quota ms\n");
    		bandwidth_request = MAX_QUOTA;
    	}
    	else if(bandwidth_request <= 0) {
    		perror("ya should NOT be here\n. bandwidth requested was <= 0\n");
    		bandwidth_request = MIN_QUOTA;
    	}
    	else if(bandwidth_request < MIN_QUOTA) {
    		printf("bandwidth requested: %lu ms. Too small, sending back minimum.\n", bandwidth_request);
    		bandwidth_request = MIN_QUOTA;
    	}
    	bandwidth_request--;
	printf("Sending bandwidth request %ld ... \n", bandwidth_request );
    	send(new_socket , &bandwidth_request , sizeof(bandwidth_request) , 0 );
	*/
	//send(new_socket , &bandwidth_send_request , sizeof(bandwidth_send_request) , 0 );
    	printf("-------------------------\n");
    }
    return 0;
}







