#include <stdio.h>
#include "flib/rta.h"

enum rta_sec_era rta_sec_era;
int prg_buff[1000];

static void print_prog(uint32_t *buff, int size)
{
	int i;
	for (i = 0; i < size; i++)
		printf("%08X\n", buff[i]);
}

int test_fifo_store_op(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;
	uint64_t loc = 0x1224445;

	PROGRAM_CNTXT_INIT(buff, 0);
	FIFOSTORE(MSG, 0, loc, 100000000, 0);
	FIFOSTORE(RNG, 0, loc, 32, 0);
	FIFOSTORE(RNGOFIFO, 0, 0, 1, 0);
	FIFOSTORE(PKA, 0, loc, 10, 0);
	FIFOSTORE(PKA0, 0, loc, 10, 0);
	FIFOSTORE(PKA1, 0, loc, 10, 0);
	FIFOSTORE(PKA2, 0, loc, 40, 0);
	FIFOSTORE(PKA3, 0, loc, 10, 0);
	FIFOSTORE(PKB, 0, loc, 10, 0);
	FIFOSTORE(PKB0, 0, loc, 10, 0);
	FIFOSTORE(PKB1, 0, loc, 10, 0);
	FIFOSTORE(PKB2, 0, loc, 10, 0);
	FIFOSTORE(PKB3, 0, loc, 10, 0);
	FIFOSTORE(PKN, 0, loc, 10, 0);
	/* Encrypted (keys) section - normal */
	FIFOSTORE(KEY1, WITH(NRM), loc, 32, 0);
	FIFOSTORE(KEY2, 0, loc, 16, 0);
	FIFOSTORE(PKE, WITH(JDK | NRM), loc, 10, 0);
	FIFOSTORE(IFIFO, 0, loc, 10, 0);
	FIFOSTORE(OFIFO, WITH(JDK), loc, 10, 0);
	FIFOSTORE(AFHA_SBOX, 0, loc, 258, 0);
	FIFOSTORE(MDHA_SPLIT_KEY, 0, loc, 128, 0);
	/* Encrypted (keys) section - enhanced */
	FIFOSTORE(KEY1, WITH(EKT), loc, 32, 0);
	FIFOSTORE(KEY2, WITH(EKT), loc, 16, 0);
	FIFOSTORE(PKE, WITH(EKT), loc, 10, 0);
	FIFOSTORE(IFIFO, WITH(EKT), loc, 10, 0);
	FIFOSTORE(OFIFO, WITH(EKT), loc, 10, 0);
	FIFOSTORE(AFHA_SBOX, WITH(EKT), loc, 258, 0);
	FIFOSTORE(MDHA_SPLIT_KEY, WITH(EKT), loc, 128, 0);
	/* Encrypted (keys) section - trusted */
	FIFOSTORE(KEY1, WITH(TK), loc, 32, 0);
	FIFOSTORE(KEY2, WITH(TK), loc, 16, 0);
	FIFOSTORE(PKE, WITH(TK), loc, 10, 0);
	FIFOSTORE(IFIFO, WITH(TK), loc, 10, 0);
	FIFOSTORE(OFIFO, WITH(TK), loc, 10, 0);
	FIFOSTORE(AFHA_SBOX, WITH(TK), loc, 258, 0);
	FIFOSTORE(MDHA_SPLIT_KEY, WITH(TK), loc, 128, 0);
	/* Encrypted (keys) section - enhanced, trusted */
	FIFOSTORE(KEY1, WITH(EKT | TK), loc, 32, 0);
	FIFOSTORE(KEY2, WITH(EKT | TK), loc, 16, 0);
	FIFOSTORE(PKE, WITH(EKT | TK), loc, 10, 0);
	FIFOSTORE(IFIFO, WITH(EKT | TK), loc, 10, 0);
	FIFOSTORE(OFIFO, WITH(EKT | TK), loc, 10, 0);
	FIFOSTORE(AFHA_SBOX, WITH(EKT | TK), loc, 258, 0);
	FIFOSTORE(MDHA_SPLIT_KEY, WITH(EKT | TK), loc, 128, 0);
	size = SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));

	return size;
}

int main(int argc, char **argv)
{
	int size;

	printf("FIFOSTORE program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = test_fifo_store_op((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
