#include "deploy.h"
#include "../common.h"

#include <asm/io.h>
#include <linux/kernel.h>

static void write_phys_mem(phys_addr_t addr, const void* data, size_t size)
{
    void __iomem* io_addr = ioremap(addr, size);
    memcpy_toio(io_addr, data, size);
    iounmap(io_addr);
}

int deploy(const char* data, size_t size, loff_t offset, int tcpu)
{
    phys_addr_t addr;

    if (offset + size > MAX_PROGRAM_SIZE) {
        pr_warn("deploy: file size too large\n");
        return -1;
    }

    if (tcpu == 1) {
        addr = DEPLOY_PADDR1;
    } else if (tcpu == 2) {
        addr = DEPLOY_PADDR2;
    } else {
        pr_warn("deploy: invalid tcpu %d\n", tcpu);
        return -1;
    }

    write_phys_mem(addr + offset, data, size);
    pr_debug("deploy: deployed to physical memory [%llx - %llx] (%zu)\n",
        addr + offset,
        addr + offset + size,
        size);

    return 0;
}
