#include "deploy_dev.h"
#include "deploy.h"

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DEV_NAME "friend_loader"
#define DEV_NAME2 "friend_loader2"
#define MINOR_COUNT 1

static dev_t deploy_dev_id;
static dev_t deploy_dev_id2;
static struct cdev deploy_chardev;
static struct cdev deploy_chardev2;

static int deploy_dev_open(struct inode* inode, struct file* filep);
static int deploy_dev_release(struct inode* inode, struct file* filep);
static ssize_t deploy_dev_write(
    struct file* filep, const char __user* buf, size_t count, loff_t* offset);
static ssize_t deploy_dev_write2(
    struct file* filep, const char __user* buf, size_t count, loff_t* offset);

static struct file_operations deploy_fops = {
    .owner = THIS_MODULE,
    .open = deploy_dev_open,
    .release = deploy_dev_release,
    .write = deploy_dev_write,
    .llseek = generic_file_llseek,
};
static struct file_operations deploy_fops2 = {
    .owner = THIS_MODULE,
    .open = deploy_dev_open,
    .release = deploy_dev_release,
    .write = deploy_dev_write2,
    .llseek = generic_file_llseek,
};

static char* write_buf;
static char* write_buf2;

int __init deploy_dev_init(void)
{
    int ret = alloc_chrdev_region(&deploy_dev_id, 0, MINOR_COUNT, DEV_NAME);
    if (ret < 0) {
        pr_warn("deploy_dev_init: alloc device number failed: %d\n", ret);
        return -1;
    }
    ret = alloc_chrdev_region(&deploy_dev_id2, 0, MINOR_COUNT, DEV_NAME2);
    if (ret < 0) {
        pr_warn("deploy_dev_init: alloc device number failed: %d\n", ret);
        return -1;
    }

    cdev_init(&deploy_chardev, &deploy_fops);
    cdev_init(&deploy_chardev2, &deploy_fops2);
    deploy_chardev.owner = THIS_MODULE;
    deploy_chardev2.owner = THIS_MODULE;
    if (cdev_add(&deploy_chardev, deploy_dev_id, MINOR_COUNT)) {
        pr_warn("deploy_dev_init: failed to add cdev\n");
        return -1;
    }
    if (cdev_add(&deploy_chardev2, deploy_dev_id2, MINOR_COUNT)) {
        pr_warn("deploy_dev_init: failed to add cdev\n");
        return -1;
    }

    pr_info(
        "deploy_init: please run 'mknod /dev/friend_loader c %d 0'\n",
        MAJOR(deploy_dev_id));
    pr_info(
        "deploy_init: please run 'mknod /dev/friend_loader2 c %d 0'\n",
        MAJOR(deploy_dev_id2));

    write_buf = (char*)kmalloc(KMALLOC_MAX_SIZE, GFP_KERNEL);
    write_buf2 = (char*)kmalloc(KMALLOC_MAX_SIZE, GFP_KERNEL);
    if (write_buf == NULL) {
        pr_warn("deploy_dev_init: failed to allocate buf\n");
        return -ENOSPC;
    }
    if (write_buf2 == NULL) {
        pr_warn("deploy_dev_init: failed to allocate buf\n");
        return -ENOSPC;
    }

    return 0;
}

void __exit deploy_dev_exit(void)
{
    kfree(write_buf);
    kfree(write_buf2);

    cdev_del(&deploy_chardev);
    cdev_del(&deploy_chardev2);
    unregister_chrdev_region(deploy_dev_id, MINOR_COUNT);
    unregister_chrdev_region(deploy_dev_id2, MINOR_COUNT);
}

static int deploy_dev_open(struct inode* inode, struct file* filep)
{
    pr_debug("deploy_dev: open\n");
    return 0;
}

static int deploy_dev_release(struct inode* inode, struct file* filep)
{
    pr_debug("deploy_dev: release\n");
    return 0;
}

static ssize_t deploy_dev_write_body(
    struct file* filep, const char __user* buf, size_t count, loff_t* offset,
    int tcpu)
{
    size_t written_size = 0;
    size_t size;
    char *wbuf = NULL;

    if (tcpu == 1) {
        wbuf = write_buf;
    } else if (tcpu == 2) {
        wbuf = write_buf2;
    } else {
        pr_warn("friend_loader: target cpu value %d wrong\n", tcpu);
        return -EFAULT;
    }

    while (written_size < count) {
        size = min(count - written_size, KMALLOC_MAX_SIZE);

        if (copy_from_user(wbuf, buf + written_size, size)) {
            pr_warn("friend_loader: copy_from_user failed\n");
            return -EFAULT;
        }

        if (deploy(wbuf, size, filep->f_pos + written_size, tcpu) < 0) {
            pr_warn("friend_loader: deploy failed\n");
            return -EFBIG;
        }

        written_size += size;
    }

    *offset += count;

    return count;
}

static ssize_t deploy_dev_write(
    struct file* filep, const char __user* buf, size_t count, loff_t* offset)
{
    return deploy_dev_write_body(filep, buf, count, offset, 1);
}

static ssize_t deploy_dev_write2(
    struct file* filep, const char __user* buf, size_t count, loff_t* offset)
{
    return deploy_dev_write_body(filep, buf, count, offset, 2);
}
