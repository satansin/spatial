CC=gcc
CXX=g++ -std=c++0x
RM=rm -f
RMDIR=rm -rf

INC=-Iinclude/
USE_EIGEN=-I../Eigen/
USE_TM=-I../trimesh2/include/ -L../trimesh2/lib.Linux64/
USE_RS=-I../rstree/ ../rstree/*.o

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
#     add the following (!!in the end!!): -ltrimesh -lgluit -fopenmp

all: query6 query3 query6_acc query3_acc index6 index3 super4pcs superg4pcs grid # analyze test

super4pcs: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs.cpp $(OBJ)/* -o $(OUT)/super4pcs.out -ltrimesh -lgluit -fopenmp #-g

superg4pcs: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/superg4pcs.cpp $(OBJ)/* -o $(OUT)/superg4pcs.out -ltrimesh -lgluit -fopenmp #-g

grid: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/grid.cpp $(OBJ)/* -o $(OUT)/grid.out -ltrimesh -lgluit -fopenmp

index6: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/index.cpp $(OBJ)/* -o $(OUT)/index6.out -ltrimesh -lgluit -fopenmp

index3: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/index.cpp $(OBJ)/* -o $(OUT)/index3.out -ltrimesh -lgluit -fopenmp -DIDX_3

query6: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query6.out -ltrimesh -lgluit -fopenmp -g

query3: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query3.out -ltrimesh -lgluit -fopenmp -DIDX_3

query6_acc: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query6_acc.out -ltrimesh -lgluit -fopenmp -DACC

query3_acc: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query3_acc.out -ltrimesh -lgluit -fopenmp -DIDX_3 -DACC

analyze: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/analyze.cpp $(OBJ)/* -o $(OUT)/analyze.out -ltrimesh -lgluit -fopenmp

test: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/test.cpp $(OBJ)/* -o $(OUT)/test.out -ltrimesh -lgluit -fopenmp #-g

build_obj: clean trans.o tetra_meas.o c_rtree.o mesh.o util.o

trans.o: $(OBJ)/
	$(CXX) $(INC) $(USE_EIGEN) $(USE_TM) -c $(SRC)/trans.cpp -o $(OBJ)/trans.o

tetra_meas.o: $(OBJ)/
	$(CXX) $(INC) $(USE_EIGEN) $(USE_TM) -c $(SRC)/tetra_meas.cpp -o $(OBJ)/tetra_meas.o

c_rtree.o: $(OBJ)/
	$(CXX) $(INC) $(USE_EIGEN) $(USE_TM) -I../rstree/ -c $(SRC)/c_rtree.cpp -o $(OBJ)/c_rtree.o

mesh.o: $(OBJ)/
	$(CXX) $(INC) $(USE_EIGEN) $(USE_TM) -c $(SRC)/mesh.cpp -o $(OBJ)/mesh.o

util.o: $(OBJ)/
	$(CXX) $(INC) -c $(SRC)/util.cpp -o $(OBJ)/util.o

$(OBJ)/:
	$(shell mkdir -p $(OBJ))

clean:
	$(RMDIR) $(OBJ)/
	$(RM) $(OUT)/*.out