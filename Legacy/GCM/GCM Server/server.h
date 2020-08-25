#ifndef __GCM__
#define __GCM__

#include <stdio.h>  
#include <string.h>   //strlen  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close  
#include <netinet/tcp.h>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h> //Macros
#include <pthread.h>

#define true 1
#define false 0
#define _init_ 2
#define __PORT__ 4444
#define __MAX_CLIENT__ 30
#define __BUFFSIZE__ 1024
#define __MAXSERVERS__ 10
#define __FAILED__ -1
#define __NOMEM__ -2
#define __QUOTA__ 5000

int cgroup_fds[__MAX_CLIENT__];
unsigned long memory_limit = 30000;
unsigned long mem_reqs = 0;
unsigned long cpu_limit = 500000;

int num_of_clients = 0;
int num_of_agents = 0;

int cgroups_root_pid[__MAX_CLIENT__];
pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;

ec_client_t* ec_cgroups[__MAX_CLIENT__];

typedef int _bool;


typedef struct gcm_server {

	int gcm_socket_fd;

	struct sockaddr_in address;

} gcm_server_t;

typedef struct ec_msg {
	uint32_t client_ip;
    uint32_t cgroup_id;
    _bool req_type;
    uint64_t rsrc_amnt;
    _bool request;

} ec_message_t;



typedef struct ec_reclaim_msg {

	unsigned short cgroup_id;

	int is_mem;

	//...maybe it needs more things

} ec_reclaim_msg_t;

typedef struct ec_cli
{
	struct sockaddr_in* cliaddr;

	int cgroup_fd;

	unsigned short cgroup_id;
	//... Maybe it will need more info. Not now
} ec_client_t;



typedef struct serv_thread_args
{
	int clifd;

	struct sockaddr_in* cliaddr;
}serv_thread_args_t;

typedef struct agent
{
	char* ip;
	
	int port;

	int sockfd;

}agent_t;

agent_t* ec_agents[__MAXSERVERS__];

ec_message_t* handle_req(char* buffer);

unsigned long handle_req(char* buffer, int client_fd);

void *handle_client_reqs(void* args);

gcm_server_t* create_mt_server();

ec_message_t* handle_mem_req(ec_message_t* req);

ec_message_t* handle_cpu_req(ec_message_t* req);

ec_message_t* handle_init_req(ec_message_t* req);


unsigned long handle_mem_req(ec_message_t* req, int client_fd);

unsigned long reclaim_memory(int client_fd);

void add_cgroup_id_to_ec(ec_message_t* req, int client_fd);

void init_agents_connection(int num_of_agents);


#endif
