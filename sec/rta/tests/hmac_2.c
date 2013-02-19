#include <stdio.h>
#include "flib/rta.h"

enum rta_sec_era rta_sec_era;

int hmac_2(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;
	uint64_t key_data = (uint64_t) 0xab8050640ull;
	int keylen = 61;
	uint64_t msg = (uint64_t) 0x806d8f600ull;
	int msglen = 1601;
	uint64_t icv = (uint64_t) 0xde5ebe080ull;

	PROGRAM_CNTXT_INIT(buff, 0);
	PROGRAM_SET_36BIT_ADDR();

	JOB_HDR(SHR_NEVER, 0, 0);
	{
		KEY(KEY2, 0, PTR(key_data), keylen, 0);
		ALG_OPERATION(OP_ALG_ALGSEL_SHA256, OP_ALG_AAI_HMAC,
			      OP_ALG_AS_INITFINAL, ICV_CHECK_ENABLE,
			      OP_ALG_DECRYPT);
		FIFOLOAD(MSG2, PTR(msg), msglen, WITH(LAST2 | EXT));
		FIFOLOAD(ICV2, PTR(icv), 32, WITH(LAST2));
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

	printf("HMAC_2 example program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = hmac_2((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
