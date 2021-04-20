#include <stdio.h>
#include <syscall-nr.h>
#include "userprog/syscall.h"
#include "threads/interrupt.h"
#include "threads/thread.h"

/* header files you probably need, they are not used yet */
#include <string.h>
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/vaddr.h"
#include "threads/init.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "devices/input.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


/* This array defined the number of arguments each syscall expects.
   For example, if you want to find out the number of arguments for
   the read system call you shall write:

   int sys_read_arg_count = argc[ SYS_READ ];

   All system calls have a name such as SYS_READ defined as an enum
   type, see `lib/syscall-nr.h'. Use them instead of numbers.
 */
const int argc[] = {
  /* basic calls */
  0, 1, 1, 1, 2, 1, 1, 1, 3, 3, 2, 1, 1,
  /* not implemented */
  2, 1,    1, 1, 2, 1, 1,
  /* extended */
  0
};




static void sys_halt(void)
{
  printf("System Call Halt\n");
  power_off();
}

static void sys_exit(int status)
{
    printf("System Call Exit\n");
    printf("Status: %d\n", status);
    thread_exit();
}


static int sys_write(int fd, char *buffer, unsigned size){

  printf("System Call Write\n");

  if(fd == STDOUT_FILENO) // Till skärmen
  {
    putbuf(buffer, size); //putbuf(const char *buffer, size_t n);
    return 1;
  }
  else if (fd == STDIN_FILENO) //Från tangentbordet
  {
    return -1;
  }

  return -1;
}


static int sys_read(int fd, char *buffer, unsigned size){

  //printf("System Call Read\n");

  if(fd == STDOUT_FILENO) // Till skärmen
  {
    return -1;
  } // STDOUT_FILENO - END

  else if (fd == STDIN_FILENO) //Från tangentbordet
  {
    unsigned counter = 0;
    while(size != 0 && counter < size)
    {
      *buffer = input_getc(); //Get char

      //Input massaging
      if(*buffer == '\r') //Check for carraige-return conversion
      {
        *buffer = '\n';
      }

      else if((*buffer == 127) && (counter > 0)) //Backspace
      {
        *(--buffer) = 0;
        putbuf("\b \b", 3);
      }

      counter++;
      putchar(*buffer);

    }

    return counter;

  }//STDOUT_FILENO - END

  return -1; //Catch error
}

static void
syscall_handler (struct intr_frame *f)
{
  int32_t* esp = (int32_t*)f->esp;
/*
  printf ("Stack top + 0: %d\n", esp[0]);
  printf ("Stack top + 1: %d\n", esp[1]);
  printf ("Stack top + 2: %d\n", esp[2]);
  printf ("Stack top + 3: %d\n", esp[3]);
  printf ("Stack top + 4: %d\n", esp[4]);
  printf ("Stack top + 5: %d\n", esp[5]);
*/

  switch ( esp[0] /* retrive syscall number */ )
  {
    case SYS_HALT:
    {
      sys_halt();
      break;
    }
    case SYS_EXIT:
    {
      sys_exit(esp[1]);
      break;
    }
    case SYS_WRITE:
    {
      f->eax = sys_write(esp[1],(char*)esp[2],esp[3]);
      break;
    }
    case SYS_READ:
    {
      f->eax = sys_read(esp[1],(char*)esp[2],esp[3]);
      break;
    }

    default:
    {
      printf ("Executed an unknown system call!\n");

      printf ("Stack top + 0: %d\n", esp[0]);
      printf ("Stack top + 1: %d\n", esp[1]);

      thread_exit ();
    }
  }
}
