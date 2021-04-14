/* halt.c

   Simple program to test whether running a user program works.

   Just invokes a system call that shuts down the OS. */

#include <syscall.h>
#include <stdio.h>
//#include "../userprog/syscall.h"

int
main (void)
{
  //printf("Inside Halt program");
  exit(EXIT_SUCCESS);
  //halt();
  /* not reached */
}
