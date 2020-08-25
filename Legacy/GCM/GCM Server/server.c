#include "server.h"

void init_agents_connection(int num_of_agents){
	int sockfd, i;
	struct sockaddr_in servaddr;

	for (i = 0; i < num_of_agents; ++i)
	{
		
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
	        printf("[ERROR] GCM:Socket creation failed! agent is not up!\n"); 
	        exit(0); 
	    }

	    memset(&servaddr, 0, sizeof(servaddr));

	    // Filling agent information 
	    servaddr.sin_family = AF_INET; 
	    servaddr.sin_port = htons(ec_agents[i]->port); 
	    servaddr.sin_addr.s_addr = inet_addr(ec_agents[i]->ip);

	    if (connect(sockfd, (struct sockaddr*)&servaddr,  
                             sizeof(servaddr)) < 0) { 
	        printf("\n [Error] GCM: Connection to agent failed \n Agent on %s is not connected!\n", ec_agents[i]->ip); 
	    }

	    ec_agents[i]->sockfd = sockfd;
	}
}

unsigned long reclaim_memory(int client_fd){
	int i = 0, j = 0;
	int num_of_agents = 1;
	char buffer[__BUFFSIZE__];
	unsigned long reclaimed = 0;

	printf("[INFO] GCM: Try to reclaim memory from other cgroups!\n");
	for(i =0; i < num_of_clients; ++i){
		
		if (ec_cgroups[i]->cgroup_fd == client_fd)
			continue;

		char* ip = inet_ntoa(ec_cgroups[i]->cliaddr->sin_addr);
		printf("[INFO] GCM: Ip of the agent: %s\n", ip);

		for (j = 0; j < num_of_agents; ++j)
		{
			if (!strcmp(ec_agents[j]->ip, ip))
			{
				ec_reclaim_msg_t* reclaim_req = (ec_reclaim_msg_t*)malloc(sizeof(ec_reclaim_msg_t));
				reclaim_req->cgroup_id = ec_cgroups[i]->cgroup_id;
				reclaim_req->req_type = 1;
				write(ec_agents[j]->sockfd, (char*)reclaim_req, sizeof(ec_reclaim_msg_t)); 
			    printf("Message from server: "); 
			    read(ec_agents[j]->sockfd, buffer, sizeof(buffer)); 
			    puts(buffer); 
			    if (strcmp(buffer,"NOMEM"))
			    	reclaimed += *((unsigned long*)buffer);
			    printf("[INFO] GCM: Reclaimed memory up to now: %lu\n", reclaimed );
			}
		}
	}
	printf("[dbg] Recalimed memory at the end of the reclaim function: %lu\n", reclaimed);
	return reclaimed;
}

unsigned long handle_mem_req(ec_message_t* req, int client_fd) {

	unsigned long ret = 0;
	unsigned long fail = 0;

	if (!req -> req_type)
		return __FAILED__;

	pthread_mutex_lock(&mlock);
	if (memory_limit > 0 || (memory_limit = reclaim_memory(client_fd)) > 0){
		printf("[dbg] handle mem req:This is successful reclaiming: %lu\n", memory_limit);
		goto success;
	}

	else
		goto failed;

	success:
		//pthread_mutex_unlock(&mlock);

		printf("[dbg] handle mem req: This is the memory limit in success path: %lu\n", memory_limit);
		//pthread_mutex_lock(&mlock);
		ret = memory_limit > __QUOTA__ ?  __QUOTA__ : memory_limit ;

		memory_limit -= ret;

		printf("[dbg] handle mem req: This is the memory limit in success path after minus ret: %lu\n", memory_limit);
		pthread_mutex_unlock(&mlock);

		return req->mem_limit + ret;
	
	failed:
		pthread_mutex_unlock(&mlock);
		printf("[INFO] No memory available on elastic container!\n");
		return fail;

}

void add_cgroup_id_to_ec(ec_message_t* req, int client_fd) {
	int i;
	pthread_mutex_lock(&mlock);

	for (i = num_of_clients-1; i >= 0; --i){
		if (ec_cgroups[i]->cgroup_fd == client_fd){
			ec_cgroups[i]->cgroup_id = req->cgroup_id;
			printf("Cgroup file descriptor alongside its id: %d, %d \n", client_fd,  ec_cgroups[i]->cgroup_id);
			break;
		}
	}
	// TODO: Add response here for the GCM init process..	

	pthread_mutex_unlock(&mlock);
}

unsigned long handle_req(char* buffer, int client_fd) {

	ec_message_t* req = (ec_message_t*) buffer;
	unsigned long ret = __FAILED__;
	printf("[dbg] maximum memory read from container: %lu\n", req->mem_limit);
	//printf("[dbg] cgroup id of the container: %d\n", req->cgroup_id);
	switch ( req -> req_type ) {

		case true:
			ret = handle_mem_req(req, client_fd);
			break;

		case false:
			//ret = handle_cpu_req(req);
			printf("[dbg] Handling cpu stuff\n");
			break;

		case _init_:
			add_cgroup_id_to_ec(req, client_fd);
			break;

		default:
			perror("[ERROR] Global Cloud Manager: handling memory/cpu request failed!");
	}

	return ret;
}

//Greg: recall this is for one EC
void *handle_client_reqs(void* args) {

	int num_bytes, i;
	unsigned long ret;
	char buffer[__BUFFSIZE__];
	serv_thread_args_t* arguments = (serv_thread_args_t*) args;
	int client_fd = arguments->clifd;

	ec_client_t* ec_cli_ = (ec_client_t*) malloc(sizeof(ec_client_t*));			
	ec_cli_ -> cliaddr = arguments->cliaddr;
	ec_cli_ -> cgroup_fd = arguments->clifd;

	//pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;

	pthread_mutex_lock(&mlock);

	//Greg: I think this is like our map of cgroups
	//Greg: Think client here is number of cgroups within this EC
	ec_cgroups[num_of_clients] = ec_cli_;
	
	num_of_clients ++;

	pthread_mutex_unlock(&mlock);

	printf("[SUCCESS] GCM thread: new connection created! new socket fd: %d and it is added to ec group\n", client_fd);

	bzero(buffer, __BUFFSIZE__);
	while( (num_bytes = read(client_fd, buffer, __BUFFSIZE__) ) > 0 ){

		ret = 0;
		//printf("[dbg] Number of bytes read: %d\n", num_bytes);
		ret = handle_req(buffer, client_fd);
		//printf("[dbg] handle request return value: %ld\n", ret);
		if (ret > 0)
		{
			printf("[dbg] We got: %lu from handle request function!\n", ret);
			if(write(client_fd, (const char*) &ret,  sizeof(unsigned long) ) < 0){
				perror("[dbg] Writing to socket failed!");
				break;
			}
		}
		else {
			printf("[FAILD] GCM Thread: [%lu] Unable to handle request!\n", mem_reqs++);
			if (write(client_fd, (const char*) &ret,  sizeof(unsigned long) ) < 0)
			{
				perror("[dbg] Writing to socket failed!");
				break;
			}
			//break;
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
	serv_thread_args_t* args;
	gcm_server = create_mt_server();

	//these root pids are gonna be inputs to the server main program
	cgroups_root_pid[0] = 0;
	cgroups_root_pid[1] = 0;
	cgroups_root_pid[2] = 0;
	cgroups_root_pid[3] = 0;
	FD_ZERO(&readfds);
	max_sd = (gcm_server -> gcm_socket_fd) + 1;
	cliaddr_len = sizeof(cliaddr);
	printf("[dbg] Maximum socket descriptor is: %d\n", max_sd);

	while(true) {

		FD_SET(gcm_server -> gcm_socket_fd, &readfds);

		select_rv = select(max_sd, &readfds, NULL, NULL, NULL);

		printf("[dbg] An event happend on the server socket!\n");

		if ( FD_ISSET(gcm_server -> gcm_socket_fd, &readfds) ) {
			if ( (clifd = accept(gcm_server -> gcm_socket_fd, (struct sockaddr*)&cliaddr, &cliaddr_len )) > 0){
				args = (serv_thread_args_t*)malloc(sizeof(serv_thread_args_t));
				args->clifd = clifd;
				args->cliaddr = &cliaddr;
				if ( pthread_create(&threads[num_of_clients], NULL, handle_client_reqs, (void*) args ) ){

					perror("[ERROR] Global Cloud Manager: Unable to create GCM server thread!");
					break;
				}
				else {
					
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

	agent_t* agent1 = (agent_t*)malloc(sizeof(agent_t));

	agent1 -> ip = "127.0.0.1";

	agent1 -> port = 4445;

	ec_agents[0] = agent1;

	init_agents_connection( 1);

	main_loop (gcm_server);

	return 0;
}
