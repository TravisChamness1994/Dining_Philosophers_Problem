#include "monitor.h"

void *philosopher(void *id);

pthread_mutex_t *monitor_mutex = PTHREAD_MUTEX_INITIALIZER;
//Are philosophers static?
int numOfSeats, numOfTurns;

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        printf("Usage: philosophers <number of philosophers> <number of turns>");
        return 1;
    }

    numOfSeats = strtod(argv[1], NULL);
    numOfTurns = strtod(argv[2], NULL);


}