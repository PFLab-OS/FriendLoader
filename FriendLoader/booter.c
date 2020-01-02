#include "booter.h"
#include "cpu_hotplug.h"

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

/* above function (boot_store) is set as booter_attr.store by __ATTR_WO macro */
static struct kobj_attribute booter_attr = __ATTR_WO(boot);

static struct attribute *friend_loader_attrs[] = {
    &booter_attr.attr,
    NULL,
};

static struct attribute_group friend_loader_attr_group = {
    .attrs = friend_loader_attrs,
};


int booter_init(void) {
    int ret;

    ret = sysfs_create_group(&THIS_MODULE->mkobj.kobj,
                             &friend_loader_attr_group);
    if (ret != 0) {
        pr_warn("friend_loader_init: sysfs_create_group failed: %d\n", ret);
        return -1;
    }

    return 0;
}


void booter_exit(void) {}


