CC=gcc
CXX=g++ -std=c++0x
RM=rm -f
RMDIR=rm -rf

# INC=-I../common/include/
# USE_EIGEN=-IEigen/
USE_TM=-I../trimesh2/include/ -L../trimesh2/lib.Linux64/
USE_RS=-I../rstree/ ../rstree/*.o

SRC=src
OBJ=obj
OUT=out

all: clean pcg kitti_join filter extract_query_manual extract_query_auto extract_query_batch pre_index test

pcg:
	$(CXX) $(INC) $(SRC)/pcg.cpp -o $(OUT)/pcg.out

kitti_join:
	$(CXX) $(INC) $(USE_TM) $(SRC)/kitti_join.cpp -o $(OUT)/kitti_join.out -ltrimesh -lgluit -fopenmp

filter:
	$(CXX) $(INC) $(USE_TM) $(SRC)/filter.cpp -o $(OUT)/filter.out -ltrimesh -lgluit -fopenmp
	
extract_query_manual:
	$(CXX) $(INC) $(USE_TM) $(SRC)/extract_query_manual.cpp -o $(OUT)/extract_query_manual.out -ltrimesh -lgluit -fopenmp

extract_query_auto:
	$(CXX) $(INC) $(USE_TM) $(SRC)/extract_query_auto.cpp -o $(OUT)/extract_query_auto.out -ltrimesh -lgluit -fopenmp

extract_query_batch:
	$(CXX) $(INC) $(USE_TM) $(SRC)/extract_query_batch.cpp -o $(OUT)/extract_query_batch.out -ltrimesh -lgluit -fopenmp

pre_index:
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/pre_index.cpp -o $(OUT)/pre_index.out -ltrimesh -lgluit -fopenmp

test:
	$(CXX) $(INC) $(USE_TM) $(SRC)/test.cpp -o $(OUT)/test.out -ltrimesh -lgluit -fopenmp

clean:
	$(RMDIR) obj/
	$(RM) out/*.out