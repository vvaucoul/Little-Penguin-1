/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_write_misc_char_device.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/03 11:10:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/03 12:43:15 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(void)
{
    printf("Open /dev/fortytwo\n");

    // Open misc device driver
    int fd = open("/dev/fortytwo", O_RDWR);
    int length = 0;
    if (fd < 0)
    {
        perror("open");
        return (1);
    }

    // Write to misc device driver
    printf("Write 'Hello world' in /dev/fortytwo\n");
    length = write(fd, "Hello world", 12);
    printf("Write %d bytes\n", length);

    printf("Write '42Born2Code' in /dev/fortytwo\n");
    length = write(fd, "42Born2Code", 12);
    printf("Write %d bytes\n", length);

    printf("Write 'vvaucoul' in /dev/fortytwo\n");
    length = write(fd, "vvaucoul", 9);
    printf("Write %d bytes\n", length);
    close(fd);

    // Open misc device driver
    fd = open("/dev/fortytwo", O_RDWR);
    length = 0;
    if (fd < 0)
    {
        perror("open");
        return (1);
    }
    char buf[9];
    // Read from misc device driver
    bzero(buf, sizeof(buf));
    length = read(fd, buf, 9);
    printf("Read '%s' from /dev/fortytwo, length: %d\n", buf, length);

    // Close misc device driver
    close(fd);
    return (0);
}