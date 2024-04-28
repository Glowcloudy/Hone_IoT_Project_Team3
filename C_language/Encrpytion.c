/*
Generate binary hash from raw text string
Used for Pre-4.1 password handling
SYNOPSIS
hash_password()
result       OUT store hash in this location
password     IN  plain text password to build hash
password_len IN  password length (password may be not null-terminated)
*/

#ifndef __SERVER__HEADER__
#define __SERVER__HEADER__
#include "Server_Header.h"
#endif

char* hash_my_password(const char *password) {

	unsigned long *result = (unsigned long*)malloc(sizeof(unsigned long) * 2);
	unsigned int password_len = strlen(password);

	register unsigned long nr = 1345345333L, add = 7, nr2 = 0x12345671L;
	unsigned long tmp;
	const char *password_end = password + password_len;

	// Not malloc for Aurduino
	char *p_s1 = malloc(sizeof(char) * 16);

	for (; password < password_end; password++)
	{
		if (*password == ' ' || *password == '\t')
			continue;
		tmp = (unsigned long)(unsigned char)*password;
		nr ^= (((nr & 63) + add)*tmp) + (nr << 8);
		nr2 += (nr2 << 8) ^ nr;
		add += tmp;
	}
	result[0] = nr & (((unsigned long)1L << 31) - 1L);
	result[1] = nr2 & (((unsigned long)1L << 31) - 1L);

	sprintf(p_s1, "%08lx%08lx", result[0], result[1]);

	return p_s1;
	free(p_s1);
}