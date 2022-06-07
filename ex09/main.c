/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 10:24:29 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/07 18:59:35 by vvaucoul         ###   ########.fr       */
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
#include <linux/ns_common.h>
#include <linux/fs_pin.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("vvaucoul <vvaucoul@student.42.fr>");
MODULE_DESCRIPTION("List all mounted points");

/*******************************************************************************
 *                              LINUX FS/MOUNT.H                               *
 ******************************************************************************/

/*
** https://github.com/torvalds/linux/blob/master/fs/mount.h
*/

/* SPDX-License-Identifier: GPL-2.0 */
struct mnt_namespace
{
    struct ns_common ns;
    struct mount *root;
    struct list_head list;
    spinlock_t ns_lock;
    struct user_namespace *user_ns;
    struct ucounts *ucounts;
    u64 seq;
    wait_queue_head_t poll;
    u64 event;
    unsigned int mounts;
    unsigned int pending_mounts;
} __randomize_layout;

struct mnt_pcp
{
    int mnt_count;
    int mnt_writers;
};

struct mountpoint
{
    struct hlist_node m_hash;
    struct dentry *m_dentry;
    struct hlist_head m_list;
    int m_count;
};

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

#define MNT_NS_INTERNAL ERR_PTR(-EINVAL)

static inline struct mount *real_mount(struct vfsmount *mnt)
{
    return container_of(mnt, struct mount, mnt);
}

static inline int mnt_has_parent(struct mount *mnt)
{
    return mnt != mnt->mnt_parent;
}

static inline int is_mounted(struct vfsmount *mnt)
{
    return !IS_ERR_OR_NULL(real_mount(mnt)->mnt_ns);
}

extern struct mount *__lookup_mnt(struct vfsmount *, struct dentry *);

extern int __legitimize_mnt(struct vfsmount *, unsigned);
extern bool legitimize_mnt(struct vfsmount *, unsigned);

static inline bool __path_is_mountpoint(const struct path *path)
{
    struct mount *m = __lookup_mnt(path->mnt, path->dentry);
    return m && likely(!(m->mnt.mnt_flags & MNT_SYNC_UMOUNT));
}

extern void __detach_mounts(struct dentry *dentry);

static inline void detach_mounts(struct dentry *dentry)
{
    if (!d_mountpoint(dentry))
        return;
    __detach_mounts(dentry);
}

static inline void get_mnt_ns(struct mnt_namespace *ns)
{
    refcount_inc(&ns->ns.count);
}

extern seqlock_t mount_lock;

struct proc_mounts
{
    struct mnt_namespace *ns;
    struct path root;
    int (*show)(struct seq_file *, struct vfsmount *);
    struct mount cursor;
};

extern const struct seq_operations mounts_op;

extern bool __is_local_mountpoint(struct dentry *dentry);
static inline bool is_local_mountpoint(struct dentry *dentry)
{
    if (!d_mountpoint(dentry))
        return false;

    return __is_local_mountpoint(dentry);
}

static inline bool is_anon_ns(struct mnt_namespace *ns)
{
    return ns->seq == 0;
}

extern void mnt_cursor_del(struct mnt_namespace *ns, struct mount *cursor);

/*******************************************************************************
 *                                    MAIN                                     *
 ******************************************************************************/

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