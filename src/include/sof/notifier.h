/*
 * Copyright (c) 2016, Intel Corporation
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
 * Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
 */

#ifndef __INCLUDE_NOTIFIER__
#define __INCLUDE_NOTIFIER__

#include <stdint.h>
#include <sof/list.h>
#include <sof/lock.h>

struct sof;

/* notifier target core masks */
#define NOTIFIER_TARGET_CORE_MASK(x)	(1 << x)
#define NOTIFIER_TARGET_CORE_ALL_MASK	0xFFFFFFFF

enum notify_id {
	NOTIFIER_ID_CPU_FREQ = 0,
	NOTIFIER_ID_I2S_FREQ,
	NOTIFIER_ID_KPB_CLIENT_EVT,
};

struct notify {
	spinlock_t lock;	/* notifier lock */
	struct list_item list;	/* list of notifiers */
};

struct notify_data {
	enum notify_id id;
	uint32_t message;
	uint32_t target_core_mask;
	uint32_t data_size;
	void *data;
};

struct notifier {
	enum notify_id id;
	struct list_item list;
	void *cb_data;
	void (*cb)(int message, void *cb_data, void *event_data);
};

struct notify **arch_notify_get(void);

void notifier_register(struct notifier *notifier);
void notifier_unregister(struct notifier *notifier);

void notifier_notify(void);
void notifier_event(struct notify_data *notify_data);

void init_system_notify(struct sof *sof);

void free_system_notify(void);

#endif
