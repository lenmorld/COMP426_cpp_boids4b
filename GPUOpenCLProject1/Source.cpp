#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>

#include <GL\glew.h>
#include <GL\freeglut.h>

/***** boids objects *******/
#include <vector>
#include <thread>
#include <string>
#include <stdlib.h> 
#include <math.h>

#include "Flock.h"
#include "Boid.h"
//#include "PVector.h"


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

/*********** program vars ********/

const int BOID_PER_FLOCK = 20;
const int FLOCKS = 7;
const int STEP_TO_CENTER_DIVISOR = 10;		// larger -> slower
const double SPEED_LIMITER = 0.01;			// larger-> faster
const double SCREEN_COHESION_RADIUS = 0.5;
const double DESIRED_DISTANCE = 0.01;
const float STEP_VELOCITY = 8;					// larger -> slower
const float NEIGHBOR_RADIUS = 0.00001;				// smaller -> more independent, bigger -> more flocking
													// extremes 0.00001, 1.0000


/************ opencl global stuff ************/

cl_device_id devices[2];				// put in an array to create a shared context
cl_int ret;

char *gpu_source_str;
size_t gpu_source_size;


// before: 1280 x 720
int windowWidth = 1920;     // Windowed mode's width
int windowHeight = 1080;     // Windowed mode's height
int windowPosX = 25;      // Windowed mode's top-left corner x
int windowPosY = 25;      // Windowed mode's top-left corner y

// colors
static std::vector < std::vector<GLfloat>> colors{
	{ 0.0f, 0.0f, 1.0f },
	{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 1.0f, 1.0f },
	{ 1.0f, 0.0f, 0.0f },
	{ 1.0f, 0.0f, 1.0f },
	{ 1.0f, 1.0f, 0.0f },
	{ 1.0f, 1.0f, 1.0f },
};

vector<Flock> flocks;


// do all OpenCL stuff every 30th of sec or when idle
void computeFunction() {

	// Create the two input vectors
	int i;
	//const int LIST_SIZE = 1024;
	int COUNT = FLOCKS * BOID_PER_FLOCK;

	// ### setup HOST DATA ####
	
	float *X = (float*)malloc(sizeof(float)* COUNT);
	float *Y = (float*)malloc(sizeof(float)* COUNT);

	float *X_V = (float*)malloc(sizeof(float)* COUNT);
	float *Y_V = (float*)malloc(sizeof(float)* COUNT);

	//INT_ARGS [COUNT, BOID_PER_FLOCK, FLOCKS, STEP_TO_CENTER_DIVISOR]

	int NUM_INT_ARGS = 4;			
	int *INT_ARGS = (int*)malloc(sizeof(int)* NUM_INT_ARGS);
	// count
	INT_ARGS[0] = COUNT;
	INT_ARGS[1] = BOID_PER_FLOCK;
	INT_ARGS[2] = FLOCKS;
	INT_ARGS[3] = STEP_TO_CENTER_DIVISOR;

	//FLOAT_ARGS [SCREEN_COHESION_RADIUS, DESIRED_DISTANCE, STEP_VELOCITY, NEIGHBOR_RADIUS]

	int NUM_FLOAT_ARGS = 4;
	float *FLOAT_ARGS = (float*)malloc(sizeof(float)* NUM_FLOAT_ARGS);
	// count
	FLOAT_ARGS[0] = SCREEN_COHESION_RADIUS;
	FLOAT_ARGS[1] = DESIRED_DISTANCE;
	FLOAT_ARGS[2] = STEP_VELOCITY;
	FLOAT_ARGS[3] = NEIGHBOR_RADIUS;


	// init X and Y to flocks' birds
	// eg 0->6
	for (int flock_id = 0; flock_id < FLOCKS; flock_id++) {

		Flock& f = flocks[flock_id];
		vector<Boid>& boids = f.boids;

		// eg 0->19
		for (int boid_id_in_flock = 0; boid_id_in_flock < BOID_PER_FLOCK; boid_id_in_flock++) {
			//Boid& b = boids[j];
			PVector location = boids[boid_id_in_flock].location;
			PVector velocity = boids[boid_id_in_flock].velocity;
			PVector new_velocity = boids[boid_id_in_flock].new_velocity;

			// index of particular boid in combined array 
			// e.g. 7 * 20 = 140 boids (0->139)
			// 6*20+19 = 139   # last boid in array : flock 6, boid 19

			int index = (flock_id * BOID_PER_FLOCK) + boid_id_in_flock;

			X[index] = location.x;
			Y[index] = location.y;

			X_V[index] = velocity.x;
			Y_V[index] = velocity.y;

			//h_vv_x[index] = new_velocity.x;
			//h_vv_y[index] = new_velocity.y;
		}
	}

	//for (i = 0; i < COUNT; i++) {
	//	A[i] = i;
	//	B[i] = LIST_SIZE - i;
	//}

	//int m1;
	//std::cin >> m1;

	// ###### Create an OpenCL context

	//cl_context context = clCreateContext(NULL, 1, &device_id_gpu, NULL, NULL, &ret);
	cl_context context = clCreateContext(NULL, 2, devices, NULL, NULL, &ret);					// 1 context with both GPU and CPU

																								//cl_context context2 = clCreateContext(NULL, 1, &device_id_cpu, NULL, NULL, &ret2);		// 1 device


																								// Create a command queue
	cl_command_queue command_queue_gpu = clCreateCommandQueue(context, devices[0], 0, &ret);				// GPU
	cl_command_queue command_queue_cpu = clCreateCommandQueue(context, devices[1], 0, &ret);				// CPU

																											// Create memory buffers on the device for each vector 
	//cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
	//	LIST_SIZE * sizeof(int), NULL, &ret);
	//cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
	//	LIST_SIZE * sizeof(int), NULL, &ret);
	//cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,				// result
	//	LIST_SIZE * sizeof(int), NULL, &ret);


	// ##### setup DEVICE MEMORY #########

	// X and Y
	cl_mem x_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
		COUNT * sizeof(float), NULL, &ret);
	cl_mem y_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
		COUNT * sizeof(float), NULL, &ret);
	// X,Y velocities
	cl_mem x_v_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
		COUNT * sizeof(float), NULL, &ret);
	cl_mem y_v_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
		COUNT * sizeof(float), NULL, &ret);

	// result objects
	cl_mem xr_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,				
		COUNT * sizeof(float), NULL, &ret);
	cl_mem yr_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,				
		COUNT * sizeof(float), NULL, &ret);

	cl_mem x_v_r_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,				
		COUNT * sizeof(float), NULL, &ret);
	cl_mem y_v_r_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		COUNT * sizeof(float), NULL, &ret);

	// INT_ARGS
	cl_mem int_args_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,				
		NUM_INT_ARGS * sizeof(int), NULL, &ret);
	// FLOAT_ARGS
	cl_mem float_args_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
		NUM_FLOAT_ARGS * sizeof(float), NULL, &ret);
	


	//###### DEVICE TO HOST ##########
	// #### Copy the lists A and B to their respective memory buffers
	// ###################################

	//ret = clEnqueueWriteBuffer(command_queue_gpu, a_mem_obj, CL_TRUE, 0,				
	//	LIST_SIZE * sizeof(int), A, 0, NULL, NULL);
	//ret = clEnqueueWriteBuffer(command_queue_gpu, b_mem_obj, CL_TRUE, 0,
	//	LIST_SIZE * sizeof(int), B, 0, NULL, NULL);

	// X,Y
	ret = clEnqueueWriteBuffer(command_queue_gpu, x_mem_obj, CL_TRUE, 0,			// X array		
		COUNT * sizeof(float), X, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue_gpu, y_mem_obj, CL_TRUE, 0,			// Y array	
		COUNT * sizeof(float), Y, 0, NULL, NULL);

	// X,Y velocities
	ret = clEnqueueWriteBuffer(command_queue_gpu, x_v_mem_obj, CL_TRUE, 0,				
		COUNT * sizeof(float), X_V, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue_gpu, y_v_mem_obj, CL_TRUE, 0,				
		COUNT * sizeof(float), Y_V, 0, NULL, NULL);

	ret = clEnqueueWriteBuffer(command_queue_gpu, int_args_mem_obj, CL_TRUE, 0,			// ARGS array	
		NUM_INT_ARGS * sizeof(int), INT_ARGS, 0, NULL, NULL);

	ret = clEnqueueWriteBuffer(command_queue_gpu, float_args_mem_obj, CL_TRUE, 0,			// ARGS array	
		NUM_FLOAT_ARGS * sizeof(float), FLOAT_ARGS, 0, NULL, NULL);

	// #### Create a program from the kernel source
	cl_program program = clCreateProgramWithSource(context, 1,
		(const char **)&gpu_source_str, (const size_t *)&gpu_source_size, &ret);

	// #### Build the program

	//ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	ret = clBuildProgram(program, 1, &devices[0], NULL, NULL, NULL);				// build for GPU 
																					
	cl_kernel kernel = clCreateKernel(program, "compute_rules", &ret);				// Create the OpenCL kernel


	// #### Set the arguments of the kernel

	//ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
	//ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
	//ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);

	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&x_mem_obj);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&y_mem_obj);

	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&x_v_mem_obj);
	ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&y_v_mem_obj);

	ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&xr_mem_obj);
	ret = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&yr_mem_obj);

	ret = clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&x_v_r_mem_obj);
	ret = clSetKernelArg(kernel, 7, sizeof(cl_mem), (void *)&y_v_r_mem_obj);

	ret = clSetKernelArg(kernel, 8, sizeof(cl_mem), (void *)&int_args_mem_obj);
	ret = clSetKernelArg(kernel, 9, sizeof(cl_mem), (void *)&float_args_mem_obj);

	// Execute the OpenCL kernel on the list
	size_t global_item_size = COUNT;			// Process the entire lists
	size_t local_item_size = 1;				// Divide work items into groups of 64				// !! what is a good group size? <---------

	//size_t local_item_size = 64;

	ret = clEnqueueNDRangeKernel(command_queue_gpu, kernel, 1, NULL,
		&global_item_size, &local_item_size, 0, NULL, NULL);

	// Read the memory buffer C on the device to the local variable C

	// #######################
	// DEVICE TO HOST
	// ########################

	//int *C = (int*)malloc(sizeof(int)*LIST_SIZE);

	float *X_R = (float*)malloc(sizeof(float)*COUNT);
	float *Y_R = (float*)malloc(sizeof(float)*COUNT);

	float *X_V_R = (float*)malloc(sizeof(float)*COUNT);
	float *Y_V_R = (float*)malloc(sizeof(float)*COUNT);

	//ret = clEnqueueReadBuffer(command_queue_gpu, c_mem_obj, CL_TRUE, 0,
	//	LIST_SIZE * sizeof(int), C, 0, NULL, NULL);

	ret = clEnqueueReadBuffer(command_queue_gpu, xr_mem_obj, CL_TRUE, 0,
		COUNT * sizeof(float), X_R, 0, NULL, NULL);
	ret = clEnqueueReadBuffer(command_queue_gpu, yr_mem_obj, CL_TRUE, 0,
		COUNT * sizeof(float), Y_R, 0, NULL, NULL);

	ret = clEnqueueReadBuffer(command_queue_gpu, x_v_r_mem_obj, CL_TRUE, 0,
		COUNT * sizeof(float), X_V_R, 0, NULL, NULL);
	ret = clEnqueueReadBuffer(command_queue_gpu, y_v_r_mem_obj, CL_TRUE, 0,
		COUNT * sizeof(float), Y_V_R, 0, NULL, NULL);

	// Display the result to the screen
	//for (i = 0; i < COUNT; i++)
		//printf("%f = %d\n", X[i], XR[i]);

	// iterate through flocks and boids, apply new position
	// or spawn new threads again just to move
	for (int flock_id = 0; flock_id < FLOCKS; flock_id++) {
		Flock& f = flocks[flock_id];
		vector<Boid>& boids = f.boids;

		// eg 0->19
		for (int j = 0; j < BOID_PER_FLOCK; j++) {
			Boid& b = boids[j];

			//e.g. last boid in 7*20 = 140 boids (0->139)		:		6*20+19 = 139 

			int index = (flock_id * BOID_PER_FLOCK) + j;

			// apply new location and velocity (copied from device to host) - LAST STEP

			// no need for orig X and Y here right?


			//b.location.x = X_R[index];
			//b.location.y = Y_R[index];

			//b.location.x = h_x[index];
			//b.location.y = h_y[index];

			b.velocity.x = X_V_R[index];
			b.velocity.y = Y_V_R[index];

			b.location.x += b.velocity.x;
			b.location.y += b.velocity.y;

			//b.velocity.x = h_v_x[index];
			//b.velocity.y = h_v_y[index];
		}
	}


	// Display the result to the screen
	//for (i = 0; i < COUNT; i++)
	//	printf("%d + %d = %d\n", A[i], B[i], C[i]);

	// Clean up
	ret = clFlush(command_queue_gpu);
	ret = clFinish(command_queue_gpu);

	ret = clFlush(command_queue_cpu);
	ret = clFinish(command_queue_cpu);

	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(x_mem_obj);
	ret = clReleaseMemObject(y_mem_obj);

	ret = clReleaseMemObject(x_v_mem_obj);
	ret = clReleaseMemObject(y_v_mem_obj);

	ret = clReleaseMemObject(xr_mem_obj);
	ret = clReleaseMemObject(yr_mem_obj);

	ret = clReleaseMemObject(x_v_r_mem_obj);
	ret = clReleaseMemObject(y_v_r_mem_obj);

	ret = clReleaseMemObject(int_args_mem_obj);
	ret = clReleaseMemObject(float_args_mem_obj);

	ret = clReleaseCommandQueue(command_queue_gpu);
	ret = clReleaseCommandQueue(command_queue_cpu);

	ret = clReleaseContext(context);
	//ret2 = clReleaseContext(context2);

	free(X);
	free(Y);

	free(X_V);
	free(Y_V);

	free(X_R);
	free(Y_R);

	free(X_V_R);
	free(Y_V_R);

	free(INT_ARGS);
	free(FLOAT_ARGS);

	glutPostRedisplay();      // Post re-paint request to activate display()
}


float RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

void displayMe(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);    // To operate on the model-view matrix
	glLoadIdentity();              // Reset model-view matrix

	for (std::vector<int>::size_type j = 0; j < flocks.size(); j++) {

		vector<GLfloat> color = colors[j];
		glColor3f(color[0], color[1], color[2]);

		glBegin(GL_TRIANGLES);
		GLfloat scale = 0.012f;		// triangle size

		vector<Boid> _boids = flocks[j].boids;
		for (std::vector<int>::size_type i = 0; i < _boids.size(); i++) {
			// create triagle from passed location (only 1 point, create 2 other points)

			PVector p1 = _boids[i].location;
			PVector p2 = PVector(p1.x + scale, p1.y);
			PVector p3 = PVector(p1.x + scale, p1.y + scale);
			glVertex2f(p1.x, p1.y);
			glVertex2f(p2.x, p2.y);
			glVertex2f(p3.x, p3.y);
		}

		glEnd();
	}

	glutSwapBuffers();			// Swap front and back buffers (of double buffered mode)
}


int main(int argc, char** argv) {

	// OpenGL

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);				// Set double buffered mode
	glutInitWindowSize(windowWidth, windowHeight);                    // window size
	glutInitWindowPosition(windowPosX, windowPosY);                // distance from the top-left screen
	glutCreateWindow("^^ BOIDS ^^");				// message displayed on top bar window

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0, 0, 100);
	glPopMatrix();

	glutDisplayFunc(displayMe);
	//glutReshapeFunc(changeSize);
	glutIdleFunc(computeFunction);					// Register callback handler if no other event

													// Set "clearing" or background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);			// Black and opaque


													// init flocks
													// with center in a random position
	for (int i = 0; i < FLOCKS; i++) {
		float r1 = RandomFloat(-0.99f, 0.99f);
		float r2 = RandomFloat(-0.99f, 0.99f);
		PVector loc(r1, r2);

		vector<Boid> boids;

		// init boids in the flock at a random position
		for (int i = 0; i < BOID_PER_FLOCK; i++) {
			float r1 = RandomFloat(-0.99f, 0.99f);
			float r2 = RandomFloat(-0.99f, 0.99f);
			PVector loc(r1, r2);
			boids.push_back(Boid(i, loc, PVector(0.0, 0.0), PVector(RandomFloat(-0.99f, 0.99f), RandomFloat(-0.99f, 0.99f))));
		}

		flocks.push_back(Flock(i, loc, PVector(0.0, 0.0), loc, boids));
	}

	/**** init openCL stuff ***/

	// ### load Kernel
	// GPU

	// Load the kernel source code into the array source_str
	FILE *fp;
	char *source_str;
	size_t source_size;

	//fp = fopen("template.cl", "r");
	fp = fopen("gpu.cl", "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	gpu_source_str = (char*)malloc(MAX_SOURCE_SIZE);
	gpu_source_size = fread(gpu_source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);
	//====================

	// ================================================
	// Get platform and device information
	cl_platform_id platform_id = NULL;				// Get platform ID, hopefully only one platform that contains both CPU and GPU
													//cl_platform_id platform_id2 = NULL;

													//cl_device_id device_id_gpu = NULL;
													////cl_device_id device_id_cpu = NULL;

													//cl_device_id devices[2];				// put in an array to create a shared context
	cl_uint ret_num_devices;

	cl_uint ret_num_platforms;
	cl_uint ret_num_platforms2;

	// ONLY ONE PLATFORM
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

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




	glutMainLoop();

	int m;
	std::cin >> m;

	return 0;
}