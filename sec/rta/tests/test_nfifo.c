#include <stdio.h>
#include "rta.h"

int test_nfifo_op(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;

	PROGRAM_CNTXT_INIT(buff, 0, 0);
	/* From Input Data FIFO to 'all' targets */
	NFIFOADD(IFIFO, MSG1, 2045, 0);
	NFIFOADD(IFIFO, MSG1, 2046, WITH(LAST1));
	NFIFOADD(IFIFO, MSG1, 2047, WITH(FLUSH1));
	NFIFOADD(IFIFO, MSG1, 2048, WITH(LAST1 | FLUSH1));

	NFIFOADD(IFIFO, MSG2, 2044, WITH(LAST2));
	NFIFOADD(IFIFO, MSG2, 2045, 0);

	NFIFOADD(IFIFO, IV1, 45, 0);
	NFIFOADD(IFIFO, IV1, 46, WITH(LAST1));
	NFIFOADD(IFIFO, IV1, 47, WITH(FLUSH1));
	NFIFOADD(IFIFO, IV1, 48, WITH(LAST1 | FLUSH1));

	NFIFOADD(IFIFO, IV2, 44, WITH(LAST2));
	NFIFOADD(IFIFO, IV2, 45, 0);

	NFIFOADD(IFIFO, ICV1, 45, 0);
	NFIFOADD(IFIFO, ICV1, 46, WITH(LAST1));
	NFIFOADD(IFIFO, ICV1, 47, WITH(FLUSH1));
	NFIFOADD(IFIFO, ICV1, 48, WITH(LAST1 | FLUSH1));

	NFIFOADD(IFIFO, ICV2, 44, WITH(LAST2));
	NFIFOADD(IFIFO, ICV2, 45, 0);

	NFIFOADD(IFIFO, PKA, 512, 0);
	NFIFOADD(IFIFO, PKA0, 512, WITH(FLUSH1));
	NFIFOADD(IFIFO, PKA1, 512, WITH(FLUSH1));
	NFIFOADD(IFIFO, PKA2, 512, 0);
	NFIFOADD(IFIFO, PKA3, 512, WITH(FLUSH1));

	NFIFOADD(IFIFO, PKB, 512, 0);
	NFIFOADD(IFIFO, PKB0, 512, WITH(FLUSH1));
	NFIFOADD(IFIFO, PKB1, 512, 0);
	NFIFOADD(IFIFO, PKB2, 512, WITH(FLUSH1));
	NFIFOADD(IFIFO, PKB3, 512, 0);

	NFIFOADD(IFIFO, PKN, 512, 0);
	NFIFOADD(IFIFO, PKE, 512, WITH(FLUSH1));

	NFIFOADD(IFIFO, AFHA_SBOX, 258, 0);

	/* From Output Data FIFO to 'all' targets */
	NFIFOADD(OFIFO, MSG1, 24, 0);
	/* From Outsnooping to 'all' targets (really all types) */
	NFIFOADD(MSGOUTSNOOP, MSG, 2045, 0);

	/* From Padding Block to 'all' targets */
	NFIFOADD(PAD, AB1, 16, WITH(PAD_ZERO));
	NFIFOADD(PAD, AB1, 16, WITH(PAD_NONZERO));
	NFIFOADD(PAD, AB1, 16, WITH(PAD_INCREMENT));
	NFIFOADD(PAD, AB1, 16, WITH(PAD_RANDOM));
	NFIFOADD(PAD, AB1, 16, WITH(PAD_ZERO_N1));
	NFIFOADD(PAD, AB1, 16, WITH(PAD_NONZERO_0));
	NFIFOADD(PAD, AB1, 16, WITH(PAD_N1));
	NFIFOADD(PAD, AB1, 16, WITH(PAD_NONZERO_N));

	/* From Alt Source to 'all' targets */
	NFIFOADD(ALTSOURCE, IV2, 24, WITH(FLUSH2));
	NFIFOADD(IFIFO, ABD, 2045, WITH(EXT));

	/* Boundary padding */
	NFIFOADD(IFIFO, ICV1, 45, WITH(BP | PAD_ZERO));
	NFIFOADD(IFIFO, ICV1, 45, WITH(BP | PAD_INCREMENT));
	size = NFIFOADD(PAD, AB1, 16, WITH(BP | PAD_NONZERO_N));

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
	printf("NFIFO ADD program\n");
	size = test_nfifo_op((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);
	return 0;
}
