#include <stdio.h>
#include <mpi.h>
#include <malloc.h>
#include <assert.h>

const double INF = 100;
void read_input()
{
	int nkeys;
	printf("Enter nkeys:\n");
	scanf("%d", &nkeys);
	double *keys = (double *) malloc(nkeys * sizeof(double));

	for (int i = 0; i < nkeys; ++i)
		scanf("%lf", &keys[i]);

	int np;
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	int tag = 50; //wtf?

	// Sending the number of keys to all others process
	for (int i = 0; i < np; ++i){
		MPI_Send(&nkeys, 1, MPI_INT, i, i, MPI_COMM_WORLD);
	}

	// Sending elements
	for (int i = 0; i < np; ++i){
		MPI_Send(keys, nkeys, MPI_DOUBLE, i, i, MPI_COMM_WORLD);
	}

}

void alloc_matrix_double(double *** matrix, int n, int m){
	*matrix = (double **) malloc (m * sizeof(double *));
	for (int i = 0; i < m; ++i)
		(*matrix)[i] = (double *) malloc (n * sizeof(double));
}

void alloc_matrix_bool(bool *** matrix, int n, int m){
	*matrix = (bool **) malloc (m * sizeof(bool *));
	for (int i = 0; i < m; ++i)
		(*matrix)[i] = (bool *) malloc (n * sizeof(bool));
}


int main(int argc, char** argv){
	int tag = 50;
	int nkeys;
	double* keys;
	double** cost;
	bool** mark;

	MPI_Status status;
	MPI_Init(&argc, &argv);

	MPI_Group workers_group;
	MPI_Comm workers_comm;
	MPI_Group BASE_GROUP;
		
	MPI_Comm_group(MPI_COMM_WORLD, &BASE_GROUP);	


	int my_rank, np;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	// creating a new group from GROUP_WORLD but without the zero process
	int ranks[1] = {0};
	MPI_Group_excl(BASE_GROUP, 1, ranks, &workers_group);

	// creating a communication channel for notify workers' groups members
	MPI_Comm_create(MPI_COMM_WORLD, workers_group, &workers_comm);

	if (!my_rank){
		read_input();
	}else{
		int from = 0;
		MPI_Recv(&nkeys, 1, MPI_INT, from, my_rank, MPI_COMM_WORLD, &status);
		// TODO: check status for error 

		keys = (double *) malloc(nkeys * sizeof(double));

		MPI_Recv(keys, nkeys, MPI_DOUBLE, from, my_rank, MPI_COMM_WORLD, &status);
		// TODO: check status for error 

		double sum = 0;
		for (int i = 0; i < nkeys; ++i)
			sum += keys[i];
		fprintf(stderr, "Sum process %d: %lf\n", my_rank, sum);

		alloc_matrix_double(&cost, nkeys + 1, nkeys + 1);
		alloc_matrix_bool(&mark, nkeys + 1, nkeys + 1);
	
		for (int i = 0; i <= nkeys; ++i)
			for (int j = 0; j <= nkeys; ++j) {
				mark[i][j] = false;
				cost[i][j] = INF;
			}

		for (int i = 0; i <= nkeys; ++i) {
				mark[i][i] = true;
				cost[i][i] = 0;
		}
		assert (np - 1 > 0);
		int delta = nkeys/(np - 1) + 1;
		for (int step = 0; step < nkeys; ++step){
				fprintf(stderr, "Step %d process %d:\n", step, my_rank);
				for (int row = delta * (my_rank - 1); row <= nkeys && row < delta * my_rank; ++row){
					int col = row + step + 1;
					if (col > nkeys) break;
					double fcost = 0;
					for (int r = row; r < col; ++r) fcost += keys[r];
					double bcost = INF;
					for (int r = row; r < col; ++r){
						if (!mark[row][r]) {
							MPI_Recv(&cost[row][r], 1, MPI_DOUBLE, MPI_ANY_SOURCE, (nkeys + 1) * row + r, MPI_COMM_WORLD, &status);
							// TODO: check status for error 
							mark[row][r] = true;
						}

						if (!mark[r + 1][col]) {
							MPI_Recv(&cost[r + 1][col], 1, MPI_DOUBLE, MPI_ANY_SOURCE, (nkeys + 1) * (r + 1) + col, MPI_COMM_WORLD, &status);
							// TODO: check status for error 
							mark[r + 1][col] = true;
						}
						double rcost = cost[row][r] + cost[r + 1][col] + fcost;	
						if (rcost < bcost)	bcost = rcost;
					}
					cost[row][col] = bcost;
					mark[row][col] = true;
					for (int other = 1; other <= np - 1; ++other) if (other != my_rank){
						MPI_Send(&bcost, 1, MPI_DOUBLE, other, (nkeys + 1)* row + col, MPI_COMM_WORLD);
					}
				}
				// every worker must wait until the step is completed by all workers
				MPI_Barrier(workers_comm);
		}
		
		if (my_rank == 1){
			fprintf(stderr, "Optimal solutions cost = %lf\n", cost[0][nkeys]);				
		}
	}

	MPI_Finalize();
	return 0;
}
