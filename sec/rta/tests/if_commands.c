#include <stdio.h>
#include "flib/rta.h"

int if_command_test(uint32_t *buff, uint8_t len)
{
	struct program prg;
	struct program *program = &prg;
	int size;

	PROGRAM_SET_36BIT_ADDR();

	PROGRAM_CNTXT_INIT(buff, 0, 0);

	if (len == 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(15), 0);
	if (len > 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(16), 0);
	else
		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(17), 0);
	if (len > 21)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(16), 0);
	else if (len == 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(18), 0);

	if (len > 21)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(16), 0);
	else if (len == 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(19), 0);
	else if (len > 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(16), 0);

	if (len > 21)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(16), 0);
	else if (len < 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(16), 0);
	else if (len == 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(20), 0);

	if (len > 21)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(16), 0);
	else if (len < 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(16), 0);
	else if (len > 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(16), 0);
	else
		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(22), 0);

	size = PROGRAM_FINALIZE();

	return size;
}

int prg_buff[1000];

static void print_prog(uint32_t *buff, int size)
{
	int i;
	for (i = 0; i < size; i++)
		printf("%08X\n", buff[i]);
}

int main(int argc, char **argv)
{
	int size;
	uint8_t len = 16;

	printf("IF_Command  example program\n");
	size = if_command_test((uint32_t *) prg_buff, len);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);
	return 0;
}
