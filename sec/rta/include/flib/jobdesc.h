#ifndef __RTA_JOBDESC_H__
#define __RTA_JOBDESC_H__

#include "rta.h"

/**
 * @file                 jobdesc.h
 * @brief                SEC Job Descriptor Construction Library
 */

/**
 * @defgroup jobdesc_group Job Descriptor Example Routines
 * @ingroup descriptor_lib_group
 * @{
 */


/**
 * @details Generate an MDHA split key
 *          Split keys are IPAD/OPAD pairs. For details, refer to MDHA Split
 *          Keys chapter in SEC Reference Manual
 *
 * @param[out] descbuf pointer to buffer to hold constructed descriptor
 * @param[out] bufsize pointer to size of descriptor once constructed
 * @param[in] alg_key  Pointer to HMAC key to generate ipad/opad from
 * @param[in] cipher   HMAC algorithm selection, one of OP_ALG_ALGSEL_*.
 *           The algorithm determines key size (bytes):
 *      -  OP_ALG_ALGSEL_MD5    - 16
 *      -  OP_ALG_ALGSEL_SHA1   - 20
 *      -  OP_ALG_ALGSEL_SHA224 - 28
 *      -  OP_ALG_ALGSEL_SHA256 - 32
 *      -  OP_ALG_ALGSEL_SHA384 - 48
 *      -  OP_ALG_ALGSEL_SHA512 - 64
 *
 * @param [out] padbuf - Pointer to buffer to store generated ipad/opad
 */

static inline void cnstr_jobdesc_mdsplitkey(uint32_t *descbuf,
					    unsigned *bufsize,
					    uintptr_t alg_key,
					    uint32_t cipher,
					    uintptr_t padbuf)
{
	/* key and split-key sizes in bytes */
	const uint8_t mdkeylen[][2] = {
		{16, 32},   /* MD5 */
		{20, 40},   /* SHA1 */
		{28, 64},   /* SHA224 */
		{32, 64},   /* SHA256 */
		{48, 128},  /* SHA384 */
		{64, 128},  /* SHA512 */
	};
	uint8_t keylen, storelen, idx;

	struct program prg;
	struct program *program = &prg;

	idx = (cipher & OP_ALG_ALGSEL_SUBMASK) >> OP_ALG_ALGSEL_SHIFT;
	keylen = mdkeylen[idx][0];
	storelen = mdkeylen[idx][1];

	PROGRAM_CNTXT_INIT(descbuf, 0);
	PROGRAM_SET_36BIT_ADDR();
	JOB_HDR(SHR_NEVER, 1, 0, 0);
	KEY(KEY2, 0, PTR(alg_key), keylen, 0);
	ALG_OPERATION(cipher,
		      OP_ALG_AAI_HMAC,
		      OP_ALG_AS_INIT,
		      ICV_CHECK_DISABLE,
		      OP_ALG_DECRYPT);
	FIFOLOAD(MSG2, PTR(0), 0, LAST2 | IMMED);
	JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, CLASS2);
	FIFOSTORE(MDHA_SPLIT_KEY, 0, padbuf, storelen, 0);
	*bufsize = PROGRAM_FINALIZE();
}

/** @} end of jobdesc_group */

#endif /* __RTA_JOBDESC_H__ */
