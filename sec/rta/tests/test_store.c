#include <stdio.h>
#include "flib/rta.h"

enum rta_sec_era rta_sec_era;
int prg_buff[1000];

int test_store(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;
	uint8_t *ctx = (uint8_t *) 0x55330087ull;
	uint32_t *sgtable = (uint32_t *) 0x22223333ull;
	int ivlen = 16;
	void *foo = (void *)0x34000080ull;
	uint32_t *descwords = (uint32_t *) 0x12889980ull;

	LABEL(here);

	PROGRAM_CNTXT_INIT(buff, 0);
	/* Class-independent CCB registers */
	STORE(CLRW, 0, PTR((intptr_t) foo), 4, 0);
	STORE(CCTRL, 0, PTR((intptr_t) foo), 4, 0);
	STORE(ICTRL, 0, PTR((intptr_t) foo), 4, 0);
	STORE(CSTAT, 0, PTR((intptr_t) foo), 4, 0);

	/* Class 1 CCB registers */
	STORE(MODE1, 0, PTR((intptr_t) foo), 4, 0);
	STORE(KEY1SZ, 0, PTR((intptr_t) foo), 4, 0);
	STORE(DATA1SZ, 0, PTR((intptr_t) foo), 4, 0);
	STORE(ICV1SZ, 0, PTR((intptr_t) foo), 4, 0);
	/* Class 1 Additional Data Size */
	STORE(AAD1SZ, 0, PTR((intptr_t) foo), 4, 0);
	STORE(IV1SZ, 0, PTR((intptr_t) foo), 4, 0);
	STORE(PKASZ, 0, PTR((intptr_t) foo), 4, 0);
	STORE(PKBSZ, 0, PTR((intptr_t) foo), 4, 0);
	STORE(PKNSZ, 0, PTR((intptr_t) foo), 4, 0);
	STORE(PKESZ, 0, PTR((intptr_t) foo), 4, 0);

	STORE(CONTEXT1, 0, PTR((intptr_t) sgtable), ivlen, WITH(SGF));
	STORE(CONTEXT1, 5, PTR((intptr_t) foo), 7, 0);

	/* Class 2 CCB registers */
	STORE(MODE2, 0, PTR((intptr_t) foo), 4, 0);
	STORE(KEY2SZ, 0, PTR((intptr_t) foo), 4, 0);
	STORE(DATA2SZ, 0, PTR((intptr_t) foo), 4, 0);
	STORE(ICV2SZ, 0, PTR((intptr_t) foo), 4, 0);
	STORE(CONTEXT2, 0, PTR((intptr_t) ctx), 16, 0);
	STORE(CONTEXT2, 8, PTR((intptr_t) foo), 4, 0);

	/* DECO / CAAM registers */
	STORE(DJQCTRL, 0, PTR((intptr_t) foo), 8, 0);
	/* DECO JQ Descriptor Address */
	STORE(DJQDA, 0, PTR((intptr_t) foo), 8, 0);
	STORE(DSTAT, 0, PTR((intptr_t) foo), 8, 0);
	STORE(DPID, 0, PTR((intptr_t) foo), 8, 0);

	SEQSTORE(MATH0, 0, 1, 0);
	SEQSTORE(MATH1, 0, 0, WITH(VLF));
	STORE(MATH2, 0, PTR((intptr_t) foo), 4, 0);
	STORE(MATH3, 0, PTR((intptr_t) foo), 4, 0);
	STORE(DESCBUF, 20, PTR((intptr_t) descwords), 4, 0);

	SET_LABEL(here);
	STORE(DESCBUF, here, PTR((intptr_t) descwords), 8, 0);
	size = SEQSTORE(DESCBUF, here, 12, 0);

	return size;
}

static void print_prog(uint32_t *buff, int size)
{
	int i;
	for (i = 0; i < size; i++)
		printf("%08x\n", buff[i]);
}

int main(int argc, char **argv)
{
	int size;

	printf("STORE program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = test_store((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
