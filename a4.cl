__kernel void countNeighbors(__local int* index, __global char * grid, __local int * count) {
  *count = 1;
}

__kernel void oclgrind(__global char *grid, __global int *gridSize) {
  __local int i;
  __local int count;

  // TODO: Get kernel ID
  __local char KERNEL_ID_CHAR = '0';

  for (i = 0; i < *gridSize; i++) {
    count = 0;
    countNeighbors(&i, grid, &count);
    if (count == 1)
    {
      grid[i] = KERNEL_ID_CHAR;
    }
    // Calculate num living neighbors

    // If next iteration is alive {}
      // Write to next row same column

    // If next iteration is dead {}
      // Write to next row same column

    // Barrier - when parallel
  }
}