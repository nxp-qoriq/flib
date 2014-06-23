/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;

unsigned test_nfifo_op(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;

	PROGRAM_CNTXT_INIT(buff, 0);
	/* From Input Data FIFO to 'all' targets */
	NFIFOADD(IFIFO, MSG1, 2045, 0);
	NFIFOADD(IFIFO, MSG1, 2046, LAST1);
	NFIFOADD(IFIFO, MSG1, 2047, FLUSH1);
	NFIFOADD(IFIFO, MSG1, 2048, LAST1 | FLUSH1);

	NFIFOADD(IFIFO, MSG2, 2043, LAST2);

	NFIFOADD(IFIFO, MSG2, 2044, FLUSH2);
	NFIFOADD(IFIFO, MSG2, 2045, 0);

	NFIFOADD(IFIFO, IV1, 45, 0);
	NFIFOADD(IFIFO, IV1, 46, LAST1);
	NFIFOADD(IFIFO, IV1, 47, FLUSH1);
	NFIFOADD(IFIFO, IV1, 48, LAST1 | FLUSH1);

	NFIFOADD(IFIFO, ICV1, 45, 0);
	NFIFOADD(IFIFO, ICV1, 46, LAST1);
	NFIFOADD(IFIFO, ICV1, 47, FLUSH1);
	NFIFOADD(IFIFO, ICV1, 48, LAST1 | FLUSH1);

	NFIFOADD(IFIFO, ICV2, 44, LAST2);
	NFIFOADD(IFIFO, ICV2, 45, 0);

	NFIFOADD(IFIFO, PKA, 512, 0);
	NFIFOADD(IFIFO, PKA0, 512, FLUSH1);
	NFIFOADD(IFIFO, PKA1, 512, FLUSH1);
	NFIFOADD(IFIFO, PKA2, 512, 0);
	NFIFOADD(IFIFO, PKA3, 512, FLUSH1);

	NFIFOADD(IFIFO, PKB, 512, 0);
	NFIFOADD(IFIFO, PKB0, 512, FLUSH1);
	NFIFOADD(IFIFO, PKB1, 512, 0);
	NFIFOADD(IFIFO, PKB2, 512, FLUSH1);
	NFIFOADD(IFIFO, PKB3, 512, 0);

	NFIFOADD(IFIFO, PKN, 512, 0);
	NFIFOADD(IFIFO, PKE, 512, FLUSH1);

	NFIFOADD(IFIFO, AFHA_SBOX, 258, 0);

	/* From Output Data FIFO to 'all' targets */
	NFIFOADD(OFIFO, MSG1, 24, 0);
	/* From Outsnooping to 'all' targets (really all types) */
	NFIFOADD(MSGOUTSNOOP, MSG, 2045, 0);

	/* From Padding Block to 'all' targets */
	NFIFOADD(PAD, AB1, 16, PAD_ZERO);
	NFIFOADD(PAD, AB1, 16, PAD_NONZERO);
	NFIFOADD(PAD, AB1, 16, PAD_INCREMENT);
	NFIFOADD(PAD, AB1, 16, PAD_RANDOM);
	NFIFOADD(PAD, AB1, 16, PAD_ZERO_N1);
	NFIFOADD(PAD, AB1, 16, PAD_NONZERO_0);
	NFIFOADD(PAD, AB1, 16, PAD_N1);
	NFIFOADD(PAD, AB1, 16, PAD_NONZERO_N);

	/* From Alt Source to 'all' targets */
	NFIFOADD(ALTSOURCE, IV1, 24, FLUSH1);
	NFIFOADD(IFIFO, MSG, 2045, EXT);

	/* Boundary padding */
	NFIFOADD(IFIFO, ICV1, 45, BP | PAD_ZERO);
	NFIFOADD(IFIFO, ICV1, 45, BP | PAD_INCREMENT);
	NFIFOADD(PAD, AB1, 16, BP | PAD_NONZERO_N);

	return PROGRAM_FINALIZE();
}

uint32_t prg_buff[1000];

int main(int argc, char **argv)
{
	unsigned size;

	pr_debug("NFIFO ADD program\n");
	rta_set_sec_era(RTA_SEC_ERA_2);
	size = test_nfifo_op(prg_buff);
	pr_debug("size = %d\n", size);
	print_prog(prg_buff, size);

	return 0;
}
