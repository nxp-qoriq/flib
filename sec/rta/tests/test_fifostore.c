/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;
uint32_t prg_buff[1000];

unsigned test_fifo_store_op(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	uint64_t loc = 0x1224445;

	PROGRAM_CNTXT_INIT(buff, 0);
	FIFOSTORE(MSG, 0, loc, 100000000, 0);
	FIFOSTORE(RNG, 0, loc, 32, 0);
	FIFOSTORE(RNGOFIFO, 0, 0, 1, 0);
	FIFOSTORE(PKA, 0, loc, 10, 0);
	FIFOSTORE(PKA0, 0, loc, 10, 0);
	FIFOSTORE(PKA1, 0, loc, 10, 0);
	FIFOSTORE(PKA2, 0, loc, 40, 0);
	FIFOSTORE(PKA3, 0, loc, 10, 0);
	FIFOSTORE(PKB, 0, loc, 10, 0);
	FIFOSTORE(PKB0, 0, loc, 10, 0);
	FIFOSTORE(PKB1, 0, loc, 10, 0);
	FIFOSTORE(PKB2, 0, loc, 10, 0);
	FIFOSTORE(PKB3, 0, loc, 10, 0);
	FIFOSTORE(PKN, 0, loc, 10, 0);
	/* Encrypted (keys) section - normal */
	FIFOSTORE(KEY1, 0, loc, 32, 0);
	FIFOSTORE(KEY2, 0, loc, 16, 0);
	FIFOSTORE(PKE, 0, loc, 10, 0);
	FIFOSTORE(AFHA_SBOX, 0, loc, 258, 0);
	FIFOSTORE(MDHA_SPLIT_KEY, 0, loc, 128, 0);
	/* Encrypted (keys) section - enhanced */
	FIFOSTORE(KEY1, EKT, loc, 32, 0);
	FIFOSTORE(KEY2, EKT, loc, 16, 0);
	FIFOSTORE(PKE, EKT, loc, 10, 0);
	FIFOSTORE(AFHA_SBOX, EKT, loc, 258, 0);
	FIFOSTORE(MDHA_SPLIT_KEY, EKT, loc, 128, 0);
	/* Encrypted (keys) section - trusted */
	FIFOSTORE(KEY1, TK, loc, 32, 0);
	FIFOSTORE(KEY2, TK, loc, 16, 0);
	FIFOSTORE(PKE, TK, loc, 10, 0);
	FIFOSTORE(AFHA_SBOX, TK, loc, 258, 0);
	FIFOSTORE(MDHA_SPLIT_KEY, TK, loc, 128, 0);
	/* Encrypted (keys) section - enhanced, trusted */
	FIFOSTORE(KEY1, EKT | TK, loc, 32, 0);
	FIFOSTORE(KEY2, EKT | TK, loc, 16, 0);
	FIFOSTORE(PKE, EKT | TK, loc, 10, 0);
	FIFOSTORE(AFHA_SBOX, EKT | TK, loc, 258, 0);
	FIFOSTORE(MDHA_SPLIT_KEY, EKT | TK, loc, 128, 0);
	SEQFIFOSTORE(MSG, 0, 12, CONT);
	SEQFIFOSTORE(MSG, 0, 0, VLF);
	SEQFIFOSTORE(SKIP, 0, 33, 0);
	SEQFIFOSTORE(SKIP, 0, 0, VLF);
	SEQFIFOSTORE(METADATA, 0, 24, 0);
	SEQFIFOSTORE(METADATA, 0, 0, VLF);
	SEQFIFOSTORE(METADATA, 0, 13, CLASS1);
	SEQFIFOSTORE(METADATA, 0, 0, CLASS1 | VLF);
	SEQFIFOSTORE(METADATA, 0, 24, CLASS2);
	SEQFIFOSTORE(METADATA, 0, 0, CLASS2 | VLF);
	SEQFIFOSTORE(METADATA, 0, 92, BOTH);
	SEQFIFOSTORE(METADATA, 0, 0, BOTH | VLF);

	return PROGRAM_FINALIZE();
}

int main(int argc, char **argv)
{
	unsigned size;

	pr_debug("FIFOSTORE program\n");
	rta_set_sec_era(RTA_SEC_ERA_5);
	size = test_fifo_store_op(prg_buff);
	pr_debug("size = %d\n", size);
	print_prog(prg_buff, size);

	return 0;
}
