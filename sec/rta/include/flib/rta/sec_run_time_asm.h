/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#ifndef __RTA_SEC_RUN_TIME_ASM_H__
#define __RTA_SEC_RUN_TIME_ASM_H__

#include "flib/desc.h"

/* flib/compat.h is not delivered in kernel */
#ifndef __KERNEL__
#include "flib/compat.h"
#endif

/**
 * enum rta_sec_era - SEC HW block revisions supported by the RTA library
 * @RTA_SEC_ERA_1: SEC Era 1
 * @RTA_SEC_ERA_2: SEC Era 2
 * @RTA_SEC_ERA_3: SEC Era 3
 * @RTA_SEC_ERA_4: SEC Era 4
 * @RTA_SEC_ERA_5: SEC Era 5
 * @RTA_SEC_ERA_6: SEC Era 6
 * @RTA_SEC_ERA_7: SEC Era 7
 * @RTA_SEC_ERA_8: SEC Era 8
 * @MAX_SEC_ERA: maximum SEC HW block revision supported by RTA library
 */
enum rta_sec_era {
	RTA_SEC_ERA_1,
	RTA_SEC_ERA_2,
	RTA_SEC_ERA_3,
	RTA_SEC_ERA_4,
	RTA_SEC_ERA_5,
	RTA_SEC_ERA_6,
	RTA_SEC_ERA_7,
	RTA_SEC_ERA_8,
	MAX_SEC_ERA = RTA_SEC_ERA_8
};

/**
 * DEFAULT_SEC_ERA - the default value for the SEC era in case the user provides
 * an unsupported value.
 */
#define DEFAULT_SEC_ERA	MAX_SEC_ERA

/**
 * USER_SEC_ERA - translates the SEC Era from internal to user representation.
 * @sec_era: SEC Era in internal (library) representation
 */
#define USER_SEC_ERA(sec_era)	(sec_era + 1)

/**
 * INTL_SEC_ERA - translates the SEC Era from user representation to internal.
 * @sec_era: SEC Era in user representation
 */
#define INTL_SEC_ERA(sec_era)	(sec_era - 1)

/* Convenience macros */
#define IMM_DATA 1
#define PTR_DATA 2
#define REG_TYPE 3

#define IMM(VAL)    VAL, IMM_DATA
#define PTR(VAL)    VAL, PTR_DATA

/**
 * enum rta_jump_type - Types of action taken by JUMP command
 * @LOCAL_JUMP: conditional jump to an offset within the descriptor buffer
 * @FAR_JUMP: conditional jump to a location outside the descriptor buffer,
 *            indicated by the POINTER field after the JUMP command.
 * @HALT: conditional halt - stop the execution of the current descriptor and
 *        writes PKHA / Math condition bits as status / error code.
 * @HALT_STATUS: conditional halt with user-specified status - stop the
 *               execution of the current descriptor and writes the value of
 *               "LOCAL OFFSET" JUMP field as status / error code.
 * @GOSUB: conditional subroutine call - similar to @LOCAL_JUMP, but also saves
 *         return address in the Return Address register; subroutine calls
 *         cannot be nested.
 * @RETURN: conditional subroutine return - similar to @LOCAL_JUMP, but the
 *          offset is taken from the Return Address register.
 * @LOCAL_JUMP_INC: similar to @LOCAL_JUMP, but increment the register specified
 *                  in "SRC_DST" JUMP field before evaluating the jump
 *                  condition.
 * @LOCAL_JUMP_DEC: similar to @LOCAL_JUMP, but decrement the register specified
 *                  in "SRC_DST" JUMP field before evaluating the jump
 *                  condition.
 */
enum rta_jump_type {
	LOCAL_JUMP,
	FAR_JUMP,
	HALT,
	HALT_STATUS,
	GOSUB,
	RETURN,
	LOCAL_JUMP_INC,
	LOCAL_JUMP_DEC
};

/**
 * enum rta_jump_cond - How test conditions are evaluated by JUMP command
 * @ALL_TRUE: perform action if ALL selected conditions are true
 * @ALL_FALSE: perform action if ALL selected conditions are false
 * @ANY_TRUE: perform action if ANY of the selected conditions is true
 * @ANY_FALSE: perform action if ANY of the selected conditions is false
 */
enum rta_jump_cond {
	ALL_TRUE,
	ALL_FALSE,
	ANY_TRUE,
	ANY_FALSE
};

/**
 * enum rta_share_type - Types of sharing for JOB_HDR and SHR_HDR commands
 * @SHR_NEVER: nothing is shared; descriptors can execute in parallel (i.e. no
 *             dependencies are allowed between them).
 * @SHR_WAIT: shared descriptor and keys are shared once the descriptor sets
 *            "OK to share" in DECO Control Register (DCTRL).
 * @SHR_SERIAL: shared descriptor and keys are shared once the descriptor has
 *              completed.
 * @SHR_ALWAYS: shared descriptor is shared anytime after the descriptor is
 *              loaded.
 * @SHR_DEFER: valid only for JOB_HDR; sharing type is the one specified
 *             in the shared descriptor associated with the job descriptor.
 */
enum rta_share_type {
	SHR_NEVER,
	SHR_WAIT,
	SHR_SERIAL,
	SHR_ALWAYS,
	SHR_DEFER
};

/* Registers definitions */
enum rta_regs {
	/* CCB Registers */
	_CONTEXT1 = 1,
#define CONTEXT1        _CONTEXT1, REG_TYPE
	_CONTEXT2,
#define CONTEXT2        _CONTEXT2, REG_TYPE
	_KEY1,
#define KEY1            _KEY1, REG_TYPE
	_KEY2,
#define KEY2            _KEY2, REG_TYPE
	_KEY1SZ,
#define KEY1SZ          _KEY1SZ, REG_TYPE
	_KEY2SZ,
#define KEY2SZ          _KEY2SZ, REG_TYPE
	_ICV1SZ,
#define ICV1SZ          _ICV1SZ, REG_TYPE
	_ICV2SZ,
#define ICV2SZ          _ICV2SZ, REG_TYPE
	_DATA1SZ,
#define DATA1SZ         _DATA1SZ, REG_TYPE
	_DATA2SZ,
#define DATA2SZ         _DATA2SZ, REG_TYPE
	_ALTDS1,
#define ALTDS1          _ALTDS1, REG_TYPE
	_IV1SZ,
#define IV1SZ           _IV1SZ, REG_TYPE
	_AAD1SZ,
#define AAD1SZ          _AAD1SZ, REG_TYPE
	_MODE1,
#define MODE1           _MODE1, REG_TYPE
	_MODE2,
#define MODE2           _MODE2, REG_TYPE
	_CCTRL,
#define CCTRL           _CCTRL, REG_TYPE
	_DCTRL,
#define DCTRL           _DCTRL, REG_TYPE
	_ICTRL,
#define ICTRL           _ICTRL, REG_TYPE
	_CLRW,
#define CLRW            _CLRW, REG_TYPE
	_CSTAT,
#define CSTAT           _CSTAT, REG_TYPE
	_IFIFO,
#define IFIFO           _IFIFO, REG_TYPE
	_NFIFO,
#define NFIFO           _NFIFO, REG_TYPE
	_OFIFO,
#define OFIFO           _OFIFO, REG_TYPE
	_PKASZ,
#define PKASZ           _PKASZ, REG_TYPE
	_PKBSZ,
#define PKBSZ           _PKBSZ, REG_TYPE
	_PKNSZ,
#define PKNSZ           _PKNSZ, REG_TYPE
	_PKESZ,
#define PKESZ           _PKESZ, REG_TYPE
	/* DECO Registers */
	_MATH0,
#define MATH0           _MATH0, REG_TYPE
	_MATH1,
#define MATH1           _MATH1, REG_TYPE
	_MATH2,
#define MATH2           _MATH2, REG_TYPE
	_MATH3,
#define MATH3           _MATH3, REG_TYPE
	_DESCBUF,
#define DESCBUF         _DESCBUF, REG_TYPE
	_JOBDESCBUF,
#define JOBDESCBUF      _JOBDESCBUF, REG_TYPE
	_SHAREDESCBUF,
#define SHAREDESCBUF    _SHAREDESCBUF, REG_TYPE
	_DPOVRD,
#define DPOVRD          _DPOVRD, REG_TYPE
	_DJQDA,
#define DJQDA           _DJQDA, REG_TYPE
	_DSTAT,
#define DSTAT           _DSTAT, REG_TYPE
	_DPID,
#define DPID            _DPID, REG_TYPE
	_DJQCTRL,
#define DJQCTRL         _DJQCTRL, REG_TYPE
	_ALTSOURCE,
#define ALTSOURCE       _ALTSOURCE, REG_TYPE
	_SEQINSZ,
#define SEQINSZ         _SEQINSZ, REG_TYPE
	_SEQOUTSZ,
#define SEQOUTSZ        _SEQOUTSZ, REG_TYPE
	_VSEQINSZ,
#define VSEQINSZ        _VSEQINSZ, REG_TYPE
	_VSEQOUTSZ,
#define VSEQOUTSZ        _VSEQOUTSZ, REG_TYPE
	/* PKHA Registers */
	_PKA,
#define PKA             _PKA, REG_TYPE
	_PKN,
#define PKN             _PKN, REG_TYPE
	_PKA0,
#define PKA0            _PKA0, REG_TYPE
	_PKA1,
#define PKA1            _PKA1, REG_TYPE
	_PKA2,
#define PKA2            _PKA2, REG_TYPE
	_PKA3,
#define PKA3            _PKA3, REG_TYPE
	_PKB,
#define PKB             _PKB, REG_TYPE
	_PKB0,
#define PKB0            _PKB0, REG_TYPE
	_PKB1,
#define PKB1            _PKB1, REG_TYPE
	_PKB2,
#define PKB2            _PKB2, REG_TYPE
	_PKB3,
#define PKB3            _PKB3, REG_TYPE
	_PKE,
#define PKE             _PKE, REG_TYPE
	/* Pseudo registers */
	_AB1,
#define AB1             _AB1, REG_TYPE
	_AB2,
#define AB2             _AB2, REG_TYPE
	_ABD,
#define ABD             _ABD, REG_TYPE
	_IFIFOABD,
#define IFIFOABD        _IFIFOABD, REG_TYPE
	_IFIFOAB1,
#define IFIFOAB1        _IFIFOAB1, REG_TYPE
	_IFIFOAB2,
#define IFIFOAB2        _IFIFOAB2, REG_TYPE
	_AFHA_SBOX,
#define AFHA_SBOX       _AFHA_SBOX, REG_TYPE
	_MDHA_SPLIT_KEY,
#define MDHA_SPLIT_KEY  _MDHA_SPLIT_KEY, REG_TYPE
	_JOBSRC,
#define JOBSRC          _JOBSRC, REG_TYPE
	_ZERO,
#define ZERO            _ZERO, REG_TYPE
	_ONE,
#define ONE             _ONE, REG_TYPE
	_AAD1,
#define AAD1            _AAD1, REG_TYPE
	_IV1,
#define IV1             _IV1, REG_TYPE
	_IV2,
#define IV2             _IV2, REG_TYPE
	_MSG1,
#define MSG1            _MSG1, REG_TYPE
	_MSG2,
#define MSG2            _MSG2, REG_TYPE
	_MSG,
#define MSG             _MSG, REG_TYPE
	_MSGOUTSNOOP,
#define MSGOUTSNOOP     _MSGOUTSNOOP, REG_TYPE
	_MSGINSNOOP,
#define MSGINSNOOP      _MSGINSNOOP, REG_TYPE
	_ICV1,
#define ICV1            _ICV1, REG_TYPE
	_ICV2,
#define ICV2            _ICV2, REG_TYPE
	_SKIP,
#define SKIP            _SKIP, REG_TYPE
	_NONE,
#define NONE            _NONE, REG_TYPE
	_RNGOFIFO,
#define RNGOFIFO        _RNGOFIFO, REG_TYPE
	_RNG,
#define RNG             _RNG, REG_TYPE
	_IDFNS,
#define IDFNS           _IDFNS, REG_TYPE
	_ODFNS,
#define ODFNS           _ODFNS, REG_TYPE
	_NFIFOSZ,
#define NFIFOSZ         _NFIFOSZ, REG_TYPE
	_SZ,
#define SZ              _SZ, REG_TYPE
	_PAD,
#define PAD             _PAD, REG_TYPE
	_SAD1,
#define SAD1            _SAD1, REG_TYPE
	_AAD2,
#define AAD2            _AAD2, REG_TYPE
	_BIT_DATA,
#define BIT_DATA        _BIT_DATA, REG_TYPE
	_NFIFO_SZL,
#define NFIFO_SZL	_NFIFO_SZL, REG_TYPE
	_NFIFO_SZM,
#define NFIFO_SZM	_NFIFO_SZM, REG_TYPE
	_NFIFO_L,
#define NFIFO_L		_NFIFO_L, REG_TYPE
	_NFIFO_M,
#define NFIFO_M		_NFIFO_M, REG_TYPE
	_SZL,
#define SZL		_SZL, REG_TYPE
	_SZM,
#define SZM		_SZM, REG_TYPE
	_JOBDESCBUF_EFF,
#define JOBDESCBUF_EFF	_JOBDESCBUF_EFF, REG_TYPE
	_SHAREDESCBUF_EFF,
#define SHAREDESCBUF_EFF	_SHAREDESCBUF_EFF, REG_TYPE
	_METADATA,
#define METADATA	_METADATA, REG_TYPE
	_GTR,
#define GTR		_GTR, REG_TYPE
	_STR,
#define STR		_STR, REG_TYPE
	_OFIFO_SYNC,
#define OFIFO_SYNC      _OFIFO_SYNC, REG_TYPE
	_MSGOUTSNOOP_ALT
#define MSGOUTSNOOP_ALT  _MSGOUTSNOOP_ALT, REG_TYPE
};

/* Command flags */
#define FLUSH1          0x00000001
#define LAST1           0x00000002
#define LAST2           0x00000004
#define IMMED           0x00000008
#define SGF             0x00000010
#define VLF             0x00000020
#define EXT             0x00000040
#define CONT            0x00000080
#define SEQ             0x00000100
#define AIDF		0x00000200
#define FLUSH2          0x00000400

#define CLASS1          0x00000800
#define CLASS2          0x00001000
#define BOTH            0x00002000

/* SEQ IN/OUT PTR Command specific flags */
#define RBS             0x00010000
#define INL             0x00020000
#define PRE             0x00040000
#define RTO             0x00080000
#define RJD             0x00100000
#define SOP		0x00200000
#define RST		0x00400000
#define EWS		0x00800000

#define ENC             0x00004000	/* Encrypted Key */
#define EKT             0x00008000	/* AES CCM Encryption (default is
					 * AES ECB Encryption) */
#define TK              0x00010000	/* Trusted Descriptor Key (default is
					 * Job Descriptor Key) */
#define NWB             0x00020000	/* No Write Back Key */
#define PTS             0x00040000	/* Plaintext Store */

/* HEADER Command specific flags */
#define RIF             0x00010000
#define DNR             0x00020000
#define CIF             0x00040000
#define PD              0x00080000
#define RSMS            0x00100000
#define TD              0x00200000
#define MTD             0x00400000
#define REO             0x00800000
#define SHR             0x01000000
#define SC		0x02000000
/* Extended HEADER specific flags */
#define DSV		0x00000080
#define DSEL_MASK	0x00000007	/* DECO Select */
#define FTD		0x00000100

/* JUMP Command specific flags */
#define NIFP            0x00100000
#define NIP             0x00200000
#define NOP             0x00400000
#define NCP             0x00800000
#define CALM            0x01000000

#define MATH_Z          0x02000000
#define MATH_N          0x04000000
#define MATH_NV         0x08000000
#define MATH_C          0x10000000
#define PK_0            0x20000000
#define PK_GCD_1        0x40000000
#define PK_PRIME        0x80000000
#define SELF            0x00000001
#define SHRD            0x00000002
#define JQP             0x00000004

/* NFIFOADD specific flags */
#define PAD_ZERO        0x00010000
#define PAD_NONZERO     0x00020000
#define PAD_INCREMENT   0x00040000
#define PAD_RANDOM      0x00080000
#define PAD_ZERO_N1     0x00100000
#define PAD_NONZERO_0   0x00200000
#define PAD_N1          0x00800000
#define PAD_NONZERO_N   0x01000000
#define OC              0x02000000
#define BM              0x04000000
#define PR              0x08000000
#define PS              0x10000000
#define BP              0x20000000

/* MOVE Command specific flags */
#define WAITCOMP        0x00010000
#define SIZE_WORD	0x00020000
#define SIZE_BYTE	0x00040000
#define SIZE_DWORD	0x00080000

/* MOVE command type */
#define __MOVE		1
#define __MOVEB		2
#define __MOVEDW	3

/* MATH command specific flags */
#define IFB         MATH_IFB
#define NFU         MATH_NFU
#define STL         MATH_STL
#define SWP         MATH_SWP

/**
 * struct program - descriptor buffer management structure
 * @current_pc:	current offset in descriptor
 * @current_instruction: current instruction in descriptor
 * @first_error_pc: offset of the first error in descriptor
 * @start_pc: start offset in descriptor buffer
 * @buffer: buffer carrying descriptor
 * @shrhdr: shared descriptor header
 * @jobhdr: job descriptor header
 * @ps: pointer fields size; if ps is true, pointers will be 36bits in
 *      length; if ps is false, pointers will be 32bits in length
 * @bswap: if true, perform byte swap on a 4-byte boundary
 */
struct program {
	unsigned current_pc;
	unsigned current_instruction;
	unsigned first_error_pc;
	unsigned start_pc;
	uint32_t *buffer;
	uint32_t *shrhdr;
	uint32_t *jobhdr;
	bool ps;
	bool bswap;
};

static inline void rta_program_cntxt_init(struct program *program,
					 uint32_t *buffer, unsigned offset)
{
	program->current_pc = 0;
	program->current_instruction = 0;
	program->first_error_pc = 0;
	program->start_pc = offset;
	program->buffer = buffer;
	program->shrhdr = NULL;
	program->jobhdr = NULL;
	program->ps = false;
	program->bswap = false;
}

static inline void __rta__desc_bswap(uint32_t *buff, unsigned buff_len)
{
	unsigned i;

	for (i = 0; i < buff_len; i++)
		buff[i] = swab32(buff[i]);
}

static inline unsigned rta_program_finalize(struct program *program)
{
	/* Descriptor is not allowed to go beyond 64 words size */
	if (program->current_pc > MAX_CAAM_DESCSIZE)
		pr_err("Descriptor Size exceeded max limit of 64 words\n");

	/* Descriptor is erroneous */
	if (program->first_error_pc)
		pr_err("Descriptor creation error\n");

	/* Update descriptor length in shared and job descriptor headers */
	if (program->shrhdr != NULL) {
		*program->shrhdr |= program->current_pc;
		if (program->bswap)
			__rta__desc_bswap(program->shrhdr, program->current_pc);
	} else if (program->jobhdr != NULL) {
		*program->jobhdr |= program->current_pc;
		if (program->bswap)
			__rta__desc_bswap(program->jobhdr, program->current_pc);
	}

	return program->current_pc;
}

static inline unsigned rta_program_set_36bit_addr(struct program *program)
{
	program->ps = true;
	return program->current_pc;
}

static inline unsigned rta_program_set_bswap(struct program *program)
{
	program->bswap = true;
	return program->current_pc;
}

static inline void __rta_out32(struct program *program, uint32_t val)
{
	program->buffer[program->current_pc] = val;
	program->current_pc++;
}

static inline void __rta_out64(struct program *program, bool is_ext,
			       uint64_t val)
{
	if (is_ext)
		__rta_out32(program, upper_32_bits(val));

	__rta_out32(program, lower_32_bits(val));
}

static inline unsigned rta_word(struct program *program, uint32_t val)
{
	unsigned start_pc = program->current_pc;

	__rta_out32(program, val);

	return start_pc;
}

static inline unsigned rta_dword(struct program *program, uint64_t val)
{
	unsigned start_pc = program->current_pc;

	__rta_out64(program, true, val);

	return start_pc;
}

static inline unsigned rta_copy_data(struct program *program, uint8_t *data,
				     unsigned length)
{
	unsigned i;
	unsigned start_pc = program->current_pc;
	uint8_t *tmp = (uint8_t *)&program->buffer[program->current_pc];

	for (i = 0; i < length; i++)
		*tmp++ = data[i];
	program->current_pc += (length + 3) / 4;

	return start_pc;
}

static inline void __rta_inline_data(struct program *program, uint64_t data,
				     int data_type, uint32_t length)
{
	if (data_type == IMM_DATA) {
		__rta_out64(program, length > 4, data);
	} else {
		uint8_t *tmp = (uint8_t *)&program->buffer[program->current_pc];
		uint32_t i;

		for (i = 0; i < length; i++)
			*tmp++ = ((uint8_t *)(uintptr_t)data)[i];
		program->current_pc += ((length + 3) / 4);
	}
}

static inline unsigned rta_desc_len(uint32_t *buffer, uint32_t mask)
{
	 return *buffer & mask;
}

static inline unsigned rta_desc_bytes(uint32_t *buffer, uint32_t mask)
{
	 return (unsigned)(rta_desc_len(buffer, mask) * CAAM_CMD_SZ);
}

static inline unsigned rta_set_label(struct program *program)
{
	return program->current_pc + program->start_pc;
}

/* Operators */
#define ADD      (0x00 << MATH_FUN_SHIFT)
#define ADDC     (0x01 << MATH_FUN_SHIFT)
#define SUB      (0x02 << MATH_FUN_SHIFT)
#define SUBB     (0x03 << MATH_FUN_SHIFT)
#define OR       (0x04 << MATH_FUN_SHIFT)
#define AND      (0x05 << MATH_FUN_SHIFT)
#define XOR      (0x06 << MATH_FUN_SHIFT)
#define LSHIFT   (0x07 << MATH_FUN_SHIFT)
#define RSHIFT   (0x08 << MATH_FUN_SHIFT)
#define SHLD     (0x09 << MATH_FUN_SHIFT)
#define ZBYTE    (0x0A << MATH_FUN_SHIFT)
#define BSWAP    (0x0B << MATH_FUN_SHIFT)


static inline int rta_patch_move(struct program *program, int line,
				 unsigned new_ref, bool check_swap)
{
	uint32_t opcode;
	bool bswap = check_swap && program->bswap;

	if (line < 0)
		return -EINVAL;

	opcode = bswap ? swab32(program->buffer[line]) : program->buffer[line];

	opcode &= (uint32_t)~MOVE_OFFSET_MASK;
	opcode |= (new_ref << (MOVE_OFFSET_SHIFT + 2)) & MOVE_OFFSET_MASK;
	program->buffer[line] = bswap ? swab32(opcode) : opcode;

	return 0;
}

static inline int rta_patch_jmp(struct program *program, int line,
				unsigned new_ref, bool check_swap)
{
	uint32_t opcode;
	bool bswap = check_swap && program->bswap;

	if (line < 0)
		return -EINVAL;

	opcode = bswap ? swab32(program->buffer[line]) : program->buffer[line];

	opcode &= (uint32_t)~JUMP_OFFSET_MASK;
	opcode |= (new_ref - (line + program->start_pc)) & JUMP_OFFSET_MASK;
	program->buffer[line] = bswap ? swab32(opcode) : opcode;

	return 0;
}

static inline int rta_patch_header(struct program *program, int line,
				   unsigned new_ref, bool check_swap)
{
	uint32_t opcode;
	bool bswap = check_swap && program->bswap;

	if (line < 0)
		return -EINVAL;

	opcode = bswap ? swab32(program->buffer[line]) : program->buffer[line];

	opcode &= (uint32_t)~HDR_START_IDX_MASK;
	opcode |= (new_ref << HDR_START_IDX_SHIFT) & HDR_START_IDX_MASK;
	program->buffer[line] = bswap ? swab32(opcode) : opcode;

	return 0;
}

static inline int rta_patch_load(struct program *program, int line,
				 unsigned new_ref)
{
	uint32_t opcode;

	if (line < 0)
		return -EINVAL;

	opcode = program->buffer[line] & (uint32_t)~LDST_OFFSET_MASK;

	if (opcode & (LDST_SRCDST_WORD_DESCBUF | LDST_CLASS_DECO))
		opcode |= (new_ref << LDST_OFFSET_SHIFT) & LDST_OFFSET_MASK;
	else
		opcode |= (new_ref << (LDST_OFFSET_SHIFT + 2)) &
			  LDST_OFFSET_MASK;

	program->buffer[line] = opcode;

	return 0;
}

static inline int rta_patch_store(struct program *program, int line,
				  unsigned new_ref, bool check_swap)
{
	uint32_t opcode;
	bool bswap = check_swap && program->bswap;

	if (line < 0)
		return -EINVAL;

	opcode = bswap ? swab32(program->buffer[line]) : program->buffer[line];

	opcode &= (uint32_t)~LDST_OFFSET_MASK;

	switch (opcode & LDST_SRCDST_MASK) {
	case LDST_SRCDST_WORD_DESCBUF:
	case LDST_SRCDST_WORD_DESCBUF_JOB:
	case LDST_SRCDST_WORD_DESCBUF_SHARED:
	case LDST_SRCDST_WORD_DESCBUF_JOB_WE:
	case LDST_SRCDST_WORD_DESCBUF_SHARED_WE:
		opcode |= ((new_ref) << LDST_OFFSET_SHIFT) & LDST_OFFSET_MASK;
		break;
	default:
		opcode |= (new_ref << (LDST_OFFSET_SHIFT + 2)) &
			  LDST_OFFSET_MASK;
	}

	program->buffer[line] = bswap ? swab32(opcode) : opcode;

	return 0;
}

static inline int rta_patch_raw(struct program *program, int line,
				unsigned mask, unsigned new_val,
				bool check_swap)
{
	uint32_t opcode;
	bool bswap = check_swap && program->bswap;

	if (line < 0)
		return -EINVAL;

	opcode = bswap ? swab32(program->buffer[line]) : program->buffer[line];

	opcode &= (uint32_t)~mask;
	opcode |= new_val & mask;
	program->buffer[line] = bswap ? swab32(opcode) : opcode;

	return 0;
}

static inline int __rta_map_opcode(uint32_t name,
				  const uint32_t (*map_table)[2],
				  unsigned num_of_entries, uint32_t *val)
{
	unsigned i;

	for (i = 0; i < num_of_entries; i++)
		if (map_table[i][0] == name) {
			*val = map_table[i][1];
			return 0;
		}

	return -EINVAL;
}

static inline void __rta_map_flags(uint32_t flags,
				   const uint32_t (*flags_table)[2],
				   unsigned num_of_entries, uint32_t *opcode)
{
	unsigned i;

	for (i = 0; i < num_of_entries; i++) {
		if (flags_table[i][0] & flags)
			*opcode |= flags_table[i][1];
	}
}

#endif /* __RTA_SEC_RUN_TIME_ASM_H__ */
