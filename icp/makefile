CC=gcc
CXX=g++ -std=c++0x
RM=rm -f
RMDIR=rm -rf

INC=-I../src/include/
USE_EIGEN=-I../Eigen/
USE_TM=-I../trimesh2/include/ -L../trimesh2/lib.Linux64/

# SRC=src
OBJ=obj
#OUT=src/out

all:
	$(CXX) $(INC) $(USE_TM) $(USE_EIGEN) icp.cpp -o icp.out -ltrimesh -lgluit -fopenmp