// (c) Tivole

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define PLACES_ON_LOBBY 8
#define PLACES_ON_SOFA 10
#define NUM_OF_CLIENTS 20

sem_t try, lobby, sofa, armchair, sit, done;

void *Ti_Client(void *arg);
void *Ti_Barber(void *arg);

int main() {
	pthread_t Barber, Client[NUM_OF_CLIENTS];
	int id, result;

	sem_init(&try, 0, 0);
	sem_init(&lobby, 0, PLACES_ON_LOBBY);
	sem_init(&sofa, 0, PLACES_ON_SOFA);
	sem_init(&armchair, 0, 1);
	sem_init(&sit, 0, 0);
	sem_init(&done, 0, 0);

	result = pthread_create(&Barber, NULL, &Ti_Barber, NULL);
	if(result) { printf("(!) Barber didn't come.\n"); return EXIT_FAILURE; }
	else printf("[\tBarber came. \t\t]\n");

	for(id = 1; id <= (NUM_OF_CLIENTS+1); id++) {
		result = pthread_create(&Client[id-1], NULL, &Ti_Client, &id);
		if(result) { printf("(!) Client %i didn't come.\n", id);  return EXIT_FAILURE; }
		else printf("[\tClient %i came.\t\t]\n", id);
		sem_wait(&try);
	}

	for (id = 0; id < NUM_OF_CLIENTS; id++) pthread_join(Client[id], NULL);

	sem_destroy(&try);
	sem_destroy(&lobby);
	sem_destroy(&sofa);
	sem_destroy(&armchair);
	sem_destroy(&sit);
	sem_destroy(&done);

	return EXIT_SUCCESS;
}

void *Ti_Barber(void *arg) {
	while(1) {
		sem_wait(&sit);
		sleep(3);
		sem_post(&done);
	}
}

void *Ti_Client(void *arg) {
	int id = *(int*)arg;
	sem_post(&try);
	sem_wait(&lobby);
	printf("\033[35;1m{\tClient %i enter lobby.\t}\033[0m\n", id);
	sleep(1);
	sem_wait(&sofa);
	sem_post(&lobby);
	printf("\033[34;1m<\tClient %i sit on sofa.\t>\033[0m\n", id);
	sleep(1);
	sem_wait(&armchair);
	sem_post(&sofa);
	sem_post(&sit);
	printf("\033[36;1m(Client %i is getting a haircut.\t)\033[0m\n", id);
	sem_wait(&done);
	printf("\033[32;1m\n<<<\tClient %i had a haircut.\t>>>\033[0m\n\n", id);
	sem_post(&armchair);
	return NULL;
}
