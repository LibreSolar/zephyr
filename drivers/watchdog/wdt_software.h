/*
 * Copyright (c) 2020 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_WATCHDOG_SOFTWARE_H_
#define ZEPHYR_DRIVERS_WATCHDOG_SOFTWARE_H_

#include <zephyr/types.h>
#include <kernel.h>
#include <device.h>

/*
 * Software watchdog channel data
 */
struct sw_wdt_channel {
	/* timeout in ms of corresponding thread/channel for sw_watchdog */
	uint32_t timeout;
	/* most recent feed time from k_uptime_get() for this channel */
	int64_t feed_time;
};

/**
 * @brief Install new timeout.
 *
 * Adds a new timeout to the list of software watchdog channels.
 *
 * @param timeout_ms Timeout in milliseconds
 *
 * @retval channel_id If successful, a positive value starting from 1
 *                    indicating the index of the channel to which the timeout
 *                    was assigned. This value is supposed to be used as the
 *                    parameter in calls to wdt_feed(). Channel 0 is reserved
 *                    for the hardware watchdog.
 * @retval -ENOMEM If no more timeouts can be installed.
 */
int sw_wdt_install_timeout(uint32_t timeout_ms);

/**
 * @brief Feed specified watchdog timeout.
 *
 * @param channel_id Index of the fed channel, must be > 0.
 *
 * @retval 0 If successful.
 * @retval -EINVAL If there is no installed timeout for supplied channel.
 */
int sw_wdt_feed(int channel_id);

/**
 * @brief Get minimum installed timeout.
 *
 * The returned smallest timeout of all installed channels must be set as
 * the timeout for the hardware watchdog to act as a fallback and trigger
 * a reset.
 *
 * @retval timeout The smallest timeout of all installed channels.
 * @retval 0 If no watchdogs have been installed.
 */
uint32_t sw_wdt_min_timeout(void);

#endif /* ZEPHYR_DRIVERS_WATCHDOG_SOFTWARE_H_ */
