############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project bg
set_top bg
add_files ../src/bilateral_grid.cpp -cflags "-O3"
open_solution "solution1"
set_part {xczu7ev-ffvc1156-2-e}
create_clock -period 4 -name default
csynth_design
export_design -rtl verilog -format ip_catalog
exit
