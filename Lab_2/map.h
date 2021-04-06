/* do not forget the guard against multiple includes */
#ifndef MAP_H
#define MAP_H

#include <stdlib.h>

typedef char* value_t;
typedef int key_t;

#define PANIC(message) do { printf("PANIC: %s\n", message); exit(1); } while (0)

struct list
{
  key_t key;
  value_t value;
  struct list* previous;
  struct list* next;
};

struct map
{
  struct list* first_entry_pointer;
  struct list* last_entry_pointer;
  int elem_counter;
};


void map_deinit(struct map* object_pointer);

void map_init(struct map* object_pointer);

key_t map_insert(struct map* object_pointer, value_t val);

value_t map_find(struct map* object_pointer, key_t k);

value_t map_remove(struct map* object_pointer, key_t k); // Don't forget to deallocate memory





#endif
