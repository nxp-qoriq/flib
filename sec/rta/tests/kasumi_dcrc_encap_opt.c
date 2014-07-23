/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;

LABEL(encap_job_seqout);
REFERENCE(ref_job_seqout);
LABEL(all_done);
REFERENCE(ref_all_done);
LABEL(encap_job_seqin);
REFERENCE(ref_job_seqin);
LABEL(encap_share_end);

uint64_t desc_addr_1 = 0x00000040ull;

unsigned build_shdesc_kasumi_dcrc_encap(struct program *prg, uint32_t *buff,
					unsigned buffpos)
{
	struct program *program = prg;
	uint32_t key_size = 16;

	LABEL(foo);
	REFERENCE(pmove1);
	LABEL(get_data);
	REFERENCE(pmove4);
	LABEL(do_dcrc);
	REFERENCE(pjump1);
	LABEL(process_pdu);
	REFERENCE(pjump3);
	LABEL(build_move);
	REFERENCE(pjump4);

	PROGRAM_SET_36BIT_ADDR();

	PROGRAM_CNTXT_INIT(buff, buffpos);
	SHR_HDR(SHR_NEVER, 6, 0);
	{
		{	/* 3G RLC ENCAP PDB */
			SET_LABEL(foo);
			WORD(0);
			WORD(0);
			WORD(0);
			WORD(0x44444444);
			WORD(0x55555555);
		}
		pjump1 = JUMP(do_dcrc, LOCAL_JUMP, ALL_TRUE, MATH_N);

		/* Start bringing in Preamble */
		LOAD(0, DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0, IMMED);
		SEQFIFOLOAD(PKA0, 60, 0);
		LOAD(0, DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0, IMMED);
		pmove1 = MOVE(IFIFOABD, 0, DESCBUF, 0, 12, IMMED);
		MOVE(IFIFOABD, 0, KEY1, 0, key_size, IMMED);
		MOVE(IFIFOABD, 0, CONTEXT2, 32, 32, IMMED);
		LOAD(key_size, KEY1SZ, 0, 4, IMMED);
		MOVE(CONTEXT2, 32, MATH0, 0, 32, IMMED);
		/* Get SEQ IN PTR command from Job Descriptor */
		ref_job_seqout = MOVE(DESCBUF, 0, MATH2, 0, 12,
				      WAITCOMP | IMMED);
		/* Mask out the bit so SOP becomes SIP; maintain ptr value */
		MATHB(MATH2, AND, 0xf7ffffffffffffff, MATH2, 8, IMMED2);
		MATHB(SEQINSZ, ADD, MATH3, MATH3, 8, NFU);
		/* swap words */
		MATHB(MATH0, SHLD, MATH3, MATH3, 8, 0);
		ref_job_seqin = MOVE(MATH2, 0, DESCBUF, 0, 12, IMMED);
		MOVE(CONTEXT2, 48, MATH2, 0, 16, WAITCOMP | IMMED);

		/* Build a MOVE command to process input data */
		SET_LABEL(build_move);
		/*
		 * RTA does not support generating MATH commands with LEN = 8
		 * and no immediate inlined in the command, thus use WORD().
		 */
		WORD(0xaa004004);
		MOVE(IFIFOABD, 0, OFIFO, 0, 0, IMMED);

		MATHB(ZERO, ADD, MATH0, VSEQINSZ, 2, NFU);
		MATHB(ZERO, ADD, MATH0, VSEQOUTSZ, 2, NFU);
		MATHB(MATH0, SHLD, MATH0, MATH0, 8, NFU);
		pmove4 = MOVE(MATH0, 0, DESCBUF, 0, 4, WAITCOMP | IMMED);
		LOAD(0, DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0, IMMED);
		SEQFIFOLOAD(PKA0, 0, VLF);
		SEQFIFOSTORE(MSG, 0, 0, VLF);
		LOAD(0, DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0, IMMED);

		SET_LABEL(get_data);
		WORD(0);
		ref_all_done = JUMP(all_done, LOCAL_JUMP, ALL_TRUE, MATH_N);

		SET_LABEL(process_pdu);
		MATHB(ZERO, ADD, MATH1, SEQINSZ, 4, 0);
		PROTOCOL(OP_TYPE_ENCAP_PROTOCOL, OP_PCLID_3G_RLC_PDU,
			 OP_PCL_3G_RLC_KASUMI);
		LOAD(CLRW_CLR_C1CTX | CLRW_CLR_C1ICV | CLRW_CLR_C1MODE |
		     CLRW_CLR_C1DATAS, CLRW, 0, 4, IMMED);
		LOAD(CCTRL_RESET_CHA_KFHA, CCTRL, 0, 4, IMMED);
		MOVE(CONTEXT2, 32, MATH0, 0, 32, WAITCOMP | IMMED);
		MATHB(MATH3, SUB, ONE, MATH3, 8, 0);
		MOVE(MATH3, 0, CONTEXT2, 56, 8, IMMED);
		pjump3 = JUMP(process_pdu, LOCAL_JUMP, ANY_FALSE, MATH_Z);
		MATHB(MATH0, LSHIFT, 48, MATH0, 8, IFB | IMMED2);
		MOVE(MATH0, 0, DESCBUF, 4, 4, IMMED);
		MATHB(ZERO, SUB, ONE, NONE, 4, 0);
		MATHB(ZERO, ADD, MATH2, SEQINSZ, 4, NFU);
		MATHB(ZERO, ADD, MATH2, MATH0, 4, NFU);
		pjump4 = JUMP(build_move, LOCAL_JUMP, ALL_TRUE, 0);

		SET_LABEL(do_dcrc);
		PROTOCOL(OP_TYPE_ENCAP_PROTOCOL, OP_PCLID_3G_DCRC,
			 OP_PCL_3G_DCRC_CRC7);
		SET_LABEL(encap_share_end);
	}

	PATCH_JUMP(pjump1, do_dcrc);
	PATCH_JUMP(pjump3, process_pdu);
	PATCH_JUMP(pjump4, build_move);
	PATCH_MOVE(pmove1, foo);
	PATCH_MOVE(pmove4, get_data);

	return PROGRAM_FINALIZE();
}

unsigned build_jbdesc_kasumi_dcrc_encap(struct program *prg, uint32_t *buff,
					unsigned buffpos)
{
	struct program *program = prg;
	uint32_t input_frame_length = 2356;
	uint32_t output_frame_length = 2302;
	uint64_t pdu_in_addr_1 = 0x155ull;
	uint64_t pdu_out_addr_1 = 0x095a0967ull;

	PROGRAM_CNTXT_INIT(buff, buffpos);
	JOB_HDR(SHR_ALWAYS, buffpos, desc_addr_1, REO | SHR);
	{
		SET_LABEL(encap_job_seqout);
		SET_LABEL(all_done);
		SEQOUTPTR(pdu_out_addr_1, output_frame_length, EXT);
		SET_LABEL(encap_job_seqin);
		SEQINPTR(pdu_in_addr_1, input_frame_length, EXT);
	}

	return PROGRAM_FINALIZE();
}

int main(int argc, char **argv)
{
	uint32_t lte_desc[60];
	uint32_t job_desc[20];
	unsigned lte_desc_size, job_desc_size;
	struct program lte_desc_prgm;
	struct program job_desc_prgm;

	rta_set_sec_era(RTA_SEC_ERA_3);

	memset(lte_desc, 0xFF, sizeof(lte_desc));
	lte_desc_size =
	    build_shdesc_kasumi_dcrc_encap(&lte_desc_prgm, lte_desc, 0);

	memset(job_desc, 0xFF, sizeof(job_desc));
	job_desc_size =
	    build_jbdesc_kasumi_dcrc_encap(&job_desc_prgm, job_desc,
					   lte_desc_size);

	PATCH_JUMP_NON_LOCAL(&lte_desc_prgm, ref_all_done, all_done);
	PATCH_MOVE_NON_LOCAL(&lte_desc_prgm, ref_job_seqout, encap_job_seqout);
	PATCH_MOVE_NON_LOCAL(&lte_desc_prgm, ref_job_seqin, encap_job_seqin);

	pr_debug("KASUMI DCRC Decryption program\n");
	pr_debug("size = %d\n", lte_desc_size);
	print_prog((uint32_t *)lte_desc, lte_desc_size);

	pr_debug("size = %d\n", job_desc_size);
	print_prog((uint32_t *)job_desc, job_desc_size);

	return 0;
}
