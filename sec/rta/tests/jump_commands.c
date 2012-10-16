#include <stdio.h>
#include "flib/rta.h"

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

	PROGRAM_CNTXT_INIT(buff, 0, 0);
	SHR_HDR(SHR_NEVER, 0, 0);
	{
		SET_LABEL(aaa);
		SET_LABEL(bbb);
		bbb = 7;

		MOVE(CONTEXT2, 0, CONTEXT1, 0, IMM(4), 0);
		pjump1 = JUMP(IMM(aaa), LOCAL_JUMP, ALL_TRUE, 0);
		pjump2 = JUMP(IMM(bbb), LOCAL_JUMP, ALL_TRUE, 0);
		ref1_jump_fff = JUMP(IMM(fff), LOCAL_JUMP, ALL_TRUE, 0);
		ref1_jump_ggg = JUMP(IMM(ggg), LOCAL_JUMP, ALL_TRUE, 0);
		ref1_jump_zzz = JUMP(IMM(zzz), LOCAL_JUMP, ALL_TRUE, 0);

		MOVE(CONTEXT2, 0, CONTEXT1, 0, IMM(24), 0);

		phdr1 = JOB_HDR(SHR_NEVER, aaa, 0);
		phdr2 = JOB_HDR(SHR_NEVER, bbb, 0);
		ref1_job_fff = JOB_HDR(SHR_NEVER, fff, 0);
		ref1_job_ggg = JOB_HDR(SHR_NEVER, ggg, 0);
		ref1_job_zzz = JOB_HDR(SHR_NEVER, zzz, 0);

		phdr3 = SHR_HDR(SHR_NEVER, aaa, 0);
		phdr4 = SHR_HDR(SHR_NEVER, bbb, 0);
		ref1_shr_fff = SHR_HDR(SHR_NEVER, fff, 0);
		ref1_shr_ggg = SHR_HDR(SHR_NEVER, ggg, 0);
		ref1_shr_zzz = SHR_HDR(SHR_NEVER, zzz, 0);

		SET_LABEL(share1end);
	}

	PATCH_JUMP(pjump1, aaa);
	PATCH_JUMP(pjump2, bbb);
	PATCH_HDR(phdr1, aaa);
	PATCH_HDR(phdr2, bbb);
	PATCH_HDR(phdr3, aaa);
	PATCH_HDR(phdr4, bbb);

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

	PROGRAM_CNTXT_INIT(buff, buffpos, 0);
	JOB_HDR(SHR_NEVER, sharehdr, WITH(SHR));
	{
		SET_LABEL(fff);	/* first instruction in job header */
		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(8), 0);

		ref1_jump_aaa = JUMP(IMM(aaa), LOCAL_JUMP, ALL_TRUE, 0);
		ref1_jump_bbb = JUMP(IMM(bbb), LOCAL_JUMP, ALL_TRUE, 0);
		pjump1 = JUMP(IMM(fff), LOCAL_JUMP, ALL_TRUE, 0);
		pjump2 = JUMP(IMM(ggg), LOCAL_JUMP, ALL_TRUE, 0);
		ref1_jump_yyy = JUMP(IMM(yyy), LOCAL_JUMP, ALL_TRUE, 0);
		ref2_jump_zzz = JUMP(IMM(zzz), LOCAL_JUMP, ALL_TRUE, 0);

		ref1_job_aaa = JOB_HDR(SHR_NEVER, aaa, 0);
		ref1_job_bbb = JOB_HDR(SHR_NEVER, bbb, 0);
		phdr1 = JOB_HDR(SHR_NEVER, fff, 0);
		phdr2 = JOB_HDR(SHR_NEVER, ggg, 0);
		ref1_job_yyy = JOB_HDR(SHR_NEVER, yyy, 0);
		ref2_job_zzz = JOB_HDR(SHR_NEVER, zzz, 0);

		ref1_shr_aaa = SHR_HDR(SHR_NEVER, aaa, 0);
		ref1_shr_bbb = SHR_HDR(SHR_NEVER, bbb, 0);
		phdr3 = SHR_HDR(SHR_NEVER, fff, 0);
		phdr4 = SHR_HDR(SHR_NEVER, ggg, 0);
		ref1_shr_yyy = SHR_HDR(SHR_NEVER, yyy, 0);
		ref2_shr_zzz = SHR_HDR(SHR_NEVER, zzz, 0);

		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, 0);

		SET_LABEL(ggg);
		MOVE(MATH0, 0, CONTEXT2, 0, IMM(4), 0);
		JUMP(IMM(40), LOCAL_JUMP, ALL_TRUE, 0);
		JUMP(IMM(-2), LOCAL_JUMP, ALL_TRUE, 0);
	}
	PATCH_JUMP(pjump1, fff);
	PATCH_JUMP(pjump2, ggg);
	PATCH_HDR(phdr1, fff);
	PATCH_HDR(phdr2, ggg);
	PATCH_HDR(phdr3, fff);
	PATCH_HDR(phdr4, ggg);

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

	PROGRAM_CNTXT_INIT(buff, buffpos, 0);
	{
		SET_LABEL(yyy);
		yyy = 63;	/* last word in descbuf [63] */

		ref2_jump_aaa = JUMP(IMM(aaa), LOCAL_JUMP, ALL_TRUE, 0);
		ref2_jump_bbb = JUMP(IMM(bbb), LOCAL_JUMP, ALL_TRUE, 0);
		pjump1 = JUMP(IMM(yyy), LOCAL_JUMP, ALL_TRUE, 0);
		ref3_jump_zzz = JUMP(IMM(zzz), LOCAL_JUMP, ALL_TRUE, 0);

		ref2_job_aaa = JOB_HDR(SHR_NEVER, aaa, 0);
		phdr2 = JOB_HDR(SHR_NEVER, yyy, 0);
		phdr1 = JOB_HDR(SHR_NEVER, zzz, 0);

		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, 0);

		SET_LABEL(zzz);
		MOVE(CONTEXT2, 0, CONTEXT1, 0, IMM(44), 0);
	}
	PATCH_JUMP(pjump1, yyy);
	PATCH_HDR(phdr1, zzz);
	PATCH_HDR(phdr2, yyy);

	size = PROGRAM_FINALIZE();
	return size;
}

int jump_cmd_desc4(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;

	PROGRAM_CNTXT_INIT(buff, buffpos, 0);
	{
		JUMP(IMM(28), LOCAL_JUMP, ALL_TRUE, 0);
	}

	size = PROGRAM_FINALIZE();
	return size;
}

int jump_cmd_desc5(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;

	PROGRAM_CNTXT_INIT(buff, buffpos, 0);
	{
		/* class done tests */
		JUMP(PTR(0x500), FAR_JUMP, ALL_TRUE,
				WITH(CLASS1 | PK_0 | MATH_N));
		JUMP(PTR(0x500), FAR_JUMP, ANY_FALSE,
				WITH(CLASS1 | MATH_Z | MATH_N));
		JUMP(PTR(0x500), FAR_JUMP, ALL_TRUE, WITH(BOTH | NOP));
		JUMP(PTR(0x500), FAR_JUMP, ALL_TRUE, WITH(CLASS2 | NOP));

		JUMP(PTR(0), HALT, ANY_TRUE, WITH(CALM | NOP | SHRD));
		JUMP(PTR(0x42), HALT_STATUS, ALL_FALSE, WITH(PK_0 | MATH_Z));
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CLASS1 | CLASS2));
	}
	size = PROGRAM_FINALIZE();
	return size;
}

int jump_cmd_desc6(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;

	PROGRAM_CNTXT_INIT(buff, buffpos, 0);
	JOB_HDR(SHR_NEVER, 0, 0);
	{
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(NIFP));
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(NIFP | NIP));
		JUMP(IMM(-1), LOCAL_JUMP, ALL_TRUE, 0);
	}

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

	memset(desc1, 0x00, sizeof(desc1));
	buf1len = jump_cmd_desc1(&desc1_prgm, desc1, 0);

	memset(desc2, 0x00, sizeof(desc2));
	buf2len = jump_cmd_desc2(&desc2_prgm, desc2, buf1len);

	memset(desc3, 0x00, sizeof(desc3));
	buf3len = jump_cmd_desc3(&desc3_prgm, desc3, 55);

	memset(desc4, 0x00, sizeof(desc4));
	buf4len = jump_cmd_desc4(&desc4_prgm, desc4, 44);

	memset(desc5, 0x00, sizeof(desc5));
	buf5len = jump_cmd_desc5(&desc5_prgm, desc5, 0);

	memset(desc6, 0x00, sizeof(desc6));
	buf6len = jump_cmd_desc6(&desc6_prgm, desc6, 0);

	PATCH_JUMP_NON_LOCAL(&desc2_prgm, ref1_jump_aaa, &desc1_prgm, aaa);
	PATCH_JUMP_NON_LOCAL(&desc3_prgm, ref2_jump_aaa, &desc1_prgm, aaa);
	PATCH_JUMP_NON_LOCAL(&desc2_prgm, ref1_jump_bbb, &desc1_prgm, bbb);
	PATCH_JUMP_NON_LOCAL(&desc3_prgm, ref2_jump_bbb, &desc1_prgm, bbb);
	PATCH_JUMP_NON_LOCAL(&desc1_prgm, ref1_jump_fff, &desc2_prgm, fff);
	PATCH_JUMP_NON_LOCAL(&desc1_prgm, ref1_jump_ggg, &desc2_prgm, ggg);
	PATCH_JUMP_NON_LOCAL(&desc2_prgm, ref2_jump_zzz, &desc3_prgm, zzz);
	PATCH_JUMP_NON_LOCAL(&desc1_prgm, ref1_jump_zzz, &desc3_prgm, zzz);
	PATCH_JUMP_NON_LOCAL(&desc3_prgm, ref3_jump_zzz, &desc3_prgm, zzz);

	PATCH_HDR_NON_LOCAL(&desc2_prgm, ref1_job_aaa, &desc1_prgm, aaa);
	PATCH_HDR_NON_LOCAL(&desc2_prgm, ref1_shr_aaa, &desc1_prgm, aaa);
	PATCH_HDR_NON_LOCAL(&desc3_prgm, ref2_job_aaa, &desc1_prgm, aaa);
	PATCH_HDR_NON_LOCAL(&desc2_prgm, ref1_job_bbb, &desc1_prgm, bbb);
	PATCH_HDR_NON_LOCAL(&desc2_prgm, ref1_shr_bbb, &desc1_prgm, bbb);

	PATCH_HDR_NON_LOCAL(&desc1_prgm, ref1_job_fff, &desc2_prgm, fff);
	PATCH_HDR_NON_LOCAL(&desc1_prgm, ref1_shr_fff, &desc2_prgm, fff);

	PATCH_HDR_NON_LOCAL(&desc1_prgm, ref1_job_ggg, &desc2_prgm, ggg);
	PATCH_HDR_NON_LOCAL(&desc1_prgm, ref1_shr_ggg, &desc2_prgm, ggg);

	PATCH_HDR_NON_LOCAL(&desc1_prgm, ref1_job_zzz, &desc3_prgm, zzz);
	PATCH_HDR_NON_LOCAL(&desc1_prgm, ref1_shr_zzz, &desc3_prgm, zzz);
	PATCH_HDR_NON_LOCAL(&desc2_prgm, ref2_job_zzz, &desc3_prgm, zzz);
	PATCH_HDR_NON_LOCAL(&desc2_prgm, ref2_shr_zzz, &desc3_prgm, zzz);

	PATCH_HDR_NON_LOCAL(&desc2_prgm, ref1_job_yyy, &desc3_prgm, yyy);
	PATCH_HDR_NON_LOCAL(&desc2_prgm, ref1_shr_yyy, &desc3_prgm, yyy);
	PATCH_JUMP_NON_LOCAL(&desc2_prgm, ref1_jump_yyy, &desc3_prgm, yyy);

	printf("JUMP commands program\n");
	printf("size = %d\n", buf1len);
	print_prog((uint32_t *) desc1, buf1len);

	printf("size = %d\n", buf2len);
	print_prog((uint32_t *) desc2, buf2len);

	printf("size = %d\n", buf3len);
	print_prog((uint32_t *) desc3, buf3len);

	printf("size = %d\n", buf4len);
	print_prog((uint32_t *) desc4, buf4len);

	printf("size = %d\n", buf5len);
	print_prog((uint32_t *) desc5, buf5len);

	printf("size = %d\n", buf6len);
	print_prog((uint32_t *) desc6, buf6len);

	return 0;
}
