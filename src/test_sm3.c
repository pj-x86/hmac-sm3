#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sm3.h"

/**
 * 结果验证对比1: echo -n "1111" | openssl sm3
 * 结果验证对比2: echo -n "1111" | gmssl sm3
 */
int main(int argc, char *argv[])
{
	unsigned char digest[SM3_HASH_SIZE];
	char output[SM3_HASH_SIZE*2+1] = {0};

	if (argc < 2)
	{
		printf("usage: %s string\n", argv[0]);
		exit(-1);
	}

	SM3Calc((const unsigned char *)argv[1], strlen(argv[1]), digest);

	for (int i = 0; i < SM3_HASH_SIZE; ++i)
		sprintf(output + (i << 1), "%02x", (unsigned char) digest[i]);
	output[SM3_HASH_SIZE*2] = 0;

	printf("%s 's sm3 digest: %s\n", argv[1], output);

	return 0;
}
