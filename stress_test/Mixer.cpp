#include "Mixer.h"

#include <jemalloc/jemalloc.h>

Mixer::Mixer(vector<shared_ptr<Producer>> producers, int numProducers, int me,
             vector<shared_ptr<ToFreeQueue>> toFreeQueues)
    : producers_(producers), producersRemaining_(numProducers),
			toFreeQueues_(toFreeQueues), me_(me),
      producerIdPicker_(0, producers.size() - 1),
			consumerIdPicker_(0, toFreeQueues.size() - 1)  {}

// Picks next producer for the mixer to run. Currently uniform random choice
const Producer &Mixer::pickProducer() {
  int producerIndex = this->producerIdPicker_(this->generator_);
  return *(this->producers_[producerIndex]);
}

// Picks next producer for the mixer to run. Currently uniform random choice
ToFreeQueue& Mixer::pickConsumer() {
  int consumerIndex = this->consumerIdPicker_(this->generator_);
  return *(this->toFreeQueues_[consumerIndex]);
}

void Mixer::run() {
  while (this->producersRemaining_ > 0) {
    this->toFreeQueues_[this->me_]->free();
		// otherwise run a random producer
		Allocation a = this->pickProducer().run();
		if (!a.isEmpty()) {
			this->pickConsumer().addToFree(std::move(a));
		}
		producersRemaining_--;
  }
	// Flush thread cache to get more accurate stats
	mallctl("thread.tcache.flush", NULL, NULL, NULL, 0);
	// TODO : need to cleanup remaining memory, but it's possible it will be
	// added to our free queue at a later time
}
