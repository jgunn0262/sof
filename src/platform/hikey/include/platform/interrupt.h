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

#ifndef __INCLUDE_PLATFORM_INTERRUPT__
#define __INCLUDE_PLATFORM_INTERRUPT__

#include <stdint.h>
#include <sof/string.h>
#include <sof/interrupt-map.h>

/* IRQ numbers - TODO: To be completed and confirmed */
/* IRQ numbers  - lots of external IRQs missing due to lack of docs */
#define IRQ_NUM_EXT_IPC         3       /* Level 3 */
#define IRQ_NUM_SOFTWARE1       1       /* Level 3 */
#define IRQ_NUM_TIMER1          5       /* Level 3 */
#define IRQ_NUM_TIMER2          6       /* Level 4 */
#define IRQ_NUM_PROFILE         19      /* Level 3 */
#define IRQ_NUM_WRITE_ERR       29      /* Level 3 */
#define IRQ_NUM_NMI             0       /* Level 6 */

/* IRQ Masks */
#define IRQ_MASK_EXT_IPC        (1 << IRQ_NUM_EXT_IPC)
#define IRQ_MASK_TIMER1         (1 << IRQ_NUM_TIMER1)
#define IRQ_MASK_SOFTWARE1      (1 << IRQ_NUM_SOFTWARE1)
#define IRQ_MASK_TIMER2         (1 << IRQ_NUM_TIMER2)
#define IRQ_MASK_PROFILE        (1 << IRQ_NUM_PROFILE)
#define IRQ_MASK_WRITE_ERR      (1 << IRQ_NUM_WRITE_ERR)


/* no nested interrupts */
#define PLATFORM_IRQ_CHILDREN	0

#endif
