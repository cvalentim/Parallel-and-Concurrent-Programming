#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cilk/cilk.h>
#include <tbb/mutex.h>
#include <iterator>
#include <functional>
#include <vector>
#include <iostream>

using namespace std;

const double INF = 10000000000.0;
double *p; // probability of each key
double **cost;
vector<vector<tbb::mutex *> > lockers;

/*
Allocate a two-dimensional array with m rows and n columns.
*/
void alloc_matrix_double(double *** matrix, int n, int m){
	*matrix = (double **) malloc (m * sizeof(double *));
	for (int i = 0; i < m; ++i)
		(*matrix)[i] = (double *) malloc (n * sizeof(double));
}

/*
Allocate a two-dimensional array with m rows and n columns.
*/
void alloc_matrix_mutex(vector<vector<tbb::mutex *> >* matrix, int n, int m){
	matrix->resize(m);
	for (int i = 0; i < m; ++i){
		matrix->at(i).resize(n);
		for (int j = 0; j < n; ++j)
			matrix->at(i)[j] = new tbb::mutex;
	}
}

double calc(int start, int end){
	if (start >= end) return 0.0;

	while (lockers[start][end]->try_lock())
		usleep(10);
	
	if (cost[start][end] != -1) {
		lockers[start][end]->unlock();
		return cost[start][end];
	}

	cost[start][end] = INF;

	double fcost = 0;
	for (int j = start; j < end; ++j) fcost += p[j];

	for (int root = start; root < end; ++root){
		double c_left = cilk_spawn calc(start, root);
		double c_right = cilk_spawn calc(root + 1, end);
		cilk_sync;
		cost[start][end] = min(cost[start][end], c_left + c_right + fcost);
	}
	lockers[start][end]->unlock();
	return cost[start][end];	
}

int main(int argc, char *argv[]){
	double bestcost; 
	int n;

	scanf("%d", &n);
	p = (double *) malloc (n * sizeof(double));
	for (int i = 0; i < n; ++i)
		scanf("%lf",&p[i]);
	
	/* Find Optimal Binary Search Tree */

	alloc_matrix_double(&cost, n + 1, n + 1);	
	alloc_matrix_mutex(&lockers, n + 1, n + 1);	

	for (int i = 0; i < n + 1; ++i)
		for (int j = 0; j < n + 1; ++j) cost[i][j] = -1;

	/* Print structure of binary search tree */
	double res = calc(0, n);
	printf("Optimal solution cost = %lf\n", cost[0][n]);
	return 0;
}
