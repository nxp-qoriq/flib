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
		pjump1 = JUMP(IMM(do_dcrc), LOCAL_JUMP, ALL_TRUE, WITH(MATH_N));

		/* Start bringing in Preamble */
		LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0, 0);
		SEQFIFOLOAD(PKA0, 60, 0);
		LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0, 0);
		pmove1 = MOVE(IFIFOABD, 0, DESCBUF, 0, IMM(12), 0);
		MOVE(IFIFOABD, 0, KEY1, 0, IMM(key_size), 0);
		MOVE(IFIFOABD, 0, CONTEXT2, 32, IMM(32), 0);
		LOAD(IMM(key_size), KEY1SZ, 0, 4, 0);
		MOVE(CONTEXT2, 32, MATH0, 0, IMM(32), 0);
		/* Get SEQ IN PTR command from Job Descriptor */
		ref_job_seqout = MOVE(DESCBUF, 0, MATH2, 0, IMM(12),
				      WITH(WAITCOMP));
		/* Mask out the bit so SOP becomes SIP; maintain ptr value */
		MATHB(MATH2, AND, IMM(0xf7ffffffffffffff), MATH2, 8, 0);
		MATHB(SEQINSZ, ADD, MATH3, MATH3, 8, WITH(NFU));
		/* swap words */
		MATHB(MATH0, SHLD, MATH3, MATH3, 8, 0);
		ref_job_seqin = MOVE(MATH2, 0, DESCBUF, 0, IMM(12), 0);
		MOVE(CONTEXT2, 48, MATH2, 0, IMM(16), WITH(WAITCOMP));

		/* Build a MOVE command to process input data */
		SET_LABEL(build_move);
		/*
		 * RTA does not support generating MATH commands with LEN = 8
		 * and no immediate inlined in the command, thus use WORD().
		 */
		WORD(0xaa004004);
		MOVE(IFIFOABD, 0, OFIFO, 0, IMM(0), 0);

		MATHB(ZERO, ADD, MATH0, VSEQINSZ, 2, WITH(NFU));
		MATHB(ZERO, ADD, MATH0, VSEQOUTSZ, 2, WITH(NFU));
		MATHB(MATH0, SHLD, MATH0, MATH0, 8, WITH(NFU));
		pmove4 = MOVE(MATH0, 0, DESCBUF, 0, IMM(4), WITH(WAITCOMP));
		LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0, 0);
		SEQFIFOLOAD(PKA0, 0, WITH(VLF));
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));
		LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0, 0);

		SET_LABEL(get_data);
		WORD(0);
		ref_all_done = JUMP(IMM(all_done), LOCAL_JUMP, ALL_TRUE,
				    WITH(MATH_N));

		SET_LABEL(process_pdu);
		MATHB(ZERO, ADD, MATH1, SEQINSZ, 4, 0);
		PROTOCOL(OP_TYPE_ENCAP_PROTOCOL, OP_PCLID_3G_RLC_PDU,
			 WITH(OP_PCL_3G_RLC_KASUMI));
		LOAD(IMM
		     (CLRW_CLR_C1CTX | CLRW_CLR_C1ICV | CLRW_CLR_C1MODE |
		      CLRW_CLR_C1DATAS), CLRW, 0, 4, 0);
		LOAD(IMM(CCTRL_RESET_CHA_KFHA), CCTRL, 0, 4, 0);
		MOVE(CONTEXT2, 32, MATH0, 0, IMM(32), WITH(WAITCOMP));
		MATHB(MATH3, SUB, ONE, MATH3, 8, 0);
		MOVE(MATH3, 0, CONTEXT2, 56, IMM(8), 0);
		pjump3 = JUMP(IMM(process_pdu), LOCAL_JUMP, ANY_FALSE,
			      WITH(MATH_Z));
		MATHB(MATH0, LSHIFT, IMM(48), MATH0, 8, WITH(IFB));
		MOVE(MATH0, 0, DESCBUF, 4, IMM(4), 0);
		MATHB(ZERO, SUB, ONE, NONE, 4, 0);
		MATHB(ZERO, ADD, MATH2, SEQINSZ, 4, WITH(NFU));
		MATHB(ZERO, ADD, MATH2, MATH0, 4, WITH(NFU));
		pjump4 = JUMP(IMM(build_move), LOCAL_JUMP, ALL_TRUE, 0);

		SET_LABEL(do_dcrc);
		PROTOCOL(OP_TYPE_ENCAP_PROTOCOL, OP_PCLID_3G_DCRC,
			 WITH(OP_PCL_3G_DCRC_CRC7));
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
	JOB_HDR(SHR_ALWAYS, buffpos, desc_addr_1, WITH(REO | SHR));
	{
		SET_LABEL(encap_job_seqout);
		SET_LABEL(all_done);
		SEQOUTPTR(pdu_out_addr_1, output_frame_length, WITH(EXT));
		SET_LABEL(encap_job_seqin);
		SEQINPTR(pdu_in_addr_1, input_frame_length, WITH(EXT));
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
	print_prog((uint32_t *) lte_desc, lte_desc_size);

	pr_debug("size = %d\n", job_desc_size);
	print_prog((uint32_t *) job_desc, job_desc_size);

	return 0;
}
