#pragma once

// File deployment
// XXX: mem=2GでLinuxが起動していると仮定
#define DEPLOY_PHYS_ADDR_START 0xba700000UL  // 0xba6fc000UL
#define DEPLOY_PHYS_ADDR_END 0xbf6bffffUL
#define DEPLOY_PHYS_MEM_SIZE (DEPLOY_PHYS_ADDR_END - DEPLOY_PHYS_ADDR_START)

// Time measuring
#include <linux/time.h>

static inline long ktime_diff_ns(ktime_t start, ktime_t end)
{
    return ktime_to_ns(ktime_sub(end, start));
}
