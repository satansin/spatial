CC=gcc
CXX=g++ -std=c++0x
RM=rm -f
RMDIR=rm -rf

INC=-Isrc/include/
USE_EIGEN=-IEigen/
USE_TM=-Itrimesh2/include/ -Ltrimesh2/lib.Linux64/

SRC=src
OBJ=src/obj
OUT=src/out

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

# All compilations are recommended to add $(INC)
# How to use Eigen:
#     add $(USE_EIGEN)
# How to use trimesh2:
#     add $(USE_TM)
#     add the following !!in the end of out!!: -ltrimesh -lgluit -fopenmp

all: clean pcd_prepro pcg exh kitti range 4pcs

pcd_prepro:
	$(CXX) $(INC) $(SRC)/pcd_prepro.cpp -o $(OUT)/pcd_prepro.out

pcg:
	$(CXX) $(INC) $(SRC)/pcg.cpp -o $(OUT)/pcg.out

exh: point.o tetra_meas.o
	$(CXX) $(INC) $(USE_TM) $(SRC)/exh.cpp $(OBJ)/* -o $(OUT)/exh.out -ltrimesh -lgluit -fopenmp

kitti:
	$(CXX) $(INC) $(USE_TM) $(SRC)/kitti.cpp -o $(OUT)/kitti.out -ltrimesh -lgluit -fopenmp

range:
	$(CXX) $(INC) $(USE_TM) $(SRC)/range.cpp -o $(OUT)/range.out -ltrimesh -lgluit -fopenmp

4pcs: trans.o point.o
	$(CXX) $(INC) $(USE_TM) $(SRC)/4pcs.cpp $(OBJ)/* -o $(OUT)/4pcs.out -ltrimesh -lgluit -fopenmp

trans.o: point.o
	$(CXX) $(INC) $(USE_EIGEN) -c $(SRC)/trans.cpp -o $(OBJ)/trans.o

point.o: $(OBJ)/
	$(CXX) $(INC) -c $(SRC)/point.cpp -o $(OBJ)/point.o

tetra_meas.o:
	$(CXX) $(INC) $(USE_EIGEN) $(USE_TM) -c $(SRC)/tetra_meas.cpp -o $(OBJ)/tetra_meas.o

# circumcenter.o:
# 	$(CXX) $(INC) -c $(SRC)/circumcenter.cpp -o $(OBJ)/circumcenter.o

$(OBJ)/:
	$(shell mkdir -p $(OBJ))

clean:
	$(RMDIR) src/obj/
	$(RM) src/out/*.out