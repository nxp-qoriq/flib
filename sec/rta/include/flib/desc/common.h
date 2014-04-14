/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#ifndef __DESC_COMMON_H__
#define __DESC_COMMON_H__

/**
 * DOC: Shared Descriptor Constructors - shared structures
 *
 * Data structures shared between algorithm, protocol implementations.
 */

/**
 * struct alginfo - Container for algorithm details
 * @algtype: algorithm selector; for valid values, see documentation of the
 *           functions where it is used.
 * @key: address where algorithm key resides
 * @keylen: length of the provided key, in bytes
 * @key_enc_flags: key encryption flags
 */
struct alginfo {
	uint32_t algtype;
	uint64_t key;
	uint32_t keylen;
	uint32_t key_enc_flags;
};

/**
 * struct protcmd - Container for Protocol Operation Command fields
 * @optype: command type
 * @protid: protocol Identifier
 * @protinfo: protocol Information
 */
struct protcmd {
	uint32_t optype;
	uint32_t protid;
	uint16_t protinfo;
};

#endif /* __DESC_ALGO_H__ */
