#include "monitor.h"
#include <pthread.h>

void *philosopher(void *id);
enum STATE{ THINKING, HUNGRY, EATING} state[MAX_PHILOSOPHERS];
pthread_cond_t self[MAX_PHILOSOPHERS];

pthread_mutex_t monitor_mutex = PTHREAD_MUTEX_INITIALIZER;
int numOfSeats, numOfTurns;

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        printf("Usage: philosophers <number of philosophers> <number of turns>");
        return 1;
    }

    numOfSeats = strtod(argv[1], NULL);
    //Test Seat Input against Max Seats allowed
    if(numOfSeats > MAX_PHILOSOPHERS)
    {
        printf("Number of philosophers exceeds Maximum of 200.\n");
        return 1;
    }

    numOfTurns = strtod(argv[2], NULL);

    if(numOfTurns > MAX_TURNS)
    {
        printf("Number of turns exceeds Maximum of 200.\n");
        return 1;
    }

    enum STATE state[numOfSeats];
    pthread_cond_t self[numOfSeats];
    initialization_code();

    srand(time(NULL));

    printf("Start a dinner for %d diners\n", numOfSeats);
    pthread_t philosopher_tid[numOfSeats];

    long i;
    for(i = 0; i < numOfSeats; i++)
    {
        pthread_create(&philosopher_tid[i], NULL, philosopher, (void *) i);
    }

    for(i = 0; i < numOfSeats; i++)
    {
        pthread_join(philosopher_tid[i], NULL);
    }
//Why does this not work?
//    for(i = 0; i < numOfSeats; i++)
//    {
//        pthread_cond_destroy(&self[i]);
//    }
//    pthread_mutex_destroy(&monitor_mutex);

    printf("\nDinner is no more.\n");

    return 0;
}

void *philosopher(void *arg)
{
    int id = (long) arg;
    printf("Philosopher no. %d sits at the table.\n", id);
    int i;
    for(i = 0; i < numOfTurns; i++)
    {
        printf("Philsopher no. %d tries to grab the chopsticks.\n",id);
        pickup(id);
        printf("Philsopher no. %d grabbed the chopsticks.\n",id);
        printf("Philosopher no. %d Eats for the %d time: Yummy yummy, food in tummy <3!\n", id, i);
        usleep(EAT_TIME);
        printf("Philsopher no. %d stopped eating\n", id);
        putdown(id);
        printf("Philsopher no. %d has returned the chopsticks\n", id);
        printf("");
    }
    printf(">>>>>Philosopher no. %d has finished the meal<<<<<\n",id);
    pthread_exit(NULL);
}

void pickup (int i)
{
    pthread_mutex_lock(&monitor_mutex);
    state[i] = HUNGRY;
    test(i);
    if (state[i] != EATING)
        pthread_cond_wait(&(self[i]), &monitor_mutex);
    pthread_mutex_unlock(&monitor_mutex);
}

void putdown (int i)
{
    pthread_mutex_lock(&monitor_mutex);
    state[i] = THINKING;
    // test left and right neighbors
    test((i + (numOfSeats - 1)) % numOfSeats);
    test((i + 1) % numOfSeats);
    pthread_mutex_unlock(&monitor_mutex);
}

void test(int i)
{
    if ( (state[((i + (numOfSeats - 1)) % numOfSeats)] != EATING) &&
         (state[i] == HUNGRY) && (state[(i + 1) % numOfSeats] != EATING) )
    {
        state[i] = EATING ;
        pthread_cond_signal(&(self[i]));
    }
}

void initialization_code()
{
    for (int i = 0; i < numOfSeats; i++)
        state[i] = THINKING;
}
