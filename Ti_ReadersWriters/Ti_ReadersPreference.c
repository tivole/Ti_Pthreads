// (c) Tivole

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_OF_WRITERS 3
#define NUM_OF_READERS 30

sem_t writer, reader, try;
pthread_mutex_t resource = PTHREAD_MUTEX_INITIALIZER;
int conditionOfLibrary = 0, writeTimes[NUM_OF_WRITERS], readTimes = 0;

void *Ti_Writers(void*);
void *Ti_Readers(void*);

int main(void) {

	int id, result;
	pthread_t writers[NUM_OF_WRITERS], readers[NUM_OF_READERS];
	sem_init(&writer, 0, 0);
	sem_init(&reader, 0, 0);
	sem_init(&try, 0, 0);

	srand(time(NULL));

	for (id = 1; id < (NUM_OF_READERS+1); id++) {
		result = pthread_create(&readers[id-1], NULL, Ti_Readers, &id);
		if(result) { printf("(!) Reader %i didn't come.\n\n", id); return EXIT_FAILURE; }
		else printf("[{\tReader %i came.\t\t}]\n", id);
		sem_wait(&try);
	}

	for (id = 1; id < (NUM_OF_WRITERS+1); id++) {
		result = pthread_create(&writers[id-1], NULL, Ti_Writers, &id);
		if(result) { printf("(!) Writer %i didn't come.\n\n", id); return EXIT_FAILURE; }
		else printf("[<\tWriter %i came.\t\t>]\n", id);
		sem_wait(&try);
	}

	sleep(60);

	/****************       Results       ****************/

	printf("\n\n");
	for(id = 0; id < 42; id++) printf("-");
	printf("\n");
	for(id = 1; id < (NUM_OF_WRITERS+1); id++) {
		printf("|\tWriter %i wrote %d times.\t\t|\n", id, writeTimes[id-1]);
	}
	for(id = 0; id < 42; id++) printf("-");
	printf("\n|\tReaders are read %d times.\t|\n", readTimes);
	for(id = 0; id < 42; id++) printf("-");
	printf("\n\n");

	sem_destroy(&writer);
	sem_destroy(&reader);
	sem_destroy(&try);
	pthread_mutex_destroy(&resource);

	printf("\n\t\tDONE!\n");

	return EXIT_SUCCESS;
}

void *Ti_Writers(void* arg) {
	int id = *(int *)arg;
	sem_post(&try);
	writeTimes[id-1] = 0;
	while(1) {
		pthread_mutex_lock(&resource);
		if(!conditionOfLibrary) {
			conditionOfLibrary = -1;
			pthread_mutex_unlock(&resource);
			printf("\033[31;1m<<<\tWriter %i is writing\t>>>\033[0m\n", id);
			writeTimes[id-1]++;
			sleep(1);
			pthread_mutex_lock(&resource);
			conditionOfLibrary = 0;
			pthread_mutex_unlock(&resource);
			sem_post(&writer);
			sem_post(&reader);
      sleep(2);
		} else {
			pthread_mutex_unlock(&resource);
			sem_wait(&writer);
		}
	}
	return NULL;
}

void *Ti_Readers(void* arg) {
	int id = *(int *)arg, waitingToGo = 0;
	sem_post(&try);
	while(1) {
		pthread_mutex_lock(&resource);
		if(conditionOfLibrary >= 0) {
			conditionOfLibrary++;
			pthread_mutex_unlock(&resource);
			if(waitingToGo) { sem_post(&reader); waitingToGo = 0; }
			printf("\033[32;1m {\tReader %i is reading\t}\033[0m\n", id);
			readTimes++;
			sleep(rand()%2 + 1);
			pthread_mutex_lock(&resource);
			conditionOfLibrary--;
			if(!conditionOfLibrary) sem_post(&writer);
			pthread_mutex_unlock(&resource);
			sleep(rand() % 3 + 3);
		} else {
			pthread_mutex_unlock(&resource);
			waitingToGo = 1;
			sem_wait(&reader);
		}
	}
	return NULL;
}
