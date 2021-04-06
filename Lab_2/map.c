#include "map.h"
#include <stdio.h>

void map_init(struct map* object_pointer)
{
  object_pointer->first_entry_pointer = (struct list*) malloc(sizeof(struct list));
  object_pointer->last_entry_pointer = object_pointer->first_entry_pointer;
  object_pointer->elem_counter = 0;
  object_pointer->first_entry_pointer->key = 0;
  object_pointer->first_entry_pointer->value = '\0';
}


key_t map_insert(struct map* object_pointer, value_t val)
{
  struct list* temp_ptr;

  //Step 1 allocate memory
  temp_ptr = (struct list*) malloc(sizeof(struct list));
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
  if(k > object_pointer->elem_counter ||  k < 1)
  {
    //PANIC("Key not found");
    //bad
    printf("\n key not valid or found \n");
    return '\0'; //NULL
  }
  //Find data
  struct list* temp_ptr = object_pointer->first_entry_pointer;
  for(int i = 1;i <= k;i++)
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
    return '\0'; //NULL
  }

  struct list* temp_ptr = object_pointer->last_entry_pointer;
  struct list* temp_previous_ptr;
  struct list* temp_next_ptr;
  value_t temp_val;

  //find element to removed
  while(temp_ptr->key != k && k <= object_pointer->elem_counter && k > 0)
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
  if(k > 0 && object_pointer->elem_counter > 0)
  {
    temp_next_ptr->previous = temp_previous_ptr;
    temp_previous_ptr->next = temp_next_ptr;
  }
  else if(temp_ptr->next == NULL)
  {
    temp_previous_ptr->next = NULL;
  }
  else if(object_pointer->elem_counter == 1)
  {
    temp_previous_ptr->previous = object_pointer->first_entry_pointer;
  }
  else
  {
    printf("%s\n", "ERROR");
    return '\0';
  }

  //remove
  free(temp_ptr);

  //return
  return temp_val;
}



void map_deinit(struct map* object_pointer)
{
  printf("%s\n", "Destrutor >:(");

  struct list *temp_ptr  = object_pointer->last_entry_pointer;
  struct list *temp_previous_ptr;


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
