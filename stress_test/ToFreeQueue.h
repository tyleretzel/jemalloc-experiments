#pragma once

#include <mutex>
#include <queue>
#include <thread>

#include "Producers.h"

class ToFreeQueue {
public:
	// frees all allocations whose lifetime has elapsed
	void free();
	// Add an allocation to be freed after a particular time
  void addToFree(Allocation a);

private:
	std::mutex lock_;
	std::priority_queue<Allocation, std::vector<Allocation>, std::greater<Allocation>> q_;
};
