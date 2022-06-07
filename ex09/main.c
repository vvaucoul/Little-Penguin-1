/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 10:24:29 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/07 16:51:13 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/mount.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/seq_file.h>
#include <linux/poll.h>
#include <linux/nsproxy.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("vvaucoul <vvaucoul@student.42.fr>");
MODULE_DESCRIPTION("List all mounted points");

/* REF: https://android.googlesource.com/kernel/common/+/refs/heads/android12-5.4/fs/mount.h */
struct mount
{
    struct hlist_node mnt_hash;
    struct mount *mnt_parent;
    struct dentry *mnt_mountpoint;
    struct vfsmount mnt;
    union
    {
        struct rcu_head mnt_rcu;
        struct llist_node mnt_llist;
    };
#ifdef CONFIG_SMP
    struct mnt_pcp __percpu *mnt_pcp;
#else
    int mnt_count;
    int mnt_writers;
#endif
    struct list_head mnt_mounts;
    struct list_head mnt_child;
    struct list_head mnt_instance;
    const char *mnt_devname;
    struct list_head mnt_list;
    struct list_head mnt_expire;
    struct list_head mnt_share;
    struct list_head mnt_slave_list;
    struct list_head mnt_slave;
    struct mount *mnt_master;
    struct mnt_namespace *mnt_ns;
    struct mountpoint *mnt_mp;
    union
    {
        struct hlist_node mnt_mp_list;
        struct hlist_node mnt_umount;
    };
    struct list_head mnt_umounting;
#ifdef CONFIG_FSNOTIFY
    struct fsnotify_mark_connector __rcu *mnt_fsnotify_marks;
    __u32 mnt_fsnotify_mask;
#endif
    int mnt_id;
    int mnt_group_id;
    int mnt_expiry_mark;
    struct hlist_head mnt_pins;
    struct hlist_head mnt_stuck_children;
} __randomize_layout;

struct ns_common
{
    atomic_long_t stashed;
    const struct proc_ns_operations *ops;
    unsigned int inum;
    refcount_t count;
};

/* REF: https://android.googlesource.com/kernel/common/+/refs/heads/android12-5.4/fs/mount.h */
struct mnt_namespace
{
    struct ns_common ns;
    struct mount *root;
    struct list_head list;
    /* Namespace's spinlock */
    spinlock_t ns_lock;
    struct user_namespace *user_ns;
    struct ucounts *ucounts;
    u64 seq;
    wait_queue_head_t poll;
    u64 event;
    unsigned int mounts;
    unsigned int pending_mounts;
} __randomize_layout;

#define PROC_MOUNT_NAME "mymounts"

static struct proc_dir_entry *entry;

static ssize_t mp_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos)
{
    return (count);
}

static void kstrjoin(unsigned char **buff, size_t *length, const char *str)
{
    size_t new_length;
    char *tmp;

    new_length = strlen(str);
    tmp = krealloc(*buff, *length + new_length, GFP_KERNEL);
    if (!tmp)
        kfree(*buff);
    else
        memcpy(tmp + *length, str, new_length);
    *buff = tmp;
    *length += new_length;
}

static unsigned char *get_mounted_path(char *buff, struct mount *mnt)
{
    struct path mount_path =
        {
            .dentry = mnt->mnt.mnt_root,
            .mnt = &mnt->mnt,
        };
    return (d_path(&mount_path, buff, PATH_MAX + 1));
}

static char *get_mounted_points_string(size_t *length)
{
    unsigned char *buff = NULL;
    struct mount *mnt = NULL;
    struct mnt_namespace *c_namespace = current->nsproxy->mnt_ns;
    char *path;

    path = kmalloc(PATH_MAX + 1, GFP_KERNEL);
    if (!path)
    {
        *length = 0;
        return (NULL);
    }
    list_for_each_entry(mnt, &c_namespace->list, mnt_list)
    {
        kstrjoin(&buff, length, mnt->mnt_devname);
        if (!buff)
            break;
        kstrjoin(&buff, length, " ");
        if (!buff)
            break;
        kstrjoin(&buff, length, get_mounted_path(path, mnt));
        if (!buff)
            break;
        kstrjoin(&buff, length, "\n");
        if (!buff)
            break;
    }
    if (!buff)
    {
        *length = 0;
        return (NULL);
    }
    return (buff);
}

static ssize_t mp_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
    char *buffer = NULL;
    ssize_t ret;

    ret = 0;
    buffer = get_mounted_points_string(&ret);
    if (buffer == NULL)
    {
        printk(KERN_ERR "Error: mount points string is invalid\n");
        return (-ENOMEM);
    }
    ret = simple_read_from_buffer(ubuf, count, ppos, buffer, ret);
    kfree(buffer);
    return (ret);
}

static const struct proc_ops mp_ops = {
    .proc_read = mp_read,
    .proc_write = mp_write,
};

static int __init init_list_mount_points(void)
{
    printk(KERN_DEBUG "init_list_mount_points\n");
    entry = proc_create(PROC_MOUNT_NAME, 0444, NULL, &mp_ops);
    if (entry == NULL)
    {
        printk(KERN_ERR "Error creating proc entry\n");
        return -1;
    }
    return (0);
}

static void __exit exit_list_mount_points(void)
{
    printk(KERN_INFO "Cleanup module\n");
    if (entry)
        proc_remove(entry);
}

module_init(init_list_mount_points);
module_exit(exit_list_mount_points);