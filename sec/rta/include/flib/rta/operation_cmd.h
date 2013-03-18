#ifndef __RTA_OPERATION_CMD_H__
#define __RTA_OPERATION_CMD_H__

extern enum rta_sec_era rta_sec_era;

static inline int32_t alg_aai_aes(uint16_t aai)
{
	uint16_t aes_mode = aai & OP_ALG_AESA_MODE_MASK;

	if (aai & OP_ALG_AAI_C2K) {
		if (rta_sec_era < RTA_SEC_ERA_5)
			return -1;
		if ((aes_mode != OP_ALG_AAI_CCM) &&
		    (aes_mode != OP_ALG_AAI_GCM))
			return -1;
	}

	switch (aes_mode) {
	case OP_ALG_AAI_CBC_CMAC:
	case OP_ALG_AAI_CTR_CMAC_LTE:
	case OP_ALG_AAI_CTR_CMAC:
		if (rta_sec_era < RTA_SEC_ERA_2)
			return -1;
		/* no break */
	case OP_ALG_AAI_CTR:
	case OP_ALG_AAI_CBC:
	case OP_ALG_AAI_ECB:
	case OP_ALG_AAI_OFB:
	case OP_ALG_AAI_CFB:
	case OP_ALG_AAI_XTS:
	case OP_ALG_AAI_CMAC:
	case OP_ALG_AAI_XCBC_MAC:
	case OP_ALG_AAI_CCM:
	case OP_ALG_AAI_GCM:
	case OP_ALG_AAI_CBC_XCBCMAC:
	case OP_ALG_AAI_CTR_XCBCMAC:
		return 0;
	}

	return -1;
}

static inline int32_t alg_aai_des(uint16_t aai)
{
	uint16_t aai_code = aai & ~OP_ALG_AAI_CHECKODD;

	switch (aai_code) {
	case OP_ALG_AAI_CBC:
	case OP_ALG_AAI_ECB:
	case OP_ALG_AAI_CFB:
	case OP_ALG_AAI_OFB:
		return 0;
	}

	return -1;
}

static inline int32_t alg_aai_md5(uint16_t aai)
{
	switch (aai) {
	case OP_ALG_AAI_HMAC:
		if (rta_sec_era < RTA_SEC_ERA_2)
			return -1;
		/* no break */
	case OP_ALG_AAI_SMAC:
	case OP_ALG_AAI_HASH:
	case OP_ALG_AAI_HMAC_PRECOMP:
		return 0;
	}

	return -1;
}

static inline int32_t alg_aai_sha(uint16_t aai)
{
	switch (aai) {
	case OP_ALG_AAI_HMAC:
		if (rta_sec_era < RTA_SEC_ERA_2)
			return -1;
		/* no break */
	case OP_ALG_AAI_HASH:
	case OP_ALG_AAI_HMAC_PRECOMP:
		return 0;
	}

	return -1;
}

static inline int32_t alg_aai_rng(uint16_t aai)
{
	uint16_t rng_mode = aai & OP_ALG_RNG_MODE_MASK;
	uint16_t rng_sh = aai & OP_ALG_AAI_RNG_SH_MASK;

	switch (rng_mode) {
	case OP_ALG_AAI_RNG:
	case OP_ALG_AAI_RNG_NOZERO:
	case OP_ALG_AAI_RNG_ODD:
		break;
	default:
		return -1;
	}

	/* State Handle bits are reserved for SEC Era < 5 */
	if ((rta_sec_era < RTA_SEC_ERA_5) && rng_sh)
		return -1;

	switch (rng_sh) {
	case OP_ALG_AAI_RNG_SH0:
	case OP_ALG_AAI_RNG_SH1:
		return 0;
	}

	return -1;
}

static inline int32_t alg_aai_crc(uint16_t aai)
{
	uint16_t aai_code = aai & OP_ALG_CRC_POLY_MASK;

	switch (aai_code) {
	case OP_ALG_AAI_802:
	case OP_ALG_AAI_3385:
	case OP_ALG_AAI_CUST_POLY:
		return 0;
	}

	return -1;
}

static inline int32_t alg_aai_kasumi(uint16_t aai)
{
	switch (aai) {
	case OP_ALG_AAI_GSM:
	case OP_ALG_AAI_EDGE:
	case OP_ALG_AAI_F8:
	case OP_ALG_AAI_F9:
		return 0;
	}

	return -1;
}

static inline int32_t alg_aai_snow_f9(uint16_t aai)
{
	if (aai == OP_ALG_AAI_F9)
		return 0;

	return -1;
}

static inline int32_t alg_aai_snow_f8(uint16_t aai)
{
	if (aai == OP_ALG_AAI_F8)
		return 0;

	return -1;
}

static inline int32_t alg_aai_zuce(uint16_t aai)
{
	if (aai == OP_ALG_AAI_F8)
		return 0;

	return -1;
}

static inline int32_t alg_aai_zuca(uint16_t aai)
{
	if (aai == OP_ALG_AAI_F9)
		return 0;

	return -1;
}

struct alg_aai_map {
	uint32_t chipher_algo;
	int32_t (*aai_func)(uint16_t);
	uint32_t class;
};

static const struct alg_aai_map alg_table[] = {
/*1*/	{ OP_ALG_ALGSEL_AES,      alg_aai_aes,    OP_TYPE_CLASS1_ALG },
	{ OP_ALG_ALGSEL_DES,      alg_aai_des,    OP_TYPE_CLASS1_ALG },
	{ OP_ALG_ALGSEL_3DES,     alg_aai_des,    OP_TYPE_CLASS1_ALG },
	{ OP_ALG_ALGSEL_ARC4,     NULL,           OP_TYPE_CLASS1_ALG },
	{ OP_ALG_ALGSEL_MD5,      alg_aai_md5,    OP_TYPE_CLASS2_ALG },
	{ OP_ALG_ALGSEL_SHA1,     alg_aai_md5,    OP_TYPE_CLASS2_ALG },
	{ OP_ALG_ALGSEL_SHA224,   alg_aai_sha,    OP_TYPE_CLASS2_ALG },
	{ OP_ALG_ALGSEL_SHA256,   alg_aai_sha,    OP_TYPE_CLASS2_ALG },
	{ OP_ALG_ALGSEL_SHA384,   alg_aai_sha,    OP_TYPE_CLASS2_ALG },
	{ OP_ALG_ALGSEL_SHA512,   alg_aai_sha,    OP_TYPE_CLASS2_ALG },
	{ OP_ALG_ALGSEL_RNG,      alg_aai_rng,    OP_TYPE_CLASS1_ALG },
	{ OP_ALG_ALGSEL_SNOW_F8,  alg_aai_snow_f8, OP_TYPE_CLASS1_ALG },
	{ OP_ALG_ALGSEL_KASUMI,   alg_aai_kasumi, OP_TYPE_CLASS1_ALG },
/*14*/	{ OP_ALG_ALGSEL_CRC,      alg_aai_crc,    OP_TYPE_CLASS2_ALG },
	{ OP_ALG_ALGSEL_SNOW_F9,  alg_aai_snow_f9, OP_TYPE_CLASS2_ALG },
	{ OP_ALG_ALGSEL_ZUCE,     alg_aai_zuce,   OP_TYPE_CLASS1_ALG },
/*17*/	{ OP_ALG_ALGSEL_ZUCA,     alg_aai_zuca,   OP_TYPE_CLASS2_ALG }
};

/*
 * Allowed OPERATION algorithms for each SEC Era.
 * Values represent the number of entries from alg_table[] that are supported.
 */
static const uint8_t alg_table_sz[] = {14, 15, 15, 15, 17};

static inline uint32_t operation(struct program *program, uint32_t cipher_algo,
	  uint16_t aai, uint8_t algo_state, uint8_t icv_checking, uint8_t enc)
{
	uint32_t opcode = CMD_OPERATION;
	uint32_t i, found = 0;

	for (i = 0; i < alg_table_sz[rta_sec_era]; i++) {
		if (alg_table[i].chipher_algo == cipher_algo) {
			opcode |= cipher_algo | alg_table[i].class;
			/* nothing else to verify */
			if (alg_table[i].aai_func == NULL) {
				found = 1;
				break;
			}

			aai &= OP_ALG_AAI_MASK;

			if (-1 ==  (*alg_table[i].aai_func)(aai)) {
				pr_debug("OPERATION: Bad AAI Type. "
						"SEC Program Line: %d\n",
						program->current_pc);
				goto err;
			}
			opcode |= aai;
			found = 1;
			break;
		}
	}
	if (!found) {
		pr_debug("OPERATION: Invalid Command. SEC Program Line: %d\n",
				program->current_pc);
		goto err;
	}

	switch (algo_state) {
	case OP_ALG_AS_UPDATE:
	case OP_ALG_AS_INIT:
	case OP_ALG_AS_FINALIZE:
	case OP_ALG_AS_INITFINAL:
		opcode |= algo_state;
		break;
	default:
		pr_debug("Invalid Operation Command\n");
		goto err;
	}

	switch (icv_checking) {
	case ICV_CHECK_DISABLE:
		opcode |= OP_ALG_ICV_OFF;
		break;
	case ICV_CHECK_ENABLE:
		opcode |= OP_ALG_ICV_ON;
		break;
	default:
		pr_debug("Invalid Operation Command\n");
		goto err;
	}

	switch (enc) {
	case OP_ALG_DECRYPT:
		opcode |= OP_ALG_DECRYPT;
		break;
	case OP_ALG_ENCRYPT:
		opcode |= OP_ALG_ENCRYPT;
		break;
	default:
		pr_debug("Invalid Operation Command\n");
		goto err;
	}

	program->buffer[program->current_pc++] = opcode;
	program->current_instraction++;
	return program->current_pc;
 err:
	program->first_error_pc = program->current_pc;
	/* skipping one word regardless immediates */
	return program->current_pc++;
}

/*
 * OPERATION PKHA routines
 */
static inline int32_t pkha_clearmem(uint32_t pkha_op)
{
	switch (pkha_op) {
	case (OP_ALG_PKMODE_CLEARMEM_ALL):
	case (OP_ALG_PKMODE_CLEARMEM_ABE):
	case (OP_ALG_PKMODE_CLEARMEM_ABN):
	case (OP_ALG_PKMODE_CLEARMEM_AB):
	case (OP_ALG_PKMODE_CLEARMEM_AEN):
	case (OP_ALG_PKMODE_CLEARMEM_AE):
	case (OP_ALG_PKMODE_CLEARMEM_AN):
	case (OP_ALG_PKMODE_CLEARMEM_A):
	case (OP_ALG_PKMODE_CLEARMEM_BEN):
	case (OP_ALG_PKMODE_CLEARMEM_BE):
	case (OP_ALG_PKMODE_CLEARMEM_BN):
	case (OP_ALG_PKMODE_CLEARMEM_B):
	case (OP_ALG_PKMODE_CLEARMEM_EN):
	case (OP_ALG_PKMODE_CLEARMEM_N):
	case (OP_ALG_PKMODE_CLEARMEM_E):
		return 0;
	}

	return -1;
}

static inline int32_t pkha_mod_arithmetic(uint32_t pkha_op)
{
	pkha_op &= ~OP_ALG_PKMODE_OUT_A;

	switch (pkha_op) {
	case (OP_ALG_PKMODE_MOD_ADD):
	case (OP_ALG_PKMODE_MOD_SUB_AB):
	case (OP_ALG_PKMODE_MOD_SUB_BA):
	case (OP_ALG_PKMODE_MOD_MULT):
	case (OP_ALG_PKMODE_MOD_MULT_IM):
	case (OP_ALG_PKMODE_MOD_MULT_IM_OM):
	case (OP_ALG_PKMODE_MOD_EXPO):
	case (OP_ALG_PKMODE_MOD_EXPO_TEQ):
	case (OP_ALG_PKMODE_MOD_EXPO_IM):
	case (OP_ALG_PKMODE_MOD_EXPO_IM_TEQ):
	case (OP_ALG_PKMODE_MOD_REDUCT):
	case (OP_ALG_PKMODE_MOD_INV):
	case (OP_ALG_PKMODE_MOD_MONT_CNST):
	case (OP_ALG_PKMODE_MOD_CRT_CNST):
	case (OP_ALG_PKMODE_MOD_GCD):
	case (OP_ALG_PKMODE_MOD_PRIMALITY):
	case (OP_ALG_PKMODE_F2M_ADD):
	case (OP_ALG_PKMODE_F2M_MUL):
	case (OP_ALG_PKMODE_F2M_MUL_IM):
	case (OP_ALG_PKMODE_F2M_MUL_IM_OM):
	case (OP_ALG_PKMODE_F2M_EXP):
	case (OP_ALG_PKMODE_F2M_EXP_TEQ):
	case (OP_ALG_PKMODE_F2M_AMODN):
	case (OP_ALG_PKMODE_F2M_INV):
	case (OP_ALG_PKMODE_F2M_R2):
	case (OP_ALG_PKMODE_F2M_GCD):
	case (OP_ALG_PKMODE_ECC_F2M_ADD):
	case (OP_ALG_PKMODE_ECC_F2M_ADD_IM_OM_PROJ):
	case (OP_ALG_PKMODE_ECC_F2M_DBL):
	case (OP_ALG_PKMODE_ECC_F2M_DBL_IM_OM_PROJ):
	case (OP_ALG_PKMODE_ECC_F2M_MUL):
	case (OP_ALG_PKMODE_ECC_F2M_MUL_TEQ):
	case (OP_ALG_PKMODE_ECC_F2M_MUL_R2):
	case (OP_ALG_PKMODE_ECC_F2M_MUL_R2_TEQ):
	case (OP_ALG_PKMODE_ECC_F2M_MUL_R2_PROJ):
	case (OP_ALG_PKMODE_ECC_F2M_MUL_R2_PROJ_TEQ):
	case (OP_ALG_PKMODE_ECC_MOD_ADD):
	case (OP_ALG_PKMODE_ECC_MOD_ADD_IM_OM_PROJ):
	case (OP_ALG_PKMODE_ECC_MOD_DBL):
	case (OP_ALG_PKMODE_ECC_MOD_DBL_IM_OM_PROJ):
	case (OP_ALG_PKMODE_ECC_MOD_MUL):
	case (OP_ALG_PKMODE_ECC_MOD_MUL_TEQ):
	case (OP_ALG_PKMODE_ECC_MOD_MUL_R2):
	case (OP_ALG_PKMODE_ECC_MOD_MUL_R2_TEQ):
	case (OP_ALG_PKMODE_ECC_MOD_MUL_R2_PROJ):
	case (OP_ALG_PKMODE_ECC_MOD_MUL_R2_PROJ_TEQ):
		return 0;
	}

	return -1;
}

static inline int32_t pkha_copymem(uint32_t pkha_op)
{
	switch (pkha_op) {
	case (OP_ALG_PKMODE_COPY_NSZ_A0_B0):
	case (OP_ALG_PKMODE_COPY_NSZ_A0_B1):
	case (OP_ALG_PKMODE_COPY_NSZ_A0_B2):
	case (OP_ALG_PKMODE_COPY_NSZ_A0_B3):
	case (OP_ALG_PKMODE_COPY_NSZ_A1_B0):
	case (OP_ALG_PKMODE_COPY_NSZ_A1_B1):
	case (OP_ALG_PKMODE_COPY_NSZ_A1_B2):
	case (OP_ALG_PKMODE_COPY_NSZ_A1_B3):
	case (OP_ALG_PKMODE_COPY_NSZ_A2_B0):
	case (OP_ALG_PKMODE_COPY_NSZ_A2_B1):
	case (OP_ALG_PKMODE_COPY_NSZ_A2_B2):
	case (OP_ALG_PKMODE_COPY_NSZ_A2_B3):
	case (OP_ALG_PKMODE_COPY_NSZ_A3_B0):
	case (OP_ALG_PKMODE_COPY_NSZ_A3_B1):
	case (OP_ALG_PKMODE_COPY_NSZ_A3_B2):
	case (OP_ALG_PKMODE_COPY_NSZ_A3_B3):
	case (OP_ALG_PKMODE_COPY_NSZ_B0_A0):
	case (OP_ALG_PKMODE_COPY_NSZ_B0_A1):
	case (OP_ALG_PKMODE_COPY_NSZ_B0_A2):
	case (OP_ALG_PKMODE_COPY_NSZ_B0_A3):
	case (OP_ALG_PKMODE_COPY_NSZ_B1_A0):
	case (OP_ALG_PKMODE_COPY_NSZ_B1_A1):
	case (OP_ALG_PKMODE_COPY_NSZ_B1_A2):
	case (OP_ALG_PKMODE_COPY_NSZ_B1_A3):
	case (OP_ALG_PKMODE_COPY_NSZ_B2_A0):
	case (OP_ALG_PKMODE_COPY_NSZ_B2_A1):
	case (OP_ALG_PKMODE_COPY_NSZ_B2_A2):
	case (OP_ALG_PKMODE_COPY_NSZ_B2_A3):
	case (OP_ALG_PKMODE_COPY_NSZ_B3_A0):
	case (OP_ALG_PKMODE_COPY_NSZ_B3_A1):
	case (OP_ALG_PKMODE_COPY_NSZ_B3_A2):
	case (OP_ALG_PKMODE_COPY_NSZ_B3_A3):
	case (OP_ALG_PKMODE_COPY_NSZ_A_E):
	case (OP_ALG_PKMODE_COPY_NSZ_A_N):
	case (OP_ALG_PKMODE_COPY_NSZ_B_E):
	case (OP_ALG_PKMODE_COPY_NSZ_B_N):
	case (OP_ALG_PKMODE_COPY_NSZ_N_A):
	case (OP_ALG_PKMODE_COPY_NSZ_N_B):
	case (OP_ALG_PKMODE_COPY_NSZ_N_E):
	case (OP_ALG_PKMODE_COPY_SSZ_A0_B0):
	case (OP_ALG_PKMODE_COPY_SSZ_A0_B1):
	case (OP_ALG_PKMODE_COPY_SSZ_A0_B2):
	case (OP_ALG_PKMODE_COPY_SSZ_A0_B3):
	case (OP_ALG_PKMODE_COPY_SSZ_A1_B0):
	case (OP_ALG_PKMODE_COPY_SSZ_A1_B1):
	case (OP_ALG_PKMODE_COPY_SSZ_A1_B2):
	case (OP_ALG_PKMODE_COPY_SSZ_A1_B3):
	case (OP_ALG_PKMODE_COPY_SSZ_A2_B0):
	case (OP_ALG_PKMODE_COPY_SSZ_A2_B1):
	case (OP_ALG_PKMODE_COPY_SSZ_A2_B2):
	case (OP_ALG_PKMODE_COPY_SSZ_A2_B3):
	case (OP_ALG_PKMODE_COPY_SSZ_A3_B0):
	case (OP_ALG_PKMODE_COPY_SSZ_A3_B1):
	case (OP_ALG_PKMODE_COPY_SSZ_A3_B2):
	case (OP_ALG_PKMODE_COPY_SSZ_A3_B3):
	case (OP_ALG_PKMODE_COPY_SSZ_B0_A0):
	case (OP_ALG_PKMODE_COPY_SSZ_B0_A1):
	case (OP_ALG_PKMODE_COPY_SSZ_B0_A2):
	case (OP_ALG_PKMODE_COPY_SSZ_B0_A3):
	case (OP_ALG_PKMODE_COPY_SSZ_B1_A0):
	case (OP_ALG_PKMODE_COPY_SSZ_B1_A1):
	case (OP_ALG_PKMODE_COPY_SSZ_B1_A2):
	case (OP_ALG_PKMODE_COPY_SSZ_B1_A3):
	case (OP_ALG_PKMODE_COPY_SSZ_B2_A0):
	case (OP_ALG_PKMODE_COPY_SSZ_B2_A1):
	case (OP_ALG_PKMODE_COPY_SSZ_B2_A2):
	case (OP_ALG_PKMODE_COPY_SSZ_B2_A3):
	case (OP_ALG_PKMODE_COPY_SSZ_B3_A0):
	case (OP_ALG_PKMODE_COPY_SSZ_B3_A1):
	case (OP_ALG_PKMODE_COPY_SSZ_B3_A2):
	case (OP_ALG_PKMODE_COPY_SSZ_B3_A3):
	case (OP_ALG_PKMODE_COPY_SSZ_A_E):
	case (OP_ALG_PKMODE_COPY_SSZ_A_N):
	case (OP_ALG_PKMODE_COPY_SSZ_B_E):
	case (OP_ALG_PKMODE_COPY_SSZ_B_N):
	case (OP_ALG_PKMODE_COPY_SSZ_N_A):
	case (OP_ALG_PKMODE_COPY_SSZ_N_B):
	case (OP_ALG_PKMODE_COPY_SSZ_N_E):
		return 0;
	}

	return -1;
}

static inline uint32_t pkha_operation(struct program *program, uint32_t op_pkha)
{
	uint32_t opcode = CMD_OPERATION | OP_TYPE_PK | OP_ALG_PK;
	uint32_t pkha_func;


	pkha_func = op_pkha & OP_ALG_PK_FUN_MASK;

	switch (pkha_func) {
	case (OP_ALG_PKMODE_CLEARMEM):
		if (-1 == pkha_clearmem(op_pkha)) {
			pr_debug("OPERATION PKHA: Type not supported. "
				"SEC Program Line: %d\n", program->current_pc);
			goto err;
		}
		break;
	case (OP_ALG_PKMODE_MOD_ADD):
	case (OP_ALG_PKMODE_MOD_SUB_AB):
	case (OP_ALG_PKMODE_MOD_SUB_BA):
	case (OP_ALG_PKMODE_MOD_MULT):
	case (OP_ALG_PKMODE_MOD_EXPO):
	case (OP_ALG_PKMODE_MOD_REDUCT):
	case (OP_ALG_PKMODE_MOD_INV):
	case (OP_ALG_PKMODE_MOD_MONT_CNST):
	case (OP_ALG_PKMODE_MOD_CRT_CNST):
	case (OP_ALG_PKMODE_MOD_GCD):
	case (OP_ALG_PKMODE_MOD_PRIMALITY):
	case (OP_ALG_PKMODE_ECC_MOD_ADD):
	case (OP_ALG_PKMODE_ECC_MOD_DBL):
	case (OP_ALG_PKMODE_ECC_MOD_MUL):
		if (-1 == pkha_mod_arithmetic(op_pkha)) {
			pr_debug("OPERATION PKHA: Type not supported. "
				"SEC Program Line: %d\n", program->current_pc);
			goto err;
		}
		break;
	case (OP_ALG_PKMODE_COPY_NSZ):
	case (OP_ALG_PKMODE_COPY_SSZ):
		if (-1 == pkha_copymem(op_pkha)) {
			pr_debug("OPERATION PKHA: Type not supported. "
				"SEC Program Line: %d\n", program->current_pc);
			goto err;
		}
		break;
	default:
		pr_debug("Invalid Operation Command\n");
		goto err;
	}

	opcode |= op_pkha;

	program->buffer[program->current_pc++] = opcode;
	program->current_instraction++;
	return program->current_pc;

 err:
	program->first_error_pc = program->current_pc;
	program->current_instraction++;
	program->current_pc++;
	return program->current_pc;
}

#endif /* __RTA_OPERATION_CMD_H__ */
