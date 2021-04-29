#include <stddef.h>
#include "../threads/thread.h"

#include "flist.h"
//#include <stdio.h>

void map_init(struct map* object_pointer)
{
  object_pointer->first_entry_pointer = (struct file_list*) malloc(sizeof(struct file_list));
  object_pointer->last_entry_pointer = object_pointer->first_entry_pointer;
  object_pointer->elem_counter = 1;
  object_pointer->first_entry_pointer->key = 1;
  object_pointer->first_entry_pointer->value = NULL;
}


key_t map_insert(struct map* object_pointer, value_t val)
{

  struct file_list* temp_ptr;

  struct map* Temp_Map = &(thread_current()->File_Map);

  //Step 1 allocate memory
  temp_ptr = (struct file_list*) malloc(sizeof(struct file_list));
  if(temp_ptr == NULL)
  {
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

  return temp_ptr->key;

}


value_t map_find(struct map* object_pointer, key_t k)
{
  if(k > object_pointer->elem_counter ||  k < 2)
  {
    //PANIC("Key not found");
    //bad
    //printf("\n key not valid or found \n");
    return NULL; //NULL
  }
  //Find data
  struct file_list* temp_ptr = object_pointer->first_entry_pointer;
  for(int i = 1;i < k;i++)
  {
    temp_ptr = temp_ptr->next;
  }

  return temp_ptr->value;
}


value_t map_remove(struct map* object_pointer, key_t k)
{
  if(k > object_pointer->elem_counter)
  {
    //PANIC("Key not found");
    //bad
    return NULL; //NULL
  }

  struct file_list* temp_ptr = object_pointer->last_entry_pointer;
  struct file_list* temp_previous_ptr;
  struct file_list* temp_next_ptr;
  value_t temp_val;

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
    //printf("%s\n", "ERROR");
    return NULL;
  }

  //remove
  free(temp_ptr);

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
