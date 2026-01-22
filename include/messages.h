#ifndef MESSAGES_H
#define MESSAGES_H

#include <assert.h>
#include <stdlib.h>

enum STORAGE_STATE {
  PERSISTENT,
  LOCAL
};

struct Message {
  struct Message* next_msg;
  char* body;
};

struct Messages {
  int length;
  struct Message* first_msg;
  struct Message* last_msg;
};


int add_message(struct Messages* messages, char* message) {
  assert(messages != NULL && "The messages list should not be null");

  struct Message* new_msg;

  new_msg = malloc(sizeof(struct Message));

  assert(new_msg != NULL && "The memory for this message was not properly allocated");

  new_msg->body = message;
  new_msg->next_msg = NULL;
  
  if (messages->length == 0) {
    messages->first_msg =  new_msg;
    messages->last_msg = messages->first_msg;
  }
  else {
    messages->last_msg->next_msg = new_msg;
    messages->last_msg = new_msg;
  }

  messages->length += 1;

  return 0;
}

struct Message* digest_message(struct Messages* messages){
  assert(messages != NULL && "The messages list should not be null");
  
  struct Message* msg = messages->first_msg;

  if (msg == NULL) {
    return NULL;
  }

  if (messages->length == 1) {
    messages->first_msg = NULL;
    messages->last_msg = NULL;
  }
  else {
    messages->first_msg = msg->next_msg;
  }

  messages->length -= 1;
  return msg;
}

int free_messages(struct Messages* message_list) {
  struct Message* current_msg = message_list->first_msg;
  struct Message* next_msg = NULL;

  while(current_msg != NULL) {
    next_msg = current_msg->next_msg;
    free(current_msg);
    current_msg = next_msg;
    
  }

  return 0;
}


#endif // MESSAGES_H
