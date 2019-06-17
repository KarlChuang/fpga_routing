math=-lm
def=
opts=-O3

def=
ifeq ("$(PLATFORM)","SCO_UNIX")
  def='-dy -K PIC -DNOLONGLONG'
  dl=-lc
else
  dl=-ldl
endif

all: main

main: src/main/main.o src/ILP/createModel.o src/graph/graph.o src/graph/iostream.o src/graph/advancedTraverse.o lp_solve/lp_MDO.o lp_solve/shared/commonlib.o lp_solve/colamd/colamd.o lp_solve/shared/mmio.o lp_solve/shared/myblas.o lp_solve/ini.o lp_solve/lp_rlp.o lp_solve/lp_crash.o lp_solve/bfp/bfp_LUSOL/lp_LUSOL.o lp_solve/bfp/bfp_LUSOL/LUSOL/lusol.o lp_solve/lp_Hash.o lp_solve/lp_lib.o lp_solve/lp_wlp.o lp_solve/lp_matrix.o lp_solve/lp_mipbb.o lp_solve/lp_MPS.o lp_solve/lp_params.o lp_solve/lp_presolve.o lp_solve/lp_price.o lp_solve/lp_pricePSE.o lp_solve/lp_report.o lp_solve/lp_scale.o lp_solve/lp_simplex.o lp_solve/lp_SOS.o lp_solve/lp_utils.o lp_solve/yacc_read.c
	g++ $(opts) $(def) -DYY_NEVER_INTERACTIVE -DPARSER_LP -DINVERSE_ACTIVE=INVERSE_LUSOL -DRoleIsExternalInvEngine $^ -o bin/main $(math) $(dl)

src/main/main.o: src/main/main.cpp
	g++ -Iinclude -Ilp_solve -Ilp_solve/bfp -Ilp_solve/bfp/bfp_LUSOL -Ilp_solve/bfp/bfp_LUSOL/LUSOL -Ilp_solve/colamd -Ilp_solve/shared $(opts) $(def) -DYY_NEVER_INTERACTIVE -DPARSER_LP -DINVERSE_ACTIVE=INVERSE_LUSOL -DRoleIsExternalInvEngine -c $^ -o $@

src/graph/graph.o: src/graph/graph.cpp include/graph.h
	g++ -I include -c src/graph/graph.cpp -o src/graph/graph.o

src/graph/iostream.o: src/graph/iostream.cpp include/graph.h
	g++ -I include -c src/graph/iostream.cpp -o src/graph/iostream.o

src/graph/advancedTraverse.o: src/graph/advancedTraverse.cpp include/graph.h
	g++ -I include -c src/graph/advancedTraverse.cpp -o src/graph/advancedTraverse.o

src/ILP/createModel.o: src/ILP/createModel.cpp include/ILP.h
	g++ -Iinclude -Ilp_solve -Ilp_solve/bfp -Ilp_solve/bfp/bfp_LUSOL -Ilp_solve/bfp/bfp_LUSOL/LUSOL -Ilp_solve/colamd -Ilp_solve/shared $(opts) $(def) -DYY_NEVER_INTERACTIVE -DPARSER_LP -DINVERSE_ACTIVE=INVERSE_LUSOL -DRoleIsExternalInvEngine -c src/ILP/createModel.cpp -o $@

%.o: %.c
	g++ -Ilp_solve -Ilp_solve/bfp -Ilp_solve/bfp/bfp_LUSOL -Ilp_solve/bfp/bfp_LUSOL/LUSOL -Ilp_solve/colamd -Ilp_solve/shared $(opts) $(def) -DYY_NEVER_INTERACTIVE -DPARSER_LP -DINVERSE_ACTIVE=INVERSE_LUSOL -DRoleIsExternalInvEngine -c $^ -o $@

clean:
	@rm -rf ./**/*o
	@rm -rf ./**/**/*o
