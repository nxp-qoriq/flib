#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;

int test_decryption(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;
	uint64_t data_in = (uint64_t) 0x82ba84e1cull;
	uint64_t data_out = (uint64_t) 0x582007840ull;
	uint64_t key_data = (uint64_t) 0xdd5fa8880ull;
	uint64_t iv = (uint64_t) 0x8c0b21d80ull;
	int datasz = 1024;
	int ivlen = 16;
	int keylen = 16;

	PROGRAM_CNTXT_INIT(buff, 0);
	PROGRAM_SET_36BIT_ADDR();

	JOB_HDR(SHR_NEVER, 0, 0, 0);
	{
		LOAD(PTR(iv), CONTEXT1, 0, ivlen, 0);
		KEY(KEY1, 0, PTR(key_data), keylen, 0);
		ALG_OPERATION(OP_ALG_ALGSEL_AES, OP_ALG_AAI_CBC,
			      OP_ALG_AS_INITFINAL, 0, OP_ALG_DECRYPT);
		FIFOLOAD(MSG1, PTR(data_in), datasz, WITH(LAST1));
		FIFOSTORE(MSG, 0, data_out, datasz, WITH(EXT));
	}
	size = PROGRAM_FINALIZE();
	return size;
}

int prg_buff[1000];

int main(int argc, char **argv)
{
	int size;

	printf("Decryption program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = test_decryption((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
