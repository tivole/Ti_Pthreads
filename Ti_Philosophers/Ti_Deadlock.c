// (c) Tivole

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5

sem_t sem;
pthread_mutex_t Ti_Fork[N];

void *Ti_Philosophers_Deadlock(void *);

int main(void) {
	pthread_t philosopher[N];
	int i, result;

	sem_init(&sem, 0, 0);
	for (i = 0; i < N; i++) pthread_mutex_init(&Ti_Fork[i], NULL);

	for(i = 0; i < N; i++) {
		result = pthread_create(&philosopher[i], NULL, &Ti_Philosophers_Deadlock, &sem);
		if(result) { printf(" > (!) Philosopher %i didn't come\n\n", i+1); return EXIT_FAILURE;	}
		else printf(" > Philosopher %i came\n\n", i+1);
	 }

	 sleep(5);

	 for (i = 0; i < N; i++) {
		result = pthread_join(philosopher[i], NULL);
		if(result) { printf(" > (!) Philosopher %i didn't die\n\n", i+1); return EXIT_FAILURE; }
		else printf(" > Philosopher %i is dead \n\n", i+1);
	 }

	sem_destroy(&sem);

	printf("DONE\n");

	return EXIT_SUCCESS;
}

void *Ti_Philosophers_Deadlock(void *arg) {
	int id = *(int*)arg;
	sem_post(&sem);

	while (1) {
		pthread_mutex_lock(&Ti_Fork[id % N]);
		sleep(2);
		printf(" > Philosopher %i took left fork\n\n", id);
		pthread_mutex_lock(&Ti_Fork[(id+1) % N]);
		sleep(2);
		printf(" > Philosopher %i took right fork\n\n", id);
	}

	return NULL;
}
