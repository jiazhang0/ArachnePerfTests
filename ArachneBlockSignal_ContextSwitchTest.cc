#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <atomic>

#include "Arachne.h"
#include "Cycles.h"
#include "TimeTrace.h"
#include "Util.h"


using PerfUtils::Cycles;
using PerfUtils::TimeTrace;


#define NUM_ITERATIONS 10000
#define NUM_THREADS_IN_CYCLE 2


Arachne::ThreadId tids[NUM_THREADS_IN_CYCLE];

volatile int consumerIsReady = 0;

using PerfUtils::Util::pinAvailableCore;

void producer() {
    pinAvailableCore();
	for (int i = 0; i < NUM_ITERATIONS*NUM_THREADS_IN_CYCLE; i++) {
        int index = i % NUM_THREADS_IN_CYCLE;
		while (!consumerIsReady);
		consumerIsReady = 0;
        TimeTrace::record("Producer about to signal %x", index);
        Arachne::signal(tids[index]);
        TimeTrace::record("Producer finished signaling %x", index);
	}
    printf("Producer finished\n");
    fflush(stdout);
    TimeTrace::setOutputFileName("BlockSignal_ContextSwitch.log");
    TimeTrace::print();
}

void consumer(int cid) {
    if (cid == 0) {
        pinAvailableCore();
        consumerIsReady = 1;
    }
	for (int i = 0; i < NUM_ITERATIONS; i++) {
        Arachne::block();
        TimeTrace::record("Consumer just woke up %x", cid);
		consumerIsReady = 1;
	}
    printf("Consumer %d finished\n", cid);
    fflush(stdout);
}

int main(int argc, const char** argv){
    // Initialize the library
    Arachne::init(&argc, argv);

    for (int i = 0; i < NUM_THREADS_IN_CYCLE; i++) {
        tids[i] = Arachne::createThread(1, consumer,  i);
    }
	Arachne::createThread(0, producer);

    printf("Created Producer and consumer threads\n");
    fflush(stdout);
    // Must be the last call
    Arachne::waitForTermination();
}
