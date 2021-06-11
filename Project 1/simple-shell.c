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

#define PRMTSIZ 255
#define MAXARGS 63

void print_arg(char *arg);

int main(void)
{
    int argc = 0;
    while (1)
    {
        char input[PRMTSIZ + 1] = {0x0};
        char *ptr = input;
        char *args[MAXARGS + 1] = {NULL};

        // prompt
        printf("My Shell$ ");
        fgets(input, PRMTSIZ, stdin);

        // ignore empty input
        if (*ptr == '\n')
            continue;

        // convert input line to list of arguments
        for (int i = 0; i < sizeof(args) && *ptr; ptr++)
        {
            if (*ptr == ' ')
                continue;
            if (*ptr == '\n')
                break;
            for (args[i++] = ptr; *ptr && *ptr != ' ' && *ptr != '\n'; ptr++)
                ;
            *ptr = '\0';

            argc = i;
        }

        //args array is the user input string array
        printf("Received user commands: ");
        for (int i = 0; i < argc; i++)
        {
            print_arg(args[i]);
        }
        printf("\n");

        // built-in commands: help is listed as an example
        if (strcmp("help", args[0]) == 0)
        {
            printf("This is a simple shell.\n");
        }

        // fork child and execute program

        // wait for program to finish and print exit status
    }
}

void print_arg(char *arg)
{
    int i = 0;
    while (arg[i] && arg[i] != '\0' && arg[i] != ' ' && arg[i] != '\n')
    {
        printf("%c", arg[i++]);
    }
    printf(" ");
}