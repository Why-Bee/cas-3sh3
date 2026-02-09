// PLSem.c
// Part 2: Banking System Problem
// Author: Yash Bhatia (bhatiy1)
// Student ID: 400362372

// Task: Augment part 1 banking system with mutexes AND two semaphores
// Semaphores must ensure deposit doesn't go more than 400 and withdraw doesn't go less than 0

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 10
#define NUM_DEPOSIT 7
#define NUM_WITHDRAW 3

// Variables accessible to all threads
int amount;
pthread_mutex_t lock; // mutex lock for critical section
sem_t max_funds; // semaphore to control space available (max 400)
sem_t min_funds; // semaphore to control funds available (min 0)
void *deposit(void *param); // thread function for deposit
void *withdraw(void *param); // thread function for withdraw

int main(int argc, char *argv[]) {
    // initialize the mutex lock
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Mutex initialization failed\n");
        return 1;
    }

    // initialize semaphores
    // max_funds: initial value = 4 (can accommodate 400/100 = 4 deposits max)
    // min_funds: initial value = 0 (no funds available initially)
    if (sem_init(&max_funds, 0, 4) != 0) {
        printf("Semaphore (max_funds) initialization failed\n");
        pthread_mutex_destroy(&lock);
        return 1;
    }
    if (sem_init(&min_funds, 0, 0) != 0) {
        printf("Semaphore (min_funds) initialization failed\n");
        pthread_mutex_destroy(&lock);
        sem_destroy(&max_funds);
        return 1;
    }

    // check for 1 parameter (the amount to deposit/withdraw, should be 100)
    if (argc != 2) {
        printf("Usage: %s <amount>\n", argv[0]);
        return 1;
    }
    
    amount = 0; // initial amount in the account
    pthread_t threads[NUM_THREADS]; // thread identifiers
    pthread_attr_t attr; // set of attributes for the threads
    // use default thread attributes
    pthread_attr_init(&attr);

    // create 7 deposit threads and 3 withdraw threads
    int thread_count = 0;
    
    // create deposit threads
    for (int i = 0; i < NUM_DEPOSIT; i++) {
        if (pthread_create(&threads[thread_count], &attr, deposit, argv[1]) != 0) {
            printf("Error creating deposit thread %d\n", i);
            return 1;
        }
        thread_count++;
    }
    
    // create withdraw threads
    for (int i = 0; i < NUM_WITHDRAW; i++) {
        if (pthread_create(&threads[thread_count], &attr, withdraw, argv[1]) != 0) {
            printf("Error creating withdraw thread %d\n", i);
            return 1;
        }
        thread_count++;
    }

    // wait for all threads to exit
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // destroy mutex and semaphores
    pthread_mutex_destroy(&lock);
    sem_destroy(&max_funds);
    sem_destroy(&min_funds);

    // Print final amount in the account
    printf("Final amount = %d\n", amount);
    return 0;
}

// Thread function for deposit
void *deposit(void *param) {
    printf("Deposit thread beginning execution\n");
    int deposit_amount = atoi(param); // get the deposit amount from the argument
    
    // wait on max_funds (ensures amount won't exceed 400)
    sem_wait(&max_funds);
    
    pthread_mutex_lock(&lock); // lock the mutex before modifying the amount
    amount += deposit_amount; // perform the deposit
    printf("Deposit amount = %d\n", amount);
    pthread_mutex_unlock(&lock); // unlock the mutex after modification
    
    // post on min_funds (signal that funds are now available)
    sem_post(&min_funds);
    
    pthread_exit(0); // signal that thread is done
}

// Thread function for withdraw
void *withdraw(void *param) {
    printf("Withdraw thread beginning execution\n");
    int withdraw_amount = atoi(param); // get the withdraw amount from the argument
    
    // wait on min_funds (ensures amount won't go below 0)
    sem_wait(&min_funds);
    
    pthread_mutex_lock(&lock); // lock the mutex before modifying the amount
    amount -= withdraw_amount; // perform the withdrawal
    printf("Withdraw amount = %d\n", amount);
    pthread_mutex_unlock(&lock); // unlock the mutex after modification
    
    // post on max_funds (signal that space is now available)
    sem_post(&max_funds);
    
    pthread_exit(0); // signal that thread is done
}