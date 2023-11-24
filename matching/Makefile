NAME = multithreaded_imfant
CC = g++
LIBRARIES = iMFAnt.cpp
HEADERS = iMFAnt.hpp
UNAME_S := $(shell uname -s)



all: $(NAME)

$(NAME): $(HEADERS) $(LIBRARIES) $(NAME).cpp

	$(CC) -fopenmp -o $@ $(NAME).cpp $(LIBRARIES) -O3 -DREPS=5

clean:
	rm -f $(NAME)
