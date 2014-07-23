/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;
uint32_t prg_buff[1000];

unsigned test_fifo_load_op(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	uint64_t msg = 0x12004201;
	uint64_t aad_ptr = 0x200;
	uint64_t src_dma = 0x800;
	uint64_t iv_ptr = 0x02230104;
	uint64_t icv_ptr = 0xf3230d01;
	uint64_t auth_ptr = 0x13422591;
	uint64_t mod = 0x24128820;

	PROGRAM_CNTXT_INIT(buff, 0);

	FIFOLOAD(MSG1, msg, 1, FLUSH1);
	FIFOLOAD(MSG1, msg, 2, LAST1);
	FIFOLOAD(MSG2, msg, 3, LAST2);
	FIFOLOAD(MSG2, msg, 4, 0);

	FIFOLOAD(BIT_DATA, msg, 4003, CLASS1 | FLUSH1);
	FIFOLOAD(BIT_DATA, msg, 4003, CLASS2 | LAST2 | EXT);

	FIFOLOAD(MSGOUTSNOOP, src_dma, 0x212, LAST1 | LAST2 | SGF);
	FIFOLOAD(MSGINSNOOP, src_dma, 0x212, LAST1 | LAST2 | SGF);
	FIFOLOAD(AAD1, aad_ptr, 800, SGF);
	FIFOLOAD(AAD1, aad_ptr, 800, 0);
	FIFOLOAD(ICV1, icv_ptr, 12, LAST1);
	FIFOLOAD(ICV1, icv_ptr, 12, FLUSH1 | LAST2 | BOTH | EXT);
	FIFOLOAD(ICV1, icv_ptr, 12, FLUSH1);
	FIFOLOAD(ICV2, icv_ptr, 12, LAST2);
	FIFOLOAD(ICV2, icv_ptr, 12, LAST2);
	FIFOLOAD(IV1, iv_ptr, 12, LAST1);
	FIFOLOAD(IV1, iv_ptr, 12, LAST1);

	FIFOLOAD(MSGINSNOOP, 0x616263, 3, LAST1 | LAST2 | IMMED);
	if (rta_sec_era >= RTA_SEC_ERA_3)
		FIFOLOAD(IFIFO, auth_ptr, 12, 0);
	SEQFIFOLOAD(SKIP, 0, VLF);
	FIFOLOAD(PKA, 0, 0, IMMED | COPY);
	SEQFIFOLOAD(PKN, 0, 0);

	FIFOLOAD(PKA, 0x07, 1, IMMED);
	FIFOLOAD(PKB, 0x07, 1, IMMED);
	FIFOLOAD(PKA0, 0x07, 1, IMMED);
	FIFOLOAD(PKA1, 0x03, 1, IMMED);
	FIFOLOAD(PKA2, 0x04, 1, IMMED);
	FIFOLOAD(PKA3, 0x12131415, 4, IMMED);
	FIFOLOAD(PKB0, 0x07, 1, IMMED);
	FIFOLOAD(PKB1, 0x03, 1, IMMED);
	FIFOLOAD(PKB2, 0x04, 1, IMMED);
	FIFOLOAD(PKB3, 0x12131415, 4, IMMED);
	FIFOLOAD(PKN, mod, 12, 0);

	return PROGRAM_FINALIZE();
}

int main(int argc, char **argv)
{
	unsigned size;

	pr_debug("FIFOLOAD program\n");
	rta_set_sec_era(RTA_SEC_ERA_3);
	size = test_fifo_load_op(prg_buff);
	pr_debug("size = %d\n", size);
	print_prog(prg_buff, size);

	return 0;
}
