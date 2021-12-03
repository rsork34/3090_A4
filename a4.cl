__kernel void oclgrind(__global char *grid, __global int *gridSize, __global int *numKernels) {
  // Read in args
  int kernelCount = *numKernels; 
  int rowLength = *gridSize;

  // Calculate columns for kernel to work on
  int offset = rowLength % kernelCount;
  int cellsPerKernel = rowLength / kernelCount;
  int startIndex = (get_global_id(0) * cellsPerKernel);
  int endIndex = startIndex + cellsPerKernel + offset;

  // Columns don't evenly divide - give extra to rank 0
  if (offset > 0 && get_global_id(0) != 0) {
    startIndex += offset;
  }

  // serial version gets ID of 1 kernal
  char rank = (kernelCount < 10) ? (get_global_id(0) + '0') : 'X';

  int totalCells = (rowLength) * (rowLength);
  for (int i = 0; i < totalCells; i++) {
    // Barrier - when parallel
    barrier(CLK_LOCAL_MEM_FENCE);

    // Check if on last row - no more writes required
    if (i + rowLength > totalCells) {
      break;
    }

    // Index in row, not in total grid
    int curColIndex = i % rowLength;

    // TODO: don't continue, but have index skip to next row
    // Ensure kernel only works on its own columns
    if (curColIndex < startIndex || curColIndex >= endIndex) {
      continue;
    }

    // If first row overwrite placeholder with rank
    if (i < rowLength && grid[i] != '.') {
      grid[i] = rank;
    }
    
    // Counter for living neighbors
    int liveNeighbours = 0;
    
    // check if cell is on left edge
    if ((curColIndex - 1) < 0) {
      if (grid[i + 1] != '.') { // check right neighbour
        liveNeighbours++;
      }
      if (grid[i + 2] != '.') { // check right neighbour's right neighbour
        liveNeighbours++;
      }
    }
    // check if cell is 1 space from left edge
    else if ((curColIndex - 1) == 0) {
      if (grid[i - 1] != '.') { // check left neighbour
        liveNeighbours++;
      }
      if (grid[i + 1] != '.') { // check right neighbour
        liveNeighbours++;
      }
      if (grid[i + 2] != '.') { // check right neighbour's right neighbour
        liveNeighbours++;
      }
    }
    // check if cell is 1 space from right edge
    else if ((curColIndex + 1) == (rowLength - 1)) {
      if (grid[i + 1] != '.') { // check right neighbour
        liveNeighbours++;
      }
      if (grid[i - 1] != '.') { // check left neighbour
        liveNeighbours++;
      }
      if (grid[i - 2] != '.') { // check left neighbour's left neighbour
        liveNeighbours++;
      }
    }
    // check if cell is on right edge
    else if ((curColIndex + 1) > (rowLength - 1)) {
      if (grid[i - 1] != '.') { // check left neighbour
        liveNeighbours++;
      }
      if (grid[i - 2] != '.') { // check left neighbour's left neighbour
        liveNeighbours++;
      }
    }
    // check if cell is in the middle of the grid
    else {
      if (grid[i - 1] != '.') { // check left neighbour
        liveNeighbours++;
      }
      if (grid[i - 2] != '.') { // check left neighbour's left neighbour
        liveNeighbours++;
      }
      if (grid[i + 1] != '.') { // check right neighbour
        liveNeighbours++;
      }
      if (grid[i + 2] != '.') { // check right neighbour's right neighbour
        liveNeighbours++;
      }
    }

    // Current cell is dead
    if (grid[i] == '.') {
      // Cell is alive for next iteration
      if (liveNeighbours == 2 || liveNeighbours == 3) {
        grid[i + rowLength] = rank;
      }
    }
    // Current cell is alive
    else {
      // Cell is alive for next iteration
      if (liveNeighbours == 2 || liveNeighbours == 4) {
        grid[i + rowLength] = rank;
      }
    }
  }
}