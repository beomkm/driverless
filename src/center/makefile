all: main

#CC = gcc
CC = clang++

#debug
CFLAGS = -Wall -O1 -g -std=gnu++11

#release
#CFLAGS = -03 -std=gnu++11

#libraries
LIBS = -L ./$(BINDIR) -lcenter -lpthread

#dir
BINDIR = bin

_OBJS = Serial.o Control.o Receiver.o
OBJS = $(patsubst %,$(BINDIR)/%,$(_OBJS))

main: center.cpp $(BINDIR)/libcenter.a
	$(CC) $(CFLAGS) -o $(BINDIR)/center.elf center.cpp $(LIBS)

$(BINDIR)/%.o: %.cpp %.hpp
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/libcenter.a: $(OBJS)
	ar crv $@ $^

clean:
	rm $(BINDIR)/*
