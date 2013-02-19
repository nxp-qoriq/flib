#include <stdio.h>
#include "flib/rta.h"

enum rta_sec_era rta_sec_era;

/* Subroutine to populate a Descriptor buffer */
int build_rsa_verify_desc(uint32_t *buff, uint32_t n_len, uint32_t e_len,
			  const uint64_t n, const uint64_t e,
			  const uint64_t f, const uint64_t g)
{
	struct program prg;
	struct program *program = &prg;
	int size;

	PROGRAM_SET_36BIT_ADDR();

	PROGRAM_CNTXT_INIT(buff, 0);
	JOB_HDR(SHR_NEVER, 0, 0);
	{
		{	/* RSA Verify */
			WORD(((e_len << 12) + n_len));	/* e length, n length */
			DWORD(f);	/* output */
			DWORD(g);	/* input */
			DWORD(n);	/* modulus */
			DWORD(e);	/* public expnenent */
			WORD(n_len);	/* g length */

		}
		PROTOCOL(OP_TYPE_UNI_PROTOCOL, OP_PCLID_RSAENCRYPT, 0);
	}
	size = PROGRAM_FINALIZE();
	return size;
}

int test_rsa_verify(uint32_t *buff)
{
	int size;
	uint32_t n_len = 128;
	uint32_t e_len = 3;

	uint64_t buf_phys = 0x200320024ull;
	uint64_t e = (uint64_t) buf_phys;	/* 1st vector in buffer */
	uint64_t n = e + e_len;	/* 2nd                  */
	uint64_t g = n + n_len;	/* 3rd                  */
	uint64_t f = g;		/* 4th overwrites 3rd   */

	size = build_rsa_verify_desc(buff, n_len, e_len, n, e, f, g);
	return size;
}

int prg_buff[1000];

static void print_prog(uint32_t *buff, int size)
{
	int i;
	for (i = 0; i < size; i++)
		printf("%08X\n", buff[i]);
}

int main(int argc, char **argv)
{
	int size;

	printf("RSA Verify example program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = test_rsa_verify((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
