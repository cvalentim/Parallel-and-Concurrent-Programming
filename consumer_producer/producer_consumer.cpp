#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#define TAM_BUFFER 5
#define NREADERS 5
#define NWRITERS 1

/*
	Semaphore:
	sem_init --> gives an initial value to semaphore	
	sem_wait --> "P" operation
	sem_post --> "V" operation
*/

int buf[TAM_BUFFER];

int p = 0, c = 0;
int nr = 0, nw = 0; // RW: (nr == 0 or nw == 0) and nw <= 1

sem_t e, // controls entry to critical section 
	r, // used to delay readers
	w; // used to delay writers

int dr = 0, // number of delayed readers
	dw = 0; // number of delayed writers

int next_free = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int already_read[TAM_BUFFER];


void* read(void * arg){
	int next_data =0;

	while (1){
			// <await (nw == 0) nr = nr + 1; >
			sem_wait(&e);
			if (nw > 0) {
				++dr;
				sem_post(&e);
				sem_wait(&r);
			}
			++nr;

			if (dr > 0) {
				--dr;
				sem_post(&r);
			}
			else sem_post(&e);
			
			// real stuff
			// since severall readers could be here, 
			// i am using mutex for consistence of "next_data"

			pthread_mutex_lock(&mutex);
			if (next_data != next_free || (next_data == next_free && already_read[next_data] != NREADERS && already_read[next_data] != -1)){
				printf("Reader %d reading %d\n", *(int *)arg, buf[next_data]);
				already_read[next_data]++;
				next_data = (next_data + 1)%TAM_BUFFER;
			}
		
			pthread_mutex_unlock(&mutex);
			
			// <nr = nr - 1; >
			sem_wait(&e);
			--nr;
			if (nr == 0 && dw > 0) {
				--dw;
				sem_post(&w);
			}
			else sem_post(&e);
	}
}


void* write(void * arg){
	while (1){
		// <await (nr == 0 and nw == 0) nw = nw + 1;>
		sem_wait(&e); // wait for mutual exclusion

		if (nr > 0 || nw > 0){ // is there any active readers or writers?
			++dw; // include me in the writers waiting queue 
			sem_post(&e); // release the mutual exclusion
			sem_wait(&w); // wait to someone else give the baton
		}
		++nw; //oh, yeah, i am an active writer 

		// SIGNAL, let see who is waiting?
		sem_post(&e); // i dont need mutual exclusion anymore as i am an active writer

		
		// real stuff, writing to data base
		// no mutex needed here since only one writer at time 
		// can be here
		pthread_mutex_lock(&mutex);
		if (already_read[next_free] == -1 || already_read[next_free] == NREADERS){
				already_read[next_free] = 0;
				int data = rand()%100;
				printf("Writer %d producing %d\n", *(int *) arg, data);	
				buf[next_free] = data;
				next_free = (next_free + 1)%TAM_BUFFER;
		}
		pthread_mutex_unlock(&mutex);
		// <nw = nw - 1; >
		sem_wait(&e); //i need mutual exclusion again
		--nw; //not more an active writer

		// SIGNAL
		if (dr > 0) // some reader waiting?
		{
			--dr; //no more
			sem_post(&r); // release some reader
		}else if (dw > 0) {
			--dw;
			sem_post(&w);
		} else sem_post(&e);
	}
}


int main(){
	pthread_t reader[NREADERS];
	pthread_t writer[NWRITERS];	
	
	sem_init(&e, 0, 1);
	sem_init(&r, 0, 0);
	sem_init(&w, 0, 0);

	memset(already_read, -1, sizeof(already_read));
	// creates all readers threads
	for (int i = 0; i < NREADERS; ++i)
		if (pthread_create(&reader[i], NULL, read, (void *) new int(i))){
				printf("Error creating reader %d ...\n", i);
		}	

	// creates all writers threads
	for (int i = 0; i < NWRITERS; ++i)
		if (pthread_create(&writer[i], NULL, write, (void *) new int(i))){
				printf("Error creating writer %d ...\n", i);
		}

	// wait for all readers to finish
	for (int i = 0; i < NREADERS; ++i)
		pthread_join(reader[i], NULL);

	// wait for all writers to finish
	for (int i = 0; i < NWRITERS; ++i)
		pthread_join(writer[i], NULL);
			
	return 0;
}
