#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#include <CL/cl2.hpp>

#include <utility>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <iterator>

// Hello World will be used repeatedly, so defining a const for it
const std::string hw("Hello World\n");

// Not going to worry about error recovery
inline void
checkErr(cl_int err, const char* name)
{
    if (err != CL_SUCCESS)
    {
        std::cerr << "ERROR: " << name
                    << " (code: " << err << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int
main()
{
    cl_int err;
    std::vector<cl::Platform> platformList;
    cl::Platform::get(&platformList);
    // Ternary operator:  boolean expression ? do if true : do if false
    checkErr(platformList.size()==0 ? -1 : CL_SUCCESS, "No OpenCL Platforms Found");
    std::cerr << "Number of platforms is: " << platformList.size() << std::endl;

    std::string platformVendor;
    platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
    std::cerr << "Platform vendor is: " << platformVendor << '\n';
    cl_context_properties cprops[3] = 
        {CL_CONTEXT_PLATFORM,
        (cl_context_properties)(platformList[0])(),
        0};
    
    cl::Context context(CL_DEVICE_TYPE_CPU, cprops, NULL, NULL, &err);
    checkErr(err, "Context definition");

    // Allocate buffer to hold kernel result
    //TODO: Understand this
    // The +1 is for the NULL terminator
    char* outH = new char[hw.length()+1];
    cl::Buffer outCL(
        context,
        CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
        hw.length()+1,
        outH,
        &err);
    checkErr(err, "Buffer allocation");

    // Get list of devices from Context
    cl::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
    checkErr(devices.size()>0 ? CL_SUCCESS : -1, "No devices found in Context");

    std::ifstream file("lesson1_kernels.cl");
    checkErr(file.is_open() ? CL_SUCCESS : -1, "Couldn't load lesson1_kernels.cl");

    //TODO: understand wtf is going on here
    // Load kernel source file line by line
    std::string prog(std::istreambuf_iterator<char>(file),
                     (std::istreambuf_iterator<char>()));
    //cl::Program::Sources source(prog);

    cl::Program program(context, prog);
    err = program.build(devices,"");
    checkErr(file.is_open() ? CL_SUCCESS : -1, "Program build failure");

    std::cout << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0], &err) << std::endl;
    checkErr(err, "test");



    // Initialize the kernel
    cl::Kernel kernel(program, "hello", &err);
    checkErr(err, "Kernel initialization failed");

    err = kernel.setArg(0, outCL);
    checkErr(err, "Setting first kernel argument failed");

    // Queue the kernel for execution
    cl::CommandQueue queue(context, devices[0], 0, &err);
    checkErr(err, "Queueing the kernel failed");
    cl::Event event;
    err = queue.enqueueNDRangeKernel(
        kernel,
        cl::NullRange, //offset
        cl::NDRange(hw.length()+1), // global
        cl::NDRange(1,1), // local
        NULL,
        &event);
    
    event.wait(); // Wait until command has finished before proceeding
    err = queue.enqueueReadBuffer(
        outCL, //buffer
        CL_TRUE, // blocking
        0, //offset
        hw.length()+1, // size
        outH); // pointer
    checkErr(err, "enqueueReadBuffer Failed");
    std::cout << outH;

    return EXIT_SUCCESS;
}
