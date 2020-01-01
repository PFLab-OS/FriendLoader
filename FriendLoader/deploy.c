#include "common.h"
#include "deploy.h"

#include <asm/io.h>
#include <linux/kernel.h>

#define MAXSIZE 0x100000 /* 1MB */

static void write_phys_mem(phys_addr_t addr, const void* data, size_t size);

int deploy(const char* data, size_t size, loff_t offset, int tcpu)
{
    phys_addr_t addr;

    if (offset + size > MAXSIZE) {
        pr_warn("deploy: file size too large\n");
        return -1;
    }

    if (tcpu == 1)
        addr = PADDR1;
    else
        addr = PADDR2;

    write_phys_mem(addr + offset, data, size);
    pr_debug("deploy: deployed to physical memory [%llx - %llx] (%zu)\n",
        addr + offset,
        addr + offset + size,
        size);

    return 0;
}

static void write_phys_mem(phys_addr_t addr, const void* data, size_t size)
{
    void __iomem* io_addr = ioremap(addr, size);
    memcpy_toio(io_addr, data, size);
    iounmap(io_addr);
}
