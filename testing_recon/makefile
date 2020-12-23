CC=gcc
CXX=g++ -std=c++0x
RM=rm -f
RMDIR=rm -rf

INC=-I../core/include/
USE_EIGEN=-I../Eigen/
USE_TM=-I../trimesh2/include/ -L../trimesh2/lib.Linux64/
USE_RS=-I../rstree/ ../rstree/*.o

OBJ=../core/obj
# OBJ=obj_tmp

# examples for more complex make (see original webpage):
# https://stackoverflow.com/questions/2481269/how-to-make-a-simple-c-makefile

# All compilations are recommended to add $(INC)
# How to use Eigen:
#     add $(USE_EIGEN)
# How to use trimesh2:
#     add $(USE_TM)
#     add the following (!!in the end!!): -ltrimesh -lgluit -fopenmp

all: grid_recon index_recon query_recon

grid_recon:
	$(CXX) $(INC) $(USE_TM) $(USE_RS) grid_recon.cpp $(OBJ)/* -o grid_recon.out -ltrimesh -lgluit -fopenmp -D_DNT

index_recon:
	$(CXX) $(INC) $(USE_TM) $(USE_RS) index_recon.cpp $(OBJ)/* -o index3_recon.out -ltrimesh -lgluit -fopenmp -D_IDX3 -g #-D_CLR -g

query_recon:
	$(CXX) $(INC) $(USE_TM) $(USE_RS) query_recon.cpp $(OBJ)/* -o query_recon.out -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_DNT -D_CPQ -D_IDX3 # -D_CLR -g #-DVERBOSE=0 #-DTEST_MODE


# all: super4pcs_prob superg4pcs_prob \
# 	 query_donut_prob3 query_donut_prob3_cpq \
# 	 query_prob3 query_prob3_cpq \
# 	 query_3nn_prob3 query_3nn_prob3_cpq \
# 	 tk_query_donut_prob3 tk_query_prob3 tk_query_3nn_prob3 tk_super4pcs_prob tk_superg4pcs_prob \
# 	 grid_gt grid_donut grid_3nn index3 index6 \
# # 	 goicp tk_goicp \
# # 	 query_donut_prob3 query_donut_prob3_cpq query_donut_prob6 query_donut_prob6_cpq \
# # 	 tk_super4pcs tk_super4pcs_prob tk_superg4pcs tk_superg4pcs_prob \
# # 	 tk_gt6_cpq tk_gt3_cpq tk_donut6_cpq tk_donut3_cpq \
# # 	 tk_gt6_cpq_prob tk_gt3_cpq_prob tk_donut6_cpq_prob tk_donut3_cpq_prob tk_3nn6_cpq_prob tk_3nn3_cpq_prob \
# # 	 query_det3_cpq query_det6_cpq query_donut_det6_cpq query_donut_det3_cpq\
# # 	 query_donut_prob6 query_donut_prob6_cpq query_prob6 query_prob6_cpq query_3nn_prob6 query_3nn_prob6_cpq \
# # 	 super4pcs_det superg4pcs_det \

# # 	 grid_gt grid_donut grid_3nn grid_3lnn index3
# # 	 query_det query_det_eps query_det_icp query_det_nar \
# # 	 anal_reg anal_nn anal_diam anal_q z_test \

# super4pcs_det: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs.cpp $(OBJ)/* -o $(OUT)/super4pcs_det.out   -ltrimesh -lgluit -fopenmp -g

# super4pcs_prob: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs.cpp $(OBJ)/* -o $(OUT)/super4pcs_prob.out  -ltrimesh -lgluit -fopenmp -D_PROB -D_CLR -g

# superg4pcs_det: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs.cpp $(OBJ)/* -o $(OUT)/superg4pcs_det.out  -ltrimesh -lgluit -fopenmp -D_GEN -g

# superg4pcs_prob: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs.cpp $(OBJ)/* -o $(OUT)/superg4pcs_prob.out -ltrimesh -lgluit -fopenmp -D_GEN -D_PROB -D_CLR -g

# # tk_super4pcs: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs_tk.cpp $(OBJ)/* -o $(OUT)/tk_super4pcs.out       -ltrimesh -lgluit -fopenmp -g

# tk_super4pcs_prob: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs.cpp $(OBJ)/* -o $(OUT)/tk_super4pcs_prob.out  -ltrimesh -lgluit -fopenmp -D_PROB -D_TK -D_CLR -g

# # tk_superg4pcs: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs_tk.cpp $(OBJ)/* -o $(OUT)/tk_superg4pcs.out      -ltrimesh -lgluit -fopenmp -D_GEN -g

# tk_superg4pcs_prob: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/super4pcs.cpp $(OBJ)/* -o $(OUT)/tk_superg4pcs_prob.out -ltrimesh -lgluit -fopenmp -D_GEN -D_PROB -D_TK -D_CLR -g


# tk_goicp: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/goicp_tk.cpp $(OBJ)/* -o $(OUT)/tk_goicp.out -ltrimesh -lgluit -fopenmp #-g


# grid_gt: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/grid.cpp $(OBJ)/* -o $(OUT)/grid_gt.out    -ltrimesh -lgluit -fopenmp -D_CLR -g

# grid_donut: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/grid.cpp $(OBJ)/* -o $(OUT)/grid_donut.out -ltrimesh -lgluit -fopenmp -D_DNT # -D_CLR -g

# grid_3nn: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/grid.cpp $(OBJ)/* -o $(OUT)/grid_3nn.out   -ltrimesh -lgluit -fopenmp -D_3NN -D_CLR

# grid_3lnn: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/grid.cpp $(OBJ)/* -o $(OUT)/grid_3lnn.out  -ltrimesh -lgluit -fopenmp -D_3NN -D_3LNN


# index6: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/index.cpp $(OBJ)/* -o $(OUT)/index6.out -ltrimesh -lgluit -fopenmp

# index3: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/index.cpp $(OBJ)/* -o $(OUT)/index3.out -ltrimesh -lgluit -fopenmp -D_IDX3 -D_CLR -g

# # index3_3nn: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/index.cpp $(OBJ)/* -o $(OUT)/index3_3nn.out -ltrimesh -lgluit -fopenmp -D_IDX3 -D_3NN


# query_det6_cpq: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_det6_cpq.out -ltrimesh -lgluit -fopenmp -D_CPQ #-DVERBOSE=1 -DTEST_MODE

# query_det3_cpq: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_det3_cpq.out -ltrimesh -lgluit -fopenmp -D_IDX3 -D_CPQ #-DVERBOSE=1 #-DTEST_MODE -g

# query_donut_det6_cpq: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_donut_det6_cpq.out -ltrimesh -lgluit -fopenmp -D_DNT -D_CPQ #-DVERBOSE=1 -DTEST_MODE

# query_donut_det3_cpq: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_donut_det3_cpq.out -ltrimesh -lgluit -fopenmp -D_DNT -D_IDX3 -D_CPQ #-DVERBOSE=1 #-DTEST_MODE -g

# # query_det_eps: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_det_eps.out -ltrimesh -lgluit -fopenmp -D_IDX3 -D_PROB #-DTEST_MODE -DVERBOSE=5

# # query_det_icp: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_det_icp.out -ltrimesh -lgluit -fopenmp -D_IDX3 -DICPONLY #-DTEST_MODE -DVERBOSE=5

# # query_det_nar: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_det_nar.out -ltrimesh -lgluit -fopenmp -D_IDX3 -D_ACC #-DTEST_MODE -DVERBOSE=5


# # tk_gt6_cpq: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query_tk.cpp $(OBJ)/* -o $(OUT)/tk_gt6_cpq.out -ltrimesh -lgluit -fopenmp -D_CPQ #-DVERBOSE=1 -DTEST_MODE

# # tk_gt3_cpq: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query_tk.cpp $(OBJ)/* -o $(OUT)/tk_gt3_cpq.out -ltrimesh -lgluit -fopenmp -D_IDX3 -D_CPQ #-DVERBOSE=1 -DTEST_MODE

# # tk_donut6_cpq: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query_tk.cpp $(OBJ)/* -o $(OUT)/tk_donut6_cpq.out -ltrimesh -lgluit -fopenmp -D_DNT -D_CPQ #-DVERBOSE=1 -DTEST_MODE

# # tk_donut3_cpq: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query_tk.cpp $(OBJ)/* -o $(OUT)/tk_donut3_cpq.out -ltrimesh -lgluit -fopenmp -D_DNT -D_IDX3 -D_CPQ #-DVERBOSE=1 -DTEST_MODE


# # tk_gt6_cpq_prob: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query_tk.cpp $(OBJ)/* -o $(OUT)/tk_gt6_cpq_prob.out -ltrimesh -lgluit -fopenmp -D_PROB -D_CPQ #-DVERBOSE=1 -DTEST_MODE

# # tk_gt3_cpq_prob: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query_tk.cpp $(OBJ)/* -o $(OUT)/tk_gt3_cpq_prob.out -ltrimesh -lgluit -fopenmp -D_PROB -D_IDX3 -D_CPQ #-DVERBOSE=1 -DTEST_MODE

# # tk_donut6_cpq_prob: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query_tk.cpp $(OBJ)/* -o $(OUT)/tk_donut6_cpq_prob.out -ltrimesh -lgluit -fopenmp -D_PROB -D_DNT -D_CPQ #-DVERBOSE=1 -DTEST_MODE

# # tk_donut3_cpq_prob: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query_tk.cpp $(OBJ)/* -o $(OUT)/tk_donut3_cpq_prob.out -ltrimesh -lgluit -fopenmp -D_PROB -D_DNT -D_IDX3 -D_CPQ #-DVERBOSE=1 -DTEST_MODE

# # tk_3nn6_cpq_prob: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query_tk.cpp $(OBJ)/* -o $(OUT)/tk_3nn6_cpq_prob.out -ltrimesh -lgluit -fopenmp -D_PROB -D_3NN -D_CPQ #-DVERBOSE=1 -DTEST_MODE

# # tk_3nn3_cpq_prob: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query_tk.cpp $(OBJ)/* -o $(OUT)/tk_3nn3_cpq_prob.out -ltrimesh -lgluit -fopenmp -D_PROB -D_3NN -D_IDX3 -D_CPQ #-DVERBOSE=1 -DTEST_MODE


# tk_query_donut_prob3: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/tk_query_donut_prob3.out  -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_DNT -D_IDX3 -D_CLR -D_TK

# tk_query_prob3: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/tk_query_prob3.out        -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_IDX3 -D_CLR -D_TK

# tk_query_3nn_prob3: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/tk_query_3nn_prob3.out    -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_IDX3 -D_3NN -D_CLR -D_TK


# query_prob3: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_prob3.out           -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_IDX3 -D_CLR #-DVERBOSE=2 #-DTEST_MODE #-g

# query_prob3_cpq: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_prob3_cpq.out       -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_IDX3 -D_CPQ #-DVERBOSE=5 -DTEST_MODE -g

# query_prob6: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_prob6.out           -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC #-DVERBOSE=1 -DTEST_MODE -g

# query_prob6_cpq: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_prob6_cpq.out       -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_CPQ #-DVERBOSE=1 -DTEST_MODE -g


# query_donut_prob3: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_donut_prob3.out     -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_DNT -D_IDX3 -D_CLR -g #-DVERBOSE=1 #-DTEST_MODE

# query_donut_prob3_cpq: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_donut_prob3_cpq.out -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_DNT -D_CPQ -D_IDX3 -D_CLR -g #-DVERBOSE=0 #-DTEST_MODE

# query_donut_prob6: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_donut_prob6.out     -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_DNT -g #-DVERBOSE=0 -DTEST_MODE

# query_donut_prob6_cpq: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_donut_prob6_cpq.out -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_DNT -D_CPQ -g #-DVERBOSE=0 #-DTEST_MODE


# query_3nn_prob3: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_3nn_prob3.out       -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_IDX3 -D_3NN -D_CLR #-DVERBOSE=1 #-DTEST_MODE -g

# query_3nn_prob3_cpq: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_3nn_prob3_cpq.out   -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_IDX3 -D_3NN -D_CPQ #-DVERBOSE=5 -DTEST_MODE -g

# query_3nn_prob6: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_3nn_prob6.out       -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_3NN #-DVERBOSE=1 #-DTEST_MODE -g

# query_3nn_prob6_cpq: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_3nn_prob6_cpq.out   -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_3NN -D_CPQ #-DVERBOSE=1 #-DTEST_MODE -g


# query_3lnn_prob3: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_3lnn_prob3.out      -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_IDX3 -D_3NN -D_3LNN #-DVERBOSE=1 -DTEST_MODE -g

# query_3lnn_prob3_cpq: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/query.cpp $(OBJ)/* -o $(OUT)/query_3lnn_prob3_cpq.out  -ltrimesh -lgluit -fopenmp -D_PROB -D_ACC -D_IDX3 -D_3NN -D_3LNN -D_CPQ #-DVERBOSE=1 #-DTEST_MODE -g


# analyze: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/analyze.cpp $(OBJ)/* -o $(OUT)/analyze.out -ltrimesh -lgluit -fopenmp

# anal_reg: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/anal_reg.cpp $(OBJ)/* -o $(OUT)/anal_reg.out -ltrimesh -lgluit -fopenmp

# anal_nn: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/anal_nn.cpp $(OBJ)/* -o $(OUT)/anal_nn.out -ltrimesh -lgluit -fopenmp

# anal_q: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/anal_q.cpp $(OBJ)/* -o $(OUT)/anal_q.out -ltrimesh -lgluit -fopenmp

# anal_diam: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/anal_diam.cpp $(OBJ)/* -o $(OUT)/anal_diam.out -ltrimesh -lgluit -fopenmp

# z_test: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/z_test.cpp $(OBJ)/* -o $(OUT)/z_test.out -ltrimesh -lgluit -fopenmp

# # binary_helper: build_obj
# # 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/binary_helper.cpp $(OBJ)/* -o $(OUT)/binary_helper.out -ltrimesh -lgluit -fopenmp #-g


# goicp: build_obj
# 	$(CXX) $(INC) $(USE_TM) $(USE_RS) $(SRC)/goicp.cpp $(OBJ)/* -o $(OUT)/goicp.out -ltrimesh -lgluit -fopenmp #-g


# build_obj: clean trans.o tetra_meas.o c_rtree.o mesh.o util.o

# trans.o: $(OBJ)/
# 	$(CXX) $(INC) $(USE_EIGEN) $(USE_TM) -c $(SRC)/trans.cpp -o $(OBJ)/trans.o

# tetra_meas.o: $(OBJ)/
# 	$(CXX) $(INC) $(USE_EIGEN) $(USE_TM) -c $(SRC)/tetra_meas.cpp -o $(OBJ)/tetra_meas.o

# c_rtree.o: $(OBJ)/
# 	$(CXX) $(INC) $(USE_EIGEN) $(USE_TM) -I../rstree/ -c $(SRC)/c_rtree.cpp -o $(OBJ)/c_rtree.o

# mesh.o: $(OBJ)/
# 	$(CXX) $(INC) $(USE_EIGEN) $(USE_TM) -c $(SRC)/mesh.cpp -o $(OBJ)/mesh.o

# util.o: $(OBJ)/
# 	$(CXX) $(INC) -c $(SRC)/util.cpp -o $(OBJ)/util.o

# $(OBJ)/:
# 	$(shell mkdir -p $(OBJ))

# clean:
# 	$(RMDIR) $(OBJ)/
# 	$(RM) $(OUT)/*.out