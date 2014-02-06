/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#ifndef __RTA_JUMP_CMD_H__
#define __RTA_JUMP_CMD_H__

extern enum rta_sec_era rta_sec_era;

static const uint32_t jump_test_cond[][2] = {
	{ NIFP,     JUMP_COND_NIFP },
	{ NIP,      JUMP_COND_NIP },
	{ NOP,      JUMP_COND_NOP },
	{ NCP,      JUMP_COND_NCP },
	{ CALM,     JUMP_COND_CALM },
	{ SELF,     JUMP_COND_SELF },
	{ SHRD,     JUMP_COND_SHRD },
	{ JQP,      JUMP_COND_JQP },
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

static inline unsigned rta_jump(struct program *program, uint64_t address,
				int address_type, uint32_t jump_type,
				uint32_t test_type, uint32_t test_condition)
{
	uint32_t opcode = CMD_JUMP;
	unsigned start_pc = program->current_pc;

	if (((jump_type == GOSUB) || (jump_type == RETURN)) &&
	    (rta_sec_era < RTA_SEC_ERA_4)) {
		pr_debug("JUMP: Jump type not supported by SEC Era %d\n",
			 USER_SEC_ERA(rta_sec_era));
		goto err;
	}

	switch (jump_type) {
	case (LOCAL_JUMP):
		opcode |= JUMP_TYPE_LOCAL;
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
	case (GOSUB):
		opcode |= JUMP_TYPE_GOSUB;
		break;
	case (RETURN):
		opcode |= JUMP_TYPE_RETURN;
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
	__rta_map_flags(test_condition, jump_test_cond,
			ARRAY_SIZE(jump_test_cond), &opcode);

	/* write local offset field for local jumps and user-defined halt */
	if ((jump_type == LOCAL_JUMP) || (jump_type == GOSUB) ||
	    (jump_type == HALT_STATUS))
		opcode |= (uint32_t)(address & JUMP_OFFSET_MASK);

	__rta_out32(program, opcode);
	program->current_instruction++;

	if (jump_type == FAR_JUMP)
		__rta_out64(program, program->ps, address);

	return start_pc;

 err:
	program->first_error_pc = start_pc;
	program->current_instruction++;
	return start_pc;
}

#endif /* __RTA_JUMP_CMD_H__ */
