// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2022.1.2 (win64) Build 3605665 Fri Aug  5 22:53:37 MDT 2022
// Date        : Mon Nov 28 12:10:04 2022
// Host        : LAPTOP-L4R3AOA2 running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub
//               d:/final/complete/complete.gen/sources_1/ip/mmcm_fpro/mmcm_fpro_stub.v
// Design      : mmcm_fpro
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7a100tcsg324-1
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
module mmcm_fpro(clk_100M, clk_25M, clk_40M, clk_67M, reset, locked, 
  clk_in_100M)
/* synthesis syn_black_box black_box_pad_pin="clk_100M,clk_25M,clk_40M,clk_67M,reset,locked,clk_in_100M" */;
  output clk_100M;
  output clk_25M;
  output clk_40M;
  output clk_67M;
  input reset;
  output locked;
  input clk_in_100M;
endmodule
