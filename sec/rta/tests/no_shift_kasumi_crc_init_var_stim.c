#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

/* Global variables section */
enum rta_sec_era rta_sec_era;

static LABEL(encap_share_end);
static REFERENCE(encap_share_end_ref3);
static REFERENCE(encap_share_end_ref4);
static REFERENCE(encap_share_end_ref5);
static REFERENCE(encap_share_end_ref6);
static REFERENCE(encap_share_end_ref7);
static LABEL(back_from_extras);
static REFERENCE(back_from_extras_ref1);
static LABEL(yet_more);
static REFERENCE(yet_more_ref1);
static LABEL(loop_start);
static REFERENCE(loop_start_ref1);

/* the last group of extra commands, stored temporarily in C2 context */
const uint32_t num_extras = 10;
const uint32_t num_extras_bytes = 10 * 4;

/* the first group of extra commands, moved to desc buf right off */
const uint32_t num_more_extras = 11;
const uint32_t num_more_extras_bytes = 11 * 4;

/* the second group of extra commands, stored temporarily in C1 context */
const uint32_t num_even_more_extras = 11;
const uint32_t num_even_more_extras_bytes = 11 * 4;

/* the third group of extra commands, stored in C2 context */
const uint32_t num_yet_more_extras = 8;
const uint32_t num_yet_more_extras_bytes = 8 * 4;

/* the fourth group of extra commands, sotred in C1 context */
const uint32_t num_still_more_extras = 2;
const uint32_t num_still_more_extras_bytes = 2 * 4;

/* lengths in the various contexts: */
const uint32_t num_ctx2_extras =
    /*num_extras_bytes */ 10 * 4 + /*num_yet_more_extras_bytes */ 8 * 4;
const uint32_t num_ctx1_extras =
    /*num_even_more_extras_bytes */ 11 * 4 + 4 +
    /*num_still_more_extras_bytes */ 2 * 4;

int generate_lte_code(struct program *prg, uint32_t *buff, int mdatalen,
		      int buffpos)
{
	int size;
	struct program *program = prg;

	LABEL(last_info);
	LABEL(datasz_len);
	LABEL(datasz1_len);
	LABEL(info_len);
	LABEL(after_info);
	REFERENCE(pjump1);
	REFERENCE(pjump2);
	REFERENCE(pjump3);
	REFERENCE(pmove1);
	REFERENCE(pmove2);
	REFERENCE(pmove3);
	REFERENCE(pmove4);
	REFERENCE(pmove5);
	REFERENCE(pmove6);
	REFERENCE(pmove7);
	REFERENCE(pmove8);
	REFERENCE(pmove9);

	PROGRAM_CNTXT_INIT(buff, 0);

	SHR_HDR(SHR_ALWAYS, 0, 0);
	{
		WORD(0x04800004);
		WORD(0x80050000);

		/* NOTE Math 0 contains 0 at this point */
		MATHB(SEQINSZ, ADD, MATH0, VSEQINSZ, BYTES_4, 0);
		LOAD(IMM(0), DCTRL, 8, 0, 0);
		SEQFIFOLOAD(MSG1, 0, WITH(VLF));
		LOAD(IMM(0), DCTRL, 4, 0, 0);
		/* First extra command words */
		pmove1 =
		    MOVE(IFIFOABD, 0, DESCBUF, encap_share_end,
			 IMM(num_more_extras_bytes), WITH(FLUSH1 | WAITCOMP));
		pjump1 = JUMP(IMM(encap_share_end), LOCAL_JUMP, ALL_TRUE, 0);

		SET_LABEL(back_from_extras);

		/* here is the main loop */
		SET_LABEL(loop_start);
		/* get length of this PDU */
		MOVE(IFIFOABD, 0, MATH0, 4, IMM(4), WITH(FLUSH1));

		/* calculate the data size for CRC */
		pmove2 = MOVE(DESCBUF, datasz_len, MATH0, 0, IMM(4), 0);
		pmove3 = MOVE(MATH0, 0, DESCBUF, datasz_len, IMM(8), 0);

		/*
		 * The input to cipher is 2 bytes less than B. And the amount
		 * to SEQ FIFO STORE is equal to B
		 */
		MATHB(ZERO, ADD, MATH0, VSEQOUTSZ, BYTES_2, 0);
		/* B-2: Size of each PDU not including the header */
		MATHB(MATH0, SUB, IMM(2), MATH0, BYTES_2, 0);

		pmove4 = MOVE(DESCBUF, datasz1_len, MATH0, 0, IMM(4), 0);
		pmove5 = MOVE(MATH0, 0, DESCBUF, datasz1_len, IMM(8), 0);

		/* calculate the info fifo size for CRC */
		/* get stuff that precedes where INFO LENGTH goes */
		pmove6 = MOVE(DESCBUF, info_len, MATH0, 0, IMM(6), 0);
		/* put it back */
		pmove7 = MOVE(MATH0, 0, DESCBUF, info_len, IMM(8), 0);
		/* get stuff that precedes where INFO LENGTH goes */
		pmove8 = MOVE(DESCBUF, last_info, MATH0, 0, IMM(6), 0);
		/* put it back */
		pmove9 = MOVE(MATH0, 0, DESCBUF, last_info, IMM(8), 0);

		/*
		 * MATH3 - HFN/Bearer
		 * MATH1 - C
		 * MATH0 - nothing (scratch)
		 * MATH2 - N (counting down)
		 */

		/*
		 * Note that it's ok to mess with C1 Context since we
		 * overwrite all the tmp stuff with the actual starting
		 * context before the OPERATION is executed. We also cheat by
		 * writing the context AFTER we do the data size. That's ok,
		 * because we always have the context ready prior to the
		 * OPERATION command.
		 */

		/* Get PDU header */
		MOVE(IFIFOABD, 0, CONTEXT1, 0, IMM(2), WITH(FLUSH1));

		SET_LABEL(datasz_len);
		LOAD(IMM(0), DATA2SZ, 0, BYTES_4, 0);
		SET_LABEL(datasz1_len);
		LOAD(IMM(0), DATA1SZ, 0, BYTES_4, 0);

		/* PDU Header to Math 3 */
		MOVE(CONTEXT1, 0, MATH0, 0, IMM(8), WITH(WAITCOMP));
		/* Get PDU into right 2 bytes */
		MATHB(MATH0, RSHIFT, IMM(48), MATH0, BYTES_8, WITH(IFB));
		/* PDU Header to Math 3 left 2 bytes */
		MOVE(CONTEXT1, 0, MATH0, 0, IMM(2), 0);
		/* Put it where CRC can snoop & FIFO STORE can get it */
		MOVE(MATH0, 0, OFIFO, 0, IMM(8), 0);

		/* now extract the SEQ NUM for context. mask out all but the
		 * SEQ NUM */
		MATHB(MATH0, AND, IMM(0x7FF8), MATH0, BYTES_4, 0);
		/* get SEQ NUM to right 12 bits of left word */
		MATHB(MATH0, LSHIFT, IMM(29), MATH0, BYTES_8, WITH(IFB));
		/* merge in HFN, bearer, D */
		MATHB(MATH0, OR, MATH3, MATH0, BYTES_8, 0);
		/* get context in place */
		MOVE(MATH0, 0, CONTEXT1, 0, IMM(8), 0);

		ALG_OPERATION(OP_ALG_ALGSEL_KASUMI,
			      OP_ALG_AAI_F8, OP_ALG_AS_INITFINAL,
			      ICV_CHECK_DISABLE, DIR_ENC);
		/* Have CRC grab the PDU */
		LOAD(IMM
		     (NFIFOENTRY_STYPE_OFIFO | NFIFOENTRY_DEST_CLASS2 |
		      NFIFOENTRY_DTYPE_MSG | 2), NFIFO, 0, BYTES_4, 0);

		/* need to know if both C & N are 0 */
		MATHB(MATH1, ADD, MATH2, MATH0, BYTES_2, 0);
		/* but haven't decremented N yet */
		MATHB(MATH0, SUB, ONE, MATH0, BYTES_2, 0);
		/* do the final entry */
		pjump2 = JUMP(IMM(last_info), LOCAL_JUMP, ALL_TRUE,
				WITH(MATH_Z));
		SET_LABEL(info_len);
		LOAD(IMM(NFIFOENTRY_STYPE_SNOOP | NFIFOENTRY_DEST_BOTH |
			 NFIFOENTRY_DTYPE_MSG | NFIFOENTRY_LC1),
		     NFIFO, 0, BYTES_4, 0);

		/* skip over the last entry */
		pjump3 = JUMP(IMM(after_info), LOCAL_JUMP, ALL_TRUE, 0);
		SET_LABEL(last_info);
		LOAD(IMM(NFIFOENTRY_STYPE_SNOOP | NFIFOENTRY_DEST_BOTH |
			 NFIFOENTRY_DTYPE_MSG | NFIFOENTRY_LC1 |
			 NFIFOENTRY_LC2), NFIFO, 0, BYTES_4, 0);
		SET_LABEL(after_info);

		/* Create offset in output FIFO of 6 */
		WORD(0x16860016);
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));

		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CLASS1 | NOP));
		LOAD(IMM
		     (CLRW_CLR_C1MODE | CLRW_CLR_C1DATAS | CLRW_CLR_C1ICV |
		      CLRW_CLR_C1CTX), CLRW, 0, BYTES_4, 0);

		SET_LABEL(encap_share_end);
	}

	/* Patch section */
	PATCH_JUMP(program, pjump1, encap_share_end);
	PATCH_JUMP(program, pjump2, last_info);
	PATCH_JUMP(program, pjump3, after_info);

	PATCH_MOVE(program, pmove1, encap_share_end);
	PATCH_MOVE(program, pmove2, datasz_len);
	PATCH_MOVE(program, pmove3, datasz_len);
	PATCH_MOVE(program, pmove4, datasz1_len);
	PATCH_MOVE(program, pmove5, datasz1_len);
	PATCH_MOVE(program, pmove6, info_len);
	PATCH_MOVE(program, pmove7, info_len);
	PATCH_MOVE(program, pmove8, last_info);
	PATCH_MOVE(program, pmove9, last_info);

	size = PROGRAM_FINALIZE();
	return size;
}

/*
 * This command group goes in last and is there when the descriptor finishes
 * This command group waits at the end of CTX2 until it is put in place
 */
int generate_extra_desc_code(struct program *prg, uint32_t *buff, int mdatalen,
			     int buffpos)
{
	int size;
	struct program *program = prg;

	PROGRAM_CNTXT_INIT(buff, buffpos);

	MATHB(MATH2, SUB, ONE, MATH2, 4, 0);	/* Done with a PDU */

	/* back to loop start if not zero & Kasumi done */
	loop_start_ref1 = JUMP(IMM(loop_start), LOCAL_JUMP, ALL_FALSE, MATH_Z);
	/* done with loop, move the tail */
	MATHB(ZERO, ADD, MATH1, VSEQOUTSZ, 2, 0);
	/* get the Tail */
	MOVE(IFIFOABD, 0, MATH1, 0, IMM(0), 0);
	/* label tail_move_i really goes here */
	MOVE(MATH1, 0, IFIFOAB2, 0, IMM(0), WITH(LAST2));
	/* put it out there; MOVE makes sure store data is there */
	SEQSTORE(MATH1, 0, 0, WITH(VLF));
	SEQSTORE(CONTEXT2, 0, 2, 0);

	JUMP(NONE, HALT_STATUS, ALL_TRUE, 0);
	size = PROGRAM_FINALIZE();
	return size;
}

/*
 * This command group goes directly to the descriptor buffer,
 * it is never in context
 */
int generate_more_extra_desc_code(struct program *prg, uint32_t *buff,
				  int mdatalen, int buffpos)
{
	int size;
	struct program *program = prg;

	PROGRAM_CNTXT_INIT(buff, buffpos);

	/* end extras to C2 context for later use */
	MOVE(IFIFOABD, 0, CONTEXT2, 0, IMM(num_ctx2_extras), WITH(FLUSH1));
	MOVE(IFIFOABD, 0, CONTEXT1, 0, IMM(num_ctx1_extras), WITH(FLUSH1));
	MOVE(IFIFOABD, 0, MATH0, 4, IMM(28), WITH(FLUSH1));

	MOVE(CONTEXT2, 48, MATH1, 0, IMM(6), 0); /* fix up the tail length */
	MOVE(MATH1, 0, CONTEXT2, 48, IMM(8), 0);
	MOVE(CONTEXT2, 52, MATH1, 0, IMM(6), 0); /* fix up the tail length */
	MOVE(MATH1, 0, CONTEXT1, 52, IMM(8), 0);

	MATHB(ZERO, ADD, MATH0, VSEQOUTSZ, 4, 0);	/* size of the header */
	MOVE(CONTEXT2, 4, MATH0, 0, IMM(4), 0);	/* fix up head size */
	encap_share_end_ref3 = MOVE(CONTEXT1, 0, DESCBUF, encap_share_end,
				    IMM(num_even_more_extras_bytes),
				    WITH(WAITCOMP));
	encap_share_end_ref4 =
	    JUMP(IMM(encap_share_end), LOCAL_JUMP, ALL_TRUE, 0);

	size = PROGRAM_FINALIZE();
	return size;

}

/*
 * This command group sits at the start of CTX1 until needed.
 * It is the 2nd group to go in.
 */
int generate_even_more_extra_desc_code(struct program *prg, uint32_t *buff,
				       int mdatalen, int buffpos)
{
	int size;
	struct program *program = prg;

	PROGRAM_CNTXT_INIT(buff, buffpos);

	MOVE(MATH0, 0, CONTEXT2, 4, IMM(8), 0);
	/* fix up head info FIFO entry */
	MOVE(CONTEXT2, 8, MATH0, 0, IMM(6), 0);
	MOVE(MATH0, 0, CONTEXT2, 8, IMM(8), 0);
	MOVE(CONTEXT2, 16, MATH0, 0, IMM(6), 0);	/* fix up head move */
	MOVE(MATH0, 0, CONTEXT2, 16, IMM(8), 0);
	MOVE(IFIFOABD, 0, KEY1, 0, IMM(16), WITH(FLUSH1));

	LOAD(IMM(16), KEY1SZ, 0, 4, 0);

	encap_share_end_ref5 = MOVE(CONTEXT2, 0, DESCBUF, encap_share_end,
				    IMM(num_yet_more_extras_bytes), 0);
	yet_more_ref1 = MOVE(CONTEXT1, 48, DESCBUF, yet_more,
			     IMM(num_still_more_extras_bytes), 0);
	encap_share_end_ref6 =
	    JUMP(IMM(encap_share_end), LOCAL_JUMP, ALL_TRUE, 0);

	size = PROGRAM_FINALIZE();
	return size;
}

/*
 * This command group sits in the first 8 words of CTX2 until needed.
 * It is the 3rd group to go in.
 */
int generate_yet_more_extra_desc_code(struct program *prg, uint32_t *buff,
				      int mdatalen, int buffpos)
{
	int size;
	struct program *program = prg;

	PROGRAM_CNTXT_INIT(buff, buffpos);

	/* get CRC init value into place */
	MOVE(IFIFOABD, 0, CONTEXT2, 0, IMM(4), WITH(FLUSH1));

	LOAD(IMM(0), DATA2SZ, 0, 4, 0);
	MOVE(IFIFOABD, 0, OFIFO, 0, IMM(0), WITH(FLUSH1));
	LOAD(IMM
	     (NFIFOENTRY_STYPE_OFIFO | NFIFOENTRY_DEST_CLASS2 |
	      NFIFOENTRY_DTYPE_MSG | 2), NFIFO, 0, 4, 0);

	/*
	 * It's a race - but the needed commands should be in the pipeline
	 * before the MOVE overwrites them!
	 */
	encap_share_end_ref7 =
	    MOVE(CONTEXT2, 32, DESCBUF, encap_share_end, IMM(num_extras_bytes),
		 0);
	ALG_OPERATION(OP_ALG_ALGSEL_CRC,
		      OP_ALG_AAI_CUST_POLY | OP_ALG_AAI_IVZ | OP_ALG_AAI_DOC |
		      OP_ALG_AAI_DIS | OP_ALG_AAI_DOS, OP_ALG_AS_FINALIZE,
		      ICV_CHECK_DISABLE, DIR_ENC);

	size = PROGRAM_FINALIZE();
	return size;
}

/*
 * This command group sits in the last 2 words of CTX1 until needed.
 * It is the 4th group to go in. (It is actually present at the same time as
 * the 3rd group!
 */
int generate_still_more_extra_desc_code(struct program *prg, uint32_t *buff,
					int mdatalen, int buffpos)
{
	int size;
	struct program *program = prg;

	PROGRAM_CNTXT_INIT(buff, buffpos);

	SET_LABEL(yet_more);
	SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));
	back_from_extras_ref1 =
	    JUMP(IMM(back_from_extras), LOCAL_JUMP, ALL_TRUE, 0);

	size = PROGRAM_FINALIZE();
	return size;
}

int main(int argc, char *argv[])
{
	uint32_t lte_desc[20000];
	uint32_t extra_desc[20000];
	uint32_t more_extra_desc[20000];
	uint32_t even_more_extra_desc[20000];
	uint32_t yet_more_extra_desc[20000];
	uint32_t still_more_extra_desc[20000];
	int lte_desc_size, extra_desc_size, more_extra_desc_size;
	int even_more_extra_desc_size, yet_more_extra_desc_size;
	int still_more_extra_desc_size;

	struct program lte_prgm;
	struct program extra_prgm;
	struct program more_extra_prgm;
	struct program even_more_extra_prgm;
	struct program yet_more_extra_prgm;
	struct program still_more_extra_prgm;

	rta_set_sec_era(RTA_SEC_ERA_1);

	memset(lte_desc, 0xFF, sizeof(lte_desc));
	lte_desc_size = generate_lte_code(&lte_prgm, lte_desc, 500, 0);

	memset(extra_desc, 0xFF, sizeof(extra_desc));
	extra_desc_size =
	    generate_extra_desc_code(&extra_prgm, extra_desc, 500,
				     lte_desc_size);

	memset(more_extra_desc, 0xFF, sizeof(more_extra_desc));
	more_extra_desc_size =
	    generate_more_extra_desc_code(&more_extra_prgm, more_extra_desc,
					  500, lte_desc_size);

	memset(even_more_extra_desc, 0xFF, sizeof(even_more_extra_desc));
	even_more_extra_desc_size =
	    generate_even_more_extra_desc_code(&even_more_extra_prgm,
					       even_more_extra_desc, 500,
					       lte_desc_size);

	memset(yet_more_extra_desc, 0xFF, sizeof(yet_more_extra_desc));
	yet_more_extra_desc_size =
	    generate_yet_more_extra_desc_code(&yet_more_extra_prgm,
					      yet_more_extra_desc, 500,
					      lte_desc_size);

	memset(still_more_extra_desc, 0xFF, sizeof(still_more_extra_desc));
	still_more_extra_desc_size =
	    generate_still_more_extra_desc_code(&still_more_extra_prgm,
						still_more_extra_desc, 500,
						(lte_desc_size +
						 even_more_extra_desc_size));

	/* Patch section start */
	PATCH_MOVE(&more_extra_prgm, encap_share_end_ref3, encap_share_end);

	PATCH_JUMP(&more_extra_prgm, encap_share_end_ref4, encap_share_end);

	PATCH_MOVE(&even_more_extra_prgm, encap_share_end_ref5,
		   encap_share_end);

	PATCH_JUMP(&even_more_extra_prgm, encap_share_end_ref6,
		   encap_share_end);

	PATCH_MOVE(&yet_more_extra_prgm, encap_share_end_ref7, encap_share_end);

	PATCH_JUMP(&still_more_extra_prgm, back_from_extras_ref1,
		   back_from_extras);

	PATCH_MOVE(&even_more_extra_prgm, yet_more_ref1, yet_more);

	PATCH_JUMP(&extra_prgm, loop_start_ref1, loop_start);
	/* Patch section end */

	pr_debug("#title LTE Commands\n");
	print_prog(lte_desc, lte_desc_size);

	pr_debug("#title Extra Descriptor Commands\n");
	print_prog(extra_desc, extra_desc_size);

	pr_debug("#title More Extra Descriptor Commands\n");
	print_prog(more_extra_desc, more_extra_desc_size);

	pr_debug("#title Even More Extra Descriptor Commands\n");
	print_prog(even_more_extra_desc, even_more_extra_desc_size);

	pr_debug("#title Yet More Extra Descriptor Commands\n");
	print_prog(yet_more_extra_desc, yet_more_extra_desc_size);

	pr_debug("#title Still More Extra Descriptor Commands\n");
	print_prog(still_more_extra_desc, still_more_extra_desc_size);

	return 0;
}

/*
 lte_desc
 -------------
[00] B8800335       shrhdr: (see manual) jumpto=[00]
[01] 04800004          key: class2-keyreg len=4 imm
[02] 80050000               key=[80050000]
[03] A8080A04         math: (seqin + math0)->vseqin len=4
[04] 16860800           ld: deco-deco-ctrl len=0 offs=8 imm -auto-nfifo-entries
[05] 2B100000    seqfifold: class1 msgdata vlf
[06] 16860400           ld: deco-deco-ctrl len=0 offs=4 imm +auto-nfifo-entries
[07] 7D83D42C         move: deco-algnblk -> descbuf+212[53], len=44 wait a_ms!?
[08] A000002D         jump: all-match[] always-jump offset=45 local->[53]
[09] 7C840404         move: deco-algnblk -> math0+4, len=4 a_ms!?
[10] 78345804         move: descbuf+88[22] -> math0, len=4
[11] 78435808         move: math0 -> descbuf+88[22], len=8
[12] A80C0B02         math: (0 + math0)->vseqout len=2
[13] A8204002         math: (math0 - imm1)->math0 len=2
[14] 00000002               imm1=2
[15] 78346004         move: descbuf+96[24] -> math0, len=4
[16] 78436008         move: math0 -> descbuf+96[24], len=8
[17] 7834AC06         move: descbuf+172[43] -> math0, len=6
[18] 7843AC08         move: math0 -> descbuf+172[43], len=8
[19] 7834B806         move: descbuf+184[46] -> math0, len=6
[20] 7843B808         move: math0 -> descbuf+184[46], len=8
[21] 7C800002         move: deco-algnblk -> class1-ctx+0, len=2 a_ms!?
[22] 14820004           ld: ccb2-datasz len=4 offs=0 imm
[23] 00000000               data:0x00000000
[24] 12820004           ld: ccb1-datasz len=4 offs=0 imm
[25] 00000000               data:0x00000000
[26] 79040008         move: class1-ctx+0 -> math0, len=8 wait
[27] AC804008         math: (math0 >> imm1)->math0 len=8 ifb
[28] 00000030               imm1=48
[29] 78040002         move: class1-ctx+0 -> math0, len=2
[30] 78420008         move: math0+0 -> ofifo, len=8
[31] A8504004         math: (math0 & imm1)->math0 len=4
[32] 00007FF8               imm1=32760
[33] AC704008         math: (math0 << imm1)->math0 len=8 ifb
[34] 0000001D               imm1=29
[35] A8403008         math: (math0 | math3)->math0 len=8
[36] 78400008         move: math0 -> class1-ctx+0, len=8
[37] 82700C0D    operation: cls1-op kasumi f8 init-final enc
[38] 10FA0004           ld: ind-nfifo len=4 offs=0 imm
[39] 81F00002               <nfifo_entry: ofifo->class2 type=msg/rsvd len=2>
[40] A8012002         math: (math1 + math2)->math0 len=2
[41] A820C002         math: (math0 - 1)->math0 len=2
[42] A0000404         jump: jsl0 all-match[math-z] offset=4 local->[46]
[43] 10FA0004           ld: ind-nfifo len=4 offs=0 imm
[44] D3F00000               <nfifo_entry: snoop->both type=msg/rsvd lc1 len=0>
[45] A0000003         jump: all-match[] always-jump offset=3 local->[48]
[46] 10FA0004           ld: ind-nfifo len=4 offs=0 imm
[47] F3F00000               <nfifo_entry: snoop->both type=msg/rsvd
				lc2 lc1 len=0>
[48] 16860016           ld: deco-deco-ctrl len=22 offs=0 imm ofifo_offset=valid
				ofifo_offset=2
[49] 69300000   seqfifostr: msgdata vlf
[50] A3000201         jump: class1-done jsl1 all-match[nop] offset=1 local->[51]
[51] 10880004           ld: ind-clrw len=4 offs=0 imm
[52] 0000002D               clrw: clr_c1mode clr_c1datas clr_c1icvs clr_c1ctx

extra_desc
--------------
[00] A822C204         math: (math2 - 1)->math2 len=4
[01] A00104D3         jump: jsl0 all-mismatch[math-z] offset=-45 local->[-10]
[02] A80C1B02         math: (0 + math1)->vseqout len=2
[03] 78850000         move: ififo->deco-alnblk -> math1+0, len=0
[04] 7A590000         move: math1+0 -> class2-alnblk, len=0 a_ls=last
[05] 5F090000      seqstr: deco math1 vlf offs=0
[06] 5C200002      seqstr: ccb2 ctx len=2 offs=0
[07] A0C00051         jump: all-match[] always-jump halt-user status=81

more_extra_desc
--------------
[00] 7C810048         move: ififo->deco-alnblk -> class2-ctx+0, len=72 !!a_ms!!
[01] 7C800038         move: ififo->deco-alnblk -> class1-ctx+0, len=56 !!a_ms!!
[02] 7C84041C         move: ififo->deco-alnblk -> math0+4, len=28 !!a_ms!!
[03] 78153006         move: class2-ctx+48 -> math1, len=6
[04] 78513008         move: math1 -> class2-ctx+48, len=8
[05] 78153406         move: class2-ctx+52 -> math1, len=6
[06] 78503408         move: math1 -> class1-ctx+52, len=8
[07] A80C0B04         math: (0 + math0)->vseqout len=4
[08] 78140404         move: class2-ctx+4 -> math0, len=4
[09] 7903D42C         move: class1-ctx+0 -> descbuf+212[53], len=44 wait
[10] A00000F6         jump: all-match[] always-jump offset=-10 local->[01]

even_more_extra_desc
--------------
[00] 78410408         move: math0 -> class2-ctx+4, len=8
[01] 78140806         move: class2-ctx+8 -> math0, len=6
[02] 78410808         move: math0 -> class2-ctx+8, len=8
[03] 78141006         move: class2-ctx+16 -> math0, len=6
[04] 78411008         move: math0 -> class2-ctx+16, len=8
[05] 7C8D0010         move: ififo->deco-alnblk -> class1-key+0, len=16 !!a_ms!!
[06] 12810004           ld: ccb1-keysz len=4 offs=0 imm
[07] 00000010               data:0x00000010
[08] 7813D420         move: class2-ctx+0 -> descbuf+212[53], len=32
[09] 7E030008         move: class1-ctx+48 -> descbuf+0[00], len=8 (aux_ms)
				(aux_ls)
[10] A00000F6         jump: all-match[] always-jump offset=-10 local->[01]

yet_more_extra_desc
--------------
[00] 7C810004         move: ififo->deco-alnblk -> class2-ctx+0, len=4 !!a_ms!!
[01] 14820004           ld: ccb2-datasz len=4 offs=0 imm
[02] 00000000               data:0x00000000
[03] 7C820000         move: ififo->deco-alnblk -> ofifo, len=0 !!a_ms!!
[04] 10FA0004           ld: ind-nfifo len=4 offs=0 imm
[05] 81F00002               <nfifo_entry: ofifo->class2 type=msg/rsvd len=2>
[06] 7C13D428         move: class2-ctx+32 -> descbuf+212[53], len=40 (aux_ms)
[07] 80900009    operation: uni-pcl <rsvd00> prot_info=0x0009

still_more_extra_desc
--------------
[00] 69300000   seqfifostr: msgdata len=vsol
[01] A00000C8         jump: all-match[] always-jump offset=-56 local->[-7]

*/
