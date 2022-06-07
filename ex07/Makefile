# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/05/30 12:36:50 by vvaucoul          #+#    #+#              #
#    Updated: 2022/06/03 20:27:35 by vvaucoul         ###   ########.fr        #
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
	@echo "Done."

install:
	@echo "Installing..."
	@sudo rmmod main ; sudo insmod main.ko
	@echo "Done."

test:
	@echo "Compiling..."
	@gcc main_debugfs_module_test.c -o test
	@echo "Done."

.PHONY: all clean install test