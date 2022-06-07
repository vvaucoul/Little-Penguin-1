# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/05/30 12:36:50 by vvaucoul          #+#    #+#              #
#    Updated: 2022/06/07 15:01:41 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

obj-m += main.o

all:
	@echo "Compiling..."
	@make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	@echo "Done."

clean:
	@echo "Cleaning..."
	@make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	@rm -f test
	@echo "Done."

install:
	@sudo rmmod main ; sudo insmod main.ko

test:
	@gcc -Wall -Wextra -Werror -o test read_write_misc_char_device.c

.PHONY: all clean install test