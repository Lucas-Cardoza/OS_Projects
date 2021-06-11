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
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

// Global Constants
#define LINE_LEN 100
#define MAX_THREADS 512

// Function prototypes
void *compute_pi(void *);

// Global Variables
long sample_points;
long total_hits;
long total_misses;
long hits[MAX_THREADS];
long sample_points_per_thread;
int num_threads;

// Definitions of Structures
typedef struct 
{
  long sampleSize;
  int threadCount;
}testSample;

typedef struct 
{
  long sampleSize;
  int threadCount;
  double cpuTime;
}testResults;



int main()
{
  /* local variables */
  int ii;     // Counter for loop
  int retval = 0;   // Temporary return value
  pthread_t p_threads[MAX_THREADS];   // Create thread  
  pthread_attr_t attr;    // Thread attribute
  double computed_pi, cpuTimeUsed;    // Approx for pi and execution time variables
  clock_t start, end;   // Timer start and stop variables
  char line[LINE_LEN + 1];    // Array of data read form file provided
  char* token;    // String found in line read from file provided
  const char delimeter[2] = ",";    // The delimeter for reading data in file provided
  testSample test[LINE_LEN + 1];    // Instance of testSample structure
  testResults results[LINE_LEN + 1];    // Instance of testResults structure
  size_t count = 0;   // Counter for number of pairs of 'tokens' found in file provided

  pthread_attr_init(&attr);   // Thread attribute initiation
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);   // Setting contention scope attribute of the thread

  FILE *file = fopen("data22.txt", "r");    // Open file provided and read from file
  if (file == NULL)
  {
    perror("File failed to open");
    exit(1);
  }

  // Getting count of pairs of tokens in file
  while (fscanf(file, "%ld,%d", &test[count].sampleSize, &test[count].threadCount) == 2)
  {
    count++;
  }

  fclose(file);   // Close file

  for (size_t i = 0; i < count; i++)
  {
    sample_points = test[i].sampleSize;   // Setting sample points to number found in file
    printf("Sample size: %ld \n", sample_points);
    results[i].sampleSize = sample_points;    // Saving number of sample points to results 

    num_threads = test[i].threadCount;    // Setting number of threads to number found in file
    printf("Thread Count: %d \n", num_threads);
    results[i].threadCount = num_threads;   // Saving number of threads to results

    total_hits = 0;
    sample_points_per_thread = sample_points / num_threads;

    start = clock();    // Start timer
    for (ii = 0; ii < num_threads; ii++)
    {
      hits[ii] = ii;
      pthread_create(&p_threads[ii], &attr, compute_pi, (void *)&hits[ii]);
    }

    for (ii = 0; ii < num_threads; ii++)
    {
      pthread_join(p_threads[ii], NULL);
      total_hits += hits[ii];
    }

    // Compute approximation of PI
    computed_pi = 4.0 * (double)total_hits / ((double)(sample_points)); 
    end = clock();    // Stop timer

    // Compute time of execution
    cpuTimeUsed = ((double) (end - start)) / CLOCKS_PER_SEC;    
    results[i].cpuTime = cpuTimeUsed;   // Save execution time to results

    printf("Computed PI = %lf\n", computed_pi);
    printf("CPU time used was: %lf \n \n", cpuTimeUsed);
  }

  // Print all data in results structures to screen
  for (int i = 0; i < count; i++)
  {
    printf("%ld, %d, %lf \n", results[i].sampleSize, results[i].threadCount, results[i].cpuTime);
  }

  // Create a new file and write all data in results structures
  FILE *newFile = fopen("testResults.csv", "w");    
  if (newFile == NULL) 
  {
    return -1; 
  }

  fprintf(newFile, "Sample Size, Thread Count, CPU Time to Execute \n");

  for (int i = 0; i < count; i++) 
  { 
    fprintf(newFile, "%ld, %d, %lf \n", results[i].sampleSize, results[i].threadCount, results[i].cpuTime);  
  } 

  fclose(newFile);    // Close new file
  
  /* return to calling environment */
  return (retval);
}


//********************************************************************
//
// Compute PI Function
//
// This function performs the calculations to approximate PI with the
// given sample points passed to it by the calling function.  
//
// Return Value
// ------------
// void (none)                        
//
// Value Parameters
// ----------------
// None 
//
// Reference Parameters
// --------------------
// s		long int		size of the sample
//
// Local Variables
// ---------------
// seed int   
// ii		long		Loop Iteration Variable
// hit_pointer long   pointer to local_hits
// local_hits long    number of hits in PI approximation
// rand_no_x  double  random number for x value
// rand_no_y  double random number for y value
//*******************************************************************
void *compute_pi(void *s)
{
  int seed;
  long ii;
  long *hit_pointer;
  long local_hits;
  double rand_no_x;
  double rand_no_y;

  // Initialize local variables
  hit_pointer = (long *)s;
  seed = *hit_pointer;
  local_hits = 0;

  // Compute approximation of PI
  for (ii = 0; ii < sample_points_per_thread; ii++)
  {
    rand_no_x = (double)(rand_r(&seed)) / (double)RAND_MAX;
    rand_no_y = (double)(rand_r(&seed)) / (double)RAND_MAX;
    if (((rand_no_x - 0.5) * (rand_no_x - 0.5) +
         (rand_no_y - 0.5) * (rand_no_y - 0.5)) <= 0.25)
      local_hits++;
    seed *= ii;
  }

  *hit_pointer = local_hits;
  pthread_exit(0);
}