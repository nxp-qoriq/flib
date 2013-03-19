#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;

/*
 *  Test variable length of immediate byte data in a command.
 *  This only works in Style 3.
 */
uint8_t modulus[20] = {
	0x9A, 0x27, 0x7B, 0x10, 0x42, 0x3F, 0xEC, 0xDB,
	0x0F, 0x3B, 0x1A, 0x52, 0x2A, 0x61, 0x2D, 0x14,
	0x6E, 0xA3, 0x45, 0x4D
};				/* [] modulus */

uint8_t secret[20] = {
	0x42, 0x35, 0x1a, 0x45, 0x67, 0x71, 0x54, 0xf4,
	0xb3, 0x2f, 0xc9, 0x98, 0xac, 0x42, 0x11, 0x43,
	0x53, 0xA2, 0x10, 0x08
};				/* [] secret */

int var_test(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;
	int secret_len = sizeof(secret);
	uint64_t secret_out = 0x3000200;

	PROGRAM_SET_36BIT_ADDR();

	PROGRAM_CNTXT_INIT(buff, 0);
	/* RSA Encrypt */
	JOB_HDR(SHR_NEVER, 0, 0, 0);
	{
		FIFOLOAD(PKN, PTR((intptr_t) &modulus), (sizeof(modulus)),
			 WITH(IMMED));
		FIFOLOAD(PKA, PTR((intptr_t) &secret), secret_len,
			 WITH(IMMED));
		KEY(PKE, 0, IMM(0x03), 1, 0);
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_EXPO);
		FIFOSTORE(PKB, 0, secret_out, (sizeof(modulus)), 0);
	}
	size = PROGRAM_FINALIZE();

	return size;
}

int prg_buff[1000];

int main(int argc, char **argv)
{
	int size;

	printf("VAR_1 example program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = var_test((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
