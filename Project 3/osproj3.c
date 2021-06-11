//********************************************************************
//
// Student: Lucas Cardoza
// Course: CSC 360 Operating Systems
// Programming Project #3: Producer/Consumer
// Due Date: Friday, September 25, 2020, 11:59 PM
// Instructor: Dr. Siming Liu
//
//********************************************************************


#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "buffer.h"
#include <time.h>
#include <string.h> 


// Function prototypes
void *producer(void *thread_data);
void *consumer(void *thread_data);
bool isPrime(int);
void calcTimer(long int *first, long int *last);


// Globals  
#define MAX_THREADS 50  // Maximum threads allowed. This is just a random number I chose.
bool isFinished = false;    // Control for main to stop threads
double timer;   // Total for all timers
int divisor = 100;  // Divisor to keep random numbers under 100


// Struct to hold data used by threads
struct Thread_Data
{
    int tid;  // Thread's counter number
    int thdSleep;   // Time for thread to sleep
    int count;  // Tracker for number of items produced or consumed
    int exitValue;  // Return value of thread exit
    char *message;  // User request for snapshots
};


// Initialize Producer and Consumer structs
struct Thread_Data thread_data_array[MAX_THREADS];


int main(int argc, char *argv[])    // Get command line input form user
{   
    clock_t startC, endC; // Timer for simulation.
    startC = clock();   // Start timer for main
    pthread_mutex_init(&mutex, NULL);   // Initialize mutex
    int threadCount;     // Thread counter
    int mainSleep = atoi(argv[1]);  // Getting command line input for time main will sleep
    int threadSleep = atoi(argv[2]);    // Getting command line input for time threads will sleep
    int numProThd = atoi(argv[3]);      // Getting command line input for number of Producer threads
    int numConThd = atoi(argv[4]);      // Getting command line input for number of Consumer threads
    char *messages = argv[5];       // Getting command line input for request to print out snapshots
    sem_init(&empty, 0, 5);   //Initialize semaphore for buffer empty signal (&empty), (0) shared with other threads, (5) set empty spaces to 5
    sem_init(&full, 0, 0);   //Initialize semaphore for buffer full signal (&full), (0) shared with other threads, (0) set full spaces to 0
    int index = (numProThd + 1);

    if ((numProThd + numConThd) > MAX_THREADS)  // Checking for maximum number of threads
    {
        printf("Max threads is limited to a total of %d\n", MAX_THREADS);
    }
    else if (numProThd > MAX_THREADS)    // Checking for maximum number of Producer threads
    {
        numProThd = MAX_THREADS;
        printf("Max threads is limited to a total of %d\n", MAX_THREADS);
    }
    else if (numConThd > MAX_THREADS)   // Checking for maximum number of Consumer threads
    {
        numConThd = MAX_THREADS;
        printf("Max threads is limited to a total of %d\n", MAX_THREADS - numProThd);
    }    

    if (strcmp(messages, "yes")== 0)
    {
        printf("\nStarting Threads...\n");
        printBuff();
    }
    else
    {
        printf("\nStarting Threads...\n");
    }
    

    pthread_t tid[MAX_THREADS]; // Setting array of thread IDs

    for (int i = 1; i <= numProThd; i++)
    {
        (thread_data_array[i]).tid = i;       // Setting thread struct values
        (thread_data_array[i]).thdSleep = threadSleep;
        (thread_data_array[i]).message = messages;

        threadCount = pthread_create(&tid[i], NULL, producer, (void*) &thread_data_array[i]);    // Creating Producer threads and passing their structs

        if (threadCount)     // Error message if Producer threads are not created
        {
            printf("\nError: %d: Producer thread could not be created\n", threadCount);
            exit(1);
        }
    }

    for (int i = index; i <= (numConThd + numProThd); i++)
    {   
        (thread_data_array[i]).tid = i;       // Setting thread struct values
        (thread_data_array[i]).thdSleep = threadSleep;
        (thread_data_array[i]).message = messages;

        threadCount = pthread_create(&tid[numProThd + i], NULL, consumer, (void*) &thread_data_array[i]);    // Creating Consumer threads and passing their structs

        if (threadCount)     // Error message if Consumer threads are not created
        {
            printf("\nError: %d: Producer thread could not be created\n", threadCount);
            exit(1);
        }
    }

    sleep(mainSleep);   // Main function sleeps
    isFinished = true;  // Halt all threads

    for (int i = 0; i < (numProThd + numConThd); i++)     // Join all threads
    {
        pthread_join(tid[i], NULL);
    }

    endC= clock();  // Stop main timer
    calcTimer(&startC, &endC);  // Add main timer to timer total

    // Display statics
    printf("\nPRODUCER / CONSUMER SIMULATION COMPLETE\n");
    printf("=======================================\n");
    printf("Simulation Time: \t\t\t%d\n", (int) ((mainSleep) + (timer/CLOCKS_PER_SEC)));    // Can be cast as dubble to get a more accurate time
    printf("Maximum Thread Sleep Time: \t\t%d\n", threadSleep);
    printf("Number of Producer Threads: \t\t%d\n", numProThd);
    printf("Number of Consumer Threads: \t\t%d\n", numConThd);
    printf("Size of Buffer: %25d\n", BUFFER_SIZE);

    printf("\nTotal Number of Items Produced: \t%d\n", totalpros);
    for (int i = 1; i <= numProThd; i++)     // Retrive Producer's thread data
    {
        printf("  ");
        printf("Thread %d:", thread_data_array[i].tid);
        printf(" \t\t\t\t%d\n", thread_data_array[i].count);
    }

    printf("\nTotal Number of Items Consumed: \t%d\n", totalcons);
    for (int i = index; i <= (numConThd + numProThd); i++)     // Retrieve Consumer's thread data
    {
        printf("  ");
        printf("Thread %d:", thread_data_array[i].tid);
        printf(" \t\t\t\t%d\n", thread_data_array[i].count);
    }

    printf("\nNumber Of Items Remaining in Buffer: \t%d\n", items_in_buffer());
    printf("Number Of Times Buffer Was Full: \t%d\n", fullBuf);
    printf("Number Of Times Buffer Was Empty: \t%d\n", emptybuf);

    printf("\nMain function finished\n");

    return 0;
}


// Producer thread function
void *producer(void *threadArgs)
{   
    clock_t startC, endC; // Timer for Producer thread

    // Setting Producer thread struct data
    struct Thread_Data *my_data;
    my_data = (struct Thread_Data *) threadArgs; 
    int threadSleep = my_data-> thdSleep; 
    long int threadId = pthread_self();
    int proCounter = my_data->count;
    int exit = my_data->exitValue;
    char *message = my_data->message;

    while (!isFinished)    // Loop until main wakes up
    {
        startC = clock();   // Start timer for Producer thread

        sem_wait(&empty);  // Call to wait until semaphore is available
        pthread_mutex_lock(&mutex);     // Get mutex lock

        int number = (rand() % divisor);    // Produce an random item    

        if (!buffer_insert_item(number)) // Call to add item to buffer
        {
            printf("All buffers full.  Producer %ld waits.\n\n", threadId);
            pthread_mutex_unlock(&mutex);   // Release mutex
            sem_post(&full);     // Signal that buffer is full
        }
        else
        {
            proCounter++;
            my_data->count = proCounter;    // Add to individual thread's items produced

            if (strcmp(message, "yes")== 0)     // Check for request of snapshots
            {
                printf("Producer %ld writes %d\n", threadId, number);
                printBuff();
            }

            endC = clock();     // Stop Producer thread timer
            calcTimer(&startC, &endC);  // Call to calculate time

            sem_post(&full);     // Signal that buffer has an item
            pthread_mutex_unlock(&mutex);   // Release mutex
        }
        
        sleep(threadSleep);     // Thread sleeps
    }

    pthread_exit(&exit);    // Thread exits
}


// Consumer thread function
void *consumer(void *threadArgs)
{   
    clock_t startC, endC; // Timer for Consumer thread

    // Setting Consumer thread struct data
    struct Thread_Data *my_data;
    my_data = (struct Thread_Data *) threadArgs; 
    int threadSleep = my_data->thdSleep; 
    long int threadId = pthread_self();
    int conCounter = my_data->count;
    int exit = my_data->exitValue;
    char *message = my_data->message;
    int num = 0;    // Variable for item pulled from buffer

    while (!isFinished)     // Loop until main wakes up
    {
        startC = clock();   // Start timer for Consumer thread

        sem_wait(&full);    // Call to wait until semaphore is available 
        pthread_mutex_lock(&mutex); // Get mutex lock

        num = buffer[out];  // Get item from buffer

        if (!buffer_remove_item(num))   
        {
            printf("All buffers empty.  Consumer %ld waits.\n\n", threadId);
            sem_post(&empty); // Signal that buffer is empty
            pthread_mutex_unlock(&mutex);   // Release mutex
        }
        else
        {
            if (strcmp(message, "yes") == 0)     // Check for request of snapshots
            {
                if (isPrime(num))   // Call to check if item is prime
                {
                    printf("Consumer %ld reads %d   * * * PRIME * * *\n", threadId, num);
                }
                else
                {
                    printf("Consumer %ld reads %d\n", threadId, num);
                }

                conCounter++;
                my_data->count = conCounter;     // Add to individual thread's items produced  
                printBuff();
            }

            endC = clock(); // Stop timer for Consumer thread
            calcTimer(&startC, &endC);  // Call to calculate time

            sem_post(&empty); // Signal that buffer has had item removed
            pthread_mutex_unlock(&mutex);   // Release mutex
        }

        sleep(threadSleep);     // Thread sleeps
    }

    pthread_exit(&exit);    // Thread exits
}


// Function to check if number is a prime
bool isPrime(int num)
{
	bool prime = true;

	for (int i = 2; i <= num / 2; i++)
	{
		if (num % i == 0)
		{
			prime = false;
			break;
		}
	}

	return prime;
}


// Function to calculate running time
void calcTimer(long int *first, long int *last)
{
    long int time = (last - first);
    timer = timer + (int) time;
}
