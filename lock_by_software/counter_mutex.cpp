#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NTHREADS 10000

int counter = 0;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void* add(void* ptr){
	pthread_mutex_lock(&mutex1);
	int t = counter;
	//sleep(1);
	t = t + 1;
	counter = t;
	pthread_mutex_unlock(&mutex1);
}

int main(){
	pthread_t ID[NTHREADS];
	
	for (int i = 0; i < NTHREADS; ++i)
		if (pthread_create(&ID[i], NULL, add, (void *) &i)) {
			printf("Error creating thread %d...\n", i);
		}

	for (int i = 0; i < NTHREADS; ++i)
		pthread_join(ID[i], NULL);

	// Counter should be equals to NTHREADS since
	// each thread should add one to conuter
	printf("Counter = %d Expected = %d\n", counter, NTHREADS);
	return 0;
}
