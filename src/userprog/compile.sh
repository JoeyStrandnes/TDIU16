#!/bin/bash

make clean all;

make -j8 -C ../examples

pintos -v -k --fs-disk=2 -p ../examples/file_syscall_tests -a file_syscall_tests -- -f -q run 'file_syscall_tests' > output.log
