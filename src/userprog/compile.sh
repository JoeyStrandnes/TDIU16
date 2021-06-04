#!/bin/bash
clear
make clean all;

make -j8 -C ../examples

#pintos -v -k --fs-disk=2 -p ../examples/file_syscall_tests -a file_syscall_tests -- -f -q run 'file_syscall_tests'
#pintos --fs-disk=2 -k -p ../examples/line_echo -a line_echo -- -f -q run line_echo #> output.log
#pintos -v -k --fs-disk=2 -p ../examples/file_syscall_tests -a fst -- -f -q run fst #> output.log
#pintos -v -k --fs-disk=2 -p ../examples/sumargv -a sumargv -- -f -q run 'sumargv 1 2 3 4'
#pintos -v -k --fs-disk=2 -p ../examples/longrun_nowait -a no_wait -- -f -q run 'no_wait 5 5'
#pintos -v -k --fs-disk=2 -p ../examples/longrun -a longrun -- -f -q run 'longrun 20 20'
#pintos -v -k -T 120 --fs-disk=2 -p ../examples/pfs -a pfs -p ../examples/pfs_writer -a pfs_writer -p ../examples/pfs_reader -a pfs_reader -g messages -- -f -q -F=2000 run pfs
#pintos -v -k -T 120 --fs-disk=2 --qemu -p ../examples/pfs -a pfs -p ../examples/pfs_writer -a pfs_writer -p ../examples/pfs_reader -a pfs_reader -g messages -- -f -q -F=2000 run pfs

#pintos -v -k --fs-disk=2 --qemu -p ../examples/wait_test -a wait -p ../examples/dummy -a fast -p ../examples/slow_child -a slow -- -f -q run 'wait'

#pintos -v -k --fs-disk=2 --qemu -p ../examples/longrun -a longrun -p ../examples/dummy -a dummy -- -f -q run 'longrun 5 5'

pintos -v -k -T 120 --fs-disk=2 --qemu -p ../examples/pfs -a pfs -p ../examples/pfs_writer -a pfs_writer -p ../examples/pfs_reader -a pfs_reader -g messages -- -F=2000 -f -q run pfs

#pintos -v -k --fs-disk=2 -p ../examples/file_syscall_tests -a fst -- -f -q run 'fst testing one two three'
#clear
