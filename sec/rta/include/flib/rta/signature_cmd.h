#ifndef __RTA_SIGNATURE_CMD_H__
#define __RTA_SIGNATURE_CMD_H__

static inline uint32_t signature(struct program *program, uint32_t sign_type)
{
	uint32_t opcode = CMD_SIGNATURE;

	switch (sign_type) {
	case (SIGN_TYPE_FINAL):
	case (SIGN_TYPE_FINAL_RESTORE):
	case (SIGN_TYPE_FINAL_NONZERO):
	case (SIGN_TYPE_IMM_2):
	case (SIGN_TYPE_IMM_3):
	case (SIGN_TYPE_IMM_4):
		opcode |= sign_type;
		break;
	default:
		pr_debug("SIGNATURE Command: Invalid type selection\n");
		goto err;
	}
	program->buffer[program->current_pc++] = opcode;
	program->current_instraction++;

	return program->current_pc;
 err:
	program->first_error_pc = program->current_pc;
	program->current_pc++;
	program->current_instraction++;
	return program->current_pc;
}

#endif /* __RTA_SIGNATURE_CMD_H__ */
