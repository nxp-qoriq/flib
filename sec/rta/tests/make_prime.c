#include <stdio.h>
#include "flib/rta.h"

enum rta_sec_era rta_sec_era;

int make_prime_test(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;
	int prime_size = 5;
	uint64_t prime = (uint64_t) 0xeb9f70300ull;

	LABEL(gen);
	REFERENCE(pjump1);

	PROGRAM_CNTXT_INIT(buff, 0);
	JOB_HDR(SHR_NEVER, 0, 0);
	{
		/* Acquire the PKHA */
		FIFOLOAD(PKA, IMM(0), 0, 0);
		/* Write the PKHA N size, getting it ready to load */
		LOAD(IMM(prime_size), PKNSZ, 0, 4, 0);
		/* Write the PKHA A size, getting it ready to load */
		LOAD(IMM((prime_size - 1)), PKASZ, 0, 4, 0);
		SET_LABEL(gen);
		/* Turn off auto info-fifo entries */
		LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0, 0);

		/*
		   Send six bytes of random padding to DECO AB and then to
		   MATH0. The upper four will be ignored, and the next two
		   will become the first and last bytes of our candidate.
		 */
		NFIFOADD(PAD, MSG, 6, WITH(PAD_RANDOM | LAST1));
		MOVE(ABD, 0, MATH0, 0, IMM(6), WITH(WAITCOMP));
		/* Turn on MSb of first byte and LSb of last byte */
		MATHB(MATH0, OR, IMM(0x80010000), MATH0, 4, 0);
		/* Send the first and last bytes to the input data fifo */
		MOVE(MATH0, 4, IFIFOAB1, 0, IMM(2), 0);

		/* Send MSB from Input FIFO to PKHA N */
		NFIFOADD(IFIFO, PKN, 1, 0);
		/* Send middle bytes to PKHA N */
		NFIFOADD(PAD, PKN, (prime_size - 2), WITH(PAD_RANDOM | EXT));
		/* Send LSB byte from Input FIFO to PKHA N */
		NFIFOADD(IFIFO, PKN, 1, WITH(FLUSH1));
		/* Turn on auto info-fifo entries */
		LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0, 0);
		/*
		 * Now set up other inputs size the PRIME_TEST test
		 * Send random seed to PKHA A
		 */
		NFIFOADD(PAD, PKA, (prime_size - 1),
			 WITH(PAD_RANDOM | FLUSH1 | EXT));
		/* Miller-Rabin iteration count (either-endian) */
		FIFOLOAD(PKB, IMM(0x07000007), 4, 0);
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_PRIMALITY);
		pjump1 = JUMP(IMM(gen), LOCAL_JUMP, ALL_FALSE, WITH(PK_PRIME));
		FIFOSTORE(PKN, 0, prime, prime_size, 0);
	}
	PATCH_JUMP(program, pjump1, gen);

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

	printf("Make prime example program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = make_prime_test((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
