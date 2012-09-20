#ifndef __RTA_HEADER_CMD_H__
#define __RTA_HEADER_CMD_H__

static inline uint32_t shr_header(struct program *program, uint32_t share,
			   uint32_t start_idx, uint32_t flags)
{
	uint32_t opcode = CMD_SHARED_DESC_HDR;

	switch (share) {
	case SHR_ALWAYS:
		opcode |= HDR_SHARE_ALWAYS;
		break;
	case SHR_SERIAL:
		opcode |= HDR_SHARE_SERIAL;
		break;
	case SHR_NEVER:
		opcode |= HDR_SHARE_NEVER;
		break;
	case SHR_WAIT:
		opcode |= HDR_SHARE_WAIT;
		break;
	default:
		pr_debug("SHR_DESC: SHARE VALUE is not supported. "
				"SEC Program Line: %d\n", program->current_pc);
		goto err;
	}

	opcode |= HDR_ONE;
	opcode |= (start_idx & HDR_START_IDX_MASK) << HDR_START_IDX_SHIFT;

	if (flags & RIF)
		opcode |= HDR_RIF;
	if (flags & DNR)
		opcode |= HDR_DNR;
	if (flags & CIF)
		opcode |= HDR_CLEAR_IFIFO;
	if (flags & HDR_SAVECTX)
		opcode |= HDR_SAVECTX;
	if (flags & PD)
		opcode |= HDR_PROP_DNR;

	program->buffer[program->current_pc] = opcode;
	program->current_instraction++;

	if (program->current_instraction == 1) {
		program->shrhdr = program->buffer;
		*program->shrhdr |= (program->current_pc << 16);
	}
	return program->current_pc++;
 err:
	program->first_error_pc = program->current_pc;
	program->current_instraction++;
	return program->current_pc++;
}

static inline uint32_t job_header(struct program *program, uint32_t share,
			   uintptr_t shr_desc, uint32_t flags)
{
	uint32_t opcode = CMD_DESC_HDR;

	switch (share) {

	case SHR_ALWAYS:
		opcode |= HDR_SHARE_ALWAYS;
		break;
	case SHR_SERIAL:
		opcode |= HDR_SHARE_SERIAL;
		break;
	case SHR_NEVER:
		opcode |= HDR_SHARE_NEVER;
		break;
	case SHR_WAIT:
		opcode |= HDR_SHARE_WAIT;
		break;
	case SHR_DEFER:
		opcode |= HDR_SHARE_DEFER;
		break;
	default:
		pr_debug("JOB_DESC: SHARE VALUE is not supported. "
				"SEC Program Line: %d\n", program->current_pc);
		goto err;
	}

	opcode |= HDR_ONE;

	if (flags & RSMS)
		opcode |= HDR_RSLS;
	if (flags & DNR)
		opcode |= HDR_DNR;
	if (flags & TD)
		opcode |= HDR_TRUSTED;
	if (flags & MTD)
		opcode |= HDR_MAKE_TRUSTED;
	if (flags & REO)
		opcode |= HDR_REVERSE;
	if (flags & SHR)
		opcode |= HDR_SHARED;

	program->buffer[program->current_pc] = opcode;
	program->current_instraction++;

	if (program->current_instraction == 1) {
		program->jobhdr = program->buffer;

		if (opcode & HDR_SHARED) {
			*program->jobhdr |= ((shr_desc & HDR_DESCLEN_SHR_MASK)
						<< 16);
			program->current_pc++;

			if (program->ps == 1) {
				program->buffer[program->current_pc++] =
						high_32b(shr_desc);
				program->buffer[program->current_pc] =
						low_32b(shr_desc);
			} else {
				program->buffer[program->current_pc] =
						low_32b(shr_desc);
			}
		} else {
			*program->jobhdr |= (program->current_pc << 16);
		}
	}

	/* Note: descriptor length is set in program_finalize routine */
	return program->current_pc++;
 err:
	program->first_error_pc = program->current_pc;
	program->current_instraction++;
	return program->current_pc++;
}

#endif /* __RTA_HEADER_CMD_H__ */
