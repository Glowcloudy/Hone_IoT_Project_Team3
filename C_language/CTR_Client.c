#ifndef __SERVER__HEADER__
#define __SERVER__HEADER__
#include "Server_Header.h"
#endif

// Message Token
void tokenMessage(char *msg) {

	DB_Command command;
	char *tempArr[3] = {NULL,NULL,NULL};
	int i = 0;
	int isCorrupted = 0;

	memset(&command, 0, sizeof(struct DB_Command));   // init DB structure
	char *ptr = strtok(msg, ":");

	while(ptr != NULL) {

		if(i >= 3) {
			isCorrupted = 1;
			break;
		}

        tempArr[i] = ptr;
		i++;

		ptr = strtok(NULL, ":");
	}

	if(isCorrupted == 1 || tempArr[0] == NULL || tempArr[1] == NULL) {
		printf("ERROR : Data is Corrupted.\n");
		return;
	}
	else {
		if(tempArr[2] != NULL) {
			strncpy(command.command, "INSERT", sizeof(command.command));
			strncpy(command.db_name, tempArr[0], sizeof(command.db_name));
			strncpy(command.table_name, "VALUE", sizeof(command.table_name));
			strncpy(command.arg_name, "NULL", sizeof(command.arg_name));
			command.value1 = time(NULL);
			command.value2 = atoi(tempArr[1]);
			command.value3 = atoi(tempArr[2]);

			db_Command(command);
		}
		else {
			strncpy(command.command, "INSERT", sizeof(command.command));
			strncpy(command.db_name, tempArr[0], sizeof(command.db_name));
			strncpy(command.table_name, "VALUE", sizeof(command.table_name));
			strncpy(command.arg_name, "NULL", sizeof(command.arg_name));
			command.value1 = time(NULL);
			command.value2 = atoi(tempArr[1]);
			command.value3 = 999;

			db_Command(command);
		}
	}
}

// CLient Control Thread Function
void * ctr_Client(void * arg) {
	
	// Structure Copy
	Client CL = *(Client *)arg;
	
	int str_len = 0;
	char msg[BUF_SIZE];
	
	while(1) {
		
		// Init msg array to 0
		memset(msg, 0, sizeof(msg));

		// Read Socket Message
		str_len = read(CL.num_sock, msg, sizeof(msg));
		
		#ifdef __DEBUG_
		printf("%d socket message, %s\n", CL.num_sock, msg);
		#endif
		
		// if read() return <=0, break loop and excute disconnect code
		if(str_len <= 0) {
			break;
		}

		// Sensor Data Token
		if(strstr(msg, "LIGHT") != NULL || strstr(msg, "HT") != NULL || strstr(msg, "SOUND") != NULL) {
			tokenMessage(msg);
		}
		
		// Send message to all Client
		send_msg(msg, str_len, CL.num_sock);
	}
	
	// Disconnect Code
	pthread_mutex_lock(&mutex_c);
	for(int i = 0; i < cl_num; i++) {
		if(CL.num_sock == cl_socks[i].num_sock) {
			for(int j = i; j < cl_num; j++)
				cl_socks[j] = cl_socks[j + 1];
		}
	}
	cl_num--;
	#ifdef __DEBUG_
	printf("%d socket disconnect! %d\n", CL.num_sock, cl_num);
	#endif
	pthread_mutex_unlock(&mutex_c);

	// Socket close
	close(CL.num_sock);
	
	return NULL;
}