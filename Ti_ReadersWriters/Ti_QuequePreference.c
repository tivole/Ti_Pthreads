// (c) Tivole

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_OF_READERS 30
#define NUM_OF_WRITERS 5
#define READERS_PREFERENCE 1
#define WRITERS_PREFERENCE 0

sem_t writer, reader, try;
pthread_mutex_t resource = PTHREAD_MUTEX_INITIALIZER;
int preference = WRITERS_PREFERENCE, waitingReaders = 0, waitingWriters = 0, conditionOfLibrary = 0, writeTimes[NUM_OF_WRITERS], readTimes = 0;

void *Ti_Writers(void*);
void *Ti_Readers(void*);
void *Dispatcher(void*);

int main(void) {

	int id, result;
	pthread_t writers[NUM_OF_WRITERS], readers[NUM_OF_READERS], dispatcher;

	sem_init(&writer, 0, 0);
	sem_init(&reader, 0, 0);
	sem_init(&try, 0, 0);

	result = pthread_create(&dispatcher, NULL, Dispatcher, NULL);
	if(result) { printf("(!) Dispatcher didn't come.\n\n"); return EXIT_FAILURE; }
	else printf("[{\tDispatcher came.\t\t}]\n");


	for(id = 1; id < (NUM_OF_WRITERS+1); id++) {
		result = pthread_create(&writers[id-1], NULL, Ti_Writers, &id);
		if(result) { printf("(!) Writer %i didn't come.\n\n", id); return EXIT_FAILURE; }
		else printf("[{\tWriter %i came.\t\t}]\n", id);
		sem_wait(&try);
	}


	for(id = 1; id < (NUM_OF_READERS+1); id++) {
		result = pthread_create(&readers[id-1], NULL, Ti_Readers, &id);
		if(result) { printf("(!) Reader %i didn't come.\n\n", id); return EXIT_FAILURE; }
		else printf("[{\tReader %i came.\t\t}]\n", id);
		sem_wait(&try);
	}

	sleep(60);

	sem_destroy(&writer);
	sem_destroy(&reader);
	sem_destroy(&try);
	pthread_mutex_destroy(&resource);

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

	/****************       *******       ****************/



	printf("\n\t\tDONE!\n");

	return EXIT_SUCCESS;
}

void *Dispatcher(void*arg) {

	while(1) {
		if(((double)waitingReaders/NUM_OF_READERS) > ((double)waitingWriters/NUM_OF_WRITERS)) {
			preference = READERS_PREFERENCE;
		}else {
			preference = WRITERS_PREFERENCE;
		}
		sleep(3);
	}

	return NULL;
}

void *Ti_Writers(void* arg) {
	int id = *(int *)arg;
	sem_post(&try);
	writeTimes[id-1] = 0;
	while(1) {
		pthread_mutex_lock(&resource);
		if(!conditionOfLibrary) {
			conditionOfLibrary = -1;
			printf("\033[31;1m<<<\tWriter %i is writing\t>>>\033[0m\t\tin queque [%d] writers", id, waitingWriters);
			if(preference == READERS_PREFERENCE) printf("\t\t\033[35;1mR\033[0m\n");
			else printf("\t\t\033[32;1mW\033[0m\n");
			pthread_mutex_unlock(&resource);
			writeTimes[id-1]++;

			/// sleep
			if(preference == READERS_PREFERENCE) sleep(4);
			else sleep(2);

			pthread_mutex_lock(&resource);
			conditionOfLibrary = 0;
			pthread_mutex_unlock(&resource);

			if(preference == READERS_PREFERENCE) {
				sem_post(&writer);
				sem_post(&reader);

				/// sleep
				sleep(3);
			}else {
				sem_post(&reader);
				sem_post(&writer);

				/// sleep
				sleep(2);
			}
		} else {
			waitingWriters++;
			pthread_mutex_unlock(&resource);
			sem_wait(&writer);
			pthread_mutex_lock(&resource);
			waitingWriters--;
			pthread_mutex_unlock(&resource);
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
			if(waitingWriters == 0 || preference != WRITERS_PREFERENCE) {
				conditionOfLibrary++;
				pthread_mutex_unlock(&resource);
				if(waitingToGo) { sem_post(&reader);  waitingToGo = 0; }
				pthread_mutex_lock(&resource);
				printf("\033[36;1m {\tReader %i is reading\t}\033[0m\t\tin queque [%d] readers", id,  waitingReaders);
				if(preference == READERS_PREFERENCE) printf("\t\t\033[35;1mR\033[0m\n");
				else printf("\t\t\033[32;1mW\033[0m\n");
				pthread_mutex_unlock(&resource);
				readTimes++;

				/// sleep
				if(preference == READERS_PREFERENCE) sleep(1.2);
				else sleep(2);

				pthread_mutex_lock(&resource);
				conditionOfLibrary--;
				if(!conditionOfLibrary) sem_post(&writer);
				pthread_mutex_unlock(&resource);

				/// sleep
				if(preference == READERS_PREFERENCE) sleep(0.5);
				else sleep(2);
			}else {
				goto here;
			}
		} else {
			here:
			waitingReaders++;
			pthread_mutex_unlock(&resource);
			waitingToGo = 1;
			sem_wait(&reader);
			pthread_mutex_lock(&resource);
			waitingReaders--;
			pthread_mutex_unlock(&resource);
		}
	}
	return NULL;
}
