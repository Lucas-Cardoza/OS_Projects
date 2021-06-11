
#include <stdio.h>

#include <sys/time.h>

void sum_task(); //sum 1 to 1 billion

int main()
{
  //timer
  struct timeval start, end;
  long mtime, seconds, useconds;
  gettimeofday(&start, NULL); //timer

  sum_task();

  gettimeofday(&end, NULL); //timer

  //timer
  seconds = end.tv_sec - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  mtime = ((seconds)*1000 + useconds / 1000.0) + 0.5;
  printf("Elapsed time: %ld milliseconds\n", mtime);
}

void sum_task()
{
  long sum = 0;
  for (int i = 1; i < 1000000000; i++)
  {
    sum += i;
  }
  printf("The sum of 1 to 1b is: %ld\n", sum);
}