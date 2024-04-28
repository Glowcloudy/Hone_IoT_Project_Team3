#ifndef __SERVER__HEADER__
#define __SERVER__HEADER__
#include "Server_Header.h"
#endif

int main(int argc, char *argv[]) {
	
	cl_num = 0;                        // Init Client Socket Number 
	int sv_Sock, cl_Sock;              // Socket Discripter
	pthread_t t_id;
	
	Client sock_client;                // Socket Client Structure
	
	if(argc != 2) {
		printf("Usage : %s <PORT>\n", argv[0]);
		exit(1);
	}
	
	pthread_mutex_init(&mutex_c, NULL);  // Mutex init
	sv_Sock = server_Init(argv[1]);      // Server init
	
	// Server Command Thread
	#ifndef __DEBUG_
	pthread_create(&t_id, NULL, sv_Function, (void*)NULL);
	#endif
	
	// MAIN THREAD
	while(1) {
		
		sock_client = cl_Accept(sv_Sock);    // Client Connect accept
		if(sock_client.num_sock == -1)       // If Client Connect failed, ignore failed client socket
			continue;                  
			
		pthread_mutex_lock(&mutex_c);                 
		cl_socks[cl_num++] = sock_client;    // Socket Insert Array
		
		#ifdef __DEBUG_
		printf("%d socket! %d\n", sock_client.num_sock, cl_num);
		#endif
		
		pthread_mutex_unlock(&mutex_c);               
		
		// CLient Thread Create
		pthread_create(&t_id, NULL, ctr_Client, (void*)&sock_client);
		pthread_detach(t_id);
	}
	
	close(sv_Sock); // Socket Close.
	return 0;
}