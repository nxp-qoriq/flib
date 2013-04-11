#ifndef __RTA_LOAD_CMD_H__
#define __RTA_LOAD_CMD_H__

extern enum rta_sec_era rta_sec_era;

/* Allowed length and offset masks for each SEC Era in case DST = _DCTRL */
static const uint32_t load_len_mask_allowed[] = {
	0x000000ee,
	0x000000ef,
	0x000000ef,
	0x000000ef,
	0x000000ef
};

static const uint32_t load_off_mask_allowed[] = {
	0x0000000f,
	0x000000ff,
	0x000000ff,
	0x000000ff,
	0x000000ff
};

#define IMM_MUST 0
#define IMM_CAN  1
#define IMM_NO   2
#define IMM_DSNM 3 /* it doesn't matter the src type */

enum e_lenoff {
	LENOF_03,
	LENOF_4,
	LENOF_48,
	LENOF_448,
	LENOF_18,
	LENOF_32,
	LENOF_24,
	LENOF_16,
	LENOF_8,
	LENOF_128,
	LENOF_256,
	DSNM /* it doesn't matter the lenght/offset values */
};

struct load_map {
	uint32_t dst;
	uint32_t dst_opcode;
	enum e_lenoff len_off;
	uint8_t imm_src;

};

static const struct load_map load_dst[] = {
/*1*/	{ _KEY1SZ,  LDST_CLASS_1_CCB | LDST_SRCDST_WORD_KEYSZ_REG,
		    LENOF_4,   IMM_MUST },
	{ _KEY2SZ,  LDST_CLASS_2_CCB | LDST_SRCDST_WORD_KEYSZ_REG,
		    LENOF_4,   IMM_MUST },
	{ _DATA1SZ, LDST_CLASS_1_CCB | LDST_SRCDST_WORD_DATASZ_REG,
		    LENOF_448, IMM_MUST },
	{ _DATA2SZ, LDST_CLASS_2_CCB | LDST_SRCDST_WORD_DATASZ_REG,
		    LENOF_448, IMM_MUST },
	{ _ICV1SZ,  LDST_CLASS_1_CCB | LDST_SRCDST_WORD_ICVSZ_REG,
		    LENOF_4,   IMM_MUST },
	{ _ICV2SZ,  LDST_CLASS_2_CCB | LDST_SRCDST_WORD_ICVSZ_REG,
		    LENOF_4,   IMM_MUST },
	{ _CCTRL,   LDST_CLASS_IND_CCB | LDST_SRCDST_WORD_CHACTRL,
		    LENOF_4,   IMM_MUST },
	{ _DCTRL,   LDST_CLASS_DECO | LDST_IMM | LDST_SRCDST_WORD_DECOCTRL,
		    DSNM,      IMM_DSNM },
	{ _ICTRL,   LDST_CLASS_IND_CCB | LDST_SRCDST_WORD_IRQCTRL,
		    LENOF_4,   IMM_MUST },
	{ _DPOVRD,  LDST_CLASS_DECO | LDST_SRCDST_WORD_DECO_PCLOVRD,
		    LENOF_4,   IMM_MUST },
	{ _CLRW,    LDST_CLASS_IND_CCB | LDST_SRCDST_WORD_CLRW,
		    LENOF_4,   IMM_MUST },
	{ _AAD1SZ,  LDST_CLASS_1_CCB | LDST_SRCDST_WORD_DECO_AAD_SZ,
		    LENOF_4,   IMM_MUST },
	{ _IV1SZ,   LDST_CLASS_1_CCB | LDST_SRCDST_WORD_CLASS1_IV_SZ,
		    LENOF_4,   IMM_MUST },
	{ _ALTDS1,  LDST_CLASS_1_CCB | LDST_SRCDST_WORD_ALTDS_CLASS1,
		    LENOF_448, IMM_MUST },
	{ _PKASZ,   LDST_CLASS_1_CCB | LDST_SRCDST_WORD_PKHA_A_SZ,
		    LENOF_4,   IMM_MUST, },
	{ _PKBSZ,   LDST_CLASS_1_CCB | LDST_SRCDST_WORD_PKHA_B_SZ,
		    LENOF_4,   IMM_MUST },
	{ _PKNSZ,   LDST_CLASS_1_CCB | LDST_SRCDST_WORD_PKHA_N_SZ,
		    LENOF_4,   IMM_MUST },
	{ _PKESZ,   LDST_CLASS_1_CCB | LDST_SRCDST_WORD_PKHA_E_SZ,
		    LENOF_4,   IMM_MUST },
	{ _NFIFO,   LDST_CLASS_IND_CCB | LDST_SRCDST_WORD_INFO_FIFO,
		    LENOF_48,  IMM_MUST },
	{ _IFIFO,   LDST_SRCDST_BYTE_INFIFO,  LENOF_18, IMM_MUST },
	{ _OFIFO,   LDST_SRCDST_BYTE_OUTFIFO, LENOF_18, IMM_MUST },
	{ _MATH0,   LDST_CLASS_DECO | LDST_SRCDST_WORD_DECO_MATH0,
		    LENOF_32,  IMM_CAN },
	{ _MATH1,   LDST_CLASS_DECO | LDST_SRCDST_WORD_DECO_MATH1,
		    LENOF_24,  IMM_CAN },
	{ _MATH2,   LDST_CLASS_DECO | LDST_SRCDST_WORD_DECO_MATH2,
		    LENOF_16,  IMM_CAN },
	{ _MATH3,   LDST_CLASS_DECO | LDST_SRCDST_WORD_DECO_MATH3,
		    LENOF_8,   IMM_CAN },
	{ _CONTEXT1, LDST_CLASS_1_CCB | LDST_SRCDST_BYTE_CONTEXT,
		    LENOF_128, IMM_CAN },
	{ _CONTEXT2, LDST_CLASS_2_CCB | LDST_SRCDST_BYTE_CONTEXT,
		    LENOF_128, IMM_CAN },
	{ _KEY1,    LDST_CLASS_1_CCB | LDST_SRCDST_BYTE_KEY,
		    LENOF_32,  IMM_CAN },
	{ _KEY2,    LDST_CLASS_2_CCB | LDST_SRCDST_BYTE_KEY,
		    LENOF_32,  IMM_CAN },
	{ _DESCBUF, LDST_CLASS_DECO | LDST_SRCDST_WORD_DESCBUF,
		    LENOF_256,  IMM_NO },
	{ _DPID,    LDST_CLASS_DECO | LDST_SRCDST_WORD_PID,
		    LENOF_448, IMM_MUST },
/*32*/	{ _IDFNS,   LDST_SRCDST_WORD_IFNSR, LENOF_18,  IMM_MUST },
	{ _ODFNS,   LDST_SRCDST_WORD_OFNSR, LENOF_18,  IMM_MUST },
	{ _ALTSOURCE, LDST_SRCDST_BYTE_ALTSOURCE, LENOF_18,  IMM_MUST },
/*35*/	{ _NFIFO_SZL, LDST_SRCDST_WORD_INFO_FIFO_SZL, LENOF_48, IMM_MUST },
	{ _NFIFO_SZM, LDST_SRCDST_WORD_INFO_FIFO_SZM, LENOF_03, IMM_MUST },
	{ _NFIFO_L, LDST_SRCDST_WORD_INFO_FIFO_L, LENOF_48, IMM_MUST },
	{ _NFIFO_M, LDST_SRCDST_WORD_INFO_FIFO_M, LENOF_03, IMM_MUST },
	{ _SZL,     LDST_SRCDST_WORD_SZL, LENOF_48, IMM_MUST },
/*40*/	{ _SZM,     LDST_SRCDST_WORD_SZM, LENOF_03, IMM_MUST }
};

/*
 * Allowed LOAD destinations for each SEC Era.
 * Values represent the number of entries from load_dst[] that are supported.
 */
static const uint32_t load_dst_sz[] = { 31, 34, 34, 40, 40 };

static inline int8_t load_check_len_offset(int8_t pos, uint32_t length,
		uint32_t offset)
{
	if ((load_dst[pos].dst == _DCTRL) &&
	    ((length & ~load_len_mask_allowed[rta_sec_era]) ||
	     (offset & ~load_off_mask_allowed[rta_sec_era])))
		goto err;

	switch (load_dst[pos].len_off) {
	case (LENOF_03):
		if ((length > 3) || (offset))
			goto err;
		break;
	case (LENOF_4):
		if ((length != 4) || (offset != 0))
			goto err;
		break;
	case (LENOF_48):
		if (!(((length == 4) && (offset == 0))
		      || ((length == 8) && (offset == 0))))
			goto err;
		break;
	case (LENOF_448):
		if (!(((length == 4) && (offset == 0))
		      || ((length == 4) && (offset == 4))
		      || ((length == 8) && (offset == 0))))
			goto err;
		break;
	case (LENOF_18):
		if ((length < 1) || (length > 8) || (offset != 0))
			goto err;
		break;
	case (LENOF_32):
		if ((length > 32) || (offset > 32) || ((offset + length) > 32))
			goto err;
		break;
	case (LENOF_24):
		if ((length > 24) || (offset > 24) || ((offset + length) > 24))
			goto err;
		break;
	case (LENOF_16):
		if ((length > 16) || (offset > 16) || ((offset + length) > 16))
			goto err;
		break;
	case (LENOF_8):
		if ((length > 8) || (offset > 8) || ((offset + length) > 8))
			goto err;
		break;
	case (LENOF_128):
		if ((length > 128) || (offset > 128) ||
				     ((offset + length) > 128))
			goto err;
		break;
	case (LENOF_256):
		if ((length < 1) || (length > 256) || ((length + offset) > 256))
			goto err;
		break;
	case (DSNM):
		break;
	default:
		goto err;
		break;
	}

	return 0;
err:
	return -1;
}

static inline unsigned load(struct program *program, uint64_t src,
			    uint32_t src_type, uint64_t dst, uint32_t dst_type,
			    uint32_t offset, uint32_t length, uint32_t flags)
{
	uint32_t opcode = 0, i;
	int8_t pos = -1;
	unsigned start_pc = program->current_pc;

	if (flags & SEQ)
		opcode = CMD_SEQ_LOAD;
	else
		opcode = CMD_LOAD;

	if ((length & 0xffffff00) || (offset & 0xffffff00)) {
		pr_debug("LOAD: Bad length/offset passed. Should be 8 bits\n");
		goto err;
	}

	if (flags & SGF)
		opcode |= LDST_SGF;
	if (flags & VLF)
		opcode |= LDST_VLF;

	/* check load destination, length and offset and source type */
	for (i = 0; i < load_dst_sz[rta_sec_era]; i++)
		if (dst == load_dst[i].dst) {
			pos = i;
			break;
		}
	if (-1 == pos) {
		pr_debug("LOAD: Invalid dst. SEC Program Line: %d\n",
				program->current_pc);
		goto err;
	}

	if ((src_type == IMM_DATA) || (flags & IMMED)) {
		if (load_dst[pos].imm_src == IMM_NO) {
			pr_debug("LOAD: Invalid source type. "
					"SEC Program Line: %d\n",
					program->current_pc);
			goto err;
		}
		opcode |= LDST_IMM;
	} else if (load_dst[pos].imm_src == IMM_MUST) {
		pr_debug("LOAD IMM: Invalid source type. "
				"SEC Program Line: %d\n",
				program->current_pc);
		goto err;
	}

	if (-1 == load_check_len_offset(pos, length, offset)) {
		pr_debug("LOAD: Invalid length/offset. SEC Program Line: %d\n",
				program->current_pc);
		goto err;
	}

	opcode |= load_dst[pos].dst_opcode;

	/* DESC BUFFER: length / offset values are specified in 4-byte words */
	if (dst == _DESCBUF) {
		opcode |= (length >> 2);
		opcode |= ((offset >> 2) << LDST_OFFSET_SHIFT);
	} else {
		opcode |= length;
		opcode |= (offset << LDST_OFFSET_SHIFT);
	}

	program->buffer[program->current_pc] = opcode;
	program->current_pc++;
	program->current_instruction++;

	/* DECO COTROL: skip writing pointer of imm data */
	if (dst == _DCTRL)
		return program->current_pc;

	/* For data copy, 3 possible ways to specify how to copy data:
	 *  - src_type is IMM: copy data directly from src( max 8 bytes)
	 *  - src_type is PTR, but data should be immed: copy the data imm
	 *    from the location specified by user
	 *  - src_type is PTR and is not SEQ cmd: copy the address */
	if (src_type == IMM_DATA) {
		if (length > BYTES_4) {
			program->buffer[program->current_pc] = high_32b(src);
			program->current_pc++;
		}

		program->buffer[program->current_pc] = low_32b(src);
		program->current_pc++;
	}

	if ((src_type == PTR_DATA) && (flags & IMMED)) {
		uint8_t *tmp =
			(uint8_t *) &program->buffer[program->current_pc];

		for (i = 0; i < length; i++)
			*tmp++ = ((uint8_t *)(uintptr_t)src)[i];
		program->current_pc += ((length + 3) / 4);

		return program->current_pc;
	}

	if ((src_type == PTR_DATA) && (!(flags & SEQ))) {
		if (program->ps == 1) {
			program->buffer[program->current_pc] = high_32b(src);
			program->current_pc++;
		}

		program->buffer[program->current_pc] = low_32b(src);
		program->current_pc++;
	}

	return start_pc;

 err:
	program->first_error_pc = start_pc;
	program->current_instruction++;
	return start_pc;
}

#endif /* __RTA_LOAD_CMD_H__*/
