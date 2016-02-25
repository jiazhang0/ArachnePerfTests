#include <stdio.h>
#include <vector>

#include "Arachne.h"
#include "Cycles.h"
#include "TimeTrace.h"


using PerfUtils::Cycles;
using PerfUtils::TimeTrace;

void printEveryTwo(int start, int end) {
}

int main(){
    // Initialize the library
    Arachne::threadInit();
    uint64_t startTime = Cycles::rdtsc();

    // Measure the thread creation overhead in the creating thread.
    for (int i = 0; i < 1000; i++)
        Arachne::createThread(-1, printEveryTwo,1,i);
    
    uint64_t timePerYield = (Cycles::rdtsc() - startTime) / 1000;
    printf("Thread creation average time %lu\n", Cycles::toNanoseconds(timePerYield));

    fflush(stdout);
    // Must be the last call
    Arachne::mainThreadJoinPool();
}
