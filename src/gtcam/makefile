all: main

#for recipe 'clean'
SHELL:=/bin/bash -O extglob

#CC = gcc
CC = clang++

#debug
CFLAGS = -Wall -O1 -g -std=gnu++11

#release
#CFLAGS = -03 -std=gnu++11

#libraries
LIBS = -L ./$(BINDIR) -llanetr -lpthread

#includes
INCL = -I /usr/local/include/eigen3/

#open cv
OCVLIBS = `pkg-config opencv --cflags --libs`

#dir
BINDIR = bin

_OBJS = LaneDetector.o LaneModels.o
OBJS = $(patsubst %,$(BINDIR)/%,$(_OBJS))

main: main.cpp $(BINDIR)/liblanetr.a
	$(CC) $(CFLAGS) -o $(BINDIR)/main.elf main.cpp $(LIBS) $(OCVLIBS)

$(BINDIR)/%.o: %.cpp %.hpp
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCL) $(OCVLIBS)

$(BINDIR)/liblanetr.a: $(OBJS)
	ar crv $@ $^

clean:
	rm $(BINDIR)/!(tracking.mp4)
