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
 * enum rta_data_type - Indicates how is the data provided and how to include it
 *                      in the descriptor.
 * @RTA_DATA_PTR: Data is in memory and accessed by reference; data address is a
 *               physical (bus) address.
 * @RTA_DATA_IMM: Data is inlined in descriptor and accessed as immediate data;
 *               data address is a virtual address.
 * @RTA_DATA_IMM_DMA: (AIOP only) Data is inlined in descriptor and accessed as
 *                   immediate data; data address is a physical (bus) address
 *                   in external memory and CDMA is programmed to transfer the
 *                   data into descriptor buffer being built in Workspace Area.
 */
enum rta_data_type {
	RTA_DATA_PTR = 1,
	RTA_DATA_IMM,
	RTA_DATA_IMM_DMA
};

/**
 * struct alginfo - Container for algorithm details
 * @algtype: algorithm selector; for valid values, see documentation of the
 *           functions where it is used.
 * @keylen: length of the provided algorithm key, in bytes
 * @key: address where algorithm key resides; virtual address if key_type is
 *       RTA_DATA_IMM, physical (bus) address if key_type is RTA_DATA_PTR or
 *       RTA_DATA_IMM_DMA.
 * @key_enc_flags: key encryption flags; see encrypt_flags parameter of KEY
 *                 command for valid values.
 * @key_type: enum rta_data_type
 */
struct alginfo {
	uint32_t algtype;
	uint32_t keylen;
	uint64_t key;
	uint32_t key_enc_flags;
	enum rta_data_type key_type;
};

static inline uint32_t inline_flags(enum rta_data_type data_type)
{
	switch (data_type) {
	case RTA_DATA_PTR:
		return 0;
	case RTA_DATA_IMM:
		return IMMED | COPY;
	case RTA_DATA_IMM_DMA:
		return IMMED | DCOPY;
	default:
		/* warn and default to RTA_DATA_PTR */
		pr_warn("RTA: defaulting to RTA_DATA_PTR parameter type\n");
		return 0;
	}
}

#define INLINE_KEY(alginfo)	inline_flags(alginfo->key_type)

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
