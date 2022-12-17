# Makefile for the smash program
CXX = g++
CXXFLAGS = -std=c++11 -g -Wall -Werror -pthread -pedantic-errors -DNDEBUG
CXXLINK = $(CXX)
OBJS = main.o Bank.o Account.o
RM = rm -f
# Creating the  executable
Bank: $(OBJS)
	$(CXXLINK) $(CXXFLAGS) $(OBJS) -o Bank
# Creating the object files
main.o: main.cpp Bank.h Account.h
Bank.o: Bank.cpp Bank.h Account.h
Account.o: Account.cpp Account.h
# Cleaning old files before new make
clean:
	$(RM) *.o Bank