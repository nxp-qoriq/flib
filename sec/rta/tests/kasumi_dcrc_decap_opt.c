#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;

LABEL(decap_job_seqoutptr);
REFERENCE(ref_decap_job);
LABEL(decap_share_end);

uint64_t desc_addr_1 = 0x00000ac0ull;

int build_shdesc_kasumi_dcrc_decap(struct program *prg, uint32_t *buff,
				   int buffpos)
{
	struct program *program = prg;
	int size;
	uint32_t key_size = 16;

	LABEL(foo);
	REFERENCE(pmove1);
	REFERENCE(pmove2);
	LABEL(handle_kasumi);
	REFERENCE(pjump1);
	LABEL(process_pdu);
	REFERENCE(pjump2);

	PROGRAM_CNTXT_INIT(buff, buffpos);
	SHR_HDR(SHR_NEVER, 6, 0);
	{
		{	/* 3G RLC DECAP PDB */
			SET_LABEL(foo);
			WORD(0);
			WORD(0);
			WORD(0);
			WORD(0x44444444);
			pjump1 = WORD(0x55555555);
		}
		JUMP(IMM(handle_kasumi), LOCAL_JUMP, ALL_TRUE, WITH(MATH_N));
		/*
		 * bring in all the "extra" stuff with an extra word to make
		 * things space out correctly
		 * Start bringing in Preamble and set up PDB for DCRC
		 */
		LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0, 0);
		SEQFIFOLOAD(PKA0, 60, 0);
		LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0, 0);
		MOVE(IFIFOABD, 0, CONTEXT1, 0, IMM(12), 0);
		MOVE(IFIFOABD, 0, KEY1, 0, IMM(key_size), 0);
		MOVE(IFIFOABD, 0, CONTEXT1, 32, IMM(32), 0);
		LOAD(IMM(key_size), KEY1SZ, 0, 4, 0);
		MOVE(CONTEXT1, 36, MATH0, 0, IMM(4), WITH(WAITCOMP));
		/* get PDU len in left 2 bytes */
		pmove1 = MATHB(MATH0, LSHIFT, IMM(16), MATH0, 8, WITH(IFB));
		MOVE(MATH0, 0, DESCBUF, foo, IMM(4), 0);
		PROTOCOL(OP_TYPE_DECAP_PROTOCOL, OP_PCLID_3G_DCRC,
			 WITH(OP_PCL_3G_DCRC_CRC7));
		ref_decap_job = MATHB(ZERO, SUB, ONE, NONE, 4, 0);
		/* re-execute Job Descriptor */
		SHR_HDR(SHR_NEVER, decap_job_seqoutptr, 0);

		SET_LABEL(handle_kasumi);
		pmove2 = MOVE(CONTEXT1, 32, MATH0, 0, IMM(32), 0);
		MOVE(CONTEXT1, 0, DESCBUF, foo, IMM(12), 0);
		/* skip over the preamble and the DCRC header */
		MATHB(MATH0, ADD, IMM(60), VSEQINSZ, 4, 0);
		SEQFIFOLOAD(SKIP, 0, WITH(VLF));
		/*
		 * store the Math register stuff back in the Class 2 Context
		 * Register - only need #PDUs and size
		 */
		MOVE(MATH1, 0, CONTEXT2, 8, IMM(8), 0);
		MOVE(MATH3, 0, CONTEXT2, 0, IMM(8), 0);
		/* need to clear the Class 1 Context prior to first run! */
		LOAD(IMM
		     (CLRW_CLR_C1CTX | CLRW_CLR_C1ICV | CLRW_CLR_C1MODE |
		      CLRW_CLR_C1DATAS), CLRW, 0, 4, 0);
		LOAD(IMM(CCTRL_RESET_CHA_KFHA), CCTRL, 0, 4, 0);

		SET_LABEL(process_pdu);
		MATHB(MATH1, ADD, ONE, SEQINSZ, 4, 0);
		PROTOCOL(OP_TYPE_DECAP_PROTOCOL, OP_PCLID_3G_RLC_PDU,
			 WITH(OP_PCL_3G_RLC_KASUMI));
		/* Give up KFHA so somebody else can use it during DCRC */
		LOAD(IMM
		     (CLRW_CLR_C1CTX | CLRW_CLR_C1ICV | CLRW_CLR_C1MODE |
		      CLRW_CLR_C1DATAS), CLRW, 0, 4, 0);
		LOAD(IMM(CCTRL_RESET_CHA_KFHA), CCTRL, 0, 4, 0);
		MOVE(CONTEXT2, 0, MATH0, 0, IMM(16), WITH(WAITCOMP));
		MATHB(MATH0, SUB, ONE, MATH0, 8, 0);
		pjump2 = MOVE(MATH0, 0, CONTEXT2, 0, IMM(8), 0);
		JUMP(IMM(process_pdu), LOCAL_JUMP, ANY_FALSE, WITH(MATH_Z));
		/*
		 * For ERA < 5: WORKAROUND FOR INPUT FIFO NIBBLE SHIFT
		 * REGISTER BUG
		 * MOVE(MATH0, 0, IFIFOAB1, 0, IMM(1), 0);
		 * LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_IFIFO, 0, 0);
		 * MOVE(IFIFOAB2, 0, OFIFO, 0, IMM(1), WITH(FLUSH2|WAITCOMP));
		 */
		SET_LABEL(decap_share_end);

	}

	PATCH_JUMP(program, pjump1, handle_kasumi);
	PATCH_JUMP(program, pjump2, process_pdu);
	PATCH_MOVE(program, pmove1, foo);
	PATCH_MOVE(program, pmove2, foo);

	size = PROGRAM_FINALIZE();
	return size;
}

int build_jbdesc_kasumi_dcrc_decap(struct program *prg, uint32_t *buff,
				   int buffpos)
{
	struct program *program = prg;
	int size;
	uint32_t input_frame_length = 89;
	uint32_t output_frame_length = 89;
	uint64_t pdu_in_addr_1 = 0x00000b78ull;
	uint64_t pdu_out_addr_1 = 0x098e449dull;

	PROGRAM_CNTXT_INIT(buff, buffpos);
	JOB_HDR(SHR_ALWAYS, buffpos, desc_addr_1, WITH(REO | SHR));
	{
		SET_LABEL(decap_job_seqoutptr);
		SEQOUTPTR(pdu_out_addr_1, output_frame_length, WITH(EXT));
		SEQINPTR(pdu_in_addr_1, input_frame_length, WITH(EXT));
	}
	size = PROGRAM_FINALIZE();
	return size;
}

int main(int argc, char **argv)
{
	uint32_t lte_desc[60];
	uint32_t job_desc[20];
	int lte_desc_size, job_desc_size;

	struct program lte_desc_prgm;
	struct program job_desc_prgm;

	rta_set_sec_era(RTA_SEC_ERA_2);

	memset(lte_desc, 0xFF, sizeof(lte_desc));
	lte_desc_size =
	    build_shdesc_kasumi_dcrc_decap(&lte_desc_prgm, lte_desc, 0);

	memset(job_desc, 0xFF, sizeof(job_desc));
	job_desc_size =
	    build_jbdesc_kasumi_dcrc_decap(&job_desc_prgm, job_desc,
					   lte_desc_size);

	PATCH_HDR(&lte_desc_prgm, ref_decap_job, decap_job_seqoutptr);

	printf("KASUMI DCRC Decryption program\n");
	printf("size = %d\n", lte_desc_size);
	print_prog((uint32_t *) lte_desc, lte_desc_size);

	printf("size = %d\n", job_desc_size);
	print_prog((uint32_t *) job_desc, job_desc_size);

	return 0;
}
