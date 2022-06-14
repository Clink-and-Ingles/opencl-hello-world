hello_world: main.cpp lesson1_kernels.cl
	gcc -o hello_world -I/usr/include/CL -L/usr/lib/x86_64-linux-gnu/libOpenCL.so -L/home/liam/gcc-11.2.0/lib64/libstdc++.so main.cpp -lOpenCL -lstdc++

