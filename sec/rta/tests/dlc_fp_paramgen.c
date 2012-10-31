#include <stdio.h>
#include "flib/rta.h"

/*
  Example job to create DLC/DSA Domain Parameters for q=p

  This runs on CAAM Era 2 or later (as long as there is a PKHA)

  Algorithm is modeled after IEEE Std 1363-2000, A.16.1, except:
    -- A substitute method for q generation A.15.7
    -- rmin/rmax are fixed for a  given r_size
    -- pmin/pmax are fixed for a  given q_size
    -- It does not handle the test of k for use with DHC or MQVC

  This will generate values of length 8*size bits for q and r
*/

uint rta_sec_era;

int r_size = 20;		/* 160 bits */
int q_size = 128;
uint64_t dom_r_addr = 0x51d121510ull;
uint64_t dom_q_addr = 0x7afd02ed0ull;
uint64_t dom_g_addr = 0x6393df32aull;

uint64_t generate_dlc_fp_params_addr = 0x7891c4c75ull;
uint64_t make_X_addr = 0x861776ed8ull;
uint64_t make_q_addr = 0x4b633925aull;
uint64_t make_g_addr = 0x7aa7742f1ull;

int generate_dlc_fp_params(struct program *prg, uint32_t *buff)
{
	struct program *program = prg;
	int size = 0;

	LABEL(new_r);
	REFERENCE(ref_new_r);

	PROGRAM_CNTXT_INIT(buff, 0);
	PROGRAM_SET_36BIT_ADDR();

	JOB_HDR(SHR_NEVER, 0, 0);
	{
		SEQFIFOLOAD(PKA, 0, 0);	/* acquire / wake up the PKHA */

		/* 1. Generate a random prime r */
		SET_LABEL(new_r);
		LOAD(IMM(r_size), PKNSZ, 0, 4, 0);
		LOAD(IMM((r_size - 1)), PKASZ, 0, 4, 0);
		LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0, 0);
		/* Get random MSB and LSB do abd */
		NFIFOADD(PAD, MSG, 2, WITH(PAD_RANDOM | LAST1));
		/* Now into math0 */
		MOVE(IFIFOABD, 0, MATH0, 4, IMM(2), WITH(WAITCOMP));
		/* Turn on MSb and LSb */
		MATHB(MATH0, OR, IMM(0x80010000), MATH0, 4, 0);
		/* Send them to the ififo */
		MOVE(MATH0, 4, IFIFOAB1, 0, IMM(2), 0);
		/* Send MSB to pkn */
		NFIFOADD(IFIFO, PKN, 1, 0);
		/* and middle random bytes */
		NFIFOADD(PAD, PKN, (r_size - 2), WITH(PAD_RANDOM | EXT));
		/* Send LSB to pkn (with flush1, so loading finishes) */
		NFIFOADD(IFIFO, PKN, 1, WITH(FLUSH1));
		LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0, 0);
		/* Random seed for the Miller-Rabin primality test */
		NFIFOADD(PAD, PKA, (r_size - 1),
			 WITH(FLUSH1 | PAD_RANDOM | EXT));
		/* Iteration count */
		FIFOLOAD(PKB, IMM(0x32000032), 4, 0);
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_PRIMALITY);

		/* If r did not test prime, go try another */
		ref_new_r = JUMP(IMM(new_r), LOCAL_JUMP, ALL_FALSE,
				WITH(PK_PRIME));
		FIFOSTORE(PKN, 0, dom_r_addr, r_size, 0);

		/* Set up maximum number of tries to compute a q from this r */
		MATHB(ZERO, ADD, IMM(4096), MATH3, 4, 0);
		JUMP(PTR(make_X_addr), FAR_JUMP, ALL_TRUE, 0);
	}
	PATCH_JUMP(ref_new_r, new_r);

	size = PROGRAM_FINALIZE();
	return size;
}

int dlc_fp_make_x(struct program *prg, uint32_t *buff)
{
	struct program *program = prg;
	int size;

	PROGRAM_CNTXT_INIT(buff, 0);
	PROGRAM_SET_36BIT_ADDR();

	JOB_HDR(SHR_NEVER, 0, 0);
	{
		/* 2. Generate a random prime q */
		LOAD(IMM(q_size), PKNSZ, 0, 4, 0);
		/*
		 * Start with a random value X with MSb and LSb set
		 * (This is the same 'generate random' recipe as used for r,
		 * but with a different size)
		 */
		LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0, 0);
		NFIFOADD(PAD, MSG, 2, WITH(PAD_RANDOM | LAST1));
		MOVE(IFIFOABD, 0, MATH0, 4, IMM(2), WITH(WAITCOMP));
		MATHB(MATH0, OR, IMM(0x80010000), MATH0, 4, 0);
		MOVE(MATH0, 4, IFIFOAB1, 0, IMM(2), 0);
		NFIFOADD(IFIFO, PKN, 1, 0);
		NFIFOADD(PAD, PKN, (q_size - 2), WITH(EXT | PAD_RANDOM));
		NFIFOADD(IFIFO, PKN, 1, WITH(FLUSH1));
		LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0, 0);

		/* Let X finish loading into pkn before storing it */
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(NIFP));
		FIFOSTORE(PKN, 0, dom_q_addr, q_size, 0);
		JUMP(PTR(make_q_addr), FAR_JUMP, ALL_TRUE, 0);
	}
	size = PROGRAM_FINALIZE();

	return size;
}

int dlc_fp_make_q(struct program *prg, uint32_t *buff)
{
	struct program *program = prg;
	int size;

	LABEL(store_q);
	REFERENCE(ref_store_q);

	PROGRAM_CNTXT_INIT(buff, 0);
	PROGRAM_SET_36BIT_ADDR();

	JOB_HDR(SHR_NEVER, 0, 0);
	{
		/* Calculate 2r */
		FIFOLOAD(PKA, PTR(dom_r_addr), r_size, 0);
		PKHA_OPERATION(OP_ALG_PKMODE_CLEARMEM_B);
		PKHA_OPERATION(OP_ALG_PKMODE_COPY_NSZ_A_B);
		/* 2r  ( % X) */
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_ADD);
		PKHA_OPERATION(OP_ALG_PKMODE_COPY_NSZ_B_N);

		/* Calculate c = X % 2r   ( % X) */
		FIFOLOAD(PKA, PTR(dom_q_addr), q_size, 0);	/* X */
		/* Set pknsz to byte count of 2*r -always one more than r */
		LOAD(IMM((r_size + 1)), PKNSZ, 0, 4, 0);
		/* c = X % 2r  ( % X) */
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_REDUCT);

		/*
		 * q = X - c + 1   ( % X)
		 * This guarantees that (q - 1) % 2r === 0
		 */
		/* X */
		FIFOLOAD(PKN, PTR(dom_q_addr), q_size, 0);
		PKHA_OPERATION(OP_ALG_PKMODE_CLEARMEM_A);
		PKHA_OPERATION(OP_ALG_PKMODE_COPY_SSZ_B_A);
		/* c */
		PKHA_OPERATION(OP_ALG_PKMODE_COPY_NSZ_N_B);
		/* X - c   ( % X) */
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_SUB_BA);
		FIFOLOAD(PKA, IMM(0x01000001), 4, 0);
		/* q = X - c + 1   ( % X) */
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_ADD);

		/* Set up prime test for q */
		PKHA_OPERATION(OP_ALG_PKMODE_COPY_NSZ_B_N);
		LOAD(IMM((q_size - 1)), PKASZ, 0, 4, 0);
		NFIFOADD(PAD, PKA, q_size - 1, WITH(PAD_RANDOM | EXT | FLUSH1));
		FIFOLOAD(PKB, IMM(0x14000014), 4, 0);
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_PRIMALITY);

		ref_store_q =
		    JUMP(IMM(store_q), LOCAL_JUMP, ALL_TRUE, WITH(PK_PRIME));

		/* Decrement try-X counter */
		MATHB(MATH3, SUB, ONE, MATH3, 4, 0);
		/* Go get another X */
		JUMP(PTR(make_X_addr), FAR_JUMP, ALL_FALSE, WITH(MATH_Z));
		/* Start over with a new r */
		JUMP(PTR(generate_dlc_fp_params_addr), FAR_JUMP, ALL_TRUE, 0);

		SET_LABEL(store_q);
		FIFOSTORE(PKN, 0, dom_q_addr, q_size, 0);
		JUMP(PTR(make_g_addr), FAR_JUMP, ALL_TRUE, 0);
	}
	PATCH_JUMP(ref_store_q, store_q);
	size = PROGRAM_FINALIZE();

	return size;
}

int dlc_fp_make_g(struct program *prg, uint32_t *buff)
{
	struct program *program = prg;
	int size;

	LABEL(h_loop);
	REFERENCE(ref_h_loop);
	LABEL(found_g);
	REFERENCE(ref_found_g);

	PROGRAM_CNTXT_INIT(buff, 0);
	PROGRAM_SET_36BIT_ADDR();

	JOB_HDR(SHR_NEVER, 0, 0);
	{
		/*
		 * 3. Let k = (q - 1) / r
		 * Compute 1/r % q  ( % q )
		 */
		FIFOLOAD(PKA, PTR(dom_r_addr), r_size, 0);
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_INV);	/* 1/r */
		FIFOSTORE(PKB, 0, dom_g_addr, q_size, 0); /* tmp */

		/* Compute q-1  ( % q ) */
		PKHA_OPERATION(OP_ALG_PKMODE_COPY_NSZ_N_A);
		FIFOLOAD(PKB, IMM(0x01000001), 4, 0);
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_SUB_AB);

		/* k = q-1 / r  ( % q ) */
		FIFOLOAD(PKA, PTR(dom_g_addr), q_size, 0);
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_MULT);

		PKHA_OPERATION(OP_ALG_PKMODE_COPY_NSZ_B_E);

		/*
		 * Algorithm to look for g
		 * h = 2
		 * do {
		 * G = h ^ k mod q
		 * if (G != 1) break
		 * h++
		 * } while true
		 */
		FIFOLOAD(PKB, IMM(0x02000002), 4, 0);	/* h = 2 */
		PKHA_OPERATION(OP_ALG_PKMODE_CLEARMEM_A);
		/* get q-sized 2 into pka */
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_ADD | OP_ALG_PKMODE_OUT_A);

		/* 5. Choose an integer h, not already chosen, satisfying
		 * 1 < h < q - 1 */
		SET_LABEL(h_loop);
		/* save h */
		FIFOSTORE(PKB, 0, dom_g_addr, q_size, 0);
		/* 6. Compute g = h^e % q */
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_EXPO);

		/*
		 * 7. If g = 1 then go to step 5
		 * Test g != 1 by calculating checking (g-1) != 0
		 */
		FIFOLOAD(PKB, IMM(0x01000001), 4, 0);
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_SUB_BA);
		ref_found_g =
		    JUMP(IMM(found_g), LOCAL_JUMP, ANY_FALSE, WITH(PK_0));

		/* H++ */
		FIFOLOAD(PKB, PTR(dom_g_addr), q_size, 0);
		FIFOLOAD(PKA, IMM(0x01000001), 4, 0);
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_ADD);
		ref_h_loop = JUMP(IMM(h_loop), LOCAL_JUMP, ALL_TRUE, 0);

		SET_LABEL(found_g);

		/* Restore g = g+1 */
		FIFOLOAD(PKA, IMM(0x01000001), 4, 0);
		PKHA_OPERATION(OP_ALG_PKMODE_MOD_ADD);

		/* 8. Output q, r, g */
		FIFOSTORE(PKB, 0, dom_g_addr, q_size, 0);
	}
	PATCH_JUMP(ref_h_loop, h_loop);
	PATCH_JUMP(ref_found_g, found_g);

	size = PROGRAM_FINALIZE();

	return size;
}

static void print_prog(uint32_t *buff, int size)
{
	int i;
	for (i = 0; i < size; i++)
		printf("%08X\n", buff[i]);
}

int main(int argc, char **argv)
{
	uint32_t gen_dlc_fp_desc[64];
	uint32_t make_x_desc[64];
	uint32_t make_q_desc[64];
	uint32_t make_g_desc[64];

	struct program gen_dlc_fp_prgm;
	struct program make_x_prgm;
	struct program make_q_prgm;
	struct program make_g_prgm;
	int size;

	rta_set_sec_era(1);

	printf("Generate DLC FP domain parameters\n");
	size = generate_dlc_fp_params(&gen_dlc_fp_prgm, gen_dlc_fp_desc);
	printf("size = %d\n", size);
	print_prog(gen_dlc_fp_desc, size);

	printf("Make X\n");
	size = dlc_fp_make_x(&make_x_prgm, make_x_desc);
	printf("size = %d\n", size);
	print_prog(make_x_desc, size);

	printf("Make q\n");
	size = dlc_fp_make_q(&make_q_prgm, make_q_desc);
	printf("size = %d\n", size);
	print_prog(make_q_desc, size);

	printf("Make g\n");
	size = dlc_fp_make_g(&make_g_prgm, make_g_desc);
	printf("size = %d\n", size);
	print_prog(make_g_desc, size);

	return 0;
}
