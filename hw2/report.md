# Report for Homework #2

## `matrixSum-omp.c`

Creates a matrix filled with random numbers for a given size.

Uses OpenMP for reduction to go though the entire matrix (with given amount of workers taking a section each), meanwhile each worker also finds the indecies of the minimum and maximum numbers in their section. After reduction is complete all workers local minimums and maximums are compared to find which are globally minimum and maximum.

### Speedup

#### 100 \* 100 matrix

| Threads |  Median time    |  Speedup   |
| ------- | --------------- | ---------- |
| `1`     | `0.000038147` s | `1` (base) |
| `2`     | `0.000103951` s | `0.366`    |
| `4`     | `0.000046969` s | `0.812`    |
| `8`     | `0.000113964` s | `0.335`    |

For a matrix this small the cost of creating the threads likely exeeds the gain from lowering each threads workload.

#### 1 000 \* 1 000 matrix

| Threads |  Median time   |  Speedup   |
| ------- | -------------- | ---------- |
| `1`     | `0.00405312` s | `1` (base) |
| `2`     | `0.00513005` s | `0.790`    |
| `4`     | `0.00760698` s | `0.533`    |
| `8`     | `0.00341582` s | `1.187`    |

Here we can begin to see some improvment when using 8 threads.

#### 10 000 \* 10 000 matrix

| Threads |  Median time |  Speedup   |
| ------- | ------------ | ---------- |
| `1`     | `0.351798` s | `1` (base) |
| `2`     | `0.227320` s | `1.548`    |
| `4`     | `0.245848` s | `1.431`    |
| `8`     | `0.199039` s | `1.767`    |

We see generally increasing speedups when increasing the amount of threads as we have a large enough matrix that each thread has so much to do that making the work a fration of itself is valuable even when having to create threads.

## `quickSort-omp.c`

Creates an array of the given size filled with random numbers.

Creates an OpenMP task for every pass of quicksort. Each task checks if it's array is ready to be sorted with insertion sort (a predefined value between 5-15) and otherwise finds a pivot and splits the array such that all numbers above the pivot are above it in the array and all numbers below are below it. When the array has been divided it creates 2 new tasks, one for dividing/insertion sorting everything above the pivot and one for everything below it.

### Speedup

It seems that the open mp task system might be inefficient when creating large amounts of tasks (perhaps because creating and taking tasks are critical actions). As such there is no clear gain from parallelizing. Perhaps if a limit was set such that a task doesn't create new tasks when an array is small enough (before switch to insertion) there would be an improvment since tasks would only be created for large workloads.

#### 1 000 array

| Threads |  Median time     |  Speedup   |
| ------- | ---------------- | ---------- |
| `1`     | `0.0001029970` s | `1` (base) |
| `2`     | `0.0000851154` s | `1.210`    |
| `4`     | `0.0001161100` s | `0.887`    |
| `8`     | `0.0000958443` s | `1.074`    |

#### 10 000 array

| Threads |  Median time   |  Speedup   |
| ------- | -------------- | ---------- |
| `1`     | `0.00173092` s | `1` (base) |
| `2`     | `0.00138593` s | `1.249`    |
| `4`     | `0.00132394` s | `1.307`    |
| `8`     | `0.00317907` s | `0.544`    |

#### 100 000 array

| Threads |  Median time  |  Speedup   |
| ------- | ------------- | ---------- |
| `1`     | `0.0168941` s | `1` (base) |
| `2`     | `0.0510740` s | `0.331`    |
| `4`     | `0.0177119` s | `0.954`    |
| `8`     | `0.0185931` s | `0.909`    |

#### 1 000 000 array

| Threads |  Median time |  Speedup   |
| ------- | ------------ | ---------- |
| `1`     | `0.171680` s | `1` (base) |
| `2`     | `0.172123` s | `0.997`    |
| `4`     | `0.229976` s | `0.747`    |
| `8`     | `0.171354` s | `1.002`    |
