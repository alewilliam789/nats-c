TARGET=main

SRC=main.c
OBJ=$(SRC:.c=.o)
PROJECT_DIR = $(shell pwd)
CC=gcc

# The --param=min-pagesize=0 argument is to fix the error:
# error: array subscript 0 is outside array bounds of ‘volatile uint8_t[0]’
#        {aka ‘volatile unsigned char[]’}
# ...which is incorrectly reported in some versions of gcc
CFLAGS=-Wpedantic -Werror -Wall -Wextra
INCLUDE=-I$(PROJECT_DIR)/include

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $(PROJECT_DIR) $(OBJ) -o $@

%.o : main.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

% : %_test.c
	rm -rf ./tests/*.elf

	$(CC) $(CFLAGS) $(INCLUDE) $< -g -o ./tests/$*_test.elf

clean: 
	rm -rf *.o *.elf
