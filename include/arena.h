#ifndef ARENA_H
#define ARENA_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define BASE_SIZE 256

struct Arena {
  uint32_t len;
  uint32_t size;
  void* memory_space;
};


u_int32_t arena_init(struct Arena *arena, size_t arena_size) {
  void* new_memory; 
  
  if (arena_size > 0 && arena_size*4 < BASE_SIZE) {
    new_memory = malloc(arena_size*4);
    arena->size = arena_size;
  }
  else {
    new_memory = malloc(BASE_SIZE);
    arena->size = BASE_SIZE;
  }

  assert(new_memory != NULL && "The memory was not allocated for the arena");

  arena->memory_space = new_memory;
  arena->len = 0;

  return 0;
}

void* allocate_memory(struct Arena *arena, uint32_t memory_size,  bool set_null) {
  
  assert(memory_size != 0);

  void* new_pointer = (char *)arena->memory_space + arena->len;

  uint32_t total_len = memory_size+arena->len;

  if (total_len > arena->size) {
    
    // Update size to make sure it's greater than 
    while (total_len > arena->size) {
      arena->size = arena->size*arena->size;
    }

    void* new_block = malloc(arena->size);

    assert(new_block != NULL);
    arena->memory_space = memcpy(new_block, arena->memory_space, arena->len);
  }

  if (set_null) {
    void* new_pointer = memset(new_pointer, NULL, memory_size);
  }

  arena->len += memory_size;

  return new_pointer;
}

#endif
