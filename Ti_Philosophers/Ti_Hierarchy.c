// (c) Tivole

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5

sem_t sem;
static pthread_mutex_t Ti_Fork[N];
int eat[N];

void *Ti_Philosophers_Hierarchy(void *);

int main(void) {

	int i, result;
	pthread_t philosopher[N];
	sem_init(&sem, 0, 0);

	for(i = 0; i < N; i++) pthread_mutex_init(&Ti_Fork[i], NULL);

	for(i = 1; i < (N+1); i++) {
		result = pthread_create(&philosopher[i-1], NULL, Ti_Philosophers_Hierarchy, &i);
		if(result) return EXIT_FAILURE;
		sem_wait(&sem);
	}

	sleep(30);

	for(i = 0; i < N; i++) printf(" >> Philosopher %i ate %d times.\n\n", i, eat[i]);

	sem_destroy(&sem);

	for(i = 0; i < N; i++) pthread_mutex_destroy(&Ti_Fork[i]);

	printf("DONE\n");

	return EXIT_SUCCESS;
}

void *Ti_Philosophers_Hierarchy(void *arg) {
	int id = *(int *)arg;
	sem_post(&sem);
    eat[id] = 0;

	while(1) {
		if((id)%2) {
			pthread_mutex_lock(&Ti_Fork[id % N]);
			printf(" > Philosopher %i took the left fork\n\n", id);
			pthread_mutex_lock(&Ti_Fork[(id+1) % N]);
			printf(" > Philosopher %i took the right fork\n\n", id);
			sleep(2);
			printf(" > Philosopher %i ate\n\n", id);
			eat[id-1]++;
			printf(" > Philosopher %i put the forks\n\n", id);
			pthread_mutex_unlock(&Ti_Fork[id % N]);
			pthread_mutex_unlock(&Ti_Fork[(id+1) % N]);
			sleep(2);
		} else {
			pthread_mutex_lock(&Ti_Fork[(id+1) % N]);
			printf(" > Philosopher %i took the right fork\n\n", id);
			pthread_mutex_lock(&Ti_Fork[id % N]);
			printf(" > Philosopher %i took the left fork\n\n", id);
			sleep(2);
			printf(" > Philosopher %i ate\n\n", id);
			eat[id-1]++;
			printf(" > Philosopher %i put the forks\n\n", id);
			pthread_mutex_unlock(&Ti_Fork[(id+1) % N]);
			pthread_mutex_unlock(&Ti_Fork[id % N]);
			sleep(2);
		}
	}

	return NULL;
}
