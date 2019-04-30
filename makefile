CC=gcc
CXX=g++ -std=c++0x
RM=rm -f

INC=-Isrc/include/
USE_EIGEN=-IEigen/
USE_TM=-Itrimesh2/include/ -Ltrimesh2/lib.Linux64/

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

all: clean pcd_prepro pcg exh trans sta

pcd_prepro:
	$(CXX) $(INC) $(USE_TM) src/pcd_prepro.cpp -o src/out/pcd_prepro -ltrimesh -lgluit -fopenmp

pcg:
	$(CXX) $(INC) src/pcg.cpp -o src/out/pcg

exh: point.o circumcenter.o
	$(CXX) $(INC) src/exh.cpp src/obj/point.o src/obj/circumcenter.o -o src/out/exh

trans: point.o
	$(CXX) $(INC) $(USE_EIGEN) src/trans.cpp src/obj/point.o -o src/out/trans

sta:
	$(CXX) $(INC) src/sta.cpp -o src/out/sta

point.o:
	$(CXX) $(INC) -c src/point.cpp -o src/obj/point.o

circumcenter.o:
	$(CXX) $(INC) -c src/circumcenter.cpp -o src/obj/circumcenter.o

clean:
	$(RM) src/obj/*
	$(RM) src/out/*