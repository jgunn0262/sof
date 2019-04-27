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

#include <sof/alloc.h>
#include <uapi/ipc/header.h>
#include <platform/memory.h>

/* Heap blocks for system runtime */
static struct block_hdr sys_rt_block64[HEAP_SYS_RT_COUNT64];
static struct block_hdr sys_rt_block512[HEAP_SYS_RT_COUNT512];
static struct block_hdr sys_rt_block1024[HEAP_SYS_RT_COUNT1024];

/* Heap memory for system runtime */
static struct block_map sys_rt_heap_map[] = {
	BLOCK_DEF(64, HEAP_SYS_RT_COUNT64, sys_rt_block64),
	BLOCK_DEF(512, HEAP_SYS_RT_COUNT512, sys_rt_block512),
	BLOCK_DEF(1024, HEAP_SYS_RT_COUNT1024, sys_rt_block1024),
};

/* Heap blocks for modules */
static struct block_hdr mod_block16[HEAP_RT_COUNT16];
static struct block_hdr mod_block32[HEAP_RT_COUNT32];
static struct block_hdr mod_block64[HEAP_RT_COUNT64];
static struct block_hdr mod_block128[HEAP_RT_COUNT128];
static struct block_hdr mod_block256[HEAP_RT_COUNT256];
static struct block_hdr mod_block512[HEAP_RT_COUNT512];
static struct block_hdr mod_block1024[HEAP_RT_COUNT1024];

/* Heap memory map for modules */
static struct block_map rt_heap_map[] = {
	BLOCK_DEF(16, HEAP_RT_COUNT16, mod_block16),
	BLOCK_DEF(32, HEAP_RT_COUNT32, mod_block32),
	BLOCK_DEF(64, HEAP_RT_COUNT64, mod_block64),
	BLOCK_DEF(128, HEAP_RT_COUNT128, mod_block128),
	BLOCK_DEF(256, HEAP_RT_COUNT256, mod_block256),
	BLOCK_DEF(512, HEAP_RT_COUNT512, mod_block512),
	BLOCK_DEF(1024, HEAP_RT_COUNT1024, mod_block1024),
};

/* Heap blocks for buffers */
static struct block_hdr buf_block[HEAP_BUFFER_COUNT];

/* Heap memory map for buffers */
static struct block_map buf_heap_map[] = {
	BLOCK_DEF(HEAP_BUFFER_BLOCK_SIZE, HEAP_BUFFER_COUNT, buf_block),
};

struct mm memmap = {
	.system[0] = {
		.heap = HEAP_SYSTEM_BASE,
		.size = HEAP_SYSTEM_SIZE,
		.info = {.free = HEAP_SYSTEM_SIZE,},
		.caps = SOF_MEM_CAPS_RAM | SOF_MEM_CAPS_CACHE |
			SOF_MEM_CAPS_DMA,
	},
	.system_runtime[0] = {
		.blocks = ARRAY_SIZE(sys_rt_heap_map),
		.map = sys_rt_heap_map,
		.heap = HEAP_SYS_RUNTIME_BASE,
		.size = HEAP_SYS_RUNTIME_SIZE,
		.info = {.free = HEAP_SYS_RUNTIME_SIZE,},
		.caps = SOF_MEM_CAPS_RAM | SOF_MEM_CAPS_CACHE |
			SOF_MEM_CAPS_DMA,
	},
	.runtime[0] = {
		.blocks = ARRAY_SIZE(rt_heap_map),
		.map = rt_heap_map,
		.heap = HEAP_RUNTIME_BASE,
		.size = HEAP_RUNTIME_SIZE,
		.info = {.free = HEAP_RUNTIME_SIZE,},
		.caps = SOF_MEM_CAPS_RAM | SOF_MEM_CAPS_CACHE |
			SOF_MEM_CAPS_DMA,
	},
	.buffer[0] = {
		.blocks = ARRAY_SIZE(buf_heap_map),
		.map = buf_heap_map,
		.heap = HEAP_BUFFER_BASE,
		.size = HEAP_BUFFER_SIZE,
		.info = {.free = HEAP_BUFFER_SIZE,},
		.caps = SOF_MEM_CAPS_RAM | SOF_MEM_CAPS_CACHE |
			SOF_MEM_CAPS_DMA,
	},
	.total = {.free = HEAP_SYSTEM_SIZE + HEAP_SYS_RUNTIME_SIZE +
			HEAP_RUNTIME_SIZE + HEAP_BUFFER_SIZE,},
};

void platform_init_memmap(void)
{
	/*  memmap has been initialized statically as a part of .data */
}
