#ifndef _PLIST_H_
#define _PLIST_H_


/* Place functions to handle a running process here (process list).

   plist.h : Your function declarations and documentation.
   plist.c : Your implementation.

   The following is strongly recommended:

   - A function that given process inforamtion (up to you to create)
     inserts this in a list of running processes and return an integer
     that can be used to find the information later on.

   - A function that given an integer (obtained from above function)
     FIND the process information in the list. Should return some
     failure code if no process matching the integer is in the list.
     Or, optionally, several functions to access any information of a
     particular process that you currently need.

   - A function that given an integer REMOVE the process information
     from the list. Should only remove the information when no process
     or thread need it anymore, but must guarantee it is always
     removed EVENTUALLY.

   - A function that print the entire content of the list in a nice,
     clean, readable format.

 */


#include <stdlib.h>
#include "threads/synch.h"

typedef int value_t2;
typedef int key_t;


 struct process_list
 {
   key_t key;
   value_t2 Process_ID;
   value_t2 Parent_ID;
   value_t2 Exit_Status;
   bool is_active;
   bool is_waited_upon;
   struct semaphore wait_lock;
   struct process_list* previous;
   struct process_list* next;
 };


 struct process_map
 {
   struct process_list* first_entry_pointer;
   struct process_list* last_entry_pointer;
   struct lock list_lock;
   int elem_counter;
 };


void process_map_deinit(void);

void process_map_init(void);

key_t process_map_insert(value_t2 ProcessID, value_t2 ParentID);

key_t process_map_find(value_t2 ProcessID);

value_t2 process_map_remove(key_t k); // Don't forget to deallocate memory :)

void process_map_set_exit_status(value_t2 process_id, int status);

int process_map_get_exit_status(value_t2 process_id);

void set_active_status(value_t2 process_id, bool status);

bool get_active_status(value_t2 process_id);

int get_wait_status(value_t2 process_id);

int use_wait_lock(value_t2 process_id);

bool is_child_process(value_t2 child_id);

#endif
