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
 * IEEE 802.16 WiMAX Protocol Data Block
 */
/** @addtogroup defines_group
 *  @{
 */
#define WIMAX_PDBOPTS_FCS       0x01    /**< Options Byte with FCS enabled */
#define WIMAX_PDBOPTS_AR        0x40    /**< Options Byte with AR enabled */
/** @}*/
#define WIMAX_PDB_B0            0x19    /* Initial Block B0 Flags */
#define WIMAX_PDB_CTR           0x01    /* Counter Block Flags */

struct wimax_encap_pdb {
	uint8_t rsvd[3];                /* Reserved Bits */
	uint8_t options;                /* Options Byte */
	uint32_t nonce;                 /* Nonce Constant */
	uint8_t b0_flags;               /* Initial Block B0 */
	uint8_t ctr_flags;              /* Counter Block Flags */
	uint16_t ctr_init_count;
	/* begin DECO writeback region */
	uint32_t pn;                    /* Packet Number */
	/* end DECO writeback region */
};

struct wimax_decap_pdb {
	uint8_t rsvd[3];                /* Reserved Bits */
	uint8_t options;                /* Options Byte */
	uint32_t nonce;                 /* Nonce Constant */
	uint8_t iv_flags;               /* Initialization Vector Flags */
	uint8_t ctr_flags;              /* Counter Block Flags */
	uint16_t ctr_init_count;
	/* begin DECO writeback region */
	uint32_t pn;                    /* Packet Number */
	uint8_t rsvd1[2];               /* Reserved Bits */
	uint16_t antireplay_len;
	uint32_t antireplay_scorecard_hi;
	uint32_t antireplay_scorecard_lo;
	/** end DECO writeback region */
};

/**
 * @defgroup pdb_group SEC Protocol Data Block Data Structures
 * @ingroup descriptor_lib_group
 * @{
 */
/** @} end of pdb_group */

/**
 * @defgroup ipsec_encap_pdb ipsec_encap_pdb
 * @ingroup pdb_group
 * @{
 */
/** @} end of ipsec_encap_pdb */

/**
 * @defgroup ipsec_decap_pdb ipsec_decap_pdb
 * @ingroup pdb_group
 * @{
 */
/** @} end of ipsec_decap_pdb */

/** @addtogroup defines_group
 *  @{
 */
/* General IPSec ESP encap / decap PDB options */
#define PDBOPTS_ESP_ESN		0x10   /**< extended sequence included */
#define PDBOPTS_ESP_IPVSN	0x02   /**< process IPv6 header */
#define PDBOPTS_ESP_TUNNEL	0x01   /**< tunnel mode next-header byte */

/* IPSec ESP Encap PDB options */
#define PDBOPTS_ESP_UPDATE_CSUM 0x80   /**< update ip header checksum */
#define PDBOPTS_ESP_DIFFSERV	0x40   /**< copy TOS/TC from inner iphdr */
#define PDBOPTS_ESP_IVSRC	0x20   /**< IV comes from internal random gen */
#define PDBOPTS_ESP_IPHDRSRC	0x08   /**< IP header comes from PDB */
#define PDBOPTS_ESP_INCIPHDR	0x04   /**< prepend IP header to output frame */

/* IPSec ESP Decap PDB options */
#define PDBOPTS_ESP_ARSNONE	0x00   /**< no antireplay window */
#define PDBOPTS_ESP_ARS64	0xc0   /**< 64-entry antireplay window */
#define PDBOPTS_ESP_ARS32	0x40   /**< 32-entry antireplay window */
#define PDBOPTS_ESP_VERIFY_CSUM 0x20   /**< validate ip header checksum */
#define PDBOPTS_ESP_OUTFMT	0x08   /**< output only decapsulation */
#define PDBOPTS_ESP_AOFL	0x04   /**< adjust out frame len (SEC>=5.3) */

#define PDBHMO_ESP_DECAP_SHIFT	12
#define PDBHMO_ESP_ENCAP_SHIFT	4

/**
 * IPsec ESP decrement TTL (IPv4) / Hop limit (IPv6) HMO option
 */
#define PDBHMO_ESP_DECAP_DTTL	(0x02 << PDBHMO_ESP_DECAP_SHIFT)
#define PDBHMO_ESP_ENCAP_DTTL	(0x02 << PDBHMO_ESP_ENCAP_SHIFT)

/**
 * Decap - DiffServ Copy - Copy the IPv4 TOS or IPv6 Traffic Class byte
 * from the outer IP header to the inner IP header.
 */
#define PDBHMO_ESP_DIFFSERV	(0x01 << PDBHMO_ESP_DECAP_SHIFT)

/**
 * Encap - Copy DF bit - if an IPv4 tunnel mode outer IP header is coming from
 * the PDB, copy the DF bit from the inner IP header to the outer IP header.
 */
#define PDBHMO_ESP_DFBIT	(0x04 << PDBHMO_ESP_ENCAP_SHIFT)

/** @} */ /* end of defines_group */

/**
 * @struct    ipsec_encap_cbc pdb.h
 * @ingroup   ipsec_encap_pdb
 * @details   IV field for IPsec CBC encapsulation
 */
struct ipsec_encap_cbc {
	uint32_t iv[4];
};


/**
 * @struct    ipsec_encap_ctr pdb.h
 * @ingroup   ipsec_encap_pdb
 * @details   Nonce and IV fields for IPsec CTR encapsulation
 */
struct ipsec_encap_ctr {
	uint32_t ctr_nonce;
	uint32_t ctr_initial;
	uint32_t iv[2];
};

/**
 * @struct    ipsec_encap_ccm pdb.h
 * @ingroup   ipsec_encap_pdb
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
 * @struct    ipsec_encap_gcm pdb.h
 * @ingroup   ipsec_encap_pdb
 * @details   Salt and IV fields for IPsec GCM encapsulation
 */
struct ipsec_encap_gcm {
	uint32_t salt; /* lower 24 bits */
	uint32_t rsvd1;
	uint32_t iv[2];
};

/**
 * @struct    ipsec_encap_pdb pdb.h
 * @ingroup   ipsec_encap_pdb
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
 * @ingroup   ipsec_decap_pdb
 * @details   Placeholder for reserved words
 */
struct ipsec_decap_cbc {
	uint32_t rsvd[2];
};

/**
 * @struct    ipsec_decap_ctr pdb.h
 * @ingroup   ipsec_decap_pdb
 * @details   Salt and counter fields for IPsec CTR decapsulation
 */
struct ipsec_decap_ctr {
	uint32_t salt;
	uint32_t ctr_initial;
};

/**
 * @struct    ipsec_decap_ctr pdb.h
 * @ingroup   ipsec_decap_pdb
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
 * @ingroup   ipsec_decap_pdb
 * @details   Salt field for IPsec GCM decapsulation
 */
struct ipsec_decap_gcm {
	uint32_t salt;
	uint32_t resvd;
};

/**
 * @struct    ipsec_decap_pdb pdb.h
 * @ingroup   ipsec_decap_pdb
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

/*
 * PDCP Control Plane Protocol Data Blocks
 */
#define PDCP_C_PLANE_PDB_HFN_SHIFT		5
#define PDCP_C_PLANE_PDB_BEARER_SHIFT		27
#define PDCP_C_PLANE_PDB_DIR_SHIFT		26
#define PDCP_C_PLANE_PDB_HFN_THR_SHIFT		5

#define PDCP_U_PLANE_PDB_OPT_SHORT_SN		0x2
#define PDCP_U_PLANE_PDB_SHORT_SN_HFN_SHIFT	7
#define PDCP_U_PLANE_PDB_LONG_SN_HFN_SHIFT	12
#define PDCP_U_PLANE_PDB_15BIT_SN_HFN_SHIFT	15
#define PDCP_U_PLANE_PDB_BEARER_SHIFT		27
#define PDCP_U_PLANE_PDB_DIR_SHIFT		26
#define PDCP_U_PLANE_PDB_SHORT_SN_HFN_THR_SHIFT	7
#define PDCP_U_PLANE_PDB_LONG_SN_HFN_THR_SHIFT	12
#define PDCP_U_PLANE_PDB_15BIT_SN_HFN_THR_SHIFT	15

struct pdcp_pdb {
	union {
		uint32_t opt;
		uint32_t rsvd;
	}opt_res;
	uint32_t hfn_res;	/* HyperFrame number,(27, 25 or 21 bits),
				 * left aligned & right-padded with zeros. */
	uint32_t bearer_dir_res;/* Bearer(5 bits), packet direction (1 bit),
				 * left aligned & right-padded with zeros. */
	uint32_t hfn_thr_res;	/* HyperFrame number threshold (27, 25 or 21
				 * bits), left aligned & right-padded with
				 * zeros. */
};

/*
 * PDCP internal PDB types
 */
enum pdb_type_e {
	PDCP_PDB_TYPE_NO_PDB,
	PDCP_PDB_TYPE_FULL_PDB,
	PDCP_PDB_TYPE_REDUCED_PDB,
	PDCP_PDB_TYPE_INVALID
};

/*
 * SRTP Protocol Data Blocks
 */
#define SRTP_PDBOPTS_MKI	0x08
#define SRTP_PDBOPTS_AR		0x40
#define SRTP_CIPHER_SALT_LEN	14

struct srtp_encap_pdb {
	uint8_t x_len;			   /* RTP Extension length */
	uint8_t mki_len;		   /* MKI length */
	uint8_t n_tag;			   /* ICV length */
	uint8_t options;		   /* Options Byte */
	uint32_t cnst0;			   /* Constant Bits */
	uint8_t rsvd[2];		   /* Reserved Bits */
	uint16_t cnst1;			   /* Constant Bits */
	uint8_t salt[SRTP_CIPHER_SALT_LEN];/* Cipher Salt */
	uint16_t cnst2;			   /* Constant Bits */
	uint32_t rsvd1;			   /* Reserved Bits */
	uint32_t roc;			   /* Rollover Counter */
	uint32_t opt_mki;		   /* MKI */
};

struct srtp_decap_pdb {
	uint8_t x_len;			   /* RTP Extension length */
	uint8_t mki_len;		   /* MKI length */
	uint8_t n_tag;			   /* ICV length */
	uint8_t options;		   /* Options Byte */
	uint32_t cnst0;			   /* Constant Bits */
	uint8_t rsvd[2];		   /* Reserved Bits */
	uint16_t cnst1;			   /* Constant Bits */
	uint8_t salt[SRTP_CIPHER_SALT_LEN];/* Cipher Salt */
	uint16_t cnst2;			   /* Constant Bits */
	uint16_t rsvd1;			   /* Reserved Bits */
	uint16_t seq_num;		   /* Sequence Number */
	uint32_t roc;			   /* Rollover Counter */
	uint64_t antireplay_scorecard;	   /* Anti-replay Scorecard */
};

/*
 * IEEE 802.11 WiFi Protocol Data Block
 */
#define WIFI_PDBOPTS_FCS	0x01
#define WIFI_PDBOPTS_AR		0x40

/**
 * @def WIFI_B0_FLAGS
 * The value of B0 flags used for WiFi AES-CCM Context construction.
 *
 */
#define WIFI_B0_FLAGS			0x59

/**
 * @def WIFI_FRM_CTRL_MASK
 * The Frame Control Mask value used in computing the additional
 * authentication data (AAD).
 */
#define WIFI_FRM_CTRL_MASK		0x8fc7

/**
 * @def WIFI_SEQ_CTRL_MASK
 * The Sequence Control Mask value used in computing the additional
 * authentication data (AAD). *
 */
#define WIFI_SEQ_CTRL_MASK		0x0f00

/**
 * @def WIFI_CTR_FLAGS
 * The value of Counter Flags used for CCM Counter block construction.
 */
#define WIFI_CTR_FLAGS			0x01

struct wifi_encap_pdb {
	uint16_t mac_hdr_len;	/* MAC Header Length - 24 or 30 bytes */
	uint8_t rsvd;		/* Reserver bits */
	uint8_t options;	/* Options byte */
	uint8_t b0_flags;	/* CCM B0 block flags */
	uint8_t pri;		/* Frame Priority */
	uint16_t pn1;		/* Packet Number MS 16 bits */
	uint32_t pn2;		/* Packet Number LS 32 bits */
	uint16_t frm_ctrl_mask; /* Frame Control mask */
	uint16_t seq_ctrl_mask; /* Sequence Control mask */
	uint16_t rsvd1;	    /* Reserved bits */
	uint8_t cnst;		/* CCM Counter block constant - 0x0000 */
	uint8_t key_id;		/* Key ID*/
	uint8_t ctr_flags;	/* CCM Counter block flags */
	uint8_t rsvd2;		/* Reserved bits */
	uint16_t ctr_init;	/* CCM Counter block init counter */
};

struct wifi_decap_pdb {
	uint16_t mac_hdr_len;	/* MAC Header Length - 24 or 30 bytes */
	uint8_t rsvd;		/* Reserver bits */
	uint8_t options;	/* Options byte */
	uint8_t b0_flags;	/* CCM B0 block flags */
	uint8_t pri;		/* Frame Priority */
	uint16_t pn1;		/* Packet Number MS 16 bits */
	uint32_t pn2;		/* Packet Number LS 32 bits */
	uint16_t frm_ctrl_mask; /* Frame Control mask */
	uint16_t seq_ctrl_mask; /* Sequence Control mask */
	uint32_t rsvd1;	    /* Reserved bits */
	uint8_t ctr_flags;	/* CCM Counter block flags */
	uint8_t rsvd2;		/* Reserved bits */
	uint16_t ctr_init;	/* CCM Counter block init counter */
};

#endif /* __RTA_PDB_H__ */
