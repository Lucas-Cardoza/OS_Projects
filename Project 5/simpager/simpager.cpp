//********************************************************************
//
// Student: Lucas Cardoza
// Course: CSC 360 Operating Systems
// Programming Project #5: Simulation of Page Replacement Strategies
// Due Date: Thursday, November 12, 2020, 2:00 PM
// Instructor: Dr. Siming Liu
//
//********************************************************************


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_set>    // While doing research for this project, I learned about unordered sets and maps, so I wanted to use them. 
#include <unordered_map>


// Function Prototypes
int fifoRutine(std::vector<int> &pageVec, int numOfFrames, int numOfPages);
int lruRoutine(std::vector<int> &pageVec, int numOfFrames, int numOfPages);
int optRoutine(std::vector<int>& pageVec, int numOfFrames, int numOfPages);
int randRoutine(std::vector<int>& pageVec, int numOfFrames, int numOfPages);


// Globals
const int MAX_LRU_NUMBER = 1000; // The max times a page will be used. I just picked a random number. 


int main()
{
    // Local Variables
    std::vector<int> pageVec;   // Vector to hold pages
    int numOfFrames = 0;        // Variable to hold total number of frames available 
    int numberOfPages;          // Variable to hold total number of pages
    int totalFaults = 0;        // Variable to hold total page faults
    std::string pageString;     // String variable to temporally hold pages
    std::string sim, frames;    // Variable to hold simulation types
    std::ifstream pageInput("testcase1.txt");       // Input file holding simulation data
    std::ofstream simOutputFile("simResults.txt");  // Output file for simulation results.

    if (!pageInput)             // Check for file open error
    {
        std::cout << "Error opening input file" << std::endl;
        system("pause");
        return -1;
    }

    while (!pageInput.eof())            // Read until EOF
    {
        getline(pageInput, pageString); // Get pages
        getline(pageInput, frames);     // Get number of frames available

        simOutputFile << "Page Reference String: \n" << pageString << std::endl;
        simOutputFile << "Number of Frames: " << frames << std::endl;

        for (int i = 0; i < pageString.length(); i += 2)    
        {
            pageVec.push_back(pageString[i] - 48);  // Convert page string variable into int vector
        }

        numOfFrames = (frames[0] - 48);     // Convert number of string frames into int
        numberOfPages = pageVec.size();     // Get total number of pages

        while (!pageInput.eof())
        {
            getline(pageInput, sim);        // Get simulation to be run
            if (sim[0] == 'F')      // Check to see if FIFO simulation will be run
            {
                totalFaults = fifoRutine(pageVec, numOfFrames, numberOfPages);  
                simOutputFile << sim << ": " << totalFaults << std::endl;
            }
            if (sim[0] == 'L')      // Check to see if LRU simulation will be run
            {
                totalFaults = lruRoutine(pageVec, numOfFrames, numberOfPages);
                simOutputFile << sim << ": " << totalFaults << std::endl;
            }
            if (sim[0] == 'O')      // Check to see if OPT simulation will be run
            {
                totalFaults = optRoutine(pageVec, numOfFrames, numberOfPages);
                simOutputFile << sim << ": " << totalFaults << std::endl;
            }
            if (sim[0] == 'R')     // Check to see if RAND simulation will be run
            {
                totalFaults = randRoutine(pageVec, numOfFrames, numberOfPages);
                simOutputFile << sim << ": " << totalFaults << std::endl;
            }
        }
    }

    pageInput.close();      // CLose simulation info file
    simOutputFile.close();  // Close output file

    return 0;
}


//******************************************************************** 
// 
// FIFO Simulation Function
// 
// This function performs the simulation of a first in first out (FIFO) 
// page replacement simulation of an operating system.  
// 
// Return Value 
// ------------ 
// int                      The value of the total amount of page faults
// 
// Value Parameters 
// ---------------- 
// numOfFrames      int     The number of frames available to the simulation
// numOfPages       int     The total number of pages the simulation will run
// 
// Reference Parameters 
// -------------------- 
// pageVec          int     A vector holding the list of pages to be run
// 
//*******************************************************************
int fifoRutine(std::vector<int> &pageVec, int numOfFrames, int numOfPages)
{
    // Local Variables
    int pageFaults = 0, value = 0;              // Counter for number of page faults. Value of front page in queue
    std::queue<int> fifoQ;                      // Queue to hold pages in a FIFO order
    std::unordered_set<int> pagesInFrame;       // Set to hold pages currently in frames

    for (int i = 0; i < numOfPages; i++)        // Loop through all pages
    {
        if (pagesInFrame.size() < numOfFrames)  // Check to see if there is an empty frame
        {
            if (pagesInFrame.find(pageVec[i]) == pagesInFrame.end())  // Check to see if page currently being requested is in a frame
            {
                pagesInFrame.insert(pageVec[i]);  // If page was not in a frame, add it to the frame set
                pageFaults++;                     // Because page was not in a frame, it is a page fault
                fifoQ.push(pageVec[i]);           // Push page into FIFO queue
            }
        }
        else    // There are no empty frames. Must now remove the first page in queue
        {
            if (pagesInFrame.find(pageVec[i]) == pagesInFrame.end()) // Check to see if currently requested page is already in a frame
            {
                value = fifoQ.front();           // Store value of first page in FIFO queue
                fifoQ.pop();                     // Pop page out of queue
                pagesInFrame.erase(value);       // Remove page from frame set
                pagesInFrame.insert(pageVec[i]); // Insert new page into frame set
                fifoQ.push(pageVec[i]);          // Push new page into FIFO queue
                pageFaults++;                    // Because the new page had to be added to frame set, it is a page fault
            }
        }
    }

    return pageFaults;
}


//******************************************************************** 
// 
// LRU Simulation Function
// 
// This function performs the simulation of a least recently used (LRU)
// page replacement simulation of an operating system.  
// 
// Return Value 
// ------------ 
// int                      The value of the total amount of page faults
// 
// Value Parameters 
// ---------------- 
// numOfFrames      int     The number of frames available to the simulation
// numOfPages       int     The total number of pages the simulation will run
// 
// Reference Parameters 
// -------------------- 
// pageVec          int     A vector holding the list of pages to be run
// 
//*******************************************************************
int lruRoutine(std::vector<int>& pageVec, int numOfFrames, int numOfPages)
{
    // Local Variables
    int pageFaults = 0;                             // Counter for number of page faults
    std::unordered_set<int> pagesInFrame;           // Set to hold pages currently in frames
    std::unordered_map<int, int> recenlyUsedPages;  // Mapped set of recently used pages

    for (int i = 0; i < numOfPages; i++)            // Loop through all pages
    { 
        if (pagesInFrame.size() < numOfFrames)      // Check to see if there is an empty frame
        {
            if (pagesInFrame.find(pageVec[i]) == pagesInFrame.end()) // Check to see if page currently being requested is in a frame
            {
                pagesInFrame.insert(pageVec[i]);    // If page was not in a frame, add it to the frame set
                pageFaults ++;                      // Because page was not in a frame, it is a page fault
            }

            recenlyUsedPages[pageVec[i]] = i;       // Add page to recently used mapped set
        }
        else    // There are no empty frames. Must now remove least most recently used page
        {
            if (pagesInFrame.find(pageVec[i]) == pagesInFrame.end()) // Check to see if currently requested page is already in a frame
            {
                int lru = MAX_LRU_NUMBER;   // Variable to hold LRU page
                int index = 0;              // Variable to temperately hold page's index in recentlyUsedPages

                for (auto j = pagesInFrame.begin(); j != pagesInFrame.end(); j++) // Range loop to look through frame set for LRU page
                {
                    if (recenlyUsedPages[*j] < lru) // Check to see if value mapped to bucket j is < LRU 
                    {
                        lru = recenlyUsedPages[*j]; // Page at bucket j, is the LRU page
                        index = *j;                   // The index of the page to be removed from LRU set            
                    }
                }

                pagesInFrame.erase(index);            // Remove LRU page from frame set (using the page's index)
                pagesInFrame.insert(pageVec[i]);    // Insert current page into frame set
                pageFaults ++;                      // Increase page fault because current page was added to frame set 
            }

            recenlyUsedPages[pageVec[i]] = i;       // Increment to next page in LRU set
        }
    }

    return pageFaults;
}


//******************************************************************** 
// 
// OPT Simulation Function
// 
// This function performs the simulation of an optimal (OPT) page 
// replacement simulation of an operating system.  
// 
// Return Value 
// ------------ 
// int                      The value of the total amount of page faults
// 
// Value Parameters 
// ---------------- 
// numOfFrames      int     The number of frames available to the simulation
// numOfPages       int     The total number of pages the simulation will run
// 
// Reference Parameters 
// -------------------- 
// pageVec          int     A vector holding the list of pages to be run
// 
//*******************************************************************
int optRoutine(std::vector<int>& pageVec, int numOfFrames, int numOfPages)
{
    // Local Variables
    int pageFaults = 0;         // Page faults counter
    int j = 0, l = 0, index = 0, farthest = 0, remove = 0;  // Index variables
    std::vector<int> pagesInFrame;    // Vector to hold pages in frames

    for (int i = 0; i < numOfPages; i++)    // Loop through all pages
    {
        for (j = 0; j < pagesInFrame.size(); j++) // Loop to see if current page is in a frame
            if (pagesInFrame[j] == pageVec[i])    // Check to see if current page is in frame
            {
                break;  // If so, break
            }

        if (j == pagesInFrame.size())     // If j == frame.size(), then the current page is not in the frame
        {
            if (pagesInFrame.size() < numOfFrames)    // See if there is room in frames to add current page
            {
                pagesInFrame.push_back(pageVec[i]);   // If so, add current page to frames
            }
            else  // There was no empty frames. Must find page to remove, so current page can be added to frames
            {
                index = i + 1;      // Look at next page to see what is coming up next
                farthest = index;   // Farthest index the page will be found
                remove = -1;       // Recent is the most recent page index.  

                for (int k = 0; k < pagesInFrame.size(); k++)     // Loop through frames to find the farthest page request
                {
                    for (l = index; l < numOfPages; l++)    // Loop through pages to find where currently looked at page will be requested next
                    {
                        if (pagesInFrame[k] == pageVec[l])        // If page in frames == page in page string, that is then next request for that page
                        {
                            if (l > farthest)   // Check to see if l is the farthest page in frames to be looked at or not
                            {
                                farthest = l;   // If not, set currently looked at page in frame to the farthest
                                remove = k;    // Remove is the index of the page that needs to be removed
                            }

                            break;  
                        }
                    }

                    if (l == numOfPages)    // Page will not be requested again (reached the end of page string)
                    {
                        remove = k;         // Because this page will not be requested again, it should be removed from the frames
                        break;
                    }
                }

                pagesInFrame[remove] = pageVec[i];   // Replace the farthest page with current page
            }

            pageFaults++;   // Add to page faults
        }
    }

    return pageFaults;
}


//******************************************************************** 
// 
// RAND Simulation Function
// 
// This function performs the simulation of an random (RAND) page 
// replacement simulation of an operating system.  
// 
// Return Value 
// ------------ 
// int                      The value of the total amount of page faults
// 
// Value Parameters 
// ---------------- 
// numOfFrames      int     The number of frames available to the simulation
// numOfPages       int     The total number of pages the simulation will run
// 
// Reference Parameters 
// -------------------- 
// pageVec          int     A vector holding the list of pages to be run
// 
//*******************************************************************
int randRoutine(std::vector<int>& pageVec, int numOfFrames, int numOfPages)
{
    // Local Variables
    int pageFaults = 0, index = 0, j = 0; // Counter for number of page faults; Random index value; Index variable;      
    std::vector<int> pagesInFrame;        // Set to hold pages currently in frames

    for (int i = 0; i < numOfPages; i++)        // Loop through all pages
    {
        for (j = 0; j < pagesInFrame.size(); j++)     // Loop to see if current page is in a frame
        {
            if (pagesInFrame[j] == pageVec[i])    // Check to see if current page is in frame
            {
                break;  // If page is in a frame, break
            }
        }

        if (j == pagesInFrame.size())     // If j == frame.size(), then the current page is not in the frame
        {
            if (pagesInFrame.size() < numOfFrames)    // See if there is room in frames to add current page
            {
                pagesInFrame.push_back(pageVec[i]);   // If so, add current page to frames
                pageFaults++;       // Because the new page had to be added to frame set, it is a page fault
            }
            else  // There was no empty frames. Must find page to remove, so current page can be added to frames
            {
                index = rand() % 3; // Generate random number for index of page to be removed
                pagesInFrame.erase(pagesInFrame.begin() + index);              // Remove page at index from frame set
                pagesInFrame.insert(pagesInFrame.begin() + index, pageVec[i]); // Insert new page into frame set at index
                pageFaults++;       // Because the new page had to be added to frame set, it is a page fault
            }
        }
    }

    return pageFaults;
}