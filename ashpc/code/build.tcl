open_project -reset spmv_hls

set_top kernel
add_files     ../code/kernel.cpp
add_files     ../code/kernel.h
add_files -tb ../code/testbench.cpp -cflags "-Wl,--stack,10485760"

add_files -tb ../code/dwt_512.mtx

open_solution "solution" -reset

set_part {xcu250-figd2104-2L-e}
create_clock -period 10 -name default

config_flow -target vitis

csim_design
csynth_design

exit
