/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 20:15:09 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/08 19:34:44 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vaucouleur Vincent <vvaucoul@student.42.fr>");
MODULE_DESCRIPTION("A simple misc char driver");

#define DEVICE_NAME "fortytwo"
#define CLASS_NAME "ft"

static const char login[9] = "vvaucoul";
static const short login_length = 8;

static ssize_t misc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    return (simple_read_from_buffer(buf, count, ppos, login, login_length));
}

static ssize_t misc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    char tmp[9];
    ssize_t length;

    length = simple_write_to_buffer(tmp, sizeof(tmp), ppos, buf, count);
    if (length > login_length)
        return (-EINVAL);
    if (length == login_length)
    {
        if (memcmp(tmp, login, login_length) == 0)
            return (length);
    }
    return (-EINVAL);
}

const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = misc_read,
    .write = misc_write,
};

static struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = login,
    .fops = &fops,
};

static int major = -1;
static struct class *class = NULL;
static struct device *device = NULL;

static int __init misc_init(void)
{
    printk(KERN_INFO "Initializing misc char driver with minor number %d\n", misc.minor);

    // Register Major number
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0)
    {
        printk(KERN_ERR "Registering char device failed with %d\n", major);
        return (-1);
    }

    // Register class
    class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(class))
    {
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ERR "Creating class failed with %ld\n", PTR_ERR(class));
        return (-1);
    }

    // Register device
    device = device_create(class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(device))
    {
        class_destroy(class);
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ERR "Creating device failed with %ld\n", PTR_ERR(device));
        return (-1);
    }
    return (misc_register(&misc));
}

static void __exit misc_cleanup(void)
{
    printk(KERN_INFO "Cleaning up misc char driver\n");
    device_destroy(class, device->devt);
    class_destroy(class);
    unregister_chrdev(major, DEVICE_NAME);
    misc_deregister(&misc);
}

module_init(misc_init);
module_exit(misc_cleanup);