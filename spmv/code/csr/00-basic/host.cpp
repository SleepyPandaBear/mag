#include "iostream"
#include "fstream"
#include "iterator"
#include "vector"
#include "stdlib.h"
#include "stdio.h"

// XRT includes
#include "xrt/xrt_bo.h" // type:ignore
#include "experimental/xrt_xclbin.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"

// OpenCL API 
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include "CL/cl2.hpp"
#include "CL/cl_ext_xilinx.h"

#include "cl_helpers.cpp"

#include "../../../sparse_matrix_converter/code/converter_include_all.h"


#define DATA_SIZE 1024
#define KERNEL_CL "smpv"

constexpr char KERNEL_FILE_PATH[] = "spmv.xclbin";
// PLATFORM=xilinx_u250_gen3x16_xdma_4_1_202210_1
// PLATFORM=xilinx_u280_gen3x16_xdma_1_202211_1
// constexpr char PLATFORM[] = "xilinx_u250_gen3x16_xdma_4_1_202210_1";
constexpr char PLATFORM[] = "xilinx_u280_gen3x16_xdma_1_202211_1";

int 
main(int ArgumentCount, char** ArgumentValues) 
{
    // TODO: One argument for matrix market file, another one for block size if applicable.
    // cl_int Error;

    if(ArgumentCount != 2)
    {
        printf("Wrong arguments, use program the following way: ./host path_to_mm_file");
        return 1;
    }

    char *MatrixMarketFilePath = ArgumentValues[1];



    int a[DATA_SIZE];
    int b[DATA_SIZE];
    int c[DATA_SIZE];

    for (int i = 0; i < DATA_SIZE; i++) {
        a[i] = 1;
        b[i] = 1;
        c[i] = 0;
    }

    cl::Platform Platform = GetPlatform();
    cl::Device Device = GetDevice(&Platform, PLATFORM);
    cl::Context Context = CreateContext(&Device);
    cl::CommandQueue CommandQueue = CreateCommandQueue(&Context, &Device);

    // Create device buffers
    // cl::Buffer buffer_a(Context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, DATA_SIZE * sizeof(int), a, &Error);    
    // cl::Buffer buffer_b(Context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, DATA_SIZE * sizeof(int), b, &Error);    
    // cl::Buffer buffer_res(Context,  CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, DATA_SIZE * sizeof(int), c, &Error);

    cl::Program Program = CreateProgram(KERNEL_FILE_PATH, &Device, &Context);
    cl::Kernel Kernel = CreateKernel(&Program, "spmv");

    // Write host data to device
    // CommandQueue.enqueueMigrateMemObjects({buffer_a, buffer_b}, 0 );

    // Kernel.setArg(0, buffer_res);
    // Kernel.setArg(1, buffer_a);
    // Kernel.setArg(2, buffer_b);
    // Kernel.setArg(3, DATA_SIZE);

    // Enqueue the kernel for execution
    CommandQueue.enqueueTask(Kernel);

    // Read the output buffer back to the host
    // CommandQueue.enqueueMigrateMemObjects({buffer_res}, CL_MIGRATE_MEM_OBJECT_HOST);
    CommandQueue.finish();

    std::cout << "Calculating sum of resulting array: " << std::endl;
    int sum = 0;
    for (int i = 0; i < DATA_SIZE; i++){
        sum += c[i] + a[i] + b[i];
    }
    std::cout << "SUM: " << sum << std::endl;

    return 0;
}