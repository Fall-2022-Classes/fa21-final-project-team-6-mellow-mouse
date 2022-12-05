# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct D:\final\vitis\mcs_top_complete\platform.tcl
# 
# OR launch xsct and run below command.
# source D:\final\vitis\mcs_top_complete\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {mcs_top_complete}\
-hw {D:\final\complete\mcs_top_complete.xsa}\
-out {D:/final/vitis}

platform write
domain create -name {standalone_microblaze_I} -display-name {standalone_microblaze_I} -os {standalone} -proc {microblaze_I} -runtime {cpp} -arch {32-bit} -support-app {empty_application}
platform generate -domains 
platform active {mcs_top_complete}
platform generate -quick
platform generate
platform config -updatehw {D:/final/complete/mcs_top_complete.xsa}
platform active {mcs_top_complete}
platform config -updatehw {D:/final/complete/mcs_top_complete.xsa}
platform generate
platform active {mcs_top_complete}
platform config -updatehw {D:/final/complete/mcs_top_complete.xsa}
platform generate -domains 
platform clean
platform generate
platform active {mcs_top_complete}
platform config -updatehw {D:/final/complete/mcs_top_complete.xsa}
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform config -updatehw {D:/final/complete/mcs_top_complete.xsa}
platform clean
platform generate
platform config -updatehw {D:/final/complete/mcs_top_complete.xsa}
platform clean
platform generate
platform generate
platform active {mcs_top_complete}
platform config -updatehw {D:/final/complete/mcs_top_complete.xsa}
platform generate -domains 
platform clean
platform generate
platform generate
