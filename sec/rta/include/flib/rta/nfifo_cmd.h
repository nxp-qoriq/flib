#ifndef __RTA_NFIFO_CMD_H__
#define __RTA_NFIFO_CMD_H__

extern enum rta_sec_era rta_sec_era;

static const uint32_t nfifo_src[][2] = {
/*1*/	{ _IFIFO,       NFIFOENTRY_STYPE_DFIFO },
	{ _OFIFO,       NFIFOENTRY_STYPE_OFIFO },
	{ _PAD,         NFIFOENTRY_STYPE_PAD },
/*4*/	{ _MSGOUTSNOOP, NFIFOENTRY_STYPE_SNOOP | NFIFOENTRY_DEST_BOTH },
	{ _ALTSOURCE,   NFIFOENTRY_STYPE_ALTSOURCE }
};

/*
 * Allowed NFIFO LOAD sources for each SEC Era.
 * Values represent the number of entries from nfifo_src[] that are supported.
 */
static const uint8_t nfifo_src_sz[] = {4, 5, 5, 5, 5};

static const uint32_t nfifo_data[][2] = {
	{ _MSG,   NFIFOENTRY_DTYPE_MSG },
	{ _MSG1,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_MSG },
	{ _MSG2,  NFIFOENTRY_DEST_CLASS2 | NFIFOENTRY_DTYPE_MSG },
	{ _IV1,   NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_IV },
	{ _IV2,   NFIFOENTRY_DEST_CLASS2 | NFIFOENTRY_DTYPE_IV },
	{ _ICV1,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_ICV },
	{ _ICV2,  NFIFOENTRY_DEST_CLASS2 | NFIFOENTRY_DTYPE_ICV },
	{ _SAD1,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_SAD },
	{ _AAD1,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_AAD },
	{ _AAD2,  NFIFOENTRY_DEST_CLASS2 | NFIFOENTRY_DTYPE_AAD },
	{ _AFHA_SBOX, NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_SBOX },
	{ _SKIP,  NFIFOENTRY_DTYPE_SKIP },
	{ _PKE,   NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_E },
	{ _PKN,   NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_N },
	{ _PKA,   NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_A },
	{ _PKA0,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_A0 },
	{ _PKA1,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_A1 },
	{ _PKA2,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_A2 },
	{ _PKA3,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_A3 },
	{ _PKB,   NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_B },
	{ _PKB0,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_B0 },
	{ _PKB1,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_B1 },
	{ _PKB2,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_B2 },
	{ _PKB3,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_B3 },
	{ _AB1,   NFIFOENTRY_DEST_CLASS1 },
	{ _AB2,   NFIFOENTRY_DEST_CLASS2 },
	{ _ABD,   NFIFOENTRY_DEST_DECO }
};

static const uint32_t nfifo_flags[][2] = {
/*1*/	{ LAST1,         NFIFOENTRY_LC1 },
	{ LAST2,         NFIFOENTRY_LC2 },
	{ FLUSH1,        NFIFOENTRY_FC1 },
	{ BP,            NFIFOENTRY_BND },
	{ PAD_ZERO,      NFIFOENTRY_PTYPE_ZEROS },
	{ PAD_NONZERO,   NFIFOENTRY_PTYPE_RND_NOZEROS },
	{ PAD_INCREMENT, NFIFOENTRY_PTYPE_INCREMENT },
	{ PAD_RANDOM,    NFIFOENTRY_PTYPE_RND },
	{ PAD_ZERO_N1,   NFIFOENTRY_PTYPE_ZEROS_NZ },
	{ PAD_NONZERO_0, NFIFOENTRY_PTYPE_RND_NZ_LZ },
	{ PAD_N1,        NFIFOENTRY_PTYPE_N },
/*12*/	{ PAD_NONZERO_N, NFIFOENTRY_PTYPE_RND_NZ_N },
	{ FLUSH2,        NFIFOENTRY_FC2 },
	{ OC,            NFIFOENTRY_OC }
};

/*
 * Allowed NFIFO LOAD flags for each SEC Era.
 * Values represent the number of entries from nfifo_flags[] that are supported.
 */
static const uint8_t nfifo_flags_sz[] = {12, 14, 14, 14, 14};

static const uint32_t nfifo_pad_flags[][2] = {
	{ BM, NFIFOENTRY_BM },
	{ PS, NFIFOENTRY_PS },
	{ PR, NFIFOENTRY_PR }
};

/*
 * Allowed NFIFO LOAD pad flags for each SEC Era.
 * Values represent the number of entries from nfifo_pad_flags[] that are
 * supported.
 */
static const uint8_t nfifo_pad_flags_sz[] = {2, 2, 2, 2, 3};

static inline uint32_t nfifo_load(struct program *program, uint32_t src,
				  int type_src, uint32_t data, int type_data,
				  uint32_t length, uint32_t flags)
{
	uint32_t opcode = 0, val;
	int32_t ret = 0;
	uint32_t load_cmd = CMD_LOAD | LDST_IMM | LDST_CLASS_IND_CCB \
				     | LDST_SRCDST_WORD_INFO_FIFO;

	/* write source field */
	ret = map_opcode(src, nfifo_src, nfifo_src_sz[rta_sec_era], &val);
	if (ret == -1) {
		pr_debug("NFIFO: Invalid SRC. SEC PC: %d; Instr: %d\n",
				program->current_pc,
				program->current_instraction);
		goto err;
	}
	opcode |= val;

	/* write type field */
	ret = map_opcode(data, nfifo_data, ARRAY_SIZE(nfifo_data), &val);
	if (ret == -1) {
		pr_debug("NFIFO: Invalid data. SEC PC: %d; Instr: %d\n",
				program->current_pc,
				program->current_instraction);
		goto err;
	}
	opcode |= val;

	/* write DL field */
	if (!(flags & EXT)) {
		opcode |= length & NFIFOENTRY_DLEN_MASK;
		load_cmd |= 4;
	} else {
		load_cmd |= 8;
	}

	/* write flags */
	map_flags(flags, nfifo_flags, nfifo_flags_sz[rta_sec_era], &opcode);

	/* in case of padding, check the destination */
	if (src == _PAD)
		map_flags(flags, nfifo_pad_flags,
			  nfifo_pad_flags_sz[rta_sec_era], &opcode);

	/* write LOAD command first */
	program->buffer[program->current_pc] = load_cmd;
	program->current_pc++;
	program->buffer[program->current_pc] = opcode;
	program->current_pc++;

	if (flags & EXT) {
		program->buffer[program->current_pc] =
		    length & NFIFOENTRY_DLEN_MASK;
		program->current_pc++;
	}
	program->current_instraction++;

	return program->current_pc;

 err:
	program->first_error_pc = program->current_pc;
	program->current_instraction++;
	return program->current_pc;
}

#endif /* __RTA_NFIFO_CMD_H__ */
