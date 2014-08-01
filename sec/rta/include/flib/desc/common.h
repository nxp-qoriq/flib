/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#ifndef __DESC_COMMON_H__
#define __DESC_COMMON_H__

#include "flib/rta.h"

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

/**
 * split_key_len - Compute MDHA split key length for a given algorithm
 * @hash: Hashing algorithm selection, one of OP_ALG_ALGSEL_* - MD5, SHA1,
 *        SHA224, SHA384, SHA512.
 *
 * Return: MDHA split key length
 */
static inline uint32_t split_key_len(uint32_t hash)
{
	/* Sizes for MDHA pads (*not* keys): MD5, SHA1, 224, 256, 384, 512 */
	static const uint8_t mdpadlen[] = { 16, 20, 32, 32, 64, 64 };
	uint32_t idx;

	idx = (hash & OP_ALG_ALGSEL_SUBMASK) >> OP_ALG_ALGSEL_SHIFT;

	return (uint32_t)(mdpadlen[idx] * 2);
}

/**
 * split_key_pad_len - Compute MDHA split key pad length for a given algorithm
 * @hash: Hashing algorithm selection, one of OP_ALG_ALGSEL_* - MD5, SHA1,
 *        SHA224, SHA384, SHA512.
 *
 * Return: MDHA split key pad length
 */
static inline uint32_t split_key_pad_len(uint32_t hash)
{
	return ALIGN(split_key_len(hash), 16);
}

#endif /* __DESC_COMMON_H__ */
