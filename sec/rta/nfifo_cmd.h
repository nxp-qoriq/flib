#ifndef __RTA_NFIFO_CMD_H__
#define __RTA_NFIFO_CMD_H__

static const uint32_t nfifo_src[5][2] = {
	{ _IFIFO,       IFIFOENTRY_STYPE_DFIFO },
	{ _OFIFO,       IFIFOENTRY_STYPE_OFIFO },
	{ _PAD,         IFIFOENTRY_STYPE_PAD },
	{ _MSGOUTSNOOP, IFIFOENTRY_STYPE_SNOOP | IFIFOENTRY_DEST_BOTH },
	{ _ALTSOURCE,   IFIFOENTRY_STYPE_ALTSOURCE }
};

static const uint32_t nfifo_data[27][2] = {
	{ _MSG,   IFIFOENTRY_DTYPE_MSG },
	{ _MSG1,  IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_MSG },
	{ _MSG2,  IFIFOENTRY_DEST_CLASS2 | IFIFOENTRY_DTYPE_MSG },
	{ _IV1,   IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_IV },
	{ _IV2,   IFIFOENTRY_DEST_CLASS2 | IFIFOENTRY_DTYPE_IV },
	{ _ICV1,  IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_ICV },
	{ _ICV2,  IFIFOENTRY_DEST_CLASS2 | IFIFOENTRY_DTYPE_ICV },
	{ _SAD1,  IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_SAD },
	{ _AAD1,  IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_AAD },
	{ _AAD2,  IFIFOENTRY_DEST_CLASS2 | IFIFOENTRY_DTYPE_AAD },
	{ _AFHA_SBOX, IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_SBOX },
	{ _SKIP,  IFIFOENTRY_DTYPE_SKIP },
	{ _PKE,   IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_PK_E },
	{ _PKN,   IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_PK_N },
	{ _PKA,   IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_PK_A },
	{ _PKA0,  IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_PK_A0 },
	{ _PKA1,  IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_PK_A1 },
	{ _PKA2,  IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_PK_A2 },
	{ _PKA3,  IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_PK_A3 },
	{ _PKB,   IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_PK_B },
	{ _PKB0,  IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_PK_B0 },
	{ _PKB1,  IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_PK_B1 },
	{ _PKB2,  IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_PK_B2 },
	{ _PKB3,  IFIFOENTRY_DEST_CLASS1 | IFIFOENTRY_DTYPE_PK_B3 },
	{ _AB1,   IFIFOENTRY_DEST_CLASS1 },
	{ _AB2,   IFIFOENTRY_DEST_CLASS2 },
	{ _ABD,   IFIFOENTRY_DEST_DECO }
};

static const uint32_t nfifo_flags[14][2] = {
	{ LAST1,         IFIFOENTRY_LC1 },
	{ LAST2,         IFIFOENTRY_LC2 },
	{ FLUSH1,        IFIFOENTRY_FC1 },
	{ FLUSH2,        IFIFOENTRY_FC2 },
	{ OC,            IFIFOENTRY_OC },
	{ BP,            IFIFOENTRY_BND },
	{ PAD_ZERO,      IFIFOENTRY_PTYPE_ZEROS },
	{ PAD_NONZERO,   IFIFOENTRY_PTYPE_RND_NOZEROS },
	{ PAD_INCREMENT, IFIFOENTRY_PTYPE_INCREMENT },
	{ PAD_RANDOM,    IFIFOENTRY_PTYPE_RND },
	{ PAD_ZERO_N1,   IFIFOENTRY_PTYPE_ZEROS_NZ },
	{ PAD_NONZERO_0, IFIFOENTRY_PTYPE_RND_NZ_LZ },
	{ PAD_N1,        IFIFOENTRY_PTYPE_N },
	{ PAD_NONZERO_N, IFIFOENTRY_PTYPE_RND_NZ_N }
};

static const uint32_t nfifo_pad_flags[3][2] = {
	{ BM, IFIFOENTRY_BM },
	{ PR, IFIFOENTRY_OC },
	{ PS, IFIFOENTRY_PS }
};

static inline uint32_t nfifo_load(struct program *program, uint32_t src,
				  int type_src, uint32_t data, int type_data,
				  uint32_t length, uint32_t flags)
{
	uint32_t opcode = 0, val;
	int32_t ret = 0;
	uint32_t load_cmd = CMD_LOAD | LDST_IMM | LDST_CLASS_IND_CCB \
				     | LDST_SRCDST_WORD_INFO_FIFO;

	/* write source field */
	ret = map_opcode(src, nfifo_src,
			 sizeof(nfifo_src)/sizeof(nfifo_src[0]), &val);
	if (ret == -1) {
		pr_debug("NFIFO: Invalid SRC. SEC PC: %d; Instr: %d\n",
				program->current_pc,
				program->current_instraction);
		goto err;
	}
	opcode |= val;

	/* write type field */
	ret = map_opcode(data, nfifo_data,
			 sizeof(nfifo_data)/sizeof(nfifo_data[0]), &val);
	if (ret == -1) {
		pr_debug("NFIFO: Invalid data. SEC PC: %d; Instr: %d\n",
				program->current_pc,
				program->current_instraction);
		goto err;
	}
	opcode |= val;

	/* write DL field */
	if (!(flags & EXT)) {
		opcode |= length & IFIFOENTRY_DLEN_MASK;
		load_cmd |= 4;
	} else {
		load_cmd |= 8;
	}

	/* write flags */
	map_flags(flags, nfifo_flags,
			sizeof(nfifo_flags)/sizeof(nfifo_flags[0]), &opcode);

	/* in case of padding, check the destination */
	if (src == _PAD)
		map_flags(flags, nfifo_pad_flags,
			  sizeof(nfifo_pad_flags)/sizeof(nfifo_pad_flags[0]),
			  &opcode);

	/* write LOAD command first */
	program->buffer[program->current_pc++] = load_cmd;
	program->buffer[program->current_pc++] = opcode;

	if (flags & EXT)
		program->buffer[program->current_pc++] =
		    length & IFIFOENTRY_DLEN_MASK;
	program->current_instraction++;

	return program->current_pc;

 err:
	program->first_error_pc = program->current_pc;
	program->current_instraction++;
	program->current_pc++;
	return program->current_pc;
}

#endif /* __RTA_NFIFO_CMD_H__ */
