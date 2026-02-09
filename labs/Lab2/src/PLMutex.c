// PLMutex.c
// Part 1: Banking System Problem
// Author: Yash Bhatia (bhatiy1)
// Student ID: 400362372

// Task: Create the banking system while protecting critical sections
// Use Mutex locking and pthread API

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 6

// Variables accessible to all threads
int amount;
pthread_mutex_t lock; // mutex lock for critical section
void *deposit(void *param); // thread function for deposit
void *withdraw(void *param); // thread function for withdraw

int main(int argc, char *argv[]) {
    // initialize the mutex lock
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Mutex initialization failed\n");
        return 1;
    }

    // check for 2 parameters
    if (argc != 3) {
        printf("Usage: %s <deposit amount> <withdraw amount>\n", argv[0]);
        return 1;
    }
    
    amount = 0; // initial amount in the account
    pthread_t threads[NUM_THREADS]; // thread identifiers
    pthread_attr_t attr; // set of attributes for the threads
    // use default thread attributes
    pthread_attr_init(&attr);

    // create threads for deposit and withdraw
    for (int i = 0; i < NUM_THREADS; i++) {
        if (i % 2 == 0) { // even index: deposit thread
            pthread_create(&threads[i], &attr, deposit, argv[1]);
        } else { // odd index: withdraw thread
            pthread_create(&threads[i], &attr, withdraw, argv[2]);
        }
    }

    // wait for all threads to exit
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&lock);

    // Print final amount in the account
    printf("Final amount = %d\n", amount);
    return 0;
}

// Thread function for deposit
void *deposit(void *param) {
    int deposit_amount = atoi(param); // get the deposit amount from the argument
    pthread_mutex_lock(&lock); // lock the mutex before modifying the amount
    amount += deposit_amount; // perform the deposit
    printf("Deposit amount = %d\n", amount);
    pthread_mutex_unlock(&lock); // unlock the mutex after modification
    pthread_exit(0); // signal that thread is done
}

// Thread function for withdraw
void *withdraw(void *param) {
    int withdraw_amount = atoi(param); // get the withdraw amount from the argument
    pthread_mutex_lock(&lock); // lock the mutex before modifying the amount
    amount -= withdraw_amount; // perform the withdrawal
    printf("Withdraw amount = %d\n", amount);
    pthread_mutex_unlock(&lock); // unlock the mutex after modification
    pthread_exit(0); // signal that thread is done
}