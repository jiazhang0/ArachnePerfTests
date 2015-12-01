#include <stdio.h>
#include <vector>

#include "Arachne.h"
#include "Cycles.h"


using PerfUtils::Cycles;

void printEveryTwo(int start, int end) {
    std::vector<uint64_t> cycleCounts;
    cycleCounts.reserve(100000);
    for (int i = start; i < end; i+=2) {
       cycleCounts.push_back(Cycles::rdtsc());
       Arachne::yield();
    }
    if (start == 1) {
        uint64_t startTime = Cycles::toNanoseconds(cycleCounts[0]);
        for (int i = 0; i < cycleCounts.size(); i++) {
            printf("%lu\n", Cycles::toNanoseconds(cycleCounts[i]) - startTime);
        }
        fflush(stdout);
    }
}

int main(){
    // Initialize the library
    Arachne::threadInit();       

    // Add some work
    Arachne::createTask([](){ printEveryTwo(1,999); });
    Arachne::createTask([](){ printEveryTwo(2,1000); });
    
    // Must be the last call
    Arachne::mainThreadJoinPool();
}

