#ifndef __SERVER__HEADER__
#define __SERVER__HEADER__
#include "Server_Header.h"
#endif

// Define ERROR MESSAGE
const char LOG_ERR_A[]      = "ALREADY_LOGINED\n";
const char LOG_ERR_F[]      = "LOGIN_FAILED\n";
const char LOG_SUC[]        = "LOGIN_SUCCESS\n";
const char LOG_SUC_DEBUG[]  = "LOGIN_SUCCESS_DEBUG\n";

// Client Connection accept *FORM( ID:PASSWD:LOGINMODE )
Client cl_Accept(int sv_Sock) {

	struct sockaddr_in cl;       // Client Socket Struct
	int cl_Sock;                 // Client Socket Distritor
	int cl_size = sizeof(cl);    // Client Struct Size
	
	int read_b;                  // Read bytes
	char read_msg[BUF_SIZE];     // Read Message

	Client __currentClient;       // Client Structure for login success
	Login __currentLogin;         // LOGIN Structure
	
	// init Struct
	memset(&__currentClient, 0, sizeof(struct Client));
	memset(&__currentLogin, 0, sizeof(struct Login));

	// Accept connection 
	cl_Sock = accept(sv_Sock, (struct sockaddr*)&cl, &cl_size);
	
	// If accept fail, return __currentClient(Socket Number : -1)
	if(cl_Sock < 0) {
		#ifdef __DEBUG_
		perror("accept fail.");
		#endif
		
		__currentClient.num_sock = -1;
		return __currentClient;
	}
	
	// Read login message 
	read_b = read(cl_Sock, &read_msg, BUF_SIZE);
	read_msg[read_b] = '\0';
	#ifdef __DEBUG_
	printf("Read Complete. %s\n", read_msg);
	#endif
	
	// If read login message, return __currentClient(Socket Number : -1)
	if(read_b <= 0 ) {
		#ifdef __DEBUG_
		printf("Read Login Info faild\n");
		#endif
		
		// Write error message to client socket
		write(cl_Sock, LOG_ERR_F, sizeof(LOG_ERR_F));

		__currentClient.num_sock = -1;
		close(cl_Sock);
		return __currentClient;
	}
	
	#ifdef __DEBUG_
	printf("Read Check Complete.\n");
	#endif
	
	// Tokenization login message
	tokenLogin(&__currentLogin, read_msg);
	
	// If tokenization login message , return __currentClient(Socket Number : -1)
	if(strcmp(__currentLogin.ID, "FAIL") == 0 && strcmp(__currentLogin.PW, "FAIL") == 0) {
		
		#ifdef __DEBUG_
		printf("Login Structure corrupted.\n");
		#endif
		
		// Write error message to client socket
		write(cl_Sock, LOG_ERR_F, sizeof(LOG_ERR_F));

		__currentClient.num_sock = -1;
		close(cl_Sock);
		return __currentClient;
	}
	
	// Check Same connection
	pthread_mutex_lock(&mutex_c);
	for(int i = 0; i <= cl_num; i++) {
		if(cl_socks[i].ID == __currentLogin.ID) {
			
			// Same Connection Detect, return __currentClient(Socket Number : -1)
			#ifdef __DEBUG_
			printf("ID : %s is already Login!\n", __currentLogin.ID);
			#endif

			// Write error message to client socket
			write(cl_Sock, LOG_ERR_A, sizeof(LOG_ERR_A));

			__currentClient.num_sock = -1;
			close(cl_Sock);
			// Need to unlock mutex in detect same connection situation
			pthread_mutex_unlock(&mutex_c);
			return __currentClient;
		}
	}
	pthread_mutex_unlock(&mutex_c);
	
	#ifdef __DEBUG_
	printf("ID : %s, PASSWORD(HASH) : %s LOGIN MODE : %d\n", __currentLogin.ID, __currentLogin.PW , __currentLogin.login_MODE);
	#endif
	
	// Login Mode Check
	if(__currentLogin.login_MODE < 0 || __currentLogin.login_MODE > 2 ) {
		
		// Detect wrong login mode, return __currentClient(Socket Number : -1)
		#ifdef __DEBUG_
		printf("ID : %s is Wrong Login Mode!\n", __currentLogin.ID);
		#endif
		
		// Write error message to client socket
		write(cl_Sock, LOG_ERR_F, sizeof(LOG_ERR_F));
		__currentClient.num_sock = -1;
		close(cl_Sock);
		return __currentClient;
	}
	
	// Debug Mode Login Process
	if(__currentLogin.login_MODE == 2) { 
		
		#ifdef __DEBUG_ 
		printf("ID : %s in DEBUG MODE!\n", __currentLogin.ID);
		#endif
		
		// Write Login clear message
		write(cl_Sock, LOG_SUC_DEBUG, sizeof(LOG_SUC_DEBUG));
		
		// Client structure set
		strncpy(__currentClient.ID, __currentLogin.ID, strlen(__currentLogin.ID));   // ID SET
		__currentClient.num_sock = cl_Sock;                                          // Socket Number Set
		memcpy(&(__currentClient.info_client), &cl, sizeof(struct sockaddr_in));     // Socket Structure Set
		__currentClient.login_MODE = __currentLogin.login_MODE;                      // Login Mode Set
		
		#ifdef __DEBUG_
		printf("ID(client_debug) : %s Check.\n", __currentClient.ID);
		printf("ID(login_debug) : %s Check.\n", __currentLogin.ID);
		#endif
		
		// retrun __currentClient(Socket Number : connected socket number)
		return __currentClient;
	}
	
	// Auth Check start
	if(auth(__currentLogin) < 0) {
		
		// If Auth fail, return __currentClient(Socket Number : -1)
		#ifdef __DEBUG_
		printf("ID : %s Auth Fail.\n", __currentLogin.ID);
		#endif
		
		// Write error message to client socket
		write(cl_Sock, LOG_ERR_F, sizeof(LOG_ERR_F));

		__currentClient.num_sock = -1;
		close(cl_Sock);
		return __currentClient;
	}
	
	// Client structure set
	strncpy(__currentClient.ID, __currentLogin.ID, strlen(__currentLogin.ID));
	__currentClient.num_sock = cl_Sock;
	memcpy(&(__currentClient.info_client), &cl, sizeof(struct sockaddr_in));
	__currentClient.login_MODE = __currentLogin.login_MODE;
	
	#ifdef __DEBUG_
	printf("ID(client) : %s Check.\n", __currentClient.ID);
	printf("ID(login) : %s Check.\n", __currentLogin.ID);
	#endif
	
	// Write Login clear message to client socket
	write(cl_Sock, LOG_SUC, sizeof(LOG_SUC));
	
	return __currentClient;
}