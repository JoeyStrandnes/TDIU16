#include <stddef.h>
#include "../threads/thread.h"

#include "flist.h"
//#include <stdio.h>

void map_init(struct map* object_pointer)
{
  object_pointer->first_entry_pointer = (struct file_list*) malloc(sizeof(struct file_list));
  object_pointer->last_entry_pointer = object_pointer->first_entry_pointer;
  object_pointer->elem_counter = 1;
  object_pointer->ID_counter = 1;
  object_pointer->first_entry_pointer->key = 1;
  object_pointer->first_entry_pointer->value = NULL;
  lock_init(&object_pointer->map_lock);
}


key_t map_insert(struct map* object_pointer, value_t val)
{
  lock_acquire(&object_pointer->map_lock);
  struct file_list* temp_ptr;

  //Step 1 allocate memory
  //printf("Before malloc \n");
  temp_ptr = (struct file_list*) malloc(sizeof(struct file_list));
  if(temp_ptr == NULL)
  {
    lock_release(&object_pointer->map_lock);
    return -1;
  }

  //printf("after malloc \n temp_ptr: %u \n", temp_ptr);

  //Add to chain
  object_pointer->last_entry_pointer->next = temp_ptr;
  temp_ptr->previous = object_pointer->last_entry_pointer;
  object_pointer->last_entry_pointer = temp_ptr;


  //Increment elem_counter
  object_pointer->elem_counter++;
  object_pointer->ID_counter++;

  //Add data

  temp_ptr->key = object_pointer->ID_counter;
  temp_ptr->value = val;
  temp_ptr->next = NULL;

  lock_release(&object_pointer->map_lock);
  return temp_ptr->key;

}


value_t map_find(struct map* object_pointer, key_t k)
{
  lock_acquire(&object_pointer->map_lock);
  if(k > object_pointer->ID_counter ||  k < 2)
  {
    //PANIC("Key not found");
    //bad
    //printf("\n key not valid or found \n");
    //lock_release(&object_pointer->map_lock);
    return NULL; //NULL
  }
  //printf("### K: %d Elem Counter: %d\n", k, object_pointer->elem_counter);
  //Find data
  struct file_list* temp_ptr = object_pointer->first_entry_pointer;
  while(temp_ptr != NULL)
  {
    printf("temp_ptr->key: %d \n", temp_ptr->key);
    if(temp_ptr->key == k)
    {
      break;
    }

    printf("temp_ptr: %u Last entry pointer: %u \n", temp_ptr, object_pointer->last_entry_pointer);
    if(temp_ptr != object_pointer->last_entry_pointer && temp_ptr != NULL)
    {
      temp_ptr = temp_ptr->next;
    }
    else
    {
      temp_ptr = NULL;
    }
  }
  lock_release(&object_pointer->map_lock);
  
  if(temp_ptr != NULL)
  {
    return temp_ptr->value;
  }
  return NULL;
}


value_t map_remove(struct map* object_pointer, key_t k)
{
  lock_acquire(&object_pointer->map_lock);
  if(k > object_pointer->ID_counter)
  {
    //PANIC("Key not found");
    //bad
    lock_release(&object_pointer->map_lock);
    return NULL; //NULL
  }

  struct file_list* temp_ptr = object_pointer->last_entry_pointer;
  //printf("TEMP_PTR: %u \n", temp_ptr);
  struct file_list* temp_previous_ptr;
  struct file_list* temp_next_ptr;
  value_t temp_val;
  //printf("### BEFORE CRASH \n");
  //find element to removed
  while(temp_ptr->key != k && temp_ptr != object_pointer->last_entry_pointer && k > 1)
  {
    //printf("### Temp_ptr -> Key: %d K: %d \n", temp_ptr->key, k);
    temp_ptr = temp_ptr->previous;
  }

  //removal preparation
  temp_previous_ptr = temp_ptr->previous;
  temp_next_ptr = temp_ptr->next;
  temp_val = temp_ptr->value;
  object_pointer->elem_counter--;

  //printf("### elem counter: %d Key: %d \n", object_pointer->elem_counter, k);

  //patching up the chain
  if(k > 1 && object_pointer->elem_counter > 2)
  {
    temp_next_ptr->previous = temp_previous_ptr;
    temp_previous_ptr->next = temp_next_ptr;
  }
  else if(object_pointer->elem_counter == 2)
  {
    temp_previous_ptr->previous = object_pointer->first_entry_pointer;
    object_pointer->last_entry_pointer = object_pointer->first_entry_pointer;
    object_pointer->first_entry_pointer->next = NULL;
  }
  else if(temp_ptr->next == NULL)
  {
    temp_previous_ptr->next = NULL;
    object_pointer->last_entry_pointer = temp_previous_ptr;
  }
  else
  {
    //printf("%s\n", "ERROR");
    lock_release(&object_pointer->map_lock);
    return NULL;
  }

  //remove
  free(temp_ptr);
  lock_release(&object_pointer->map_lock);
  //return
  return temp_val;
}



void map_deinit(struct map* object_pointer)
{
  //printf("%s\n", "Destrutor >:(");

  struct file_list *temp_ptr  = object_pointer->last_entry_pointer;
  struct file_list *temp_previous_ptr;


  while(temp_ptr != object_pointer->first_entry_pointer)
  {
    //printf("%s\n", temp_ptr->value);


    temp_previous_ptr = temp_ptr->previous;
    //printf("%s\n", temp_previous_ptr->value);
    free(temp_ptr);
    temp_ptr = temp_previous_ptr;
  }

  free(object_pointer->first_entry_pointer);


}
