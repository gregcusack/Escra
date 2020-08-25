// Server running in user space
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#define PORT 4444
#define PERIOD 1000000
#define QUOTA 1000000
#define MIN_QUOTA 5000
#define MAX 32

struct payload {
	uint16_t group_id;
	uint8_t resource;		//mem=0 or cpu=1
	uint8_t type;		//request=0 or give back=1
	uint32_t amount;		//max = 1.07 petabytes mem
};

void payload_hton(struct payload *p) {
	p->group_id = htons(p->group_id);
	p->amount = htonl(p->amount);
}

void payload_ntoh(struct payload *p) {
	p->group_id = ntohs(p->group_id);
	p->amount = ntohl(p->amount);
}


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
    char *hello = "Hello from server";
//    char* bandwidth_refill = "1000000";
    int32_t bandwidth_request;
    int32_t bandwidth_refill;
    char *bandwidth_refill_char;
    struct payload rx_pkg;
    uint16_t rx_group_id;
    uint32_t rx_amount;

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
    while(1) {
    	bzero(buffer, MAX);
    	valread = read(new_socket, &rx_pkg, sizeof(rx_pkg));
    	payload_ntoh(&rx_pkg);
    	rx_group_id = rx_pkg.group_id;
    	rx_amount = rx_pkg.amount;

    	printf("Rx. GID: %d, BW request: %dns\n", rx_group_id, rx_amount);

    	if(rx_amount > QUOTA) {
    		printf("bandwidth requested exceeds allowable quota. sending back quota ms\n");
    		rx_amount = QUOTA;
    	}
    	else if(rx_amount <= 0) {
    		perror("ya should NOT be here\n. bandwidth requested was <= 0\n");
    		rx_amount = MIN_QUOTA;
    	}
    	else if(rx_amount < QUOTA) {
    		printf("bandwidth requested: %dms. Too small, sending back minimum.\n", rx_amount);
    		rx_amount = MIN_QUOTA;
    	}
    	rx_pkg.group_id = rx_group_id;
    	rx_pkg.amount = rx_amount;
    	rx_pkg.resource = 1;
    	rx_pkg.type = 1;

    	payload_hton(&rx_pkg);
    	send(new_socket , &rx_pkg , sizeof(rx_pkg) , 0 );

    	printf("-------------------------\n");
    }
    return 0;
}






