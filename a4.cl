__kernel void oclgrind(__global char *grid, __global int *gridSize) {
  int i;
  int liveNeighbours;
  int rowLength;
  int totalCells;
  int curColIndex;
  
  // serial version gets ID of 1 kernal
  char rank = get_global_id(1) + '0';

  totalCells = (*gridSize) * (*gridSize);
  rowLength = *gridSize;

  for (i = 0; i < totalCells; i++) {
    // Check if on last row - no more writes required
    if (i + rowLength > totalCells) {
      break;
    }

    // If first row overwrite placeholder with rank
    if (i < rowLength && grid[i] != '.') {
      grid[i] = rank;
    }
    
    // Index in row, not in total grid
    curColIndex = i % rowLength;
    
    // Counter for living neighbors
    liveNeighbours = 0;
    
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
      if (liveNeighbours == 2 || liveNeighbours == 4) {
        grid[i + rowLength] = rank;
      }
    }

    // Barrier - when parallel
    barrier(CLK_LOCAL_MEM_FENCE);
  }
}