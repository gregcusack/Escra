#include "agent.h"

unsigned long handle_mem_req(ec_reclaim_msg_t* req) {
	unsigned long ret = 0;
	printf("[INFO] Agent: Trying to get some memory from: %d\n", req->cgroup_id );
	unsigned long avail_mem = 0;
	//for ( count = 0; count < req -> num_of_cgroups; ++count)
	//{
	ret = syscall(__NR_SYSCALL__, req->cgroup_id, false);

	printf("[dbg] shrink_mem syscall: Syscall returned %lu . \n", ret);

	avail_mem += ret;

	//}
	return avail_mem;

}

unsigned long handle_req(char* buffer) {

	ec_reclaim_msg_t* req = (ec_reclaim_msg_t*) buffer;
	unsigned long ret = __FAILED__;
	printf("[dbg] maximum memory read from container: %d\n", req->cgroup_id);
	//printf("[dbg] cgroup id of the container: %d\n", req->cgroup_id);
	switch ( req -> is_mem ) {

		case true:
			ret = handle_mem_req(req);
			break;

		case false:
			//ret = handle_cpu_req(req);
			printf("[dbg] Handling cpu stuff\n");
			break;

		default:
			perror("[ERROR] Global Cloud Manager: handling memory/cpu request failed!");
	}

	return ret;
}

void *handle_client_reqs(void* clifd) {

	int num_bytes;
	unsigned long ret;
	char buffer[__BUFFSIZE__];
	int client_fd = *((int*) clifd);

	printf("[SUCCESS] GCM thread: new connection created! new socket fd: %d\n", client_fd);

	bzero(buffer, __BUFFSIZE__);
	while( (num_bytes = read(client_fd, buffer, __BUFFSIZE__) ) > 0 ){

		ret = 0;
		//printf("[dbg] Number of bytes read: %d\n", num_bytes);
		ret = handle_req(buffer);
		//printf("[dbg] handle request return value: %ld\n", ret);
		if (ret > 0)
		{
			//printf("[dbg] We got the new max! It's time to send!\n");
			if(write(client_fd, (const char*) &ret,  sizeof(unsigned long) ) < 0){
				perror("[dbg] Writing to socket failed!");
				break;
			}
		}
		else {
			printf("[FAILD] GCM Thread: [%lu] Unable to handle request!\n", mem_reqs++);
			if (write(client_fd, "NOMEM" ,  sizeof("NOMEM")+1 ) < 0)
			{
				perror("[dbg] Writing to socket failed!");
				break;
			}
		}
	}
		//maybe we should do some more things here
	//printf("Closing Connection!\n");
	//close(client_fd);
	pthread_exit(NULL);
}


gcm_server_t* create_mt_server(){

	int addrlen, opt = true;
	gcm_server_t* gcm_server;

	gcm_server = (gcm_server_t*) malloc( sizeof(gcm_server_t) );

	if ( ((gcm_server -> gcm_socket_fd) = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("[ERROR] Global Cloud Manager: Server creation failed!");
		exit(EXIT_FAILURE);
	}
	printf("[dbg] This is the server socket file descriptor: %d\n", gcm_server -> gcm_socket_fd);
	if (setsockopt(gcm_server -> gcm_socket_fd, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, sizeof(opt) ) < 0)
	{
		perror("[ERROR] Global Cloud Manager: Setting socket options failed!");
		exit(EXIT_FAILURE);
	}
	
	gcm_server -> address.sin_family = AF_INET;   
    gcm_server -> address.sin_addr.s_addr = INADDR_ANY;   
    gcm_server -> address.sin_port = htons( __PORT__ );

    if (bind(gcm_server->gcm_socket_fd, (struct sockaddr *)&gcm_server->address, sizeof(gcm_server->address))<0)   
    {
        perror("[ERROR] Global Cloud Manager: Binding socket failed!");
        exit(EXIT_FAILURE);
    }

    if (listen(gcm_server->gcm_socket_fd, 5) < 0)   
    {
        perror("[ERROR] Global Cloud Manager: Listenning on socket failed!");
        exit(EXIT_FAILURE);
    }
    printf("[dbg]GCM server socket successfully created!\n");
    return gcm_server;
}

void main_loop(gcm_server_t* gcm_server) {

	fd_set readfds;
	int max_sd, sd, cliaddr_len, clifd, select_rv;
	struct sockaddr_in cliaddr;
	pthread_t threads[__MAX_CLIENT__];

	gcm_server = create_mt_server();

	FD_ZERO(&readfds);
	max_sd = (gcm_server -> gcm_socket_fd) + 1;
	cliaddr_len = sizeof(cliaddr);
	printf("[dbg] Maximum socket descriptor is: %d\n", max_sd);

	while(true) {

		FD_SET(gcm_server -> gcm_socket_fd, &readfds);

		select_rv = select(max_sd, &readfds, NULL, NULL, NULL);

		printf("[dbg] An event happend on the agent server socket!\n");

		if ( FD_ISSET(gcm_server -> gcm_socket_fd, &readfds) ) {
			if ( (clifd = accept(gcm_server -> gcm_socket_fd, (struct sockaddr*)&cliaddr, &cliaddr_len )) > 0){
				if ( pthread_create(&threads[num_of_clients], NULL, handle_client_reqs, (void*) &clifd ) ){

					perror("[ERROR] EC Agent: Unable to create agent server thread!");
					break;
				}
				else {
					cgroup_fds[num_of_clients] = clifd;
					num_of_clients ++;
				}

			}
			else {
				printf("[ERROR] Unable to accept connection! Try again!\n");
				continue;
			}
		}
	}
}

int main(int argc, char const *argv[])
{

	gcm_server_t* gcm_server;

	main_loop (gcm_server);

	return 0;
}
