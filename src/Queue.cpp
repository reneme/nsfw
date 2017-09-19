#include "../includes/Queue.h"
#include <iostream>
#include <chrono>

#pragma unmanaged

void EventQueue::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    queue.clear();
}

std::size_t EventQueue::count() {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.size();
}

std::unique_ptr<Event> EventQueue::dequeue() {
  std::lock_guard<std::mutex> lock(mutex);
  if (queue.empty()) {
      return nullptr;
  }

  auto& front = queue.front();
  auto retVal = std::move(front);
  queue.pop_front();

  return retVal;
}

std::unique_ptr<std::vector<Event*>> EventQueue::dequeueAll() {
  if (!mutex.try_lock()) {
    auto start = std::chrono::high_resolution_clock::now();
    ++cCont;
    mutex.lock();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << std::endl;
  }

  if (queue.empty()) {
      mutex.unlock();
      return nullptr;
  }

  const auto queueSize = queue.size();
  std::unique_ptr<std::vector<Event*>> events(new std::vector<Event*>(queueSize, nullptr));
  for (size_t i = 0; i < queueSize; ++i) {
      auto& front = queue.front();
      (*events)[i] = front.release();
      queue.pop_front();
  }

  mutex.unlock();
  return events;
}

void EventQueue::enqueue(const EventType type, const std::string& directory, const std::string& fileA, const std::string& fileB) {
  std::lock_guard<std::mutex> lock(mutex);
  queue.emplace_back(std::unique_ptr<Event>(new Event(type, directory, fileA, fileB)));
}
