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


/*
 * PROGRAM_CNTXT_INIT must be called before any descriptor run-time assembly
 * call type field carry info i.e. whether descriptor is shared or
 * Job descriptor
 */
#define PROGRAM_CNTXT_INIT(buffer, offset, sec_era) \
	program_cntxt_init(program, buffer, offset, sec_era)

/*
 * PROGRAM_FINALIZE must be called to mark completion of run-time assembly call
 */
#define PROGRAM_FINALIZE() program_finalize(program)

/* PROGRAM_SET_36BIT_ADDR must be called to set pointer size to 36 bits */
#define PROGRAM_SET_36BIT_ADDR() program_set_36bit_addr(program)

/********* Data directives **********/
#define WORD(val) word(program, val)
#define DWORD(val) dword(program, val)
#define ENDIAN_DATA(data, len) endian_data(program, (data), (len))

/*
 * Configures Shared Descriptor HEADER command
 *
 * @param[in] share    Descriptor share state: [SHR_ALWAYS, SHR_SERIAL,
 *                     SHR_NEVER, SHR_WAIT];
 * @param[in] start_idx Index of the word in descriptor buffer where the
 *                     execution of the Shared Descriptor should start.
 * @param[in] flags    Operational flags: [RIF, DNR, CIF, HDR_SAVECTX, PD];
 * @retval             Updated descriptor size on success.
 */
#define SHR_HDR(share, start_idx, flags) \
	shr_header(program, share, start_idx, flags)

/*
 * Configures JOB Descriptor HEADER command
 *
 * @param[in] share    Descriptor share state: [SHR_ALWAYS, SHR_SERIAL,
 *                     SHR_NEVER, SHR_WAIT, SHR_DEFER];
 * @param[in] share_desc Pointer to shared descriptor, in case SHR bit is set;
 * @param[in] flags    Operational flags: [RSMS, DNR, TD, MTD, REO, SHR];
 * @retval             Updated descriptor size on succes.
 */
#define JOB_HDR(share, share_desc, flags) \
	job_header(program, share, share_desc, flags)

/*
 * Configures MOVE and MOVE_LEN commands
 *
 * @param[in] src      Internal source of data that will be moved: [CONTEXT1,
 *                     CONTEXT2, OFIFO, DESCBUF, MATH0-MATH3, IFIFOABD,
 *                     IFIFOAB1, IFIFOAB2, AB1, AB2, ABD];
 * @param[in] src_offset Offset in source data;
 * @param[in] dst      Internal destination of data that will be moved:
 *                     [CONTEXT1, CONTEXT2, OFIFO, DESCBUF, MATH0-MATH3,
 *                     IFIFOAB1, IFIFOAB2, IFIFO, PKA, KEY1, KEY2, ALTSOURCE];
 * @param[in] dst_offset Offset in destination data;
 * @param[in] length   Size of data to be moved;
 *                     @Note: for MOVE should be specified using IMM(size);
 *                     @Note: for MOVE_LEN should be specified using
 *                     MATH0-MATH3;
 * @param[in] opt      Operational flags: [WAITCOMP, FLUSH1, FLUSH2, LAST1,
 *                     LAST2];
 * @retval             Updated descriptor size on success.
 */
#define MOVE(src, src_offset, dst, dst_offset, length, opt) \
	move(program, src, src_offset, dst, dst_offset, length, opt)

/*
 * Configures SEC FIFOLOAD command to load message data,
 * PKHA data, IV, ICV, AAD and bit length message data into Input Data FIFO.
 *
 * @param[in] data     Input data type to store: [PKHA regs, IFIFO, MSG1, MSG2,
 *                     MSGOUTSNOOP, MSGINSNOOP, IV1, IV2, AAD1, ICV1, ICV2,
 *                     BIT_DATA, SKIP];
 * @param[in] src      Pointer or actual data in case of immidiate load;
 *                     @Note: IMM or PTR macros must be used to indicate type;
 * @param[in] length   Number of bytes to load.
 * @param[in] flags    Operational flags: [SGF, IMMED, EXT, DONE1, DONE2,
 *                     DONEBOTH, FLUSH1, LAST1, LAST2];
 * @retval             Updated descriptor size on succes.
 */
#define FIFOLOAD(data, src, length, flags) \
	fifo_load(program, data, src, length, flags)

/*
 * Configures SEC SEQ FIFOLOAD command to load message data,
 * PKHA data, IV, ICV, AAD and bit length message data into Input Data FIFO.
 *
 * @param[in] data     Input data type to store: [PKHA regs, IFIFO, MSG1, MSG2,
 *                     MSGOUTSNOOP, MSGINSNOOP, IV1, IV2, AAD1, ICV1, ICV2,
 *                     BIT_DATA, SKIP];
 * @param[in] length   Number of bytes to load.
 *                     @Note: Can be set to 0 for seq cmd w/ VLF set;
 *                     @Note: SEQFIFOLOAD(SKIP, length, flags)
 * @param[in] flags    Operational flags: [VLF, DONE1, DONE2, DONEBOTH, FLUSH1,
 *                     LAST1, LAST2];
 * @retval             Updated descriptor size on succes.
 */
#define SEQFIFOLOAD(data, length, flags) \
	fifo_load(program, data, NONE, length, WITH(flags|SEQ))

/*
 * Configures SEC FIFOSTORE command, to move data from Output Data FIFO to
 * external memory via DMA
 *
 * @param[in] data     Output data type to store: [PKHA regs, IFIFO, OFIFO,
 *                     RNG, RNGOFIFO, AFHA_SBOX, MDHA_SPLIT_KEY, MSG, KEY1,
 *                     KEY2, SKIP];
 * @param[in] encrypt_flags  Store data encryption mode: [EKT, NRM, TK, JDK]
 * @param[in] dst      Pointer to store location;
 * @param[in] length   Number of bytes to load.
 * @param[in] flags    Operational flags: Operational flags: [SGF, CONT, EXT];
 * @retval             Updated descriptor size on succes.
 */
#define FIFOSTORE(data, encrypt_flags, dst, length, flags) \
	fifo_store(program, data, encrypt_flags, dst, length, flags)

/*
 * Configures SEC SEQ FIFOSTORE command, to move data from Output Data FIFO
 * to external memory via DMA
 *
 * @param[in] data     Output data type to store: [PKHA regs, IFIFO, OFIFO,
 *                     RNG, RNGOFIFO, AFHA_SBOX, MDHA_SPLIT_KEY, MSG, KEY1,
 *                     KEY2, SKIP];
 * @param[in] encrypt_flags  Store data encryption mode: [EKT, NRM, TK, JDK]
 * @param[in] length   Number of bytes to load.
 *                     @Note: Can be set to 0 for seq cmd w/ VLF set;
 *                     @Note: SEQFIFOSTORE(SKIP, 0, length, flags)
 * @param[in] flags    Operational flags: Operational flags: [VLF, CONT, EXT];
 * @retval             Updated descriptor size on succes.
 */
#define SEQFIFOSTORE(data, encrypt_flags, length, flags) \
	fifo_store(program, data, encrypt_flags, 0, length, WITH(flags|SEQ))

/*
 * Configures SEC KEY and SEQ KEY commands
 *
 * @param[in] key_dst  Key store location: [KEY1, KEY2, PKE, AFHA_SBOX,
 *                     MDHA_SPLIT_KEY];
 * @param[in] encrypt_flags  Key encryption mode: [ENC, EKT, TK, NWB]
 * @param[in] src      Pointer or actual data in case of immidiate load;
 * @param[in] length   Number of bytes to load.
 * @param[in] flags    Operational flags: [SGF, IMMED, SEQ];
 * @retval             Updated descriptor size on succes.
 */
#define KEY(key_dst, encrypt_flags, src, length, flags) \
	key(program, key_dst, encrypt_flags, src, length, flags)

/*
 * Configures SEC SEQ IN PTR command
 *
 * @param[in] src      Starting address for Input Sequence
 * @param[in] length   Number of bytes in (or to be added to) Input Sequence
 * @param[in] flags    Operational flags: [RBS, INL, SGF, PRE, EXT, RTO, RJD];
 *                     @Note: When PRE or RTO are set, src must be 0.
 * @retval             Updated descriptor size on succes.
 */
#define SEQINPTR(src, length, flags)   seq_in_ptr(program, src, length, flags)

/*
 * Configures SEC SEQ OUT PTR command
 *
 * @param[in] dst      Starting address for Output Sequence
 * @param[in] length   Number of bytes in (or to be added to) Output Sequence
 * @param[in] flags    Operational flags: [SGF, PRE, EXT, RTO];
 *                     @Note: When PRE or RTO are set, src must be 0.
 * @retval             Updated descriptor size on succes.
 */
#define SEQOUTPTR(dst, length, flags)  seq_out_ptr(program, dst, length, flags)

/*
 * Configures ALGORITHM OPERATION command
 *
 * @param[in] cipher_alg Algorithm to be used for the operations.
 * @param[in] aai        Additional algorithm information; contains mode
 *                       information that is associated with the algorithm;
 *                       check desc.h file for specific values.
 * @param[in] algo_state Algorithm state; defines the state of the algorithm
 *                       that is being executed; check desc.h file for specific
 *                       values.
 * @param[in] icv_check  ICV checking; selects whether the algorithm should
 *                       check calculated ICV with known ICV.
 * @param[in] enc        Encrypt/Decrypt; select between encryption and
 *                       decryption.
 * @retval               Updated descriptor size on succes.
 */
#define ALG_OPERATION(cipher_alg, aai, algo_state, icv_check, enc) \
	operation(program, cipher_alg, aai, algo_state, icv_check, enc)

/*
 * Configures PROTOCOL OPERATION command
 *
 * @param[in] optype
 *    OP_TYPE_UNI_PROTOCOL/OP_TYPE_DECAP_PROTOCOL/OP_TYPE_ENCAP_PROTOCOL
 * @param[in] protid    Protocol identifier
 * OP_TYPE_UNI_PROTOCOL:
 *    OP_PCLID_SSL30_PRF      OP_PCLID_IKEV1_PRF        OP_PCLID_DSAVERIFY
 *    OP_PCLID_TLS10_PRF      OP_PCLID_RSADECRYPT       OP_PCLID_DIFFIEHELLMAN
 *    OP_PCLID_TLS11_PRF      OP_PCLID_PUBLICKEYPAIR    OP_PCLID_DSASIGN
 *    OP_PCLID_DTLS10_PRF     OP_PCLID_IKEV2_PRF        OP_PCLID_RSAENCRYPT
 *    OP_PCLID_TLS12_PRF
 *
 * OP_TYPE_DECAP_PROTOCOL / OP_TYPE_ENCAP_PROTOCOL
 *    OP_PCLID_SSL30          OP_PCLID_MACSEC           OP_PCLID_IPSEC
 *    OP_PCLID_TLS10          OP_PCLID_WIFI             OP_PCLID_3G_RLC_SDU
 *    OP_PCLID_TLS11          OP_PCLID_WIMAX            OP_PCLID_LTE_PDCP_USER
 *    OP_PCLID_TLS12          OP_PCLID_3G_DCRC          OP_PCLID_DTLS10
 *    OP_PCLID_SRTP           OP_PCLID_3G_RLC_PDU
 *    OP_PCLID_BLOB           OP_PCLID_LTE_PDCP_CTRL OP_PCLID_SRTP
 *
 * @param[in] protoinfo Protocol dependent value; check desc.h file for
 *                      specific values.
 * @retval              Updated descriptor size on succes.
 */

#define PROTOCOL(optype, protid, protoinfo) \
	proto_operation(program, optype, protid, protoinfo)

/*
 * Configures PKHA OPERATION command
 *
 * @param[in] op_pkha    PKHA operation; indicates what modular arithmetic
 *                       function to execute; check desc.h file for specific
 *                       values;
 * @retval               Updated descriptor size on succes.
 */
#define PKHA_OPERATION(op_pkha)   pkha_operation(program, op_pkha)

/*
 * Configures JUMP command
 *
 * @param[in] addr       Local offset for local jumps or address pointer for
 *                       non-local jumps;
 *                       @Note: IMM or PTR macros must be used to indicate
 *                       type;
 * @param[in] jump_type  Type of action taken by jump: [LOCAL_JUMP, GOSUB,
 *                       RETURN, HALT, HALT_STATUS, FAR_JUMP];
 * @param[in] test_type  Defines how jump conditions are evaluated: [ALL_TRUE,
 *                       ALL_FALSE, ANY_TRUE, ANY_FALSE];
 * @param[in] cond       Jump conditions: [DONE1, DONE2, BOTH];
 *                       Various sharing and wait conditions: [NIFP, NIP, NOP,
 *                       NCP, CALM, SELF, SHARED, JQP]; @Note: JSL = 1;
 *                       Math and PKHA status conditions: [Z, N, NV, C, PK0,
 *                       PK1, PKP]; @Note: JSL = 0;
 * @retval               Updated descriptor size on succes.
 */
#define JUMP(addr, jump_type, test_type, cond) \
	jump(program, addr, jump_type, test_type, cond)

/*
 * Configures SEC LOAD command to load data registers from descriptor or
 * from a memory location
 *
 * @param[in] addr     Immidiate value or pointer to the data to be loaded;
 *                     @Note: IMM or PTR macros must be used to indicate type;
 * @param[in] dst      Destination register;
 * @param[in] offset   Start point to write data in destination register;
 * @param[in] length   Number of bytes to load.
 * @param[in] flags    Operational flags: Operational flags: [VLF];
 * @retval             Updated descriptor size on succes.
 */
#define LOAD(addr, dst, offset, length, flags) \
	load(program, addr, dst, offset, length, flags)

/*
 * Configures SEC SEQ LOAD command to load data registers from descriptor or
 * from a memory location
 *
 * @param[in] dst      Destination register;
 * @param[in] offset   Start point to write data in destination register;
 * @param[in] length   Number of bytes to load.
 * @param[in] flags    Operational flags: Operational flags: [SGF];
 * @retval             Updated descriptor size on succes.
 */
#define SEQLOAD(dst, offset, length, flags) \
	load(program, NONE, dst, offset, length, WITH(flags|SEQ))

/*
 * Configures SEC STORE command to read data from registers and write them to
 * a memory location
 *
 * @param[in] src      Immidiate value or source register for data to be
 *                     stored: [KEY1SZ, KEY2SZ, DJQDA, MODE1, MODE2, DJQCTRL,
 *                     DATA1SZ, DATA2SZ, DSTAT, ICV1SZ, ICV2SZ, DPID, CCTRL,
 *                     ICTRL, CLRW, CSTAT, MATH0-MATH3, PKHA regs, CONTEXT1,
 *                     CONTEXT2, DESCBUF, JOBDESCBUF, SHAREDESCBUF];
 *                     @Note: IMM must be used to indicate immidiate value;
 * @param[in] offset   Start point for reading from source register;
 * @param[in] dst      Pointer to store location;
 *                     @Note: PTR must be used to indicate pointer value;
 * @param[in] length   Number of bytes to store.
 * @param[in] flags    Operational flags: Operational flags: [VLF|IMM];
 * @retval             Updated descriptor size on succes.
 */
#define STORE(src, offset, dst, length, flags) \
	store(program, src, offset, dst, length, flags)

/*
 * Configures SEC SEQ STORE command to read data from registers and write
 * them to a memory location
 *
 * @param[in] src      Immidiate value or source register for data to be
 *                     stored: [KEY1SZ, KEY2SZ, DJQDA, MODE1, MODE2, DJQCTRL,
 *                     DATA1SZ, DATA2SZ, DSTAT, ICV1SZ, ICV2SZ, DPID, CCTRL,
 *                     ICTRL, CLRW, CSTAT, MATH0-MATH3, PKHA regs, CONTEXT1,
 *                     CONTEXT2, DESCBUF, JOBDESCBUF, SHAREDESCBUF];
 *                     @Note: IMM must be used to indicate immidiate value;
 * @param[in] offset   Start point for reading from source register;
 * @param[in] length   Number of bytes to store.
 * @param[in] flags    Operational flags: Operational flags: [SGF];
 * @retval             Updated descriptor size on succes.
 */
#define SEQSTORE(src, offset, length, flags) \
	store(program, src, offset, NONE, length, WITH(flags|SEQ))

/*
 * Configures SEC MATH command to perform binary operations;
 * operand1 (operator) operand2 -> result
 *
 * @param[in] operand1 First operand: [MATH0-MATH3; DPOVRD, SEQINSZ, SEQOUTSZ,
 *                     VSEQINSZ, VSEQOUTSZ, ZERO, ONE, NONE, Immidiate value];
 *                     @Note: IMM must be used to indicate immidiate value;
 * @param[in] operator Function to be performed: [ADD, ADDC, SUB, SUBB, OR,
 *                     AND, XOR, LSHIFT, RSHIFT, SHLD];
 * @param[in] operand2 Second operand: [MATH0-MATH3; DPOVRD, VSEQINSZ,
 *                     VSEQOUTSZ, ABD, OFIFO, JOBSRC, ZERO, ONE, SEQINSZ,
 *                     Immidiate value];
 *                     @Note: IMM must be used to indicate immidiate value;
 * @param[in] result   Destination for the result: [MATH0-MATH3; DPOVRD,
 *                     SEQINSZ, SEQOUTSZ, NONE, VSEQINSZ, VSEQOUTSZ];
 * @param[in] length   Lenght in bytes of the operation and the immidiate
 *                     value, if there is one;
 * @param[in] opt      Operational flags: Operational flags: [SGF];
 * @retval             Updated descriptor size on succes.
 */
#define MATHB(operand1, operator, operand2, result, length, opt) \
	math(program, operand1, operator, operand2, result, length, opt)

/*
 * Configures SEC MATH command to perform unary operations;
 * operand (operator)  -> result
 *
 * @param[in] operand  Operand: [MATH0-MATH3; DPOVRD, SEQINSZ, SEQOUTSZ,
 *                     VSEQINSZ, VSEQOUTSZ, ZERO, ONE, NONE, Immidiate value];
 *                     @Note: IMM must be used to indicate immidiate value;
 * @param[in] operator Function to be performed: [ZBYTE, BSWAP];
 * @param[in] result   Destination for the result: [MATH0-MATH3; DPOVRD,
 *                     SEQINSZ, SEQOUTSZ, NONE, VSEQINSZ, VSEQOUTSZ];
 * @param[in] length   Lenght in bytes of the operation and the immidiate
 *                     value, if there is one;
 * @param[in] opt      Operational flags: Operational flags: [SGF];
 * @retval             Updated descriptor size on succes.
 */
#define MATHU(operand1, operator, result, length, opt) \
	math(program, operand1, operator, _NONE, 0, result, length, opt)

/*
 * Configures SIGNATURE command
 *
 * @param[in] sign_type Signature type: [SIGN_TYPE_FINAL,
 *                      SIGN_TYPE_FINAL_RESTORE; SIGN_TYPE_FINAL_NONZERO,
 *                      SIGN_TYPE_IMM_2, SIGN_TYPE_IMM_3, SIGN_TYPE_IMM_4];
 *                      @Note: After signature command, must be used DWORD and
 *                      WORD to insert signature in descriptor buffer;
 * @retval             Updated descriptor size on succes.
 */
#define SIGNATURE(sign_type)   signature(program, sign_type)

/*
 * Configures NFIFO command; a shortcut of RTA Load command to write to
 * iNfo FIFO;
 *
 * @param[in] src      Source of the data for the Alignment Block: [IFIFO,
 *                     OFIFO, PAD, MSGOUTSNOOP, ALTSOURCE];
 * @param[in] data     Type of data that is going through the Input Data FIFO:
 *                     [MSG, MSG1, MSG2, IV1, IV2, ICV1, ICV2, SAD1, AAD1, AAD2,
 *                     AFHA_SBOX, SKIP, PKHA regs, AB1, AB2, ABD];
 * @param[in] flags    Operational flags: [LAST1, LAST2, FLUSH1, FLUSH2, OC,
 *                     BP];
 *                     When PAD is selected as source: [BM, PR, PS]
 *		       Select padding type: [PAD_ZERO, PAD_NONZERO,
 *		       PAD_INCREMENT, PAD_RANDOM, PAD_ZERO_N1, PAD_NONZERO_0,
 *		       PAD_N1, PAD_NONZERO_N];
 * @retval             Updated descriptor size on succes.
 */
#define NFIFOADD(src, data, length, flags) \
	nfifo_load(program, src, data, length, flags)

/********** Labels and references ***********/
#define REFERENCE(ref)    uint32_t ref = 0xFFFFFFFF;
#define LABEL(label)      uint32_t label = 0;
#define SET_LABEL(label)  label = set_label(program)

/*
 * Auxiliary commands to resolve self referential code.
 *
 * @param[in] line     Position in descriptor buffer where the update will be
 *                     done; this value is previously retained in program flow
 *                     using a reference near the sequence to be modified.
 * @param[in] new_ref  Updated value that will be inserted in descriptor
 *                     buffer at the specified line; this value is previously
 *                     obtained using SET_LABEL macro near the line that will
 *                     be used as reference.
 *                     For jump, move and load command patching, the value
 *                     represents the offset field.
 *                     For header command patching, the value represents the
 *                     start index field.
 *
 */
#define PATCH_JUMP(line, new_ref) patch_jmp(program, line, new_ref)
#define PATCH_MOVE(line, new_ref) patch_move(program, line, new_ref)
#define PATCH_LOAD(line, new_ref) patch_load(program, line, new_ref)
#define PATCH_HDR(line, new_ref) patch_header(program, line, new_ref)

/*
 * Auxiliary commands to resolve referential code between two program buffers.
 *
 * @param[in] src_program Buffer to be updated.
 * @param[in] line     Position in source descriptor buffer where the update
 *                     will be done; this value is previously retained in
 *                     program flow using a reference near the sequence to be
 *                     modified.
 * @param[in] dst_program Buffer that contain the new reference.
 * @param[in] new_ref  Updated value that will be inserted in source descriptor
 *                     buffer at the specified line; this value is previously
 *                     obtained using SET_LABEL macro near the line that will
 *                     be used as reference.
 *                     For jump, move and load command patching, the value
 *                     represents the offset field.
 *                     For header command patching, the value represents the
 *                     start index field.
 *
 */
#define PATCH_JUMP_NON_LOCAL(src_program, line, dst_program, new_ref) \
	patch_jump_non_local(src_program, line, dst_program, new_ref)
#define PATCH_MOVE_NON_LOCAL(src_program, line, dst_program, new_ref) \
	patch_move_non_local(src_program, line, dst_program, new_ref)
#define PATCH_HDR_NON_LOCAL(src_program, line, dst_program, new_ref) \
	patch_header_non_local(src_program, line, dst_program, new_ref)

#endif
