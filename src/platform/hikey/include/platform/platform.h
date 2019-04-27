/*
 * Copyright (c) 2019, John Gunn
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the Intel Corporation nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: John Gunn <jgunn0262@gmail.com>
 */

#ifndef __PLATFORM_PLATFORM_H__
#define __PLATFORM_PLATFORM_H__

#define PLATFORM_CORE_COUNT	1

#define PLATFORM_MASTER_CORE_ID	0

//#define PLATFORM_LPSRAM_EBB_COUNT 1

//#define LPSRAM_BANK_SIZE (64 * 1024)

//#define LPSRAM_SIZE (PLATFORM_LPSRAM_EBB_COUNT * LPSRAM_BANK_SIZE)

#if !defined(__ASSEMBLER__) && !defined(LINKER)

#include <sof/platform.h>
#include <platform/clk.h>
#include <platform/interrupt.h>

struct sof;

/*! \def PLATFORM_DEFAULT_CLOCK
 *  \brief clock source for audio pipeline
 *
 *  There are two types of clock: cpu clock which is a internal clock in
 *  xtensa core, and i2s clock which is provided by external HW IP.
 *  The choice depends on HW features on different platform
 */
#define PLATFORM_DEFAULT_CLOCK CLK_I2S

/*! \def PLATFORM_WORKQ_DEFAULT_TIMEOUT
 *  \brief work queue default timeout in microseconds
 */
#define PLATFORM_WORKQ_DEFAULT_TIMEOUT	1000

/* IPC Interrupt */
#define PLATFORM_IPC_INTERRUPT	IRQ_NUM_EXT_IPC

/* Host page size */
#define HOST_PAGE_SIZE		4096
#define PLATFORM_PAGE_TABLE_SIZE	256

/* pipeline IRQ */
#define PLATFORM_SCHEDULE_IRQ	IRQ_NUM_SOFTWARE1

#define PLATFORM_IRQ_TASK_HIGH	IRQ_NUM_SOFTWARE1
#define PLATFORM_IRQ_TASK_MED	IRQ_NUM_SOFTWARE1
#define PLATFORM_IRQ_TASK_LOW	IRQ_NUM_SOFTWARE1

#define PLATFORM_SCHEDULE_COST	200

/* maximum preload pipeline depth */
#define MAX_PRELOAD_SIZE	20

/* DMA treats PHY addresses as host address unless within DSP region */
#define PLATFORM_HOST_DMA_MASK	0x00000000

/* Platform stream capabilities */
#define PLATFORM_MAX_CHANNELS	4
#define PLATFORM_MAX_STREAMS	5

/* clock source used by scheduler for deadline calculations */
#define PLATFORM_SCHED_CLOCK	PLATFORM_DEFAULT_CLOCK

/* DMA channel drain timeout in microseconds - TODO: caclulate based on topology */
#define PLATFORM_DMA_TIMEOUT	1333

/* DMA host transfer timeouts in microseconds */
#define PLATFORM_HOST_DMA_TIMEOUT	200

/* DMA link transfer timeouts in microseconds
 * TODO: timeout should be reduced
 * (DMA might needs some further changes to do that)
 */
#define PLATFORM_LINK_DMA_TIMEOUT	1000

/* WorkQ window size in microseconds */
#define PLATFORM_WORKQ_WINDOW	2000

/* platform WorkQ clock */
#define PLATFORM_WORKQ_CLOCK	PLATFORM_DEFAULT_CLOCK

/* local buffer size of DMA tracing */
#define DMA_TRACE_LOCAL_SIZE	HOST_PAGE_SIZE

/* trace bytes flushed during panic */
#define DMA_FLUSH_TRACE_SIZE	(MAILBOX_TRACE_SIZE >> 2)

/* the interval of DMA trace copying */
#define DMA_TRACE_PERIOD		500000

/*
 * the interval of reschedule DMA trace copying in special case like half
 * fullness of local DMA trace buffer
 */
#define DMA_TRACE_RESCHEDULE_TIME	100

/* DSP should be idle in this time frame */
#define PLATFORM_IDLE_TIME	750000

/* DSP default delay in cycles */
#define PLATFORM_DEFAULT_DELAY	12

/* DSP LPE delay in cycles */
#define PLATFORM_LPE_DELAY 2000

/* timer driven scheduling start offset in microseconds */
#define PLATFORM_TIMER_START_OFFSET	100

static inline uint32_t panic_read(uint32_t reg)
{
	return *((volatile uint32_t*)(HIKEY_PANIC_STACK_BASE + reg));
}

static inline void panic_write(uint32_t reg, uint32_t val)
{
	*((volatile uint32_t*)(HIKEY_PANIC_STACK_BASE + reg)) = val;
}

/* Platform defined panic code */
static inline void platform_panic(uint32_t p)
{
	panic_write(0, p);
}

/* Platform defined trace code */
#define platform_trace_point(__x) \
	panic_write(0, __x)

extern struct timer *platform_timer;

#endif
#endif
