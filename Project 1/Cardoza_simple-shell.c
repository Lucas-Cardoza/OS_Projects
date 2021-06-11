//********************************************************************
//
// Student: Lucas Cardoza
// Course: CSC 360 Operating Systems
// Programming Project #1: Simple WSL Shell
// Due Date: September 5, 2020, 11:59pm
// Instructor: Dr. Siming Liu
//
//********************************************************************

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

// Global Constants
#define PRMTSIZ 255
#define MAXARGS 63
#define INDEXSIZE 5

// Function prototypes
void print_arg(char *arg);
void printPids(int *pastPids); 
void childProcess(char **args);


int main(void)
{
    // Local variables
    int argc = 0;      // Argument counter
    int index = 0;   // Index counter for pastPids array
    int pastPids[INDEXSIZE] = {0};  // Array (all elements intialized to 0) to hold the last five pids of child processes.
    
    while (1)
    {
        char input[PRMTSIZ + 1] = {0x0}; // Setting the out of bounds variable for PRMTSIZ
        char *ptr = input;  // Pointer to input
        char *args[MAXARGS + 1] = {NULL};   // Setting the out of bounds variable for MAXARGS

        // Prompt
        printf("My Shell$ ");
        fgets(input, PRMTSIZ, stdin);

        // Ignore empty input from users
        if (*ptr == '\n')
            continue;

        // Convert input line to list of arguments
        for (int i = 0; i < sizeof(args) && *ptr; ptr++)
        {
            if (*ptr == ' ')
                continue;
            if (*ptr == '\n')
                break;
            for (args[i++] = ptr; *ptr && *ptr != ' ' && *ptr != '\n'; ptr++);
                *ptr = '\0';

            argc = i;
        }

        // args array is the user input string array
        printf("Received user commands: ");
        for (int i = 0; i < argc; i++)
        {
            print_arg(args[i]);
        }
        printf("\n");

        // Built-in commands
        if (strcmp("help", args[0]) == 0)   // Command to display help info
        {
            printf("Type 'menu' for command options. \n");
        }

        if (strcmp("exit", args[0]) == 0)   // Command to exit the shell
        {
            printf("Exiting shell. \n");
            exit(0);
        }

        if (strcmp("menu", args[0]) == 0)   // Command to display the menu options
        {
            printf("Here are the menu options: \n");
            printf("help \n"); 
            printf("exit \n");
            printf("fork \n");  
            printf("showpid \n");
        }

        if (strcmp("fork", args[0]) == 0)   // Command to execute child process (fork) and execute program
        {
            // Local variables
            pid_t pid;
            int status = 0;
            pid = fork();

            if (pid < 0)    // If the pid is < 0, this means that the creation of a child process was unsuccessful
            {
                perror("Fork failed. Sorry. \n");
                exit(-1);
            }
            else if (pid == 0)  // A pid of 0 means that child process was created
            { 
                childProcess(args);
            }
            else    // A positive number for pid means this is the parent process
            {
                wait(&status);  // Wait for child program to finish and print exit status
                printf("Parent PID is: %d \n", getpid());
                printf("Child process is finished with status %d \n", status);
            }

            pastPids[index] = pid;
            index++;
            if (index == INDEXSIZE) // Creating a circular array for the last five child pids
            {
                index = 0;
            }
        } 

        if(strcmp("showpid", args[0]) == 0) // Command to display array of pids for child processes
        {
            printPids(pastPids);           
        }       
    }

    return 0;
}


//********************************************************************
//
// childProcess Function
//
// Function will execute the execvp() system call along with print the 
// child pid.
//
// Return Value
// ------------
// void                        No return value
//
// Value Parameters
// ----------------
// None
//
// Reference Parameters
// --------------------
// args     char array             The array of chars the user input
//
// Local Variables
// ---------------
// None
//
//*******************************************************************
void childProcess(char **args)
{
    printf("This is the child process, and the parent process will wait for me to complete \n");
    printf("Child PID is : %d \n", getpid());
    execvp(args[0], args);  // Execute fork system call 
}


//********************************************************************
//
// print_arg Function
//
// This function will traverse the args array and print the user's
// input.
//
// Return Value
// ------------
// void                         No return value
//
// Value Parameters
// ----------------
// None
//
// Reference Parameters
// --------------------
// arg      char array      The array of chars the user input
//
// Local Variables
// ---------------
// i		int		Loop Iteration Variable
//
//*******************************************************************
void print_arg(char *arg)
{
    int i = 0;  // Local variable

    while (arg[i] && arg[i] != '\0' && arg[i] != ' ' && arg[i] != '\n')
    {
        printf("%c", arg[i++]);
    }
    printf(" ");
}


//********************************************************************
//
// printPids Fuction
//
// This function will print the array of the past five child pids if 
// there are any. If none have been created, it will declare so.
//
// Return Value
// ------------
// void                         No return value
//
// Value Parameters
// ----------------
// None
//
// Reference Parameters
// --------------------
// pastPids     int array       The array of child pids
//
// Local Variables
// ---------------
// i		int		Loop Iteration Variable
//
// Global Constants
// ----------------
// INDEXSIZE     int             Global constant
//
//*******************************************************************
void printPids(int *pastPids)
{
    int i = 0;  // Local variable

    if (pastPids[i] != 0)
    {
        printf("The past child pids: \n");
        for (i; i < INDEXSIZE; i++) // Loop through the array and display the past child pids
        {
            if (pastPids[i] == 0)   // If the element is 0, this means there are no more pids to print
            {
                i = INDEXSIZE;
            }
            else
            {
                printf("%d \n", pastPids[i]);   // Print the next element in the array  
            }
        }
    }        
    else
    {
        printf("No child processes have been created yet \n");  // There are no elements recorded in the array
    }
}