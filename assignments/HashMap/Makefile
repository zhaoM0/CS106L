
CFLAGS = -Wall -Werror -std=c++17
SRCS   = main.cpp
DEPS   = hashmap.cpp hashmap.h hashmap_iterator.h

.DEFAULT_GOAL := main
main: $(SRCS) $(DEPS)
	g++ $(SRCS) -o $@ $(CFLAGS) 

.PHONY: run clean
run:
	./main

clean:
	rm -f ./main