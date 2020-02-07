#pragma once

// File deployment
// XXX: Assuming Linux is running on HiKey960 board and with `mem=2G` boot parameter.
// memory space is 80896KB (> 79MB)
#define PADDR1 0xba700000
#define PADDR2 0xbc700000

// Time measuring
#include <linux/ktime.h>

static inline long ktime_diff_ns(ktime_t start, ktime_t end)
{
    return ktime_to_ns(ktime_sub(end, start));
}
