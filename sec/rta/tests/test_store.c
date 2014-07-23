/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;
uint32_t prg_buff[1000];
uint32_t jd1_buff[64];
uint32_t jd2_buff[64];
uint32_t sd_buff[64];
uintptr_t descwords = (uintptr_t) 0x12889980ull;
int word_size = sizeof(uint32_t);
REFERENCE(pstore2);
REFERENCE(pstore3);
REFERENCE(pstore4);
LABEL(mod_loc2);
LABEL(shr_loc);

unsigned test_store_cmds(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	uintptr_t ctx = (uintptr_t) 0x55330087ull;
	uintptr_t sgtable = (uintptr_t) 0x22223333ull;
	int ivlen = 16;
	uintptr_t foo = (uintptr_t)0x34000080ull;
	uint8_t abc[] = {0x61, 0x62, 0x63};
	uint here = 32;

	PROGRAM_CNTXT_INIT(buff, 0);
	/* Class-independent CCB registers */
	STORE(CLRW, 0, foo, 4, 0);
	STORE(CCTRL, 0, foo, 4, 0);
	STORE(ICTRL, 0, foo, 4, 0);
	STORE(CSTAT, 0, foo, 4, 0);

	/* Class 1 CCB registers */
	STORE(MODE1, 0, foo, 4, 0);
	STORE(KEY1SZ, 0, foo, 4, 0);
	STORE(DATA1SZ, 0, foo, 4, 0);
	STORE(ICV1SZ, 0, foo, 4, 0);
	/* Class 1 Additional Data Size */
	STORE(AAD1SZ, 0, foo, 4, 0);
	STORE(IV1SZ, 0, foo, 4, 0);
	STORE(PKASZ, 0, foo, 4, 0);
	STORE(PKBSZ, 0, foo, 4, 0);
	STORE(PKNSZ, 0, foo, 4, 0);
	STORE(PKESZ, 0, foo, 4, 0);

	STORE(CONTEXT1, 0, sgtable, ivlen, SGF);
	STORE(CONTEXT1, 5, foo, 7, 0);

	/* Class 2 CCB registers */
	STORE(MODE2, 0, foo, 4, 0);
	STORE(KEY2SZ, 0, foo, 4, 0);
	STORE(DATA2SZ, 0, foo, 4, 0);
	STORE(ICV2SZ, 0, foo, 4, 0);
	STORE(CONTEXT2, 0, ctx, 16, 0);
	STORE(CONTEXT2, 8, foo, 4, 0);

	/* DECO / SEC registers */
	STORE(DJQCTRL, 0, foo, 8, 0);
	/* DECO JQ Descriptor Address */
	STORE(DJQDA, 0, foo, 8, 0);
	STORE(DSTAT, 0, foo, 8, 0);
	STORE(DPID, 0, foo, 8, 0);

	SEQSTORE(MATH0, 0, 1, 0);
	SEQSTORE(MATH1, 0, 0, VLF);
	STORE(MATH2, 0, foo, 4, 0);
	STORE(MATH3, 0, foo, 4, 0);
	STORE(DESCBUF, 20 * word_size, descwords, 4 * word_size, 0);

	STORE(DESCBUF, here, descwords, 8 * word_size, 0);
	SEQSTORE(DESCBUF, here, 12 * word_size, 0);

	/* Immediate data */
	STORE(0x010203, 0, foo, 3, IMMED);
	STORE((uintptr_t) &abc, 0, foo, 3, IMMED | COPY);
	SEQSTORE(0x010203, 0, 3, IMMED);
	SEQSTORE(0x01020304050607, 0, 7, IMMED | VLF);
	SEQSTORE((uintptr_t) &abc, 0, 3, IMMED | COPY);

	return PROGRAM_FINALIZE();
}

unsigned test_store_jd1(uint32_t *buff, unsigned buffpos)
{
	struct program prg;
	struct program *program = &prg;
	REFERENCE(pstore);
	LABEL(mod_loc1);

	PROGRAM_CNTXT_INIT(buff, 0);

	JOB_HDR(SHR_NEVER, buffpos, 0, 0);
	pstore = STORE(JOBDESCBUF, mod_loc1, 0, 4 * word_size, 0);
	SET_LABEL(mod_loc1);
	MOVE(CONTEXT1, 0, CONTEXT2, 0, 1, IMMED);

	PATCH_STORE(pstore, mod_loc1);

	return PROGRAM_FINALIZE();
}

unsigned test_store_sd(struct program *share_desc, uint32_t *buff,
		       unsigned buffpos)
{
	struct program *program = share_desc;
	REFERENCE(pstore);

	PROGRAM_CNTXT_INIT(buff, 0);

	SHR_HDR(SHR_NEVER, buffpos, 0);
	pstore = STORE(SHAREDESCBUF, shr_loc, 0, 4 * word_size, 0);
	SET_LABEL(shr_loc);
	MOVE(CONTEXT1, 0, CONTEXT2, 0, 1, IMMED);
	MATHB(ZERO, ADD, ONE, MATH1, 1, 0);
	MATHB(ZERO, ADD, ONE, MATH1, 1, 0);
	MATHB(ZERO, ADD, ONE, MATH1, 1, 0);
	pstore2 = STORE(JOBDESCBUF, 0, 0, 4 * word_size, 0);

	PATCH_STORE(pstore, shr_loc);

	return PROGRAM_FINALIZE();
}

unsigned test_store_jd2(struct program *job_desc, uint32_t *buff,
			unsigned buffpos)
{
	struct program *program = job_desc;
	uint64_t shrloc = 0x72650040ull;

	PROGRAM_CNTXT_INIT(buff, 0);

	JOB_HDR(SHR_NEVER, buffpos, shrloc, SHR);
	MATHB(ZERO, ADD, ONE, MATH1, 1, 0);
	SET_LABEL(mod_loc2);
	MOVE(CONTEXT1, 0, CONTEXT2, 0, 1, IMMED);
	STORE(DESCBUF, mod_loc2 * word_size, descwords, 4 * word_size, 0);
	MATHB(ZERO, ADD, ONE, MATH1, 1, 0);
	MATHB(ZERO, ADD, ONE, MATH1, 1, 0);
	pstore3 = STORE(JOBDESCBUF, 0, 0, 4 * word_size, 0);
	pstore4 = STORE(SHAREDESCBUF, 0, 0, 4 * word_size, 0);

	return PROGRAM_FINALIZE();
}

int main(int argc, char **argv)
{
	struct program share_desc, job_desc;
	unsigned cmd_size, jd1_size, jd2_size, sd_size;

	rta_set_sec_era(RTA_SEC_ERA_3);

	cmd_size = test_store_cmds(prg_buff);
	jd1_size = test_store_jd1(jd1_buff, 0);
	sd_size = test_store_sd(&share_desc, sd_buff, 0);
	jd2_size = test_store_jd2(&job_desc, jd2_buff, sd_size);

	PATCH_STORE_NON_LOCAL(&share_desc, pstore2, mod_loc2);
	PATCH_STORE_NON_LOCAL(&job_desc, pstore3, mod_loc2);
	PATCH_STORE_NON_LOCAL(&job_desc, pstore4, shr_loc);

	pr_debug("STORE commands\n");
	pr_debug("size = %d\n", cmd_size);
	print_prog(prg_buff, cmd_size);

	pr_debug("STORE Job Desc #1\n");
	pr_debug("size = %d\n", jd1_size);
	print_prog(jd1_buff, jd1_size);

	pr_debug("STORE Shared Desc\n");
	pr_debug("size = %d\n", sd_size);
	print_prog(sd_buff, sd_size);

	pr_debug("STORE Job Desc #2\n");
	pr_debug("size = %d\n", jd2_size);
	print_prog(jd2_buff, jd2_size);

	return 0;
}
