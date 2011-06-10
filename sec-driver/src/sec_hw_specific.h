/* Copyright (c) 2011 Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SEC_HW_SPECIFIC_H
#define SEC_HW_SPECIFIC_H

/*==============================================================================
                                INCLUDE FILES
==============================================================================*/
#include "fsl_sec.h"
#include "sec_utils.h"

/*==============================================================================
                              DEFINES AND MACROS
==============================================================================*/

/** Maximum length in words (4 bytes)  for IV(Initialization Vector) */
// TODO: Modify back to be 6 bytes...cannot do MAC-I checking with AES anyway....
// will do memcmp in software.
#define SEC_IV_MAX_LENGTH          12

/** Maximum length in words (4 bytes)  for IV (Initialization Vector) template.
 * Stores F8 IV followed by F9 IV. */
#define SEC_IV_TEMPLATE_MAX_LENGTH 8

/*****************************************************************
 * SEC REGISTER CONFIGURATIONS
 *****************************************************************/

/** Memory range assigned for registers of a job ring */
#define SEC_CH_REG_RANGE                    0x100
/** Offset to the registers of a job ring, as mapped by default by SEC engine */
#define SEC_CH_REG_RANGE_START_NORMAL       0x1000
/** Offset to the registers of a job ring, mapped in an alternate 4k page 
 * by SEC engine  if configured to do so with MCR (Master Control Register)*/
#define SEC_CH_REG_RANGE_START_ALTERNATE    0x0000
/** Offset to the registers of a job ring.
 * Is different for each job ring. */
#define CHAN_BASE(job_ring) ((    (job_ring->alternate_register_range == TRUE) ? \
                    SEC_CH_REG_RANGE_START_ALTERNATE : SEC_CH_REG_RANGE_START_NORMAL) \
                    + (job_ring->jr_id * SEC_CH_REG_RANGE))

/*****************************************************************
 * ISR(Interrupt Status Register) offset
 *****************************************************************/

/** Offset to higher 32 bits of ISR register.*/
#define SEC_REG_ISR         0x1010
/** Offset to lower 32 bits of ISR register */
#define SEC_REG_ISR_LO      0x1014

/*****************************************************************
 * ISR(Interrupt Status Register) values
 *****************************************************************/

/** Job ring errors mask */
#define SEC_REG_ISR_CHERR   0xaa
/** Job ring done mask */
#define SEC_REG_ISR_CHDONE  0x55

/** Mask to check if DONE interrupt was generated for a certain job ring */
#define SEC_REG_GET_VAL_ISR_DONE(job_ring_id) (1 << (job_ring_id)* 2)
/** Mask to check if Error interrupt was generated for a certain job ring */
#define SEC_REG_GET_VAL_ISR_ERR(job_ring_id) (2 << (job_ring_id)* 2)



/*****************************************************************
 * IER(Interrupt Enable Register) offset
 *****************************************************************/

/** Offset from the SEC register base address for IER register.
 * Enables/disables done/error interrupts per channel, at controller level.*/
#define SEC_REG_IER     0x1008
/** Offset to lower 32 bits of IER register */
#define SEC_REG_IER_LO  0x100C


/*****************************************************************
 * IER(Interrupt Enable Register) values
 *****************************************************************/

/** Enable DONE interrupts for all 4 job rings */
#define SEC_REG_VAL_IER_DONE    0x00055
/** Enable DONE interrupts for a certain job ring */
#define SEC_REG_SET_VAL_IER_DONE(job_ring_id) (1 << (job_ring_id)* 2)
/** Enable Error interrupts for a certain job ring */
#define SEC_REG_SET_VAL_IER_ERR(job_ring_id) (2 << (job_ring_id)* 2)

/*****************************************************************
 * CCR(Channel Configuration Register) offset
 *****************************************************************/

/** Offset to higher 32 bits of CCR for a job ring */
#define SEC_REG_CCCR(jr)        (CHAN_BASE(jr) + SEC_REG_CCCR_OFFSET_HI)
/** Offset to lower 32 bits of CCR for a job ring */
#define SEC_REG_CCCR_LO(jr)     (CHAN_BASE(jr) + SEC_REG_CCCR_OFFSET_LO)

/** Offset to higher 32 bits of CCR, Channel Configuration Register */
#define SEC_REG_CCCR_OFFSET_HI  0x0108
/** Offset to lower 32 bits of CCR, Channel Configuration Register */
#define SEC_REG_CCCR_OFFSET_LO  0x010C

/*****************************************************************
 * CCR(Channel Configuration Register) values
 *****************************************************************/

/* Job ring reset */
#define SEC_REG_CCCR_VAL_RESET      0x1
/* Job ring continue. Do same operations as for reset
but do not reset FIFO with jobs. See SEC 3.1 reference manual for more details. */
#define SEC_REG_CCCR_VAL_CONTINUE   0x2
/* Extended address enable (36bit) */
#define SEC_REG_VAL_CCCR_LO_EAE     0x20
/* Enable done writeback */
#define SEC_REG_VAL_CCCR_LO_CDWE    0x10
/* Enable done IRQ */
#define SEC_REG_VAL_CCCR_LO_CDIE    0x2
/* Enable ICV writeback if descriptor configured for ICV */
#define SEC_REG_VAL_CCCR_LO_IWSE    0x80

/*****************************************************************
 * FFER(Fetch Fifo Enqueue Register) offset
 *****************************************************************/

/**  Offset to higher 32 bits of fetch fifo enqueue register
 * (FFER) for a job ring */
#define SEC_REG_FFER(jr)        (CHAN_BASE(jr) + SEC_REG_FFER_OFFSET_HI)
/**  Offset to lower 32 bits of fetch fifo enqueue register
 * (FFER) for a job ring */
#define SEC_REG_FFER_LO(jr)     (CHAN_BASE(jr) + SEC_REG_FFER_OFFSET_LO)


/**  Offset to higher 32 bits of fetch fifo enqueue register (FFER) */
#define SEC_REG_FFER_OFFSET_HI  0x0148
/**  Offset to lower 32 bits of fetch fifo enqueue register (FFER) */
#define SEC_REG_FFER_OFFSET_LO  0x014C

/*****************************************************************
 * CSR(Channel Status Register) offset
 *****************************************************************/

/**  Offset to higher 32 bits of CSR register for a job ring */
#define SEC_REG_CSR(jr)       (CHAN_BASE(jr) + SEC_REG_CSR_OFFSET_HI)
/**  Offset to lower 32 bits of CSR register for a job ring */
#define SEC_REG_CSR_LO(jr)    (CHAN_BASE(jr) + SEC_REG_CSR_OFFSET_LO)


/**  Offset to higher 32 bits of CSR */
#define SEC_REG_CSR_OFFSET_HI   0x0110
/**  Offset to lower 32 bits of CSR */
#define SEC_REG_CSR_OFFSET_LO   0x0114

/*****************************************************************
 * CDPR(Current Descriptor Pointer Register) offset
 *****************************************************************/

/**  Offset to higher 32 bits of CDPR register for a job ring */
#define SEC_REG_CDPR(jr)        (CHAN_BASE(jr) +  SEC_REG_CDPR_OFFSET_HI)
/**  Offset to lower 32 bits of CDPR register for a job ring */
#define SEC_REG_CDPR_LO(jr)     (CHAN_BASE(jr) +  SEC_REG_CDPR_OFFSET_LO)


/**  Offset to higher 32 bits of CDPR */
#define SEC_REG_CDPR_OFFSET_HI  0x0140
/**  Offset to lower 32 bits of CDPR */
#define SEC_REG_CDPR_OFFSET_LO  0x0144

/*****************************************************************
 * CSR(Channel Status Register) values
 *****************************************************************/

#define SEC_REG_CSR_ERROR_MASK  0xFFFF /* Extract error field from CSR */

/* Specific error codes */
#define SEC_REG_CSR_LO_DOF    0x8000 /* double FF write oflow error */
#define SEC_REG_CSR_LO_SOF    0x4000 /* single FF write oflow error */
#define SEC_REG_CSR_LO_MDTE   0x2000 /* master data transfer error */
#define SEC_REG_CSR_LO_IDH    0x0400 /* illegal desc hdr error */
#define SEC_REG_CSR_LO_EU     0x0100 /* EU error detected */
#define SEC_REG_CSR_LO_WDT    0x0080 /* watchdog timeout */
#define SEC_REG_CSR_LO_SGML   0x0040 /* scatter/gather length mismatch error */
#define SEC_REG_CSR_LO_RSI    0x0020 /* RAID size incorrect error */
#define SEC_REG_CSR_LO_RSG    0x0010 /* RAID scatter/gather error */



/*****************************************************************
 * STEU IMR (Interrupt Mask Register) offset.
 * STEU is execution unit implementing SNOW F8 and F9.
 *****************************************************************/

/**  Offset to higher 32 bits of STEU IMR */
#define SEC_REG_STEU_IMR        0xD038
/**  Offset to lower 32 bits of STEU IMR */
#define SEC_REG_STEU_IMR_LO     0xD03C

/*****************************************************************
 * STEU ISR (Interrupt Status Register) offset.
 * STEU is execution unit implementing SNOW F8 and F9.
 *****************************************************************/

/**  Offset to higher 32 bits of STEU ISR */
#define SEC_REG_STEU_ISR        0xD030
/**  Offset to lower 32 bits of STEU ISR */
#define SEC_REG_STEU_ISR_LO     0xD034
/*****************************************************************
 * STEU IMR (Interrupt Mask Register) values
 * STEU is execution unit implementing SNOW F8 and F9.
 *****************************************************************/

/** Disable integrity check error interrupt in STEU */
#define SEC_REG_STEU_IMR_DISABLE_ICE    0x4000


/*****************************************************************
 * AESU SR(Status Register) offset.
 * AESU is execution unit implementing AES CTR and AES CMAC.
 *****************************************************************/

/**  Offset to higher 32 bits of AESU SR */
#define SEC_REG_AESU_SR         0x4028
/**  Offset to lower 32 bits of AESU SR */
#define SEC_REG_AESU_SR_LO      0x402C

/*****************************************************************
 * AESU ISR (Interrupt Status Register) offset.
 * AESU is execution unit implementing AES CTR and AES CMAC.
 *****************************************************************/

/**  Offset to higher 32 bits of AESU ISR */
#define SEC_REG_AESU_ISR        0x4030
/**  Offset to lower 32 bits of AESU ISR */
#define SEC_REG_AESU_ISR_LO     0x4034
/*****************************************************************
 * AESU IMR (Interrupt Mask Register) offset.
 * AESU is execution unit implementing AES CTR and AES CMAC.
 *****************************************************************/

/**  Offset to higher 32 bits of AESU IMR */
#define SEC_REG_AESU_IMR        0x4038
/**  Offset to lower 32 bits of AESU IMR */
#define SEC_REG_AESU_IMR_LO     0x403C


/*****************************************************************
 * AESU IMR (Interrupt Mask Register) values
 * AESU is execution unit implementing AES CTR and AES CMAC.
 *****************************************************************/

/** Disable integrity check error interrupt in STEU */
#define SEC_REG_AESU_IMR_DISABLE_ICE    0x4000


/*****************************************************************
 * Descriptor format: Header Dword values
 *****************************************************************/

/** Written back when packet processing is done,
 * in lower 32 bits of Descriptor header */
#define SEC_DESC_HDR_DONE           0xff000000
/** Request done notification (DN) per descriptor */
#define SEC_DESC_HDR_DONE_NOTIFY    0x00000001

/** Mask used to extract ICCR0 result from lower 32 bits of Descriptor header.
 * ICCR0 indicates result for integrity check performed by primary execution unit.
 * ICCR1 does the same for secondary execution unit.
 * Only ICCR0 used by driver. */
#define SEC_DESC_HDR_ICCR0_MASK     0x18000000
/** Written back when MAC-I check passed for packet,
 * in lower 32 bits of Descriptor header */
#define SEC_DESC_HDR_ICCR0_PASS     0x08000000
/** Written back when MAC-I check failed for packet,
 * in lower 32 bits of Descriptor header */
#define SEC_DESC_HDR_ICCR0_FAIL     0x10000000

/** Determines the processing type. Outbound means encrypting packets */
#define SEC_DESC_HDR_DIR_OUTBOUND   0x00000000
/** Determines the processing type. Inbound means decrypting packets */
#define SEC_DESC_HDR_DIR_INBOUND    0x00000002


/** Mask used to extract primary EU configuration from SEC descriptor header */
#define SEC_DESC_HDR_SEL0_MASK          0xf0000000

/*****************************************************************
 * SNOW descriptor configuration
 *****************************************************************/

/**  Select STEU execution unit, the one implementing SNOW 3G */
#define SEC_DESC_HDR_EU_SEL0_STEU       0x90000000
/** Mode data used to program STEU execution unit for F8 processing */
#define SEC_DESC_HDR_MODE0_STEU_F8      0x00900000
/** Mode data used to program STEU execution unit for F9 processing,
 *  no MAC-I check */
#define SEC_DESC_HDR_MODE0_STEU_F9      0x01a00000
/** Mode data used to program STEU execution unit for F9 processing,
 *  with MAC-I check */
#define SEC_DESC_HDR_MODE0_STEU_F9_ICV  0x05a00000
/** Select SNOW 3G descriptor type = common_nonsnoop */
#define SEC_DESC_HDR_DESC_TYPE_STEU     0x00000010

/*****************************************************************
 * AES descriptor configuration
 *****************************************************************/

/**  Select AESU execution unit, the one implementing AES */
#define SEC_DESC_HDR_EU_SEL0_AESU           0x60000000
/** Mode data used to program AESU execution unit for AES CTR processing */
#define SEC_DESC_HDR_MODE0_AESU_CTR         0x00600000
/** Mode data used to program AESU execution unit for AES CMAC processing */
#define SEC_DESC_HDR_MODE0_AESU_CMAC        0x04400000
/** Mode data used to program AESU execution unit for AES CMAC processing.
 * Perform MAC-I check */
#define SEC_DESC_HDR_MODE0_AESU_CMAC_ICV    0x06400000
/** Select AES descriptor type = common_nonsnoop */
#define SEC_DESC_HDR_DESC_TYPE_AESU         0x00000010

/*****************************************************************
 * Macros manipulating descriptor header
 *****************************************************************/

/** Check if a descriptor has the DONE bits set.
 * If yes, it means the packet tied to the descriptor 
 * is processed by SEC engine already.*/
#define hw_job_is_done(descriptor)          ((descriptor->hdr & SEC_DESC_HDR_DONE) == SEC_DESC_HDR_DONE)

/** Check if integrity check performed by primary execution unit failed */
#define hw_icv_check_failed(descriptor)     ((descriptor->hdr_lo & SEC_DESC_HDR_ICCR0_MASK) == SEC_DESC_HDR_ICCR0_FAIL)

/** Check if integrity check performed by primary execution unit passed */
#define hw_icv_check_passed(descriptor)     ((descriptor->hdr_lo & SEC_DESC_HDR_ICCR0_MASK) == SEC_DESC_HDR_ICCR0_PASS)

/** Enable done writeback in descriptor header dword after packet is processed by SEC engine */
#define hw_job_enable_writeback(descriptor_hdr) ((descriptor_hdr) |= SEC_DESC_HDR_DONE_NOTIFY)

/** Return 0 if no error generated on this job ring.
 * Return non-zero if error. */
#define hw_job_ring_error(jr) (in_be32((jr)->register_base_addr + SEC_REG_CSR_LO(jr)) & SEC_REG_CSR_ERROR_MASK)

 /** Some error types require that the same error bit is set to 1 to clear the error source.
  * Use this macro for this purpose.
  */
#define hw_job_ring_clear_error(jr, value) (setbits32((jr)->register_base_addr + SEC_REG_CSR_LO(jr), (value)))


/** Read pointer to current descriptor that is beeing processed on a job ring. */
#if defined(__powerpc64__) && defined(CONFIG_PHYS_64BIT)
#define hw_get_current_descriptor(jr) ((dma_addr_t)((in_be32((jr)->register_base_addr + SEC_REG_CDPR(jr)) << 32)  |  \
                                                    (in_be32((jr)->register_base_addr + SEC_REG_CDPR_LO(jr)))))
#else
#define hw_get_current_descriptor(jr) ((dma_addr_t) (in_be32((jr)->register_base_addr + SEC_REG_CDPR_LO(jr))))
#endif

/*****************************************************************
 * Macros manipulating SEC registers for a job ring/channel
 *****************************************************************/

/** @brief Enable DONE IRQ generation for a job ring/channel in SEC device.
 *
 * @note This function is used in NAPI and pure IRQ functioning modes.
 *       In pure polling mode DONE IRQ generation is NOT enabled at job ring level!
 *
 * Configuration is done at SEC engine controller level (on SEC 3.1).
 * At job ring level the interrupts are always generated but can be
 * masked out at controller level.
 *
 * @param [in] job_ring     The job ring
 */
#define hw_enable_done_irq_on_job_ring(job_ring) \
{\
    uint32_t reg_val = 0; \
\
    reg_val = SEC_REG_SET_VAL_IER_DONE((job_ring)->jr_id); \
    /* Configure interrupt generation at controller level, in SEC hw */ \
    setbits32((job_ring)->register_base_addr + SEC_REG_IER , reg_val); \
}

/** @brief Enqueue descriptor into a job ring's FIFO.
 * A descriptor points to an input packet to be processed as well as
 * to an output packet where SEC will write processing result.
 * The descriptor also points to the specific cryptographic operations
 * that must be applied on the input packet.
 *
 * @param [in] job_ring     The job ring
 * @param [in] descriptor   Physical address of descriptor.
 */
#if defined(__powerpc64__) && defined(CONFIG_PHYS_64BIT)

#define hw_enqueue_packet_on_job_ring(job_ring, descriptor) \
{\
    /* Write higher 32 bits. Only relevant when Extended address\
       is enabled(36 bit physical addresses).\
       @note address must be big endian\
    */\
    out_be32(job_ring->register_base_addr + SEC_REG_FFER(job_ring),\
             PHYS_ADDR_HI(descriptor));\
    /* Write lower 32 bits. This is the trigger to insert the descriptor\
       into the channel's FETCH FIFO.\
       @note: This is why higher 32 bits MUST ALWAYS be written prior to\
       the lower 32 bits, when 36 physical addressing is ON!\
       @note address must be big endian\
    */\
    out_be32(job_ring->register_base_addr + SEC_REG_FFER_LO(job_ring),\
             PHYS_ADDR_LO(descriptor));\
}

#else //#if defined(__powerpc64__) && defined(CONFIG_PHYS_64BIT)

#define hw_enqueue_packet_on_job_ring(job_ring, descriptor) \
{\
    /* Write lower 32 bits of FFER. This is the trigger to insert the descriptor\
       into the channel's FETCH FIFO.\
       @note: This is why higher 32 bits MUST ALWAYS be written prior to\
       the lower 32 bits, when 36 physical addressing is ON!\
       @note address must be big endian\
    */\
    out_be32(job_ring->register_base_addr + SEC_REG_FFER_LO(job_ring),\
             (descriptor));\
}

#endif
/*==============================================================================
                                    ENUMS
==============================================================================*/

/*==============================================================================
                         STRUCTURES AND OTHER TYPEDEFS
==============================================================================*/
/** Forward structure declaration */
typedef struct sec_job_ring_t sec_job_ring_t;

/** SEC Descriptor pointer entry */
struct sec_ptr {
    uint16_t len;       /*< length */
    uint8_t j_extent;   /*< jump to sg link table and/or extent */
    uint8_t eptr;       /*< extended address */
    uint32_t ptr;       /*< address */
};

/** A descriptor that instructs SEC engine how to process a packet.
 * On SEC 3.1 a descriptor consists of 8 double-words: 
 * one header dword and seven pointer dwords. */
struct sec_descriptor_t
{
    volatile uint32_t hdr;      /*< header high bits */
    uint32_t hdr_lo;            /*< header low bits */
    struct sec_ptr ptr[7];      /*< ptr/len pair array */
    /*< Initialization Vector. Need to have it here because it 
     * is updated with info from each packet!!! */
    uint32_t __CACHELINE_ALIGNED iv[SEC_IV_MAX_LENGTH];

} __CACHELINE_ALIGNED;

/** Cryptographic data belonging to a SEC context.
 * Can be considered a joint venture between:
 * - a 'shared descriptor' (the descriptor header word)
 * - a PDB(protocol data block) */
typedef struct sec_crypto_pdb_s
{
    uint32_t crypto_hdr;    /*< Higher 32 bits of Descriptor Header dword, used for encrypt/decrypt operations.
                                Lower 32 bits are reserved and unused. */
    uint32_t auth_hdr;      /*< Higher 32 bits of Descriptor Header dword, used for authentication operations.
                                Lower 32 bits are reserved and unused. */
    uint32_t iv_template[SEC_IV_TEMPLATE_MAX_LENGTH];       /*< Template for Initialization Vector. 
                                                                HFN is stored and maintained here. */
    uint32_t hfn_threshold; /*< Threshold for HFN configured by User Application. 
                                Bitshifted left to skip SN bits from first word of IV. */
    uint32_t hfn_mask;      /*< Mask applied on IV to extract HFN */
    uint32_t sn_mask;       /*< Mask applied on PDCP header to extract SN */
    uint8_t sns;            /*< SN Short. Is set to 1 if short sequence number is used:
                                - 5 bit for c-plane and 7 bit for d-plane.
                                Is set to 0 if long SN is used:
                                - 12 bit for d-plane */
}sec_crypto_pdb_t;
/*==============================================================================
                                 CONSTANTS
==============================================================================*/

/*==============================================================================
                         GLOBAL VARIABLE DECLARATIONS
==============================================================================*/

/*==============================================================================
                            FUNCTION PROTOTYPES
==============================================================================*/

/** @brief Initialize a job ring/channel in SEC device.
 * Write configuration register/s to properly initialize a job ring.
 *
 * @param [in] job_ring     The job ring
 *
 * @retval 0 for success
 * @retval other for error
 */
int hw_reset_job_ring(sec_job_ring_t *job_ring);

/** @brief Reset a job ring/channel in SEC device.
 * Write configuration register/s to reset a job ring.
 *
 * @param [in] job_ring     The job ring
 *
 * @retval 0 for success
 * @retval -1 in case job ring reset failed
 */
int hw_shutdown_job_ring(sec_job_ring_t *job_ring);

/** @brief Reset and continue for a job ring/channel in SEC device.
 * Write configuration register/s to reset some settings for a job ring
 * but still continue processing. FIFO with all already submitted jobs
 * will be kept.
 *
 * @param [in] job_ring     The job ring
 *
 * @retval 0 for success
 * @retval -1 in case job ring reset and continue failed
 */
int hw_reset_and_continue_job_ring(sec_job_ring_t *job_ring);

/** @brief Handle a job ring/channel error in SEC device.
 * Identify the error type and clear error bits if required.
 * Return information if job ring must be restarted.
 *
 * @param [in]  job_ring        The job ring
 * @param [in]  sec_error_code  The job ring's error code as first read from SEC engine
 * @param [out] reset_required  If set to #TRUE, the job ring must be reset.
 */
void hw_handle_job_ring_error(sec_job_ring_t *job_ring,
                              uint32_t sec_error_code,
                              uint32_t *reset_required);

/*============================================================================*/


#endif  /* SEC_HW_SPECIFIC_H */
