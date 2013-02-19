#include <stdio.h>
#include "flib/rta.h"

enum rta_sec_era rta_sec_era;

int f2m_calc_c_test(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;

	LABEL(bit_search_top);
	REFERENCE(pjump1);
	LABEL(shr_end);

	PROGRAM_CNTXT_INIT(buff, 0);
	SHR_HDR(SHR_NEVER, 0, 0);
	{
		/*
		 * Compute c = b^(2^(m-2)) for 'b' and input poly q (modulus)
		 * The 'input frame' consists of q followed b.  The output
		 * frame will contain 'c'.  All vectors will be / must be
		 * length of q in bytes.
		 * 1) Compute m and load the Polynomial
		 *    Get byte count / 2 (i.e., q size)
		 */
		MATHB(SEQINSZ, RSHIFT, ONE, VSEQINSZ, 4, 0);
		MATHB(VSEQINSZ, SUB, ONE, MATH2, 4, 0);

		/* Test for 'too big' (to fit into 4096-bit PKHA memory...) */
		MATHB(MATH2, RSHIFT, IMM(9), MATH1, 4, 0);
		JUMP(IMM(0x12), HALT_STATUS, ALL_FALSE, WITH(MATH_Z));

		/* Direct first byte of polynomial to MATH1 via Class 1 CTX */
		SEQLOAD(CONTEXT1, 0, 1, 0);
		/* wait for load to complete */
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(NIP));
		MOVE(CONTEXT1, 0, MATH1, 0, IMM(1), 0);

		/*
		 * Load full polynomial into PKHA N RAM (after recomputing full
		 * byte count)
		 */
		SEQINPTR(0, 1, WITH(RTO)); /* Back up the to the beginning */
		MATHB(MATH2, ADD, ONE, VSEQINSZ, 4, 0);
		SEQFIFOLOAD(PKN, 0, WITH(VLF));

		/* Start with m = 8 * (poly_size - 1) - 1 */
		MATHB(MATH2, LSHIFT, IMM(3), MATH0, 4, 0);
		MATHB(MATH2, SUB, ONE, MATH2, 4, 0);
		/* Move first byte from MSB to LSB */
		MATHB(MATH1, RSHIFT, IMM(56), MATH1, 8, WITH(IFB));
		JUMP(IMM(0x13), HALT_STATUS, ANY_TRUE, WITH(MATH_Z));

		SET_LABEL(bit_search_top);
		MATHB(MATH0, ADD, ONE, MATH0, 2, 0);	/* m = m + 1 */
		MATHB(MATH1, RSHIFT, ONE, MATH1, 2, 0);	/* byte >> 1 */
		pjump1 = JUMP(IMM(bit_search_top), LOCAL_JUMP, ALL_FALSE,
			      WITH(MATH_Z));

		MATHB(MATH0, SUB, IMM(3), MATH0, 2, 0);	/* m-2 */
		LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0, 0);
		LOAD(IMM(2), PKESZ, 0, 4, 0);
		MATHB(MATH0, LSHIFT, IMM(48), MATH0, 8, WITH(IFB));
		MOVE(MATH0, 0, IFIFOAB1, 0, IMM(2), 0);
		NFIFOADD(IFIFO, PKE, 2, WITH(FLUSH1));
		LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0, 0);

		/* pke is now 'm-2'; load 2 into pka */
		FIFOLOAD(PKA, IMM(0x02000002), 4, 0);
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_EXPO);
		PKHA_OPERATION(OP_ALG_PKMODE_COPY_SSZ_B_E);

		/* Now load curve parameter b */
		MATHB(SEQINSZ, ADD, ZERO, VSEQINSZ, 4, 0);
		SEQFIFOLOAD(PKA, 0, WITH(VLF));
		PKHA_OPERATION(OP_ALG_PKMODE_F2M_EXP);

		MATHB(SEQOUTSZ, ADD, ZERO, VSEQOUTSZ, 4, 0);
		SEQFIFOSTORE(PKB, 0, 0, WITH(VLF));

		SET_LABEL(shr_end);
	}
	PATCH_JUMP(program, pjump1, bit_search_top);
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

	printf("F2M_calc_c example program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = f2m_calc_c_test((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
