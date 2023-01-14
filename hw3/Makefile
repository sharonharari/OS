# Makefile for the smash program
CXX = g++
CXXFLAGS = -std=c++11 -g -Wall -Werror -pthread -pedantic-errors -DNDEBUG
CXXLINK = $(CXX)
OBJS = server.o adt.o
RM = rm -f
# Creating the  executable
ttftps: $(OBJS)
	$(CXXLINK) $(CXXFLAGS) $(OBJS) -o ttftps
# Creating the object files
server.o: server.cpp adt.h
adt.o: adt.cpp adt.h
# Cleaning old files before new make
clean:
	$(RM) *.o ttftps