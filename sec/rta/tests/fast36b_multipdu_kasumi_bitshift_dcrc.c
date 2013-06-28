#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;

uint64_t shraddr = 0x51100030ull;
REFERENCE(ref_jump_reload);
LABEL(reload);

unsigned build_shdesc_kasumi_bitshift_dcrc(struct program *prg, uint32_t *buff,
					   unsigned buffpos)
{
	struct program *program = prg;

	LABEL(do_dcrc);
	REFERENCE(pjump1);
	LABEL(lab23);
	REFERENCE(pjump2);
	LABEL(lab33);
	REFERENCE(pmove1);
	LABEL(lab35);
	REFERENCE(pjump3);
	LABEL(pdb1);
	REFERENCE(pmove2);

	PROGRAM_CNTXT_INIT(buff, buffpos);
	PROGRAM_SET_36BIT_ADDR();

	SHR_HDR(SHR_NEVER, 6, 0);
	{
		{
			/* PDB: 3G RLC ENCAP PDB */
			SET_LABEL(pdb1);
			WORD(0x00000000); /*[01] 00000000  opt_word  */
			WORD(0x00000000); /*[02] 00000000  HFN       */
			WORD(0x00000000); /*[03] 00000000  Bearer,Dir*/
			WORD(0x44444444); /*[04] 44444444  THRESHOLD */
			WORD(0x55555555); /*[05] 55555555  0x55555555*/
		}
		/* [06] A000082E         jump:  jsl0 all-match[math-n]
		 *                              offset=46 local->[52] */
		pjump1 = JUMP(IMM(do_dcrc), LOCAL_JUMP, ALL_TRUE, WITH(MATH_N));
		/* [07] 1C200040        seqld: ccb2-ctx len=64 offs=0 */
		SEQLOAD(CONTEXT2, 0, 64, 0);
		/* [08] 7813040C         move:  class2-ctx->descbuf, len=12,
		 *                              dstoffs=4(w1), srcoffs=0(0) */
		pmove2 = MOVE(CONTEXT2, 0, DESCBUF, 0, IMM(12), 0);
		/* [09] 78142020         move:  class2-ctx->math0, len=32,
		 *                              srcoffs=32 */
		MOVE(CONTEXT2, 32, MATH0, 0, IMM(32), 0);
		/* [10] 781D1010         move:  class2-ctx->class1-key, len=16,
		 *                              srcoffs=16 */
		MOVE(CONTEXT2, 16, KEY1, 0, IMM(16), 0);
		/* [11] 12810004           ld: ccb1-keysz len=4 offs=0 imm */
		/* [12] 00000010               data:0x00000010  */
		LOAD(IMM(16), KEY1SZ, 0, 4, 0);
		/* [13] 79360000         move:  descbuf->math2, len=0,
		 *                              srcoffs=0(w0) wait */
		MOVE(DESCBUF, 0, MATH2, 0, IMM(0), WAITCOMP);
		/* [14] A8524208         math: (math2 & imm1)->math2 len=8 */
		/* [15] F7FFFFFF               imm1=0xf7ffffff_ffffffff */
		/* [16] FFFFFFFF  */
		MATHB(MATH2, AND, IMM(0xf7ffffffffffffff), MATH2, 8, 0);
		/* [17] 78630000         move:  math2->descbuf, len=0,
		 *                              dstoffs=0(w0) */
		MOVE(MATH2, 0, DESCBUF, 0, IMM(0), 0);
		/* [18] 79360000         move:  descbuf->math2, len=0,
		 *                              srcoffs=0(w0) wait */
		MOVE(DESCBUF, 0, MATH2, 0, IMM(0), WITH(WAITCOMP));
		/* [19] AA021208         math: (math2 + math1)->math2 len=8
		 *                              nfu */
		MATHB(MATH2, ADD, MATH1, MATH2, 8, WITH(NFU));
		/* [20] AE224208         math: (math2 - imm1)->math2 len=8 ifb
		 *                             nfu */
		/* [21] 00000042               imm1=66 */
		MATHB(MATH2, SUB, IMM(66), MATH2, 8, WITH(NFU | IFB));
		/* [22] 78630000         move:  math2->descbuf, len=0,
		 *                              dstoffs=0(w0) */
		MOVE(MATH2, 0, DESCBUF, 0, IMM(0), 0);
		SET_LABEL(lab23);
		/* [23] AA004004         math: (math0 + imm1)->math0 len=4
		 *                             nfu */
		/* [24] 78820000               imm1=2021785600 */
		MATHB(MATH0, ADD, IMM(0x78820000), MATH0, 4, WITH(NFU));
		/* [25] AA0C0A02         math: (0 + math0)->vseqin len=2
		 *                              nfu */
		MATHB(ZERO, ADD, MATH0, VSEQINSZ, 2, WITH(NFU));
		/* [26] AA0C0B02         math: (0 + math0)->vseqout len=2
		 *                              nfu */
		MATHB(ZERO, ADD, MATH0, VSEQOUTSZ, 2, WITH(NFU));
		/* [27] AA900008         math: (<math0> shld math0)->math0
		 *                             len=8 nfu */
		MATHB(MATH0, SHLD, MATH0, MATH0, 8, WITH(NFU));
		/* [28] 79438404         move:  math0->descbuf, len=4,
		 *                              dstoffs=132(w33) wait */
		pmove1 = MOVE(MATH0, 0, DESCBUF, 0, IMM(4), WITH(WAITCOMP));
		/* [29] 16860800           ld: deco-deco-ctrl len=0 offs=8
		 *                             imm -auto-nfifo-entries */
		LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0, 0);
		/* [30] 2B000000    seqfifold: class1 pk-a0 vlf */
		SEQFIFOLOAD(PKA0, 0, WITH(VLF));
		/* [31] 69300000   seqfifostr: deco msgdata vlf */
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));
		/* [32] 16860400           ld: deco-deco-ctrl len=0 offs=4
		 *                             imm +auto-nfifo-entries */
		LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0, 0);
		SET_LABEL(lab33);
		/* [33] 78820000         move:  deco-algnblk->ofifo, len=0 */
		MOVE(IFIFOABD, 0, OFIFO, 0, IMM(0), 0);
		/* [34] A0000816         jump:  jsl0 all-match[math-n]
		 *                              offset=22 local->[56] */
		ref_jump_reload = JUMP(IMM(reload), LOCAL_JUMP, ALL_TRUE,
				       WITH(MATH_N));
		SET_LABEL(lab35);
		/* [35] A80C3804         math: (0 + math3)->seqin len=4 */
		MATHB(ZERO, ADD, MATH3, SEQINSZ, 4, 0);
		/* [36] 87320001    operation: encap 3g_rlc_pdu pclinfo=0x0001
		 *                             kasumi */
		PROTOCOL(OP_TYPE_ENCAP_PROTOCOL, OP_PCLID_3G_RLC_PDU,
			 WITH(OP_PCL_3G_RLC_KASUMI));
		/* [37] 10880004           ld: ind-clrw len=4 offs=0 imm */
		/* [38] 0000002D               clrw: clr_c1mode clr_c1datas
		 *                                   clr_c1icv clr_c1ctx */
		LOAD(IMM
		     (CLRW_CLR_C1CTX | CLRW_CLR_C1ICV | CLRW_CLR_C1MODE |
		      CLRW_CLR_C1DATAS), CLRW, 0, 4, 0);
		/* [39] 10860004           ld: ind-cha-ctrl len=4 offs=0 imm */
		/* [40] 00000010               ctrl: reset_cha_kfha */
		LOAD(IMM(CCTRL_RESET_CHA_KFHA), CCTRL, 0, 4, 0);
		/* [41] 79142020         move:  class2-ctx->math0, len=32,
		 *                              srcoffs=32 wait */
		MOVE(CONTEXT2, 32, MATH0, 0, IMM(32), WITH(WAITCOMP));
		/* [42] A821C108         math: (math1 - 1)->math1 len=8 */
		MATHB(MATH1, SUB, ONE, MATH1, 8, 0);
		/* [43] 78512808         move:  math1->class2-ctx, len=8,
		 *                              dstoffs=40 */
		MOVE(MATH1, 0, CONTEXT2, 40, IMM(8), 0);
		/* [44] A00104F7         jump:  jsl0 all-mismatch[math-z]
		 *                              offset=-9 local->[35] */
		pjump3 = JUMP(IMM(lab35), LOCAL_JUMP, ANY_FALSE, WITH(MATH_Z));
		/* [45] AC704008         math: (math0 << imm1)->math0 len=8
		 *                             ifb */
		/* [46] 00000030               imm1=48 */
		MATHB(MATH0, LSHIFT, IMM(48), MATH0, 8, WITH(IFB));
		/* [47] 78430404         move:  math0->descbuf, len=4,
		 *                              dstoffs=4(w1) */
		MOVE(MATH0, 0, DESCBUF, 4, IMM(4), 0);
		/* [48] A82CCF04         math: (0 - 1)->none len=4 */
		MATHB(ZERO, SUB, ONE, NONE, 4, 0);
		/* [49] AA0C2804         math: (0 + math2)->seqin len=4 nfu */
		MATHB(ZERO, ADD, MATH2, SEQINSZ, 4, WITH(NFU));
		/* [50] AA0C2004         math: (0 + math2)->math0 len=4 nfu */
		MATHB(ZERO, ADD, MATH2, MATH0, 4, WITH(NFU));
		/* [51] A00000E4         jump:  all-match[] always-jump
		 *                              offset=-28 local->[23] */
		pjump2 = JUMP(IMM(lab23), LOCAL_JUMP, ALL_TRUE, 0);
		SET_LABEL(do_dcrc);
		/* [52] 87310710    operation: encap 3g_dcrc pclinfo=0x0710
		 *                             crc7 */
		PROTOCOL(OP_TYPE_ENCAP_PROTOCOL, OP_PCLID_3G_DCRC,
			 WITH(OP_PCL_3G_DCRC_CRC7));
	}
	PATCH_JUMP(pjump1, do_dcrc);
	PATCH_JUMP(pjump2, lab23);
	PATCH_MOVE(pmove1, lab33);
	PATCH_JUMP(pjump3, lab35);
	PATCH_MOVE(pmove2, pdb1);

	return PROGRAM_FINALIZE();
}

unsigned build_jbdesc_kasumi_bitshift_dcrc(struct program *prg, uint32_t *buff,
					   unsigned buffpos)
{
	struct program *program = prg;

	PROGRAM_CNTXT_INIT(buff, buffpos);
	PROGRAM_SET_36BIT_ADDR();
	JOB_HDR(SHR_ALWAYS, buffpos, shraddr, WITH(REO | SHR));
	{
		SET_LABEL(reload);
		SEQINPTR(0, 0, 0);
		SEQOUTPTR(0, 0, 0);
	}

	return PROGRAM_FINALIZE();
}

int main(int argc, char **argv)
{
	uint32_t share[64];
	uint32_t job[16];
	unsigned lte_desc_size, job_desc_size;
	struct program lte_desc_prgm;
	struct program job_desc_prgm;

	rta_set_sec_era(RTA_SEC_ERA_2);

	memset(share, 0xFF, sizeof(share));
	lte_desc_size =
	    build_shdesc_kasumi_bitshift_dcrc(&lte_desc_prgm, share, 0);

	memset(job, 0xFF, sizeof(job));
	job_desc_size =
	    build_jbdesc_kasumi_bitshift_dcrc(&job_desc_prgm, job,
					      lte_desc_size);

	PATCH_JUMP_NON_LOCAL(&lte_desc_prgm, ref_jump_reload, reload);

	pr_debug("FAST36B multipdu kasumi bitshift program\n");
	pr_debug("size = %d\n", lte_desc_size);
	print_prog((uint32_t *) share, lte_desc_size);

	pr_debug("size = %d\n", job_desc_size);
	print_prog((uint32_t *) job, job_desc_size);

	return 0;
}
