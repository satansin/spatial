CC=gcc
CXX=g++ -std=c++0x
RM=rm -f

# examples for more complex make (see original webpage):
# https://stackoverflow.com/questions/2481269/how-to-make-a-simple-c-makefile

# SRCS=tool.cc support.cc
# OBJS=$(subst .cc,.o,$(SRCS))

# all: tool

# tool: $(OBJS)
#     $(CXX) $(LDFLAGS) -o tool $(OBJS) $(LDLIBS) 

# tool.o: tool.cc support.hh

# support.o: support.hh support.cc

# clean:
#     $(RM) $(OBJS)

# distclean: clean
#     $(RM) tool

all: clean pcg.out exh.out trans.out sta.out

pcg.out:
	$(CXX) src/pcg.cpp -o src/pcg.out

exh.out: point.o circumcenter.o
	$(CXX) src/exh.cpp src/point.o src/circumcenter.o -o src/exh.out

trans.out: point.o
	$(CXX) src/trans.cpp src/point.o -o src/trans.out

sta.out:
	$(CXX) src/sta.cpp -o src/sta.out

point.o:
	$(CXX) -c src/point.cpp -o src/point.o

circumcenter.o:
	$(CXX) -c src/circumcenter.cpp -o src/circumcenter.o

clean:
	$(RM) src/*.o
	$(RM) src/*.out