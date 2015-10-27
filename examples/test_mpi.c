#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]){
	int rank, nprocs;
	printf("Starting test MPI program ...\n");
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
	printf("MPI initialized successfully. I am rank %d out of %d\n",rank,nprocs);
	MPI_Finalize();
	return 0;
}
