#ifndef __RTA_KEY_CMD_H__
#define __RTA_KEY_CMD_H__

static inline uint32_t key(struct program *program, uint32_t key_dst,
		uint32_t key_type, uint32_t encrypt_flags,
		uintptr_t dst, uint32_t dst_type, uint32_t length,
		uint32_t flags)
{
	uint32_t opcode = 0, is_seq_cmd = 0;
	uint8_t *tmp, i;

	/* write cmd type */
	if (flags & SEQ) {
		opcode = CMD_SEQ_KEY;
		is_seq_cmd = 1;
	} else
		opcode = CMD_KEY;

	if (dst_type == IMM_DATA)
		flags |= IMMED;

	/* check parameters */
	if ((flags & IMMED) && (is_seq_cmd)) {
		pr_debug("KEY: Invalid flag. SEC PC: %d; Instr: %d\n",
		     program->current_pc, program->current_instraction);
		goto err;
	}
	if ((flags & SGF) && ((flags & IMMED) || (is_seq_cmd))) {
		pr_debug("KEY: Invalid flag. SEC PC: %d; Instr: %d\n",
		     program->current_pc, program->current_instraction);
		goto err;
	}
	if ((key_dst == _AFHA_SBOX) && (flags & IMMED)) {
		pr_debug("KEY: Invalid flag. SEC PC: %d; Instr: %d\n",
		     program->current_pc, program->current_instraction);
		goto err;
	}
	if ((key_dst == _AFHA_SBOX) && (length != 258)) {
		pr_debug("KEY: Invalid flag. SEC PC: %d; Instr: %d\n",
		     program->current_pc, program->current_instraction);
		goto err;
	}

	/* write key destination and class fields */
	switch (key_dst) {
	case (_KEY1):
		opcode |= KEY_DEST_CLASS1;
		break;
	case (_KEY2):
		opcode |= KEY_DEST_CLASS2;
		break;
	case (_PKE):
		opcode |= KEY_DEST_CLASS1 | KEY_DEST_PKHA_E;
		break;
	case (_AFHA_SBOX):
		opcode |= KEY_DEST_CLASS1 | KEY_DEST_AFHA_SBOX;
		break;
	case (_MDHA_SPLIT_KEY):
		opcode |= KEY_DEST_CLASS2 | KEY_DEST_MDHA_SPLIT;
		break;
	default:
		pr_debug("KEY: Invalid destination. SEC PC: %d; Instr: %d\n",
		     program->current_pc, program->current_instraction);
		goto err;
		break;
	}

	/* write key command specific flags */
	if (encrypt_flags & ENC)
		opcode |= KEY_ENC;
	if (encrypt_flags & EKT)
		opcode |= KEY_EKT;
	if (encrypt_flags & TK)
		opcode |= KEY_TK;
	if (encrypt_flags & NWB)
		opcode |= KEY_NWB;

	/* write general command flags */
	if (flags & IMMED)
		opcode |= KEY_IMM;
	if (flags & SGF)
		opcode |= KEY_SGF;

	/* write length */
	length &= 0x000003FF;
	opcode |= length;

	program->buffer[program->current_pc] = opcode;
	program->current_pc++;
	program->current_instraction++;

	if (opcode & KEY_IMM) {
		tmp = (uint8_t *) &program->buffer[program->current_pc];
		for (i = 0; i < length; i++)
			*tmp++ = ((uint8_t *) dst)[i];
		program->current_pc += ((length + 3) / 4);
	} else {
		if (program->ps == 1) {
			program->buffer[program->current_pc++] = high_32b(dst);
			program->buffer[program->current_pc++] = low_32b(dst);
		} else {
			program->buffer[program->current_pc++] = low_32b(dst);
		}
	}

	return program->current_pc;
 err:
	program->first_error_pc = program->current_pc;
	program->current_instraction++;
	program->current_pc++;
	return program->current_pc;
}

#endif /* __RTA_KEY_CMD_H__ */
