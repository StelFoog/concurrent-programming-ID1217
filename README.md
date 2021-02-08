# ID1217 Concurrent Programming KTH

## Homework

### Homework 1

1. **Compute Sum, Min and Max of Matrix Elements** 
2. **Quicksort:** Completed

	Currently slightly to slow due to creating too many treads. To improve a threadpool could be implemented.
	- Performance: With 50000 items in an array we average ~0.64s in runtime. With 100000 items we average ~2.5s in runtime. 2.5/0.64 = ~3.9, this is... not great. It's not terrible but as we should see slightly more than 2 (maybe 2.4 at most) we deperatly need a thread pool or something else that prevents the overhead from creating thousands of threads.
	- Improved perfomance: After altering the code so a thread only creates one new thread after a completed quicksort step (and does the other half itself) the performance has nearly doubled.
3. **Compute pi:** Completed

	Implemented by multiplying the ratio of randomly generated points (x and y generated to be between 0 and 1) that have a distance of less than 1 from (0, 0) and total generated points by 4. Tends slightly to 3.12 strangely?
	- Performance: Very quick, only at 10000000 points do we spend more than 1s before we get results.
4. **The Unix tee command:** Completed

	Implemented by having main thread read  stdIn char by char and sending them though pipes to stdOut printing thread and file printing thread. There was an issue when setting a file as stdIn where the program closed before both pipes had finished reading and printing. To handle this we end up with a situation where not all threads close leaving us hanging but doing nothing.

	Due to issues with file printing the system I ended up implementing was very inefficient. Having to open and close the file for every character. When not implemented this way the file ended up either not printing or printing ~9 random chars for every char ment to be printed.
	- Performance: Due to the issues with file printing the file printing thread ends up being somewhat slow. It's fast enough that a human typing would never notice however.
5. **The Unix diff command**
6. **Find Palindromic Words**
7. **The 8-queens problem**
