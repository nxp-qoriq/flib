#ifndef __RTA_FIFO_LOAD_STORE_CMD_H__
#define __RTA_FIFO_LOAD_STORE_CMD_H__

static const uint32_t fifo_load_table[23][2] = {
	{ _PKA0,        FIFOLD_CLASS_CLASS1 | FIFOLD_TYPE_PK_A0 },
	{ _PKA1,        FIFOLD_CLASS_CLASS1 | FIFOLD_TYPE_PK_A1 },
	{ _PKA2,        FIFOLD_CLASS_CLASS1 | FIFOLD_TYPE_PK_A2 },
	{ _PKA3,        FIFOLD_CLASS_CLASS1 | FIFOLD_TYPE_PK_A3 },
	{ _PKB0,        FIFOLD_CLASS_CLASS1 | FIFOLD_TYPE_PK_B0 },
	{ _PKB1,        FIFOLD_CLASS_CLASS1 | FIFOLD_TYPE_PK_B1 },
	{ _PKB2,        FIFOLD_CLASS_CLASS1 | FIFOLD_TYPE_PK_B2 },
	{ _PKB3,        FIFOLD_CLASS_CLASS1 | FIFOLD_TYPE_PK_B3 },
	{ _PKA,         FIFOLD_CLASS_CLASS1 | FIFOLD_TYPE_PK_A },
	{ _PKB,         FIFOLD_CLASS_CLASS1 | FIFOLD_TYPE_PK_B },
	{ _PKN,         FIFOLD_CLASS_CLASS1 | FIFOLD_TYPE_PK_N },
	{ _IFIFO,       FIFOLD_CLASS_CLASS1 | FIFOLD_TYPE_DFIFO },
	{ _SKIP,        FIFOLD_CLASS_SKIP },
	{ _MSG1,        FIFOLD_CLASS_CLASS1 | FIFOLD_TYPE_MSG },
	{ _MSG2,        FIFOLD_CLASS_CLASS2 | FIFOLD_TYPE_MSG },
	{ _MSGOUTSNOOP, FIFOLD_CLASS_BOTH | FIFOLD_TYPE_MSG1OUT2 },
	{ _MSGINSNOOP,  FIFOLD_CLASS_BOTH | FIFOLD_TYPE_MSG },
	{ _IV1,         FIFOLD_CLASS_CLASS1 | FIFOLD_TYPE_IV },
	{ _IV2,         FIFOLD_CLASS_CLASS2 | FIFOLD_TYPE_IV },
	{ _AAD1,        FIFOLD_CLASS_CLASS1 | FIFOLD_TYPE_AAD },
	{ _ICV1,        FIFOLD_CLASS_CLASS1 | FIFOLD_TYPE_ICV },
	{ _ICV2,        FIFOLD_CLASS_CLASS2 | FIFOLD_TYPE_ICV },
	{ _BIT_DATA,    FIFOLD_TYPE_BITDATA }
};

static inline uint32_t fifo_load(struct program *program, uint32_t src,
				 uint32_t type_src, uint64_t loc,
				 uint32_t type_loc, uint32_t length,
				 uint32_t flags)
{
	uint32_t opcode = 0;
	uint32_t is_seq_cmd = 0, ext_length = 0, val = 0;
	uint8_t *tmp;
	int8_t ret = 0, i;

	/* write command type field */
	if (flags & SEQ) {
		opcode = CMD_SEQ_FIFO_LOAD;
		is_seq_cmd = 1;
	} else
		opcode = CMD_FIFO_LOAD;

	if (type_loc == IMM_DATA)
		flags |= IMMED;

	/* Parameters checking */
	if (is_seq_cmd) {
		if (flags & IMMED) {
			pr_debug("SEQ FIFO LOAD: Invalid command\n");
			goto err;
		}
		if ((flags & VLF) && ((flags & EXT) || (length >> 16))) {
			pr_debug("SEQ FIFO LOAD: Invalid usage of VLF\n");
			goto err;
		}
	} else {
		if (src == _SKIP) {
			pr_debug("FIFO LOAD: Invalid src\n");
			goto err;
		}
		if ((flags & IMMED) && (flags & SGF)) {
			pr_debug("FIFO LOAD: Invalid usage of SGF and IMM\n");
			goto err;
		}
		if ((flags & IMMED) && ((flags & EXT) || (length >> 16))) {
			pr_debug("FIFO LOAD: Invalid usage of EXT and IMM\n");
			goto err;
		}
	}

	/* write input data type field */
	ret = map_opcode(src, fifo_load_table, sizeof(fifo_load_table), &val);
	if (ret == -1) {
		pr_debug("FIFO LOAD: Source value is not supported. "
				"SEC Program Line: %d\n", program->current_pc);
		goto err;
	}
	opcode |= val;

	if (flags & CLASS1)
		opcode |= FIFOLD_CLASS_CLASS1;
	if (flags & CLASS2)
		opcode |= FIFOLD_CLASS_CLASS2;
	if (flags & BOTH)
		opcode |= FIFOLD_CLASS_BOTH;

	/* write fields: SGF|VLF, IMM, [LC1, LC2, F1] */
	if (flags & FLUSH1)
		opcode |= FIFOLD_TYPE_FLUSH1;
	if (flags & LAST1)
		opcode |= FIFOLD_TYPE_LAST1;
	if (flags & LAST2)
		opcode |= FIFOLD_TYPE_LAST2;
	if (flags & SGF)
		opcode |= FIFOLDST_SGF;
	if (flags & VLF)
		opcode |= FIFOLDST_VLF;
	if (flags & IMMED)
		opcode |= FIFOLD_IMM;

	/*
	 * Verify if extended length is required. In case of BITDATA, calculate
	 * number of full bytes and additional valid bits.
	 */
	if ((flags & EXT) || (length >> 16)) {
		opcode |= FIFOLDST_EXT;
		if (src == _BIT_DATA) {
			ext_length = (length / 8);
			length = (length % 8);
		} else {
			ext_length = length;
			length = 0;
		}
	}
	opcode |= (uint16_t) length;

	program->buffer[program->current_pc] = opcode;
	program->current_pc++;
	program->current_instraction++;

	/* write pointer or immidiate data field */
	if (opcode & FIFOLD_IMM) {
		tmp = (uint8_t *) &program->buffer[program->current_pc];
		for (i = 0; i < length; i++)
			*tmp++ = ((uint8_t *) &loc)[i];
		program->current_pc += ((length + 3) / 4);
	} else if ((opcode & CMD_MASK) == CMD_FIFO_LOAD) {
		if (program->ps == 1) {
			program->buffer[program->current_pc++] = high_32b(loc);
			program->buffer[program->current_pc++] = low_32b(loc);
		} else {
			program->buffer[program->current_pc++] = low_32b(loc);
		}
	}

	/* write extended length field */
	if (opcode & FIFOLDST_EXT) {
		program->buffer[program->current_pc] = ext_length;
		program->current_pc++;
	}
	return program->current_pc;

 err:
	program->first_error_pc = program->current_pc;
	program->current_instraction++;
	program->current_pc++;	/* skipping one word regardless immediates */
	return program->current_pc;
}

static const uint32_t fifo_store_table[20][2] = {
	{ _PKA0,      FIFOST_TYPE_PKHA_A0 },
	{ _PKA1,      FIFOST_TYPE_PKHA_A1 },
	{ _PKA2,      FIFOST_TYPE_PKHA_A2 },
	{ _PKA3,      FIFOST_TYPE_PKHA_A3 },
	{ _PKB0,      FIFOST_TYPE_PKHA_B0 },
	{ _PKB1,      FIFOST_TYPE_PKHA_B1 },
	{ _PKB2,      FIFOST_TYPE_PKHA_B2 },
	{ _PKB3,      FIFOST_TYPE_PKHA_B3 },
	{ _PKA,       FIFOST_TYPE_PKHA_A },
	{ _PKB,       FIFOST_TYPE_PKHA_B },
	{ _PKN,       FIFOST_TYPE_PKHA_N },
	{ _PKE,       FIFOST_TYPE_PKHA_E_JKEK },
	{ _RNG,       FIFOST_TYPE_RNGSTORE },
	{ _RNGOFIFO,  FIFOST_TYPE_RNGFIFO },
	{ _AFHA_SBOX, FIFOST_TYPE_AF_SBOX_JKEK },
	{ _MDHA_SPLIT_KEY, FIFOST_CLASS_CLASS2KEY | FIFOST_TYPE_SPLIT_KEK },
	{ _MSG,       FIFOST_TYPE_MESSAGE_DATA },
	{ _KEY1,      FIFOST_CLASS_CLASS1KEY | FIFOST_TYPE_KEY_KEK },
	{ _KEY2,      FIFOST_CLASS_CLASS2KEY | FIFOST_TYPE_KEY_KEK },
	{ _SKIP,      FIFOST_TYPE_SKIP }
};

static inline uint32_t fifo_store(struct program *program, uint32_t src,
				  uint32_t type_src, uint32_t encrypt_flags,
				  uint64_t dst, uint32_t length, uint32_t flags)
{
	uint32_t opcode = 0;
	uint32_t is_seq_cmd = 0, val = 0;
	int8_t ret = 0;

	/* write command type field */
	if (flags & SEQ) {
		opcode = CMD_SEQ_FIFO_STORE;
		is_seq_cmd = 1;
	} else
		opcode = CMD_FIFO_STORE;

	/* Parameter checking */
	if (is_seq_cmd) {
		if ((flags & VLF) && ((length >> 16) || (flags & EXT))) {
			pr_debug("SEQ FIFO STORE: Invalid usage of VLF\n");
			goto err;
		}
		if (dst) {
			pr_debug("SEQ FIFO STORE: Invalid command\n");
			goto err;
		}
	} else {
		if ((src == _RNGOFIFO) && ((dst) || (flags & EXT))) {
			pr_debug("FIFO STORE: Invalid destination\n");
			goto err;
		}

	}

	/* write output data type field */
	ret = map_opcode(src, fifo_store_table, sizeof(fifo_store_table), &val);
	if (ret == -1) {
		pr_debug("FIFO STORE: Source type not supported. "
				"SEC Program Line: %d\n", program->current_pc);
		goto err;
	}
	opcode |= val;

	if (encrypt_flags & TK)
		opcode |= (0x1 << FIFOST_TYPE_SHIFT);
	if (encrypt_flags & EKT) {
		opcode |= (0x10 << FIFOST_TYPE_SHIFT);
		opcode &= 0xFFDFFFFF;
	}
	if (encrypt_flags & EKT)
		opcode &= 0xFFDFFFFF;

	/* write flags fields: CONT, VLF|SGF */
	if (flags & CONT)
		opcode |= FIFOST_CONT;
	if ((flags & VLF) && (is_seq_cmd))
		opcode |= FIFOLDST_VLF;
	if ((flags & SGF) && (!is_seq_cmd))
		opcode |= FIFOLDST_SGF;

	/* Verify if extended length is required */
	if ((length >> 16) || (flags & EXT))
		opcode |= FIFOLDST_EXT;
	else
		opcode |= (uint16_t) length;

	program->buffer[program->current_pc] = opcode;
	program->current_pc++;
	program->current_instraction++;

	/* write pointer field */
	if ((!is_seq_cmd) && (dst)) {
		if (program->ps == 1) {
			program->buffer[program->current_pc++] = high_32b(dst);
			program->buffer[program->current_pc++] = low_32b(dst);
		} else {
			program->buffer[program->current_pc++] = low_32b(dst);
		}
	}

	/* write extended length field */
	if (opcode & FIFOLDST_EXT) {
		program->buffer[program->current_pc] = length;
		program->current_pc++;
	}
	return program->current_pc;

 err:
	program->first_error_pc = program->current_pc;
	program->current_pc++;
	program->current_instraction++;
	return program->current_pc;
}

#endif /* __RTA_FIFO_LOAD_STORE_CMD_H__ */
