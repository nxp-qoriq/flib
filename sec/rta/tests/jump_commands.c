#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;

LABEL(aaa);
REFERENCE(ref1_jump_aaa);
REFERENCE(ref2_jump_aaa);
REFERENCE(ref1_job_aaa);
REFERENCE(ref1_shr_aaa);
REFERENCE(ref2_job_aaa);
LABEL(bbb);
REFERENCE(ref1_jump_bbb);
REFERENCE(ref2_jump_bbb);
REFERENCE(ref1_job_bbb);
REFERENCE(ref1_shr_bbb);
LABEL(fff);
REFERENCE(ref1_jump_fff);
REFERENCE(ref1_job_fff);
REFERENCE(ref1_shr_fff);
LABEL(ggg);
REFERENCE(ref1_jump_ggg);
REFERENCE(ref1_job_ggg);
REFERENCE(ref1_shr_ggg);
LABEL(zzz);
REFERENCE(ref2_jump_zzz);
REFERENCE(ref1_jump_zzz);
REFERENCE(ref3_jump_zzz);
REFERENCE(ref1_job_zzz);
REFERENCE(ref1_shr_zzz);
REFERENCE(ref2_job_zzz);
REFERENCE(ref2_shr_zzz);
LABEL(yyy);
REFERENCE(ref1_jump_yyy);
REFERENCE(ref1_job_yyy);
REFERENCE(ref1_shr_yyy);

uint32_t sharehdr = 0x200000;

int jump_cmd_desc1(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;

	LABEL(share1end);
	REFERENCE(pjump1);
	REFERENCE(pjump2);
	REFERENCE(phdr1);
	REFERENCE(phdr2);
	REFERENCE(phdr3);
	REFERENCE(phdr4);

	PROGRAM_CNTXT_INIT(buff, 0);
	SHR_HDR(SHR_NEVER, 0, 0);
	{
		SET_LABEL(aaa);
		SET_LABEL(bbb);
		bbb = 7;

		pjump1 = MOVE(CONTEXT2, 0, CONTEXT1, 0, IMM(4), 0);
		pjump2 = JUMP(IMM(aaa), LOCAL_JUMP, ALL_TRUE, 0);
		ref1_jump_fff = JUMP(IMM(bbb), LOCAL_JUMP, ALL_TRUE, 0);
		ref1_jump_ggg = JUMP(IMM(fff), LOCAL_JUMP, ALL_TRUE, 0);
		ref1_jump_zzz = JUMP(IMM(ggg), LOCAL_JUMP, ALL_TRUE, 0);
		JUMP(IMM(zzz), LOCAL_JUMP, ALL_TRUE, 0);

		phdr1 = MOVE(CONTEXT2, 0, CONTEXT1, 0, IMM(24), 0);

		phdr2 = JOB_HDR(SHR_NEVER, aaa, 0, 0);
		ref1_job_fff = JOB_HDR(SHR_NEVER, bbb, 0, 0);
		ref1_job_ggg = JOB_HDR(SHR_NEVER, fff, 0, 0);
		ref1_job_zzz = JOB_HDR(SHR_NEVER, ggg, 0, 0);
		phdr3 = JOB_HDR(SHR_NEVER, zzz, 0, 0);

		phdr4 = SHR_HDR(SHR_NEVER, aaa, 0);
		ref1_shr_fff = SHR_HDR(SHR_NEVER, bbb, 0);
		ref1_shr_ggg = SHR_HDR(SHR_NEVER, fff, 0);
		ref1_shr_zzz = SHR_HDR(SHR_NEVER, ggg, 0);
		SHR_HDR(SHR_NEVER, zzz, 0);

		SET_LABEL(share1end);
	}

	PATCH_JUMP(program, pjump1, aaa);
	PATCH_JUMP(program, pjump2, bbb);
	PATCH_HDR(program, phdr1, aaa);
	PATCH_HDR(program, phdr2, bbb);
	PATCH_HDR(program, phdr3, aaa);
	PATCH_HDR(program, phdr4, bbb);

	size = PROGRAM_FINALIZE();
	return size;
}

int jump_cmd_desc2(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;

	REFERENCE(pjump1);
	REFERENCE(pjump2);
	REFERENCE(phdr1);
	REFERENCE(phdr2);
	REFERENCE(phdr3);
	REFERENCE(phdr4);
	LABEL(imm_offset);

	PROGRAM_CNTXT_INIT(buff, buffpos);
	JOB_HDR(SHR_NEVER, buffpos, sharehdr, WITH(SHR));
	{
		SET_LABEL(fff);	/* first instruction in job header */
		ref1_jump_aaa = MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(8), 0);

		ref1_jump_bbb = JUMP(IMM(aaa), LOCAL_JUMP, ALL_TRUE, 0);
		pjump1 = JUMP(IMM(bbb), LOCAL_JUMP, ALL_TRUE, 0);
		pjump2 = JUMP(IMM(fff), LOCAL_JUMP, ALL_TRUE, 0);
		ref1_jump_yyy = JUMP(IMM(ggg), LOCAL_JUMP, ALL_TRUE, 0);
		ref2_jump_zzz = JUMP(IMM(yyy), LOCAL_JUMP, ALL_TRUE, 0);
		ref1_job_aaa = JUMP(IMM(zzz), LOCAL_JUMP, ALL_TRUE, 0);

		ref1_job_bbb = JOB_HDR(SHR_NEVER, aaa, 0, 0);
		phdr1 = JOB_HDR(SHR_NEVER, bbb, 0, 0);
		phdr2 = JOB_HDR(SHR_NEVER, fff, 0, 0);
		ref1_job_yyy = JOB_HDR(SHR_NEVER, ggg, 0, 0);
		ref2_job_zzz = JOB_HDR(SHR_NEVER, yyy, 0, 0);
		ref1_shr_aaa = JOB_HDR(SHR_NEVER, zzz, 0, 0);

		ref1_shr_bbb = SHR_HDR(SHR_NEVER, aaa, 0);
		phdr3 = SHR_HDR(SHR_NEVER, bbb, 0);
		phdr4 = SHR_HDR(SHR_NEVER, fff, 0);
		ref1_shr_yyy = SHR_HDR(SHR_NEVER, ggg, 0);
		ref2_shr_zzz = SHR_HDR(SHR_NEVER, yyy, 0);
		SHR_HDR(SHR_NEVER, zzz, 0);

		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, 0);

		SET_LABEL(ggg);
		MOVE(MATH0, 0, CONTEXT2, 0, IMM(4), 0);
		SET_LABEL(imm_offset);
		JUMP(IMM(10 - imm_offset), LOCAL_JUMP, ALL_TRUE, 0);
		JUMP(IMM(-2), LOCAL_JUMP, ALL_TRUE, 0);
	}
	PATCH_JUMP(program, pjump1, fff);
	PATCH_JUMP(program, pjump2, ggg);
	PATCH_HDR(program, phdr1, fff);
	PATCH_HDR(program, phdr2, ggg);
	PATCH_HDR(program, phdr3, fff);
	PATCH_HDR(program, phdr4, ggg);

	size = PROGRAM_FINALIZE();
	return size;
}

int jump_cmd_desc3(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;

	REFERENCE(pjump1);
	REFERENCE(phdr1);
	REFERENCE(phdr2);

	PROGRAM_CNTXT_INIT(buff, buffpos);
	{
		SET_LABEL(yyy);
		yyy = 63;	/* last word in descbuf [63] */

		ref2_jump_aaa = 0;
		ref2_jump_bbb = JUMP(IMM(aaa), LOCAL_JUMP, ALL_TRUE, 0);
		pjump1 = JUMP(IMM(bbb), LOCAL_JUMP, ALL_TRUE, 0);
		ref3_jump_zzz = JUMP(IMM(yyy), LOCAL_JUMP, ALL_TRUE, 0);
		ref2_job_aaa = JUMP(IMM(zzz), LOCAL_JUMP, ALL_TRUE, 0);

		phdr2 = JOB_HDR(SHR_NEVER, aaa, 0, 0);
		phdr1 = JOB_HDR(SHR_NEVER, yyy, 0, 0);
		JOB_HDR(SHR_NEVER, zzz, 0, 0);

		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, 0);

		SET_LABEL(zzz);
		MOVE(CONTEXT2, 0, CONTEXT1, 0, IMM(44), 0);
	}
	PATCH_JUMP(program, pjump1, yyy);
	PATCH_HDR(program, phdr1, zzz);
	PATCH_HDR(program, phdr2, yyy);

	size = PROGRAM_FINALIZE();
	return size;
}

int jump_cmd_desc4(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;

	PROGRAM_CNTXT_INIT(buff, buffpos);
	{
		JUMP(IMM(7 - buffpos), LOCAL_JUMP, ALL_TRUE, 0);
	}

	size = PROGRAM_FINALIZE();
	return size;
}

int jump_cmd_desc5(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;

	PROGRAM_CNTXT_INIT(buff, buffpos);
	{
		/* class done tests */
		JUMP(PTR(0x500), FAR_JUMP, ALL_TRUE,
				WITH(CLASS1 | PK_0 | MATH_N));
		JUMP(PTR(0x500), FAR_JUMP, ALL_FALSE,
				WITH(CLASS1 | MATH_Z | MATH_N));
		JUMP(PTR(0x500), FAR_JUMP, ALL_TRUE, WITH(BOTH | NOP));
		JUMP(PTR(0x500), FAR_JUMP, ALL_TRUE, WITH(CLASS2 | NOP));
		JUMP(PTR(0x500), FAR_JUMP, ALL_TRUE, WITH(CLASS2 | NOP));

		JUMP(PTR(0), HALT, ALL_TRUE, WITH(CALM | NOP | SHRD));
		JUMP(PTR(0x42), HALT_STATUS, ANY_FALSE, WITH(PK_0 | MATH_Z));
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CLASS1 | CLASS2));
	}
	size = PROGRAM_FINALIZE();
	return size;
}

int jump_cmd_desc6(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;

	PROGRAM_CNTXT_INIT(buff, buffpos);
	JOB_HDR(SHR_NEVER, 0, 0, 0);
	{
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(NIFP));
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(NIFP | NIP));
		JUMP(IMM(-1), LOCAL_JUMP, ALL_TRUE, 0);
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(NIFP));
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(NIFP | NIP));
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(NIFP | SHRD));
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(NIFP | SHRD));
		JUMP(IMM(1), LOCAL_JUMP, ANY_FALSE, WITH(NIFP | SHRD));
		JUMP(IMM(1), LOCAL_JUMP, ANY_FALSE, WITH(NIFP | SHRD));
		JUMP(IMM(1), LOCAL_JUMP, ANY_FALSE, WITH(NIFP | SHRD));
		JUMP(IMM(1), LOCAL_JUMP, ALL_FALSE,
		     WITH(CLASS1 | PK_PRIME | MATH_N));
		JUMP(IMM(1), LOCAL_JUMP, ALL_FALSE,
		     WITH(CLASS1 | PK_PRIME | MATH_N));
		JUMP(IMM(2), LOCAL_JUMP, ANY_FALSE, WITH(CLASS1 | JQP));
		JUMP(IMM(2), LOCAL_JUMP, ANY_TRUE, WITH(MATH_Z | MATH_N));
		JUMP(IMM(2), LOCAL_JUMP, ANY_FALSE, WITH(MATH_Z | MATH_N));
		JUMP(IMM(2), LOCAL_JUMP, ANY_FALSE, WITH(MATH_Z | MATH_N));
	}

	size = PROGRAM_FINALIZE();
	return size;
}

int main(int argc, char **argv)
{
	uint32_t desc1[64];
	uint32_t desc2[64];
	uint32_t desc3[64];
	uint32_t desc4[50];
	uint32_t desc5[50];
	uint32_t desc6[50];
	int buf1len, buf2len, buf3len;
	int buf4len, buf5len, buf6len;

	struct program desc1_prgm;
	struct program desc2_prgm;
	struct program desc3_prgm;
	struct program desc4_prgm;
	struct program desc5_prgm;
	struct program desc6_prgm;

	rta_set_sec_era(RTA_SEC_ERA_1);

	memset(desc1, 0x00, sizeof(desc1));
	buf1len = jump_cmd_desc1(&desc1_prgm, desc1, 0);

	memset(desc2, 0x00, sizeof(desc2));
	buf2len = jump_cmd_desc2(&desc2_prgm, desc2, buf1len);

	memset(desc3, 0x00, sizeof(desc3));
	buf3len = jump_cmd_desc3(&desc3_prgm, desc3, 55);

	memset(desc4, 0x00, sizeof(desc4));
	buf4len = jump_cmd_desc4(&desc4_prgm, desc4, 55);

	memset(desc5, 0x00, sizeof(desc5));
	buf5len = jump_cmd_desc5(&desc5_prgm, desc5, 0);

	memset(desc6, 0x00, sizeof(desc6));
	buf6len = jump_cmd_desc6(&desc6_prgm, desc6, 0);

	PATCH_JUMP(&desc2_prgm, ref1_jump_aaa, aaa);
	PATCH_JUMP(&desc3_prgm, ref2_jump_aaa, aaa);
	PATCH_JUMP(&desc2_prgm, ref1_jump_bbb, bbb);
	PATCH_JUMP(&desc3_prgm, ref2_jump_bbb, bbb);
	PATCH_JUMP(&desc1_prgm, ref1_jump_fff, fff);
	PATCH_JUMP(&desc1_prgm, ref1_jump_ggg, ggg);
	PATCH_JUMP(&desc2_prgm, ref2_jump_zzz, zzz);
	PATCH_JUMP(&desc1_prgm, ref1_jump_zzz, zzz);
	PATCH_JUMP(&desc3_prgm, ref3_jump_zzz, zzz);

	PATCH_HDR(&desc2_prgm, ref1_job_aaa, aaa);
	PATCH_HDR(&desc2_prgm, ref1_shr_aaa, aaa);
	PATCH_HDR(&desc3_prgm, ref2_job_aaa, aaa);
	PATCH_HDR(&desc2_prgm, ref1_job_bbb, bbb);
	PATCH_HDR(&desc2_prgm, ref1_shr_bbb, bbb);

	PATCH_HDR(&desc1_prgm, ref1_job_fff, fff);
	PATCH_HDR(&desc1_prgm, ref1_shr_fff, fff);

	PATCH_HDR(&desc1_prgm, ref1_job_ggg, ggg);
	PATCH_HDR(&desc1_prgm, ref1_shr_ggg, ggg);

	PATCH_HDR(&desc1_prgm, ref1_job_zzz, zzz);
	PATCH_HDR(&desc1_prgm, ref1_shr_zzz, zzz);
	PATCH_HDR(&desc2_prgm, ref2_job_zzz, zzz);
	PATCH_HDR(&desc2_prgm, ref2_shr_zzz, zzz);

	PATCH_HDR(&desc2_prgm, ref1_job_yyy, yyy);
	PATCH_HDR(&desc2_prgm, ref1_shr_yyy, yyy);
	PATCH_JUMP(&desc2_prgm, ref1_jump_yyy, yyy);

	pr_debug("JUMP commands program\n");
	pr_debug("Jump desc #1\n");
	pr_debug("size = %d\n", buf1len);
	print_prog((uint32_t *) desc1, buf1len);

	pr_debug("Jump desc #2\n");
	pr_debug("size = %d\n", buf2len);
	print_prog((uint32_t *) desc2, buf2len);

	pr_debug("Jump desc #3\n");
	pr_debug("size = %d\n", buf3len);
	print_prog((uint32_t *) desc3, buf3len);

	pr_debug("Jump desc #4\n");
	pr_debug("size = %d\n", buf4len);
	print_prog((uint32_t *) desc4, buf4len);

	pr_debug("Jump desc #5\n");
	pr_debug("size = %d\n", buf5len);
	print_prog((uint32_t *) desc5, buf5len);

	pr_debug("Jump desc #6\n");
	pr_debug("size = %d\n", buf6len);
	print_prog((uint32_t *) desc6, buf6len);

	return 0;
}
