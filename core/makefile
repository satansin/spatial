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

all: query3 query6 query6_adtk \
	 super4pcs \
	 grid index3 index6 pair_dist goicp \
# 	 goicp tk_goicp \
# 	 tk_super4pcs tk_super4pcs_prob tk_superg4pcs tk_superg4pcs_prob \

# 	 anal_sd_ratio anal_reg anal_nn anal_diam anal_q anal_query z_test \
#	 query3_clr super4pcs_clr grid_clr index3_clr \

super4pcs: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs.cpp $(OBJ)/* -o $(OUT)/super.out     -ltrimesh -lgluit -fopenmp -g

super4pcs_clr: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs.cpp $(OBJ)/* -o $(OUT)/super_clr.out -ltrimesh -lgluit -fopenmp -D_CLR -g

# tk_super4pcs: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs_tk.cpp $(OBJ)/* -o $(OUT)/tk_super4pcs.out       -ltrimesh -lgluit -fopenmp -g

# tk_super4pcs_prob: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs.cpp $(OBJ)/* -o $(OUT)/tk_super4pcs_prob.out  -ltrimesh -lgluit -fopenmp -D_PROB -D_TK -D_CLR -g

# # tk_superg4pcs: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs_tk.cpp $(OBJ)/* -o $(OUT)/tk_superg4pcs.out      -ltrimesh -lgluit -fopenmp -D_GEN -g

# tk_superg4pcs_prob: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs.cpp $(OBJ)/* -o $(OUT)/tk_superg4pcs_prob.out -ltrimesh -lgluit -fopenmp -D_GEN -D_PROB -D_TK -D_CLR -g


tk_goicp: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/goicp_tk.cpp $(OBJ)/* -o $(OUT)/tk_goicp.out -ltrimesh -lgluit -fopenmp #-g


grid: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/grid.cpp $(OBJ)/* -o $(OUT)/grid.out     -ltrimesh -lgluit -fopenmp

grid_clr: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/grid.cpp $(OBJ)/* -o $(OUT)/grid_clr.out -ltrimesh -lgluit -fopenmp -D_CLR


index6: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/index.cpp $(OBJ)/* -o $(OUT)/index6.out     -ltrimesh -lgluit -fopenmp -g

index3: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/index.cpp $(OBJ)/* -o $(OUT)/index3.out     -ltrimesh -lgluit -fopenmp -D_IDX3 -g

index3_clr: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/index.cpp $(OBJ)/* -o $(OUT)/index3_clr.out -ltrimesh -lgluit -fopenmp -D_IDX3 -D_CLR -g


pair_dist: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/pair_dist.cpp $(OBJ)/* -o $(OUT)/pair_dist.out -ltrimesh -lgluit -fopenmp


query3: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query3.out     -ltrimesh -lgluit -fopenmp -D_IDX3 #-DVERBOSE=1 -g #-DTEST_MODE #-g

query6: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query6.out     -ltrimesh -lgluit -fopenmp #-DVERBOSE=2 #-DTEST_MODE #-g

query3_clr: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query3_clr.out -ltrimesh -lgluit -fopenmp -D_IDX3 -D_CLR #-DVERBOSE=2 #-DTEST_MODE #-g


query6_adtk: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query_adtk.cpp $(OBJ)/* -o $(OUT)/query6_adtk.out     -ltrimesh -lgluit -fopenmp -g #-DVERBOSE=2 #-DTEST_MODE #-g


analyze: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/analyze.cpp $(OBJ)/* -o $(OUT)/analyze.out -ltrimesh -lgluit -fopenmp

anal_reg: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/anal_reg.cpp $(OBJ)/* -o $(OUT)/anal_reg.out -ltrimesh -lgluit -fopenmp

anal_nn: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/anal_nn.cpp $(OBJ)/* -o $(OUT)/anal_nn.out -ltrimesh -lgluit -fopenmp

anal_q: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/anal_q.cpp $(OBJ)/* -o $(OUT)/anal_q.out -ltrimesh -lgluit -fopenmp

anal_diam: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/anal_diam.cpp $(OBJ)/* -o $(OUT)/anal_diam.out -ltrimesh -lgluit -fopenmp

anal_sd_ratio: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/anal_sd_ratio.cpp $(OBJ)/* -o $(OUT)/anal_sd_ratio.out -ltrimesh -lgluit -fopenmp

anal_query: build_obj
	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/anal_query.cpp $(OBJ)/* -o $(OUT)/anal_query.out -ltrimesh -lgluit -fopenmp -D_IDX3 -DVERBOSE=-1 -DTEST_MODE

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