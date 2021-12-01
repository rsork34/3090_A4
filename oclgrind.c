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
cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename);

char *createGrid(int gridDimensions, int pattern);
void randomGridInit(char *grid, int dimensions);
void flipFlopGridInit(char *grid, int gridDimensions);
void spiderGridInit(char *grid, int gridDimensions);

#endif

int main(int argc, char *argv[])
{
   cl_device_id device;
   cl_context context;
   cl_program program;
   cl_int err, num_groups;
   size_t local_size, global_size;

   /* Create device and context */
   device = create_device();
   context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
   if (err < 0)
   {
      perror("Couldn't create a context");
      exit(1);
   }

   /* Build program */
   program = build_program(context, device, PROGRAM_FILE);

   /* Create data buffer */
   // TODO: FIGURE THIS OUT
   global_size = 8;
   local_size = 4;
   num_groups = global_size / local_size;

   return 0;
}

cl_device_id create_device()
{
   cl_platform_id platform;
   cl_device_id dev;
   int err;

   /* Identify a platform */
   err = clGetPlatformIDs(1, &platform, NULL);
   if (err < 0)
   {
      perror("Couldn't identify a platform");
      exit(1);
   }

   /* Access a device */
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
   if (err == CL_DEVICE_NOT_FOUND)
   {
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
   }
   if (err < 0)
   {
      perror("Couldn't access any devicesss");
      exit(1);
   }

   return dev;
}

/* Create program from a file and compile it */
cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename)
{
   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;
   int err;

   /* Read program file and place content into buffer */
   program_handle = fopen(filename, "r");
   if (program_handle == NULL)
   {
      perror("Couldn't find the program file");
      exit(1);
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char *)malloc(program_size + 1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);

   /* Create program from file */
   program = clCreateProgramWithSource(ctx, 1,
                                       (const char **)&program_buffer, &program_size, &err);
   if (err < 0)
   {
      perror("Couldn't create the program");
      exit(1);
   }
   free(program_buffer);

   /* Build program */
   err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
   if (err < 0)
   {

      /* Find size of log and print to std output */
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                            0, NULL, &log_size);
      program_log = (char *)malloc(log_size + 1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                            log_size + 1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }

   return program;
}

char *createGrid(int gridDimensions, int pattern)
{
   // Allocate memory
   char *grid = (char *)calloc(gridDimensions * gridDimensions + 1, sizeof(char));
   for (int i = 0; i < gridDimensions * gridDimensions; i++)
   {
      grid[i] = '.';
   }
   grid[gridDimensions * gridDimensions] = '\0';

   switch (pattern)
   {
   case 0:
      randomGridInit(grid, gridDimensions);
      break;
   case 1:
      flipFlopGridInit(grid, gridDimensions);
      break;
   case 2:
      spiderGridInit(grid, gridDimensions);
      break;

   default:
      break;
   }

   return grid;
}

void randomGridInit(char *grid, int dimensions)
{
   // Initialize random seed
   srand(time(NULL));

   // Fills only the first row
   for (int i = 0; i < dimensions; i++)
   {
      // 50% chance
      if (rand() / (RAND_MAX / 3) == 0)
      {
         // TODO: based on rank
         grid[i] = 'X';
      }
   }
}

// TODO: figure out what happens if grid row length < 4
void flipFlopGridInit(char *grid, int gridDimensions)
{
   int centerIndex = gridDimensions / 2;
   grid[centerIndex - 2] = 'X';
   grid[centerIndex] = 'X';
   grid[centerIndex + 1] = 'X';
}

// TODO: figure out what happens if grid row length < 6
void spiderGridInit(char *grid, int gridDimensions)
{
   int spiderLength = 6;
   int startIndex = (gridDimensions / 2) - 3;
   for (int i = startIndex; i < startIndex + spiderLength; i++)
   {
      grid[i] = 'X';
   }
}
