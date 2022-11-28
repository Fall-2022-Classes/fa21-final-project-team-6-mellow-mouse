-- Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2022.1.2 (win64) Build 3605665 Fri Aug  5 22:53:37 MDT 2022
-- Date        : Mon Nov 28 12:10:04 2022
-- Host        : LAPTOP-L4R3AOA2 running 64-bit major release  (build 9200)
-- Command     : write_vhdl -force -mode synth_stub
--               d:/final/complete/complete.gen/sources_1/ip/mmcm_fpro/mmcm_fpro_stub.vhdl
-- Design      : mmcm_fpro
-- Purpose     : Stub declaration of top-level module interface
-- Device      : xc7a100tcsg324-1
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity mmcm_fpro is
  Port ( 
    clk_100M : out STD_LOGIC;
    clk_25M : out STD_LOGIC;
    clk_40M : out STD_LOGIC;
    clk_67M : out STD_LOGIC;
    reset : in STD_LOGIC;
    locked : out STD_LOGIC;
    clk_in_100M : in STD_LOGIC
  );

end mmcm_fpro;

architecture stub of mmcm_fpro is
attribute syn_black_box : boolean;
attribute black_box_pad_pin : string;
attribute syn_black_box of stub : architecture is true;
attribute black_box_pad_pin of stub : architecture is "clk_100M,clk_25M,clk_40M,clk_67M,reset,locked,clk_in_100M";
begin
end;
