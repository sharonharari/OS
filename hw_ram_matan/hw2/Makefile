CCC = g++
CXXFLAGS = -g -Wall -pthread
CXXLINK = $(CCC)
OBJS = main.o bank.o ATMs.o account.o log.o
files: $(OBJS)
	$(CXXLINK) $(CXXFLAGS) -o Bank $(OBJS)
bank.o: bank.cpp bank.h account.h log.h
ATMs.o: ATMs.cpp ATMs.h bank.h
account.o: account.cpp account.h log.h
log.o: log.cpp log.h
main.o: main.cpp bank.h ATMs.h
clean:
	rm -f *.o *.bak *~"#"* core Bank