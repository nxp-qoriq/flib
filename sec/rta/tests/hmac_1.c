/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;

unsigned hmac_1(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	uint64_t key_data = (uint64_t) 0xacd1f8000ull;
	int keylen = 64;
	uint64_t msg = (uint64_t) 0xef7452b00ull;
	int msglen = 1601;
	uint64_t hmac = (uint64_t) 0x859adb800ull;

	PROGRAM_CNTXT_INIT(buff, 0);
	PROGRAM_SET_36BIT_ADDR();

	JOB_HDR(SHR_NEVER, 0, 0, 0);
	{
		KEY(MDHA_SPLIT_KEY, 0, PTR(key_data), keylen, 0);
		ALG_OPERATION(OP_ALG_ALGSEL_SHA256, OP_ALG_AAI_HMAC_PRECOMP,
			      OP_ALG_AS_INITFINAL, 0, OP_ALG_ENCRYPT);
		FIFOLOAD(MSG2, PTR(msg), msglen, LAST2 | EXT);
		STORE(CONTEXT2, 0, PTR(hmac), 32, 0);
	}

	return PROGRAM_FINALIZE();
}

uint32_t prg_buff[1000];

int main(int argc, char **argv)
{
	unsigned size;

	pr_debug("HMAC_1 example program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = hmac_1(prg_buff);
	pr_debug("size = %d\n", size);
	print_prog(prg_buff, size);

	return 0;
}
