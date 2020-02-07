#include "cpu_hotplug.h"
#include "../common.h"

#include <linux/cpu.h>
#include <linux/psci.h>

#include <asm/smp_plat.h>

#define CPU1 6
#define CPU2 7
#define CPUS 0xc0

int __init cpu_unplug(void)
{
    int ret;

    ret = cpu_down(CPU1);
    if (ret < 0)
        return ret;

    ret = cpu_down(CPU2);
    if (ret < 0)
        return ret;

    return CPUS;
}

int cpu_start()
{
    psci_ops.cpu_on(cpu_logical_map(CPU1), DEPLOY_PADDR1);
    psci_ops.cpu_on(cpu_logical_map(CPU2), DEPLOY_PADDR2);

    return 0;
}

int __exit cpu_replug(void)
{
    int ret = cpu_up(CPU1);
    if (ret < 0)
        return ret;
    ret = cpu_up(CPU2);
    if (ret < 0)
        return ret;

    return CPUS;
}
