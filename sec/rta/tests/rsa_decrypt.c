/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;

/* Subroutine to populate a Descriptor buffer */
unsigned build_rsa_decrypt_desc(uint32_t *buff, uint32_t n_len, uint32_t p_len,
				uint32_t q_len,
				const uint64_t p, const uint64_t q,
				const uint64_t dp, const uint64_t dq,
				const uint64_t c,
				const uint64_t t1, const uint64_t t2,
				const uint64_t g, const uint64_t f,
				const uint64_t msglen)
{
	struct program prg;
	struct program *program = &prg;
	LABEL(pdb_end);

	PROGRAM_CNTXT_INIT(buff, 0);
	PROGRAM_SET_36BIT_ADDR();

	JOB_HDR(SHR_NEVER, 0, 0, 0);
	{
		{	/* RSA Decrypt */
			WORD(n_len);
			DWORD(g);
			DWORD(f);
			DWORD(c);
			DWORD(p);
			DWORD(dq);
			DWORD(dp);
			DWORD(q);
			DWORD(t1);
			DWORD(t2);
			WORD(((q_len << 12) | p_len));
			SET_LABEL(pdb_end);
		}
		PROTOCOL(OP_TYPE_UNI_PROTOCOL, OP_PCLID_RSADECRYPT,
			 WITH(OP_PCL_RSAPROT_OP_DEC_PQDPDQC |
			      OP_PCL_RSAPROT_FMT_PKCSV15));
		STORE(MATH0, 4, PTR(msglen), 4, 0);
	}
	PATCH_HDR(0, pdb_end);

	return PROGRAM_FINALIZE();
}

unsigned test_rsa_decrypt(uint32_t *buff)
{
	uint32_t n_len = 128;	/* RSA 1024 */
	uint32_t p_len = 64;
	uint32_t q_len = 64;

	uint64_t buf_phys = 0x200320024ull;
	uint64_t msglen = (uint64_t) (buf_phys); /* 1st vector in buffer */
	uint64_t p = buf_phys;	/* 2nd  */
	uint64_t q = p + p_len;	/* 3rd  */
	uint64_t c = q + q_len;	/* 4th  */
	uint64_t dp = q + p_len;	/* 5th  */
	uint64_t dq = dp + p_len;	/* 6th  */
	uint64_t t1 = dq + q_len;	/* 7th  */
	uint64_t t2 = t1 + p_len;	/* 8th  */
	uint64_t g = t1 + q_len;	/* 9th  */
	uint64_t f = g;		/* 10th overwrites 9th  */

	return build_rsa_decrypt_desc(buff, n_len, p_len, q_len, p, q,
				      dp, dq, c, t1, t2, g, f, msglen);
}

uint32_t prg_buff[1000];

int main(int argc, char **argv)
{
	unsigned size;

	pr_debug("RSA Decrypt example program\n");
	rta_set_sec_era(RTA_SEC_ERA_2);
	size = test_rsa_decrypt(prg_buff);
	pr_debug("size = %d\n", size);
	print_prog(prg_buff, size);

	return 0;
}
