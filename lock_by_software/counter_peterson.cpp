#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NTHREADS 3


// begin of locking routines 
int level[NTHREADS+2];
int victim[NTHREADS+2];

void start_lock(){
	for (int i = 1; i <= NTHREADS; ++i)
		level[i] = 0;
}

void lock(int id){
	for (int L = 1; L < NTHREADS; ++L){
		level[id] = L;
		victim[L] = id;
		while (1)
		{
			bool canstop = true;
			for (int k = 1; k <= NTHREADS; ++k) if (k != id)
				if (level[k] >= L && victim[L] == id){ 
					 canstop = false;
					 break;
				}
			if (canstop) break;
		}
	}
}

void unlock(int id){
	level[id] = 0;
}

// end of lock routines 

int counter = 0;

void* add(void* ptr){
#ifdef FIX
	lock(*(int *) ptr);
#endif
	int t = counter;
	sleep(1);
	t = t + 1;
	counter = t;
#ifdef FIX
	unlock(*(int *) ptr);
#endif
}

int main(){
	pthread_t ID[NTHREADS];
	start_lock();
	
	for (int i = 1; i <= NTHREADS; ++i)
		if (pthread_create(&ID[i-1], NULL, add, (void *) new int (i))) {
			printf("Error creating thread %d...\n", i);
		}

	for (int i = 0; i < NTHREADS; ++i)
		pthread_join(ID[i], NULL);

	// Counter should be equals to NTHREADS since
	// each thread should add one to conuter
	printf("Counter = %d Expected = %d\n", counter, NTHREADS);
	return 0;
}
