CC=gcc
CXX=g++ -std=c++0x
RM=rm -f
RMDIR=rm -rf

# INC=-I../common/include/
# USE_EIGEN=-IEigen/
USE_TM=-I../trimesh2/include/ -L../trimesh2/lib.Linux64/
USE_RS=-I../rstree/ ../rstree/*.o
USE_GSL=-I/usr/local/GNU/gsl/include/ -L/usr/local/GNU/gsl/lib/

SRC=src
OBJ=obj
OUT=out

all: clean filter extract_query_rand extract_query_spec pre_index obj_batch_sel obj_gen obj_combine noise

filter:
	$(CXX) $(INC) $(USE_TM) $(SRC)/filter.cpp -o $(OUT)/filter.out -ltrimesh -lgluit -fopenmp

extract_query_rand:
	$(CXX) $(INC) $(USE_TM) $(USE_GSL) $(SRC)/extract_query_rand.cpp -o $(OUT)/extract_query_rand.out -ltrimesh -lgluit -fopenmp -lgsl -lgslcblas -lm

extract_query_spec:
	$(CXX) $(INC) $(USE_TM) $(USE_GSL) $(SRC)/extract_query_spec.cpp -o $(OUT)/extract_query_spec.out -ltrimesh -lgluit -fopenmp -lgsl -lgslcblas -lm

pre_index:
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/pre_index.cpp -o $(OUT)/pre_index.out -ltrimesh -lgluit -fopenmp

obj_gen:
	$(CXX) $(INC) $(USE_TM) $(USE_GSL) $(SRC)/obj_gen.cpp -o $(OUT)/obj_gen.out -ltrimesh -lgluit -fopenmp -lgsl -lgslcblas -lm

obj_batch_sel:
	$(CXX) $(INC) $(USE_TM) $(SRC)/obj_batch_sel.cpp -o $(OUT)/obj_batch_sel.out -ltrimesh -lgluit -fopenmp

obj_combine:
	$(CXX) $(INC) $(USE_TM) $(SRC)/obj_combine.cpp -o $(OUT)/obj_combine.out -ltrimesh -lgluit -fopenmp

noise:
	$(CXX) $(INC) $(USE_TM) $(USE_GSL) $(SRC)/noise.cpp -o $(OUT)/noise.out -ltrimesh -lgluit -fopenmp -lgsl -lgslcblas -lm

clean:
	$(RMDIR) obj/
	$(RM) out/*.out