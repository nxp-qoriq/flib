#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;

/*
 * Example AES-CBC encrypt blockcipher-using-sequence pointer
 */
int test_blkcipher_seq(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;
	void *data_in = (void *)0x1234567;
	void *data_out = (void *)0x6123475;
	void *key_addr = (void *)0x778899AA;
	void *iv = (void *)0x11223344;
	int datasz = 512;
	int ivlen = 16;
	int keylen = 32;

	PROGRAM_CNTXT_INIT(buff, 0);
	JOB_HDR(SHR_NEVER, 0, 0);
	SEQINPTR((intptr_t) data_in, datasz, 0);
	SEQOUTPTR((intptr_t) data_out, datasz, 0);
	LOAD(PTR((intptr_t) iv), CONTEXT1, 0, ivlen, 0);
	KEY(KEY1, 0, PTR((intptr_t) key_addr), keylen, 0);
	MATHB(SEQINSZ, ADD, ZERO, VSEQINSZ, 4, 0);
	MATHB(SEQOUTSZ, ADD, ZERO, VSEQOUTSZ, 4, 0);
	ALG_OPERATION(OP_ALG_ALGSEL_AES, OP_ALG_AAI_CBC, 0, 0, OP_ALG_ENCRYPT);
	SEQFIFOLOAD(MSG1, 0, WITH(VLF | LAST1));
	SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));

	size = PROGRAM_FINALIZE();

	return size;
}

int prg_buff[1000];

int main(int argc, char **argv)
{
	int size;

	printf("BLKCIPHER SEQ program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = test_blkcipher_seq((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
