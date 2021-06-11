//********************************************************************
//
// Student: Lucas Cardoza
// Course: CSC 360 Operating Systems
// Programming Project #2: Programming with pThread
// Due Date: September 12, 2020, 11:59pm
// Instructor: Dr. Siming Liu
//
//********************************************************************

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// Global Constants
#define ARRAYSIZE 48    // Max Fibonacci numbers before I started getting negative numbers.


// Global Variables
int input = 0;  // User's input value
int sharedData[ARRAYSIZE];  // Shared array to hold Fibonacci numbers

// Function prototypes
void *FibGenerator(void *param);    // Routine the thread will run


int main()
{
    // Local variables
    pthread_t thread;     // Thread
    pthread_attr_t attr;     // Thread attribute

    // Prompt
    printf("Enter a number (less than %d) of Fibonacci numbers that the program will generate: ", ARRAYSIZE);
    scanf("%d", &input);    // Getting user's input value

    sharedData[0] = 0;  // Setting the first Fibonacci sequence numbers
    sharedData[1] = 1;

    pthread_attr_init(&attr);   // Setting thread's attribute
    pthread_create(&thread, &attr, FibGenerator, NULL);    // Create new thread
    pthread_join(thread, NULL);        // Waiting for thread to finish its process

    // Output Fibonacci sequence 
    for (int i = 0; i < input; i++)
    {
        printf("%d \n", sharedData[i]);
    }

    return 0;
}


//********************************************************************
//
// Fibonacci Sequence Geenerator Function
//
// This function will take user input, and based on that input gererate
// that number of Fibonacci sequence numbers.
//
// Return Value
// ------------
// void                     
//
// Value Parameters
// ----------------
// None
//
// Reference Parameters
// --------------------
// Also none. The pthread function that calls this function is passing
// NULL, and so no reference parameters are being passed to this function
//
// Local Variables
// ---------------
// i		int		Loop Iteration Variable
//
//*******************************************************************
void *FibGenerator(void *arg) 
{
    printf("Child thread is running now");
	
    if (input > 1)
    {
        sharedData[1] = 1;
	    for (int i = 2; i < input; i++) 
        {
            sharedData[i] = sharedData[i -1] + sharedData[i -2];
        }
    }

    pthread_exit(0);    // Exit the the thread process
}

