#ifndef __SEC_RTA__
#define __SEC_RTA__

#include "rta/sec_run_time_asm.h"
#include "rta/fifo_load_store_cmd.h"
#include "rta/header_cmd.h"
#include "rta/jump_cmd.h"
#include "rta/key_cmd.h"
#include "rta/load_cmd.h"
#include "rta/math_cmd.h"
#include "rta/move_cmd.h"
#include "rta/nfifo_cmd.h"
#include "rta/operation_cmd.h"
#include "rta/protocol_cmd.h"
#include "rta/seq_in_out_ptr_cmd.h"
#include "rta/signature_cmd.h"
#include "rta/store_cmd.h"

/**
 * @file                 rta.h
 * @brief                RTA API definition.
 */

/**
 * @defgroup rta_api_group RTA API
 * Contains RTA API details.
 *
 * @defgroup program_group Descriptor Buffer Management Routines
 * @ingroup rta_api_group
 * Contains details of RTA descriptor buffer management routines
 * and of routines for SEC Era management.
 *
 * @defgroup cmd_group CAAM Commands Routines
 * @ingroup rta_api_group
 * Contains details of RTA wrapper routines over SEC engine commands.
 *
 * @defgroup refcode_group Self Referential Code Management Routines
 * @ingroup rta_api_group
 * Contains details of RTA self referential code routines.
 */

/**
 * @mainpage
 *
 * Runtime Assembler Library is an easy and flexible runtime method for
 * writing SEC descriptors. Its implements a thin abstraction layer above
 * SEC commands set; the resulting code is compact and similar to a
 * descriptor sequence.
 *
 * RTA library improves comprehension of the SEC code, adds flexibility for
 * writing complex descriptors and keeps the code lightweight. Should be used
 * by whom needs to encode descriptors at runtime, with comprehensible flow
 * control in descriptor.
 *
 * The \subpage userman "User Manual" page contains more details about RTA.
 *
 * \image html rta_arch.png "RTA Integration Overview" width=10cm
 *
 * In SDK package, an example of RTA usage in user space is included. The
 * inclusion of RTA in kernel space in the diagram above is only demonstrative.
 */

/** @addtogroup program_group
 *  @{
 */

/**
 * @def                PROGRAM_CNTXT_INIT
 * @details            @b PROGRAM_CNTXT_INIT must be called before any
 *                     descriptor run-time assembly call type field carry
 *                     info i.e. whether descriptor is shared or Job
 *                     descriptor.
 *
 * @param[in] buffer   Input buffer where the descriptor will be placed
 *                     (@c uint32_t *).
 * @param[in] offset   Offset in input buffer from where the data will be
 *                     written (@c int).
 */
#define PROGRAM_CNTXT_INIT(buffer, offset) \
	program_cntxt_init(program, buffer, offset)

/**
 * @def                PROGRAM_FINALIZE
 * @details            @b PROGRAM_FINALIZE must be called to mark completion
 *                     of @b RTA call.
 */
#define PROGRAM_FINALIZE() program_finalize(program)

/**
 * @def                PROGRAM_SET_36BIT_ADDR
 * @details            @b PROGRAM_SET_36BIT_ADDR must be called to set pointer
 *                     size to 36 bits.
 */
#define PROGRAM_SET_36BIT_ADDR() program_set_36bit_addr(program)

/**
 * @def                WORD
 * @details            @b WORD must be called to insert in descriptor buffer a
 *                     32bits value.
 *
 * @param[in] val      Input value to be written in descriptor buffer
 *                     (@c uint32_t).
 */
#define WORD(val) word(program, val)

/**
 * @def                DWORD
 * @details            @b DWORD must be called to insert in descriptor buffer a
 *                     64bits value.
 *
 * @param[in] val      Input value to be written in descriptor buffer
 *                     (@c uint64_t).
 */
#define DWORD(val) dword(program, val)

/**
 * @def                ENDIAN_DATA
 * @details            @b ENDIAN_DATA must be called to insert in descriptor buffer
 *                     data larger than 64bits.
 *
 * @param[in] data     Input data to be written in descriptor buffer
 *                     (@c uint8_t *).
 * @param[in] len      Length of input data (@c int).
 */
#define ENDIAN_DATA(data, len) endian_data(program, (data), (len))

/**
 * @brief              SEC HW block revision.
 *
 * This *must not be confused with SEC version*:
 * - SEC HW block revision format is "v"
 * - SEC revision format is "x.y"
 */
extern enum rta_sec_era rta_sec_era;

/**
 * @brief              Set SEC Era HW block revision for which the RTA library
 *                     will generate the descriptors.
 * @warning            Must be called *only once*, *before* using any other
 *                     RTA API routine.
 * @warning            *Not thread safe*.
 *
 * @param[in] era      SEC Era (<c> enum rta_sec_era</c>).
 */
static inline void rta_set_sec_era(enum rta_sec_era era)
{
	if (era > MAX_SEC_ERA) {
		rta_sec_era = DEFAULT_SEC_ERA;
		pr_debug("Unsupported SEC ERA. Defaulting to ERA %d\n",
			 DEFAULT_SEC_ERA + 1);
	} else {
		rta_sec_era = era;
	}
}

/**
 * @brief              Get SEC Era HW block revision for which the RTA library
 *                     will generate the descriptors.
 *
 * @return             SEC Era (@c uint).
 */
static inline uint rta_get_sec_era()
{
	 return rta_sec_era;
}

/** @} */ /* end of program_group */


/** @addtogroup cmd_group
 *  @{
 */

/**
 * @def                SHR_HDR
 * @details            Configures Shared Descriptor @b HEADER command
 *
 * @param[in] share    Descriptor share state:@n @li <em>SHR_ALWAYS, SHR_SERIAL,
 *                     SHR_NEVER, SHR_WAIT</em>.
 * @param[in] start_idx Index in descriptor buffer where the execution of the
 *                     Shared Descriptor should start (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li <em>RIF, DNR, CIF, SC, PD</em>.
 * @return             @li Updated descriptor size on success (@c uint32_t).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define SHR_HDR(share, start_idx, flags) \
	shr_header(program, share, start_idx, flags)

/**
 * @def                JOB_HDR
 * @details            Configures JOB Descriptor @b HEADER command.
 *
 * @param[in] share    Descriptor share state:@n @li <em>SHR_ALWAYS,
 *                     SHR_SERIAL, SHR_NEVER, SHR_WAIT, SHR_DEFER</em>.
 * @param[in] share_desc Pointer to shared descriptor, in case @em SHR bit is
 *                     set (@c uint64_t).
 * @param[in] flags    Operational flags:@n @li <em>RSMS, DNR, TD, MTD, REO,
 *                     SHR</em>.
 * @return             @li Updated descriptor size on success (@c uint32_t).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define JOB_HDR(share, share_desc, flags) \
	job_header(program, share, share_desc, flags)

/**
 * @def                MOVE
 * @details            Configures @b MOVE and @b MOVE_LEN commands.
 *
 * @param[in] src      Internal source of data that will be moved:@n @li
 *                     <em>CONTEXT1, CONTEXT2, OFIFO, DESCBUF, MATH0-MATH3,
 *                     IFIFOABD, IFIFOAB1, IFIFOAB2, AB1, AB2, ABD</em>.
 * @param[in] src_offset Offset in source data (@c uint16_t).
 * @param[in] dst      Internal destination of data that will be moved:@n @li
 *                     <em>CONTEXT1, CONTEXT2, OFIFO, DESCBUF, MATH0-MATH3,
 *                     IFIFOAB1, IFIFOAB2, IFIFO, PKA, KEY1, KEY2,
 *                     ALTSOURCE</em>.
 * @param[in] dst_offset Offset in destination data (@c uint16_t).
 * @param[in] length   Size of data to be moved:@n @li for @b MOVE should be
 *                     specified using @b IMM macro; @li for @b MOVE_LEN
 *                     should be specified using @e MATH0-MATH3.
 * @param[in] opt      Operational flags:@n @li <em>WAITCOMP, FLUSH1, FLUSH2,
 *                     LAST1, LAST2</em>.
 * @return             @li Updated descriptor size on success (@c uint32_t).
 *                     @li First error program counter will be incremented on error;
 *                     In debug mode, a log message will be shown at output.
 */
#define MOVE(src, src_offset, dst, dst_offset, length, opt) \
	move(program, src, src_offset, dst, dst_offset, length, opt)

/**
 * @def                FIFOLOAD
 * @details            Configures SEC @b FIFOLOAD command to load message data,
 *                     PKHA data, IV, ICV, AAD and bit length message data into
 *                     Input Data FIFO.
 *
 * @param[in] data     Input data type to store:@n @li <em>PKHA registers,
 *                     IFIFO, MSG1, MSG2, MSGOUTSNOOP, MSGINSNOOP, IV1, IV2,
 *                     AAD1, ICV1, ICV2, BIT_DATA, SKIP</em>.
 * @param[in] src      Pointer or actual data in case of immediate load; @b IMM
 *                     or @b PTR macros must be used to indicate type.
 * @param[in] length   Number of bytes to load (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li <em>SGF, IMMED, EXT, CLASS1,
 *                     CLASS2, BOTH, FLUSH1, LAST1, LAST2</em>.
 * @return             @li Updated descriptor size on success (@c uint32_t).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define FIFOLOAD(data, src, length, flags) \
	fifo_load(program, data, src, length, flags)

/**
 * @def                SEQFIFOLOAD
 * @details            Configures SEC <b>SEQ FIFOLOAD</b> command to load
 *                     message data, PKHA data, IV, ICV, AAD and bit length
 *                     message data into Input Data FIFO.
 *
 * @param[in] data     Input data type to store:@n @li <em>PKHA registers,
 *                     IFIFO, MSG1, MSG2, MSGOUTSNOOP, MSGINSNOOP, IV1, IV2,
 *                     AAD1, ICV1, ICV2, BIT_DATA, SKIP</em>.
 * @param[in] length   Number of bytes to load; can be set to 0 for SEQ command
 *                     w/ @e VLF set (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li <em>VLF, CLASS1, CLASS2, BOTH,
 *                     FLUSH1, LAST1, LAST2</em>.
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define SEQFIFOLOAD(data, length, flags) \
	fifo_load(program, data, NONE, length, WITH(flags|SEQ))

/**
 * @def                FIFOSTORE
 * @details            Configures SEC @b FIFOSTORE command, to move data from
 *                     Output Data FIFO to external memory via DMA.
 *
 * @param[in] data     Output data type to store:@n @li <em>PKHA registers,
 *                     IFIFO, OFIFO, RNG, RNGOFIFO, AFHA_SBOX, MDHA_SPLIT_KEY,
 *                     MSG, KEY1, KEY2, SKIP</em>.
 * @param[in] encrypt_flags  Store data encryption mode:@n @li <em>EKT, NRM,
 *                     TK, JDK</em>.
 * @param[in] dst      Pointer to store location (@c uint64_t).
 * @param[in] length   Number of bytes to load (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li <em>SGF, CONT, EXT, CLASS1,
 *                     CLASS2, BOTH</em>.
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define FIFOSTORE(data, encrypt_flags, dst, length, flags) \
	fifo_store(program, data, encrypt_flags, dst, length, flags)

/**
 * @def                SEQFIFOSTORE
 * @details            Configures SEC <b>SEQ FIFOSTORE</b> command, to move
 *                     data from Output Data FIFO to external memory via DMA.
 *
 * @param[in] data     Output data type to store:@n @li <em>PKHA registers,
 *                     IFIFO, OFIFO, RNG, RNGOFIFO, AFHA_SBOX, MDHA_SPLIT_KEY,
 *                     MSG, KEY1, KEY2, SKIP</em>.
 * @param[in] encrypt_flags  Store data encryption mode:@n @li <em>EKT, NRM,
 *                     TK, JDK</em>.
 * @param[in] length   Number of bytes to load; can be set to 0 for SEQ command
 *                     w/ @e VLF set (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li <em>VLF, CONT, EXT, CLASS1,
 *                     CLASS2, BOTH</em>.
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define SEQFIFOSTORE(data, encrypt_flags, length, flags) \
	fifo_store(program, data, encrypt_flags, 0, length, WITH(flags|SEQ))

/**
 * @def                KEY
 * @details            Configures SEC @b KEY and <b>SEQ KEY</b> commands.
 *
 * @param[in] key_dst  Key store location:@n @li <em>KEY1, KEY2, PKE,
 *                     AFHA_SBOX, MDHA_SPLIT_KEY</em>.
 * @param[in] encrypt_flags  Key encryption mode:@n @li <em>ENC, EKT, TK,
 *                     NWB</em>.
 * @param[in] src      Pointer or actual data in case of immediate load
 *                     (@c uint64_t).
 * @param[in] length   Number of bytes to load (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li <em>SGF, IMMED, SEQ</em>.
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define KEY(key_dst, encrypt_flags, src, length, flags) \
	key(program, key_dst, encrypt_flags, src, length, flags)

/**
 * @def                SEQINPTR
 * @details            Configures SEC <b>SEQ IN PTR</b> command.
 *
 * @param[in] src      Starting address for Input Sequence (@c uintptr_t).
 * @param[in] length   Number of bytes in (or to be added to) Input Sequence
 *                     (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li <em>RBS, INL, SGF, PRE, EXT,
 *                     RTO, RJD</em> (when @e PRE or @e RTO are set, @e src
 *                     parameter must be 0).
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define SEQINPTR(src, length, flags)   seq_in_ptr(program, src, length, flags)

/**
 * @def                SEQOUTPTR
 * @details            Configures SEC <b>SEQ OUT PTR</b> command.
 *
 * @param[in] dst      Starting address for Output Sequence (@c uintptr_t).
 * @param[in] length   Number of bytes in (or to be added to) Output Sequence
 *                     (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li <em>SGF, PRE, EXT, RTO</em>.
 *                     (when @e PRE or @e RTO are set, @e dst parameter must
 *                     be 0).
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define SEQOUTPTR(dst, length, flags)  seq_out_ptr(program, dst, length, flags)

/**
 * @def                  ALG_OPERATION
 * @details              Configures <b>ALGORITHM OPERATION</b> command.
 *
 * @param[in] cipher_alg Algorithm to be used.
 * @param[in] aai        Additional algorithm information; contains mode
 *                       information that is associated with the algorithm
 *                       (check desc.h for specific values).
 * @param[in] algo_state Algorithm state; defines the state of the algorithm
 *                       that is being executed (check desc.h file for specific
 *                       values).
 * @param[in] icv_check  ICV checking; selects whether the algorithm should
 *                       check calculated ICV with known ICV:@n @li
 *                       <em>ICV_CHECK_ENABLE, ICV_CHECK_DISABLE</em>.
 * @param[in] enc        Selects between encryption and decryption:@n @li
 *                       <em>OP_ALG_ENCRYPT, OP_ALG_DECRYPT</em>.
 * @return               @li Updated descriptor size on success.
 *                       @li First error program counter will be incremented on
 *                       error; in debug mode, a log message will be shown at
 *                       output.
 */
#define ALG_OPERATION(cipher_alg, aai, algo_state, icv_check, enc) \
	operation(program, cipher_alg, aai, algo_state, icv_check, enc)

/**
 * @def                PROTOCOL
 * @details            Configures <b>PROTOCOL OPERATION</b> command.
 *
 * @param[in] optype   Operation type:@n @li <em>OP_TYPE_UNI_PROTOCOL/
 *                     OP_TYPE_DECAP_PROTOCOL/OP_TYPE_ENCAP_PROTOCOL</em>.
 * @param[in] protid   Protocol identifier value (check desc.h file for
 *                     specific values).
 * @param[in] protoinfo  Protocol dependent value (check desc.h file for
 *                     specific values).
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define PROTOCOL(optype, protid, protoinfo) \
	proto_operation(program, optype, protid, protoinfo)

/**
 * @def                PKHA_OPERATION
 * @details            Configures <b>PKHA OPERATION</b> command.
 *
 * @param[in] op_pkha  PKHA operation; indicates the modular arithmetic
 *                     function to execute (check desc.h file for specific
 *                     values).
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define PKHA_OPERATION(op_pkha)   pkha_operation(program, op_pkha)

/**
 * @def                JUMP
 * @details            Configures @b JUMP command.
 *
 * @param[in] addr     Local offset for local jumps or address pointer for
 *                     non-local jumps; @b IMM or @b PTR macros must be used to
 *                     indicate type.
 * @param[in] jump_type Type of action taken by jump:@n @li <em>LOCAL_JUMP,
 *                     GOSUB, RETURN, HALT, HALT_STATUS, FAR_JUMP</em>.
 * @param[in] test_type Defines how jump conditions are evaluated:@n @li
 *                     <em>ALL_TRUE, ALL_FALSE, ANY_TRUE, ANY_FALSE</em>.
 * @param[in] cond     Jump conditions:@n @li operational flags <em>DONE1,
 *                     DONE2, BOTH</em>.
 *                     @li various sharing and wait conditions (@e JSL = 1):
 *                     <em>NIFP, NIP, NOP, NCP, CALM, SELF, SHARED, JQP</em>.
 *                     @li Math and PKHA status conditions (@e JSL = 0):
 *                     <em>Z, N, NV, C, PK0, PK1, PKP</em>.
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define JUMP(addr, jump_type, test_type, cond) \
	jump(program, addr, jump_type, test_type, cond)

/**
 * @def                LOAD
 * @details            Configures SEC @b LOAD command to load data registers
 *                     from descriptor or from a memory location.
 *
 * @param[in] addr     Immediate value or pointer to the data to be loaded;
 *                     @b IMM or @b PTR macros must be used to indicate type.
 * @param[in] dst      Destination register (@c uint64_t).
 * @param[in] offset   Start point to write data in destination register
 *                     (@c uint32_t).
 * @param[in] length   Number of bytes to load (@c uint32_t).
 * @param[in] flags    Operational flags: @e VLF.
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define LOAD(addr, dst, offset, length, flags) \
	load(program, addr, dst, offset, length, flags)

/**
 * @def                SEQLOAD
 * @details            Configures SEC <b>SEQ LOAD</b> command to load data
 *                     registers from descriptor or from a memory location.
 *
 * @param[in] dst      Destination register (@c uint64_t).
 * @param[in] offset   Start point to write data in destination register
 *                     (@c uint32_t).
 * @param[in] length   Number of bytes to load (@c uint32_t).
 * @param[in] flags    Operational flags: @e SGF.
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define SEQLOAD(dst, offset, length, flags) \
	load(program, NONE, dst, offset, length, WITH(flags|SEQ))

/**
 * @def                STORE
 * @details            Configures SEC @b STORE command to read data from
 *                     registers and write them to a memory location.
 *
 * @param[in] src      Immediate value or source register for data to be
 *                     stored:@n @li <em>KEY1SZ, KEY2SZ, DJQDA, MODE1, MODE2,
 *                     DJQCTRL, DATA1SZ, DATA2SZ, DSTAT, ICV1SZ, ICV2SZ, DPID,
 *                     CCTRL, ICTRL, CLRW, CSTAT, MATH0-MATH3, PKHA registers,
 *                     CONTEXT1, CONTEXT2, DESCBUF, JOBDESCBUF,
 *                     SHAREDESCBUF</em>.
 *                     @b IMM must be used to indicate immediate value.
 * @param[in] offset   Start point for reading from source register
 *                     (@c uint16_t).
 * @param[in] dst      Pointer to store location; @b PTR must be used to
 *                     indicate pointer value (@c uint64_t).
 * @param[in] length   Number of bytes to store (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li <em>VLF, IMM</em>.
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define STORE(src, offset, dst, length, flags) \
	store(program, src, offset, dst, length, flags)

/**
 * @def                SEQSTORE
 * @details            Configures SEC <b>SEQ STORE</b> command to read data
 *                     from registers and write them to a memory location.
 *
 * @param[in] src      Immediate value or source register for data to be
 *                     stored:@n @li <em>KEY1SZ, KEY2SZ, DJQDA, MODE1, MODE2,
 *                     DJQCTRL, DATA1SZ, DATA2SZ, DSTAT, ICV1SZ, ICV2SZ, DPID,
 *                     CCTRL, ICTRL, CLRW, CSTAT, MATH0-MATH3, PKHA registers,
 *                     CONTEXT1, CONTEXT2, DESCBUF, JOBDESCBUF,
 *                     SHAREDESCBUF</em>.
 *                     @n @b IMM must be used to indicate immediate value.
 * @param[in] offset   Start point for reading from source register
 *                     (@c uint16_t).
 * @param[in] length   Number of bytes to store (@c uint32_t).
 * @param[in] flags    Operational flags: @e SGF.
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define SEQSTORE(src, offset, length, flags) \
	store(program, src, offset, NONE, length, WITH(flags|SEQ))

/**
 * @def                MATHB
 * @details            Configures SEC @b MATH command to perform binary
 *                     operations.
 *
 * @param[in] operand1 First operand:@n @li <em>MATH0-MATH3, DPOVRD, SEQINSZ,
 *                     SEQOUTSZ, VSEQINSZ, VSEQOUTSZ, ZERO, ONE, NONE,
 *                     Immediate value</em>.
 *                     @n @b IMM must be used to indicate immediate value.
 * @param[in] operator Function to be performed:@n @li <em>ADD, ADDC, SUB,
 *                     SUBB, OR, AND, XOR, LSHIFT, RSHIFT, SHLD</em>.
 * @param[in] operand2 Second operand:@n @li <em>MATH0-MATH3, DPOVRD, VSEQINSZ,
 *                     VSEQOUTSZ, ABD, OFIFO, JOBSRC, ZERO, ONE, SEQINSZ,
 *                     Immediate value</em>.@n @b IMM must be used to indicate
 *                     immediate value.
 * @param[in] result   Destination for the result:@n @li <em>MATH0-MATH3,
 *                     DPOVRD, SEQINSZ, SEQOUTSZ, NONE, VSEQINSZ,
 *                     VSEQOUTSZ</em>.
 * @param[in] length   Length in bytes of the operation and the immediate
 *                     value, if there is one (@c int).
 * @param[in] opt      Operational flags: @e SGF.
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define MATHB(operand1, operator, operand2, result, length, opt) \
	math(program, operand1, operator, operand2, result, length, opt)

/**
 * @def                MATHU
 * @details            Configures SEC @b MATH command to perform unary
 *                     operations.
 *
 * @param[in] operand1 Operand:@n @li <em>MATH0-MATH3, DPOVRD, SEQINSZ,
 *                     SEQOUTSZ, VSEQINSZ, VSEQOUTSZ, ZERO, ONE, NONE,
 *                     Immediate value</em>. @n @b IMM must be used to
 *                     indicate immediate value.
 * @param[in] operator Function to be performed:@n @li <em>ZBYTE, BSWAP</em>.
 * @param[in] result   Destination for the result:@n @li <em>MATH0-MATH3,
 *                     DPOVRD, SEQINSZ, SEQOUTSZ, NONE, VSEQINSZ,
 *                     VSEQOUTSZ</em>.
 * @param[in] length   Length in bytes of the operation and the immediate
 *                     value, if there is one (@c int).
 * @param[in] opt      Operational flags: @e SGF.
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define MATHU(operand1, operator, result, length, opt) \
	math(program, operand1, operator, _NONE, 0, result, length, opt)

/**
 * @def                SIGNATURE
 * @details            Configures @b SIGNATURE command.
 *
 * @param[in] sign_type Signature type:@n @li <em>SIGN_TYPE_FINAL,
 *                     SIGN_TYPE_FINAL_RESTORE, SIGN_TYPE_FINAL_NONZERO,
 *                     SIGN_TYPE_IMM_2, SIGN_TYPE_IMM_3, SIGN_TYPE_IMM_4</em>.
 *                     @n After @b SIGNATURE command, @b DWORD or
 *                     @b WORD must be used to insert signature in descriptor
 *                     buffer.
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define SIGNATURE(sign_type)   signature(program, sign_type)

/**
 * @def                NFIFOADD
 * @details            Configures @b NFIFO command, a shortcut of RTA Load
 *                     command to write to iNfo FIFO.
 *
 * @param[in] src      Source for the input data in Alignment Block:@n @li
 *                     <em>IFIFO, OFIFO, PAD, MSGOUTSNOOP, ALTSOURCE</em>.
 * @param[in] data     Type of data that is going through the Input Data FIFO:
 *                     @n @li <em>MSG, MSG1, MSG2, IV1, IV2, ICV1, ICV2, SAD1,
 *                     AAD1, AAD2, AFHA_SBOX, SKIP, PKHA registers, AB1, AB2,
 *                     ABD</em>.
 * @param[in] length   Length of the data copied in FIFO registers
 *                     (@c uint32_t).
 * @param[in] flags    Select options between:@n @li operational flags:
 *                     <em>LAST1, LAST2, FLUSH1, FLUSH2, OC, BP</em>.
 *                     @li when PAD is selected as source: <em>BM, PR, PS</em>.
 *		       @li padding type: <em>PAD_ZERO, PAD_NONZERO,
 *		       PAD_INCREMENT, PAD_RANDOM, PAD_ZERO_N1, PAD_NONZERO_0,
 *		       PAD_N1, PAD_NONZERO_N</em>.
 * @return             @li Updated descriptor size on success.
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define NFIFOADD(src, data, length, flags) \
	nfifo_load(program, src, data, length, flags)

/** @} */ /* end of cmd_group */

/** @addtogroup refcode_group
 *  @{
 */

/**
 * @def                REFERENCE
 * @details            @b REFERENCE initialize a variable used for storing an
 *                     index inside a descriptor buffer.
 *
 * @param[in] ref      Reference to a descriptor buffer's index where an update
 *                     is required with a value that will be known latter in
 *                     the program flow.
 */
#define REFERENCE(ref)    uint32_t ref = 0xFFFFFFFF;

/**
 * @def                LABEL
 * @details            @b LABEL initialize a variable used for storing an index
 *                     inside a descriptor buffer.
 *
 * @param[in] label    Label stores the value with what should be updated the
 *                     REFERENCE line in the descriptor buffer.
 */
#define LABEL(label)      uint32_t label = 0;

/**
 * @def                SET_LABEL
 * @details            @b SET_LABEL routine sets a @b LABEL value.
 *
 * @param[in] label    Value that will be inserted in a line previously written
 *                     in the descriptor buffer.
 */
#define SET_LABEL(label)  label = set_label(program)

/**
 * @def                PATCH_JUMP
 * @details            Auxiliary command to resolve self referential code.
 *
 * @param[in] program  Buffer to be updated (<c>struct program *</c>).
 * @param[in] line     Position in descriptor buffer where the update will be
 *                     done; this value is previously retained in program flow
 *                     using a reference near the sequence to be modified.
 * @param[in] new_ref  Updated value that will be inserted in descriptor
 *                     buffer at the specified line; this value is previously
 *                     obtained using @b SET_LABEL macro near the line that
 *                     will be used as reference.
 *                     For @b JUMP command, the value represents the offset
 *                     field.
 */
#define PATCH_JUMP(program, line, new_ref) patch_jmp(program, line, new_ref)

/**
 * @def                PATCH_MOVE
 * @details            Auxiliary command to resolve self referential code.
 *
 * @param[in] program  Buffer to be updated (<c>struct program *</c>).
 * @param[in] line     Position in descriptor buffer where the update will be
 *                     done; this value is previously retained in program flow
 *                     using a reference near the sequence to be modified
 *                     (@c uint32_t).
 * @param[in] new_ref  Updated value that will be inserted in descriptor
 *                     buffer at the specified line; this value is previously
 *                     obtained using @b SET_LABEL macro near the line that
 *                     will be used as reference (@c uint32_t).
 *                     For @b MOVE command, the value represents the offset
 *                     field.
 */
#define PATCH_MOVE(program, line, new_ref) patch_move(program, line, new_ref)

/**
 * @def                PATCH_LOAD
 * @details            Auxiliary command to resolve self referential code.
 *
 * @param[in] program  Buffer to be updated (<c>struct program *</c>).
 * @param[in] line     Position in descriptor buffer where the update will be
 *                     done; this value is previously retained in program flow
 *                     using a reference near the sequence to be modified
 *                     (@c uint32_t).
 * @param[in] new_ref  Updated value that will be inserted in descriptor
 *                     buffer at the specified line; this value is previously
 *                     obtained using @b SET_LABEL macro near the line that
 *                     will be used as reference (@c uint32_t).
 *                     For @b LOAD command, the value represents the offset
 *                     field.
 */
#define PATCH_LOAD(program, line, new_ref) patch_load(program, line, new_ref)

/**
 * @def                PATCH_HDR
 * @details            Auxiliary command to resolve self referential code.
 *
 * @param[in] program  Buffer to be updated (<c>struct program *</c>).
 * @param[in] line     Position in descriptor buffer where the update will be
 *                     done; this value is previously retained in program flow
 *                     using a reference near the sequence to be modified
 *                     (@c uint32_t).
 * @param[in] new_ref  Updated value that will be inserted in descriptor
 *                     buffer at the specified line; this value is previously
 *                     obtained using @b SET_LABEL macro near the line that
 *                     will be used as reference (@c uint32_t).
 *                     For @b HEADER command, the value represents the start
 *                     index field.
 */
#define PATCH_HDR(program, line, new_ref) patch_header(program, line, new_ref)

/** @} */ /* end of refcode_group */


#endif
