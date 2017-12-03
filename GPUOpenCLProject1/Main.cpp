#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

int main(void) {
	// Create the two input vectors
	int i;
	const int LIST_SIZE = 1024;
	int *A = (int*)malloc(sizeof(int)*LIST_SIZE);
	int *B = (int*)malloc(sizeof(int)*LIST_SIZE);
	for (i = 0; i < LIST_SIZE; i++) {
		A[i] = i;
		B[i] = LIST_SIZE - i;
	}

	//int m1;
	//std::cin >> m1;

	// Load the kernel source code into the array source_str
	FILE *fp;
	char *source_str;
	size_t source_size;

	fp = fopen("template.cl", "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);


	//====================


	// ================================================

	// Get platform and device information
	cl_platform_id platform_id = NULL;				// Get platform ID, hopefully only one platform that contains both CPU and GPU
	//cl_platform_id platform_id2 = NULL;

	//cl_device_id device_id_gpu = NULL;
	////cl_device_id device_id_cpu = NULL;

	cl_device_id devices[2];				// put in an array to create a shared context

	cl_uint ret_num_devices;

	cl_uint ret_num_platforms;
	cl_uint ret_num_platforms2;

	// ONLY ONE PLATFORM
	cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

	//cl_int ret2 = clGetPlatformIDs(1, &platform_id2, &ret_num_platforms2);

	//ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1,
	//	&device_id, &ret_num_devices);

	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1,
		&devices[0], &ret_num_devices);												// only 1 for HP laptop

	if (CL_SUCCESS == ret)
		printf("\nDetected GPU device : %d \n", ret_num_devices);
	else
		printf("\nError calling clGetDeviceIDs. Error code: %d", ret);

	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1,
		&devices[1], &ret_num_devices);

	if (CL_SUCCESS == ret)
		printf("\nDetected CPU device: %d \n", ret_num_devices);
	else
		printf("\nError calling clGetDeviceIDs. Error code: %d", ret);

	// Create an OpenCL context
	//cl_context context = clCreateContext(NULL, 1, &device_id_gpu, NULL, NULL, &ret);
	cl_context context = clCreateContext(NULL, 2, devices, NULL, NULL, &ret);					// 1 context with both GPU and CPU

	//cl_context context2 = clCreateContext(NULL, 1, &device_id_cpu, NULL, NULL, &ret2);		// 1 device


	// Create a command queue
	cl_command_queue command_queue_gpu = clCreateCommandQueue(context, devices[0], 0, &ret);				// GPU
	cl_command_queue command_queue_cpu = clCreateCommandQueue(context, devices[0], 0, &ret);				// CPU

	// Create memory buffers on the device for each vector 
	cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
		LIST_SIZE * sizeof(int), NULL, &ret);
	cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
		LIST_SIZE * sizeof(int), NULL, &ret);
	cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,				// result
		LIST_SIZE * sizeof(int), NULL, &ret);

	// Copy the lists A and B to their respective memory buffers
	ret = clEnqueueWriteBuffer(command_queue_gpu, a_mem_obj, CL_TRUE, 0,				// or command_queue_cpu
		LIST_SIZE * sizeof(int), A, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue_gpu, b_mem_obj, CL_TRUE, 0,
		LIST_SIZE * sizeof(int), B, 0, NULL, NULL);

	// Create a program from the kernel source
	cl_program program = clCreateProgramWithSource(context, 1,
		(const char **)&source_str, (const size_t *)&source_size, &ret);

	// Build the program
	//ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	ret = clBuildProgram(program, 1, &devices[0], NULL, NULL, NULL);				// build for GPU 

	// Create the OpenCL kernel
	cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);

	// Set the arguments of the kernel
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);

	// Execute the OpenCL kernel on the list
	size_t global_item_size = LIST_SIZE;			// Process the entire lists
	size_t local_item_size = 64;				// Divide work items into groups of 64

	ret = clEnqueueNDRangeKernel(command_queue_gpu, kernel, 1, NULL,
		&global_item_size, &local_item_size, 0, NULL, NULL);

	// Read the memory buffer C on the device to the local variable C
	int *C = (int*)malloc(sizeof(int)*LIST_SIZE);
	ret = clEnqueueReadBuffer(command_queue_gpu, c_mem_obj, CL_TRUE, 0,
		LIST_SIZE * sizeof(int), C, 0, NULL, NULL);

	// Display the result to the screen
	for (i = 0; i < LIST_SIZE; i++)
		printf("%d + %d = %d\n", A[i], B[i], C[i]);

	// Clean up
	ret = clFlush(command_queue_gpu);
	ret = clFinish(command_queue_gpu);

	ret = clFlush(command_queue_cpu);
	ret = clFinish(command_queue_cpu);


	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(a_mem_obj);
	ret = clReleaseMemObject(b_mem_obj);
	ret = clReleaseMemObject(c_mem_obj);

	ret = clReleaseCommandQueue(command_queue_gpu);
	ret = clReleaseCommandQueue(command_queue_cpu);

	ret = clReleaseContext(context);
	//ret2 = clReleaseContext(context2);

	free(A);
	free(B);
	free(C);

	int m;
	std::cin >> m;

	return 0;
}