#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;

uint64_t share_desc_addr = 0x089c2c80ULL;
uint32_t c1_ctxt_slots = 4;
uint32_t c2_ctxt_slots = 4;
uint32_t max_slots = 8;
uint32_t data_size = 3840;

LABEL(new_source);
REFERENCE(ref1_move_new_source);
REFERENCE(ref2_move_new_source);
LABEL(first);
REFERENCE(ref1_shr_first);
LABEL(last);
REFERENCE(ref1_shr_last);

int build_shdesc_raid_xor_opt(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;
	uint32_t chunk_size = 128;
	uint32_t fetch_limit = 64;

	LABEL(label_2);
	REFERENCE(pmove1);
	REFERENCE(pmove2);
	REFERENCE(pmove3);
	REFERENCE(pjump1);
	REFERENCE(phdr1);
	REFERENCE(phdr2);
	REFERENCE(phdr3);
	LABEL(return_point);
	REFERENCE(pjump2);
	REFERENCE(pjump3);
	REFERENCE(pjump4);
	LABEL(c);
	REFERENCE(phdr4);
	LABEL(store_here);
	REFERENCE(pjump5);

	PROGRAM_CNTXT_INIT(buff, buffpos);
	SHR_HDR(SHR_NEVER, 0, 0);
	{
		SEQLOAD(CONTEXT1, 0, c1_ctxt_slots * 16, 0);
		SEQLOAD(CONTEXT2, 0, c2_ctxt_slots * 16, 0);
		LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0, 0);
		pmove1 = MOVE(CONTEXT1, 0, DESCBUF, new_source, IMM(16),
			      WITH(WAITCOMP));

		phdr1 = SHR_HDR(SHR_NEVER, label_2, 0);

		SET_LABEL(label_2);
		SET_LABEL(new_source);
		new_source += 1;

		SEQINPTR(0, 0, WITH(EXT));
		WORD(0);
		WORD(0);
		SEQFIFOLOAD(SKIP, 0, WITH(VLF));

		SET_LABEL(c);
		SEQFIFOLOAD(PKA0, chunk_size, 0);
		MATHB(VSEQOUTSZ, ADD, IMM(chunk_size), VSEQOUTSZ, SIZE(4), 0);
		NFIFOADD(IFIFO, MSG, chunk_size, WITH(LAST1));

		SET_LABEL(return_point);
		MOVE(IFIFOABD, 0, MATH0, 0, IMM(32), WITH(WAITCOMP));
		MATHB(MATH0, XOR, OFIFO, MATH0, SIZE(8), 0);
		MATHB(MATH1, XOR, OFIFO, MATH1, SIZE(8), 0);
		MATHB(MATH2, XOR, OFIFO, MATH2, SIZE(8), 0);
		MATHB(MATH3, XOR, OFIFO, MATH3, SIZE(8), 0);
		MOVE(MATH0, 0, OFIFO, 0, IMM(32), 0);
		MATHB(VSEQOUTSZ, SUB, IMM(32), VSEQOUTSZ, SIZE(4), 0);
		pjump5 = JUMP(IMM(store_here), LOCAL_JUMP, ALL_TRUE,
			      WITH(MATH_Z));
		MATHB(VSEQOUTSZ, SUB, IMM(fetch_limit), NONE, SIZE(4), 0);
		pjump1 = JUMP(IMM(label_2), LOCAL_JUMP, ALL_TRUE, WITH(MATH_Z));
		pjump2 = JUMP(IMM(return_point), LOCAL_JUMP, ALL_TRUE, 0);

		SET_LABEL(store_here);
		SEQFIFOSTORE(MSG, 0, chunk_size, 0);
		MATHB(SEQOUTSZ, SUB, ONE, NONE, SIZE(4), 0);
		JUMP(IMM(0), HALT_STATUS, ALL_TRUE, WITH(MATH_N));
		MOVE(IFIFOABD, 0, OFIFO, 0, IMM(chunk_size), 0);
		phdr2 = SHR_HDR(SHR_NEVER, label_2, 0);

		SET_LABEL(first);
		SEQFIFOLOAD(SKIP, 0, WITH(VLF));
		MATHB(SEQINSZ, SUB, ONE, NONE, SIZE(4), 0);
		pjump3 = JUMP(IMM(return_point), LOCAL_JUMP, ALL_TRUE,
			      WITH(MATH_N));
		SEQFIFOLOAD(PKA0, chunk_size, 0);
		NFIFOADD(IFIFO, MSG, chunk_size, WITH(LAST1));

		pmove2 = MOVE(CONTEXT1, 16, DESCBUF, new_source, IMM(16), 0);
		MATHB(VSEQINSZ, SUB, ONE, NONE, SIZE(4), 0);
		pjump4 = JUMP(IMM(return_point), LOCAL_JUMP, ANY_FALSE,
			      WITH(MATH_N));
		MOVE(IFIFOABD, 0, OFIFO, 0, IMM(128), 0);
		MATHB(SEQINSZ, ADD, IMM(chunk_size), SEQOUTSZ, SIZE(4), 0);
		phdr3 = SHR_HDR(SHR_NEVER, label_2, 0);

		SET_LABEL(last);
		SEQFIFOLOAD(SKIP, 0, WITH(VLF));
		MATHB(VSEQINSZ, ADD, IMM(chunk_size), VSEQINSZ, SIZE(4), 0);
		pmove3 = MOVE(CONTEXT1, 0, DESCBUF, new_source, IMM(16), 0);
		phdr4 = SHR_HDR(SHR_NEVER, c, 0);
	}

	PATCH_MOVE(pmove1, new_source);
	PATCH_MOVE(pmove2, new_source);
	PATCH_MOVE(pmove3, new_source);
	PATCH_HDR(phdr1, label_2);
	PATCH_HDR(phdr2, label_2);
	PATCH_HDR(phdr3, label_2);
	PATCH_HDR(phdr4, c);
	PATCH_JUMP(pjump1, label_2);
	PATCH_JUMP(pjump2, return_point);
	PATCH_JUMP(pjump3, return_point);
	PATCH_JUMP(pjump4, return_point);
	PATCH_JUMP(pjump5, store_here);

	size = PROGRAM_FINALIZE();
	return size;
}

int build_jbdesc_raid_xor_opt(struct program *prg, uint32_t *buff, int buffpos)
{
	struct program *program = prg;
	int size;
	uint64_t context_ptr = 0x08858d80ULL;
	uint64_t store_ptr = 0x00000040ULL;

	PROGRAM_CNTXT_INIT(buff, buffpos);
	JOB_HDR(SHR_ALWAYS, buffpos, share_desc_addr, WITH(REO | SHR));
	{
		SEQOUTPTR(store_ptr, data_size, WITH(EXT));
		SEQINPTR(context_ptr, 8 * 16, 0);
	}
	size = PROGRAM_FINALIZE();
	return size;
}

int build_more_cmds_raid_xor_opt(struct program *prg, uint32_t *buff,
				 int buffpos)
{
	struct program *program = prg;
	int size;
	uint64_t load_data_addr[8];
	int i;
	uint32_t num_sources = 8;

	load_data_addr[0] = 0x00000f40ULL;
	load_data_addr[1] = 0x09dd9d00ULL;
	load_data_addr[2] = 0x00001e40ULL;
	load_data_addr[3] = 0x096fcc80ULL;
	load_data_addr[4] = 0x093c41c0ULL;
	load_data_addr[5] = 0x082acf40ULL;
	load_data_addr[6] = 0x08868a80ULL;
	load_data_addr[7] = 0x08093d40ULL;

	PROGRAM_CNTXT_INIT(buff, buffpos);
	{
		SEQINPTR(load_data_addr[0], data_size, WITH(EXT));
		ref1_shr_first = SHR_HDR(SHR_NEVER, first, 0);
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, 0);

		for (i = 1; i < (num_sources - 1); i++)
			if (i < (c1_ctxt_slots - 1)) {
				SEQINPTR(load_data_addr[i], data_size,
					 WITH(EXT));
				ref1_move_new_source =
					MOVE(CONTEXT1, (16 * (i + 1)), DESCBUF,
					     new_source, IMM(16), 0);
				JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, 0);
				PATCH_MOVE(ref1_move_new_source, new_source);
			} else {
				SEQINPTR(load_data_addr[i], data_size,
					 WITH(EXT));
				ref2_move_new_source =
					MOVE(CONTEXT2, (16 * (i + 1)), DESCBUF,
					     new_source, IMM(16), 0);
				JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, 0);
				PATCH_MOVE(ref2_move_new_source, new_source);
			}

		SEQINPTR(load_data_addr[i], data_size, WITH(EXT));
		ref1_shr_last = SHR_HDR(SHR_NEVER, last, 0);
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, 0);
	}

	PATCH_HDR(ref1_shr_first, first);
	PATCH_HDR(ref1_shr_last, last);

	size = PROGRAM_FINALIZE();
	return size;

}

int main(int argc, char **argv)
{
	uint32_t sharedesc[64];
	uint32_t jobdesc[10];
	uint32_t context_buf[64];
	int shr_size, job_size, ctx_size;

	struct program shr_desc_prgm;
	struct program job_desc_prgm;
	struct program ctx_buf_prgm;

	rta_set_sec_era(RTA_SEC_ERA_1);

	memset(sharedesc, 0xFF, sizeof(sharedesc));
	shr_size = build_shdesc_raid_xor_opt(&shr_desc_prgm, sharedesc, 0);

	memset(jobdesc, 0xFF, sizeof(jobdesc));
	job_size = build_jbdesc_raid_xor_opt(&job_desc_prgm, jobdesc, shr_size);

	memset(context_buf, 0xFF, sizeof(context_buf));
	ctx_size = build_more_cmds_raid_xor_opt(&ctx_buf_prgm, context_buf, 7);

	pr_debug("raid xor program shared desc\n");
	pr_debug("size = %d\n", shr_size);
	print_prog((uint32_t *) sharedesc, shr_size);

	pr_debug("raid xor program job desc\n");
	pr_debug("size = %d\n", job_size);
	print_prog((uint32_t *) jobdesc, job_size);

	pr_debug("raid xor program job context\n");
	pr_debug("size = %d\n", ctx_size);
	print_prog((uint32_t *) context_buf, ctx_size);

	return 0;
}
