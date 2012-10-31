#include <stdio.h>
#include "flib/rta.h"

uint rta_sec_era;

int test_signature_op(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;

	PROGRAM_CNTXT_INIT(buff, 0);
	{
		SIGNATURE(SIGN_TYPE_IMM_2);
		SIGNATURE(SIGN_TYPE_IMM_3);
		SIGNATURE(SIGN_TYPE_IMM_4);
		SIGNATURE(SIGN_TYPE_FINAL);
		DWORD(0x0000000000000000);
		DWORD(0x0000000000000000);
		DWORD(0x0000000000000000);
		DWORD(0x0000000000000000);

		SIGNATURE(SIGN_TYPE_FINAL_RESTORE);
		DWORD(0x0000000000000000);
		DWORD(0x0000000000000000);
		DWORD(0x0000000000000000);
		DWORD(0x0000000000000000);

		SIGNATURE(SIGN_TYPE_FINAL_NONZERO);
		DWORD(0x0000000000000000);
		DWORD(0x0000000000000000);
		DWORD(0x0000000000000000);
		DWORD(0x0000000000000000);

		SIGNATURE(SIGN_TYPE_FINAL);
		DWORD(0x123454320002222);
		DWORD(0x123454320002222);
		DWORD(0x123454320002222);
		DWORD(0x123454320002222);

		SIGNATURE(SIGN_TYPE_FINAL_RESTORE);
		DWORD(0x123454320002222);
		DWORD(0x123454320002222);
		DWORD(0x123454320002222);
		DWORD(0x123454320002222);

		SIGNATURE(SIGN_TYPE_FINAL_NONZERO);
		DWORD(0x123454320002222);
		DWORD(0x123454320002222);
		DWORD(0x123454320002222);
		size = DWORD(0x123454320002222);
	}

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

	printf("SIGNATURE program\n");
	rta_set_sec_era(1);
	size = test_signature_op((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
