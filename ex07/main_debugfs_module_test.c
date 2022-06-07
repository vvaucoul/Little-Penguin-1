/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_debugfs_module_test.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/03 18:49:01 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/03 21:34:34 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define TEST_FOO_NBR 128
#define THREAD_DELAY 1000

typedef struct s_t_thread
{
    size_t id;
    char *msg;
} t_thread_test;

static int open_file(const char *f_name, int flags);

static void *routine(void *ptr)
{
    t_thread_test *thr = (t_thread_test *)ptr;
    char *msg = (char *)thr->msg;
    char buff[128];
    int fd = 0;
    size_t i = 0;

    while (1)
    {
        bzero(buff, 128);
        fd = open_file("/sys/kernel/debug/fortytwo/foo", O_RDWR);
        write(fd, "msg", strlen(msg));
        printf("[%ld] Write: %s\n", thr->id, msg);
        usleep(THREAD_DELAY);
        close(fd);
        fd = open_file("/sys/kernel/debug/fortytwo/foo", O_RDWR);
        read(fd, buff, sizeof(buff));
        printf("[%ld] Read: %s\n", thr->id, buff);
        usleep(THREAD_DELAY);
        close(fd);
        ++i;
        if (i > 10)
            return ((void *)-1);
    }
}

static int open_file(const char *f_name, int flags)
{
    int fd = open(f_name, flags);
    if (fd < 0)
    {
        perror("open");
        return (-1);
    }
    return (fd);
}

int main(void)
{
    /*******************************************************************************
     *                                     ID                                      *
     ******************************************************************************/

    // WRITE
    printf("- Test ID\n");
    int fd = open_file("/sys/kernel/debug/fortytwo/id", O_RDWR);
    int ret = 0;
    ret = write(fd, "vvaucoul", 9);
    printf("Write 'vvaucoul' in id | %d\n", ret);
    close(fd);

    // READ
    char buff[9];
    bzero(&buff, 9);
    fd = open_file("/sys/kernel/debug/fortytwo/id", O_RDWR);
    read(fd, buff, 8);
    close(fd);
    printf("Read '%s' from ID\n", buff);

    /*******************************************************************************
     *                                   JIFFIES                                   *
     ******************************************************************************/

    // WRITE

    printf("- Test JIFFIES\n");
    printf("- Check open permissions\n");
    fd = open_file("/sys/kernel/debug/fortytwo/jiffies", O_RDWR);
    fd = open_file("/sys/kernel/debug/fortytwo/jiffies", O_RDONLY);
    ret = 0;
    ret = write(fd, "vvaucoul", 9);
    printf("Write 'vvaucoul' in id | %d\n", ret);
    close(fd);

    // READ
    char buff_jiffies[128];
    bzero(&buff_jiffies, 128);
    size_t loop = 0;
    while (loop < 10)
    {
        bzero(&buff_jiffies, 128);
        fd = open_file("/sys/kernel/debug/fortytwo/jiffies", O_RDONLY);
        read(fd, buff_jiffies, 127);
        printf("Read %s", buff_jiffies);
        ++loop;
        usleep(500000);
        close(fd);
    }

    /*******************************************************************************
     *                                     FOO                                     *
     ******************************************************************************/

    // todo foo test

    printf("- Test FOO\n");
    pthread_t thread[TEST_FOO_NBR];
    for (size_t i = 0; i < TEST_FOO_NBR; i++)
    {
        printf("   - Create thread [%ld]\n", i);
        pthread_create(&thread[i], NULL, &routine, &((t_thread_test){i, (char *){"vvaucoul"}}));
        usleep(THREAD_DELAY);
    }
    for (size_t i = 0; i < TEST_FOO_NBR; i++)
    {
        printf("   - Join thread [%ld]\n", i);
        pthread_join(thread[i], NULL);
    }
    printf("Exit test\n");
    return (0);
}