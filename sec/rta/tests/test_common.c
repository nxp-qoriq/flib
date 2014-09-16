/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#include <stdint.h>
#include <stdio.h>
#include "test_common.h"

void print_prog(uint32_t *buff, int size)
{
	int i;

	if (size < 0) {
		printf("Invalid descriptor size (%d)\n", size);
		return;
	}

	for (i = 0; i < size; i++)
		printf("%08x\n", buff[i]);
}
