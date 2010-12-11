#include <stdio.h>
#include <values.h>
#include <malloc.h>

const double INF = 10000000000.0;

/*
Allocate a two-dimensional array with m rows and n columns.
*/

void alloc_matrix(void *** a, int m, int n, int size)
{
	void *storage;	

	storage = (void *) malloc (m * n * size);
	*a = (void **) malloc (m * sizeof(void *));

	for (int i = 0; i < m; ++i)
		(*a)[i] = storage + i * n * size;	
}

void print_root(int **root, int low, int high)
{
	printf("Root of tree spanning %d-%d is %d\n",
			low, high, root[low][high+1]);

	if (low < root[low][high + 1])
		print_root(root, low, root[low][high+1] -1);

	if (root[low][high + 1] < high + 1)
		print_root(root, root[low][high+1]+1, high);
}

int main(int argc, char *argv[]){
	double bestcost; 
	int bestroot;
	int high;
	int low;
	int n;
	int r;
	double rcost;
	int **root;
	double **cost;
	double *p; // probability of each key
	
	scanf("%d", &n);
	p = (double *) malloc (n * sizeof(double));
	for (int i = 0; i < n; ++i)
		scanf("%lf",&p[i]);
	
	/* Find Optimal Binary Search Tree */

	alloc_matrix((void ***) &cost, n + 1, n + 1, sizeof(double));	
	alloc_matrix((void ***) &root, n + 1, n + 1, sizeof(int));

	for (int low = n; low >= 0; --low)
	{
			cost[low][low] = 0.0;
			root[low][low] = low;
			for (int high = low + 1; high <= n; ++high)
			{
				bestcost = INF;
				double fcost = 0;
				for (int j = low; j < high; ++j) fcost += p[j];

				for (int r = low; r < high; ++r)
				{
					rcost = cost[low][r]  + cost[r + 1][high] + fcost;
					if (rcost < bestcost)	
					{
						bestcost = rcost;
						bestroot = r;
					}
				}
				cost[low][high] = bestcost;
				root[low][high] = bestroot;
			}
	}

	/* Print structure of binary search tree */
	//print_root(root, 0, n - 1);
	printf("Optimal solution cost = %lf\n", cost[0][n]);
	return 0;
}
