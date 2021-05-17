#include <stddef.h>

#include "plist.h"


void process_map_init(struct process_map* object_pointer)
{
  object_pointer->first_entry_pointer = (struct process_list*) malloc(sizeof(struct process_list));
  object_pointer->last_entry_pointer = object_pointer->first_entry_pointer;
  object_pointer->elem_counter = 1;
  object_pointer->first_entry_pointer->key = 1;
  object_pointer->first_entry_pointer->value = NULL;
  sema_init(&object_pointer->list_sema, 1);
}


key_t process_map_insert(struct process_map* object_pointer, value_t2 val)
{
  sema_down(&object_pointer->list_sema);
  struct process_list* temp_ptr;

  //struct process_map* Temp_process_map = &(thread_current()->File_process_map);

  //Step 1 allocate memory
  temp_ptr = (struct process_list*) malloc(sizeof(struct process_list));
  if(temp_ptr == NULL)
  {
    sema_up(&object_pointer->list_sema);
    return -1;
  }


  //Add to chain
  object_pointer->last_entry_pointer->next = temp_ptr;
  temp_ptr->previous = object_pointer->last_entry_pointer;
  object_pointer->last_entry_pointer = temp_ptr;


  //Increment elem_counter
  object_pointer->elem_counter++;

  //Add data

  temp_ptr->key = object_pointer->elem_counter;
  temp_ptr->value = val;
  temp_ptr->next = NULL;

  sema_up(&object_pointer->list_sema);
  return temp_ptr->key;

}


value_t2 process_map_find(struct process_map* object_pointer, key_t k)
{
  if(k > object_pointer->elem_counter ||  k < 2)
  {
    return NULL; //NULL
  }
  //Find data
  struct process_list* temp_ptr = object_pointer->first_entry_pointer;
  for(int i = 1;i < k;i++)
  {
    temp_ptr = temp_ptr->next;
  }

  return temp_ptr->value;
}


value_t2 process_map_remove(struct process_map* object_pointer, key_t k)
{
  sema_down(&object_pointer->list_sema);
  if(k > object_pointer->elem_counter)
  {
    sema_up(&object_pointer->list_sema);
    return NULL;
  }

  struct process_list* temp_ptr = object_pointer->last_entry_pointer;
  struct process_list* temp_previous_ptr;
  struct process_list* temp_next_ptr;
  value_t2 temp_val;

  //find element to removed
  while(temp_ptr->key != k && k <= object_pointer->elem_counter && k > 1)
  {
    temp_ptr->key--;
    temp_ptr = temp_ptr->previous;

  }

  //removal preparation
  temp_previous_ptr = temp_ptr->previous;
  temp_next_ptr = temp_ptr->next;
  temp_val = temp_ptr->value;
  object_pointer->elem_counter--;

  //patching up the chain
  if(k > 1 && object_pointer->elem_counter > 1)
  {
    temp_next_ptr->previous = temp_previous_ptr;
    temp_previous_ptr->next = temp_next_ptr;
  }
  else if(object_pointer->elem_counter == 1)
  {
    temp_previous_ptr->previous = object_pointer->first_entry_pointer;
    object_pointer->last_entry_pointer = object_pointer->first_entry_pointer;
  }
  else if(temp_ptr->next == NULL)
  {
    temp_previous_ptr->next = NULL;
    object_pointer->last_entry_pointer = temp_previous_ptr;
  }
  else
  {
    sema_up(&object_pointer->list_sema);
    return NULL;
  }

  sema_up(&object_pointer->list_sema);
  free(temp_ptr);
  return temp_val;
}



void process_map_deinit(struct process_map* object_pointer)
{
  struct process_list *temp_ptr  = object_pointer->last_entry_pointer;
  struct process_list *temp_previous_ptr;


  while(temp_ptr != object_pointer->first_entry_pointer)
  {
    temp_previous_ptr = temp_ptr->previous;
    free(temp_ptr);
    temp_ptr = temp_previous_ptr;
  }

  free(object_pointer->first_entry_pointer);
}
