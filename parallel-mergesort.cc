/**
 *  \file parallel-mergesort.cc
 *
 *  \brief Implement your parallel mergesort in this file.
 */

#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include "omp.h"

#include "sort.hh"

//void printArray(int n, keytype *A)
//{
//	for (int i = 0; i < n;i ++){
//		printf("%d\n",A[i]);
//	}
//}

int binarySearch(int low, int high, keytype s, keytype *X)
{
	if (X[high] < s)
		return high + 1;
	if (X[low] > s)
		return low;
	int mid = (low + high)/2;
	while (low < high){
		if (X[mid] > s)
			high = mid;
		else
			low = mid + 1;
		mid = (low + high)/2;
	}
	return mid;
}

void parallelMerge(int p1, int t1, int p2, int t2, int p3, keytype *X, keytype *R, int threads)
{
	int n1 = t1 - p1 + 1;
	int n2 = t2 - p2 + 1;
	if (n1 < n2){
		int temp = p2; p2 = p1; p1 = temp;
		temp = t2; t2 = t1; t1 = temp;
		temp = n2; n2 = n1; n1 = temp;
	}
	if (n1 <= 0){
		return;
	}else{
		int mid1 = (p1+t1)/2;
		int mid2 = binarySearch(p2, t2, X[mid1], X);
		int b = p3 + mid1 - p1 + mid2 - p2;
		R[b] = X[mid1];
        if (threads > 1) {
#pragma omp task firstprivate(p1, mid1, p2, mid2, p3)
            {
                parallelMerge(p1, mid1 - 1, p2, mid2 - 1, p3, X, R,threads/2);
            }
#pragma omp task firstprivate(t1, mid1, t2, mid2, b)
            {
                parallelMerge(mid1 + 1, t1, mid2, t2, b + 1, X, R,threads/2);
            }
#pragma omp taskwait
        }else{
            parallelMerge(p1, mid1 - 1, p2, mid2 - 1, p3, X, R, 1);
            parallelMerge(mid1 + 1, t1, mid2, t2, b + 1, X, R ,1);
        }
	}
}
void serialMerge(int n, keytype* X, keytype* tmp) {
	int i = 0;
	int j = n / 2;
	int k = 0;
	while(i < n / 2 && j < n){
		if (X[i] < X[j])
			tmp[k++] = X[i++];
		else
			tmp[k++] = X[j++];
	}
	while(i < n / 2)
		tmp[k++] = X[i++];
	while(j < n)
		tmp[k++] = X[j++];
	memcpy(X, tmp, n * sizeof(keytype));
}

void parallelDivide(int n, keytype* A, keytype *temp, int threads)
{
	if(n <= 1){
		return ;
	}else{
		if (threads > 1){
#pragma omp task
			parallelDivide(n / 2, A, temp, threads / 2);
#pragma omp task
			parallelDivide(n - n / 2, A + n / 2, temp + n / 2, threads / 2);
#pragma omp taskwait
		}else{
			parallelDivide(n / 2, A, temp, threads / 2);
			parallelDivide(n - n / 2, A + n / 2, temp + n / 2, threads / 2);
		}
	}
	
	serialMerge(n, A, temp);
}
void printArray(int N, keytype* A){
	for (int i = 0; i < N; i++)
		std::cout << A[i] << " ";
	std::cout << "\n";
}
void parallelSort(int N, keytype* A)
{
	keytype* temp = new keytype[N];
#pragma omp parallel num_threads(8)
{
#pragma omp single nowait
	parallelDivide(N,  A, temp, omp_get_num_threads());
}	
	delete temp;
}


