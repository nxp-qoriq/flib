#include <stdio.h>
#include "flib/rta.h"

enum rta_sec_era rta_sec_era;

uint32_t shr_addr = 0x00000B80ul;

LABEL(a);
REFERENCE(ref_jumpa);
LABEL(c);
REFERENCE(ref_jumpc);
LABEL(h);
REFERENCE(ref_jumph);
LABEL(k);
REFERENCE(ref_jumpk);
LABEL(l);
REFERENCE(ref_jumpl);
LABEL(q);
REFERENCE(refq_hdr);
LABEL(s);
REFERENCE(ref1_moves);
REFERENCE(ref2_moves);

int build_shr_desc_ppp_encap(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;
	uint32_t c1_ctx_addr = 0x0972ecc0ul;
	uint32_t c2_ctx_addr = 0x094E03C0ul;

	LABEL(start);
	REFERENCE(pjump1);
	LABEL(b);
	REFERENCE(pjumpb);
	LABEL(d);
	REFERENCE(pjumpd);
	LABEL(do_nfifo);
	REFERENCE(pmove1);
	LABEL(e);
	REFERENCE(pjumpe);
	LABEL(aprm);
	REFERENCE(pjump2);
	LABEL(p);
	REFERENCE(pjumpp);
	LABEL(r);
	REFERENCE(pjumpr);
	LABEL(endshare);
	REFERENCE(pmoves);
	REFERENCE(pjumps);

	PROGRAM_CNTXT_INIT(buff, buffpos);
	SHR_HDR(SHR_NEVER, 46, 0);
	{
		{
			SET_LABEL(start);
			SEQSTORE(CONTEXT2, 20, 1, 0);
			MATHB(ZERO, ADD, IMM(64), VSEQINSZ, 4, 0);

			SET_LABEL(b);
			MATHB(SEQINSZ, SUB, IMM(64), MATH0, 8, WITH(IFB));
			pjumpd = JUMP(IMM(d), LOCAL_JUMP, ALL_FALSE,
					WITH(MATH_N));
			MATHB(ZERO, SUB, MATH0, MATH1, 8, WITH(NFU));
			NFIFOADD(PAD, MSG1, 0, WITH(PAD_ZERO | LAST1));
			pmove1 =
			    MOVE(MATH1, 0, DESCBUF, do_nfifo, IMM(8),
				 WITH(WAITCOMP));
			MATHB(VSEQINSZ, ADD, MATH0, VSEQINSZ, 4, WITH(NFU));

			SET_LABEL(d);
			SEQFIFOLOAD(MSG1, 0, WITH(VLF));
			pmoves = MOVE(CONTEXT2, 0, DESCBUF, s, IMM(72), 0);
			pjumpe = JUMP(IMM(e), LOCAL_JUMP, ALL_FALSE,
					WITH(MATH_N));

			SET_LABEL(do_nfifo);
			WORD(0);
			WORD(0);

			SET_LABEL(e);
			MATHB(ZERO, ADD, IMM(8), VSEQOUTSZ, 4, 0);
			LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0, 0);
			MOVE(IFIFOAB2, 0, OFIFO, 0, IMM(64), 0);
			pjump2 =
			    JUMP(IMM(aprm), LOCAL_JUMP, ALL_TRUE, WITH(NOP));

			SET_LABEL(a);
			LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0, 0);

			SET_LABEL(aprm);
			MATHB(ZERO, ADD, OFIFO, MATH0, 8, 0);
			pjumps = SHR_HDR(SHR_NEVER, b, 0);

			SET_LABEL(h);
			MATHB(VSEQINSZ, ADD, MATH3, VSEQOUTSZ, 4, 0);
			SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));
			MATHB(MATH3, AND, IMM(7), MATH3, 4, 0);
			pjumpp = JUMP(IMM(p), LOCAL_JUMP, ALL_TRUE,
					WITH(MATH_Z));
			MOVE(IFIFOABD, 0, OFIFO, 0, IMM(8), 0);

			SET_LABEL(r);
			LOAD(IMM(0x7e), IFIFO, 0, 1, 0);
			MATHB(MATH3, ADD, ONE, MATH3, 4, 0);
			MATHB(MATH3, AND, IMM(7), MATH3, 4, 0);
			pjumpr = JUMP(IMM(r), LOCAL_JUMP, ALL_FALSE,
					WITH(MATH_Z));

			SET_LABEL(p);
			MATHB(SEQINSZ, SUB, ONE, NONE, 4, 0);
			pjumpb = JUMP(IMM(b), LOCAL_JUMP, ALL_FALSE,
					WITH(MATH_N));
			JUMP(IMM(0), HALT_STATUS, ALL_TRUE, 0);
			JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, 0);

			SET_LABEL(q);
			MOVE(CONTEXT1, 56, MATH2, 0, IMM(8), WITH(WAITCOMP));

			SET_LABEL(l);
			MATHB(MATH1, AND, MATH2, NONE, 1, 0);
			ref_jumpk = JUMP(IMM(k), LOCAL_JUMP, ALL_TRUE,
					WITH(MATH_Z));

			MOVE(CONTEXT2, 4, IFIFOAB1, 0, IMM(1), 0);
			MATHB(MATH0, XOR, ONE, MATH0, 8, 0);
		}
		SET_LABEL(s);
		LOAD(PTR(c1_ctx_addr), CONTEXT1, 0, 64, 0);
		LOAD(PTR(c2_ctx_addr), CONTEXT2, 0, 72, 0);
		pjump1 = SHR_HDR(SHR_NEVER, start, 0);
		WORD(0);
		WORD(0);
		SET_LABEL(endshare);
	}

	PATCH_HDR(program, pjump1, start);
	PATCH_JUMP(program, pjumpb, b);
	PATCH_JUMP(program, pjumpd, d);
	PATCH_MOVE(program, pmove1, do_nfifo);
	PATCH_JUMP(program, pjumpe, e);
	PATCH_JUMP(program, pjump2, aprm);
	PATCH_JUMP(program, pjumpp, p);
	PATCH_JUMP(program, pjumpr, r);
	PATCH_HDR(program, pjumps, s);
	PATCH_MOVE(program, pmoves, s);

	size = PROGRAM_FINALIZE();
	return size;
}

/*
 * We need 0x80 Math Register 2. Loading a constant would take 2 words but we
 * didn't have another word to spare. So how to get it? Well, the command at
 * the end of the class 1 context is an absolute jump using the Shared
 * Descriptor Header. When this command is used as an absolute jump, the only
 * parts of the command used are the command field (bits 31:27) and the Start
 * Word (bits 21:16). Therefore, we can "hide" the value 0x80 in the right
 * byte of the command. So, we move the 16 bytes ending with that byte to the
 * Math 2 register, which puts that byte in the right-most byte of Math 2.
 * Then, when we do operations on Math 2, we set the length to 1 byte, wiping
 * all the other bits in the register. And, there you have it - 0x80 using only
 * one word instead of two!
 */
int build_extra_cmds(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;

	REFERENCE(pjumpk);

	PROGRAM_CNTXT_INIT(buff, buffpos);
	{
		/* imm data for MATH cmd at end of PDB in shared descriptor */
		DWORD(0x2000000000000000);
		MATHB(MATH3, ADD, ONE, MATH3, 4, 0);
		MATHB(MATH3, AND, IMM(7), NONE, 4, 0);
		pjumpk = JUMP(IMM(k), LOCAL_JUMP, ALL_FALSE, WITH(MATH_Z));
		NFIFOADD(IFIFO, MSG, 8, 0);
		MOVE(IFIFOABD, 0, OFIFO, 0, IMM(8), 0);

		SET_LABEL(k);
		MOVE(MATH0, 0, IFIFOAB1, 0, IMM(1), 0);
		MATHB(MATH0, LSHIFT, IMM(8), MATH0, 8, WITH(IFB));
		MATHB(MATH2, RSHIFT, ONE, MATH2, 1, 0);
		ref_jumpl = JUMP(IMM(l), LOCAL_JUMP, ALL_FALSE, WITH(MATH_Z));
		ref1_moves =
		    MOVE(CONTEXT2, 0, DESCBUF, s, IMM(64), WITH(WAITCOMP));
		ref_jumpc = SHR_HDR(SHR_NEVER, c, 0);
	}
	PATCH_JUMP(program, pjumpk, k);

	size = PROGRAM_FINALIZE();
	return size;
}

int build_more_cmds(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;

	LABEL(g);
	REFERENCE(pjumpg);

	PROGRAM_CNTXT_INIT(buff, buffpos);
	{
		MATHB(MATH0, XOR, IMM(0x7d7d7d7d7d7d7d7d), MATH1, 8, 0);
		MATHU(MATH1, ZBYTE, MATH1, 8, 0);

		MATHB(MATH0, XOR, IMM(0x7e7e7e7e7e7e7e7e), MATH2, 8, 0);
		MATHU(MATH2, ZBYTE, MATH2, 8, 0);
		MATHB(MATH1, OR, MATH2, MATH1, 8, 0);

		pjumpg = JUMP(IMM(g), LOCAL_JUMP, ALL_TRUE, WITH(MATH_Z));
		ref2_moves =
		    MOVE(CONTEXT1, 0, DESCBUF, s, IMM(64), WITH(WAITCOMP));
		refq_hdr = SHR_HDR(SHR_NEVER, q, 0);

		SET_LABEL(g);
		MOVE(MATH0, 0, IFIFOAB1, 0, IMM(8), 0);

		SET_LABEL(c);
		LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0, 0);
		MOVE(IFIFOABD, 0, OFIFO, 0, IMM(8), 0);
		MATHB(VSEQOUTSZ, SUB, ONE, VSEQOUTSZ, 4, 0);
		ref_jumpa = JUMP(IMM(a), LOCAL_JUMP, ALL_FALSE, WITH(MATH_Z));
		ref_jumph = SHR_HDR(SHR_NEVER, h, 0);
	}
	PATCH_JUMP(program, pjumpg, g);

	size = PROGRAM_FINALIZE();
	return size;
}

int build_jbdesc_ppp_encap(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;
	uint32_t in_addr = 0x09DDDB80ul;
	uint32_t in_len = 1450;
	uint32_t out_addr = 0x00000000ul;
	uint32_t out_len = 2902;

	PROGRAM_CNTXT_INIT(buff, buffpos);
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

	rta_set_sec_era(RTA_SEC_ERA_2);

	memset(shr, 0x00, sizeof(shr));
	shr_size = build_shr_desc_ppp_encap(&shr_desc_prgm, shr, 0);

	memset(c1_ctx, 0x00, sizeof(c1_ctx));
	c1_ctx_size = build_extra_cmds(&extra_cmds_prgm, c1_ctx, 46);

	memset(c2_ctx, 0x00, sizeof(c2_ctx));
	c2_ctx_size = build_more_cmds(&more_cmds_prgm, c2_ctx, 46);

	memset(job, 0x00, sizeof(job));
	job_size = build_jbdesc_ppp_encap(&job_desc_prgm, job, 0);

	PATCH_MOVE(&extra_cmds_prgm, ref1_moves, s);
	PATCH_MOVE(&more_cmds_prgm, ref2_moves, s);
	PATCH_JUMP(&more_cmds_prgm, ref_jumpa, a);
	PATCH_HDR(&extra_cmds_prgm, ref_jumpc, c);
	PATCH_HDR(&more_cmds_prgm, ref_jumph, h);
	PATCH_JUMP(&shr_desc_prgm, ref_jumpk, k);
	PATCH_JUMP(&extra_cmds_prgm, ref_jumpl, l);
	PATCH_HDR(&more_cmds_prgm, refq_hdr, q);

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
