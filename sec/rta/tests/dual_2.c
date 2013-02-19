#include <stdio.h>
#include "flib/rta.h"

enum rta_sec_era rta_sec_era;

int dual_2(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;
	uint64_t ctx = (uint64_t) 0xdf8093280ull;
	int ctx_size = 16;
	uint64_t cipher_key = (uint64_t) 0x5c36c3700ull;
	int cipher_key_size = 16;
	uint64_t auth_key = (uint64_t) 0xc835c4200ull;
	int auth_key_size = 4;
	uint64_t auth = (uint64_t) 0x150c8e140ull;
	int auth_size = 31;
	uint64_t ct_in = (uint64_t) 0x103e40740ull;
	uint64_t pt_out = (uint64_t) 0x7dfa8ff00ull;
	int msg_len = 128;
	uint64_t icv = (uint64_t) 0x91cad53b0ull;
	int icv_size = 32;

	PROGRAM_CNTXT_INIT(buff, 0);
	PROGRAM_SET_36BIT_ADDR();

	JOB_HDR(SHR_NEVER, 0, 0);
	{
		LOAD(PTR(ctx), CONTEXT1, 0, ctx_size, 0);
		KEY(KEY1, 0, PTR(cipher_key), cipher_key_size, 0);
		KEY(KEY2, 0, PTR(auth_key), auth_key_size, 0);
		ALG_OPERATION(OP_ALG_ALGSEL_SHA256, OP_ALG_AAI_HMAC,
			      OP_ALG_AS_INITFINAL, ICV_CHECK_ENABLE,
			      OP_ALG_ENCRYPT);
		MOVE(CONTEXT1, 0, IFIFOAB2, 0, IMM(ctx_size), WITH(WAITCOMP));
		ALG_OPERATION(OP_ALG_ALGSEL_AES, OP_ALG_AAI_CBC,
			      OP_ALG_AS_INITFINAL, 0, OP_ALG_DECRYPT);
		FIFOLOAD(MSG2, PTR(auth), auth_size, 0);
		FIFOLOAD(MSGOUTSNOOP, PTR(ct_in), msg_len, WITH(LAST1 | LAST2));
		FIFOSTORE(MSG, 0, pt_out, msg_len, 0);
		FIFOLOAD(ICV2, PTR(icv), icv_size, WITH(LAST2));
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

	printf("DUAL_2 example program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = dual_2((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
