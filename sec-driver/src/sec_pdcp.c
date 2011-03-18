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

#ifdef _cplusplus
extern "C" {
#endif

/*=================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include "sec_pdcp.h"
#include "sec_hw_specific.h"
#include "sec_utils.h"

/*==================================================================================================
                                     LOCAL DEFINES
==================================================================================================*/

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                      LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                      LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                     GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                     GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                 LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
                                     GLOBAL FUNCTIONS
==================================================================================================*/

int sec_pdcp_context_set_crypto_info(sec_context_t *ctx,
                                     sec_pdcp_context_info_t *crypto_info)
{
    sec_crypto_info_t *pdcp_info = NULL;

    ASSERT(ctx->crypto_info != NULL);
    pdcp_info = ctx->crypto_info;

    pdcp_info->hfn = crypto_info->hfn;
    //pdcp_info->crypto_key = crypto_info->cipher_key;
    //pdcp_info->auth_key = crypto_info->integrity_key;

    //TODO: finalize this when the task about PDCP descriptors is done.

    return SEC_SUCCESS;
}

int sec_pdcp_context_create_descriptor(sec_job_t *job, sec_descriptor_t *descriptor)
{
    ASSERT(job != NULL);
    ASSERT(descriptor != NULL);

//    90900010            // Header word 1
//    0000bead            // Header word 2

    descriptor->hdr = 0x90900010;
    descriptor->hdr_lo = 0x0000bead;

    hw_job_enable_writeback(descriptor);

    return SEC_SUCCESS;
}
/*================================================================================================*/

#ifdef __cplusplus
}
#endif
