#ifndef __RTA_SEC_RUN_TIME_ASM_H__
#define __RTA_SEC_RUN_TIME_ASM_H__

#include "../desc.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#ifndef pr_debug
#ifdef DEBUG
#define pr_debug(fmt, ...)    printf(fmt, ##__VA_ARGS__)
#else
#define pr_debug(fmt, ...)
#endif
#endif

#ifndef high_32b
#define high_32b(x) ((uint32_t)((x) >> 32))
#endif

#ifndef low_32b
#define low_32b(x) ((uint32_t)(x))
#endif

/**
 * ARRAY_SIZE(x) - Returns the number of elements in an array
 */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/**
 * @enum rta_sec_era
 * @details SEC HW block revisions supported by the RTA library.
 */
enum rta_sec_era {
	RTA_SEC_ERA_1,
	RTA_SEC_ERA_2,
	RTA_SEC_ERA_3,
	RTA_SEC_ERA_4,
	RTA_SEC_ERA_5,
	MAX_SEC_ERA = RTA_SEC_ERA_5 /**< Maximum SEC HW block revision
					 supported by RTA library */
};

/**
 * @def DEFAULT_SEC_ERA
 * The default value for the SEC era in case the user provides an unsupported.
 * value.
 */
#define DEFAULT_SEC_ERA	MAX_SEC_ERA

/**
 * @def USER_SEC_ERA(sec_era)
 * Translates the SEC Era from internal to user representation.
 */
#define USER_SEC_ERA(sec_era)	(sec_era + 1)

/* Convinience macros */
#define WITH(x) (x)
#define SIZE(x) (x)
#define NO_OPT 0
#define IMM_DATA 1
#define PTR_DATA 2
#define REG_TYPE 3

#define BYTES_2 2
#define BYTES_4 4
#define BYTES_8 8
#define BYTES_16 16

#define IMM(VAL)    VAL, IMM_DATA
#define PTR(VAL)    VAL, PTR_DATA

#define REGISTER    0x01000000
#define QUALIFIER   0x02000000
#define MATH_OP     0x03000000
#define MOVE_DST_SRC    0x04000000
#define FIFO        0x05000000
#define CONDITION   0x06000000
#define KEYS        0x07000000
#define TEST_TYPE   0x08000000
#define SHARE_TYPE  0x09000000
#define JUMP_TYPE   0x0A000000

#define IFB         MATH_IFB
#define NFU         MATH_NFU
#define STL         MATH_STL

#define LOCAL_JUMP  (0x00 | JUMP_TYPE)
#define FAR_JUMP    (0x01 | JUMP_TYPE)
#define HALT        (0x02 | JUMP_TYPE)
#define HALT_STATUS (0x03 | JUMP_TYPE)
#define GOSUB       (0x04 | JUMP_TYPE)
#define RETURN      (0x05 | JUMP_TYPE)

#define SHR_NEVER   (0x00 | SHARE_TYPE)
#define SHR_WAIT    (0x01 | SHARE_TYPE)
#define SHR_SERIAL  (0x02 | SHARE_TYPE)
#define SHR_ALWAYS  (0x03 | SHARE_TYPE)
#define SHR_DEFER   (0x04 | SHARE_TYPE)

#define ALL_TRUE    (0x00 | TEST_TYPE)
#define ALL_FALSE   (0x01 | TEST_TYPE)
#define ANY_TRUE    (0x02 | TEST_TYPE)
#define ANY_FALSE   (0x03 | TEST_TYPE)

/* Registers definitions */

/* CCB Registers */
#define _CONTEXT1       (0x01 | REGISTER)
#define CONTEXT1        _CONTEXT1, REG_TYPE

#define _CONTEXT2       (0x02 | REGISTER)
#define CONTEXT2        _CONTEXT2, REG_TYPE

#define _KEY1           (0x03 | REGISTER)
#define KEY1            _KEY1, REG_TYPE

#define _KEY2           (0x04 | REGISTER)
#define KEY2            _KEY2, REG_TYPE

#define _KEY1SZ         (0x05 | REGISTER)
#define KEY1SZ          _KEY1SZ, REG_TYPE

#define _KEY2SZ         (0x06 | REGISTER)
#define KEY2SZ          _KEY2SZ, REG_TYPE

#define _ICV1SZ         (0x07 | REGISTER)
#define ICV1SZ          _ICV1SZ, REG_TYPE

#define _ICV2SZ         (0x08 | REGISTER)
#define ICV2SZ          _ICV2SZ, REG_TYPE

#define _DATA1SZ        (0x09 | REGISTER)
#define DATA1SZ         _DATA1SZ, REG_TYPE

#define _DATA2SZ        (0x0a | REGISTER)
#define DATA2SZ         _DATA2SZ, REG_TYPE

#define _ALTDS1         (0x0b | REGISTER)
#define ALTDS1          _ALTDS1, REG_TYPE

#define _IV1SZ          (0x0c | REGISTER)
#define IV1SZ           _IV1SZ, REG_TYPE

#define _AAD1SZ         (0x0d | REGISTER)
#define AAD1SZ          _AAD1SZ, REG_TYPE

#define _MODE1          (0x0e | REGISTER)
#define MODE1           _MODE1, REG_TYPE

#define _MODE2          (0x0f | REGISTER)
#define MODE2           _MODE2, REG_TYPE

#define _CCTRL          (0x10 | REGISTER)
#define CCTRL           _CCTRL, REG_TYPE

#define _DCTRL          (0x11 | REGISTER)
#define DCTRL           _DCTRL, REG_TYPE

#define _ICTRL          (0x12 | REGISTER)
#define ICTRL           _ICTRL, REG_TYPE

#define _CLRW           (0x13 | REGISTER)
#define CLRW            _CLRW, REG_TYPE

#define _CSTAT          (0x14 | REGISTER)
#define CSTAT           _CSTAT, REG_TYPE

#define _IFIFO          (0x16 | FIFO)
#define IFIFO           _IFIFO, REG_TYPE

#define _NFIFO          (0x17 | FIFO)
#define NFIFO           _NFIFO, REG_TYPE

#define _OFIFO          (0x18 | FIFO)
#define OFIFO           _OFIFO, REG_TYPE

#define _PKASZ          (0x19 | REGISTER)
#define PKASZ           _PKASZ, REG_TYPE

#define _PKBSZ          (0x1a | REGISTER)
#define PKBSZ           _PKBSZ, REG_TYPE

#define _PKNSZ          (0x1b | REGISTER)
#define PKNSZ           _PKNSZ, REG_TYPE

#define _PKESZ          (0x1c | REGISTER)
#define PKESZ           _PKESZ, REG_TYPE

/* DECO Registers */
#define _MATH0          (0x1d | REGISTER)
#define MATH0           _MATH0, REG_TYPE

#define _MATH1          (0x1e | REGISTER)
#define MATH1           _MATH1, REG_TYPE

#define _MATH2          (0x1f | REGISTER)
#define MATH2           _MATH2, REG_TYPE

#define _MATH3          (0x20 | REGISTER)
#define MATH3           _MATH3, REG_TYPE

#define _DESCBUF        (0x21 | REGISTER)
#define DESCBUF         _DESCBUF, REG_TYPE

#define _JOBDESCBUF     (0x22 | REGISTER)
#define JOBDESCBUF      _JOBDESCBUF, REG_TYPE

#define _SHAREDESCBUF   (0x23 | REGISTER)
#define SHAREDESCBUF    _SHAREDESCBUF, REG_TYPE

#define _DPOVRD         (0x24 | REGISTER)
#define DPOVRD          _DPOVRD, REG_TYPE

#define _DJQDA          (0x25 | REGISTER)
#define DJQDA           _DJQDA, REG_TYPE

#define _DSTAT          (0x26 | REGISTER)
#define DSTAT           _DSTAT, REG_TYPE

#define _DPID           (0x27 | REGISTER)
#define DPID            _DPID, REG_TYPE

#define _DJQCTRL        (0x28 | REGISTER)
#define DJQCTRL         _DJQCTRL, REG_TYPE

#define _ALTSOURCE      (0x29 | REGISTER)
#define ALTSOURCE       _ALTSOURCE, REG_TYPE

#define _SEQINSZ        (0x2a | REGISTER)
#define SEQINSZ         _SEQINSZ, REG_TYPE

#define _SEQOUTSZ       (0x2b | REGISTER)
#define SEQOUTSZ        _SEQOUTSZ, REG_TYPE

#define _VSEQINSZ       (0x2c | REGISTER)
#define VSEQINSZ        _VSEQINSZ, REG_TYPE

#define _VSEQOUTSZ      (0x2e | REGISTER)
#define VSEQOUTSZ        _VSEQOUTSZ, REG_TYPE

/* PKHA Registers */
#define _PKA            (0x2f | REGISTER)
#define PKA             _PKA, REG_TYPE

#define _PKN            (0x30 | REGISTER)
#define PKN             _PKN, REG_TYPE

#define _PKA0           (0x31 | REGISTER)
#define PKA0            _PKA0, REG_TYPE

#define _PKA1           (0x32 | REGISTER)
#define PKA1            _PKA1, REG_TYPE

#define _PKA2           (0x33 | REGISTER)
#define PKA2            _PKA2, REG_TYPE

#define _PKA3           (0x34 | REGISTER)
#define PKA3            _PKA3, REG_TYPE

#define _PKB            (0x35 | REGISTER)
#define PKB             _PKB, REG_TYPE

#define _PKB0           (0x36 | REGISTER)
#define PKB0            _PKB0, REG_TYPE

#define _PKB1           (0x37 | REGISTER)
#define PKB1            _PKB1, REG_TYPE

#define _PKB2           (0x38 | REGISTER)
#define PKB2            _PKB2, REG_TYPE

#define _PKB3           (0x39 | REGISTER)
#define PKB3            _PKB3, REG_TYPE

#define _PKE            (0x3a | REGISTER)
#define PKE             _PKE, REG_TYPE

/* Pseudo registers */
#define _AB1            (0x3b | FIFO)
#define AB1             _AB1, REG_TYPE

#define _AB2            (0x3c | FIFO)
#define AB2             _AB2, REG_TYPE

#define _ABD            (0x3d | FIFO)
#define ABD             _ABD, REG_TYPE

#define _IFIFOABD       (0x3e | FIFO)
#define IFIFOABD        _IFIFOABD, REG_TYPE

#define _IFIFOAB1       (0x3f | FIFO)
#define IFIFOAB1        _IFIFOAB1, REG_TYPE

#define _IFIFOAB2       (0x40 | FIFO)
#define IFIFOAB2        _IFIFOAB2, REG_TYPE

#define _AFHA_SBOX      (0x41 | REGISTER)
#define AFHA_SBOX       _AFHA_SBOX, REG_TYPE

#define _MDHA_SPLIT_KEY (0x42 | REGISTER)
#define MDHA_SPLIT_KEY  _MDHA_SPLIT_KEY, REG_TYPE

#define _JOBSRC         (0x43 | REGISTER)
#define JOBSRC          _JOBSRC, REG_TYPE

#define _ZERO           (0x44 | REGISTER)
#define ZERO            _ZERO, REG_TYPE

#define _ONE            (0x45 | REGISTER)
#define ONE             _ONE, REG_TYPE

#define _AAD1           (0x46 | REGISTER)
#define AAD1            _AAD1, REG_TYPE

#define _IV1            (0x47 | REGISTER)
#define IV1             _IV1, REG_TYPE

#define _IV2            (0x48 | REGISTER)
#define IV2             _IV2, REG_TYPE

#define _MSG1           (0x49 | REGISTER)
#define MSG1            _MSG1, REG_TYPE

#define _MSG2           (0x4a | REGISTER)
#define MSG2            _MSG2, REG_TYPE

#define _MSG            (0x4b | REGISTER)
#define MSG             _MSG, REG_TYPE

#define _MSGOUTSNOOP    (0x4c | REGISTER)
#define MSGOUTSNOOP     _MSGOUTSNOOP, REG_TYPE

#define _MSGINSNOOP     (0x4d | REGISTER)
#define MSGINSNOOP      _MSGINSNOOP, REG_TYPE

#define _ICV1           (0x4e | REGISTER)
#define ICV1            _ICV1, REG_TYPE

#define _ICV2           (0x4f | REGISTER)
#define ICV2            _ICV2, REG_TYPE

#define _SKIP           (0x50 | REGISTER)
#define SKIP            _SKIP, REG_TYPE

#define _NONE           (0x51 | REGISTER)
#define NONE            _NONE, REG_TYPE

#define _RNGOFIFO       (0x52 | REGISTER)
#define RNGOFIFO        _RNGOFIFO, REG_TYPE

#define _RNG            (0x53 | REGISTER)
#define RNG             _RNG, REG_TYPE

#define _IDFNS          (0x54 | REGISTER)
#define IDFNS           _IDFNS, REG_TYPE

#define _ODFNS          (0x55 | REGISTER)
#define ODFNS           _ODFNS, REG_TYPE

#define _NFIFOSZ        (0x56 | REGISTER)
#define NFIFOSZ         _NFIFOSZ, REG_TYPE

#define _SZ             (0x57 | REGISTER)
#define SZ              _SZ, REG_TYPE

#define _PAD            (0x58 | REGISTER)
#define PAD             _PAD, REG_TYPE

#define _SAD1           (0x59 | REGISTER)
#define SAD1            _SAD1, REG_TYPE

#define _AAD2           (0x5a | REGISTER)
#define AAD2            _AAD2, REG_TYPE

#define _BIT_DATA       (0x5b | REGISTER)
#define BIT_DATA        _BIT_DATA, REG_TYPE

#define _NFIFO_SZL	(0x5c | REGISTER)
#define NFIFO_SZL	_NFIFO_SZL, REG_TYPE

#define _NFIFO_SZM	(0x5d | REGISTER)
#define NFIFO_SZM	_NFIFO_SZM, REG_TYPE

#define _NFIFO_L	(0x5e | REGISTER)
#define NFIFO_L		_NFIFO_L, REG_TYPE

#define _NFIFO_M	(0x5f | REGISTER)
#define NFIFO_M		_NFIFO_M, REG_TYPE

#define _SZL		(0x60 | REGISTER)
#define SZL		_SZL, REG_TYPE

#define _SZM		(0x61 | REGISTER)
#define SZM		_SZM, REG_TYPE

#define _JOBDESCBUF_EFF	(0x62 | REGISTER)
#define JOBDESCBUF_EFF	_JOBDESCBUF_EFF, REG_TYPE

#define _SHAREDESCBUF_EFF	(0x63 | REGISTER)
#define SHAREDESCBUF_EFF	_SHAREDESCBUF_EFF, REG_TYPE

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
#define WAITCOMP        0x00000200
#define FLUSH2          0x00000400

#define CLASS1          0x00000800
#define CLASS2          0x00001000
#define BOTH            0x00002000

/* SEQ IN/OUT PTR Command specific flags */
#define RBS             0x00000001
#define INL             0x00000002
#define PRE             0x00000004
#define RTO             0x00000008
#define RJD             0x00000010

#define ENC             0x00004000	/* Encrypted Key */
#define EKT             0x00008000	/* AES CCM Encryption */
#define TK              0x00010000	/* Trusted Descriptor Key */
#define NWB             0x00020000	/* No Write Back Key */
#define NRM             0x00040000	/* AES ECB Encryption */
#define JDK             0x00080000	/* Job Descriptor Key */

/* HEADER Command specific flags */
#define RIF             0x00000001
#define DNR             0x00000002
#define CIF             0x00000004
#define PD              0x00000008
#define RSMS            0x00000010
#define TD              0x00000020
#define MTD             0x00000040
#define REO             0x00000080
#define SHR             0x00000100
#define SC		0x00000200

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

/**
 * @struct    program sec_run_time_asm.h
 * @details   Descriptor buffer management structure
 */
struct program {
	int current_pc;		/**< Current offset in descriptor */
	int current_instraction;/**< Current instruction in descriptor */
	int first_error_pc;     /**< Offset of the first error in descriptor */
	int start_pc;		/**< Start offset in descriptor buffer */
	uint32_t *buffer;	/**< Buffer carrying descriptor */
	uint32_t *shrhdr;	/**< Shared Descriptor Header */
	uint32_t *jobhdr;       /**< Job Descriptor Header */
	uint8_t ps;		/**< Pointer fields size; if ps is set to 1,
				   pointers will be 36bits in length; if ps
				   is set to 0, pointers will be 32bits in
				   length. */
};

static inline void program_cntxt_init(struct program *program, uint32_t *buffer,
				      int offset)
{
	program->current_pc = 0;
	program->current_instraction = 0;
	program->first_error_pc = 0;
	program->start_pc = offset;
	program->buffer = buffer;
	program->shrhdr = NULL;
	program->jobhdr = NULL;
	program->ps = 0;
}

static inline uint32_t program_finalize(struct program *program)
{
	/* Descriptor is not allowed to go beyond 64 words size */
	if (program->current_pc > 0x40)
		pr_debug("Descriptor Size exceeded max limit of 64 words\n");

	/* Descriptor is erroneous */
	if (program->first_error_pc)
		pr_debug("Descriptor creation error\n");

	/* Update descriptor lenght in shared and job descriptor headers */
	if (program->shrhdr != NULL)
		*program->shrhdr |= program->current_pc;

	if (program->jobhdr != NULL)
		*program->jobhdr |= program->current_pc;

	return program->current_pc;
}

static inline uint32_t program_set_36bit_addr(struct program *program)
{
	program->ps = 1;
	return program->current_pc;
}

static inline uint32_t word(struct program *program, uint32_t val)
{
	*(uint32_t *) &program->buffer[program->current_pc] = val;
	return program->current_pc++;
}

static inline uint32_t dword(struct program *program, uint64_t val)
{
	*(uint32_t *) &program->buffer[program->current_pc++] = high_32b(val);
	*(uint32_t *) &program->buffer[program->current_pc++] = low_32b(val);
	return program->current_pc;
}

static inline uint32_t endian_data(struct program *program, uint8_t *data, int length)
{
	int i;

	uint32_t pc = program->current_pc;
	char *tmp = (char *)&program->buffer[program->current_pc];

	for (i = 0; i < length; i++)
		*tmp++ = data[i];
	program->current_pc += (length / 4 + 1);
	return pc;
}

static inline uint32_t set_label(struct program *program)
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


static inline void patch_move(struct program *program, uint32_t line, uint32_t new_ref)
{
	uint32_t opcode = program->buffer[line];
	opcode &= ~MOVE_OFFSET_MASK;
	opcode |= (((int8_t) new_ref) * 4) << MOVE_OFFSET_SHIFT;
	program->buffer[line] = opcode;
}

static inline void patch_jmp(struct program *program, uint32_t line, uint32_t new_ref)
{
	uint32_t opcode = program->buffer[line];
	opcode &= ~JUMP_OFFSET_MASK;
	opcode |= (new_ref - (line + program->start_pc)) & JUMP_OFFSET_MASK;
	program->buffer[line] = opcode;
}

static inline void patch_header(struct program *program, uint32_t line,
			 uint32_t new_ref)
{
	uint32_t opcode = program->buffer[line];
	opcode &= ~HDR_START_IDX_MASK;
	opcode |= (new_ref << HDR_START_IDX_SHIFT) & HDR_START_IDX_MASK;
	program->buffer[line] = opcode;
}

static inline void patch_load(struct program *program, uint32_t line, uint32_t new_ref)
{
	uint32_t opcode = program->buffer[line];
	opcode &= ~LDST_OFFSET_MASK;
	opcode |= (((int8_t) new_ref) * 4) << LDST_OFFSET_SHIFT;
	program->buffer[line] = opcode;
}

static inline int8_t map_opcode(uint32_t name, const uint32_t (*map_table)[2],
		uint8_t num_of_entries, uint32_t *val)
{
	uint8_t i;
	for (i = 0; i < num_of_entries; i++)
		if (map_table[i][0] == name) {
			*val = map_table[i][1];
			return 0;
		}
	return -1;
}

static inline void map_flags(uint32_t flags, const uint32_t (*flags_table)[2],
		uint8_t num_of_entries, uint32_t *opcode)
{
	uint8_t i;
	for (i = 0; i < num_of_entries; i++) {
		if (flags_table[i][0] & flags)
			*opcode |= flags_table[i][1];
	}
}

#endif /* __RTA_SEC_RUN_TIME_ASM_H__ */
