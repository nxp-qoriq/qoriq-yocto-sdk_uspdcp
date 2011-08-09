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
#include "sec_job_ring.h"
#include "sec_utils.h"
#include "sec_hw_specific.h"
#include "sec_config.h"

// For definition of sec_vtop and sec_ptov macros
#include "external_mem_management.h"

/*==================================================================================================
                                     LOCAL DEFINES
==================================================================================================*/
/** Command that is used by SEC user space driver and SEC kernel driver
 *  to signal a request from the former to the later to disable job DONE
 *  and error IRQs on a certain job ring.
 *  The configuration is done at SEC Controller's level.
 *  @note   Need to be kept in synch with #SEC_UIO_DISABLE_IRQ_CMD from
 *          linux-2.6/drivers/crypto/talitos.c ! */
#define SEC_UIO_DISABLE_IRQ_CMD     0

/** Command that is used by SEC user space driver and SEC kernel driver
 *  to signal a request from the former to the later to enable job DONE
 *  and error IRQs on a certain job ring.
 *  The configuration is done at SEC Controller's level.
 *  @note   Need to be kept in synch with #SEC_UIO_ENABLE_IRQ_CMD from
 *          linux-2.6/drivers/crypto/talitos.c ! */
#define SEC_UIO_ENABLE_IRQ_CMD      1

/** Command that is used by SEC user space driver and SEC kernel driver
 *  to signal a request from the former to the later to do a SEC engine reset.
 *  @note   Need to be kept in synch with #SEC_UIO_RESET_SEC_ENGINE_CMD from
 *          linux-2.6/drivers/crypto/talitos.c ! */
#define SEC_UIO_RESET_SEC_ENGINE_CMD    3

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
/* Job rings used for communication with SEC HW */
sec_job_ring_t g_job_rings[MAX_SEC_JOB_RINGS];

/*==================================================================================================
                                 LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
                                     GLOBAL FUNCTIONS
==================================================================================================*/
int init_job_ring(sec_job_ring_t * job_ring, void **dma_mem, int startup_work_mode)
{
    int ret = 0;
    int i = 0;

    ASSERT(job_ring != NULL);
    ASSERT(dma_mem != NULL);

    SEC_INFO("Job ring %d UIO fd = %d", job_ring->jr_id, job_ring->uio_fd);

    // Reset job ring in SEC hw and configure job ring registers
    ret = hw_reset_job_ring(job_ring);
    SEC_ASSERT(ret == 0, ret, "Failed to reset hardware job ring with id %d", job_ring->jr_id);

#if SEC_NOTIFICATION_TYPE == SEC_NOTIFICATION_TYPE_NAPI
    // When SEC US driver works in NAPI mode, the UA can select
    // if the driver starts with IRQs on or off.
    if(startup_work_mode == SEC_STARTUP_INTERRUPT_MODE)
    {
        SEC_INFO("Enabling DONE IRQ generation on job ring with id %d", job_ring->jr_id);
        uio_job_ring_enable_irqs(job_ring);
    }
#endif

#if SEC_NOTIFICATION_TYPE == SEC_NOTIFICATION_TYPE_IRQ
    // When SEC US driver works in pure interrupt mode, IRQ's are always enabled.
    SEC_INFO("Enabling DONE IRQ generation on job ring with id %d", job_ring->jr_id);
    uio_job_ring_enable_irqs(job_ring);
#endif

    // Memory area must start from cacheline-aligned boundary.
    // Each job entry is itself aligned to cacheline.
    SEC_ASSERT ((dma_addr_t)*dma_mem % CACHE_LINE_SIZE == 0, 
            SEC_INVALID_INPUT_PARAM,
            "Current memory position is not cacheline aligned."
            "Job ring id = %d", job_ring->jr_id);

    // Allocate job items from the DMA-capable memory area provided by UA
    ASSERT(job_ring->descriptors == NULL);

    job_ring->descriptors = *dma_mem;
#ifdef SEC_HW_VERSION_3_3
    memset(job_ring->descriptors, 0, SEC_JOB_RING_SIZE * sizeof(struct sec_descriptor_t));
    *dma_mem += SEC_JOB_RING_SIZE * sizeof(struct sec_descriptor_t);
#else
    memset(job_ring->descriptors, 0, SEC_JOB_RING_HW_SIZE * sizeof(struct sec_descriptor_t));
    *dma_mem += SEC_JOB_RING_HW_SIZE * sizeof(struct sec_descriptor_t);
#endif
#ifdef SEC_HW_VERSION_4_4
    // Got to allocate some mem for input & output ring
    ASSERT(job_ring->input_ring == NULL);

    job_ring->input_ring = *dma_mem;
    memset(job_ring->input_ring, 0, SEC_JOB_RING_HW_SIZE * sizeof(dma_addr_t));
    *dma_mem += SEC_JOB_RING_HW_SIZE * sizeof(dma_addr_t);

    ASSERT(job_ring->output_ring == NULL);

    job_ring->output_ring = *dma_mem;
    memset(job_ring->output_ring, 0, SEC_JOB_RING_HW_SIZE * sizeof(struct sec_outring_entry));
    *dma_mem += SEC_JOB_RING_HW_SIZE * sizeof(struct sec_outring_entry);

    // Got to write the actual jobring size to the hw registers
    hw_set_input_ring_size(job_ring,SEC_JOB_RING_HW_SIZE - 1);
    hw_set_output_ring_size(job_ring,SEC_JOB_RING_HW_SIZE - 1);

    // Now write the ptrs to the input and output ring
    hw_set_input_ring_start_addr(job_ring, sec_vtop(job_ring->input_ring));
    SEC_DEBUG(" Set input ring base address to: Virtual: 0x%x, Phyisical: 0x%x, Read from HW: 0x%x",
            (dma_addr_t)job_ring->input_ring,
            sec_vtop(job_ring->input_ring),
            hw_get_inp_queue_base(job_ring));

    hw_set_output_ring_start_addr(job_ring, sec_vtop(job_ring->output_ring));
    SEC_DEBUG(" Set output ring base address to: Virtual: 0x%x, Phyisical: 0x%x, Read from HW: 0x%x",
            (dma_addr_t)job_ring->output_ring,
            sec_vtop(job_ring->output_ring),
            hw_get_out_queue_base(job_ring));

#endif

    // TODO: check that we do not use more DMA mem than actually allocated/reserved for us by User App.
    // Options: 
    // - check if used more than #SEC_DMA_MEMORY_SIZE and/or
    // - implement wrapper functions that access dma_mem and maintain/increment size of dma mem used -> central point of
    // accessing dma mem -> central point of check for boundary issues!

    for(i = 0; i < SEC_JOB_RING_SIZE; i++)
    {
        // Remember virtual address for a job descriptor
        job_ring->jobs[i].descr = &job_ring->descriptors[i];
        
        // Obtain and store the physical address for a job descriptor        
        job_ring->jobs[i].descr_phys_addr = sec_vtop(&job_ring->descriptors[i]);

        SEC_ASSERT ((dma_addr_t)*dma_mem % CACHE_LINE_SIZE == 0,
                SEC_INVALID_INPUT_PARAM,
                "Current jobs[i]->mac_i [i=%d] position is not cacheline aligned.", i);
#if SEC_HW_VERSION_4_4
        // Set pointers to descriptors in input ring
        job_ring->input_ring[i] = &job_ring->descriptors[i];
        // Need to store pointer to output ring status
        job_ring->jobs[i].out_status = &job_ring->output_ring[i].status;
#else
        // Allocate DMA-capable memory where SEC 3.1 will generate MAC-I for
        // SNOW F9 and AES CMAC integrity check algorithms (PDCP control-plane).
        job_ring->jobs[i].mac_i = *dma_mem;
        memset(job_ring->jobs[i].mac_i, 0, sizeof(sec_mac_i_t));
        *dma_mem += sizeof(sec_mac_i_t);
#endif
    }
#ifdef SEC_HW_VERSION_3_1
    // Allocate normal virtual memory for internal c-plane FIFO
    for(i = 0; i < FIFO_CAPACITY; i++)
    {
        job_ring->pdcp_c_plane_fifo.items[i] =(void*) malloc(sizeof(sec_job_t));
        memset(job_ring->pdcp_c_plane_fifo.items[i], 0, sizeof(sec_job_t));
    }
#endif
    job_ring->jr_state = SEC_JOB_RING_STATE_STARTED;

    return SEC_SUCCESS;
}

int shutdown_job_ring(sec_job_ring_t * job_ring)
{
    int ret = 0;

    ASSERT(job_ring != NULL);
    if(job_ring->uio_fd != 0)
    {
        close(job_ring->uio_fd);
    }

    ret = hw_shutdown_job_ring(job_ring);
    SEC_ASSERT(ret == 0, ret, "Failed to shutdown hardware job ring with id %d", job_ring->jr_id);

    memset(job_ring, 0, sizeof(sec_job_ring_t));
#ifdef SEC_HW_VERSION_3_1
    int i;
    for(i = 0; i < FIFO_CAPACITY; i++)
    {
        free(job_ring->pdcp_c_plane_fifo.items[i]);
    }
#endif
    return SEC_SUCCESS;
}

void uio_reset_sec_engine(sec_job_ring_t *job_ring)
{
    int ret;

    // Use UIO file descriptor we have for this job ring.
    // Writing a command code to this file descriptor will make the
    // SEC kernel driver reset SEC engine.
    ret = sec_uio_send_command(job_ring, SEC_UIO_RESET_SEC_ENGINE_CMD);
    SEC_ASSERT_RET_VOID(ret == sizeof(int),
                        "Failed to request SEC engine restart through UIO control."
                        "Reset SEC driver!");
}

void uio_job_ring_enable_irqs(sec_job_ring_t *job_ring)
{
    int ret;

    // Use UIO file descriptor we have for this job ring.
    // Writing a command code to this file descriptor will make the
    // SEC kernel driver enable DONE and Error IRQs for this job ring,
    // at Controller level.
    ret = sec_uio_send_command(job_ring, SEC_UIO_ENABLE_IRQ_CMD);
    SEC_DEBUG("Jr[%p]. Enabled IRQs on jr id %d\n", job_ring, job_ring->jr_id);
    SEC_ASSERT_RET_VOID(ret == sizeof(int),
                        "Failed to request SEC engine to enable job done and "
                        "error IRQs through UIO control. Job ring id %d. Reset SEC driver!",
                        job_ring->jr_id);
}
/*================================================================================================*/

#ifdef __cplusplus
}
#endif
