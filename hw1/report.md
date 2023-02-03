# Report for Homework #1

Only covers `computePi.c` and `quickSort.c`.

## `computePi.c`

Creates a provided amount of workers which all create a given amount of points.

For each point the workers calculate they randomize two numbers between 1 and 0 (inclusive). These values are treated as the x and y coordinates for a point in the grid, then their distance from (0,0) is calculated. If the distance is one (1) or less the point is seen as within the "circle" and a counter is incremented.

Finally all workers counters are added together and used to estimate pi.

## `quickSort.c`

Creates a an unsorted list of numbers with a provided size and then sorts it using quicksort by providing a range of the array for a worker to partiton. The first worker recives the entire array as it's range.

Each worker (if it's range is larger then a set amount) partitions it's range and creates a new worker thread to deal with the range above the last pivot and itself deals with the range below. When a worker finds a range less then a set amount it instead performs an insertion sort on the range and exits.

When all workers have finished the array will be sorted.

### Possible improvments

It gets exponentially slower for larger numbers since it spends a long time creating new threads. A solution to this might be implementing a thread pool system instead.
