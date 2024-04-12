#include "iostream"
#include "fstream"
#include "iterator"
#include "vector"
#include "stdlib.h"
#include "stdio.h"

// OpenCL API 
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include "CL/cl2.hpp"
#include "CL/cl_ext_xilinx.h"


std::vector<unsigned char>
ReadBinaryFile(const std::string &Filename)
{
    std::cout << "INFO: Reading " << Filename << std::endl;
    std::ifstream File(Filename, std::ios::binary);

    if(!File.is_open())
    {
        std::cerr << "ERROR: Failed to open file: " << Filename << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "INFO: File opened successfully" << std::endl;
    }
    
    File.unsetf(std::ios::skipws);


    std::streampos FileSize;
    File.seekg(0, std::ios::end);
    FileSize = File.tellg();
    File.seekg(0, std::ios::beg);

    std::cout << "INFO: File size: " << FileSize << std::endl;

    std::vector<unsigned char> Data;
    Data.reserve(FileSize);
    Data.insert(Data.begin(),
        std::istream_iterator<unsigned char>(File),
        std::istream_iterator<unsigned char>());

    return Data;
}

cl::Platform
GetPlatform()
{
    std::vector<cl::Platform> Platforms;
    cl::Platform::get(&Platforms);
    cl::Platform Platform;

    for(cl::Platform &P: Platforms)
    {
        const std::string Name = P.getInfo<CL_PLATFORM_NAME>();
        std::cout << "PLATFORM: " << Name << std::endl;
        if(Name == "Xilinx")
        {
            Platform = P;
            break;
        }
    }

    if(Platform == cl::Platform())
    {
        std::cout << "Xilinx platform not found!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return Platform;
}

cl::Device
GetDevice(cl::Platform *Platform, const char DeviceName[])
{
    std::vector<cl::Device> Devices;
    Platform->getDevices(CL_DEVICE_TYPE_ACCELERATOR, &Devices);
    
    std::cout << "Number of devices found: " << Devices.size() << std::endl;

    cl::Device Device;
    for(cl::Device &IterDevice: Devices){
        std::cout << "DEVICE: " << IterDevice.getInfo<CL_DEVICE_NAME>() << std::endl;
        if(IterDevice.getInfo<CL_DEVICE_NAME>() == DeviceName)
            Device = IterDevice;
    }
    
    std::cout << "SELECTED DEVICE: " << Device.getInfo<CL_DEVICE_NAME>() << std::endl;

    return Device;
}

cl::Context
CreateContext(cl::Device *Device)
{
    cl_int Error;
    cl::Context Context(*Device, nullptr, nullptr, nullptr, &Error);
    if(Error)
    {
        std::cout << "CONTEXT ERROR: " << Error << std::endl;
        exit(EXIT_FAILURE);
    }

    return Context;
}

cl::CommandQueue
CreateCommandQueue(cl::Context *Context, cl::Device *Device)
{
    cl_int Error;
    cl::CommandQueue Queue(*Context, *Device, CL_QUEUE_PROFILING_ENABLE, &Error);
    if(Error)
    {
        std::cout << "COMMAND QUEUE ERROR: " << Error << std::endl;
        exit(EXIT_FAILURE);
    }

    return Queue;
}

cl::Program
CreateProgram(const char KernelFilePath[], cl::Device *Device, cl::Context *Context)
{
    cl_int Error;
    auto ProgramBinary = ReadBinaryFile(KernelFilePath);
    std::cout << "PROGRAM BINARY SIZE: " << ProgramBinary.size() << std::endl;
    cl::Program::Binaries Bins{{ProgramBinary.data(), ProgramBinary.size()}};
    std::cout << "Trying to program device: " << Device->getInfo<CL_DEVICE_NAME>() << std::endl;
    cl::Program Program(*Context, {*Device}, Bins, nullptr, &Error);
    if(Error != CL_SUCCESS) {
        std::cout << "Failed to program device with xclbin file!\n";
        std::cout << Error << std::endl;
        exit(EXIT_FAILURE);
    }

    return Program;
}

cl::Kernel
CreateKernel(cl::Program *Program, const char KernelName[])
{
    cl_int Error;
    cl::Kernel Kernel= cl::Kernel(*Program, KernelName, &Error);
    if(Error)
    {
        std::cout << "KERNEL ERROR: " << Error << std::endl;
        exit(EXIT_FAILURE);
    }

    return Kernel;
}