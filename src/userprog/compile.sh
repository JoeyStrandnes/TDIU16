#!/bin/bash
clear
make clean all;

make -j8 -C ../examples

#pintos -v -k --fs-disk=2 -p ../examples/file_syscall_tests -a file_syscall_tests -- -f -q run 'file_syscall_tests' > output.log
#pintos --fs-disk=2 -k -p ../examples/line_echo -a line_echo -- -f -q run line_echo #> output.log
pintos -v -k --fs-disk=2 -p ../examples/file_syscall_tests -a fst -- -f -q run fst #> output.log
#clear
