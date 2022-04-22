#include <condition_variable>
#include <mutex>
#include <queue>

#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

template <typename T> class ConcurrentQueue {
private:
  std::queue<T> q;
  mutable std::mutex mx;
  std::condition_variable cv;

public:
  ConcurrentQueue();
  ConcurrentQueue(ConcurrentQueue &&other);
  void push(T const &v);
  bool empty() const;
  bool try_pop(T &v);
  T wait_and_pop();
};

#endif