all: main

SHELL:=/bin/bash -O extglob

#CC = gcc
CC = clang++

#debug
CFLAGS = -Wall -O1 -g -std=gnu++11

#release
#CFLAGS = -03 -std=gnu++11

#libraries
LIBS = -L ./$(BINDIR) -llanetr -ludsserver -lpthread

#includes
INCL = -I /usr/local/include/eigen3/

#open cv
OCVLIBS = `pkg-config opencv --cflags --libs`

#dir
BINDIR = bin

_OBJS = LaneDetector.o LaneModels.o
OBJS = $(patsubst %,$(BINDIR)/%,$(_OBJS))

_OBJS_UDS = ../../common/UDSServer.o
OBJS_UDS = $(patsubst %,$(BINDIR)/%,$(_OBJS_UDS))

main: main.cpp $(BINDIR)/liblanetr.a $(BINDIR)/libudsserver.a
	$(CC) $(CFLAGS) -o $(BINDIR)/main.elf main.cpp $(LIBS) $(OCVLIBS)

$(BINDIR)/%.o: %.cpp %.hpp
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCL) $(OCVLIBS)

$(BINDIR)/liblanetr.a: $(OBJS)
	ar crv $@ $^

$(BINDIR)/libudsserver.a: $(OBJS_UDS)
	ar crv $@ $^


clean:
	rm $(BINDIR)/!(tracking.mp4)
