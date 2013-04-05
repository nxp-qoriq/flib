#ifndef __RTA_SEQ_IN_OUT_PTR_CMD_H__
#define __RTA_SEQ_IN_OUT_PTR_CMD_H__

extern enum rta_sec_era rta_sec_era;

static inline unsigned seq_in_ptr(struct program *program, uintptr_t src,
				  uint32_t length, uint32_t flags)
{
	uint32_t opcode = CMD_SEQ_IN_PTR;

	/* Parameters checking */
	if ((flags & RTO) && (flags & PRE)) {
		pr_debug("SEQ IN PTR: Invalid usage of RTO and PRE flags\n");
		goto err;
	}
	if ((rta_sec_era == RTA_SEC_ERA_1) && (flags & RJD)) {
		pr_debug("SEQ IN PTR: Replacement Job Descriptors (RJD) not "
			 "supported\n");
		goto err;
	}
	if ((flags & INL) && (flags & RJD)) {
		pr_debug("SEQ IN PTR: Invalid usage of INL and RJD flags\n");
		goto err;
	}
	if ((src) && (flags & (RTO | PRE))) {
		pr_debug("SEQ IN PTR: Invalid usage of RTO or PRE flag\n");
		goto err;
	}

	/* write flag fields */
	if (flags & RBS)
		opcode |= SQIN_RBS;
	if (flags & INL)
		opcode |= SQIN_INL;
	if (flags & SGF)
		opcode |= SQIN_SGF;
	if (flags & PRE)
		opcode |= SQIN_PRE;
	if (flags & RTO)
		opcode |= SQIN_RTO;
	if (flags & RJD)
		opcode |= SQIN_RJD;
	if ((length >> 16) || (flags & EXT))
		opcode |= SQIN_EXT;
	else
		opcode |= length & SQIN_LEN_MASK;

	program->buffer[program->current_pc] = opcode;
	program->current_pc++;
	program->current_instraction++;

	/* write pointer or immidiate data field */
	if (!(opcode & (SQIN_PRE | SQIN_RTO))) {
		program->buffer[program->current_pc] = src;
		program->current_pc++;
	}
	/* write extended length field */
	if (opcode & SQIN_EXT) {
		program->buffer[program->current_pc] = length;
		program->current_pc++;
	}
	return program->current_pc;

 err:
	program->first_error_pc = program->current_pc;
	program->current_instraction++;
	return program->current_pc;
}

static inline unsigned seq_out_ptr(struct program *program, uintptr_t dst,
				   uint32_t length, uint32_t flags)
{
	uint32_t opcode = CMD_SEQ_OUT_PTR;

	/* Parameters checking */
	if ((rta_sec_era == RTA_SEC_ERA_1) && (flags & RTO)) {
		pr_debug("SEQ IN PTR: Restoring output sequences (RTO) not "
			 "supported\n");
		goto err;
	}
	if ((flags & RTO) && (flags & PRE)) {
		pr_debug("SEQ OUT PTR: Invalid usage of RTO and PRE flags\n");
		goto err;
	}
	if ((dst) && (flags & (RTO | PRE))) {
		pr_debug("SEQ OUT PTR: Invalid usage of RTO or PRE flag\n");
		goto err;
	}

	/* write flag fields */
	if (flags & PRE)
		opcode |= SQOUT_PRE;
	if (flags & RTO)
		opcode |= SQOUT_RTO;
	if ((length >> 16) || (flags & EXT))
		opcode |= SQOUT_EXT;
	else
		opcode |= length & SQOUT_LEN_MASK;

	program->buffer[program->current_pc] = opcode;
	program->current_pc++;
	program->current_instraction++;

	/* write pointer or immidiate data field */
	if (!(opcode & (SQOUT_PRE | SQOUT_RTO))) {
		program->buffer[program->current_pc] = dst;
		program->current_pc++;
	}

	/* write extended length field */
	if (opcode & SQOUT_EXT) {
		program->buffer[program->current_pc] = length;
		program->current_pc++;
	}
	return program->current_pc;

 err:
	program->first_error_pc = program->current_pc;
	program->current_instraction++;
	return program->current_pc;
}

#endif /* __RTA_SEQ_IN_OUT_PTR_CMD_H__ */
