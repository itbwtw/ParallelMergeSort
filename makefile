CC = gcc
CXX = g++
CFLAGS =
COPTFLAGS = -O3 -fopenmp
LDFLAGS =
objects = parallel-mergesort.o driver.o sort.o
target = mergesort-omp

$(target) : $(objects) timer.o
	$(CXX) $(COPTFLAGS) -o $@ $^
%.o : %.cc sort.hh
	$(CXX) $(CFLAGS) $(COPTFLAGS) -o $@ -c $<
timer.o : timer.c timer.h
	$(CC) $(CFLAGS) $(COPTFLAGS) -o $@ -c $<
clean:
	rm -f $(target) $(objects) timer.o