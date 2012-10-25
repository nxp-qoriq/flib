#include <stdio.h>
#include "rta.h"

int hmac_1(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;
	uint64_t key_data = (uint64_t) 0xacd1f8000ull;
	int keylen = 64;
	uint64_t msg = (uint64_t) 0xef7452b00ull;
	int msglen = 1601;
	uint64_t hmac = (uint64_t) 0x859adb800ull;

	PROGRAM_CNTXT_INIT(buff, 0, 0);
	PROGRAM_SET_36BIT_ADDR();

	JOB_HDR(SHR_NEVER, 0, 0);
	{
		KEY(MDHA_SPLIT_KEY, 0, PTR(key_data), keylen, 0);
		ALG_OPERATION(OP_ALG_ALGSEL_SHA256, OP_ALG_AAI_HMAC_PRECOMP,
			      OP_ALG_AS_INITFINAL, 0, OP_ALG_ENCRYPT);
		FIFOLOAD(MSG2, PTR(msg), msglen, WITH(LAST2 | EXT));
		STORE(CONTEXT2, 0, PTR(hmac), 32, 0);
	}
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
	printf("HMAC_1 example program\n");
	size = hmac_1((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);
	return 0;
}
