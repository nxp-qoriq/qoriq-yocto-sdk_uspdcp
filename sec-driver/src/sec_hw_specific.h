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
#include "sec_job_ring.h"

/*==============================================================================
                              DEFINES AND MACROS
==============================================================================*/

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
 * IER offset
 *****************************************************************/

/** Offset from the SEC register base address for IER register.
 * Enables/disables done/error interrupts per channel, at controller level.*/
#define SEC_REG_IER             0x1008
/** Offset to lower 32 bits of IER register */
#define SEC_REG_IER_LO          0x100C

/*****************************************************************
 * IER values
 *****************************************************************/

/** Enable DONE interrupts for all 4 job rings */
#define SEC_REG_VAL_IER_DONE    0x00055
/** Enable DONE interrupts for a certain job ring */
#define SEC_REG_SET_VAL_IER_DONE(job_ring_id) (1 << (job_ring_id)* 2)
/** Enable Error interrupts for a certain job ring */
#define SEC_REG_SET_VAL_IER_ERR(job_ring_id) (2 << (job_ring_id)* 2)

/*****************************************************************
 * CCR offset
 *****************************************************************/

/** Offset to higher 32 bits of CCR, Channel Configuration Register */
#define SEC_REG_CCCR_OFFSET_HI  0x0108
/** Offset to lower 32 bits of CCR, Channel Configuration Register */
#define SEC_REG_CCCR_OFFSET_LO  0x010C
/** Offset to higher 32 bits of CCR for a job ring */
#define SEC_REG_CCCR(jr)        (CHAN_BASE(jr) + SEC_REG_CCCR_OFFSET_HI)
/** Offset to lower 32 bits of CCR for a job ring */
#define SEC_REG_CCCR_LO(jr)     (CHAN_BASE(jr) + SEC_REG_CCCR_OFFSET_LO)

/*****************************************************************
 * CCR values
 *****************************************************************/

/* Job ring reset */
#define SEC_REG_CCCR_VAL_RESET      0x1
/* Extended address enable (36bit) */
#define SEC_REG_VAL_CCCR_LO_EAE     0x20
/* Enable done writeback */
#define SEC_REG_VAL_CCCR_LO_CDWE    0x10
/* Enable done IRQ */
#define SEC_REG_VAL_CCCR_LO_CDIE    0x2


/*==============================================================================
                                    ENUMS
==============================================================================*/

/*==============================================================================
                         STRUCTURES AND OTHER TYPEDEFS
==============================================================================*/

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
 * @retval other for error
 */
int hw_shutdown_job_ring(sec_job_ring_t *job_ring);

/** @brief Enable IRQ generation for a job ring/channel in SEC device.
 *
 * @note This function is used in NAPI and pure IRQ functioning modes.
 *       In pure polling mode IRQ generation is NOT enabled at job ring level!
 *
 * Configuration is done at SEC engine controller level (on SEC 3.1).
 * At job ring level the interrupts are always generated but can be
 * masked out at controller level.
 *
 * @param [in] job_ring     The job ring
 */
void hw_enable_irq_on_job_ring(sec_job_ring_t *job_ring);

/*============================================================================*/


#endif  /* SEC_HW_SPECIFIC_H */
