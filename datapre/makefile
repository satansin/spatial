CC=gcc
CXX=g++ -std=c++0x
RM=rm -f
RMDIR=rm -rf

# INC=-Isrc/include/
# USE_EIGEN=-IEigen/
USE_TM=-I../trimesh2/include/ -L../trimesh2/lib.Linux64/

SRC=src
OBJ=obj
OUT=out

all: clean pcg kitti_join filter extract_query

pcg:
	$(CXX) $(INC) $(SRC)/pcg.cpp -o $(OUT)/pcg.out

kitti_join:
	$(CXX) $(INC) $(USE_TM) $(SRC)/kitti_join.cpp -o $(OUT)/kitti_join.out -ltrimesh -lgluit -fopenmp

filter:
	$(CXX) $(INC) $(USE_TM) $(SRC)/filter.cpp -o $(OUT)/filter.out -ltrimesh -lgluit -fopenmp
	
extract_query:
	$(CXX) $(INC) $(USE_TM) $(SRC)/extract_query.cpp -o $(OUT)/extract_query.out -ltrimesh -lgluit -fopenmp

clean:
	$(RMDIR) obj/
	$(RM) out/*.out