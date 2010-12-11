#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#define TAM_BUFFER 5
#define NREADERS 5
#define NWRITERS 1


int buf[TAM_BUFFER];

int nr = 0, nw = 0; // RW: (nr == 0 or nw == 0) and nw <= 1

sem_t e; // controls entry to critical section 


int next_free = 0;

int already_read[TAM_BUFFER];

void* read(void * arg){
	int next_data = 0;
	while (1){
			// real stuff
			// since severall readers could be here, 
			// i am using mutex for consistence of "next_data"

			if (next_data != next_free || (next_data == next_free && already_read[next_data] != NREADERS && already_read[next_data] != -1)){
				printf("Reader %d reading %d\n", *(int *)arg, buf[next_data]);
				already_read[next_data]++;
				next_data = (next_data + 1)%TAM_BUFFER;
			}
			
		}
}


void* write(void * arg){
	while (1){
		// real stuff, writing to data base
		if (already_read[next_free] == -1 || already_read[next_free] == NREADERS){
				already_read[next_free] = 0;
				int data = rand()%100;
				printf("Writer %d producing %d\n", *(int *) arg, data);	
				buf[next_free] = data;
				next_free = (next_free + 1)%TAM_BUFFER;
		}
	}
}


int main(){
	pthread_t reader[NREADERS];
	pthread_t writer[NWRITERS];	
	
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
