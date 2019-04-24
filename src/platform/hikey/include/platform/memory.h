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

#ifndef __PLATFORM_MEMORY_H__
#define __PLATFORM_MEMORY_H__

#include <config.h>
#include <arch/memory.h>

#if !defined(__ASSEMBLER__) && !defined(LINKER)
void platform_init_memmap(void);
#endif

/* physical DSP addresses */

/*
 * DSP Side - Non Secure 3.5M
 */

#define HIKEY_MUSIC_DATA_BASE           0x8B300000
#define HIKEY_MUSIC_DATA_SIZE           0x00132000

#define HIKEY_PCM_DATA_BASE           0x8B432000
#define HIKEY_PCM_DATA_SIZE           0x00100000

#define HIKEY_HIFI_UART_BASE           0x8B532000
#define HIKEY_HIFI_UART_SIZE           0x0007F000

#define HIKEY_PANIC_STACK_BASE           0x8B5B1000
#define HIKEY_PANIC_STACK_SIZE           0x00001000

#define HIKEY_ICC_DEBUG_BASE           0x8B5B2000
#define HIKEY_ICC_DEBUG_SIZE           0x00013000

#define HIKEY_FLAG_DATA_BASE           0x8B5C5000
#define HIKEY_FLAG_DATA_SIZE           0x00001000

#define HIKEY_DDR_SEC_HEAD_BASE           0x8B5C6000
#define HIKEY_DDR_SEC_HEAD_SIZE           0x00001000

#define HIKEY_AP_NV_BASE           0x8B5C7000
#define HIKEY_AP_NV_SIZE           0x00032800

#define HIKEY_AP_HIFIMB_BASE           0x8B5F9800
#define HIKEY_AP_HIFIMB_SIZE           0x00010000

#define HIKEY_CODEC_DMA_BUF_BASE           0x8B609800
#define HIKEY_CODEC_DMA_BUF_SIZE           0x0000f000

#define HIKEY_CODEC_DMA_CONF_BASE           0x8B618800
#define HIKEY_CODEC_DMA_CONF_SIZE           0x00000080

#define HIKEY_SOUND_TRIGGER_BASE           0x8B618880
#define HIKEY_SOUND_TRIGGER_SIZE           0x0000f000

#define HIKEY_PCM_UPLOAD_BASE           0x8B627880
#define HIKEY_PCM_UPLOAD_SIZE           0x00002000

#define HIKEY_SHARE_BASE           0x8B629880
#define HIKEY_SHARE_SIZE           0x00003000

#define HIKEY_UNSEC_RSVD_BASE           0x8B62C880
#define HIKEY_UNSEC_RSVD_SIZE           0x00053780

/*
 * DSP Side - Secure 9.5M
 */

#define HIKEY_HIFI_RUN_BASE           0x89200000
#define HIKEY_HIFI_RUN_SIZE           0x00600000

#define HIKEY_OCRAM_BACK_BASE           0x89800000
#define HIKEY_OCRAM_BACK_SIZE           0x00030000

#define HIKEY_TCM_BACK_BASE           0x89830000
#define HIKEY_TCM_BACK_SIZE           0x00034000

#define HIKEY_IMG_BACK_BASE           0x89864000
#define HIKEY_IMG_BACK_SIZE           0x0031C000

/* mailbox */
#define MAILBOX_BASE	HIKEY_AP_HIFIMB_BASE
#define MAILBOX_SIZE	HIKEY_AP_HIFIMB_SIZE

/* TODO: I2S and DMAC bases addresses not yet known or confirmed */
#define DMAC0_BASE	HIKEY_CODEC_DMA_CONF_BASE
#define DMAC0_SIZE	HIKEY_CODEC_DMA_CONF_SIZE

/* IRAM and DRAM */
#define IRAM_BASE	0xE8080000
#define IRAM_SIZE	0xc000

#define DRAM0_BASE	0xE8058000
#define DRAM0_SIZE	0x28000

/* log data */
#define LOG_ENTRY_ELF_BASE	0x20000000
#define LOG_ENTRY_ELF_SIZE	0x2000000

/*
 * The Heap and Stack on Hikey are organised like this :-
 *
 * +--------------------------------------------------------------------------+
 * | Offset              | Region         |  Size                             |
 * +---------------------+----------------+-----------------------------------+
 * | DRAM0_BASE          | RO Data        |  SOF_DATA_SIZE                   |
 * |                     | Data           |                                   |
 * |                     | BSS            |                                   |
 * +---------------------+----------------+-----------------------------------+
 * | HEAP_SYSTEM_BASE    | System Heap    |  HEAP_SYSTEM_SIZE                 |
 * +---------------------+----------------+-----------------------------------+
 * | HEAP_RUNTIME_BASE   | Runtime Heap   |  HEAP_RUNTIME_SIZE                |
 * +---------------------+----------------+-----------------------------------+
 * | HEAP_BUFFER_BASE    | Module Buffers |  HEAP_BUFFER_SIZE                 |
 * +---------------------+----------------+-----------------------------------+
 * | SOF_STACK_END       | Stack          |  SOF_STACK_SIZE                  |
 * +---------------------+----------------+-----------------------------------+
 * | SOF_STACK_BASE      |                |                                   |
 * +---------------------+----------------+-----------------------------------+
 */


/* Heap section sizes for module pool */
#define HEAP_RT_COUNT8		0
#define HEAP_RT_COUNT16		48
#define HEAP_RT_COUNT32		48
#define HEAP_RT_COUNT64		32
#define HEAP_RT_COUNT128	32
#define HEAP_RT_COUNT256	32
#define HEAP_RT_COUNT512	4
#define HEAP_RT_COUNT1024	4

/* Heap section sizes for system runtime heap */
#define HEAP_SYS_RT_COUNT64	64
#define HEAP_SYS_RT_COUNT512	8
#define HEAP_SYS_RT_COUNT1024	4

/* Heap configuration */
#define SOF_DATA_SIZE			0x6800

#define HEAP_SYSTEM_BASE		(DRAM0_BASE + SOF_DATA_SIZE)
#define HEAP_SYSTEM_SIZE		0xe000

#define HEAP_SYSTEM_0_BASE		HEAP_SYSTEM_BASE

#define HEAP_SYS_RUNTIME_BASE	(HEAP_SYSTEM_BASE + HEAP_SYSTEM_SIZE)
#define HEAP_SYS_RUNTIME_SIZE \
	(HEAP_SYS_RT_COUNT64 * 64 + HEAP_SYS_RT_COUNT512 * 512 + \
	HEAP_SYS_RT_COUNT1024 * 1024)

#define HEAP_RUNTIME_BASE	(HEAP_SYS_RUNTIME_BASE + HEAP_SYS_RUNTIME_SIZE)
#define HEAP_RUNTIME_SIZE \
	(HEAP_RT_COUNT8 * 8 + HEAP_RT_COUNT16 * 16 + \
	HEAP_RT_COUNT32 * 32 + HEAP_RT_COUNT64 * 64 + \
	HEAP_RT_COUNT128 * 128 + HEAP_RT_COUNT256 * 256 + \
	HEAP_RT_COUNT512 * 512 + HEAP_RT_COUNT1024 * 1024)

#define HEAP_BUFFER_BASE	(HEAP_RUNTIME_BASE + HEAP_RUNTIME_SIZE)
#define HEAP_BUFFER_SIZE \
	(DRAM0_SIZE - HEAP_RUNTIME_SIZE - SOF_STACK_TOTAL_SIZE -\
	HEAP_SYS_RUNTIME_SIZE - HEAP_SYSTEM_SIZE - SOF_DATA_SIZE)

#define HEAP_BUFFER_BLOCK_SIZE		0x180
#define HEAP_BUFFER_COUNT	(HEAP_BUFFER_SIZE / HEAP_BUFFER_BLOCK_SIZE)

#define PLATFORM_HEAP_SYSTEM		1 /* one per core */
#define PLATFORM_HEAP_SYSTEM_RUNTIME	1 /* one per core */
#define PLATFORM_HEAP_RUNTIME		1
#define PLATFORM_HEAP_BUFFER		1

/* Stack configuration */
#define SOF_STACK_SIZE		ARCH_STACK_SIZE
#define SOF_STACK_TOTAL_SIZE	ARCH_STACK_TOTAL_SIZE
#define SOF_STACK_BASE		(DRAM0_BASE + DRAM0_SIZE)
#define SOF_STACK_END		(SOF_STACK_BASE - SOF_STACK_TOTAL_SIZE)

/* Vector and literal sizes - not in core-isa.h */
#define SOF_MEM_VECT_LIT_SIZE		0x4
#define SOF_MEM_VECT_TEXT_SIZE		0x1c
#define SOF_MEM_VECT_SIZE		(SOF_MEM_VECT_TEXT_SIZE + SOF_MEM_VECT_LIT_SIZE)

#define SOF_MEM_RESET_TEXT_SIZE	0x2e0
#define SOF_MEM_RESET_LIT_SIZE		0x120
#define SOF_MEM_VECBASE_LIT_SIZE	0x178

#define SOF_MEM_RO_SIZE			0x8

#define uncache_to_cache(address)	address
#define cache_to_uncache(address)	address
#define is_uncached(address)		0

#endif
