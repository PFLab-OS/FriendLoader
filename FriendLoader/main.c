#include "common.h"
#include "cpu_hotplug.h"
#include "deploy_dev.h"
#include "flsysfs.h"

#include <linux/kernel.h>
#include <linux/module.h>

MODULE_DESCRIPTION("Friend Loader");
MODULE_LICENSE("GPL v2");

static void clear_friend_loader_buf(void)
{
    int i;
    for (i = 0; i < 4096/sizeof(long long); i++) {
        long long *llp = (long long*)__friend_loader_buf;
        llp[i] = 0;
    }
}

static int __init friend_loader_init(void)
{
    int ret;

    pr_info("friend_loader_init: init\n");

    // Device for storing program
    deploy_dev_init();

    ret = flsysfs_init();
    if (ret != 0) {
        pr_warn("friend_loader_init: flsysfs_init failed: %d\n", ret);
        return -1;
    }

    // Unplug friend core
    ret = cpu_unplug();
    if (ret < 0) {
        pr_warn("friend_loader_init: cpu_unplug failed: %d\n", ret);
        return -1;
    }
    pr_info("friend_loader_init: cpu %x down\n", ret);

    clear_friend_loader_buf();
    pr_info("friend_loader_init: buf cleared\n");

    return 0;
}

static void __exit friend_loader_exit(void)
{
    int ret = cpu_replug();
    if (ret < 0) {
        pr_warn("friend_loader_exit: cpu_replug failed: %d\n", ret);
    } else {
        pr_info("friend_loader_exit: cpu %x up\n", ret);
    }

    deploy_dev_exit();
    flsysfs_exit();

    pr_info("friend_loader_exit: exit\n");
}

module_init(friend_loader_init);
module_exit(friend_loader_exit);
