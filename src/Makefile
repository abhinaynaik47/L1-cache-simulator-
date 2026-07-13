all:run 
COM=clang++
CFLAGS=-std=c++23  -Wall -g
FILES?=  LRU.cpp cache.cpp core.cpp bus.cpp MESI.cpp
FILESH?= LRU.hpp cache.hpp core.hpp bus.hpp MESI.hpp
OBJS=$(FILES:.cpp=.o)
OUT=L1simulate
DEBUGGER=lldb
run:$(OUT)
$(OUT):$(OBJS)
	$(COM) $(CFLAGS) $(OBJS) -o $(OUT)
%.o:%.cpp
	$(COM) $(CFLAGS) -c $< -o $@

compile:
	$(COM) $(CFLAGS) $(FILE) -o $(OUT)
debug:$(OUT)
	$(DEBUGGER) -- ./$(OUT) $(ARGS)
clean:
	rm -f *.o
	rm -f $(OBJS)
	rm -rf $(OUT).dSYM
	rm -f $(OUT)
	rm -f '.!23986!LRU.o'
