#ifndef __SERVER__HEADER__
#define __SERVER__HEADER__
#include "Server_Header.h"
#endif

int server_Init(char *port) {
	
	int sv_Sock;
	struct sockaddr_in sv;
	int reuseADDR = 1;
	
	printf("서버 Config 읽는 중입니다....\n");
	read_Config();

	printf("초기화 완료, 서버 소켓 생성중입니다....\n");
	sv_Sock = socket(PF_INET, SOCK_STREAM, 0);
	
	if(sv_Sock < 0) {
		perror("socket fail.");
		exit(1);
	}
	
	// Set Socket REUSEADDR Option
	if(setsockopt(sv_Sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseADDR, sizeof(reuseADDR)) < 0) {
		perror("SERVER Setting Socket Option fail.");
		exit(1);
	}
	
	memset(&sv, 0, sizeof(sv));
	sv.sin_family = AF_INET;
	sv.sin_addr.s_addr = htonl(INADDR_ANY);
	sv.sin_port = htons(atoi(port));
	
	// Bind Fail.
	if(bind(sv_Sock, (struct sockaddr*) &sv, sizeof(sv)) < 0) {
		perror("bind error");
		exit(1);
	}
	
	// Listen Fail.
	if(listen(sv_Sock, 5) < 0) {
		perror("listen error");
		exit(1);
	}
	
	return sv_Sock;
}