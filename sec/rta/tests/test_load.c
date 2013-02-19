#include <stdio.h>
#include "flib/rta.h"

enum rta_sec_era rta_sec_era;
int prg_buff[1000];

static void print_prog(uint32_t *buff, int size)
{
	int i;
	for (i = 0; i < size; i++)
		printf("%08X\n", buff[i]);
}

const uint8_t abc[] = { 0x61, 0x62, 0x63 };

int test_load_op(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	uint32_t four = 4;
	uint32_t *sgtable = (uint32_t *) 0x12010288;
	int keylen = 32;
	void *descwords = (int *)0xaabbccddeeull;
	uint8_t *ctx = (uint8_t *) 0x10288;
	int ctxoff = 5;
	void *foo = (int *)0xabacbdcedeull;
	int size;

	LABEL(here);
	REFERENCE(here_ref);
	LABEL(where);
	REFERENCE(where_ref);

	PROGRAM_CNTXT_INIT(buff, 0);
	LOAD(IMM(03), CONTEXT1, 0, 1, 0);
	LOAD(IMM(0x616263), IFIFO, 0, 3, 0);
	LOAD(IMM(four), PKNSZ, 0, 4, 0);
	LOAD(PTR((intptr_t) ctx), CONTEXT2, ctxoff, 16, 0);
	LOAD(PTR((intptr_t) sgtable), KEY1, 0, keylen, WITH(SGF));
	LOAD(PTR((intptr_t) descwords), DESCBUF, 20, 4, 0);

	here_ref = SET_LABEL(here);
	LOAD(PTR((intptr_t) descwords), DESCBUF, here, 4, 0);
	SEQLOAD(DESCBUF, here, 4, 0);
	PATCH_LOAD(program, here_ref, here);

	where_ref = SET_LABEL(where);
	LOAD(PTR((intptr_t) descwords), DESCBUF, where, 4, 0);
	SEQLOAD(DESCBUF, where, 4, 0);
	PATCH_LOAD(program, where_ref, where);

	LOAD(IMM(0x00000003), CLRW, 0, 4, 0);
	LOAD(IMM(0x00200003), CCTRL, 0, 4, 0);
	LOAD(IMM(0x00000010), ICTRL, 0, 4, 0);
	LOAD(IMM(0), DCTRL, 8, 4, 0);
	LOAD(IMM(00000003), DATA1SZ, 0, 4, 0);
	LOAD(IMM(8), ALTDS1, 0, 4, 0);
	LOAD(IMM(0x00000303), DATA2SZ, 0, 4, 0);
	LOAD(IMM(0x00000003), ICV1SZ, 0, 4, 0);
	LOAD(IMM(0x00000303), ICV2SZ, 0, 4, 0);
	LOAD(IMM(0x010202), CONTEXT1, 0, 3, 0);
	LOAD(IMM(0x010202), CONTEXT2, 0, 3, 0);
	LOAD(IMM(0x00000003), IV1SZ, 0, 4, 0);
	LOAD(IMM(0x010202), KEY1, 3, 3, 0);
	LOAD(IMM(0x00000003), KEY1SZ, 0, 4, 0);
	LOAD(IMM(0x010202), KEY2, 0, 3, 0);
	LOAD(IMM(0x00000303), KEY2SZ, 0, 4, 0);
	LOAD(IMM(0x00000003), AAD1SZ, 0, 4, 0);
	LOAD(IMM(0x00000003), DPOVRD, 0, 4, 0);
	LOAD(IMM(0x00000303), MATH0, 0, 4, 0);
	LOAD(IMM(3), MATH1, 0, 1, 0);
	LOAD(IMM(0x0300000000000000), MATH2, 0, 8, 0);
	LOAD(IMM(3), MATH3, 0, 1, 0);
	LOAD(IMM(0x00000080), PKASZ, 0, 4, 0);
	LOAD(IMM(0x00000080), PKBSZ, 0, 4, 0);
	LOAD(IMM(0x00000080), PKNSZ, 0, 4, 0);
	LOAD(IMM(0x00000080), PKESZ, 0, 4, 0);
	LOAD(IMM(0x616263), IFIFO, 0, 3, 0);
	LOAD(IMM(0x616263), OFIFO, 0, 3, 0);
	LOAD(IMM(1), IDFNS, 0, 1, 0);
	LOAD(IMM(2), ODFNS, 0, 1, 0);
	LOAD(IMM((intptr_t) foo), DPID, 0, 8, 0);
	LOAD(IMM(0x0303303330001223), ALTSOURCE, 0, 8, 0);
	LOAD(IMM(0x02000003), NFIFO, 0, 4, 0);
	LOAD(IMM(0x0200000000000003), NFIFO, 0, 8, 0);
	LOAD(IMM(0x02000003), NFIFOSZ, 0, 4, 0);
	LOAD(IMM(0x0200000000000003), NFIFOSZ, 0, 8, 0);
	LOAD(IMM(0x02000003), NFIFO, 0, 4, 0);
	LOAD(IMM(0x0200000000000003), NFIFO, 0, 8, 0);
	LOAD(IMM(0x02000003), SZ, 0, 4, 0);
	size = LOAD(IMM(0x0200000000000003), SZ, 0, 8, 0);

	return size;
}

int main(int argc, char **argv)
{
	int size;

	printf("LOAD program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = test_load_op((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
