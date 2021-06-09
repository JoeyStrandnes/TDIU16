#include <stdlib.h>
#include "pagedir.h"
#include "thread.h"

/* verfy_*_lenght are intended to be used in a system call that accept
 * parameters containing suspisious (user mode) adresses. The
 * operating system (executng the system call in kernel mode) must not
 * be fooled into using (reading or writing) addresses not available
 * to the user mode process performing the system call.
 *
 * In pagedir.h you can find some supporting functions that will help
 * you dermining if a logic address can be translated into a physical
 * addrerss using the process pagetable. A single translation is
 * costly. Work out a way to perform as few translations as
 * possible.
 *
 * Recommended compilation command:
 *
 *  gcc -Wall -Wextra -std=gnu99 -pedantic -m32 -g pagedir.o verify_adr.c
 */
//#error Read comment above and then remove this line.

/* Verify all addresses from and including 'start' up to but excluding
 * (start+length). */
bool verify_fix_length(void* start, int length)
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

/* Verify all addresses from and including 'start' up to and including
 * the address first containg a null-character ('\0'). (The way
 * C-strings are stored.)
 */
bool verify_variable_length(char* start)
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
      if(is_end_of_string(iterator + i))
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

/* Definition of test cases. */
struct test_case_t
{
  void* start;
  unsigned length;
};

#define TEST_CASE_COUNT 6

const struct test_case_t test_case[TEST_CASE_COUNT] =
{
  {(void*)100, 100}, /* one full page */
  {(void*)199, 102},
  {(void*)101, 98},
  {(void*)250, 190},
  {(void*)250, 200},
  {(void*)250, 210}
};

/* This main program will evalutate your solution. */
int main(int argc, char* argv[])
{
  int i;
  bool result;

  if ( argc == 2 )
  {
    simulator_set_pagefault_time( atoi(argv[1]) );
  }
  thread_init();
  
  /* Test the algorithm with a given intervall (a buffer). */
  for (i = 0; i < TEST_CASE_COUNT; ++i)
  {
    start_evaluate_algorithm(test_case[i].start, test_case[i].length);
    result = verify_fix_length(test_case[i].start, test_case[i].length);
    evaluate(result);
    end_evaluate_algorithm();
  }
  
  printf("#DEBUG: after fix length test\n");

  /* Test the algorithm with a C-string (start address with
   * terminating null-character).
   */
  for (i = 0; i < TEST_CASE_COUNT; ++i)
  {
    start_evaluate_algorithm(test_case[i].start, test_case[i].length);
    result = verify_variable_length(test_case[i].start);
    evaluate(result);    
    end_evaluate_algorithm();
  }
  return 0;
}
