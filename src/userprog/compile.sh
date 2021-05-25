#!/bin/bash
clear
make clean all;

make -j8 -C ../examples

#pintos -v -k --fs-disk=2 -p ../examples/file_syscall_tests -a file_syscall_tests -- -f -q run 'file_syscall_tests' > output.log
#pintos --fs-disk=2 -k -p ../examples/line_echo -a line_echo -- -f -q run line_echo #> output.log
#pintos -v -k --fs-disk=2 -p ../examples/file_syscall_tests -a fst -- -f -q run fst #> output.log
#pintos -v -k --fs-disk=2 -p ../examples/sumargv -a sumargv -- -f -q run 'sumargv 1 2 3 4'
#pintos -v -k --fs-disk=2 -p ../examples/longrun_nowait -a no_wait -- -f -q run 'no_wait 2 2'
pintos -v -k --fs-disk=2 -p ../examples/longrun -a longrun -- -f -q run 'longrun 10 10'
#pintos -v -k --fs-disk=2 -p ../examples/wait_test -a wait_test -- -f -q run 'wait_test'
#clear
