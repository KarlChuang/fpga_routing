math=-lm
def=
opts=-g -Wall -std=c++98 -DTA_KB_SETTING

def=
ifeq ("$(PLATFORM)","SCO_UNIX")
  def='-dy -K PIC -DNOLONGLONG'
  dl=-lc
else
  dl=-ldl
endif

all: main

main: src/main/main.o src/convex/solveConvex.o src/ILP/demo.o src/ILP/solveILP.o src/ILP/adaptILP.o src/graph/graph.o src/graph/iostream.o src/graph/advancedTraverse.o src/graph/utils.o lp_solve/lp_MDO.o lp_solve/shared/commonlib.o lp_solve/colamd/colamd.o lp_solve/shared/mmio.o lp_solve/shared/myblas.o lp_solve/ini.o lp_solve/lp_rlp.o lp_solve/lp_crash.o lp_solve/bfp/bfp_LUSOL/lp_LUSOL.o lp_solve/bfp/bfp_LUSOL/LUSOL/lusol.o lp_solve/lp_Hash.o lp_solve/lp_lib.o lp_solve/lp_wlp.o lp_solve/lp_matrix.o lp_solve/lp_mipbb.o lp_solve/lp_MPS.o lp_solve/lp_params.o lp_solve/lp_presolve.o lp_solve/lp_price.o lp_solve/lp_pricePSE.o lp_solve/lp_report.o lp_solve/lp_scale.o lp_solve/lp_simplex.o lp_solve/lp_SOS.o lp_solve/lp_utils.o lp_solve/yacc_read.o
	g++ $(opts) $(def) -DYY_NEVER_INTERACTIVE -DPARSER_LP -DINVERSE_ACTIVE=INVERSE_LUSOL -DRoleIsExternalInvEngine $^ -o bin/main $(math) $(dl)

src/main/main.o: src/main/main.cpp include/graph.h
	g++ -Iinclude -Ilp_solve -Ilp_solve/bfp -Ilp_solve/bfp/bfp_LUSOL -Ilp_solve/bfp/bfp_LUSOL/LUSOL -Ilp_solve/colamd -Ilp_solve/shared $(opts) $(def) -DYY_NEVER_INTERACTIVE -DPARSER_LP -DINVERSE_ACTIVE=INVERSE_LUSOL -DRoleIsExternalInvEngine -c $< -o $@

src/graph/%.o: src/graph/%.cpp include/graph.h
	g++ -Iinclude $(opts) $(def) -c $< -o $@

src/ILP/%.o: src/ILP/%.cpp include/graph.h
	g++ -Iinclude -Ilp_solve -Ilp_solve/bfp -Ilp_solve/bfp/bfp_LUSOL -Ilp_solve/bfp/bfp_LUSOL/LUSOL -Ilp_solve/colamd -Ilp_solve/shared $(opts) $(def) -DYY_NEVER_INTERACTIVE -DPARSER_LP -DINVERSE_ACTIVE=INVERSE_LUSOL -DRoleIsExternalInvEngine -c $< -o $@

src/convex/%.o: src/convex/%.cpp include/graph.h
	g++ -I./Eigen -Iinclude -c $< -o $@

%.o: %.c
	g++ -Ilp_solve -Ilp_solve/bfp -Ilp_solve/bfp/bfp_LUSOL -Ilp_solve/bfp/bfp_LUSOL/LUSOL -Ilp_solve/colamd -Ilp_solve/shared $(opts) $(def) -DYY_NEVER_INTERACTIVE -DPARSER_LP -DINVERSE_ACTIVE=INVERSE_LUSOL -DRoleIsExternalInvEngine -c $^ -o $@

clean:
	@rm -rf ./src/**/*o

clean_all:
	@rm -rf ./src/**/*o
	@rm -rf ./**/*o
	@rm -rf ./**/**/*o
	@rm -rf ./**/**/**/*o
	@rm -rf ./**/**/**/**/*o
