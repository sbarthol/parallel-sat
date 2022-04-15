#include "concurrent_queue.h"

using namespace std;

template <typename T>
void ConcurrentQueue<T>::push(T const& v) {
  unique_lock lock(mx);
  q.push(v);
  lock.unlock();
  cv.notify_one();
}

template <typename T>
bool ConcurrentQueue<T>::empty() const {
  unique_lock lock(mx);
  return q.empty();
}

template <typename T>
bool ConcurrentQueue<T>::try_pop(T& v) {
  unique_lock lock(mx);
  if (q.empty()) {
    return false;
  }
  v = q.front();
  q.pop();
  return true;
}

template <typename T>
void ConcurrentQueue<T>::wait_and_pop(T& v) {
  unique_lock lock(mx);
  while (q.empty()) {
    cv.wait(lock);
  }
  v = q.front();
  q.pop();
}

template class ConcurrentQueue<int>;
