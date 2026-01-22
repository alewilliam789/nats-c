#include <string.h>

#include <messages.h>

struct Messages messages= {
  0,
  NULL,
  NULL
};

char* test_arr[3] = {"Hello", "all the", "World"};


void test_add_messages(void) {

  for(int i =0; i < 3; i++) {
    add_message(&messages, test_arr[i]);

    assert(messages.length == i + 1 && "This is the incorrect length for the message list");
  }

  struct Message* current_msg= messages.first_msg;

  int i = 0;
  while(current_msg != NULL) {
    assert(strcmp(current_msg->body,test_arr[i]) == 0 && "The correct string was not properly added");
    current_msg = current_msg->next_msg;
    i +=1;
  }
}

void test_digest_messages(void) {
  struct Message* current_msg = NULL;
  
  for (int i = 0; i < 3; i++) {
    current_msg = digest_message(&messages);

    assert(strcmp(current_msg->body, test_arr[i]) == 0 && "The wrong message was produced");
  }

  assert(messages.length == 0 && "This message list does not have the right length");
}



int main(void) {
  test_add_messages();
  test_digest_messages();
  return 0;
}
