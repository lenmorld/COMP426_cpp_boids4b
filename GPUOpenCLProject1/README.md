
X and Y
CL_MEM_READ_ONLY -> CL_MEM_READ_WRITE 
didnt work

read-only value not assignable inside Kernel


-> it was const in the kernel!

__global const float *X

