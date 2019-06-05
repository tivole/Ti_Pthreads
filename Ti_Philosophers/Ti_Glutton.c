// (c) Tivole

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5

sem_t sem;
sem_t sem2;
static pthread_mutex_t Ti_Fork[N];
int eat[N];

void *Ti_Philosopher(void *);
void *Ti_Glutton(void *);

int main(void) {

	int i, result;

	pthread_t philosopher[N];
	sem_init(&sem, 0, 0);
	sem_init(&sem2, 0, 0);

	for(i = 0; i < N; i++) pthread_mutex_init(&Ti_Fork[i], NULL);

	for(i = 1; i < (N+1); i++) {
		if ((i == 2) || (i == 4)) result = pthread_create(&philosopher[i-1], NULL, Ti_Glutton, &i);
		else result = pthread_create(&philosopher[i-1], NULL, Ti_Philosopher, &i);

		if(result) { printf(" > Philosopher %i didn't come\n\n", i); return EXIT_FAILURE; }
		else printf(" > Philosopher %i came\n\n", i);
		sem_wait(&sem);
	}

	sem_wait(&sem2);

	result = pthread_join(philosopher[2], NULL);
	if(result) return EXIT_FAILURE;

	for (i = 0; i < N; i++) pthread_mutex_destroy(&Ti_Fork[i]);
	sem_destroy(&sem);
	sem_destroy(&sem2);

  for(i = 1; i < (N+1); i++) printf(" >> Philosopher %i ate %d times.\n\n", i, eat[i-1]);


	printf("\nDONE\n");

  return EXIT_SUCCESS;
}

void *Ti_Philosopher(void *arg) {
	int id = *(int *)arg, hunger = 0;
	sem_post(&sem);
    eat[id] = 0;

	while(1) {
		if(hunger == 20) { printf(" > Philosopher %i dead\n\n", id); break; }

		pthread_mutex_lock(&Ti_Fork[id % N]);
		printf(" > Philosopher %i took left fork\n\n", id);

        if(pthread_mutex_trylock(&Ti_Fork[(id+1) % N])) {
            hunger++;
            printf(" > Philosopher %i couldn't take right fork. Hunger = %d\n\n", id, hunger);
            pthread_mutex_unlock(&Ti_Fork[id % N]);
            printf(" > Philosopher %i put the left fork\n\n", id);
        }
        else {
          hunger = 0;
					sleep(2);
					pthread_mutex_unlock(&Ti_Fork[(id+1) % N]);
					pthread_mutex_unlock(&Ti_Fork[id % N]);
					printf(" > Philosopher %i ate\n\n", id);
          eat[id-1]++;
        }
        sleep(2);
	}
	return arg;
}

void *Ti_Glutton(void *arg) {
	int id = *(int *)arg;
	sem_post(&sem);
  eat[id] = 0;

	while(1) {
		pthread_mutex_lock(&Ti_Fork[id % N]);
		printf(" > Glutton %i took left fork\n\n", id);
		pthread_mutex_lock(&Ti_Fork[(id+1) % N]);
		printf(" > Glutton %i took right fork\n\n", id);

		sem_post(&sem2);
    eat[id-1]++;
    sleep(2);
		sem_wait(&sem2);

		pthread_mutex_unlock(&Ti_Fork[id % N]);
		pthread_mutex_unlock(&Ti_Fork[(id+1) % N]);
		printf(" > Glutton %i put the forks\n\n", id);
		sleep(2);
	}
	return arg;
}
