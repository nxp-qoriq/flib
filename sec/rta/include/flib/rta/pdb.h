#ifndef __RTA_PDB_H__
#define __RTA_PDB_H__

/*
 * IEEE 801.AE MacSEC Protocol Data Block
 */
#define MACSEC_PDBOPTS_FCS	0x01
#define MACSEC_PDBOPTS_AR	0x40	/* used in decap only */

struct macsec_encap_pdb {
	uint16_t aad_len;
	uint8_t rsvd;
	uint8_t options;
	uint32_t sci_hi;
	uint32_t sci_lo;
	uint16_t ethertype;
	uint8_t tci_an;
	uint8_t rsvd1;
	/* begin DECO writeback region */
	uint32_t pn;
	/* end DECO writeback region */
};

struct macsec_decap_pdb {
	uint16_t aad_len;
	uint8_t rsvd;
	uint8_t options;
	uint32_t sci_hi;
	uint32_t sci_lo;
	uint8_t rsvd1[3];
	/* begin DECO writeback region */
	uint8_t antireplay_len;
	uint32_t pn;
	uint32_t antireplay_scorecard_hi;
	uint32_t antireplay_scorecard_lo;
	/* end DECO writeback region */
};


/*
 * General IPSec encap/decap PDB definitions
 */

/**
 * @struct    ipsec_encap_cbc pdb.h
 * @details   IV field for IPsec CBC encapsulation
 */
struct ipsec_encap_cbc {
	uint32_t iv[4];
};

/**
 * @struct    ipsec_encap_ctr pdb.h
 * @details   Nonce and IV fields for IPsec CTR encapsulation
 */
struct ipsec_encap_ctr {
	uint32_t ctr_nonce;
	uint32_t ctr_initial;
	uint32_t iv[2];
};

/**
 * @struct    ipsec_encap_ccm pdb.h
 * @details   Salt and IV fields for IPsec CCM encapsulation
 */
struct ipsec_encap_ccm {
	uint32_t salt; /* lower 24 bits */
	uint8_t b0_flags;
	uint8_t ctr_flags;
	uint16_t ctr_initial;
	uint32_t iv[2];
};

/**
 * @struct    ipsec_encap_ccm pdb.h
 * @details   Salt and IV fields for IPsec GCM encapsulation
 */
struct ipsec_encap_gcm {
	uint32_t salt; /* lower 24 bits */
	uint32_t rsvd1;
	uint32_t iv[2];
};

/**
 * @struct    ipsec_encap_pdb pdb.h
 * @details   Container for encapsulation PDB
 */
struct ipsec_encap_pdb {
	uint8_t hmo;
	uint8_t ip_nh;
	uint8_t ip_nh_offset;
	uint8_t options;
	uint32_t seq_num_ext_hi;
	uint32_t seq_num;
	union {
		struct ipsec_encap_cbc cbc;
		struct ipsec_encap_ctr ctr;
		struct ipsec_encap_ccm ccm;
		struct ipsec_encap_gcm gcm;
	};
	uint32_t spi;
	uint16_t rsvd;
	uint16_t ip_hdr_len;
	uint32_t ip_hdr[0]; /* optional IP Header content */
};

/**
 * @struct    ipsec_decap_cbc pdb.h
 * @details   Placeholder for reserved words
 */
struct ipsec_decap_cbc {
	uint32_t rsvd[2];
};

/**
 * @struct    ipsec_decap_ctr pdb.h
 * @details   Salt and counter fields for IPsec CTR decapsulation
 */
struct ipsec_decap_ctr {
	uint32_t salt;
	uint32_t ctr_initial;
};

/**
 * @struct    ipsec_decap_ctr pdb.h
 * @details   Salt, counter and flag fields for IPsec CCM decapsulation
 */
struct ipsec_decap_ccm {
	uint32_t salt;
	uint8_t iv_flags;
	uint8_t ctr_flags;
	uint16_t ctr_initial;
};

/**
 * @struct    ipsec_decap_gcm pdb.h
 * @details   Salt field for IPsec GCM decapsulation
 */
struct ipsec_decap_gcm {
	uint32_t salt;
	uint32_t resvd;
};

/**
 * @struct    ipsec_decap_pdb pdb.h
 * @details   Container for decapsulation PDB
 */
struct ipsec_decap_pdb {
	uint16_t ip_hdr_len;
	uint8_t ip_nh_offset;
	uint8_t options;
	union {
		struct ipsec_decap_cbc cbc;
		struct ipsec_decap_ctr ctr;
		struct ipsec_decap_ccm ccm;
		struct ipsec_decap_gcm gcm;
	};
	uint32_t seq_num_ext_hi;
	uint32_t seq_num;
	uint32_t anti_replay[2];
	uint32_t end_index[0];
};


#endif /* __RTA_PDB_H__ */
