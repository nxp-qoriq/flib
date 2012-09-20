#ifndef __RTA_JUMP_CMD_H__
#define __RTA_JUMP_CMD_H__

static const uint32_t jump_test_cond[18][2] = {
	{ NIFP,     JUMP_JSL | JUMP_COND_NIFP },
	{ NIP,      JUMP_JSL | JUMP_COND_NIP },
	{ NOP,      JUMP_JSL | JUMP_COND_NOP },
	{ NCP,      JUMP_JSL | JUMP_COND_NCP },
	{ CALM,     JUMP_JSL | JUMP_COND_CALM },
	{ SELF,     JUMP_JSL | JUMP_COND_SELF },
	{ SHRD,     JUMP_JSL | JUMP_COND_SHRD },
	{ JQP,      JUMP_JSL | JUMP_COND_JQP },
	{ MATH_Z,   JUMP_COND_MATH_Z },
	{ MATH_N,   JUMP_COND_MATH_N },
	{ MATH_NV,  JUMP_COND_MATH_NV },
	{ MATH_C,   JUMP_COND_MATH_C },
	{ PK_0,     JUMP_COND_PK_0 },
	{ PK_GCD_1, JUMP_COND_PK_GCD_1 },
	{ PK_PRIME, JUMP_COND_PK_PRIME },
	{ CLASS1,   JUMP_CLASS_CLASS1 },
	{ CLASS2,   JUMP_CLASS_CLASS2 },
	{ BOTH,     JUMP_CLASS_BOTH }
};

static inline uint32_t jump(struct program *program, int64_t address,
		     uint32_t address_type, uint32_t jump_type,
		     uint32_t test_type, uint32_t test_condition)
{
	uint32_t opcode = CMD_JUMP;

	switch (jump_type) {
	case (LOCAL_JUMP):
		opcode |= JUMP_TYPE_LOCAL;
		break;
	case (GOSUB):
		opcode |= JUMP_TYPE_GOSUB;
		break;
	case (RETURN):
		opcode |= JUMP_TYPE_RETURN;
		break;
	case (HALT):
		opcode |= JUMP_TYPE_HALT;
		break;
	case (HALT_STATUS):
		opcode |= JUMP_TYPE_HALT_USER;
		break;
	case (FAR_JUMP):
		opcode |= JUMP_TYPE_NONLOCAL;
		break;

	default:
		pr_debug("JUMP: Invalid jump type. SEC Program Line: %d\n",
			program->current_pc);
		goto err;
	}

	switch (test_type) {
	case (ALL_TRUE):
		opcode |= JUMP_TEST_ALL;
		break;
	case (ALL_FALSE):
		opcode |= JUMP_TEST_INVALL;
		break;
	case (ANY_TRUE):
		opcode |= JUMP_TEST_ANY;
		break;
	case (ANY_FALSE):
		opcode |= JUMP_TEST_INVANY;
		break;
	default:
		pr_debug
		    ("JUMP: test type not supported. SEC Program Line: %d\n",
		     program->current_pc);
		goto err;
	}

	/* write test condition field */
	map_flags(test_condition, jump_test_cond,
			sizeof(jump_test_cond)/sizeof(jump_test_cond[0]),
			&opcode);

	/* write local offset field for local jumps */
	if (jump_type != FAR_JUMP)
		opcode |= address & JUMP_OFFSET_MASK;

	program->buffer[program->current_pc] = opcode;
	program->current_instraction++;

	if (jump_type == FAR_JUMP) {
		if (program->ps == 1) {
			program->buffer[++program->current_pc] =
					high_32b(address);
			program->buffer[++program->current_pc] =
					low_32b(address);

		} else {
			program->buffer[++program->current_pc] =
					low_32b(address);
		}
	}
	return program->current_pc++;
 err:
	program->first_error_pc = program->current_pc;
	program->current_instraction++;
	return program->current_pc++;
}

#endif /* __RTA_JUMP_CMD_H__ */
