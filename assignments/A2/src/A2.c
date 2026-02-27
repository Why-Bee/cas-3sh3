/*
Assignment 2: The Sleeping Teaching Assistant

Author: Yash Bhatia - bhatiy1 - 400362372
Author: Khawja Labib - labibk - 400356836

Date: 2026-02-25

Run instructions:

1. make
2. make run
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

/*-----DEFINES------*/
#define NUM_CHAIRS 3

#define MAX_PROG 10
#define MIN_PROG 3
#define MAX_HELP 5
#define MIN_HELP 1
#define END_PROG 60 // Time a student spends programming before the simulation ends

typedef enum {
    PROGRAMMING,
    WAITING,
    BEING_HELPED
} StudentState;

typedef enum {
    SLEEPING,
    HELPING
} TAState;

void student_thread(void *arg);
void ta_thread();

sem_t student_waiting; // Semaphore to see if a student is waiting
sem_t ta_helping; // Semaphore to indicate TA is helping a student
pthread_mutex_t queue_mutex; // Mutex for accessing the hallway queue
pthread_mutex_t working_mutex; // Mutex for accessing the count of students working

// Custom circular queue
// This queue has 1 position for the student being helped and NUM_CHAIRS positions for the students waiting in the hallway. 
// The TA will always pull from the front of the queue, and students will always push to the rear of the queue.
typedef struct {
    int students[NUM_CHAIRS+1];
    int front;
    int rear;
    int count;
} HallwayQueue;

int pushToQueue(HallwayQueue *q, int student_id) {
    if (q->count == NUM_CHAIRS+1) {
        return -1; // Queue is full
    }
    q->students[q->rear] = student_id;
    q->rear = (q->rear + 1) % (NUM_CHAIRS + 1);
    q->count++;
    return 0; // Success
}

int popFromQueue(HallwayQueue *q) {
    if (q->count == 0) {
        return -1; // Queue is empty
    }
    int student_id = q->students[q->front];
    q->front = (q->front + 1) % (NUM_CHAIRS + 1);
    q->count--;
    return student_id; // Return the student ID
}

int peekQueue(HallwayQueue *q) {
    if (q->count == 0) {
        return -1; // Queue is empty
    }
    return q->students[q->front]; // Return the student ID at the front of the queue
}

int getHallwayCount(HallwayQueue *q) {
    return q->count;
}

int isQueueFull(HallwayQueue *q) {
    return q->count == NUM_CHAIRS+1;
}

HallwayQueue hallway;
bool shutdown_ta_signal = false; // Signal to indicate when the TA should shut down after all students are done programming

/*-----CODE---------*/

// Function to get the number of students from user input
int getStudentCount() {
    int n;
    printf("Enter the number of students: ");
    scanf("%d", &n);
    if (n <= 0) {
        printf("Number of students must be greater than 0. Exiting.\n");
        exit(1);
    }
    return n;
}

// TA thread function

void ta_thread() {
    // State machine
    TAState state = SLEEPING;

    while (1) {
        switch (state) {
            case SLEEPING: {
                printf("TA is sleeping.\n");
                sem_wait(&student_waiting); // Wait for a student to arrive
                printf("TA wakes up to help a student.\n");
                state = HELPING;
                break;
            }
            case HELPING: {
                // First, check if the shutdown was asserted by the student.
                pthread_mutex_lock(&working_mutex);
                if (shutdown_ta_signal) {
                    pthread_mutex_unlock(&working_mutex);
                    printf("TA is shutting down as all students are done programming.\n");
                    return; // Exit the thread
                }
                pthread_mutex_unlock(&working_mutex); // in case the shutdown signal was not asserted, continue with helping the student

                // Simulate helping a student
                // Step 1: Get the next student from the hallway queue
                pthread_mutex_lock(&queue_mutex);
                int student_id = peekQueue(&hallway); // Get the student ID at the front of the queue without removing it
                pthread_mutex_unlock(&queue_mutex);
                if (student_id == -1) {
                    printf("Error: No student to help, but TA is in HELPING state.\n");
                    exit(1);
                }

                // Step 2: Help the student
                printf("TA is helping student %d.\n", student_id);
                sleep(rand() % (MAX_HELP - MIN_HELP + 1) + MIN_HELP);
                printf("TA finished helping student %d.\n", student_id);

                // Remove the student from the queue after helping
                pthread_mutex_lock(&queue_mutex);
                popFromQueue(&hallway); // Remove the student from the queue
                pthread_mutex_unlock(&queue_mutex);

                sem_post(&ta_helping); // Signal that TA is done helping
                
                // Step 3: Check if there are more students waiting
                pthread_mutex_lock(&queue_mutex);
                if (getHallwayCount(&hallway) > 0) {
                    printf("TA sees more students waiting and continues helping.\n");
                    state = HELPING; // Continue helping the next student
                } else {
                    printf("TA goes back to sleep.\n");
                    state = SLEEPING; // Go back to sleep
                }
                pthread_mutex_unlock(&queue_mutex);
                break;
            }
        }
    }
}

void student_thread(void *arg) {
    int student_id = *((int *)arg);
    int programming_done = 0;
    free(arg); // Free the allocated memory for student ID

    StudentState state = PROGRAMMING;

    while (1) { // Student state machine
        switch (state) {
            case PROGRAMMING: {
                printf("Student %d is programming.\n", student_id);
                int programming_time = rand() % (MAX_PROG - MIN_PROG + 1) + MIN_PROG;
                sleep(programming_time); // Simulate programming time
                programming_done += programming_time;
                if (programming_done >= END_PROG) {
                    printf("Student %d has finished programming and leaves the simulation.\n", student_id);
                    return; // Exit the thread
                }
                printf("Student %d needs help and goes to the hallway.\n", student_id);
                state = WAITING;
                break;
            }
            case WAITING: {
                pthread_mutex_lock(&queue_mutex);
                if (!isQueueFull(&hallway)) { // There is room in the queue
                    pushToQueue(&hallway, student_id); // Join the hallway queue

                    // Although the student is "in hallway", there is a case where they need to just wake up the TA without waiting.
                    // As the TA implementation always pulls from the queue, we can pretend that the queue was not joined

                    if (getHallwayCount(&hallway) == 1) { // If this is the first student in the queue, wake up the TA
                        pthread_mutex_unlock(&queue_mutex);
                        sem_post(&student_waiting); // Signal that a student is waiting
                        printf("Student %d wakes up the TA.\n", student_id);
                    } else {
                        pthread_mutex_unlock(&queue_mutex);
                        printf("Student %d is waiting in the hallway. Queue count: %d\n", student_id, getHallwayCount(&hallway)-1);
                    }
                    state = BEING_HELPED; // Wait to be helped
                } else { // There are no chairs left
                    pthread_mutex_unlock(&queue_mutex);
                    printf("Student %d finds the hallway full and goes back to programming.\n", student_id);
                    state = PROGRAMMING; // Go back to programming
                }
                break;
            }
            case BEING_HELPED: {
                sem_wait(&ta_helping); // Wait for TA to finish helping
                printf("Student %d got help and goes back to programming.\n", student_id);
                state = PROGRAMMING; // Go back to programming
                break;
            }
        }
    }
}

int main() {
    srand(time(NULL));

    // Accept number of students from user
    int n = getStudentCount();

    // Initialize the hallway queue
    hallway.front = 0;
    hallway.rear = 0;
    hallway.count = 0;
    // Initialize semaphores
    sem_init(&student_waiting, 0, 0);
    sem_init(&ta_helping, 0, 0);
    // Initialize mutex
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_mutex_init(&working_mutex, NULL);

    // Create TA thread
    pthread_t ta;
    pthread_create(&ta, NULL, (void *)ta_thread, NULL);
    // Create student threads
    pthread_t students[n];
    for (int i = 0; i < n; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1; // Student IDs start from 1
        pthread_create(&students[i], NULL, (void *)student_thread, id);
    }

    // Wait for student threads to finish
    // TODO simulation end condition
    for (int i = 0; i < n; i++) {
        pthread_join(students[i], NULL);
    }
    sem_post(&student_waiting); // In case the TA is sleeping, wake it up to check the shutdown signal
    shutdown_ta_signal = true; // Signal the TA to shut down after all students are done programming
    // When the last student finishes programming, the TA will be signaled to shut down and will exit its thread, so we can join it here.
    pthread_join(ta, NULL);
}
