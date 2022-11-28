-makelib xcelium_lib/xpm -sv \
  "F:/xilinx/Vivado/2022.1/data/ip/xpm/xpm_cdc/hdl/xpm_cdc.sv" \
  "F:/xilinx/Vivado/2022.1/data/ip/xpm/xpm_memory/hdl/xpm_memory.sv" \
-endlib
-makelib xcelium_lib/xpm \
  "F:/xilinx/Vivado/2022.1/data/ip/xpm/xpm_VCOMP.vhd" \
-endlib
-makelib xcelium_lib/xil_defaultlib \
  "../../../../complete.gen/sources_1/ip/mmcm_fpro/mmcm_fpro_clk_wiz.v" \
  "../../../../complete.gen/sources_1/ip/mmcm_fpro/mmcm_fpro.v" \
-endlib
-makelib xcelium_lib/xil_defaultlib \
  glbl.v
-endlib

