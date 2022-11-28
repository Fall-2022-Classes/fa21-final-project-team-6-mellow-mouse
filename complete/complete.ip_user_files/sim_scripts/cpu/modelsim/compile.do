vlib modelsim_lib/work
vlib modelsim_lib/msim

vlib modelsim_lib/msim/xpm
vlib modelsim_lib/msim/microblaze_v11_0_9
vlib modelsim_lib/msim/xil_defaultlib
vlib modelsim_lib/msim/lib_cdc_v1_0_2
vlib modelsim_lib/msim/proc_sys_reset_v5_0_13
vlib modelsim_lib/msim/lmb_v10_v3_0_12
vlib modelsim_lib/msim/lmb_bram_if_cntlr_v4_0_21
vlib modelsim_lib/msim/blk_mem_gen_v8_4_5
vlib modelsim_lib/msim/iomodule_v3_1_8

vmap xpm modelsim_lib/msim/xpm
vmap microblaze_v11_0_9 modelsim_lib/msim/microblaze_v11_0_9
vmap xil_defaultlib modelsim_lib/msim/xil_defaultlib
vmap lib_cdc_v1_0_2 modelsim_lib/msim/lib_cdc_v1_0_2
vmap proc_sys_reset_v5_0_13 modelsim_lib/msim/proc_sys_reset_v5_0_13
vmap lmb_v10_v3_0_12 modelsim_lib/msim/lmb_v10_v3_0_12
vmap lmb_bram_if_cntlr_v4_0_21 modelsim_lib/msim/lmb_bram_if_cntlr_v4_0_21
vmap blk_mem_gen_v8_4_5 modelsim_lib/msim/blk_mem_gen_v8_4_5
vmap iomodule_v3_1_8 modelsim_lib/msim/iomodule_v3_1_8

vlog -work xpm  -incr -mfcu -sv \
"F:/xilinx/Vivado/2022.1/data/ip/xpm/xpm_memory/hdl/xpm_memory.sv" \

vcom -work xpm  -93 \
"F:/xilinx/Vivado/2022.1/data/ip/xpm/xpm_VCOMP.vhd" \

vcom -work microblaze_v11_0_9  -93 \
"../../../ipstatic/hdl/microblaze_v11_0_vh_rfs.vhd" \

vcom -work xil_defaultlib  -93 \
"../../../../complete.gen/sources_1/ip/cpu/bd_0/ip/ip_0/sim/bd_3914_microblaze_I_0.vhd" \

vcom -work lib_cdc_v1_0_2  -93 \
"../../../ipstatic/hdl/lib_cdc_v1_0_rfs.vhd" \

vcom -work proc_sys_reset_v5_0_13  -93 \
"../../../ipstatic/hdl/proc_sys_reset_v5_0_vh_rfs.vhd" \

vcom -work xil_defaultlib  -93 \
"../../../../complete.gen/sources_1/ip/cpu/bd_0/ip/ip_1/sim/bd_3914_rst_0_0.vhd" \

vcom -work lmb_v10_v3_0_12  -93 \
"../../../ipstatic/hdl/lmb_v10_v3_0_vh_rfs.vhd" \

vcom -work xil_defaultlib  -93 \
"../../../../complete.gen/sources_1/ip/cpu/bd_0/ip/ip_2/sim/bd_3914_ilmb_0.vhd" \
"../../../../complete.gen/sources_1/ip/cpu/bd_0/ip/ip_3/sim/bd_3914_dlmb_0.vhd" \

vcom -work lmb_bram_if_cntlr_v4_0_21  -93 \
"../../../ipstatic/hdl/lmb_bram_if_cntlr_v4_0_vh_rfs.vhd" \

vcom -work xil_defaultlib  -93 \
"../../../../complete.gen/sources_1/ip/cpu/bd_0/ip/ip_4/sim/bd_3914_dlmb_cntlr_0.vhd" \
"../../../../complete.gen/sources_1/ip/cpu/bd_0/ip/ip_5/sim/bd_3914_ilmb_cntlr_0.vhd" \

vlog -work blk_mem_gen_v8_4_5  -incr -mfcu \
"../../../ipstatic/simulation/blk_mem_gen_v8_4.v" \

vlog -work xil_defaultlib  -incr -mfcu \
"../../../../complete.gen/sources_1/ip/cpu/bd_0/ip/ip_6/sim/bd_3914_lmb_bram_I_0.v" \

vcom -work iomodule_v3_1_8  -93 \
"../../../ipstatic/hdl/iomodule_v3_1_vh_rfs.vhd" \

vcom -work xil_defaultlib  -93 \
"../../../../complete.gen/sources_1/ip/cpu/bd_0/ip/ip_7/sim/bd_3914_iomodule_0_0.vhd" \

vlog -work xil_defaultlib  -incr -mfcu \
"../../../../complete.gen/sources_1/ip/cpu/bd_0/sim/bd_3914.v" \
"../../../../complete.gen/sources_1/ip/cpu/sim/cpu.v" \

vlog -work xil_defaultlib \
"glbl.v"

