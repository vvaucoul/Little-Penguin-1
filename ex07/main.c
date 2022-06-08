/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 12:36:52 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/08 19:35:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vaucouleur Vincent <vvaucoul@student.42.fr>");
MODULE_DESCRIPTION("A simple debug interface using debugfs");

#define DFS_DIR_NAME "fortytwo"

#define DFS_VIRTUAL_FILE_01 "id"
#define DFS_VIRTUAL_FILE_02 "jiffies"
#define DFS_VIRTUAL_FILE_03 "foo"

static const char login[9] = "vvaucoul";
static const short login_length = 8;

/*******************************************************************************
 *                                     ID                                      *
 ******************************************************************************/

static ssize_t id_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    return (simple_read_from_buffer(buf, count, ppos, login, strlen(login)));
}

static ssize_t id_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
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

/*******************************************************************************
 *                                   JIFFIES                                   *
 ******************************************************************************/

static ssize_t jiffies_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    char timer[128];
    size_t length = 0;
    volatile unsigned long time = jiffies;

    length = sprintf(timer, "Jiffies Kernel Timer: %lu\n", time);
    return (simple_read_from_buffer(buf, count, ppos, timer, length));
}

static ssize_t jiffies_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    return (count);
}

/*******************************************************************************
 *                                     FOO                                     *
 ******************************************************************************/

static char foo_buffer[PAGE_SIZE];
size_t foo_srfbuf_size = 0;
DEFINE_MUTEX(foo_mutex);

static ssize_t foo_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    ssize_t ret = 0;

    mutex_lock(&foo_mutex);
    ret = simple_read_from_buffer(buf, count, ppos, foo_buffer, foo_srfbuf_size);
    mutex_unlock(&foo_mutex);
    return (ret);
}

static ssize_t foo_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    ssize_t ret = 0;

    mutex_lock(&foo_mutex);
    ret = simple_write_to_buffer(foo_buffer, sizeof(foo_buffer), ppos, buf, count);
    foo_srfbuf_size = ret;
    mutex_unlock(&foo_mutex);
    return (ret);
}

/*******************************************************************************
 *                                 STRUCTURES                                  *
 ******************************************************************************/

const struct file_operations id_fops = {
    .owner = THIS_MODULE,
    .read = id_read,
    .write = id_write,
};

const struct file_operations jiffies_fops = {
    .owner = THIS_MODULE,
    .read = jiffies_read,
    .write = jiffies_write,
};

const struct file_operations foo_fops = {
    .owner = THIS_MODULE,
    .read = foo_read,
    .write = foo_write,
};

struct dentry *d_entry_root = NULL;
struct dentry *d_entry_id = NULL;
struct dentry *d_entry_jiffies = NULL;
struct dentry *d_entry_foo = NULL;

/*******************************************************************************
 *                                   DEFAULT                                   *
 ******************************************************************************/

static void cleanup_debugfs_module(void)
{
    debugfs_remove(d_entry_foo);
    debugfs_remove(d_entry_jiffies);
    debugfs_remove(d_entry_id);
    debugfs_remove(d_entry_root);
}

static int __init debugfs_init(void)
{
    printk(KERN_INFO "Init debugfs module interace!\n");
    d_entry_root = debugfs_create_dir(DFS_DIR_NAME, NULL);

    // check errors
    if (ERR_PTR(-ENODEV) == NULL)
    {
        printk(KERN_ERR "Error: your kernel doesn't support debugfs !\n");
        return (1);
    }
    if (!d_entry_root)
    {
        printk(KERN_ERR "Error: debugfs_create_dir failed !\n");
        return (1);
    }

    // create dfs files
    d_entry_id = debugfs_create_file(DFS_VIRTUAL_FILE_01, 0666, d_entry_root, NULL, &id_fops);
    if (!d_entry_id)
    {
        printk(KERN_ERR "Error: debugfs_create_file failed !\n");
        cleanup_debugfs_module();
        return (1);
    }
    d_entry_jiffies = debugfs_create_file(DFS_VIRTUAL_FILE_02, 0444, d_entry_root, NULL, &jiffies_fops);
    if (!d_entry_jiffies)
    {
        printk(KERN_ERR "Error: debugfs_create_file failed !\n");
        cleanup_debugfs_module();
        return (1);
    }
    d_entry_foo = debugfs_create_file(DFS_VIRTUAL_FILE_03, 0644, d_entry_root, NULL, &foo_fops);
    if (!d_entry_foo)
    {
        printk(KERN_ERR "Error: debugfs_create_file failed !\n");
        cleanup_debugfs_module();
        return (1);
    }
    return (0);
}

static void __exit debugfs_exit(void)
{
    printk(KERN_INFO "Exit debugfs module interace!\n");
    cleanup_debugfs_module();
}

module_init(debugfs_init);
module_exit(debugfs_exit);