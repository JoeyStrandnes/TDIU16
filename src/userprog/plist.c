#include <stddef.h>

#include "plist.h"


struct process_map Process_List;

void process_map_init(void)
{
  Process_List.first_entry_pointer = (struct process_list*) malloc(sizeof(struct process_list));
  Process_List.last_entry_pointer = Process_List.first_entry_pointer;
  Process_List.elem_counter = 1;
  Process_List.first_entry_pointer->key = 1;
  Process_List.first_entry_pointer->Process_ID = 0;
  lock_init(&Process_List.list_lock);
}


key_t process_map_insert(value_t2 ProcessID, value_t2 ParentID)
{
  lock_acquire(&Process_List.list_lock);
  struct process_list* temp_ptr;

  printf("Process ID: %d\n", ProcessID);
  printf("Parent ID: %d\n", ParentID);

  //struct process_map* Temp_process_map = &(thread_current()->File_process_map);

  //Step 1 allocate memory
  temp_ptr = (struct process_list*) malloc(sizeof(struct process_list));
  if(temp_ptr == NULL)
  {
    lock_release(&Process_List.list_lock);
    return -1;
  }


  //Add to chain
  Process_List.last_entry_pointer->next = temp_ptr;
  temp_ptr->previous = Process_List.last_entry_pointer;
  Process_List.last_entry_pointer = temp_ptr;


  //Increment elem_counter
  Process_List.elem_counter++;

  //Add data

  temp_ptr->key = Process_List.elem_counter;
  temp_ptr->Process_ID = ProcessID;
  temp_ptr->Parent_ID = ParentID;
  temp_ptr->Exit_Status = -1;
  temp_ptr->next = NULL;

  lock_release(&Process_List.list_lock);
  return temp_ptr->key;

}


key_t process_map_find(value_t2 ProcessID)
{

  if(Process_List.elem_counter < 2)
  {
    return -1;
  }


  //Find data
  struct process_list* temp_ptr = Process_List.first_entry_pointer;
  for(int i = 1;i < Process_List.elem_counter;i++)
  {
    if(ProcessID == temp_ptr->Process_ID){
      break;
    }
    temp_ptr = temp_ptr->next;
  }

  return temp_ptr->key;
}


value_t2 process_map_remove(key_t k)
{
  lock_acquire(&Process_List.list_lock);
  if(k > Process_List.elem_counter)
  {
    lock_release(&Process_List.list_lock);
    return -1;
  }

  struct process_list* temp_ptr = Process_List.last_entry_pointer;
  struct process_list* temp_previous_ptr;
  struct process_list* temp_next_ptr;
  value_t2 temp_val;

  //find element to removed
  while(temp_ptr->key != k && k <= Process_List.elem_counter && k > 1)
  {
    temp_ptr->key--;
    temp_ptr = temp_ptr->previous;

  }

  //removal preparation
  temp_previous_ptr = temp_ptr->previous;
  temp_next_ptr = temp_ptr->next;
  temp_val = temp_ptr->Process_ID;
  Process_List.elem_counter--;

  //patching up the chain
  if(k > 1 && Process_List.elem_counter > 1)
  {
    temp_next_ptr->previous = temp_previous_ptr;
    temp_previous_ptr->next = temp_next_ptr;
  }
  else if(Process_List.elem_counter == 1)
  {
    temp_previous_ptr->previous = Process_List.first_entry_pointer;
    Process_List.last_entry_pointer = Process_List.first_entry_pointer;
  }
  else if(temp_ptr->next == NULL)
  {
    temp_previous_ptr->next = NULL;
    Process_List.last_entry_pointer = temp_previous_ptr;
  }
  else
  {
    lock_release(&Process_List.list_lock);
    return -1;
  }

  lock_release(&Process_List.list_lock);
  free(temp_ptr);
  return temp_val;
}



void process_map_deinit(void)
{
  struct process_list *temp_ptr  = Process_List.last_entry_pointer;
  struct process_list *temp_previous_ptr;


  while(temp_ptr != Process_List.first_entry_pointer)
  {
    temp_previous_ptr = temp_ptr->previous;
    free(temp_ptr);
    temp_ptr = temp_previous_ptr;
  }

  free(Process_List.first_entry_pointer);
}



void process_map_set_exit_status(value_t2 process_id, int status)
{
  lock_acquire(&Process_List.list_lock);
  //printf("Lock Acquired********** \n");
  struct process_list* temp_ptr =  Process_List.first_entry_pointer->next;

  //printf("Temp_ptr before loop: %u\n",temp_ptr);
  for(int i = 0; i < Process_List.elem_counter;i++)
  {
    //printf("Temp_ptr: %u\n",temp_ptr);
    
    if(temp_ptr != NULL && temp_ptr->Process_ID == process_id &&  temp_ptr != Process_List.first_entry_pointer)
    {
      //printf("inside the if segment");
      temp_ptr->Exit_Status = status;
      break;
    }
    
    if(temp_ptr != Process_List.last_entry_pointer && temp_ptr != NULL )
    {
      temp_ptr = temp_ptr->next;
    }
    
  }
  //printf("Lock Releeasurur********** \n");
  lock_release(&Process_List.list_lock);
}


int process_map_get_exit_status(value_t2 process_id)
{
  lock_acquire(&Process_List.list_lock);
  struct process_list* temp_ptr =  Process_List.first_entry_pointer;

  while(temp_ptr != NULL)
  {
    if(temp_ptr->Process_ID == process_id && temp_ptr != Process_List.first_entry_pointer)
    {
      break;
    }
    temp_ptr = temp_ptr->next;
  }

  lock_release(&Process_List.list_lock);
  return (temp_ptr == NULL) ? -1:temp_ptr->Exit_Status;
}
