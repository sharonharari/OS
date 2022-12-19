CXX = g++
CXXFLAGS = -std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG -g 
CXXLINK = $(CXX)
OBJS = ttftps.o
TARGET = ttftps
RM = rm -f

$(TARGET): $(OBJS)
	$(CXXLINK) $(CXXFLAGS) $(OBJS) -o $(TARGET)
	
ttftps.o: ttftps.cpp
	$(CXXLINK) $(CXXFLAGS) -c ttftps.cpp 

clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*