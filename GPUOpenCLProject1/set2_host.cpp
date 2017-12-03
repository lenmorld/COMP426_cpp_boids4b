// Add you host code

#include<stdio.h>
#include <iostream>
#include<CL/cl.h>

int main2(void)
{
	cl_int err;
	cl_uint numPlatforms;

	err = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (CL_SUCCESS == err)
		printf("\nDetected OpenCL platforms: %d", numPlatforms);
	else
		printf("\nError calling clGetPlatformIDs. Error code: %d", err);

	cl_uint num_devices_returned;
	cl_device_id devices[2];

	//err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1,
	//	&devices[0], &num_devices_returned);

	//if (CL_SUCCESS == err)
	//	printf("\nDetected GPUs : %d", num_devices_returned);
	//else
	//	printf("\nError calling clGetDeviceIDs. Error code: %d", err);


	err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_CPU, 1,
		&devices[1], &num_devices_returned);

	if (CL_SUCCESS == err)
		printf("\nDetected CPUs : %d", num_devices_returned);
	else
		printf("\nError calling clGetDeviceIDs. Error code: %d", err);


	//cl_device_id device_id;
	//cl_uint maxComputeUnits;

	//err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, 1, &device_id, NULL);






	int m;
	std::cin >> m;

	return 0;
}