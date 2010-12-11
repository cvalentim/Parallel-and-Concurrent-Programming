#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <algorithm>

using namespace std;

#define NTHREADS 3

bool flag[NTHREADS];
int label[NTHREADS];


void start_bakery(){
	for (int i = 0; i < NTHREADS; ++i)
		flag[i] = false, label[i] = 0;
}

void lock(int id){
	flag[id] = true;
	int id_label = -1;
	for (int i = 0; i < NTHREADS; ++i)  id_label = max(id_label, label[i]);
	label[id] = ++id_label;
	while (1){
		bool can_stop = true;
		for (int k = 0; k < NTHREADS; ++k) if (flag[k])
			if (label[id] > label[k] || (label[id] == label[k] && id > k)) can_stop = false;
		if (can_stop) break;
	}
}

void unlock(int id){
	flag[id] = false;
}

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
	
	for (int i = 0; i < NTHREADS; ++i)
		if (pthread_create(&ID[i], NULL, add, (void *) new int(i))) {
			printf("Error creating thread %d...\n", i);
		}

	for (int i = 0; i < NTHREADS; ++i)
		pthread_join(ID[i], NULL);

	// Counter should be equals to NTHREADS since
	// each thread should add one to counter
	printf("Counter = %d Expected = %d\n", counter, NTHREADS);
	return 0;
}
