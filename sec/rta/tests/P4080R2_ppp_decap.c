#include <stdio.h>
#include "rta.h"

uint32_t shr_addr = 0x089ae500ul;

LABEL(c);
REFERENCE(refc_shr);
LABEL(a);
REFERENCE(ref1_jumpa);
LABEL(h);
REFERENCE(refh_shr);
LABEL(s);
REFERENCE(ref1_moves);
REFERENCE(ref2_moves);
LABEL(t);
REFERENCE(ref_jumpt);
LABEL(k);
REFERENCE(ref_jumpk);
LABEL(q);
REFERENCE(refq_shr);
LABEL(l);
REFERENCE(ref_jumpl);

uint64_t desc_addr_1 = 0x00000ac0ull;

int build_shr_desc_ppp_decap(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;
	uint32_t c1_ctx_addr = 0x08887A00ul;
	uint32_t c2_ctx_addr = 0x09CB4780ul;

	LABEL(b);
	REFERENCE(pjumpb1)
	REFERENCE(pjumpb2)
	LABEL(d);
	REFERENCE(pjumpd);
	LABEL(do_nfifo);
	REFERENCE(pmove1);
	LABEL(e);
	REFERENCE(pjumpe);
	LABEL(p);
	REFERENCE(pjumpp);
	LABEL(r);
	REFERENCE(pjumpr);
	REFERENCE(pmoves);
	REFERENCE(pjumps);
	LABEL(endshare);

	/*
	 * NOTES ON HOW THIS DESCRIPTOR WORKS
	 * This descriptor is far too large to fit in the Descriptor Buffer.
	 * However, jumping from one descriptor to another isn't practical
	 * because we'd then have to jump back again. To get around this
	 * problem, I use the two context register. The Descriptor Buffer
	 * holds 64 words. Class 1 Context holds 16 words and Class 2 Context
	 * holds 18 words. So, things work like this:
	 *
	 * 1. The Job Descriptor holds the SEQ IN PTR and SEQ OUT PTR commands.
	 * After this runs, the Shared Descriptor starts.
	 *
	 * 2. The Shared Descriptor jumps to the end of the Shared Descriptor,
	 * where the Class 1 and 2 Context Registers are loaded. What we're
	 * putting in there is actually more descriptor. The Class 1 Context
	 * Register holds the compare routine, which detects if there's a 0x7D
	 * byte in the current dword. The Class 2 Context Register holds the
	 * Special Character Handler. (Acually, it holds part of this the rest
	 * of the special handler is in the Shared Descriptor.)
	 *
	 * 3. Start processing the data. Copy Class 1 Context over the end of
	 * the Shared Descriptor, also replacing the entire Job Descriptor.
	 * If there's a special character, copy the Class 2 Context over the
	 * last 18 words of the Descriptor Buffer, replacing the compare
	 * routine. When the special handling is done, copy Class 1 Context
	 * back again. So, we keep copying them back in as needed.
	 *
	 * 4. Care must be taken when the last byte of a dword, especially the
	 * last byte of a burst, is 0x7D. Then it must be remembered for the
	 * next byte that this byte needs to be XORed with 0x20 to flip it
	 * back!
	 *
	 * 5. Since unstuffing shortens the output frame, we can handle 128
	 * bytes at a time.
	 *
	 * NOTE: The first nine words of the special handling routine are in
	 *       the Shared Descriptor. Immediately following these nine words
	 *       is where the context register contents are copied. The Compare
	 *       Routine is self contained. However, the special handler in the
	 *       context is actually the continuation of the first nine words.
	 *
	 * NOTE: This was all set up so that it would work with 36-bit pointers.
	 *       That is, all pointers needing 2 words instead of 1. So, there
	 *       are "empty" words in various places to compensate for the fact
	 *       that these words would contain addresss if 36-bit addressing
	 *       was used instead of 32-bit addressing.
	 */
	PROGRAM_CNTXT_INIT(buff, buffpos, 0);
	SHR_HDR(SHR_NEVER, 46, 0);
	{
		{
			SET_LABEL(b);
			MATHB(ZERO, ADD, IMM(128), VSEQINSZ, 4, 0);
			MATHB(SEQINSZ, SUB, IMM(128), MATH0, 8, WITH(IFB));
			pjumpd = JUMP(IMM(d), LOCAL_JUMP, ALL_FALSE,
					WITH(MATH_N));
			MATHB(ZERO, SUB, MATH0, MATH1, 8, WITH(NFU));
			NFIFOADD(PAD, MSG1, 0, WITH(PAD_ZERO | LAST1));
			pmove1 = MOVE(MATH1, 0, DESCBUF, do_nfifo, IMM(8), 0);
			MATHB(VSEQINSZ, ADD, MATH0, VSEQINSZ, 4, WITH(NFU));

			SET_LABEL(d);
			SEQFIFOLOAD(MSG1, 0, WITH(VLF));
			pmoves = MOVE(CONTEXT1, 0, DESCBUF, s, IMM(64), 0);
			pjumpe = JUMP(IMM(e), LOCAL_JUMP, ALL_FALSE,
					WITH(MATH_N));

			SET_LABEL(do_nfifo);
			WORD(0);
			WORD(0);

			SET_LABEL(e);
			MATHB(ZERO, ADD, IMM(16), VSEQOUTSZ, 4, 0);
			LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_IFIFO, 0, 0);
			MOVE(IFIFOAB1, 0, OFIFO, 0, IMM(128), 0);

			SET_LABEL(a);
			LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_IFIFO, 0, 0);
			pjumps = JUMP(IMM(s), LOCAL_JUMP, ALL_TRUE, WITH(NOP));
			/* End of Main Loop - all 8 dwords have been handled */

			SET_LABEL(h);
			LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_IFIFO, 0, 0);
			MATHB(VSEQINSZ, ADD, MATH2, VSEQOUTSZ, 4, 0);
			SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));
			MATHB(ZERO, ADD, MATH3, NONE, 4, 0);
			pjumpp = JUMP(IMM(p), LOCAL_JUMP, ALL_TRUE,
					WITH(MATH_Z));
			MATHB(MATH3, SUB, IMM(8), MATH3, 8, WITH(IFB));
			MOVE(IFIFOABD, 0, OFIFO, 0, IMM(8), 0);

			SET_LABEL(r);
			LOAD(IMM(0x7E), IFIFO, 0, 1, 0);
			MATHB(MATH3, ADD, ONE, MATH3, 4, 0);
			pjumpr = JUMP(IMM(r), LOCAL_JUMP, ALL_FALSE,
					WITH(MATH_Z));

			SET_LABEL(p);
			MATHB(SEQINSZ, SUB, ONE, NONE, 4, 0);
			JUMP(IMM(0), HALT_STATUS, ALL_TRUE, WITH(MATH_N));
			pjumpb1 = JUMP(IMM(b), LOCAL_JUMP, ALL_TRUE, 0);

			SET_LABEL(q);
			MATHB(ZERO, ADD, IMM(0x80), MATH2, 4, 0);

			SET_LABEL(l);
			MATHB(MATH1, AND, MATH2, NONE, 4, 0);
			ref_jumpk =
			    JUMP(IMM(k), LOCAL_JUMP, ALL_FALSE, WITH(MATH_Z));
			MOVE(MATH0, 0, IFIFOAB1, 0, IMM(1), 0);
			MATHB(MATH3, ADD, ONE, MATH3, 8, 0);
			MATHB(MATH3, AND, IMM(7), MATH3, 8, WITH(IFB));
			ref_jumpt =
			    JUMP(IMM(t), LOCAL_JUMP, ALL_FALSE, WITH(MATH_Z));
		}

		SET_LABEL(s);
		LOAD(PTR(c1_ctx_addr), CONTEXT1, 0, 64, 0);
		LOAD(PTR(c2_ctx_addr), CONTEXT2, 0, 72, 0);
		pjumpb2 = SHR_HDR(SHR_NEVER, b, 0);
		WORD(0);
		WORD(0);
		SET_LABEL(endshare);
	}

	PATCH_JUMP(pjumpb1, b);
	PATCH_HDR(pjumpb2, b);
	PATCH_JUMP(pjumpd, d);
	PATCH_MOVE(pmove1, do_nfifo);
	PATCH_MOVE(pmoves, s);
	PATCH_JUMP(pjumps, s);
	PATCH_JUMP(pjumpe, e);
	PATCH_JUMP(pjumpp, p);
	PATCH_JUMP(pjumpr, r);

	size = PROGRAM_FINALIZE();
	return size;
}

int build_extra_cmds(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;

	LABEL(g);
	REFERENCE(pjumpg);

	PROGRAM_CNTXT_INIT(buff, buffpos, 0);
	{
		MATHB(ZERO, ADD, OFIFO, MATH0, 8, 0);
		MATHB(MATH0, XOR, IMM(0x7d7d7d7d7d7d7d7d), MATH1, 8, 0);
		/* in case last of prev dword was special */
		MATHB(MATH0, XOR, MATH2, MATH0, 8, 0);
		MATHB(ZERO, AND, MATH2, MATH2, 8, 0);
		MATHU(MATH1, ZBYTE, MATH1, 8, 0);
		pjumpg = JUMP(IMM(g), LOCAL_JUMP, ALL_TRUE, WITH(MATH_Z));
		ref1_moves =
		    MOVE(CONTEXT2, 0, DESCBUF, s, IMM(72), WITH(WAITCOMP));
		refq_shr = SHR_HDR(SHR_NEVER, q, 0);

		SET_LABEL(g);
		MOVE(MATH0, 0, IFIFOAB1, 0, IMM(8), 0);
		LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_IFIFO, 0, 0);
		MOVE(IFIFOABD, 0, OFIFO, 0, IMM(8), 0);

		SET_LABEL(c);
		MATHB(VSEQOUTSZ, SUB, ONE, VSEQOUTSZ, 4, 0);
		ref1_jumpa = JUMP(IMM(a), LOCAL_JUMP, ALL_FALSE, WITH(MATH_Z));
		refh_shr = SHR_HDR(SHR_NEVER, h, 0);
	}
	PATCH_JUMP(pjumpg, g);

	size = PROGRAM_FINALIZE();
	return size;
}

int build_more_cmds(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;

	REFERENCE(pjumpt);
	LABEL(i);
	REFERENCE(pjumpi);

	PROGRAM_CNTXT_INIT(buff, buffpos, 0);
	{
		NFIFOADD(IFIFO, MSG, 8, 0);
		MOVE(IFIFOABD, 0, OFIFO, 0, IMM(8), 0);

		SET_LABEL(t);
		MATHB(MATH0, LSHIFT, IMM(8), MATH0, 8, WITH(IFB));
		MATHB(MATH2, RSHIFT, ONE, MATH2, 4, 0);
		ref_jumpl = JUMP(IMM(l), LOCAL_JUMP, ALL_FALSE, WITH(MATH_Z));

		SET_LABEL(i);
		ref2_moves =
		    MOVE(CONTEXT1, 0, DESCBUF, s, IMM(64), WITH(WAITCOMP));
		refc_shr = SHR_HDR(SHR_NEVER, c, 0);

		SET_LABEL(k);
		MATHB(MATH0, XOR, IMM(0x0020000000000000), MATH0, 8, 0);
		MATHB(VSEQINSZ, SUB, ONE, VSEQINSZ, 4, 0);
		MATHB(MATH2, SUB, ONE, NONE, 8, 0);

		pjumpt = JUMP(IMM(t), LOCAL_JUMP, ALL_FALSE, WITH(MATH_Z));
		MATHB(MATH2, LSHIFT, IMM(61), MATH2, 8, WITH(IFB));
		pjumpi = JUMP(IMM(i), LOCAL_JUMP, ALL_TRUE, 0);
	}
	PATCH_JUMP(pjumpt, t);
	PATCH_JUMP(pjumpi, i);

	size = PROGRAM_FINALIZE();
	return size;
}

int build_jbdesc_ppp_decap(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;
	uint32_t in_addr = 0x00000040ul;
	uint32_t in_len = 1450;
	uint32_t out_addr = 0x0818fe00ul;
	uint32_t out_len = 2902;

	PROGRAM_CNTXT_INIT(buff, buffpos, 0);
	JOB_HDR(SHR_NEVER, shr_addr, WITH(REO | SHR));
	{
		JUMP(IMM(3), LOCAL_JUMP, ALL_TRUE, 0);
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, 0);
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, 0);
		SEQOUTPTR(out_addr, out_len, WITH(EXT));
		SEQINPTR(in_addr, in_len, WITH(EXT));
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
	uint32_t shr[64];
	uint32_t c1_ctx[20];
	uint32_t c2_ctx[20];
	uint32_t job[20];
	int shr_size, job_size, c1_ctx_size, c2_ctx_size;

	struct program shr_desc_prgm;
	struct program job_desc_prgm;
	struct program extra_cmds_prgm;
	struct program more_cmds_prgm;

	memset(shr, 0x00, sizeof(shr));
	shr_size = build_shr_desc_ppp_decap(&shr_desc_prgm, shr, 0);

	memset(c1_ctx, 0x00, sizeof(c1_ctx));
	c1_ctx_size =
	    build_extra_cmds(&extra_cmds_prgm, c1_ctx, /*shr_size */ 46);

	memset(c2_ctx, 0x00, sizeof(c2_ctx));
	c2_ctx_size =
	    build_more_cmds(&more_cmds_prgm, c2_ctx, /*c1_ctx_size */ 46);

	memset(job, 0x00, sizeof(job));
	job_size =
	    build_jbdesc_ppp_decap(&job_desc_prgm, job, /*c2_ctx_size */ 0);

	PATCH_HDR_NON_LOCAL(&more_cmds_prgm, refc_shr, &extra_cmds_prgm, c);
	PATCH_JUMP_NON_LOCAL(&extra_cmds_prgm, ref1_jumpa, &shr_desc_prgm, a);
	PATCH_HDR_NON_LOCAL(&extra_cmds_prgm, refh_shr, &shr_desc_prgm, h);
	PATCH_MOVE_NON_LOCAL(&extra_cmds_prgm, ref1_moves, &shr_desc_prgm, s);
	PATCH_MOVE_NON_LOCAL(&more_cmds_prgm, ref2_moves, &shr_desc_prgm, s);
	PATCH_JUMP_NON_LOCAL(&shr_desc_prgm, ref_jumpt, &more_cmds_prgm, t);
	PATCH_JUMP_NON_LOCAL(&shr_desc_prgm, ref_jumpk, &more_cmds_prgm, k);
	PATCH_HDR_NON_LOCAL(&extra_cmds_prgm, refq_shr, &shr_desc_prgm, q);
	PATCH_JUMP_NON_LOCAL(&more_cmds_prgm, ref_jumpl, &shr_desc_prgm, l);

	printf("PPP decap program shared desc\n");
	printf("size = %d\n", shr_size);
	print_prog((uint32_t *) shr, shr_size);

	printf("context1\n");
	printf("size = %d\n", c1_ctx_size);
	print_prog((uint32_t *) c1_ctx, c1_ctx_size);

	printf("context2\n");
	printf("size = %d\n", c2_ctx_size);
	print_prog((uint32_t *) c2_ctx, c2_ctx_size);

	printf("PPP decap program job desc\n");
	printf("size = %d\n", job_size);
	print_prog((uint32_t *) job, job_size);

	return 0;
}
