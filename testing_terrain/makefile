CC=gcc
CXX=g++ -std=c++0x
RM=rm -f
RMDIR=rm -rf

INC=-I../core/include/
USE_EIGEN=-I../../eigen/
USE_TM=-I../../trimesh2/include/ -L../../trimesh2/lib.Linux64/
USE_RS=-I../rstree/ ../rstree/*.o
USE_GSL=-I/usr/local/GNU/gsl/include/ -L/usr/local/GNU/gsl/lib/

OBJ=../core/obj

.PHONY: all simple_conv robust_conv robust_conv_tmp test noisify

all: robust_conv noisify test #simple_conv robust_conv_tmp

simple_conv:
	$(CXX) $(USE_TM) $(USE_EIGEN) simple_conv.cpp -o simple_conv -ltrimesh -lgluit -fopenmp -g

robust_conv:
	$(CXX) $(USE_TM) $(USE_EIGEN) robust_conv.cpp -o robust_conv -ltrimesh -lgluit -fopenmp -g

robust_conv_tmp:
	$(CXX) $(USE_TM) $(USE_EIGEN) robust_conv_tmp.cpp -o robust_conv_tmp -ltrimesh -lgluit -fopenmp -g

test:
	$(CXX) $(USE_TM) $(USE_EIGEN) test_functions.cpp -o test_functions -ltrimesh -lgluit -fopenmp -g -D_TEST

noisify:
	$(CXX) $(USE_TM) $(USE_EIGEN) $(USE_GSL) noisify.cpp -o noisify -ltrimesh -lgluit -fopenmp -lgsl -lgslcblas -lm -g
