#include <arena.h>
#include <messages.h>

#include <assert.h>



void test_initialize_arena(struct Arena* arena) {

 // Base case where base size is initialized
 arena_init(arena, 0);
 assert(arena->size == BASE_SIZE && "There was an issue with the base allocation");
 

 // Only initializing a set amount under base size
 arena_init(arena, 3*sizeof(struct Messages));
 assert(arena->size > 3*sizeof(struct Messages) && "There was an issue with the allocation of a set amount of data");

 assert(arena->len == 0);
}

void test_allocate_memory_arena(struct Arena* arena) {
  
  char* words[3] = {"Hello", "all the", "world"};
  struct Message* first_msg;
  struct Message* previous_msg;

  for(int i = 0; i < 3; i++) {
    struct Message* msg = allocate_memory(arena, sizeof(struct Message));
    msg->body = words[i];

    
    if ( i == 0 || i == 2){
      msg->next_msg = NULL;
      if (i == 0) {
        first_msg = msg;
      }
    }
    else {
      previous_msg->next_msg = msg;
    } 

    previous_msg = msg;
  }
  
  struct Message* current_msg = first_msg;
  int i = 0;
  while(current_msg != NULL) {
    assert(current_msg->body == words[i] && "There was an issue with the message allocation");
    current_msg = current_msg->next_msg;
    i += 1;
  }
}

void test_resize_arena(struct Arena* arena) {
  
  // Allocate an arena for only 3 messages
  arena_init(arena, 3*sizeof(struct Message));

  char* words[3] = {"Hello", "all the", "world"};
  struct Message* first_msg;
  struct Message* previous_msg;

  for(int i = 0; i < 3; i++) {
    struct Message* msg = allocate_memory(arena, sizeof(struct Message));
    msg->body = words[i];

    
    if ( i == 0 || i == 2){
      msg->next_msg = NULL;
      if (i == 0) {
        first_msg = msg;
      }
    }
    else {
      previous_msg->next_msg = msg;
    } 

    previous_msg = msg;
  }
  
  // Now try to allocate a chunk larger than that to resize arena
  allocate_memory(arena, 50*sizeof(struct Message));

  // Verify that you can still access each message
  struct Message* current_msg = first_msg;
  int i = 0;
  while(current_msg != NULL) {
    assert(current_msg->body == words[i] && "There was an issue with the message allocation");
    current_msg = current_msg->next_msg;
    i += 1;
  }

}


int main(void) {
  struct Arena test_arena = {
    0,
    0,
    NULL
  };

  test_initialize_arena(&test_arena);

  arena_init(&test_arena, 0);
  test_allocate_memory_arena(&test_arena);
  test_resize_arena(&test_arena);

  return 0;

}
