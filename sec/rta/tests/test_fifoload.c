#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;
int prg_buff[1000];

int test_fifo_load_op(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;
	uint64_t msg = 0x12004201;
	uint64_t aad_ptr = 0x200;
	uint64_t src_dma = 0x800;
	uint64_t iv_ptr = 0x02230104;
	uint64_t icv_ptr = 0xf3230d01;
	uint64_t auth_ptr = 0x13422591;
	uint64_t mod = 0x24128820;

	PROGRAM_CNTXT_INIT(buff, 0);

	FIFOLOAD(MSG1, PTR(msg), 1, WITH(FLUSH1));
	FIFOLOAD(MSG1, PTR(msg), 2, WITH(LAST1));
	FIFOLOAD(MSG2, PTR(msg), 3, WITH(LAST2));
	FIFOLOAD(MSG2, PTR(msg), 4, 0);

	FIFOLOAD(BIT_DATA, PTR(msg), 4003, WITH(CLASS1 | FLUSH1));
	FIFOLOAD(BIT_DATA, PTR(msg), 4003, WITH(CLASS2 | LAST2 | EXT));

	FIFOLOAD(MSGOUTSNOOP, PTR(src_dma), 0x212, WITH(LAST1 | LAST2 | SGF));
	FIFOLOAD(MSGINSNOOP, PTR(src_dma), 0x212, WITH(LAST1 | LAST2 | SGF));
	FIFOLOAD(AAD1, PTR(aad_ptr), 800, WITH(SGF));
	FIFOLOAD(AAD1, PTR(aad_ptr), 800, 0);
	FIFOLOAD(ICV1, PTR(icv_ptr), 12, WITH(LAST1));
	FIFOLOAD(ICV1, PTR(icv_ptr), 12, WITH(FLUSH1 | LAST2 | BOTH | EXT));
	FIFOLOAD(ICV1, PTR(icv_ptr), 12, WITH(FLUSH1));
	FIFOLOAD(ICV2, PTR(icv_ptr), 12, WITH(LAST2));
	FIFOLOAD(ICV2, PTR(icv_ptr), 12, WITH(LAST2));
	FIFOLOAD(IV1, PTR(iv_ptr), 12, WITH(LAST1));
	FIFOLOAD(IV1, PTR(iv_ptr), 12, WITH(LAST1));

	FIFOLOAD(MSGINSNOOP, IMM(0x616263), 3, WITH(LAST1 | LAST2));
	if (rta_sec_era >= RTA_SEC_ERA_3)
		FIFOLOAD(IFIFO, PTR(auth_ptr), 12, 0);
	SEQFIFOLOAD(SKIP, 0, WITH(VLF));
	FIFOLOAD(PKA, IMM(0), 0, 0);
	SEQFIFOLOAD(PKN, 0, 0);

	FIFOLOAD(PKA, IMM(0x07), 1, 0);
	FIFOLOAD(PKB, IMM(0x07), 1, 0);
	FIFOLOAD(PKA0, IMM(0x07), 1, 0);
	FIFOLOAD(PKA1, IMM(0x03), 1, 0);
	FIFOLOAD(PKA2, IMM(0x04), 1, 0);
	FIFOLOAD(PKA3, IMM(0x12131415), 4, 0);
	FIFOLOAD(PKB0, IMM(0x07), 1, 0);
	FIFOLOAD(PKB1, IMM(0x03), 1, 0);
	FIFOLOAD(PKB2, IMM(0x04), 1, 0);
	FIFOLOAD(PKB3, IMM(0x12131415), 4, 0);
	size = FIFOLOAD(PKN, PTR(mod), 12, 0);

	return size;
}

int main(int argc, char **argv)
{
	int size;

	printf("FIFOLOAD program\n");
	rta_set_sec_era(RTA_SEC_ERA_3);
	size = test_fifo_load_op((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
