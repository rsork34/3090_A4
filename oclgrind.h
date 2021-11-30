#ifndef OCLGRIND_H
#define OCLGRIND_H

#define PROGRAM_FILE "oclgrind.cl"
#define KERNEL_FUNC "oclgrind"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

cl_device_id create_device();
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename);

#endif