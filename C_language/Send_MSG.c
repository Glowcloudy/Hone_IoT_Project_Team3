#ifndef __SERVER__HEADER__
#define __SERVER__HEADER__
#include "Server_Header.h"
#endif

// Send message
void send_msg(char *msg, int len, int socket_num) { 

	pthread_mutex_lock(&mutex_c);
	for(int i = 0; i < cl_num; i++) {

		if(cl_socks[i].num_sock == socket_num)
			continue;

		write(cl_socks[i].num_sock, msg, len);
	}
	pthread_mutex_unlock(&mutex_c);
}