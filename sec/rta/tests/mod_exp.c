/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;

uint8_t modulus[20] = {
	0x9A, 0x27, 0x7B, 0x10, 0x42, 0x3F, 0xEC, 0xDB,
	0x0F, 0x3B, 0x1A, 0x52, 0x2A, 0x61, 0x2D, 0x14,
	0x6E, 0xA3, 0x45, 0x4D
};

uint8_t exponent[20] = {
	0x52, 0x06, 0xF8, 0x66, 0xFF, 0x00, 0x2A, 0x71,
	0x1F, 0xCF, 0x40, 0x4D, 0x58, 0x9F, 0xEC, 0x74,
	0xB1, 0xBB, 0x82, 0xF1
};

uint8_t generator[2] = {
	0x00, 0x02
};

unsigned mod_exp(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int field_size = 20;
	uint64_t mod = (uint64_t) 0x08049668;	/* I/O address for modulus; */
	uint64_t exp = (uint64_t) 0x0804967C;	/* I/O address for exponent; */
	uint64_t base = (uint64_t) 0x08049690;	/* I/O address for generator; */
	uint64_t res = (uint64_t) 0x332244514ull;

	PROGRAM_CNTXT_INIT(buff, 0);
	JOB_HDR(SHR_NEVER, 0, 0, 0);
	{
		FIFOLOAD(PKN, PTR(mod), field_size, 0);
		KEY(PKE, 0, PTR(exp), 20, 0);
		FIFOLOAD(PKA, PTR(base + 1), 1, 0);
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_EXPO);
		JUMP(IMM(0x42), HALT_STATUS, ALL_TRUE, WITH(PK_0));
		FIFOSTORE(PKB, 0, res, field_size, 0);
	}

	return PROGRAM_FINALIZE();
}

uint32_t prg_buff[1000];

int main(int argc, char **argv)
{
	unsigned size;

	pr_debug("MOD_EXP example program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = mod_exp(prg_buff);
	pr_debug("size = %d\n", size);
	print_prog(prg_buff, size);

	return 0;
}
