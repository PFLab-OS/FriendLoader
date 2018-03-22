#include <linux/cpu.h>
#include <linux/psci.h>

#include <asm/smp_plat.h>

#include "cpu_hotplug.h"

static int unpluged_cpu = -1;
static void select_unplug_cpu(void);

int __init cpu_unplug(void)
{
    int ret;

    select_unplug_cpu();

    ret = cpu_down(unpluged_cpu);
    if (ret < 0) {
        return ret;
    }

    return unpluged_cpu;
}

int cpu_start(phys_addr_t entry_point)
{
    if (unpluged_cpu == -1) {
        return -1;
    }

    psci_ops.cpu_on(cpu_logical_map(unpluged_cpu), entry_point);

    return 0;
}

int __exit cpu_replug(void)
{
    int ret = cpu_up(unpluged_cpu);
    if (ret < 0) {
        return ret;
    }

    return unpluged_cpu;
}

static void select_unplug_cpu(void)
{
    // TODO: Choose an appropriate CPU regarding situation.
    //       Currently choosing CPU 7 by deciding.

    unpluged_cpu = 7;
}
