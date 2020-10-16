#include "monitor.h"


enum { THINKING, HUNGRY, EATING} state[5];
pthread_cond_t self[5];

//Sets the current state to hungry.
//test
// - if the person to my left is not eating, I am hungy, and the person to my right is not eating.
// - then set my state to eating and self signals.
void pickup (int i)
{
    state[i] = HUNGRY;
    test(i);
    if (state[i] != EATING)
        pthread_cond_wait(&(self[i]), monitor_mutex);
}

void putdown (int i)
{
    state[i] = THINKING;

    // test left and right neighbors
    test((i + 4) % 5);
    test((i + 1) % 5);

}

void test(int i)
{
    if ( (state[(i + 4) % 5] != EATING) &&
         (state[i] == HUNGRY) && (state[(i + 1) % 5] != EATING) )
    {
        state[i] = EATING ;
        pthread_cond_signal(&(self[i]));
    }
}

void initialization_code()
{
    for (int i = 0; i < 5; i++)
        state[i] = THINKING;
}