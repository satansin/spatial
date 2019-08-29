CC=gcc
CXX=g++ -std=c++0x
RM=rm -f
RMDIR=rm -rf

INC=-Iinclude/
USE_EIGEN=-I../Eigen/
USE_TM=-I../trimesh2/include/ -L../trimesh2/lib.Linux64/

SRC=src
OBJ=obj
OUT=out

# examples for more complex make (see original webpage):
# https://stackoverflow.com/questions/2481269/how-to-make-a-simple-c-makefile

# All compilations are recommended to add $(INC)
# How to use Eigen:
#     add $(USE_EIGEN)
# How to use trimesh2:
#     add $(USE_TM)
#     add the following (!!in the end of out!!): -ltrimesh -lgluit -fopenmp

all: clean 4pcs index query

4pcs: trans.o point.o
	$(CXX) $(INC) $(USE_TM) $(SRC)/4pcs.cpp $(OBJ)/* -o $(OUT)/4pcs.out -ltrimesh -lgluit -fopenmp

index: point.o tetra_meas.o
	$(CXX) $(INC) $(USE_TM) $(SRC)/index.cpp $(OBJ)/* -o $(OUT)/index.out -ltrimesh -lgluit -fopenmp

query: point.o tetra_meas.o
	$(CXX) $(INC) $(USE_TM) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query.out -ltrimesh -lgluit -fopenmp

trans.o: point.o
	$(CXX) $(INC) $(USE_EIGEN) $(USE_TM) -c $(SRC)/trans.cpp -o $(OBJ)/trans.o

tetra_meas.o: point.o
	$(CXX) $(INC) $(USE_EIGEN) $(USE_TM) -c $(SRC)/tetra_meas.cpp -o $(OBJ)/tetra_meas.o

point.o: $(OBJ)/
	$(CXX) $(INC) $(USE_TM) -c $(SRC)/point.cpp -o $(OBJ)/point.o

# circumcenter.o:
# 	$(CXX) $(INC) -c $(SRC)/circumcenter.cpp -o $(OBJ)/circumcenter.o

$(OBJ)/:
	$(shell mkdir -p $(OBJ))

clean:
	$(RMDIR) $(OBJ)/
	$(RM) $(OUT)/*.out