//********************************************************************
//
// Student: Lucas Cardoza
// Course: CSC 360 Operating Systems
// Programming Project #3: Producer/Consumer
// Due Date: Friday, September 25, 2020, 11:59 PM
// Instructor: Dr. Siming Liu
//
//********************************************************************


#ifndef _BUFFER_H_DEFINED_
#define _BUFFER_H_DEFINED_

#include <stdbool.h>
#include <stdio.h>


// Globals
typedef int buffer_item;
#define BUFFER_SIZE 5
int buffer[BUFFER_SIZE] = {-1, -1, -1, -1, -1}; // Setting buffer size and initializing elements to -1
int in = 0, out = 0;    // Input and output positions in buffer
int totalpros = 0, totalcons = 0, totalItems = 0;   // Totals for all production, consumption, and items currently in buffer
int fullBuf = 0, emptybuf = 1; // Number of times buffer is either full or empty. Empty is starting with 1 because it is empty from the start
sem_t empty;  // Semaphore for buffer empty signals
sem_t full;  // Semaphore for buffer full signals
pthread_mutex_t mutex;  // Setting mutex for buffer


// Function prototypes
bool buffer_insert_item(buffer_item item);
bool buffer_remove_item(buffer_item item);
void printBuff();
int items_in_buffer();


// Function to add item into buffer
bool buffer_insert_item(buffer_item item)
{
    if (items_in_buffer() == BUFFER_SIZE)   // Check to see if buffer is full
    {
        return false;   // Buffer is full. Caller should wait until an item is "removed" from buffer
    }
    else
    {
        buffer[in] = item;      // Place item in buffer
        in = (in + 1) % BUFFER_SIZE;    // Find next point of 'in'
        totalItems++;
        totalpros++;    // Add to total items produced variable

        if (totalItems == BUFFER_SIZE)  // Check to see if buffer is now full
        {
            fullBuf++;  // Add to full buffer count
        }

        return true;    // Item was successful placed in buffer
    }
}


// Function to remove item from buffer
bool buffer_remove_item(buffer_item item)
{
    if (items_in_buffer() == 0) // Check to see if buffer is empty
    {
        return false;   // Buffer is empty. Caller should wait until an item is placed in buffer
    }
    else
    {
        out = (out + 1) % BUFFER_SIZE;  // Find next point for 'end'
        totalItems--;   // Decrements total items in buffer, but not actually remove item.
        totalcons++;    // Add to total items consumed variable

        if (totalItems == 0)    // Check to see if buffer is now empty
        {
            emptybuf++; // Add to empty buffer count
        }

        return true;    // Call to "remove" item was successful
    }
}


// Function to printout buffer
void printBuff()
{
    char r = 'R';
    char w = 'W';
    int spaceW = 5, spaceR = 5;

    // Output for number of items in buffer
    printf("(buffers occupied: %d)\n", items_in_buffer());

    // Formatting for items in buffer
    printf("buffers:");
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf("%5d", buffer[i]);
    }

    printf("\n\t ");

    // Formatting for the '----'
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf("%3s", " ----");
    }

    // Formating for the 'W' and 'R' 
    if (in == out)
    {
                spaceR = (spaceR * out) - (spaceW * in);
        spaceW = (spaceW * in) + 12;
        printf("\n%*c", spaceW, w);
        printf("%*c",spaceR, r);
    }
    
    if (in > out)
    {
        spaceW = (spaceW * in) - (spaceR * out);
        spaceR = (spaceR * out) + 12;
        printf("\n%*c",spaceR, r);
        printf("%*c", spaceW, w);
    }
    else if ( in < out)
    {
        spaceR = (spaceR * out) - (spaceW * in);
        spaceW = (spaceW * in) + 12;
        printf("\n%*c", spaceW, w);
        printf("%*c",spaceR, r);
    }
    
    printf("\n \n");
}


// Function to calculate number of valid buffer items
int items_in_buffer()
{
    return totalItems;
}

#endif  //_BUFFER_H_DEFINED_