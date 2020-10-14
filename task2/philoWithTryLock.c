#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define SLEEP_TIME ( (rand() % 5 + 1) * 1000)

void *philosopher(void *id);

//Shared data - Updated for spinlock system
pthread_spinlock_t *chopstick;

int numOfSeats, numOfTurns;

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Usage: philosophers <number of seats> <number of turns>");
        return 1;
    }

    numOfSeats = strtod(argv[1], NULL);
    numOfTurns = strtod(argv[2], NULL);

    chopstick = calloc(numOfSeats, sizeof(pthread_spinlock_t));
    // set the seed for random number generator
    srand(time(NULL));

    //To use? Refer to spinlock manual
//    pthread_attr_t attr;
//    pthread_attr_init(&attr);
//    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    printf("Start a dinner for %d diners\n", numOfSeats);

    pthread_t philosopher_tid[numOfSeats];

    long i;
    for (i = 0; i < numOfSeats; i++)
        pthread_spin_init(chopstick + i, 0);

    for (i = 0; i < numOfSeats; i++)
        pthread_create(&philosopher_tid[i], NULL, philosopher, (void *) i);

    for (i = 0; i < numOfSeats; i++)
        pthread_join(philosopher_tid[i], NULL);

    for (i = 0; i < numOfSeats; i++)
        pthread_spin_destroy(chopstick + i);

    printf("Dinner is no more.\n");

    return 0;
}

void *philosopher(void *num)
{
    int id = (long) num;

    printf("Philsopher no. %d sits at the table.\n", id);

    // philosophers arrive at the table at various times
    usleep(SLEEP_TIME);

    int i;
    for (i = 0; i < numOfTurns; i++)
    {
        printf("Philsopher no. %d gets hungry for the %d time!\n", id, i + 1);

        //First spin trylock scenario or lock, whats more appropriate?
        //I say, a normal lock is what is most appropriate because we want to grab our chopstick
        //Then we want to try for our right chopstick. If we cant get that too, then put down then left choppy.
        printf("Philsopher no. %d tries to grab chopstick %d\n", id, id);
        if(pthread_spin_trylock(&(chopstick[id])) == 0)
        {
            printf("Philsopher no. %d has grabbed chopstick %d\n", id, id);
            usleep(SLEEP_TIME * 1);
            printf("Philsopher no. %d tries to grab chopstic %d\n", id, (id + 1) % numOfSeats);

            if(pthread_spin_trylock(&(chopstick[(id + 1) % numOfSeats])) == 0)
            {
                printf("Philsopher no. %d grabbed chopstick %d\n", id, (id + 1) % numOfSeats);
                printf("Philsopher no. %d eating\n", id);
                // usleep (DELAY);
                printf("Philsopher no. %d stopped eating\n", id);
                pthread_spin_unlock(&(chopstick[id]));
                printf("Philsopher no. %d has returned chopstick %d\n", id, id);
                usleep(SLEEP_TIME * 1);
                pthread_spin_unlock(&(chopstick[(id + 1) % numOfSeats]));
                printf("Philsopher no. %d has returned chopstick %d\n", id, (id + 1) % numOfSeats);

                printf("Philsopher no. %d finished turn %d\n", id, i + 1);
            }else
            {
                pthread_spin_unlock(&(chopstick[(id)]));
                printf("Philsopher no. %d returns chopstick %d because %d was not available\n", id, id, (id + 1) % numOfSeats);
            }
        }
        else
            {
                printf("Philsopher no. %d could not grab chopstick %d\n", id, id);
            }
        usleep(SLEEP_TIME);
        // YEEEAAAAH !!!
    }

    printf(">>>>>> Philsopher no. %d finished meal. <<<<<<\n", id);

    pthread_exit(NULL);
}

