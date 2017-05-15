all: main
.PHONY: clean

SHELL:=/bin/bash -O extglob

#CC = gcc
CC = clang++

#debug
CFLAGS = -Wall -O1 -g -std=gnu++11

#release
#CFLAGS = -03 -std=gnu++11

#libraries
LIBS = `pkg-config opencv --libs` \
        -lpthread \
		-L ./$(BINDIR) -ludsserver

#includes
INCL = -I./ `pkg-config opencv --cflags` \
        -I /usr/local/include/eigen3/

#dir
BINDIR = bin

HEADER = $(wildcard ./*.h) \
		$(wildcard ./*.hpp)

_OBJS = LaneDetector.o \
		LaneModels.o \
		main.o
OBJS = $(patsubst %,$(BINDIR)/%,$(_OBJS))

_OBJS_UDS = ../../common/UDSServer.o
OBJS_UDS = $(patsubst %,$(BINDIR)/%,$(_OBJS_UDS))

main: main.cpp $(OBJS) $(BINDIR)/libudsserver.a
	$(CC) $(CFLAGS) -o $(BINDIR)/main.elf $(OBJS) $(LIBS)

$(BINDIR)/%.o: %.cpp $(HEADER)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCL)

$(BINDIR)/libudsserver.a: $(OBJS_UDS)
	ar crv $@ $^

clean:
	rm $(BINDIR)/!(tracking.mp4)
