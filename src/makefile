all: main

#CC = gcc
CC = clang++

#debug
CFLAGS = -Wall -O1 -g -std=gnu++11

#release
#CFLAGS = -03 -std=gnu++11

LIBS = -L . -lctrl

main: center.cpp libctrl.a
	$(CC) $(CFLAGS) -o center.elf center.cpp $(LIBS)

libctrl.a: Control.hpp Control.cpp Serial.hpp Serial.cpp
	$(CC) $(CFLAGS) -c Control.cpp Serial.cpp
	ar crv libctrl.a Control.o Serial.o
