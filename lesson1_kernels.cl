#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
constant char hw[] = "Hello World\n";
kernel void hello(global char* out)
{
    size_t tid = get_global_id(0); // thread id
    out[tid] = hw[tid];
}