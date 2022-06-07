# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/05/31 15:38:49 by vvaucoul          #+#    #+#              #
#    Updated: 2022/06/07 18:13:43 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

obj-m += main.o

all:
	@echo "Compiling..."
	@make -s -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules > /dev/null 2>&1
	@echo "Done."

install:
	@echo "Installing module..."
	@cp -f usb-driver-udev.rules /etc/udev/rules.d/100-usb-driver-udev.rules
	@cp -f main.ko /main.ko
	@make -s reload-udev
	@echo "Done."

clean:
	@echo "Cleaning..."
	@make -s -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean > /dev/null 2>&1
	@rm -rf /etc/udev/rules.d/100-usb-driver-udev.rules
	@echo "Done."

reload-udev:
	@echo "Reloading udev..."
	@sudo service udev restart
	@sudo udevadm control --reload
	@sudo udevadm trigger
	@echo "Done."

re: clean all

.PHONY: all clean re install reload-udev