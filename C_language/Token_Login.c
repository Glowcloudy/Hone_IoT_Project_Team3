#ifndef __SERVER__HEADER__
#define __SERVER__HEADER__
#include "Server_Header.h"
#endif

// Make 'Login Message' to 'Login Structure'
void tokenLogin(Login *arg, char *msg) {

    char *tempArr[3] = {NULL,NULL,NULL};
    int count = 0;
    int isCorrupted = 0;

	char *ptr = strtok(msg, ":");

	while(ptr != NULL) {

        if(count > 3) {
            isCorrupted = 1;
			break;
        }

        tempArr[count] = ptr;
        count++;

		ptr = strtok(NULL, ":");
	}

    //strtok fail
    if(isCorrupted == 1 || count != 3) {
        strcpy(arg->ID, "FAIL");
        strcpy(arg->PW, "FAIL");
        arg->login_MODE = -1;
    }
    //strtok sucess
    else {
        strcpy(arg->ID, tempArr[0]);
        strcpy(arg->PW, tempArr[1]);
        arg->login_MODE = atoi(tempArr[2]);
    }
}