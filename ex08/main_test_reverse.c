/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_test_reverse.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 10:17:30 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/04 10:20:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(void)
{
    int fd;

    fd = open("/dev/reverse", O_RDWR);
    if (fd < 0)
    {
        perror("open");
        return (1);
    }
    else
    {
        printf("Write '%s' into '/dev/reverse'\n", "42Born2Code");
        write(fd, "42Born2Code", 12);
        close(fd);
        fd = open("/dev/reverse", O_RDWR);

        char buff[12];
        bzero(&buff, 12);
        read(fd, buff, 12);
        printf("Read '%s' from '/dev/reverse'\n", buff);
        close(fd);
    }
}