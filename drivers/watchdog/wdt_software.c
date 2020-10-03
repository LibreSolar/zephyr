/*
 * Copyright (c) 2020 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "wdt_software.h"

#include <errno.h>

#define LOG_LEVEL CONFIG_WDT_LOG_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(wdt_software);

/* array of all software watchdog channels */
static struct sw_wdt_channel sw_wdt_channels[CONFIG_WDT_SOFTWARE_CHANNELS];

/* number of currently used channels */
static int sw_wdt_channel_count;

/**
 * @brief Software watchdog timer process function.
 *
 * This function goes through the installed software watchdog channels and
 * checks if they timed out. Only if no timeout was exceeded, the hardware
 * watchdog is fed.
 *
 * @retval 0 If all software watchdog channels were processed successfully
 * @retval -ETIMEDOUT If at least one of the software watchdog channels timed
 *                    out and a reset should be triggered
 */
static int sw_wdt_process(void)
{
	int64_t current_time = k_uptime_get();

	for (int i = 0; i < sw_wdt_channel_count; i++) {
		if ((current_time - sw_wdt_channels[i].feed_time) >
				sw_wdt_channels[i].timeout) {
			LOG_ERR("Software watchdog channel %d triggered!", i);
			return -ETIMEDOUT;
		}
	}

	return 0;
}

int sw_wdt_install_timeout(uint32_t timeout_ms)
{
	if (sw_wdt_channel_count < CONFIG_WDT_SOFTWARE_CHANNELS) {
		sw_wdt_channels[sw_wdt_channel_count].timeout = timeout_ms;
		sw_wdt_channel_count++;
		/* sw_wdt channels start counting from 1 */
		return sw_wdt_channel_count;
	} else {
		return -ENOMEM;
	}
}

int sw_wdt_feed(int channel_id)
{
	if (channel_id > 0 && channel_id <= sw_wdt_channel_count) {
		sw_wdt_channels[channel_id - 1].feed_time = k_uptime_get();
		return sw_wdt_process();
	} else {
		return -EINVAL;
	}
}

uint32_t sw_wdt_min_timeout(void)
{
	uint32_t min_timeout = sw_wdt_channels[0].timeout;

	for (int i = 1; i < sw_wdt_channel_count; i++) {
		if (sw_wdt_channels[i].timeout < min_timeout) {
			min_timeout = sw_wdt_channels[i].timeout;
		}
	}

	return (sw_wdt_channel_count > 0) ? min_timeout : 0;
}
