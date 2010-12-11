#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

int flag[2];
int victim;

void lock(int id){
	flag[id] = true;
	victim = id;
	while (flag[abs(1 - id)] && victim == id){};	
}

void unlock(int id){
	flag[id] = false;
}

int counter = 0;

void *add(void * ptr){
	printf("Thread number %ld\n", *(long int *)ptr);
	lock(*(int *)ptr);
	int temp = counter;
	temp = temp + 1;
	sleep(1);
	counter = temp;
	unlock(*(int *)ptr);
}


int main(){
	pthread_t thread1, thread2;
	int ret1, ret2;
	int id1 = 0, id2 = 1;
	ret1 = pthread_create(&thread1, NULL, add, (void *) &id1);
	ret2 = pthread_create(&thread2, NULL, add, (void *) &id2);
	
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	
	printf("Counter = %d\n", counter);
	return 0;
}
