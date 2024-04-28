//=========================================================================================//
//                              NAME : SIMPLE IOT SERVER                                   //
//                                  AUTHOR : WTPENGUIN                                     //
//                                    VERSION : 0.01                                       //
//=========================================================================================//

#ifndef __SERVER_HEADER__
#define __SERVER_HEADER__

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>

//for Rasbian
//#include "/usr/include/mariadb/mysql.h"

//for Ubuntu
#include "/usr/include/mysql/mysql.h"

//USE JSON-C(API Document : http://json-c.github.io/json-c/json-c-0.15/doc/html/index.html)
#include "/usr/local/include/json-c/json.h"

// Define for Client Structure
#define BUF_SIZE      800
#define MAX_CLNT      50
#define MAX_ID        20
#define MAX_PASSWD    20

// CLient Structure
typedef struct Client {
	struct sockaddr_in info_client;
	int num_sock;
	char ID[MAX_ID];

	// Login Mode : 1 - General Mode, 2 - System Mode
	int login_MODE;
}Client;

// Login Structure
typedef struct Login {
	char ID[MAX_ID];
	char PW[MAX_PASSWD];
	int login_MODE;
}Login;

// Config Structure
typedef struct Config {
    char HOST[16];
    char USER[21];
    char PASS[21];
    int PORT;
}Config;

// DB Structure
typedef struct DB_Command {
	char command[7];
	char db_name[8];
	char table_name[6];
	char arg_name[MAX_ID];
	char arg_pass[MAX_PASSWD];
	int value1;
	int value2;
	int value3;
}DB_Command;

// Funcion Define
int server_Init(char *port);                                  // Server Init Func
Client cl_Accept(int sv_Sock);                                // Client connection accept Func
void * ctr_Client(void * arg);                                // Client Handle Func(Thread)
void * sv_Function(void *arg);                                // Server Command Func(Thread)
void config(void);                                            // Server Config
void send_msg(char *msg, int len, int socket_num);            // Message Send Func
void tokenLogin(Login *arg, char *msg);                       // Token Login Message
void read_Config(void);                                       // Read Configure JSON File
int auth(Login USER);                                         // Authentication Function
char* hash_my_password(const char *password);                 // Function for Hash Passward
void db_Command(DB_Command com);                              // Function for DB

// Global Variable
int cl_num;           		 // Client Number
Client cl_socks[MAX_CLNT];   // Client Socket Array
pthread_mutex_t mutex_c;  	 // Mutex
Config cfg;					 // Config

#endif