# FPGA routing

## Introduction

Our project is developed for solving the CAD contest 2019 problem B.

## Compiling code

We use C++98 to compile the code. We use lp_solve module, which in writen in C, so there are some problems to use other version of C++ compiler. We write a Makefile, so you can just run `make` to comile it

If you successfully compile it, the binary file is `./fpga_routing/bin/main`.

## Run the code

To run the testcases, just run

`./fpga_routing/bin/main ${testing file} ${output file} ${mode}`

For example

`./fpga_routing/bin/main ./fpga_routing/test_cases/synopsys01-1.txt ./fpga_routing/output/synopsys01-1_greedy.txt greedy`

We have `greedy` and `ilp` two modes.

However, the compiling on `MacOS` and `CAD contest TSRI machine` is sccessful, but the compiling on `ubuntu 16.04` is not successful yet.

## Evaluate the output

Because the evaluater binary file is too large, you should download from http://iccad-contest.org/2019/problems.html > Contest Problems > 
Problem B >  (evaluater)

`{evaluater directory}/evaluater_V3 ${testing file} ${output file}`

## Test case

`./fpga_routing/test_cases/synopsys01-1.txt` is a small test case used in report section D.1.

- If you want to reproduce the result in D.1, you have to change the `acc_range` in `./fpga_routing/src/main/main.cpp` line 43 to `0.25`. This will build the best result in D.1 Fig.
- If you want to change the `Adding TDM ratio` policy, you should change `isSCSmaller` function in `./fpga_routing/src/ILP/adaptILP.cpp`. `Line 18-20` is adding small TDM ratio first, and `line 21-23` is adding small net group score first.
- If you want to let the lp_solve not set bounds to variables, you can change `./fpga_routing/src/ILP/solveILP.cpp`. Commanding `set_bounds` in line 69 and uncommanding line 70.

`./fpga_routing/test_cases/synopsys01.txt` is a test case used in report section D.2 and D.3.

- Becuase the file is too large, I cannot upload to ceiba, so you should download from http://iccad-contest.org/2019/problems.html > Contest Problems > 
Problem B > (test cases)
- If you want to reproduce the result in D.2, you have to change the `acc_range` in `./fpga_routing/src/main/main.cpp` line 43 to `0.11`.

- If you want to reproduce the result in D.3, you have to change the `acc_range` in `./fpga_routing/src/main/main.cpp` line 43 to the testing number.
