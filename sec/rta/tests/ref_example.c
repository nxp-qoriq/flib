#include <stdio.h>
#include "rta.h"

struct program capwap_program;
struct program job_desc_snippet;

uint32_t capwap_buf[1000];
uint32_t job_buf[1000];

LABEL(label_different_desc);
REFERENCE(label_different_ref);

int generate_capwap_code(struct program *program, uint32_t *buff)
{
	int size;

	PROGRAM_CNTXT_INIT(buff, 0, 0);
	SHR_HDR(SHR_ALWAYS, 0, 0);

	label_different_ref =
	    JUMP(IMM(label_different_desc), LOCAL_JUMP, ALL_TRUE, 0);
	size = PROGRAM_FINALIZE();
	return size;
}

int generate_job_desc(struct program *program, uint32_t *buff)
{
	int size;
	PROGRAM_CNTXT_INIT(buff, 0x20, 0);
	SET_LABEL(label_different_desc);
	MATHB(MATH2, XOR, MATH1, MATH3, 4, 0);
	MATHU(MATH2, BSWAP, MATH3, 2, WITH(NFU));
	size = PROGRAM_FINALIZE();
	return size;
}

int main(int argc, char **argv)
{
	generate_capwap_code(&capwap_program, capwap_buf);
	generate_job_desc(&job_desc_snippet, job_buf);
	patch_jump_non_local(&capwap_program, label_different_ref,
			     &job_desc_snippet, label_different_desc);
	return 0;
}
