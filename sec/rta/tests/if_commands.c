/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;

unsigned if_command_test(uint32_t *buff, uint8_t len)
{
	struct program prg;
	struct program *program = &prg;

	PROGRAM_SET_36BIT_ADDR();

	PROGRAM_CNTXT_INIT(buff, 0);

	if (len == 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, 15, IMMED);
	if (len > 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, 16, IMMED);
	else
		MOVE(CONTEXT1, 0, CONTEXT2, 0, 17, IMMED);
	if (len > 21)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, 16, IMMED);
	else if (len == 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, 18, IMMED);

	if (len > 21)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, 16, IMMED);
	else if (len == 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, 19, IMMED);
	else if (len > 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, 16, IMMED);

	if (len > 21)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, 16, IMMED);
	else if (len < 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, 16, IMMED);
	else if (len == 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, 20, IMMED);

	if (len > 21)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, 16, IMMED);
	else if (len < 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, 16, IMMED);
	else if (len > 16)
		MOVE(CONTEXT1, 0, CONTEXT2, 0, 16, IMMED);
	else
		MOVE(CONTEXT1, 0, CONTEXT2, 0, 22, IMMED);

	return PROGRAM_FINALIZE();
}

uint32_t prg_buff[1000];

int main(int argc, char **argv)
{
	unsigned size;
	uint8_t len = 16;

	pr_debug("IF_Command  example program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = if_command_test(prg_buff, len);
	pr_debug("size = %d\n", size);
	print_prog(prg_buff, size);

	return 0;
}
