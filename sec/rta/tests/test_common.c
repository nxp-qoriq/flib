#include <stdint.h>
#include <stdio.h>
#include "test_common.h"

void print_prog(uint32_t *buff, int size)
{
	int i;

	for (i = 0; i < size; i++)
		printf("%08x\n", buff[i]);
}
