# Makefile for the smash program
CC = g++ 
CFLAGS = -Wall -Werror -pedantic-errors -DNDEBUG *.cpp
CCLINK = $(CC)
OBJS = smash.o commands.o signals.o jobs.o
RM = rm -f
# Creating the  executable
smash: $(OBJS)
	$(CCLINK) -o smash $(OBJS)
# Creating the object files
commands.o: commands.cpp commands.h
smash.o: smash.cpp commands.h
signals.o: signals.cpp signals.h
jobs.o : jobs.cpp jobs.h
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) smash *.o *~ "#"* core.*