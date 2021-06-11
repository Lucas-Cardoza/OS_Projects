#********************************************************************
#
# Student: Lucas Cardoza
# Course: CSC 360 Operating Systems
# Programming Project #4: Banker
# Due Date: Thursday, October 22, 2020, 2:00 PM
# Instructor: Dr. Siming Liu
#
#********************************************************************


import sys  # To read command line arguments
import re   # To help map and covert strings to integers


#********************************************************************
#
# Print Matrix 
#
# This function prints the Matrix passed to it
#
# Return Value
# ------------
# void                         
#
# Value Parameters
# ----------------
# Key		string		The search key
# none	    head	    The header to columns in matrix
# none      matrix      The matrix passed to be printed
# none      w_file      The file the matrix will be printed too
#
# Local Variables
# ---------------
# i		int		Loop iteration variable
# j		int		Loop iteration variable
# x     str     String variable to hold value of matrix[i][j]
#
#********************************************************************
def printMatrix(head, matrix, w_file):
    w_file.write("{}\n" .format(head))
    for i in range(len(matrix)):
        w_file.write("{}: ".format(i))
        for j in range(len(matrix[i])):
            x = str(matrix[i][j])
            w_file.write("{} " .format(x))
        w_file.write("\n")


#********************************************************************
#
# Print Request Vector 
#
# This function prints the Request Vector passed to it
#
# Return Value
# ------------
# void                         
#
# Value Parameters
# ----------------
# Key		string		The search key
# none	    head	    The header to columns in vector
# none      vec         The request vector to be printed
# none      w_file      The file the vector will be printed too
#
# Local Variables
# ---------------
# i		int		Loop iteration variable
# x     str     String variable to hold value of vector[i]
#
#********************************************************************
def printReqVec(head, vec, w_file):
    w_file.write("\nThe Request Vector is...\n")
    w_file.write("{}\n" .format(head))
    w_file.write("{}: ".format(vec[0]))
    for i in range(len(vec) - 1):
        x = (vec[i + 1])
        w_file.write("{} " .format(x))
    w_file.write("\n")


#********************************************************************
#
# Print Available Vector 
#
# This function prints the Available Vector passed to it
#
# Return Value
# ------------
# void                         
#
# Value Parameters
# ----------------
# Key		string		The search key
# none	    head	    The header to columns in vector
# none      vec         The available vector to be printed 
# none      w_file      The file the vector will be printed too
#
# Local Variables
# ---------------
# i		int		Loop iteration variable
# x     str     String variable to hold value of vector[i]
#
#********************************************************************
def printAvailVec(head, vec, w_file):
    w_file.write("\nThe Available Vector is...\n")
    w_file.write("{}\n" .format(head))
    for i in range(len(vec)):
        x = (vec[i])
        w_file.write("{} " .format(x))
    w_file.write("\n")


#********************************************************************
#
# Get New Request 
#
# This function gets a new Request Vector from the Need Matrix
#
# Return Value
# ------------
# void                         
#
# Value Parameters
# ----------------
# Key		string		The search key
# none      index       The index to retrive the next Request Vector from
# none      reqVec      The Request Vector passed to be modified
# none      needM       The Need Matrix passed to get next Request Vector from
#
# Local Variables
# ---------------
# i		int		Loop iteration variable
#
#********************************************************************
def getNewRequest(index, reqVec, needM):
    for i in range(len(reqVec) - 1):
       reqVec[i + 1] = (needM[index][i])


def main(argv):

    allocatedMatrix = []            # Allocation Matrix
    maxMatrix = []                  # Max Matrix
    needMatrix = []                 # Need Matrix (Max Matrix - Allcoation Matrix)
    requestVec = [0, 0, 0, 0]       # Requested vector
    availableVec = [0, 0, 0, 0]     # Available vector
    currentAvailVec = [0, 0, 0, 0]  # Currently available vector (Available Vector - Requested Vector)
    resources, processes = 0, 0     # Proccesses and resources variables
    allocationStart = 3             # Allocation matrix starting point in read file
    header = "A B C D"              # Column header
    rJustHeader = header.rjust(10)  # Column header, right justified
    index = 0                       # Index for list elements 
    denyCount = 0                   # Denied request counter for while loop control
    safeState = False               # System state variable
    f_in = sys.argv[1]              # Command line argument variable
    
    with open(f_in, "r") as dataFile:               # Open file
        for position, line in enumerate(dataFile):  # Loop through the file and put data into correct matrixes and variables
            if position == 0:
                processes = int(line.strip())    # Assign value to processes variable
            if position == 2:
                resources = int(line.strip())    # Assign value to resources variable
            if position == allocationStart:
                for i in range(processes):
                    x = next(dataFile).strip()
                    temp = re.findall(r'\d+', x)
                    allocatedMatrix.append(list(map(int, temp)))    # Assign values to Allocated Matrix
                next(dataFile)
                for j in range(processes):
                    x = next(dataFile).strip()
                    temp = re.findall(r'\d+', x)
                    maxMatrix.append(list(map(int, temp)))          # Assign values to Max Matrix
                next(dataFile)
                x = next(dataFile).strip()
                temp = re.findall(r'\d+', x)
                availableVec = list(map(int, temp))                 # Assign values to Available Vector
                next(dataFile)
                x = next(dataFile).strip()
                temp = re.findall(r'\d+', x)
                requestVec = list(map(int, temp))                   # Assign values to Request vector

    index = requestVec[0]   # Setting index for list elements

    for i in range(len(allocatedMatrix)):   # Loop to create Need Matrix
        temp = []   # Temporary array to hold calculated need matrix elements
        for j in range(4):
            x = maxMatrix[i][j]         
            y = allocatedMatrix[i][j]
            temp.append(x - y)
            if j == 3:
                needMatrix.append(temp)     # Assign values to Need Matrix 
    
    w_file = open("Project4Results.txt", "w")   # Open file the results of simulation will be written to

    w_file.write("\nThere are {} processes in the system.\n" .format(processes))
    w_file.write("\nThere are {} resource types.\n" .format(resources))
    
    w_file.write("\nThe Allocation Matrix is...\n")
    printMatrix(rJustHeader, allocatedMatrix, w_file)
    w_file.write("\nThe Max Matrix is...\n")
    printMatrix(rJustHeader, maxMatrix, w_file)
    w_file.write("\nThe Need Matrix is...\n")
    printMatrix(rJustHeader, needMatrix, w_file)
    printAvailVec(header, availableVec, w_file)

    denyCount = processes   # Set denied request counter

    while processes > 0:  # Loop until there are no processes in allocation matrix
        for i in range(resources):
            if requestVec[i + 1] > needMatrix[index][i]:    # Compare request to needed (max - allocated) resources
                safeState = False   # Set system state
                break
            elif requestVec[i + 1] > availableVec[i]:       # Compare request to availabile resources
                safeState = False   # Set system state
                break  
            else:
                currentAvailVec[i] = (availableVec[i] - requestVec[i + 1]) # Calculate remaining resources
                safeState = True    # Set system state

        if safeState == True:
            w_file.write("\nTHE SYSTEM IS IN A SAFE STATE!\n")
            printReqVec(rJustHeader, requestVec, w_file)
            w_file.write("\nTHE REQUEST CAN BE GRANTED!\n")
            printAvailVec(header, currentAvailVec, w_file)

            for i in range(resources):  # Calculate amount of resources available after process is completed
                availableVec[i] = (allocatedMatrix[index][i] + availableVec[i]) 

            del allocatedMatrix[index]          # Remove process from Allocated Matrix
            del maxMatrix[index]                # Remove process from Max Matrix
            del needMatrix[index]               # Remove process from Need Matrix
            processes -= 1                      # Adjust number of processes

            w_file.write("\nThere are {} processes in the system.\n" .format(processes))
            printMatrix(rJustHeader, allocatedMatrix, w_file)
            w_file.write("\nThe Max Matrix is...\n")
            printMatrix(rJustHeader, maxMatrix, w_file)
            w_file.write("\nThe Need Matrix is...\n")
            printMatrix(rJustHeader, needMatrix, w_file)
            printAvailVec(header, availableVec, w_file)
            requestVec[0] = 0       # Set new index of Request Vector
            index = requestVec[0]   # Set new index of list elements
        else:
            w_file.write("\nTHE SYSTEM IS NOT IN A SAFE STATE!\n")
            printReqVec(rJustHeader, requestVec, w_file)
            printAvailVec(header, availableVec, w_file)
            w_file.write("\nTHE REQUEST IS DENIED!\n")

            if index == 0:
                denyCount -= 1          # Update denied requests counter

            if processes > 1:
                requestVec[0] += 1      # Set new index of Request Vector
                index = requestVec[0]   # Set new index of list elements

        if processes > 0 and denyCount > 0:
            getNewRequest(index, requestVec, needMatrix)    # Get next request from Need Matrix
        else:
            break # Exit loop if there still remains a request which cannot be granted after all others are completed

    w_file.write("\nEND\n")
    w_file.close()  # Close file the data has been written to
    return # End program


main(sys.argv[1:])



