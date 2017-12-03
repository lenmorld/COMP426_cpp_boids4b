__kernel void compute_rules(__global const float *X, __global const float *Y, __global float *XR, __global float *YR, __global int *ARGS) {
 
    // Get the index of the current element to be processed
    int i = get_global_id(0);
 
    // Do the operation
    // C[i] = A[i] + B[i];

	XR[i] = X[i] + 0.01;
	YR[i] = Y[i] + 0.01;
}