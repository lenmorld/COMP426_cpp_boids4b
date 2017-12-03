currently on:
GPUOpenCLProjectforWindows.cpp

TODO:

apply code to create context (combined or separate)
for the simple vector addition algorithm

goal: execute kernel both in CPU and GPU in parallel

=====================================

=> found a script that displays both:

1. Device: Intel(R) HD Graphics 620
 1.1 Hardware version: OpenCL 2.1
 1.2 Software version: 21.20.16.4590
 1.3 OpenCL C version: OpenCL C 2.0
 1.4 Parallel compute units: 24
2. Device: Intel(R) Core(TM) i5-7200U CPU @ 2.50GHz
 2.1 Hardware version: OpenCL 2.1 (Build 2)
 2.2 Software version: 6.8.0.2
 2.3 OpenCL C version: OpenCL C 2.0
 2.4 Parallel compute units: 4

=======================================

TODO:
try in lab computers if more than 1 device/platform found

CPU and GPU

in laptop, only GPU found

================================

1>------ Build started: Project: GPUOpenCLProject1, Configuration: Release x64 ------
1>Preprocessing: Template.cl
1>Ignoring -TARGETOS option - not supported for 64-bit version of this tool
1>OpenCL Intel(R) Graphics device was found!
1>Device name: Intel(R) HD Graphics 620
1>Device version: OpenCL 2.1
1>Device vendor: Intel(R) Corporation
1>Device profile: FULL_PROFILE
1>fcl build 1 succeeded.
1>bcl build succeeded.
1>
1>vector_add info:
1>	Maximum work-group size: 256
1>	Compiler work-group size: (0, 0, 0)
1>	Local memory size: 0
1>	Preferred multiple of work-group size: 32
1>	Minimum amount of private memory: 0
1>	Amount of spill memory used by the kernel: 0
1>
1>Build succeeded!


================================


other projects

C:\Users\lenny\source\repos

EmptyOpenCLProject2 


