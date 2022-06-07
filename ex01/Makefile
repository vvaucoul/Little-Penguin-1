# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/05/30 12:36:50 by vvaucoul          #+#    #+#              #
#    Updated: 2022/05/30 12:39:02 by vvaucoul         ###   ########.fr        #
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

.PHONY: all clean