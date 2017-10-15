#ifndef __CONCURRENCY_H__
#define __CONCURRENCY_H__

#include "common.h"

class semaphore {
public:
  semaphore(int count) :
    count_(count)
  {}

  void wait()
  {
    std::unique_lock<std::mutex> lock_(mx_);
    --count_;
    while (count_ < 0) {
      cv_.wait(lock_);
    }
  }

  void signal()
  {
    std::unique_lock<std::mutex> lock_(mx_);
    ++count_;
    if (count_ <= 0) {
      lock_.unlock();
      cv_.notify_one();
    }
  }

  std::mutex mx_;
  std::condition_variable cv_;
  int count_;
};

template<class T, size_t SIZE = 16>
class concurrent_bounded_queue {
public:
  concurrent_bounded_queue() :
    next_in(0),
    next_out(0),
    can_produce(SIZE),
    can_consume(0)
  {}

  void enqueue(T& val)
  {
    can_produce.wait();
    buffer[next_in++] = val;
    next_in %= SIZE;
    can_consume.signal();
  }

  T dequeue()
  {
    can_consume.wait();
    T& val = buffer[next_out++];
    next_out %= SIZE;
    can_produce.signal();
    return val;
  }

private:
  std::array<T, SIZE> buffer;
  int next_in;
  int next_out;
  semaphore can_produce;
  semaphore can_consume;
};

class spin_lock {
public:
  spin_lock() :
    state(FREE_STATE)
  {}

  void lock()
  {
    int tst_val = FREE_STATE;
    while (!state.compare_exchange_strong(tst_val, AQUIRED_STATE)) {
      /* do nothing, just spin*/
    }
  }

  void unlock()
  {
    state.store(FREE_STATE);
  }

private:
  static const int FREE_STATE = 0;
  static const int AQUIRED_STATE = 1;
  std::atomic_int state;
};

#endif // __CONCURRENCY_H__
