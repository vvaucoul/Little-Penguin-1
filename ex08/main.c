#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");

static ssize_t myfd_read(struct file *fp, char __user *user, size_t size, loff_t *offs);
static ssize_t myfd_write(struct file *fp, const char __user *user, size_t size, loff_t *offs);

static const struct file_operations myfd_fops = {
    .owner = THIS_MODULE,
    .read = &myfd_read,
    .write = &myfd_write
};

static struct miscdevice myfd_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "reverse",
    .fops = &myfd_fops
};

char str[PAGE_SIZE];

static int __init myfd_init(void)
{
    int retval;

    retval = misc_register(&myfd_device);
    if (retval < 0)
        return (1);
    return (1);
}

static void __exit myfd_cleanup(void)
{
    misc_deregister(&myfd_device);
}

ssize_t myfd_read(struct file *fp, char __user *user, size_t size, loff_t *offs)
{
    size_t i;
    ssize_t res;
    char *tmp;

    /**************** Malloc like a boss ***************/
    tmp = kmalloc(sizeof(char) * PAGE_SIZE * 2, GFP_KERNEL);
    for (i = 0; i < strlen(str); i++)
        tmp[i] = str[strlen(str) - i - 1];
    tmp[i] = 0x0;

    res = simple_read_from_buffer(user, size, offs, tmp, i);
    kfree(tmp);
    return (res);
}
ssize_t myfd_write(struct file *fp, const char __user *user, size_t size, loff_t *offs)
{
    return (simple_write_to_buffer(str, sizeof(str), offs, user, size));
}

module_init(myfd_init);
module_exit(myfd_cleanup);