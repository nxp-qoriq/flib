#ifndef __RTA_MATH_CMD_H__
#define __RTA_MATH_CMD_H__

static const uint32_t math_op1[12][2] = {
	{ _MATH0,     MATH_SRC0_REG0 },
	{ _MATH1,     MATH_SRC0_REG1 },
	{ _MATH2,     MATH_SRC0_REG2 },
	{ _MATH3,     MATH_SRC0_REG3 },
	{ _DPOVRD,    MATH_SRC0_PROT },
	{ _SEQINSZ,   MATH_SRC0_SEQINLEN },
	{ _SEQOUTSZ,  MATH_SRC0_SEQOUTLEN },
	{ _VSEQINSZ,  MATH_SRC0_VARSEQINLEN },
	{ _VSEQOUTSZ, MATH_SRC0_VARSEQOUTLEN },
	{ _ZERO,      MATH_SRC0_ZERO },
	{ _ONE,       MATH_SRC0_ONE },
	{ _NONE,      0 } /* dummy value */
};
static const uint32_t math_op2[13][2] = {
	{ _MATH0,     MATH_SRC1_REG0 },
	{ _MATH1,     MATH_SRC1_REG1 },
	{ _MATH2,     MATH_SRC1_REG2 },
	{ _MATH3,     MATH_SRC1_REG3 },
	{ _DPOVRD,    MATH_SRC1_PROT },
	{ _VSEQINSZ,  MATH_SRC1_VARSEQINLEN },
	{ _VSEQOUTSZ, MATH_SRC1_VARSEQOUTLEN },
	{ _ABD,       MATH_SRC1_INFIFO },
	{ _OFIFO,     MATH_SRC1_OUTFIFO },
	{ _JOBSRC,    MATH_SRC1_JOBSOURCE },
	{ _ZERO,      MATH_SRC1_ZERO },
	{ _ONE,       MATH_SRC1_ONE },
	{ _NONE,      0 } /* dummy value */
};

static const uint32_t math_result[10][2] = {
	{ _MATH0,     MATH_DEST_REG0 },
	{ _MATH1,     MATH_DEST_REG1 },
	{ _MATH2,     MATH_DEST_REG2 },
	{ _MATH3,     MATH_DEST_REG3 },
	{ _DPOVRD,    MATH_DEST_PROT },
	{ _SEQINSZ,   MATH_DEST_SEQINLEN },
	{ _SEQOUTSZ,  MATH_DEST_SEQOUTLEN },
	{ _VSEQINSZ,  MATH_DEST_VARSEQINLEN },
	{ _VSEQOUTSZ, MATH_DEST_VARSEQOUTLEN },
	{ _NONE,      MATH_DEST_NONE }
};

static inline uint32_t math(struct program *program, uint64_t operand1,
		int type_op1, uint32_t op, uint64_t operand2, int type_op2,
		uint32_t result, int type_res, int length, uint32_t options)
{
	uint32_t opcode = CMD_MATH;
	uint32_t val = 0;
	int8_t ret = 0;


	/*
	 * SHLD operation is different from others and we
	 * assume that we can have _NONE as first operand
	 * or _SEQINSZ as second operand
	 */
	if ((op != SHLD) && ((operand1 == _NONE) || (operand2 == _SEQINSZ))) {
		pr_debug("MATH: Invalid operand. SEC PC: %d; "
				"Instr: %d\n", program->current_pc,
				program->current_instraction);
		goto err;
	}

	/*
	 * We first check if it is unary operation. In that
	 * case second operand must be _NONE
	 */
	if (((op == ZBYTE) || (op == BSWAP)) && (operand2 != _NONE)) {
		pr_debug("MATH: Invalid operand2. SEC PC: %d; "
				"Instr: %d\n", program->current_pc,
				program->current_instraction);
		goto err;
	}

	/* Write first operand field */
	if (type_op1 == IMM_DATA)
		opcode |= MATH_SRC0_IMM;
	else {
		ret = map_opcode(operand1, math_op1, sizeof(math_op1), &val);
		if (ret == -1) {
			pr_debug("MATH: operand1 not supported. SEC PC: %d; "
					"Instr: %d\n", program->current_pc,
					program->current_instraction);
			goto err;
		}
		opcode |= val;
	}

	/* Write second operand field */
	if (type_op2 == IMM_DATA)
		opcode |= MATH_SRC1_IMM;
	else {
		ret = map_opcode(operand2, math_op2, sizeof(math_op2), &val);
		if (ret == -1) {
			pr_debug("MATH: operand2 not supported. SEC PC: %d; "
					"Instr: %d\n", program->current_pc,
					program->current_instraction);
			goto err;
		}
		opcode |= val;
	}

	/* Write result field */
	ret = map_opcode(result, math_result, sizeof(math_result), &val);
	if (ret == -1) {
		pr_debug("MATH: result not supported. SEC PC: %d; "
				"Instr: %d\n", program->current_pc,
				program->current_instraction);
		goto err;
	}
	opcode |= val;

	/*
	 * as we encode operations with their "real" values, we do not
	 * to translate but we do need to validate the value
	 */
	switch (op) {
	/*Binary operators */
	case (ADD):
	case (ADDC):
	case (SUB):
	case (SUBB):
	case (OR):
	case (AND):
	case (XOR):
	case (LSHIFT):
	case (RSHIFT):
	case (SHLD):
	/* Unary operators */
	case (ZBYTE):
	case (BSWAP):
		opcode |= op;
		break;
	default:
		pr_debug("MATH: operator is not supported. SEC PC: %d; "
				"Instr: %d\n", program->current_pc,
				program->current_instraction);
		goto err;
	}

	opcode |= options;

	/* Verify length */
	switch (length) {
	case (1):
		opcode |= MATH_LEN_1BYTE;
		break;
	case (2):
		opcode |= MATH_LEN_2BYTE;
		break;
	case (4):
		opcode |= MATH_LEN_4BYTE;
		break;
	case (8):
		opcode |= MATH_LEN_8BYTE;
		break;
	default:
		pr_debug("MATH: length is not supported. SEC PC: %d; "
				"Instr: %d\n", program->current_pc,
		     program->current_instraction);
		goto err;
	}

	program->buffer[program->current_pc] = opcode;
	program->current_pc++;
	program->current_instraction++;

	/* Write immediate value */
	if (type_op1 == IMM_DATA && type_op2 != IMM_DATA) {

		if ((length > 4) && !(options & IFB)) {
			*(uint64_t *) &program->buffer[program->current_pc] =
			    (operand1 >> 32) | (operand1 << 32);
			program->current_pc += 2;

		} else {
			program->buffer[program->current_pc] =
			    (uint32_t) operand1;
			program->current_pc++;
		}

	} else if (type_op2 == IMM_DATA && type_op1 != IMM_DATA) {

		if ((length > 4) && !(options & IFB)) {
			*(uint64_t *) &program->buffer[program->current_pc] =
			    (operand2 >> 32) | (operand2 << 32);
			program->current_pc += 2;

		} else {
			program->buffer[program->current_pc] =
			    (uint32_t) operand2;
			program->current_pc++;
		}

	} else if (type_op1 == IMM_DATA && type_op2 == IMM_DATA) {
		*(uint32_t *) &program->buffer[program->current_pc++] =
		    (uint32_t) operand1;
		*(uint32_t *) &program->buffer[program->current_pc++] =
		    (uint32_t) operand2;
	}

	return program->current_pc;
 err:
	program->first_error_pc = program->current_pc;
	program->current_instraction++;
	program->current_pc++;	/* skipping one word regardless immediates */
	return program->current_pc;
}

#endif /* __RTA_MATH_CMD_H__ */
