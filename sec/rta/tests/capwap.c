#include <stdio.h>
#include "flib/rta.h"
#include "test_common.h"

enum rta_sec_era rta_sec_era;
int prg_buff[1000];

int generate_capwap_code(uint32_t *buff, int mdatalen)
{
	int size;
	struct program prg;
	struct program *program = &prg;
	uint8_t key_imm[] = { 0x12, 0x13, 0x14, 0x15 };
	intptr_t key_addr = (intptr_t) &key_imm;

	PROGRAM_CNTXT_INIT(buff, 0);

	LABEL(encap_iv);
	LABEL(previous_iv);
	LABEL(new_IV_OK);
	LABEL(seqnum);
	LABEL(seqoutptr);
	LABEL(new_seqinptr);
	LABEL(skip_keyloading);
	LABEL(descbuf);

	REFERENCE(ref1);
	REFERENCE(ref2);
	REFERENCE(pjump1);
	REFERENCE(pjump2);

	SHR_HDR(SHR_ALWAYS, 0, 0);
	WORD(0x17feff00);	/* type 0x17 / protocol version */
	WORD(0x00010000);	/* Epoch / upper bits of SeqNum */
	WORD(seqnum);		/* Lower bits of SeqNum */
	SET_LABEL(encap_iv);
	/* Location of the extra, custom part of PDB */
	previous_iv = 16;
	/* All of the IV, both next and previous */
	ENDIAN_DATA(((uint8_t[]){ 00, 00}), 2);

	ref1 = MOVE(DESCBUF, seqoutptr, MATH0, 0, IMM(16), WITH(WAITCOMP));
	MATHB(MATH0, XOR, IMM(0x0840010000000000), MATH0, SIZE(8), 0);
	ref2 = MOVE(MATH0, 0, DESCBUF, new_seqinptr, IMM(8), 0);
	MOVE(IFIFOABD, 0, OFIFO, 0, IMM(mdatalen), 0);
	SEQFIFOSTORE(MSG, 0, mdatalen, 0);

	pjump2 = JUMP(IMM(skip_keyloading), LOCAL_JUMP, ALL_TRUE, WITH(SHRD));
	KEY(MDHA_SPLIT_KEY, WITH(ENC), IMM(key_addr), 4, 0);
	KEY(KEY1, WITH(EKT), IMM(key_addr), 4, 0);
	SET_LABEL(skip_keyloading);
	ALG_OPERATION(OP_ALG_ALGSEL_AES, OP_ALG_AAI_CTR_MOD128, OP_ALG_AS_INIT,
		      ICV_CHECK_DISABLE, DIR_ENC);
	SET_LABEL(new_seqinptr);
	WORD(0x0);
	WORD(0x0);
	SEQFIFOLOAD(SKIP, 59, 0);

	SEQLOAD(MATH2, 0, SIZE(8), 0);
	MOVE(DESCBUF, previous_iv, MATH0, 0, IMM(16), WITH(WAITCOMP));
	MATHB(MATH0, XOR, MATH2, MATH1, SIZE(8), 0);

	pjump1 = JUMP(IMM(new_IV_OK), LOCAL_JUMP, ALL_FALSE, WITH(MATH_Z));
	MATHB(MATH1, XOR, MATH3, MATH2, SIZE(8), 0);
	SET_LABEL(new_IV_OK);
	MOVE(MATH0, 0, DESCBUF, encap_iv, IMM(32), 0);
	SEQSTORE(DESCBUF, 4, 8, 0);
	seqoutptr = 8;

	PATCH_MOVE(program, ref1, descbuf + seqoutptr);
	PATCH_MOVE(program, ref2, descbuf + new_seqinptr);
	PATCH_JUMP(program, pjump1, new_IV_OK);
	PATCH_JUMP(program, pjump2, skip_keyloading);
	size = PROGRAM_FINALIZE();

	return size;
}

int main(int argc, char **argv)
{
	int size;

	printf("CAPWAP program\n");
	rta_set_sec_era(RTA_SEC_ERA_1);
	size = generate_capwap_code((uint32_t *) prg_buff, 0);
	printf("size = %d\n", size);
	print_prog((uint32_t *) prg_buff, size);

	return 0;
}
