/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#ifndef __RTA_SEQ_IN_OUT_PTR_CMD_H__
#define __RTA_SEQ_IN_OUT_PTR_CMD_H__

extern enum rta_sec_era rta_sec_era;

/* Allowed SEQ IN PTR flags for each SEC Era. */
static const uint32_t seq_in_ptr_flags[] = {
	RBS | INL | SGF | PRE | EXT | RTO,
	RBS | INL | SGF | PRE | EXT | RTO | RJD,
	RBS | INL | SGF | PRE | EXT | RTO | RJD,
	RBS | INL | SGF | PRE | EXT | RTO | RJD,
	RBS | INL | SGF | PRE | EXT | RTO | RJD | SOP
};

static inline unsigned rta_seq_in_ptr(struct program *program, uint64_t src,
				      uint32_t length, uint32_t flags)
{
	uint32_t opcode = CMD_SEQ_IN_PTR;
	unsigned start_pc = program->current_pc;

	/* Parameters checking */
	if ((flags & RTO) && (flags & PRE)) {
		pr_debug("SEQ IN PTR: Invalid usage of RTO and PRE flags\n");
		goto err;
	}
	if (flags & ~seq_in_ptr_flags[rta_sec_era]) {
		pr_debug("SEQ IN PTR: Flag(s) not supported by SEC Era %d\n",
			 USER_SEC_ERA(rta_sec_era));
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
	if ((flags & SOP) && (flags & (RBS | PRE | RTO | EXT))) {
		pr_debug("SEQ IN PTR: Invalid usage of SOP and (RBS or PRE or "
			 "RTO or EXT) flags\n");
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
	if (flags & SOP)
		opcode |= SQIN_SOP;
	if ((length >> 16) || (flags & EXT)) {
		if (flags & SOP) {
			pr_debug("SEQ IN PTR: Invalid usage of SOP and EXT "
				 "flags\n");
			goto err;
		}

		opcode |= SQIN_EXT;
	} else {
		opcode |= length & SQIN_LEN_MASK;
	}

	program->buffer[program->current_pc] = opcode;
	program->current_pc++;
	program->current_instruction++;

	/* write pointer or immediate data field */
	if (!(opcode & (SQIN_PRE | SQIN_RTO))) {
		if (program->ps == 1) {
			program->buffer[program->current_pc] = high_32b(src);
			program->current_pc++;
		}

		program->buffer[program->current_pc] = low_32b(src);
		program->current_pc++;
	}
	/* write extended length field */
	if (opcode & SQIN_EXT) {
		program->buffer[program->current_pc] = length;
		program->current_pc++;
	}

	return start_pc;

 err:
	program->first_error_pc = start_pc;
	program->current_instruction++;
	return start_pc;
}

static inline unsigned rta_seq_out_ptr(struct program *program, uint64_t dst,
				       uint32_t length, uint32_t flags)
{
	uint32_t opcode = CMD_SEQ_OUT_PTR;
	unsigned start_pc = program->current_pc;

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
	if (flags & SGF)
		opcode |= SQOUT_SGF;
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
	program->current_instruction++;

	/* write pointer or immediate data field */
	if (!(opcode & (SQOUT_PRE | SQOUT_RTO))) {
		if (program->ps == 1) {
			program->buffer[program->current_pc] = high_32b(dst);
			program->current_pc++;
		}

		program->buffer[program->current_pc] = low_32b(dst);
		program->current_pc++;
	}

	/* write extended length field */
	if (opcode & SQOUT_EXT) {
		program->buffer[program->current_pc] = length;
		program->current_pc++;
	}

	return start_pc;

 err:
	program->first_error_pc = start_pc;
	program->current_instruction++;
	return start_pc;
}

#endif /* __RTA_SEQ_IN_OUT_PTR_CMD_H__ */
