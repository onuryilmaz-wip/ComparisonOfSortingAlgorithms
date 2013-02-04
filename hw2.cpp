/*
 * Onur Yilmaz
 *
 * CENG315 - HW#2
 *
 */

// Libraries
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sys/time.h>
#include <math.h>


using namespace std;

// Vector for reading numbers
vector<long int> ReadNumbers;

// Vector for sorted numbers
vector<long int> SortedNumbers;

// Number of elements
long int numberOfElements;

// Function declarations
void CountingSort();
void QuickSort(int, int);
void MineWorker(int);
void ShellSort();
void RadixSort();
void TestSorted(long int, long int);

// Main function

int main() {

    // Open input
    ifstream myReadFile;
    myReadFile.open("hw2.inp");

    // Read sort type
    long int TypeOfSort;
    myReadFile >> TypeOfSort;

    // Read number of elements
    extern long int numberOfElements;
    myReadFile >> numberOfElements;

    // Read inputs
    long int temp;
    for (long int j = 0; j < numberOfElements; j++) {
        myReadFile >> temp;
        ReadNumbers.push_back(temp);
    }

    // Time related variables
    struct timeval tv_begin;
    struct timeval tv_end;

    // Although float give more information, int is used since
    // it is provided in homework text
    // float passed_miliseconds;
    int passed_miliseconds;

    // Start time
    gettimeofday(&tv_begin, NULL);

    if (TypeOfSort == 1) {
        //Count sort
        CountingSort();
    } else if (TypeOfSort == 2) {
        // Shell Sort
        SortedNumbers = ReadNumbers;
        ShellSort();
    } else if (TypeOfSort == 3) {
        // Quick sort
        SortedNumbers = ReadNumbers;
        QuickSort(0, numberOfElements);
    } else if (TypeOfSort == 4) {
        // Radix Sort
        SortedNumbers = ReadNumbers;
        RadixSort();
    } else;

    // Record ending time
    gettimeofday(&tv_end, NULL);
    passed_miliseconds = (tv_end.tv_sec - tv_begin.tv_sec)*1000.0 + (tv_end.tv_usec - tv_begin.tv_usec) / 1000.0;

    // Write time to output file
    ofstream outputFile("hw2.out", ios::app);
    outputFile << passed_miliseconds << endl;
    outputFile.close();

    // Call mine worker for minimum time calculation
    MineWorker(0);

    // Call mine worker for writing each step
    MineWorker(1);

    //TestSorted(TypeOfSort, numberOfElements);

    return 0;
}

// Test if sorting is correct

void TestSorted(long int TypeOfSort, long int numberOfElements) {
    cout << "Type: " << TypeOfSort << " Number: " << numberOfElements << endl;

    for (long int j = 0; j < numberOfElements; j++) {
        cout << SortedNumbers[j] << " ";
    }
    cout << endl;
}

// Mine worker function for solving the main problem

void MineWorker(int writeOut) {

    // Time spent
    int timeSpent = 0;

    // Open output file
    ofstream outputFile("hw2.out", ios::app);

    // Record the two quickest workers
    // They will be used over and over
    long int generalMin1 = SortedNumbers[0];
    long int generalMin2 = SortedNumbers[1];

    // Vector for upside and downside of mine
    vector<long int> Up;
    vector<long int> Down;

    // Vector iterators
    vector<long int>::iterator it;
    vector<long int>::iterator it2;

    // All miners are at downside
    Down = SortedNumbers;

    // If torch is at downside, true
    bool TorchDown = true;

    // While there is still mine worker at downside
    while (!Down.empty()) {

        /*
         * First case
         *
         * If torch is at the upside, send the quickest worker to down
         */
        if (!TorchDown) {

            // Locate the quickest at the upside
            vector<long int>::iterator minPosition;
            int toDown = Up[0];
            minPosition = Up.begin();
           
            // Write to ouputput file
            if (writeOut)
                outputFile << toDown << endl;

            // Update total time
            timeSpent = timeSpent + toDown;

            // Insert quickest worker to the start of downside 
            it = Down.begin();
            if (toDown>*Down.begin())
                it++;
            it = Down.insert(it, toDown);

            // Erase the worker from upside
            Up.erase(minPosition);

            // Torch is at the downside
            TorchDown = true;

        }            /*
             * Second case
             *
             * If both of the two quickest workers at downside, send both of them 
             * to upside
             *
             */
        else if (generalMin1 == Down[0] && generalMin2 == Down[1] && TorchDown) {

            // Insert them to upside
            it = Up.begin();
            it = Up.insert(it, Down[1]);
            it = Up.begin();
            it = Up.insert(it, Down[0]);

            // Write them to the output file
            if (writeOut)
                outputFile << Down[0] << " " << Down[1] << endl;

            // Find the maximum time spent
            int maxSent;
            if (Down[0] > Down[1])
                maxSent = Down[0];
            else maxSent = Down[1];
            // Update total spent time
            timeSpent = timeSpent + maxSent;

            // Erase from downside
            Down.erase(Down.begin(), Down.begin() + 2);

            // Torch is at upside
            TorchDown = false;
        }            /*
             * Third case
             *
             * Otherwise,
             *
             */
        else if (TorchDown) {

            // Send the slowest worker to up and write to output
            it = Up.end();
            int toUp1 = Down.back();

            if (writeOut)
                outputFile << toUp1 << " ";

            it = Up.insert(it, toUp1);

            // Erase from downside
            Down.pop_back();

            // Send the second slowest worker to up and write to output
            it = Up.end();
            int toUp2 = Down.back();

            if (writeOut)
                outputFile << toUp2 <<endl;

            it = Up.insert(it, toUp2);

            // Erase from downside
            Down.pop_back();

            // Find the maximum time spent
            int maxSent;
            if (toUp1 > toUp2)
                maxSent = toUp1;
            else maxSent = toUp2;

            // Update total time
            timeSpent = timeSpent + maxSent;

            // Torch is at the upside
            TorchDown = false;

        } else;

    }

    // If function is called for time calculation
    if (!writeOut)
        outputFile << timeSpent << endl;

    // Close output file
    outputFile.close();
}

/*
 * Counting sort
 */

void CountingSort() {

    // Vector for counts
    vector<long int> Counts;

    // Minimum and maximum for input
    // so that count vector will be limited
    long int max = ReadNumbers[0];
    long int min = ReadNumbers[0];

    // Find maximum
    for (long int i = 1; i < numberOfElements; i++) {
        if (ReadNumbers[i] > max)
            max = ReadNumbers[i];
    }
    max += 1; // For array operations

    // Find minimum
    for (long int i = 1; i < numberOfElements; i++) {
        if (ReadNumbers[i] < min)
            min = ReadNumbers[i];
    }

    // Drop elements
    Counts.assign(max - min, 0);

    // Counting
    for (long int i = 0; i < numberOfElements; i++) {
        Counts[ReadNumbers[i] - min] += 1;
    }

    // Locating
    for (long int i = 0; i < max - min; i++) {
        for (long int j = 0; j < Counts[i]; j++)
            SortedNumbers.push_back(i + min);
    }
    // Checking
    //for (long int i = 0; i < numberOfElements; i++)
    //  cout << SortedNumbers[i] << endl;
}

/*
 *  Quick Sort
 */

void QuickSort(int start, int end) {

    // Meeting start and end
    if (start == end) return;

    // If start passes end
    if (start >= end) return;

    // Pivot is assigned as first (From homework text)
    int pivotNumber;
    pivotNumber = SortedNumbers[start];

    // Find from start
    int iterator = start;

    // For temporary number
    int swapNumber;

    // From start to end
    for (int j = iterator + 1; j < end; j++) {
        // If number is larger than pivot
        if (SortedNumbers[j] < pivotNumber) {
            iterator++;
            // Swap
            swapNumber = SortedNumbers[iterator];
            SortedNumbers[iterator] = SortedNumbers[j];
            SortedNumbers[j] = swapNumber;
        }
    }
    // Save as temp
    swapNumber = SortedNumbers[iterator];
    SortedNumbers[iterator] = SortedNumbers[start];
    SortedNumbers[start] = swapNumber;

    // Quick sort the other two parts recursively
    QuickSort(start, iterator);
    QuickSort(iterator + 1, end);
}

/*
 * Shell Sort (Using gaps as 2^k - 1 )
 */

void ShellSort() {

    // For tracking gap increments 2^k -1
    int k;
    k = log(numberOfElements + 1) / log(2);

    // For swap and compare
    long int tempNumber = 0;

    // For each gap interval
    for (int gapStep = pow(2, k) - 1; gapStep != 0; k--, gapStep = pow(2, k) - 1) {

        // In each gap
        for (int i = 0; i < gapStep; i++) {

            // For all numbers
            for (int j = 0; j < numberOfElements; j = j + gapStep) {

                // Save into temp
                tempNumber = SortedNumbers[j];
                int t = j - gapStep;

                // Compare with temp and sort
                while (tempNumber < SortedNumbers[t]) {

                    if (t < 0) break;
                    SortedNumbers[t + gapStep] = SortedNumbers[t];
                    t = t - gapStep;
                }
                
                SortedNumbers[t + gapStep] = tempNumber;
            }
        }
    }
}

/*
 * Radix Sort (Using Counting Sort)
 */

void RadixSort() {

    // Vector for results
    vector<long int> ResultNumbers;
    ResultNumbers = SortedNumbers;

    // Since there are only 10 counts, array is used instead of vector
    int Counts[10];

    // Initialize array
    for (int i = 0; i < 10; i++)
        Counts[i] = 0;

    // Get the maximum
    int maxNumber = ReadNumbers[0];
    for (int i = 0; i < numberOfElements; i++) {
        if (ReadNumbers[i] > maxNumber)
            maxNumber = ReadNumbers[i];
    }

    // Exponent
    int exponent = 1;

    // For each digit 
    for (; maxNumber / exponent > 0; exponent = exponent * 10) {

        // Initialize counts array
        for (int i = 0; i < 10; i++)
            Counts[i] = 0;

        // Counting
        for (int i = 0; i < numberOfElements; i++) {
            int place = (ReadNumbers[i] / exponent) % 10;
            Counts[place]++;
        }

        // Summing counts for locations
        for (int i = 1; i < 10; i++) {
            Counts[i] = Counts[i] + Counts[i - 1];
        }

        // Locating
        for (int i = numberOfElements - 1; i >= 0; i--) {
            int place2 = (ReadNumbers[i] / exponent) % 10;
            int place3 = Counts[place2];
            Counts[place2]--;
            place3--;
            SortedNumbers[place3] = ReadNumbers[i];

        }
        // Saving to their original locations
        for (int i = 0; i < numberOfElements; i++)
            ReadNumbers[i] = SortedNumbers[i];

    }
    // Result
    SortedNumbers = ReadNumbers;
}

// End of code