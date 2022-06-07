/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 12:36:52 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/07 15:08:32 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vaucouleur Vincent <vvaucoul@student.42.fr>");
MODULE_DESCRIPTION("A simple Hello World module");

static int __init_hello_init(void)
{
    printk(KERN_INFO "Hello world !\n");
    return (0);
}

static void __exit_hello_cleanup(void)
{
    printk(KERN_INFO "Cleaning up module.\n");
}

module_init(__init_hello_init);
module_exit(__exit_hello_cleanup);