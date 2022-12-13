# Makefile for the smash program
CXX = g++
CXXFLAGS = -std=c++11 -g -Wall -Werror -pedantic-errors -DNDEBUG
CXXLINK = $(CXX)
OBJS = Bank.o Account.o
RM = rm -f
# Creating the  executable
smash: $(OBJS)
	$(CXXLINK) $(CXXFLAGS) $(OBJS) -o Bank
# Creating the object files
Bank.o: Bank.cpp Bank.h Account.h
Account.o: Account.cpp Account.h
# Cleaning old files before new make
clean:
	$(RM) *.o Bank