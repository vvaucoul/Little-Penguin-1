#!/bin/bash
/usr/src/linux-headers-$(uname -r)/scripts/checkpatch.pl main.c -no-tree -file --strict
