#include <stdio.h>
#include "rta.h"

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

int dlc_keygen(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;
	int field_size = 20;
	uint64_t mod = 0x08049668;	/* I/O address for modulus */
	uint64_t private_key = 0x32244514ull;
	uint64_t public_key = 0x32244514ull;

	LABEL(retry);
	REFERENCE(p1_retry);
	REFERENCE(p2_retry);
	REFERENCE(p3_retry);

	PROGRAM_CNTXT_INIT(buff, 0, 0);
	PROGRAM_SET_36BIT_ADDR();

	JOB_HDR(SHR_NEVER, 0, 0);
	{
		/* Step A. Load the modulus (prime) we will be using for DH. */
		FIFOLOAD(PKN, PTR(mod), field_size, 0);	/* N <= Modulus */

		/* Step B. Generate random private key. */
		/* Note that it is 'good practice' to generate eight (8) more
		 * bytes than desired, to avoid skew.  This is obviously not
		 * possible when the desired length wouldn't fit into the PKHA
		 */
		SET_LABEL(retry);
		/* Step B.1. Prime 'PKHA A SIZE' register with number of bytes
		 * to expect */
		LOAD(IMM((field_size + 8)), PKASZ, 0, 4, 0);

		/* Step B.2. Generate random 'value' for private key */
		NFIFOADD(PAD, PKA, (field_size + 8), WITH(PAD_RANDOM | EXT));

		/* Step B.3. Reduce the private key down to within the
		 * modulus */
		/* B <= private key */
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_AMODN);
		/* 'Good practice' says to make sure this value is not 0, 1,
		 * or p-1 ... */
		/* Retry if key is 0 */
		p1_retry = JUMP(IMM(retry), HALT, ALL_TRUE, PK_0);

		/* Step B.4. Store the private key for later use. */
		FIFOSTORE(PKB, 0, private_key, field_size, 0);

		FIFOLOAD(PKA, IMM(0x01000001), 4, 0);
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_ADD);
		/* Retry if key is -1 */
		p2_retry = JUMP(IMM(retry), HALT, ALL_TRUE, PK_0);

		FIFOLOAD(PKA, IMM(0x02000002), 4, 0);
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_SUB_2);
		/* Retry if key is 1 */
		p3_retry = JUMP(IMM(retry), HALT, ALL_TRUE, PK_0);

		/* Step C.  Generate the public key */
		/* A <= Generator (2) */
		FIFOLOAD(PKA, IMM(0x02000002), 4, 0);
		/* E <= Private key */
		PKHA_OPERATION(OP_ALG_PKMODE_COPY_NSZ_B_E);
		/* B <= Public Key */
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_EXP_TEQ);

		FIFOSTORE(PKB, 0, public_key, field_size, 0);
	}
	PATCH_JUMP(p1_retry, retry);
	PATCH_JUMP(p2_retry, retry);
	PATCH_JUMP(p3_retry, retry);

	size = PROGRAM_FINALIZE();

	return size;
}

int prg_buff[1000];

static void print_prog(uint32_t *buff, int size)
{
	int i;
	for (i = 0; i < size; i++)
		printf("%08x\n", buff[i]);
}

int main(int argc, char **argv)
{
	int size;
	printf("DLC KEYGEN program\n");
	size = dlc_keygen((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);
	return 0;
}
