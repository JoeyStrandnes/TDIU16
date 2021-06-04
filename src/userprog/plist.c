#include <stddef.h>

#include "plist.h"
#include "threads/thread.h"


struct process_map Process_List;

void process_map_init(void)
{
  Process_List.first_entry_pointer = (struct process_list*) malloc(sizeof(struct process_list));
  Process_List.last_entry_pointer = Process_List.first_entry_pointer;
  Process_List.elem_counter = 1;
  Process_List.ID_counter = 1;
  Process_List.first_entry_pointer->is_active = false;
  Process_List.first_entry_pointer->key = 1;
  Process_List.first_entry_pointer->Process_ID = -1;
  Process_List.first_entry_pointer->Parent_ID = -1;
  lock_init(&Process_List.list_lock);
}


key_t process_map_insert(value_t2 ProcessID, value_t2 ParentID)
{
  lock_acquire(&Process_List.list_lock);
  struct process_list* temp_ptr;

  //rintf("Process ID: %d\n", ProcessID);
  //printf("Parent ID: %d\n", ParentID);

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
  Process_List.ID_counter++;

  //Add data

  temp_ptr->key = Process_List.ID_counter;
  temp_ptr->Process_ID = ProcessID;
  temp_ptr->Parent_ID = ParentID;
  temp_ptr->Exit_Status = -1;
  temp_ptr->is_active = true;
  temp_ptr->is_waited_upon = false;
  temp_ptr->next = NULL;

  sema_init(&temp_ptr->wait_lock,0);
  lock_init(&temp_ptr->synch_lock);
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
  struct process_list* temp_ptr = Process_List.first_entry_pointer->next;
  for(int i = 1;i < Process_List.elem_counter;i++)
  { 
    lock_acquire(&temp_ptr->synch_lock);
    if(ProcessID == temp_ptr->Process_ID){
      lock_release(&temp_ptr->synch_lock);
      return temp_ptr->key;
    }
    lock_release(&temp_ptr->synch_lock);
    temp_ptr = temp_ptr->next;
  }

  return -1;
}


value_t2 process_map_remove(key_t k)
{
  if(k < 2)
  {
    return -1;
  }

  lock_acquire(&Process_List.list_lock);

  struct process_list* temp_ptr = Process_List.last_entry_pointer;
  struct process_list* temp_previous_ptr;
  struct process_list* temp_next_ptr;
  value_t2 temp_val;

  //find element to removed
  while(temp_ptr->key != k && temp_ptr != Process_List.first_entry_pointer && k > 1)
  {
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
    Process_List.first_entry_pointer->next = NULL;
  }
  else if(temp_ptr->next == NULL)
  {
    temp_previous_ptr->next = NULL;
    Process_List.last_entry_pointer = temp_previous_ptr;
  }
  else
  {
    lock_release(&Process_List.list_lock);
    debug("PLIST ERROR \n");
    return -1;
  }

  free(temp_ptr);
  lock_release(&Process_List.list_lock);
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
  
  //printf("Lock Acquired********** \n");
  struct process_list* temp_ptr =  Process_List.first_entry_pointer->next;

  if(temp_ptr == NULL)
  {
    return;
  }

  //printf("Temp_ptr before loop: %u\n",temp_ptr);
  for(int i = 1; i < Process_List.elem_counter;i++)
  {
    //printf("Temp_ptr: %u\n",temp_ptr);
    
    if(temp_ptr != NULL && temp_ptr->Process_ID == process_id)
    {
      //printf("inside the if segment");
      lock_acquire(&temp_ptr->synch_lock);
      temp_ptr->Exit_Status = status;
      //debug("SET: temp_ptr->Process_ID: %d\n", temp_ptr->Process_ID);
      lock_release(&temp_ptr->synch_lock);
      break;
    }
    if(temp_ptr != Process_List.last_entry_pointer && temp_ptr != NULL )
    {
      temp_ptr = temp_ptr->next;
    }
    
  }
  
  //printf("Lock Releeasurur********** \n");
}


int process_map_get_exit_status(value_t2 process_id)
{
  struct process_list* temp_ptr = Process_List.first_entry_pointer->next;

  while(temp_ptr != NULL)
  {
    
    if(temp_ptr->Process_ID == process_id && temp_ptr != Process_List.first_entry_pointer)
    {
      lock_acquire(&temp_ptr->synch_lock);
      lock_release(&temp_ptr->synch_lock);
      //sema_down(&temp_ptr->wait_lock);
      //pdebug("Found exit status! \n");
      //debug("GET: temp_ptr->Process_ID: %d\n", temp_ptr->Process_ID);
      return temp_ptr->Exit_Status;
    }
    //lock_release(&temp_ptr->synch_lock);
    temp_ptr = temp_ptr->next;
  }
  //return (temp_ptr == NULL) ? -1:temp_ptr->Exit_Status;
  return -1;
}



void set_active_status(value_t2 process_id, bool status)
{
  //lock_acquire(&Process_List.list_lock);
  struct process_list* temp_ptr =  Process_List.first_entry_pointer->next;

  while(temp_ptr != NULL)
  { 
    if(temp_ptr->Process_ID == process_id && temp_ptr != Process_List.first_entry_pointer)
    {
      lock_acquire(&temp_ptr->synch_lock);
      temp_ptr->is_active = status;
      lock_release(&temp_ptr->synch_lock);
      break;
    }
    //lock_release(&temp_ptr->synch_lock);
    temp_ptr = temp_ptr->next;
  }

  //lock_release(&Process_List.list_lock); 
}



bool get_active_status(value_t2 process_id)
{
  //lock_acquire(&Process_List.list_lock);
  struct process_list* temp_ptr =  Process_List.first_entry_pointer->next;
  bool status = false;

  while(temp_ptr != NULL)
  {
    
    if(temp_ptr->Process_ID == process_id && temp_ptr != Process_List.first_entry_pointer)
    {
      lock_acquire(&temp_ptr->synch_lock);
      status = temp_ptr->is_active;
      lock_release(&temp_ptr->synch_lock);
      break;
    }
    //lock_release(&temp_ptr->synch_lock);
    temp_ptr = temp_ptr->next;
  }
  //lock_release(&Process_List.list_lock);
  return status;
}



int get_wait_status(value_t2 process_id)
{
  //lock_acquire(&Process_List.list_lock);
  struct process_list* temp_ptr =  Process_List.first_entry_pointer->next;
  int status = -1;

  while(temp_ptr != NULL)
  {
    
    if(temp_ptr->Process_ID == process_id)
    { 
      lock_acquire(&temp_ptr->synch_lock);
      if(temp_ptr->is_waited_upon)
      {
        status = 1; //yes
      }
      else
      {
        status = 0; //no
      }
      lock_release(&temp_ptr->synch_lock);
      break;
    }
    //lock_release(&temp_ptr->synch_lock);
    temp_ptr = temp_ptr->next;
  }
  //lock_release(&Process_List.list_lock);
  return status;
}



int use_wait_lock(value_t2 process_id)
{
  //lock_acquire(&Process_List.list_lock);
  struct process_list* temp_ptr =  Process_List.first_entry_pointer->next;
  int status = -1;
  while(temp_ptr != NULL)
  {
    //
    if(temp_ptr->Process_ID == process_id)
    {
      lock_acquire(&temp_ptr->synch_lock);
      temp_ptr->is_waited_upon = true;
      lock_release(&temp_ptr->synch_lock);
      sema_down(&temp_ptr->wait_lock);
      //printf("#Debug wait lock. Current thread: %s", thread_tid());
      return 1;
    }
    //lock_release(&temp_ptr->synch_lock);
    temp_ptr = temp_ptr->next;
  }
  //lock_release(&Process_List.list_lock);
  return -1;
}



bool is_child_process(value_t2 child_id)
{
  //lock_acquire(&Process_List.list_lock);
  struct process_list* temp_ptr =  Process_List.first_entry_pointer->next;
  bool status = false;
  while(temp_ptr != NULL)
  {
    
    if(temp_ptr->Process_ID == child_id && temp_ptr->Parent_ID == thread_tid())
    {
      lock_acquire(&temp_ptr->synch_lock);
      status = true;
      lock_release(&temp_ptr->synch_lock);
      break;
    }
    //lock_release(&temp_ptr->synch_lock);
    temp_ptr = temp_ptr->next;
  }
  //lock_release(&Process_List.list_lock);
  return status;
}



int unlock_wait_lock(value_t2 process_id)
{
    //lock_acquire(&Process_List.list_lock);
  struct process_list* temp_ptr =  Process_List.first_entry_pointer->next;
  int status = -1;
  while(temp_ptr != NULL)
  {
    
    if(temp_ptr->Process_ID == process_id)
    {
      //debug("Unlock wait lock \n");
      lock_acquire(&temp_ptr->synch_lock);
      temp_ptr->is_waited_upon = false;
      lock_release(&temp_ptr->synch_lock);
      sema_up(&temp_ptr->wait_lock);
      //printf("### Debug wait lock. Current thread: %s", thread_tid());
      return 1;
    }
    //lock_release(&temp_ptr->synch_lock);
    temp_ptr = temp_ptr->next;
  }
  //lock_release(&Process_List.list_lock);
  return -1;
}