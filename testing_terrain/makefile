CC=gcc
CXX=g++ -std=c++0x
RM=rm -f
RMDIR=rm -rf

INC=-I../core/include/
USE_EIGEN=-I../../eigen/
USE_TM=-I../../trimesh2/include/ -L../../trimesh2/lib.Linux64/
USE_RS=-I../rstree/ ../rstree/*.o

OBJ=../core/obj

all: exe

exe:
	$(CXX) $(USE_TM) $(USE_EIGEN) matching.cpp -o matching -ltrimesh -lgluit -fopenmp -g

test:
	$(CXX) $(USE_TM) $(USE_EIGEN) matching.cpp -o matching -ltrimesh -lgluit -fopenmp -g -D_TEST