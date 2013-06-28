#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;

unsigned test_blob_example(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;

	/* I/O address of key modifier */
	uint32_t key_mod_addr = 0x092BEEBE;
	/* I/O address of data to be put in blob */
	uint32_t data_in_addr = 0x37D49F77;
	/* I/O address of blob */
	uint32_t blob_out_addr = 0x419D8FBF;
	uint32_t data_length = 75;
	uint32_t blob_length = data_length + 32 + 16;

	PROGRAM_CNTXT_INIT(buff, 0);
	JOB_HDR(SHR_NEVER, 0, 0, 0);
	{
		KEY(KEY2, 0, PTR((uintptr_t) key_mod_addr), 16, 0);
		SEQINPTR((uintptr_t) data_in_addr, data_length, 0);
		SEQOUTPTR((uintptr_t) blob_out_addr, blob_length, 0);
		PROTOCOL(OP_TYPE_ENCAP_PROTOCOL, OP_PCLID_BLOB,
			 WITH(OP_PCL_BLOB_FORMAT_NORMAL));
	}

	return PROGRAM_FINALIZE();
}

uint32_t prg_buff[1000];

int main(int argc, char **argv)
{
	unsigned size;

	pr_debug("BLOB example program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = test_blob_example(prg_buff);
	pr_debug("size = %d\n", size);
	print_prog(prg_buff, size);

	return 0;
}
