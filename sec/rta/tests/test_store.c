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

int test_store_cmds(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;
	uintptr_t ctx = (uintptr_t) 0x55330087ull;
	uintptr_t sgtable = (uintptr_t) 0x22223333ull;
	int ivlen = 16;
	uintptr_t foo = (uintptr_t)0x34000080ull;
	uint8_t abc[] = {0x61, 0x62, 0x63};
	uint here = 32;

	PROGRAM_CNTXT_INIT(buff, 0);
	/* Class-independent CCB registers */
	STORE(CLRW, 0, PTR(foo), 4, 0);
	STORE(CCTRL, 0, PTR(foo), 4, 0);
	STORE(ICTRL, 0, PTR(foo), 4, 0);
	STORE(CSTAT, 0, PTR(foo), 4, 0);

	/* Class 1 CCB registers */
	STORE(MODE1, 0, PTR(foo), 4, 0);
	STORE(KEY1SZ, 0, PTR(foo), 4, 0);
	STORE(DATA1SZ, 0, PTR(foo), 4, 0);
	STORE(ICV1SZ, 0, PTR(foo), 4, 0);
	/* Class 1 Additional Data Size */
	STORE(AAD1SZ, 0, PTR(foo), 4, 0);
	STORE(IV1SZ, 0, PTR(foo), 4, 0);
	STORE(PKASZ, 0, PTR(foo), 4, 0);
	STORE(PKBSZ, 0, PTR(foo), 4, 0);
	STORE(PKNSZ, 0, PTR(foo), 4, 0);
	STORE(PKESZ, 0, PTR(foo), 4, 0);

	STORE(CONTEXT1, 0, PTR(sgtable), ivlen, WITH(SGF));
	STORE(CONTEXT1, 5, PTR(foo), 7, 0);

	/* Class 2 CCB registers */
	STORE(MODE2, 0, PTR(foo), 4, 0);
	STORE(KEY2SZ, 0, PTR(foo), 4, 0);
	STORE(DATA2SZ, 0, PTR(foo), 4, 0);
	STORE(ICV2SZ, 0, PTR(foo), 4, 0);
	STORE(CONTEXT2, 0, PTR(ctx), 16, 0);
	STORE(CONTEXT2, 8, PTR(foo), 4, 0);

	/* DECO / CAAM registers */
	STORE(DJQCTRL, 0, PTR(foo), 8, 0);
	/* DECO JQ Descriptor Address */
	STORE(DJQDA, 0, PTR(foo), 8, 0);
	STORE(DSTAT, 0, PTR(foo), 8, 0);
	STORE(DPID, 0, PTR(foo), 8, 0);

	SEQSTORE(MATH0, 0, 1, 0);
	SEQSTORE(MATH1, 0, 0, WITH(VLF));
	STORE(MATH2, 0, PTR(foo), 4, 0);
	STORE(MATH3, 0, PTR(foo), 4, 0);
	STORE(DESCBUF, 20 * word_size, PTR(descwords), 4 * word_size, 0);

	STORE(DESCBUF, here, PTR(descwords), 8 * word_size, 0);
	SEQSTORE(DESCBUF, here, 12 * word_size, 0);

	/* Immediate data */
	STORE(IMM(0x010203), 0, PTR(foo), 3, WITH(IMMED));
	STORE(PTR((uintptr_t) &abc), 0, PTR(foo), 3, WITH(IMMED));
	SEQSTORE(IMM(0x010203), 0, 3, WITH(IMMED));
	SEQSTORE(IMM(0x01020304050607), 0, 7, WITH(IMMED | VLF));
	SEQSTORE(PTR((uintptr_t) &abc), 0, 3, WITH(IMMED));

	size = PROGRAM_FINALIZE();

	return size;
}

int test_store_jd1(uint32_t *buff, uint32_t buffpos)
{
	struct program prg;
	struct program *program = &prg;
	int size;
	REFERENCE(pstore);
	LABEL(mod_loc1);

	PROGRAM_CNTXT_INIT(buff, 0);

	pstore = JOB_HDR(SHR_NEVER, buffpos, 0, 0);
	STORE(JOBDESCBUF, mod_loc1, IMM(0), 4 * word_size, 0);
	SET_LABEL(mod_loc1);
	MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(1), 0);

	PATCH_STORE(program, pstore, mod_loc1);

	size = PROGRAM_FINALIZE();

	return size;
}

int test_store_sd(struct program *share_desc, uint32_t *buff, uint32_t buffpos)
{
	struct program *program = share_desc;
	int size;
	REFERENCE(pstore);

	PROGRAM_CNTXT_INIT(buff, 0);

	pstore = SHR_HDR(SHR_NEVER, buffpos, 0);
	STORE(SHAREDESCBUF, shr_loc, IMM(0), 4 * word_size, 0);
	SET_LABEL(shr_loc);
	MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(1), 0);
	MATHB(ZERO, ADD, ONE, MATH1, SIZE(1), 0);
	MATHB(ZERO, ADD, ONE, MATH1, SIZE(1), 0);
	pstore2 = MATHB(ZERO, ADD, ONE, MATH1, SIZE(1), 0);
	STORE(JOBDESCBUF, mod_loc2, IMM(0), 4 * word_size, 0);

	PATCH_STORE(program, pstore, shr_loc);

	size = PROGRAM_FINALIZE();

	return size;
}

int test_store_jd2(struct program *job_desc, uint32_t *buff, uint32_t buffpos)
{
	struct program *program = job_desc;
	int size;
	uint64_t shrloc = 0x72650040ull;

	PROGRAM_CNTXT_INIT(buff, 0);

	JOB_HDR(SHR_NEVER, buffpos, shrloc, WITH(SHR));
	MATHB(ZERO, ADD, ONE, MATH1, SIZE(1), 0);
	SET_LABEL(mod_loc2);
	MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(1), 0);
	STORE(DESCBUF, mod_loc2 * word_size, PTR(descwords), 4 * word_size, 0);
	MATHB(ZERO, ADD, ONE, MATH1, SIZE(1), 0);
	pstore3 = MATHB(ZERO, ADD, ONE, MATH1, SIZE(1), 0);
	pstore4 = STORE(JOBDESCBUF, mod_loc2, IMM(0), 4 * word_size, 0);
	STORE(SHAREDESCBUF, shr_loc, IMM(0), 4 * word_size, 0);

	size = PROGRAM_FINALIZE();

	return size;
}

int main(int argc, char **argv)
{
	struct program share_desc, job_desc;
	int cmd_size, jd1_size, jd2_size, sd_size;

	rta_set_sec_era(RTA_SEC_ERA_3);

	cmd_size = test_store_cmds(prg_buff);
	jd1_size = test_store_jd1(jd1_buff, 0);
	sd_size = test_store_sd(&share_desc, sd_buff, 0);
	jd2_size = test_store_jd2(&job_desc, jd2_buff, sd_size);

	PATCH_STORE(&share_desc, pstore2, mod_loc2);
	PATCH_STORE(&job_desc, pstore3, mod_loc2);
	PATCH_STORE(&job_desc, pstore4, shr_loc);

	printf("STORE commands\n");
	printf("size = %d\n", cmd_size);
	print_prog(prg_buff, cmd_size);

	printf("STORE Job Desc #1\n");
	printf("size = %d\n", jd1_size);
	print_prog(jd1_buff, jd1_size);

	printf("STORE Shared Desc\n");
	printf("size = %d\n", sd_size);
	print_prog(sd_buff, sd_size);

	printf("STORE Job Desc #2\n");
	printf("size = %d\n", jd2_size);
	print_prog(jd2_buff, jd2_size);

	return 0;
}
