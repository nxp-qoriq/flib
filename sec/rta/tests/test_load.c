/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;
uint32_t prg_buff[1000];

const uint8_t abc[] = { 0x61, 0x62, 0x63 };

unsigned test_load_op(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	uint32_t four = 4;
	uintptr_t abc_ptr = (uintptr_t) 0x22003344ul;
	uint32_t *sgtable = (uint32_t *) 0x12010288;
	int keylen = 32;
	void *descwords = (int *)0xaabbccddeeull;
	uint8_t *ctx = (uint8_t *) 0x10288;
	int ctxoff = 5;
	uintptr_t foo = (uintptr_t) 0x000000abacbdcedeull;
	int word_size = sizeof(uint32_t);
	int here = 32;
	int where = 24;

	PROGRAM_CNTXT_INIT(buff, 0);
	LOAD(IMM(03), CONTEXT1, 0, 1, 0);
	LOAD(IMM(0x616263), IFIFO, 0, 3, 0);
	LOAD(IMM(four), PKNSZ, 0, 4, 0);
	LOAD(PTR((uintptr_t) ctx), CONTEXT2, ctxoff, 16, 0);
	LOAD(PTR((uintptr_t) sgtable), KEY1, 0, keylen, WITH(SGF));
	LOAD(PTR((uintptr_t) descwords), DESCBUF, 20 * word_size, 4 * word_size,
	     0);

	LOAD(PTR((uintptr_t) descwords), DESCBUF, here, 4 * word_size, 0);
	SEQLOAD(DESCBUF, here, 4 * word_size, 0);

	LOAD(PTR((uintptr_t) descwords), DESCBUF, where, 4 * word_size, 0);
	SEQLOAD(DESCBUF, where, 4 * word_size, 0);

	LOAD(IMM(0x00000003), CLRW, 0, 4, 0);
	LOAD(IMM(0x00200003), CCTRL, 0, 4, 0);
	LOAD(IMM(0x00000010), ICTRL, 0, 4, 0);
	LOAD(IMM(0), DCTRL, 8, 4, 0);
	LOAD(IMM(00000003), DATA1SZ, 0, 4, 0);
	LOAD(IMM(8), ALTDS1, 0, 4, 0);
	LOAD(IMM(0x00000303), DATA2SZ, 0, 4, 0);
	LOAD(IMM(0x00000003), ICV1SZ, 0, 4, 0);
	LOAD(IMM(0x00000304), ICV2SZ, 0, 4, 0);
	LOAD(IMM(0x010202), CONTEXT1, 0, 3, 0);
	LOAD(IMM(0x010203), CONTEXT2, 0, 3, 0);
	LOAD(IMM(0x00000003), IV1SZ, 0, 4, 0);
	LOAD(IMM(0x010204), KEY1, 3, 3, 0);
	LOAD(IMM(0x00000003), KEY1SZ, 0, 4, 0);
	LOAD(IMM(0x010205), KEY2, 0, 3, 0);
	LOAD(IMM(0x00000305), KEY2SZ, 0, 4, 0);
	LOAD(IMM(0x00000003), AAD1SZ, 0, 4, 0);
	LOAD(IMM(0x00000003), DPOVRD, 0, 4, 0);
	LOAD(IMM(0x00000003), DPOVRD, 0, 4, 0);
	LOAD(IMM(0x00000306), MATH0, 0, 4, 0);
	LOAD(IMM(3), MATH1, 0, 1, 0);
	LOAD(IMM(0x0300000000000000), MATH2, 0, 8, 0);
	LOAD(IMM(3), MATH3, 0, 1, 0);
	LOAD(IMM(0x00000080), PKASZ, 0, 4, 0);
	LOAD(IMM(0x00000080), PKBSZ, 0, 4, 0);
	LOAD(IMM(0x00000080), PKNSZ, 0, 4, 0);
	LOAD(IMM(0x00000080), PKESZ, 0, 4, 0);
	LOAD(IMM(abc_ptr), IFIFO, 0, sizeof(abc), 0);
	LOAD(IMM(abc_ptr), OFIFO, 0, sizeof(abc), 0);
	LOAD(IMM(1), IDFNS, 0, 1, 0);
	LOAD(IMM(2), ODFNS, 0, 1, 0);
	LOAD(IMM(foo), DPID, 0, 8, 0);
	LOAD(IMM(0x0303303330001223), ALTSOURCE, 0, 8, 0);
	LOAD(IMM(0x02000003), NFIFO, 0, 4, 0);
	LOAD(IMM(0x0200000000000003), NFIFO, 0, 8, 0);
	LOAD(IMM(0x02000003), NFIFO_SZL, 0, 4, 0);
	LOAD(IMM(0x02f00003), NFIFO_SZL, 0, 4, 0);
	LOAD(IMM(0x0200000000000003), NFIFO_SZL, 0, 8, 0);
	LOAD(IMM(0x02f0000000000003), NFIFO_SZL, 0, 8, 0);
	LOAD(IMM(0x02000000), NFIFO_SZM, 0, 1, 0);
	LOAD(IMM(0x02f00000), NFIFO_SZM, 0, 1, 0);
	LOAD(IMM(0x02000003), NFIFO_L, 0, 4, 0);
	LOAD(IMM(0x02f00003), NFIFO_L, 0, 4, 0);
	LOAD(IMM(0x0200000000000003), NFIFO_L, 0, 8, 0);
	LOAD(IMM(0x02f0000000000003), NFIFO_L, 0, 8, 0);
	LOAD(IMM(0x02030000), NFIFO_M, 0, 2, 0);
	LOAD(IMM(0x02f00000), NFIFO_M, 0, 2, 0);
	LOAD(IMM(0x02000003), SZL, 0, 4, 0);
	LOAD(IMM(0x42f00003), SZL, 0, 4, 0);
	LOAD(IMM(0x0200000000000003), SZL, 0, 8, 0);
	LOAD(IMM(0x42f2000000000003), SZL, 0, 8, 0);
	LOAD(IMM(0x02000000), SZM, 0, 3, 0);
	LOAD(IMM(0x82f60000), SZM, 0, 3, 0);

	return PROGRAM_FINALIZE();
}

int main(int argc, char **argv)
{
	unsigned size;

	pr_debug("LOAD program\n");
	rta_set_sec_era(RTA_SEC_ERA_4);
	size = test_load_op(prg_buff);
	pr_debug("size = %d\n", size);
	print_prog(prg_buff, size);

	return 0;
}
