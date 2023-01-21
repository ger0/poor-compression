CXX=clang++
MAIN_SOURCES=main.cpp fixed_length.cpp
MAIN_OBJS=$(MAIN_SOURCES:%.cpp=build/%.cpp.o)
CCFLAGS=-g -Wextra -Wall -pedantic -Wall -Iinclude -I. --std=c++20
CXXFLAGS=$(CCFLAGS)
LDFLAGS=

.PHONY : run clean

build : compress 

run : build
	./compress

rd : build
	gdb -ex run ./compress

compress : $(MAIN_OBJS)
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(LDFLAGS)

build/%.cpp.o : %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MD -MP $< -c -o $@ 

FORCE:

-include $(MAIN_OBJS:.o=.d)
