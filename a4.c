#ifndef OCLGRIND_H
#define OCLGRIND_H

#define PROGRAM_FILE "a4.cl"
#define KERNEL_FUNC "oclgrind"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

cl_device_id create_device();
cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename);

// Grid initialization methods
char *createGrid();
void randomGridInit(char *grid);
void flipFlopGridInit(char *grid);
void spiderGridInit(char *grid);
void gliderGridInit(char *grid);
void faceGridInit(char *grid);

//display grid method
void displayGrid(char* grid);

// Argument validation
void validateArguments(int numArgs, char **args);
bool isValidArg(char *toCheck);

#define DEFAULT_ERR_MSG "Usage: oclgrind ./a4 <-n #> <-s #> <-i #>"
#define KERNEL_ERR_MSG "Error: Number of Kernels must be a valid non-negative number greater than or equal to 1."
#define GRIDSIZE_ERR_MSG "Error: Grid Size must be a valid non-negative number greater than or equal to 7."
#define INITIALCONFIG_ERR_MSG "Error: Initial Configuration must be a valid non-negative number betwen 0 and 4 inclusive."

#endif

// Global variables initilized with DEFAULT values
int KERNELS = 1;
int GRIDSIZE = 20;
int INITIALCONFIG = 0;

int main(int argc, char *argv[])
{
   validateArguments(argc, argv);

	 // print statements below TO BE REMOVED
	 printf("KERNELS %d\n", KERNELS);
	 printf("GRIDSIZE %d\n", GRIDSIZE);
	 printf("INITIALCONFIG %d\n", INITIALCONFIG);

   cl_device_id device;
   cl_context context;
   cl_program program;
   cl_kernel kernel;
   cl_command_queue queue;
   cl_int err, num_groups;
   size_t local_size, global_size;
   cl_mem gridBuffer, sizeBuffer;

   char *grid = createGrid();
   int gridSize = GRIDSIZE * GRIDSIZE + 1;

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

	 // TODO: FIGURE THIS OUT
   global_size = 8;
   local_size = 4;
   num_groups = global_size / local_size;

   gridBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, gridSize * sizeof(char), grid, &err);
   if (err < 0)
   {
      perror("Couldn't create a buffer");
      exit(1);
   };
	 sizeBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(gridSize), &gridSize, &err);
   if (err < 0)
   {
      perror("Couldn't create a buffer");
      exit(1);
   };

   // Create command queue
   queue = clCreateCommandQueue(context, device, 0, &err);
   if (err < 0)
   {
      perror("Couldn't create a command queue");
      exit(1);
   };

   // Create kernel
   kernel = clCreateKernel(program, KERNEL_FUNC, &err);
   if (err < 0)
   {
      perror("Couldn't create a kernel");
      exit(1);
   };

   /* Create kernel arguments */
   err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &gridBuffer);
   err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &sizeBuffer);
   if (err < 0)
   {
      perror("Couldn't create a kernel argument");
      exit(1);
   }

   clReleaseKernel(kernel);
   clReleaseMemObject(gridBuffer);
	 clReleaseMemObject(sizeBuffer);
   clReleaseCommandQueue(queue);
   clReleaseProgram(program);
   clReleaseContext(context);
   return 0;
}

cl_device_id create_device() {
	cl_platform_id platform;
	cl_device_id dev;
	int err;

	/* Identify a platform */
	err = clGetPlatformIDs(1, &platform, NULL);
	if (err < 0) {
		perror("Couldn't identify a platform");
		exit(1);
	}

	/* Access a device */
	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
	if (err == CL_DEVICE_NOT_FOUND) {
		err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
	}
	if (err < 0) {
		perror("Couldn't access any devicesss");
		exit(1);
	}

	return dev;
}

/* Create program from a file and compile it */
char *createGrid()
{
   // Allocate memory
   char *grid = (char *)calloc(GRIDSIZE * GRIDSIZE + 1, sizeof(char));
   for (int i = 0; i < GRIDSIZE * GRIDSIZE; i++)
   {
      grid[i] = '.';
   }
   grid[GRIDSIZE * GRIDSIZE] = '\0';

   switch (INITIALCONFIG)
   {
   case 0:
      randomGridInit(grid);
      break;
   case 1:
      flipFlopGridInit(grid);
      break;
   case 2:
      spiderGridInit(grid);
      break;
   case 3:
      gliderGridInit(grid);
      break;
   case 4:
      faceGridInit(grid);
      break;

   default:
      break;
   }

   return grid;
}

void randomGridInit(char *grid)
{
   // Initialize random seed
   srand(time(NULL));

   // Fills only the first row
   for (int i = 0; i < GRIDSIZE; i++)
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
void flipFlopGridInit(char *grid)
{
   int centerIndex = GRIDSIZE / 2;
   grid[centerIndex - 2] = 'X';
   grid[centerIndex] = 'X';
   grid[centerIndex + 1] = 'X';
}

// TODO: figure out what happens if grid row length < 6
void spiderGridInit(char *grid)
{
   int spiderLength = 6;
   int startIndex = (GRIDSIZE / 2) - 3;
   for (int i = startIndex; i < startIndex + spiderLength; i++)
   {
      grid[i] = 'X';
   }
}

// TODO: figure out what happens if grid row length < 5
void gliderGridInit(char *grid)
{
   flipFlopGridInit(grid);
   grid[GRIDSIZE / 2 + 2] = 'X';
}

// TODO: figure out what happens if grid row length < 7
void faceGridInit(char *grid)
{
   spiderGridInit(grid);
   grid[GRIDSIZE / 2 + 3] = 'X';
}

cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename) {
	cl_program program;
	FILE *program_handle;
	char *program_buffer, *program_log;
	size_t program_size, log_size;
	int err;

	/* Read program file and place content into buffer */
	program_handle = fopen(filename, "r");
	if (program_handle == NULL) {
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
	program = clCreateProgramWithSource(ctx, 1, (const char **)&program_buffer, &program_size, &err);
	if (err < 0) {
		perror("Couldn't create the program");
		exit(1);
	}
	free(program_buffer);

	/* Build program */
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err < 0) {

		/* Find size of log and print to std output */
		clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		program_log = (char *)malloc(log_size + 1);
		program_log[log_size] = '\0';
		clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
		printf("%s\n", program_log);
		free(program_log);
		exit(1);
	}

	return program;
}


/**
 * If any argument is invalid, the function will print the first encountered error and exit the program.
 * For valid arguments, the GLOBAL VARIABLES will be updated.
 */
void validateArguments(int numArgs, char **args) {
	// number of arguments must be an odd number
	if (numArgs > 1 && numArgs % 2 == 0) {
		printf("%s\n", DEFAULT_ERR_MSG);
		exit(1);
	}

	// flags are at odd-numbered indicies
	for (int i = 1; i < numArgs; i = i + 2) {
		if (strncmp(args[i], "-n", strlen(args[i])) == 0) {
			if (!isValidArg(args[i+1])) {
				printf("%s\n", KERNEL_ERR_MSG);
				exit(1);
			}
			else {
				KERNELS = atoi(args[i+1]);
				if (KERNELS < 1) {
					printf("%s\n", KERNEL_ERR_MSG);
					exit(1);
				}
			}
		}
		else if (strncmp(args[i], "-s", strlen(args[i])) == 0) {
			if (!isValidArg(args[i+1])) {
				printf("%s\n", GRIDSIZE_ERR_MSG);
				exit(1);
			}
			else {
				GRIDSIZE = atoi(args[i+1]);
				if (GRIDSIZE < 7) {
					printf("%s\n", GRIDSIZE_ERR_MSG);
					exit(1);
				}
			}
		}
		else if (strncmp(args[i], "-i", strlen(args[i])) == 0) {
			if (!isValidArg(args[i+1])) {
				printf("%s\n", INITIALCONFIG_ERR_MSG);
				exit(1);
			}
			else {
				INITIALCONFIG = atoi(args[i+1]);
				if (INITIALCONFIG > 4) {
					printf("%s\n", INITIALCONFIG_ERR_MSG);
					exit(1);
				}
			}
		}
		else {
			printf("%s\n", DEFAULT_ERR_MSG);
			exit(1);
		}
	}
}

// parameters must be a number greater than 0
bool isValidArg(char *toCheck) {
	for (int i = 0; i < strlen(toCheck); i++) {
		if (!isdigit(toCheck[i])) {
			return false;
		}
	}
	return true;
}

void displayGrid(char* grid){
   for(int i = 0; i<GRIDSIZE*GRIDSIZE; i++){
      if( i % GRIDSIZE == 0){
         printf("\n");
      }
      printf(" %c ", grid[i]);
   }
   printf("\n");
}