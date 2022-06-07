/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 17:51:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/07 12:43:17 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vaucouleur Vincent <vvaucoul@student.42.fr>");
MODULE_DESCRIPTION("USB Keyboard Info Loader module");

static int __init usb_keyboard_init(void)
{
    printk(KERN_INFO "USB Keyboard plugged!\n");
    return 0;
}

static void __exit usb_keyboard_exit(void)
{
    printk(KERN_INFO "USB Keyboard unplugged!\n");
}

module_init(usb_keyboard_init);
module_exit(usb_keyboard_exit);