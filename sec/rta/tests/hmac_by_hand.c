#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;

uint8_t *iv1 = (uint8_t *) 0x64;	/* input constant */
uint8_t *ipad = (uint8_t *) 0x664;
uint8_t *opad = (uint8_t *) 0x12364;

int build_hmacprecomp(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;
	uint8_t *hmac_key = (uint8_t *) (0x12);	/* input */
	unsigned int hmac_key_len = 18;	/* input */
	uint32_t *hmacprecompute_opad_phys = (uint32_t *) 0xaabb2200;

	PROGRAM_CNTXT_INIT(buff, 0);
	JOB_HDR(SHR_NEVER, 0, 0, 0);
	{
		LOAD(PTR((uintptr_t) hmac_key), CONTEXT1, 0, hmac_key_len, 0);
		MATHB(ZERO, XOR, IMM(0x3636363636363636), MATH0, 8, 0);
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(NCP));

		/* Apply the IPAD to the key */
		MOVE(CONTEXT1, 0, MATH1, 0, IMM(24), WITH(WAITCOMP));
		MATHB(MATH1, XOR, MATH0, MATH1, 8, 0);
		MATHB(MATH2, XOR, MATH0, MATH2, 8, 0);
		MATHB(MATH3, XOR, MATH0, MATH3, 8, 0);
		MOVE(MATH1, 0, CONTEXT1, 0, IMM(24), WITH(WAITCOMP));

		MOVE(CONTEXT1, 24, MATH1, 0, IMM(24), WITH(WAITCOMP));
		MATHB(MATH1, XOR, MATH0, MATH1, 8, 0);
		MATHB(MATH2, XOR, MATH0, MATH2, 8, 0);
		MATHB(MATH3, XOR, MATH0, MATH3, 8, 0);
		MOVE(MATH1, 0, CONTEXT1, 24, IMM(24), WITH(WAITCOMP));

		MOVE(CONTEXT1, 48, MATH1, 0, IMM(24), WITH(WAITCOMP));
		MATHB(MATH1, XOR, MATH0, MATH1, 8, 0);
		MATHB(MATH2, XOR, MATH0, MATH2, 8, 0);
		MATHB(MATH3, XOR, MATH0, MATH3, 8, 0);
		MOVE(MATH1, 0, CONTEXT1, 48, IMM(24), WITH(WAITCOMP));

		MOVE(CONTEXT1, 72, MATH1, 0, IMM(24), WITH(WAITCOMP));
		MATHB(MATH1, XOR, MATH0, MATH1, 8, 0);
		MATHB(MATH2, XOR, MATH0, MATH2, 8, 0);
		MATHB(MATH3, XOR, MATH0, MATH3, 8, 0);
		MOVE(MATH1, 0, CONTEXT1, 72, IMM(24), WITH(WAITCOMP));

		/* Init hash IV appropriate to desired algorithm */
		LOAD(PTR((uintptr_t) iv1), CONTEXT2, 0, 64, 0);

		/* Process padded key as first half of first block of data */
		MOVE(CONTEXT1, 0, IFIFOAB2, 0, IMM(64), 0);
		ALG_OPERATION(OP_ALG_ALGSEL_SHA512, OP_ALG_AAI_HASH,
			      OP_ALG_AS_UPDATE, 0, 0);

		/* Send IPAD as rest of block */
		MATHB(ZERO, XOR, MATH0, MATH1, 8, 0);
		MATHB(ZERO, XOR, MATH0, MATH2, 8, 0);
		MATHB(ZERO, XOR, MATH0, MATH3, 8, 0);
		MOVE(MATH0, 0, IFIFOAB2, 0, IMM(32), 0);
		MOVE(MATH0, 0, IFIFOAB2, 0, IMM(32), WITH(LAST2));

		/* This happens when MDHA is finished */
		STORE(CONTEXT2, 0, PTR((uintptr_t) ipad), 64, 0);
		JUMP(PTR((uintptr_t) hmacprecompute_opad_phys), FAR_JUMP,
		     ALL_TRUE, 0);
	}

	size = PROGRAM_FINALIZE();
	return size;
}

int build_hmacprecomp_opad(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;

	PROGRAM_CNTXT_INIT(buff, 0);
	JOB_HDR(SHR_NEVER, 0, 0, 0);
	{
		LOAD(IMM(CLRW_CLR_C2CTX | CLRW_CLR_C2MODE | CLRW_CLR_C2DATAS),
		     CLRW, 0, 4, 0);
		LOAD(IMM(CCTRL_RESET_CHA_MDHA), CCTRL, 0, 4, 0);

		/* Apply OPAD^IPAD to the key */
		MATHB(MATH1, XOR, IMM(0x5C5C5C5C5C5C5C5C), MATH1, 8, 0);

		MOVE(CONTEXT1, 0, MATH1, 0, IMM(24), WITH(WAITCOMP));
		MATHB(MATH1, XOR, MATH0, MATH1, 8, 0);
		MATHB(MATH2, XOR, MATH0, MATH2, 8, 0);
		MATHB(MATH3, XOR, MATH0, MATH3, 8, 0);
		MOVE(MATH1, 0, CONTEXT1, 0, IMM(24), WITH(WAITCOMP));

		MOVE(CONTEXT1, 24, MATH1, 0, IMM(24), WITH(WAITCOMP));
		MATHB(MATH1, XOR, MATH0, MATH1, 8, 0);
		MATHB(MATH2, XOR, MATH0, MATH2, 8, 0);
		MATHB(MATH3, XOR, MATH0, MATH3, 8, 0);
		MOVE(MATH1, 0, CONTEXT1, 24, IMM(24), WITH(WAITCOMP));

		MOVE(CONTEXT1, 48, MATH1, 0, IMM(24), WITH(WAITCOMP));
		MATHB(MATH1, XOR, MATH0, MATH1, 8, 0);
		MATHB(MATH2, XOR, MATH0, MATH2, 8, 0);
		MATHB(MATH3, XOR, MATH0, MATH3, 8, 0);
		MOVE(MATH1, 0, CONTEXT1, 48, IMM(24), WITH(WAITCOMP));

		MOVE(CONTEXT1, 72, MATH1, 0, IMM(24), WITH(WAITCOMP));
		MATHB(MATH1, XOR, MATH0, MATH1, 8, 0);
		MATHB(MATH2, XOR, MATH0, MATH2, 8, 0);
		MATHB(MATH3, XOR, MATH0, MATH3, 8, 0);
		MOVE(MATH1, 0, CONTEXT1, 72, IMM(24), WITH(WAITCOMP));

		/* Process padded key as first block of data */
		LOAD(PTR((uintptr_t) iv1), CONTEXT2, 0, 64, 0);
		MOVE(CONTEXT1, 0, IFIFOAB2, 0, IMM(64), 0);
		ALG_OPERATION(OP_ALG_ALGSEL_SHA512, OP_ALG_AAI_HASH,
			      OP_ALG_AS_UPDATE, 0, 0);

		/* Send OPAD as rest of block */
		MATHB(ZERO, XOR, IMM(0x5C5C5C5C5C5C5C5C), MATH0, 8, 0);
		MATHB(ZERO, XOR, MATH0, MATH1, 8, 0);
		MATHB(ZERO, ADD, MATH0, MATH2, 8, 0);
		MATHB(ZERO, OR, MATH0, MATH3, 8, 0);
		MOVE(MATH0, 0, IFIFOAB2, 0, IMM(32), 0);
		MOVE(MATH0, 0, IFIFOAB2, 0, IMM(32), 0);
		MOVE(MATH0, 0, IFIFOAB2, 0, IMM(32), WITH(LAST2));

		/* This happens when MDHA is finished */
		STORE(CONTEXT2, 0, PTR((uintptr_t) opad), 0, 0);
	}
	size = PROGRAM_FINALIZE();
	return size;
}

int build_hmac_from_pre(uint32_t *buff)
{
	struct program prg;
	struct program *program = &prg;
	int size;
	uint8_t *innerhash = (uint8_t *) 0x28;	/* debug */
	uint8_t *mac = (uint8_t *) 12228;
	void *msg = (void *)0x12341234ul;	/* physical ptr */
	unsigned int maclen = 28;

	PROGRAM_CNTXT_INIT(buff, 0);
	JOB_HDR(SHR_NEVER, 0, 0, 0);
	{
		/* Set up current state of inner hash */
		LOAD(PTR((uintptr_t) ipad), CONTEXT2, 0, 64, 0);
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(NIP));
		LOAD(IMM(0x0000000000000080), CONTEXT2, 64, 8, 0);

		FIFOLOAD(MSG2, PTR((uintptr_t) msg), 0, WITH(LAST2));
		ALG_OPERATION(OP_ALG_ALGSEL_SHA512, OP_ALG_AAI_HASH,
			      OP_ALG_AS_FINALIZE, 0, 0);
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CLASS2));

		LOAD(IMM(CLRW_CLR_C2MODE | CLRW_CLR_C2DATAS), CLRW, 0, 4, 0);
		LOAD(IMM(CCTRL_RESET_CHA_MDHA), CCTRL, 0, 4, 0);

		STORE(CONTEXT2, 0, PTR((uintptr_t) innerhash), maclen, 0);
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(NOP));

		/* Make inner hash be the message for the outer hash */
		MOVE(CONTEXT2, 0, IFIFOAB2, 0, IMM(maclen), WITH(LAST2));
		/* Set up current state of outer hash */
		LOAD(PTR((uintptr_t) opad), CONTEXT2, 0, 64, 0);
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(NIP));

		LOAD(IMM(0x0000000000000080), CONTEXT2, 64, 8, 0);
		ALG_OPERATION(OP_ALG_ALGSEL_SHA512, OP_ALG_AAI_HASH,
			      OP_ALG_AS_FINALIZE, 0, 0);
		STORE(CONTEXT2, 0, PTR((uintptr_t) mac), maclen, 0);
	}
	size = PROGRAM_FINALIZE();
	return size;
}

int prg_buff[1000];

int main(int argc, char **argv)
{
	int size;

	rta_set_sec_era(RTA_SEC_ERA_1);

	pr_debug("HMAC_By_Hand #1 JD program\n");
	size = build_hmacprecomp((uint32_t *) prg_buff);
	pr_debug("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	pr_debug("HMAC_By_Hand #2 JD program\n");
	size = build_hmacprecomp_opad((uint32_t *) prg_buff);
	pr_debug("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	pr_debug("HMAC_By_Hand #3 JD program\n");
	size = build_hmac_from_pre((uint32_t *) prg_buff);
	pr_debug("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
