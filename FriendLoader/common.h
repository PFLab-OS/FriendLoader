#pragma once

// File deployment
// XXX: Assuming Linux is running on HiKey960 board and with `mem=2G` boot parameter.
// memory space is 80896KB (> 79MB)
#define DEPLOY_PHYS_ADDR_START	0xba700000UL  // 0xba6fc000UL
#define DEPLOY_PHYS_ADDR_END	0xbf6bffffUL

// Time measuring
#include <linux/time.h>

static inline long ktime_diff_ns(ktime_t start, ktime_t end)
{
    return ktime_to_ns(ktime_sub(end, start));
}
