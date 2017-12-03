//#include <stdio.h>
//#include <stdlib.h>
//#include <iostream>
//
//#include <GL\glew.h>
//#include <GL\freeglut.h>
//
///***** boids objects *******/
//#include <vector>
//#include <thread>
//#include <string>
//#include <stdlib.h> 
//#include <math.h>
//
//#include "Flock.h"
//#include "Boid.h"
////#include "PVector.h"
//
//
//#ifdef __APPLE__
//#include <OpenCL/opencl.h>
//#else
//#include <CL/cl.h>
//#endif
//
//#define MAX_SOURCE_SIZE (0x100000)
//
///*********** program vars ********/
//
//const int BOID_PER_FLOCK = 20;
//const int FLOCKS = 7;
//const int STEP_TO_CENTER_DIVISOR = 1;		// larger -> slower
//const double SPEED_LIMITER = 0.02;			// larger-> faster
//const double SCREEN_COHESION_RADIUS = 2.0;
//const double DESIRED_DISTANCE = 0.01;
//
//// before: 1280 x 720
//int windowWidth = 1920;     // Windowed mode's width
//int windowHeight = 1080;     // Windowed mode's height
//int windowPosX = 25;      // Windowed mode's top-left corner x
//int windowPosY = 25;      // Windowed mode's top-left corner y
//
//vector<Flock> flocks;
//
//
//// do all OpenCL stuff every 30th of sec or when idle
//void computeFunction() {
//
//}
//
////void displayMe(void) {
////	glClear(GL_COLOR_BUFFER_BIT);
////	glMatrixMode(GL_MODELVIEW);    // To operate on the model-view matrix
////	glLoadIdentity();              // Reset model-view matrix
////
////	for (std::vector<int>::size_type j = 0; j < flocks.size(); j++) {
////
////		vector<GLfloat> color = colors[j];
////		glColor3f(color[0], color[1], color[2]);
////
////		glBegin(GL_TRIANGLES);
////		GLfloat scale = 0.012f;		// triangle size
////
////		vector<Boid> _boids = flocks[j].boids;
////		for (std::vector<int>::size_type i = 0; i < _boids.size(); i++) {
////			// create triagle from passed location (only 1 point, create 2 other points)
////
////			PVector p1 = _boids[i].location;
////			PVector p2 = PVector(p1.x + scale, p1.y);
////			PVector p3 = PVector(p1.x + scale, p1.y + scale);
////			glVertex2f(p1.x, p1.y);
////			glVertex2f(p2.x, p2.y);
////			glVertex2f(p3.x, p3.y);
////		}
////
////		glEnd();
////	}
////
////	glutSwapBuffers();			// Swap front and back buffers (of double buffered mode)
////}
//
///**
//* glVertex2f(float x, float y).
//* The point (0.0, 0.0) represents the middle of the window (not the top left corner).
//* The "2f" suffix means 2 values of float type (x and y).
//*/
//
//float RandomFloat(float a, float b) {
//	float random = ((float)rand()) / (float)RAND_MAX;
//	float diff = b - a;
//	float r = random * diff;
//	return a + r;
//}
//
//void displayMe(void) {
//	glClear(GL_COLOR_BUFFER_BIT);
//	glBegin(GL_POLYGON);
//	glVertex2f(0.0, 0.0);                    // bottom left
//	glVertex2f(0.5, 0.0);                    // bottom right
//	glVertex2f(0.5, 0.5);                    // top right
//	glVertex2f(0.0, 0.5);                    // top left
//	glEnd();
//	glFlush();
//}
//
//int main222(int argc, char** argv) {
//
//	// OpenGL
//
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_DOUBLE);				// Set double buffered mode
//	glutInitWindowSize(windowWidth, windowHeight);                    // window size
//	glutInitWindowPosition(windowPosX, windowPosY);                // distance from the top-left screen
//	glutCreateWindow("^^ BOIDS ^^");				// message displayed on top bar window
//
//	glPushMatrix();
//	glLoadIdentity();
//	glTranslatef(0, 0, 100);
//	glPopMatrix();
//
//	glutDisplayFunc(displayMe);
//	//glutReshapeFunc(changeSize);
//	glutIdleFunc(computeFunction);					// Register callback handler if no other event
//
//													// Set "clearing" or background color
//	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);			// Black and opaque
//
//
//	// init flocks
//	// with center in a random position
//	for (int i = 0; i < FLOCKS; i++) {
//		float r1 = RandomFloat(-0.99f, 0.99f);
//		float r2 = RandomFloat(-0.99f, 0.99f);
//		PVector loc(r1, r2);
//
//		vector<Boid> boids;
//
//		// init boids in the flock at a random position
//		for (int i = 0; i < BOID_PER_FLOCK; i++) {
//			float r1 = RandomFloat(-0.99f, 0.99f);
//			float r2 = RandomFloat(-0.99f, 0.99f);
//			PVector loc(r1, r2);
//			boids.push_back(Boid(i, loc, PVector(0.0, 0.0), PVector(RandomFloat(-0.99f, 0.99f), RandomFloat(-0.99f, 0.99f))));
//		}
//
//		flocks.push_back(Flock(i, loc, PVector(0.0, 0.0), loc, boids));
//	}
//
//	glutMainLoop();
//
//
//
//
//
//	// Create the two input vectors
//	int i;
//	const int LIST_SIZE = 1024;
//	int *A = (int*)malloc(sizeof(int)*LIST_SIZE);
//	int *B = (int*)malloc(sizeof(int)*LIST_SIZE);
//	for (i = 0; i < LIST_SIZE; i++) {
//		A[i] = i;
//		B[i] = LIST_SIZE - i;
//	}
//
//	//int m1;
//	//std::cin >> m1;
//
//	// Load the kernel source code into the array source_str
//	FILE *fp;
//	char *source_str;
//	size_t source_size;
//
//
//	//fp = fopen("template.cl", "r");
//	fp = fopen("gpu.cl", "r");
//	if (!fp) {
//		fprintf(stderr, "Failed to load kernel.\n");
//		exit(1);
//	}
//	source_str = (char*)malloc(MAX_SOURCE_SIZE);
//	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
//	fclose(fp);
//
//
//	//====================
//
//
//	// ================================================
//
//	// Get platform and device information
//	cl_platform_id platform_id = NULL;				// Get platform ID, hopefully only one platform that contains both CPU and GPU
//	//cl_platform_id platform_id2 = NULL;
//
//	//cl_device_id device_id_gpu = NULL;
//	////cl_device_id device_id_cpu = NULL;
//
//	cl_device_id devices[2];				// put in an array to create a shared context
//
//	cl_uint ret_num_devices;
//
//	cl_uint ret_num_platforms;
//	cl_uint ret_num_platforms2;
//
//	// ONLY ONE PLATFORM
//	cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
//
//	//cl_int ret2 = clGetPlatformIDs(1, &platform_id2, &ret_num_platforms2);
//
//	//ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1,
//	//	&device_id, &ret_num_devices);
//
//	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1,
//		&devices[0], &ret_num_devices);												// only 1 for HP laptop
//
//	if (CL_SUCCESS == ret)
//		printf("\nDetected GPU device : %d \n", ret_num_devices);
//	else
//		printf("\nError calling clGetDeviceIDs. Error code: %d", ret);
//
//	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1,
//		&devices[1], &ret_num_devices);
//
//	if (CL_SUCCESS == ret)
//		printf("\nDetected CPU device: %d \n", ret_num_devices);
//	else
//		printf("\nError calling clGetDeviceIDs. Error code: %d", ret);
//
//	// Create an OpenCL context
//	//cl_context context = clCreateContext(NULL, 1, &device_id_gpu, NULL, NULL, &ret);
//	cl_context context = clCreateContext(NULL, 2, devices, NULL, NULL, &ret);					// 1 context with both GPU and CPU
//
//	//cl_context context2 = clCreateContext(NULL, 1, &device_id_cpu, NULL, NULL, &ret2);		// 1 device
//
//
//	// Create a command queue
//	cl_command_queue command_queue_gpu = clCreateCommandQueue(context, devices[0], 0, &ret);				// GPU
//	cl_command_queue command_queue_cpu = clCreateCommandQueue(context, devices[0], 0, &ret);				// CPU
//
//	// Create memory buffers on the device for each vector 
//	cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
//		LIST_SIZE * sizeof(int), NULL, &ret);
//	cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
//		LIST_SIZE * sizeof(int), NULL, &ret);
//	cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,				// result
//		LIST_SIZE * sizeof(int), NULL, &ret);
//
//	// Copy the lists A and B to their respective memory buffers
//	ret = clEnqueueWriteBuffer(command_queue_gpu, a_mem_obj, CL_TRUE, 0,				// or command_queue_cpu
//		LIST_SIZE * sizeof(int), A, 0, NULL, NULL);
//	ret = clEnqueueWriteBuffer(command_queue_gpu, b_mem_obj, CL_TRUE, 0,
//		LIST_SIZE * sizeof(int), B, 0, NULL, NULL);
//
//	// Create a program from the kernel source
//	cl_program program = clCreateProgramWithSource(context, 1,
//		(const char **)&source_str, (const size_t *)&source_size, &ret);
//
//	// Build the program
//	//ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
//	ret = clBuildProgram(program, 1, &devices[0], NULL, NULL, NULL);				// build for GPU 
//
//	// Create the OpenCL kernel
//	cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);
//
//	// Set the arguments of the kernel
//	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
//	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
//	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);
//
//	// Execute the OpenCL kernel on the list
//	size_t global_item_size = LIST_SIZE;			// Process the entire lists
//	size_t local_item_size = 64;				// Divide work items into groups of 64
//
//	ret = clEnqueueNDRangeKernel(command_queue_gpu, kernel, 1, NULL,
//		&global_item_size, &local_item_size, 0, NULL, NULL);
//
//	// Read the memory buffer C on the device to the local variable C
//	int *C = (int*)malloc(sizeof(int)*LIST_SIZE);
//	ret = clEnqueueReadBuffer(command_queue_gpu, c_mem_obj, CL_TRUE, 0,
//		LIST_SIZE * sizeof(int), C, 0, NULL, NULL);
//
//	// Display the result to the screen
//	for (i = 0; i < LIST_SIZE; i++)
//		printf("%d + %d = %d\n", A[i], B[i], C[i]);
//
//	// Clean up
//	ret = clFlush(command_queue_gpu);
//	ret = clFinish(command_queue_gpu);
//
//	ret = clFlush(command_queue_cpu);
//	ret = clFinish(command_queue_cpu);
//
//
//	ret = clReleaseKernel(kernel);
//	ret = clReleaseProgram(program);
//	ret = clReleaseMemObject(a_mem_obj);
//	ret = clReleaseMemObject(b_mem_obj);
//	ret = clReleaseMemObject(c_mem_obj);
//
//	ret = clReleaseCommandQueue(command_queue_gpu);
//	ret = clReleaseCommandQueue(command_queue_cpu);
//
//	ret = clReleaseContext(context);
//	//ret2 = clReleaseContext(context2);
//
//	free(A);
//	free(B);
//	free(C);
//
//
//	int m;
//	std::cin >> m;
//
//	return 0;
//}