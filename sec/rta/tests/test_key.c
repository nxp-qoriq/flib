#include <stdio.h>
#include "flib/rta.h"

enum rta_sec_era rta_sec_era;

int test_key_op(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;
	uint8_t key_imm[] = { 0x12, 0x13, 0x14, 0x15 };
	intptr_t addr = (intptr_t) &key_imm;

	PROGRAM_CNTXT_INIT(buff, 0);
	KEY(MDHA_SPLIT_KEY, WITH(ENC), IMM(addr), 4, 0);
	KEY(MDHA_SPLIT_KEY, WITH(ENC), PTR(addr), 4, 0);
	size = KEY(KEY1, WITH(EKT), IMM(addr), 4, 0);

	return size;
}

int prg_buff[1000];

static void print_prog(uint32_t *buff, int size)
{
	int i;
	for (i = 0; i < size; i++)
		printf("0x%08x\n", buff[i]);
}

int main(int argc, char **argv)
{
	int size;

	printf("KEY program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = test_key_op((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
