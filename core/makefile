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
# OBJ=obj_tmp
OUT=out
# OUT=out_tmp

# examples for more complex make (see original webpage):
# https://stackoverflow.com/questions/2481269/how-to-make-a-simple-c-makefile

# All compilations are recommended to add $(INC)
# How to use Eigen:
#     add $(USE_EIGEN)
# How to use trimesh2:
#     add $(USE_TM)
#     add the following (!!in the end!!): -ltrimesh -lgluit -fopenmp

all: goicp \
	 super4pcs_det superg4pcs_det \
	 super4pcs_prob superg4pcs_prob \
	 grid_gt grid_donut grid_3nn index3 index6 \
	 query_prob3 query_prob3_cpq query_prob6 query_prob6_cpq \
	 query_3nn_prob3 query_3nn_prob3_cpq query_3nn_prob6 query_3nn_prob6_cpq \
	 query_donut_prob3 query_donut_prob3_cpq query_donut_prob6 query_donut_prob6_cpq \

# 	 grid grid_donut grid_3nn grid_3lnn index3
# 	 query_det query_det_eps query_det_icp query_det_nar \
# 	 anal_reg anal_nn anal_diam z_test \

super4pcs_det: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs.cpp $(OBJ)/* -o $(OUT)/super4pcs_det.out   -ltrimesh -lgluit -fopenmp -g

super4pcs_prob: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs.cpp $(OBJ)/* -o $(OUT)/super4pcs_prob.out  -ltrimesh -lgluit -fopenmp -D_PROB -g

superg4pcs_det: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs.cpp $(OBJ)/* -o $(OUT)/superg4pcs_det.out  -ltrimesh -lgluit -fopenmp -D_GEN -g

superg4pcs_prob: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs.cpp $(OBJ)/* -o $(OUT)/superg4pcs_prob.out -ltrimesh -lgluit -fopenmp -D_GEN -D_PROB -g


grid_gt: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/grid.cpp $(OBJ)/* -o $(OUT)/grid_gt.out    -ltrimesh -lgluit -fopenmp -g

grid_donut: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/grid.cpp $(OBJ)/* -o $(OUT)/grid_donut.out -ltrimesh -lgluit -fopenmp -D_DNT -g

grid_3nn: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/grid.cpp $(OBJ)/* -o $(OUT)/grid_3nn.out   -ltrimesh -lgluit -fopenmp -D_3NN

grid_3lnn: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/grid.cpp $(OBJ)/* -o $(OUT)/grid_3lnn.out  -ltrimesh -lgluit -fopenmp -D_3NN -D_3LNN


index6: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/index.cpp $(OBJ)/* -o $(OUT)/index6.out -ltrimesh -lgluit -fopenmp

index3: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/index.cpp $(OBJ)/* -o $(OUT)/index3.out -ltrimesh -lgluit -fopenmp -DIDX_3 -g

# index3_3nn: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/index.cpp $(OBJ)/* -o $(OUT)/index3_3nn.out -ltrimesh -lgluit -fopenmp -DIDX_3 -D_3NN


query_det: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_det.out -ltrimesh -lgluit -fopenmp -DIDX_3 #-DVERBOSE=1 -DTEST_MODE

query_det_eps: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_det_eps.out -ltrimesh -lgluit -fopenmp -DIDX_3 -DPROB #-DTEST_MODE -DVERBOSE=5

query_det_icp: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_det_icp.out -ltrimesh -lgluit -fopenmp -DIDX_3 -DICPONLY #-DTEST_MODE -DVERBOSE=5

query_det_nar: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_det_nar.out -ltrimesh -lgluit -fopenmp -DIDX_3 -DACC #-DTEST_MODE -DVERBOSE=5


query_prob3: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_prob3.out           -ltrimesh -lgluit -fopenmp -DPROB -DICPONLY -DACC -DIDX_3 #-DVERBOSE=2 #-DTEST_MODE #-g

query_prob3_cpq: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_prob3_cpq.out       -ltrimesh -lgluit -fopenmp -DPROB -DICPONLY -DACC -DIDX_3 -D_CPQ #-DVERBOSE=5 -DTEST_MODE -g

query_prob6: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_prob6.out           -ltrimesh -lgluit -fopenmp -DPROB -DICPONLY -DACC #-DVERBOSE=1 -DTEST_MODE -g

query_prob6_cpq: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_prob6_cpq.out       -ltrimesh -lgluit -fopenmp -DPROB -DICPONLY -DACC -D_CPQ #-DVERBOSE=1 -DTEST_MODE -g


query_donut_prob3: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_donut_prob3.out     -ltrimesh -lgluit -fopenmp -DPROB -DICPONLY -DACC -D_DNT -DIDX_3 -g #-DVERBOSE=1 -DTEST_MODE

query_donut_prob3_cpq: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_donut_prob3_cpq.out -ltrimesh -lgluit -fopenmp -DPROB -DICPONLY -DACC -D_DNT -D_CPQ -DIDX_3 -g #-DVERBOSE=1 #-DTEST_MODE

query_donut_prob6: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_donut_prob6.out     -ltrimesh -lgluit -fopenmp -DPROB -DICPONLY -DACC -D_DNT -g #-DVERBOSE=0 -DTEST_MODE

query_donut_prob6_cpq: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_donut_prob6_cpq.out -ltrimesh -lgluit -fopenmp -DPROB -DICPONLY -DACC -D_DNT -D_CPQ -g #-DVERBOSE=0 #-DTEST_MODE


query_3nn_prob3: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_3nn_prob3.out       -ltrimesh -lgluit -fopenmp -DPROB -DICPONLY -DACC -DIDX_3 -D_3NN #-DVERBOSE=1 #-DTEST_MODE -g

query_3nn_prob3_cpq: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_3nn_prob3_cpq.out   -ltrimesh -lgluit -fopenmp -DPROB -DICPONLY -DACC -DIDX_3 -D_3NN -D_CPQ #-DVERBOSE=1 #-DTEST_MODE -g

query_3nn_prob6: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_3nn_prob6.out       -ltrimesh -lgluit -fopenmp -DPROB -DICPONLY -DACC -D_3NN #-DVERBOSE=1 #-DTEST_MODE -g

query_3nn_prob6_cpq: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_3nn_prob6_cpq.out   -ltrimesh -lgluit -fopenmp -DPROB -DICPONLY -DACC -D_3NN -D_CPQ #-DVERBOSE=1 #-DTEST_MODE -g


query_3lnn_prob3: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_3lnn_prob3.out      -ltrimesh -lgluit -fopenmp -DPROB -DICPONLY -DACC -DIDX_3 -D_3NN -D_3LNN #-DVERBOSE=1 -DTEST_MODE -g

query_3lnn_prob3_cpq: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_3lnn_prob3_cpq.out  -ltrimesh -lgluit -fopenmp -DPROB -DICPONLY -DACC -DIDX_3 -D_3NN -D_3LNN -D_CPQ #-DVERBOSE=1 #-DTEST_MODE -g


analyze: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/analyze.cpp $(OBJ)/* -o $(OUT)/analyze.out -ltrimesh -lgluit -fopenmp

anal_reg: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/anal_reg.cpp $(OBJ)/* -o $(OUT)/anal_reg.out -ltrimesh -lgluit -fopenmp

anal_nn: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/anal_nn.cpp $(OBJ)/* -o $(OUT)/anal_nn.out -ltrimesh -lgluit -fopenmp

anal_diam: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/anal_diam.cpp $(OBJ)/* -o $(OUT)/anal_diam.out -ltrimesh -lgluit -fopenmp

z_test: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/z_test.cpp $(OBJ)/* -o $(OUT)/z_test.out -ltrimesh -lgluit -fopenmp

# binary_helper: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/binary_helper.cpp $(OBJ)/* -o $(OUT)/binary_helper.out -ltrimesh -lgluit -fopenmp #-g


goicp: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/goicp.cpp $(OBJ)/* -o $(OUT)/goicp.out -ltrimesh -lgluit -fopenmp #-g


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