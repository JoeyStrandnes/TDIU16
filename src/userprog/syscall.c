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

//#include "threads/init.h"





static void syscall_handler (struct intr_frame *);
static bool verify_variable_length(char* start);
static bool verify_fix_length(void* start, int length);
static void verify_esp(int32_t* esp);



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
  //printf("System Call Halt\n");
  power_off();
}

static void sys_exit(int status)
{
  //printf("System Call Exit: thread: %s#%d\n", thread_name(), thread_tid());
  //printf("Status: %d\n", status);
  process_exit(status);
  thread_exit();
}


static int sys_write(int fd, char *buffer, unsigned size)
{

  if(!verify_fix_length(buffer, size))
  {
    sys_exit(-1);
  }

  if(!verify_variable_length(buffer))
  {
    sys_exit(-1);
  }

  if(fd == STDOUT_FILENO) // Till skärmen
  {
    putbuf(buffer, size); //putbuf(const char *buffer, size_t n);
    return size;
  }
  else if (fd == STDIN_FILENO) //Från tangentbordet
  {
    return -1;
  }

  if(fd > STDOUT_FILENO)
  {
    struct file* file_ptr = map_find(&(thread_current()->File_Map), fd);
    if(file_ptr == NULL)
    {
      debug("file %d not found in map\n", fd);

      struct map * temp_map = &thread_current()->File_Map;
      struct file_list * temp_ptr = temp_map->first_entry_pointer;
      debug("Elements in map: %d\n", temp_map->elem_counter);

      while(temp_ptr != NULL)
      {
        debug("File Descriptor: %d\n", temp_ptr->key);
        temp_ptr = temp_ptr->next;
      }


      return -1;
    }
    //debug("WRITE: File_Ptr: %u\n", file_ptr);
    int bytes_written = file_write(file_ptr, buffer, size);
    //debug("thread: %s #%d STRING: %s \n", thread_name(), thread_tid(), buffer);
    //debug("System Call Write\n");
    return bytes_written;
  }

  return -1;
}

static int sys_read(int fd, char *buffer, unsigned size)
{

  //debug("System Call read: %d\n", fd);

  if(!verify_fix_length(buffer, size))
  {
    sys_exit(-1);
  }

  if(!verify_variable_length(buffer))
  {
    sys_exit(-1);
  }

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
      putchar(*buffer++);

    }

    return counter;

  }//STDOUT_FILENO - END
    if(fd > STDOUT_FILENO)
  {
    struct file* file_ptr = map_find(&(thread_current()->File_Map), fd);
    if(file_ptr == NULL)
    {
      return -1;
    }
    //debug("READ: File_Ptr: %u\n", file_ptr);
    //debug("System Call Read\n");
    return file_read(file_ptr, buffer, size);

  }

  return -1; //Catch error
}

static int sys_open(const char *file)
{

  if(!verify_variable_length((char*)file))
  {
    sys_exit(-1);
  }

  struct file* file_ptr = filesys_open(file);

  if(file_ptr == NULL){
      return -1;
  }

  //debug("OPEN: File_Ptr: %u\n", file_ptr);

  int temp =  map_insert(&(thread_current()->File_Map), file_ptr);

  //printf("Map insert status %d\n", temp);
  return temp;

}

static bool sys_create(const char *file, unsigned initial_size)
{
  
  if(!verify_variable_length((char*)file))
  {
    sys_exit(-1);
  }

  return filesys_create(file, initial_size);
}

static void sys_close(int fd)
{

  struct file* file_ptr = map_find(&(thread_current()->File_Map), fd);


  if(file_ptr != NULL)
  {
    file_close(file_ptr);
    map_remove(&(thread_current()->File_Map), fd);
  }


}

static bool sys_remove(const char *file)
{

  if(!verify_variable_length((char*)file))
  {
    sys_exit(-1);
  }

  return filesys_remove(file);

}


static int sys_tell(int fd)
{

  struct file* file_ptr = map_find(&(thread_current()->File_Map), fd);

  if(file_ptr != NULL)
  {

    return file_tell(file_ptr);

  }

  return -1;

}

static void sys_seek(int fd, unsigned pos)
{

  struct file* file_ptr = map_find(&(thread_current()->File_Map), fd);


  if(file_ptr != NULL)
  {
    file_seek(file_ptr, pos);
  }

}

static int sys_filesize(int fd)
{

  struct file* file_ptr = map_find(&(thread_current()->File_Map), fd);


  if(file_ptr != NULL)
  {
    return file_length(file_ptr);
  }
  return -1;

}

void sys_sleep(int millis)
{
  timer_msleep(millis);
}

tid_t sys_exec(const char* command_line)
{
  if(!verify_variable_length((char*)command_line))
  {
    return -1;
  }
  return process_execute(command_line);
}

void sys_plist(void)
{
  process_print_list();
}

int sys_wait(tid_t pid)
{
  return process_wait(pid);
}



static void syscall_handler (struct intr_frame *f)
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

  verify_esp(esp);


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
    case SYS_OPEN:
    {
      f->eax = sys_open((const char*)esp[1]);
      break;
    }
    case SYS_CLOSE:
    {
      sys_close(esp[1]);
      break;
    }
    case SYS_REMOVE:
    {
      f->eax = sys_remove((const char *)esp[1]);
      break;
    }
    case SYS_CREATE:
    {
      f->eax = sys_create((const char*)(esp[1]), (unsigned)(esp[2]));
      break;
    }
    case SYS_SEEK:
    {
      sys_seek(esp[1], (unsigned)esp[2]);
      break;
    }
    case SYS_TELL:
    {
      f->eax = sys_tell(esp[1]);
      break;
    }
    case SYS_FILESIZE:
    {
      f->eax = sys_filesize(esp[1]);
      break;
    }
    case SYS_EXEC:
    {
      f->eax = sys_exec((char*)esp[1]);
      break;
    }
    case SYS_PLIST:
    {
      sys_plist();
      break;
    }
    case SYS_SLEEP:
    {
      sys_sleep(esp[1]);
      break;
    }
    case SYS_WAIT:
    {
      f->eax = sys_wait(esp[1]);
      break;
    }


    default:
    {
      printf("Executed an unknown system call!\n");

      printf("Stack top + 0: %d\n", esp[0]);
      printf("Stack top + 1: %d\n", esp[1]);

      thread_exit ();
    }
  }
}



static bool verify_fix_length(void* start, int length)
{
  // ADD YOUR CODE HERE

  //printf("#DEBUG: start: %u length: %d\n", start, length);

  void * iterator = pg_round_down(start);
  //printf("#DEBUG: iterator: %u\n", iterator);
  void * end = (void*)((unsigned) start + length);
  //printf("#DEBUG: end: %u\n", end);

  while(iterator < end)
  {
    //printf("#DEBUG: iterator inside loop: %u\n", iterator);
    if(pagedir_get_page(thread_current()->pagedir, iterator) == NULL)
    {
      return false;
    }

    iterator = (void*) ((unsigned)iterator + PGSIZE);
  }
  return true;
}


static bool verify_variable_length(char* start)
{
  // ADD YOUR CODE HERE

  char* iterator = start;
  int chars_to_read; 

  //printf("#DEBUG: iterator: %u \n", start);

  while(pagedir_get_page(thread_current()->pagedir, iterator) != NULL)
  {
    //printf("#DEBUG: iterator in loop: %u \n", iterator);
    chars_to_read = PGSIZE - ((int)iterator % PGSIZE);
    //printf("#DEBUG: chars_to_read: %u \n", chars_to_read);
    for(int i = 0;i < chars_to_read;i++)
    {
      //printf("#DEBUG: iterator + i: %u \n", iterator + i);
      if(*(iterator + i) == '\0')
      {
        return true;
      }
    }
    iterator = (char*) iterator + chars_to_read;
    

    /*
    if(is_end_of_string(iterator))
    {
      return true;
    }
    
    iterator++;
    */
  }
  return false;
}



static void verify_esp(int32_t* esp)
{
  if(esp[0] >= SYS_NUMBER_OF_CALLS)
  {
    sys_exit(-1); //the stack pointer is not correct
  }
}