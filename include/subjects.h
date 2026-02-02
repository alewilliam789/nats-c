#ifndef SUBJECTS_H
#define SUBJECTS_H

#include "arena.h"
#include <string.h>
#define MAP_SIZE 20
#define LOAD_FACTOR 3/4

#include <messages.h>
#include <stdint.h>
#include <stdbool.h>


struct Subject {
  struct Messages* messages;
  struct SubjectHashMap* subjects;
  char* sub_name;
  uint32_t sub_len;
};


struct SubjectHashMap {
  uint32_t size;
  uint32_t length;
  uint32_t seed;
  struct Subject** map;
};

// Provided implementation of MurmurHash
uint32_t murmur3_32(const char *key, uint32_t len, uint32_t seed, uint32_t table_size) {
    static const uint32_t c1 = 0xcc9e2d51;
    static const uint32_t c2 = 0x1b873593;
    static const uint32_t r1 = 15;
    static const uint32_t r2 = 13;
    static const uint32_t m = 5;
    static const uint32_t n = 0xe6546b64;

    uint32_t hash = seed;

    const int nblocks = len / 4;
    const uint32_t *blocks = (const uint32_t *) key;
    int i;
    for (i = 0; i < nblocks; i++) {
        uint32_t k = blocks[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
    }

    const uint8_t *tail = (const uint8_t *) (key + nblocks * 4);
    uint32_t k1 = 0;

    switch (len & 3) {
    case 3:
        k1 ^= tail[2] << 16;
    case 2:
        k1 ^= tail[1] << 8;
    case 1:
        k1 ^= tail[0];

        k1 *= c1;
        k1 = (k1 << r1) | (k1 >> (32 - r1));
        k1 *= c2;
        hash ^= k1;
    }

    hash ^= len;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash % table_size;
}

 struct SubjectHashMap* hashmap_init(struct Arena* arena, uint32_t seed, uint32_t map_size) {
    struct SubjectHashMap* new_hashmap = allocate_memory(arena, sizeof(struct SubjectHashMap), false);
    
    if (map_size > 0) {
      new_hashmap->map = allocate_memory(arena, map_size*sizeof(struct Subject), true);
    }
    else {
      new_hashmap->map = allocate_memory(arena, MAP_SIZE*sizeof(struct Subject), true);
    }
    new_hashmap->size = MAP_SIZE;
    new_hashmap->length = 0;
    new_hashmap->seed  = seed;

    return new_hashmap;
 }

uint32_t hashmap_resize(struct Arena* arena, struct SubjectHashMap* hmap) {
  uint32_t new_size = hmap->size*2*sizeof(struct Subject);

  struct Subject** new_map = allocate_memory(arena, new_size, true);

  assert(new_map != NULL && "There was an issue allocating the memory for the new map");

  hmap->map = memcpy(hmap->map, new_map, hmap->size);

  hmap->size = hmap->size*2;

  return 0;
}

struct Subject* get_subject(struct SubjectHashMap* hmap, char* sub_name, uint32_t sub_len, uint32_t seed) {
  uint32_t hash = murmur3_32(sub_name, sub_len, seed, hmap->size);

  assert(hash < hmap->size && "The hash has exceeded the length of the map");
  struct Subject* new_subject = hmap->map[hash];
  
  if (strcmp(new_subject->sub_name, new_subject->sub_name) != 0) {
    uint32_t i = hash;

    while(true) {
      if (i+1 == hmap->size) {
        i = 0;
      }
      else {
        i += 1;
      }

      struct Subject* pot_sub = hmap->map[i];

      if (pot_sub != NULL && strcmp(pot_sub->sub_name, sub_name) == 0) {
        return pot_sub;
      }
    }
  }

  return new_subject;
}

uint32_t set_subject(struct Arena* arena, struct SubjectHashMap* hmap, struct Subject* new_subject, uint32_t seed) {
  uint32_t hash = murmur3_32(new_subject->sub_name, new_subject->sub_len, seed, hmap->size);

  assert(hash < hmap->size && "The hash has exceeded the length of the map");

  if ( hmap->length + 1 == hmap->size*LOAD_FACTOR) {
    hashmap_resize(arena, hmap);
  }

  struct Subject* subject = hmap->map[hash];

  if (subject == NULL) {
    hmap->map[hash] = new_subject;
  }
  else {
    assert(strcmp(new_subject->sub_name, subject->sub_name) == 0 && "An existing subject can not be replaced, but can add messages");
    
    // If collision occurs use linear probing
    if (strcmp(new_subject->sub_name, subject->sub_name) != 0) {
      uint32_t i = hash;
      
      while(true) {
        if (i+1 == hmap->size) {
          i = 0;
        }
        else {
          i += 1;
        }

        struct Subject* pot_spot = hmap->map[i];

        if (pot_spot == NULL) {
          hmap->map[i] = new_subject;
          break;
        }
      }
    }
  }

  return 0;
}


uint32_t process_subject(struct Arena* arena, struct SubjectHashMap* base_map, uint32_t seed, char* sub_name, uint32_t sub_len, char* message) {
  
  assert(message != NULL && "You can't pass an empty message to be stored in a stream");

  struct Messages* new_messages = allocate_memory(arena, sizeof(struct Messages), false);
 
  assert(new_messages != NULL && "The memory was not properly allocated for this struct");

  new_messages->length = 0;
  new_messages->first_msg = NULL;
  new_messages->last_msg = NULL;

 int errno = add_message(arena, new_messages, message);

 if (!errno) {
  return 1;
 }

 struct Subject* new_subject = allocate_memory(arena, sizeof(struct Subject), false);

 new_subject->sub_name = sub_name;
 new_subject->sub_len = sub_len;
 new_subject->subjects = hashmap_init(arena, seed, 0);
 new_subject->messages = new_messages;

  
  return 0;
}




#endif // SUBJECTS_H
