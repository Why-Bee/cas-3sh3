// Given "pthreads" API example.
// Calculates sum

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int sum; // shared between threads
void *runner(void *param); // the thread function

int main(int argc, char *argv[]) {
    pthread_t tid; // the thread identifier
    pthread_attr_t attr; // set of attributes for the thread

    // use default thread attributes
    pthread_attr_init(&attr);
    // create the thread
    pthread_create(&tid, &attr, runner, argv[1]);
    // wait for the thread to exit
    pthread_join(tid, NULL);

    printf("Sum = %d\n", sum);
}

// The thread will begin control in this function
void *runner(void *param) {
    int i, upper = atoi(param); // get the upper limit from the argument
    sum = 0;

    for (i = 1; i <= upper; i++) //  calculate sum of 1, 2, ..., upper
        sum += i;

    pthread_exit(0); // signal that thread is done
}