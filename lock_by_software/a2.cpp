#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NTHREADS 10000

int counter = 0;

void* add(void* ptr){
	++counter;
}


int main(){
	pthread_t ID[NTHREADS];
	
	for (int i = 0; i < NTHREADS; ++i)
		if (pthread_create(&ID[i], NULL, add, NULL)) {
			printf("Error creating thread %d...\n", i);
		}

	for (int i = 0; i < NTHREADS; ++i)
		pthread_join(ID[i], NULL);

	// Counter should be equals to NTHREADS since
	// each thread should add one to conuter
	printf("Counter = %d Expected = %d\n", counter, NTHREADS);
	return 0;
}
