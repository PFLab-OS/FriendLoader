#include "flsysfs.h"
#include "cpu_hotplug.h"

#include <linux/delay.h> /* ssleep */
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/sysfs.h>


static ssize_t boot_store(struct kobject *kobj, struct kobj_attribute *attr,
                          const char *buf, size_t count)
{
    if (cpu_start() == 0) {
        pr_info("friend_loader: starting cpu\n");
    } else {
        pr_warn("friend_loader: cpu_start failed\n");
    }

    return (ssize_t)count;
}

/* above function (boot_store) is set as boot_attr.store by __ATTR_WO macro */
static struct kobj_attribute boot_attr = __ATTR_WO(boot);

static int eof = 0;

/* for dhrystone experiment */
static ssize_t dump_show(struct kobject *kobj, struct kobj_attribute *attr,
                         char *buf)
{
    int volatile *flbuf;
    if (eof) { return -1; }

    flbuf = (int *)__friend_loader_buf;
    while (flbuf[0] != 2) { ssleep(1); }
    eof = 1;
    return sprintf(buf, "\n--- %d ---\n\n", flbuf[8]);
}

static ssize_t dump_store(struct kobject *kobj, struct kobj_attribute *attr,
                          const char *buf, size_t count)
{
    unsigned int *flbuf = (unsigned int *)__friend_loader_buf;
    unsigned int *flbuf_phys = (unsigned int *)__pa_symbol(__friend_loader_buf);
    int i, n;
    unsigned int *p;

    pr_info("FriendLoader buffer info\n");
    pr_info("Physical address is %p\n", flbuf_phys);
    pr_info("Virtual address is %p\n", flbuf); 
    pr_info("Following addresses are physical addresses\n");

    /* memory dump */
    /* size of flbuf is 4096B (from arch/arm64/kernel/head.S) */
    n = 4096 / sizeof(unsigned int) / 4; 
    for (i = 0; i < n; ++i) {
        p = flbuf + (i * 4);
	if (i != 0 && i != n-1
            && p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 0) {
            continue;
	}
        printk("%p: %08x %08x %08x %08x\n",
               flbuf_phys + (i * 4), p[0], p[1], p[2], p[3]);
    }

    return (ssize_t)count;
}

/* above functions are set as dump_attr.show/store by __ATTR_RW macro */
static struct kobj_attribute dump_attr = __ATTR_RW(dump);


static struct attribute *friend_loader_attrs[] = {
    &boot_attr.attr,
    &dump_attr.attr,
    NULL,
};

static struct attribute_group friend_loader_attr_group = {
    .attrs = friend_loader_attrs,
};


int __init flsysfs_init(void) {
    int ret;

    ret = sysfs_create_group(&THIS_MODULE->mkobj.kobj,
                             &friend_loader_attr_group);
    if (ret != 0) {
        pr_warn("friend_loader: flsysfs_init: sysfs_create_group failed: %d\n",
                ret);
        return -1;
    }

    return 0;
}


void __exit flsysfs_exit(void) {}


