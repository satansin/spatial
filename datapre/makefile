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

all: clean filter extract_query pre_index batch_sel test

filter:
	$(CXX) $(INC) $(USE_TM) $(SRC)/filter.cpp -o $(OUT)/filter.out -ltrimesh -lgluit -fopenmp

extract_query:
	$(CXX) $(INC) $(USE_TM) $(USE_GSL) $(SRC)/extract_query.cpp -o $(OUT)/extract_query.out -ltrimesh -lgluit -fopenmp -lgsl -lgslcblas -lm

pre_index:
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/pre_index.cpp -o $(OUT)/pre_index.out -ltrimesh -lgluit -fopenmp

batch_sel:
	$(CXX) $(INC) $(SRC)/batch_sel.cpp -o $(OUT)/batch_sel.out

test:
	$(CXX) $(INC) $(USE_TM) $(SRC)/test.cpp -o $(OUT)/test.out -ltrimesh -lgluit -fopenmp

clean:
	$(RMDIR) obj/
	$(RM) out/*.out