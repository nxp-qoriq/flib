#include <stdio.h>
#include "flib/rta.h"

uint rta_sec_era;
int prg_buff[1000];

static void print_prog(uint32_t *buff, int size)
{
	int i;
	for (i = 0; i < size; i++)
		printf("0x%08x\n", buff[i]);
}

int test_seq_ptr_op(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size, data_size = 512;
	uint64_t data_in = 0x1234567;
	uint64_t data_out = 0x6123475;

	PROGRAM_CNTXT_INIT(buff, 0);
	SEQINPTR(data_in, data_size, 0);
	SEQOUTPTR(data_out, data_size, 0);
	SEQINPTR(0, 0, WITH(RTO));
	size = SEQOUTPTR(0, 0, WITH(RTO));

	return size;
}

int main(int argc, char **argv)
{
	int size;

	printf("SEQ PTR program\n");
	rta_set_sec_era(1);
	size = test_seq_ptr_op((uint32_t *) prg_buff);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
