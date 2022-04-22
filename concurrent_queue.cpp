#include "concurrent_queue.h"

#include <vector>

#include "multi_bit_solver.h"

using namespace std;

template <typename T> ConcurrentQueue<T>::ConcurrentQueue() {}

template <typename T>
ConcurrentQueue<T>::ConcurrentQueue(ConcurrentQueue &&other) {}

template <typename T> void ConcurrentQueue<T>::push(T const &v) {
  unique_lock<std::mutex> lock(mx);
  q.push(v);
  lock.unlock();
  cv.notify_one();
}

template <typename T> bool ConcurrentQueue<T>::empty() const {
  unique_lock<std::mutex> lock(mx);
  return q.empty();
}

template <typename T> bool ConcurrentQueue<T>::try_pop(T &v) {
  unique_lock<std::mutex> lock(mx);
  if (q.empty()) {
    return false;
  }
  v = q.front();
  q.pop();
  return true;
}

template <typename T> T ConcurrentQueue<T>::wait_and_pop() {
  unique_lock<std::mutex> lock(mx);
  while (q.empty()) {
    cv.wait(lock);
  }
  T v = q.front();
  q.pop();
  return v;
}

template class ConcurrentQueue<MultiBitSolver::uintk_t>;
template class ConcurrentQueue<vector<MultiBitSolver::uintk_t>>;
