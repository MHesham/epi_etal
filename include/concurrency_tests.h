#ifndef __CONCURRENCY_TESTS_H__
#define __CONCURRENCY_TESTS_H__

#include "concurrency.h"

void c19_3_interleaving_threads_test()
{
  std::mutex m;
  std::condition_variable cv;
  static const bool ODD_TURN = true;
  static const bool EVEN_TURN = false;
  bool current_turn = ODD_TURN;

  auto worker = [&m, &cv, &current_turn](int start, bool turn, std::string name) {
    std::thread::id my_id = std::this_thread::get_id();
    std::unique_lock<std::mutex> lock_t(m, std::defer_lock);

    lock_t.lock();
    std::cout << "starting thread " << name << " with id = " << my_id << std::endl;
    lock_t.unlock();

    for (int i = start; i <= 100; i += 2) {
      // 1. Wait for the other worker to signal
      lock_t.lock();
      cv.wait(lock_t, [&current_turn, turn]() {
        return current_turn == turn;
      });
      lock_t.unlock();

      // 2. Work 1 time
      lock_t.lock();
      std::cout << name << ":" << i << std::endl;
      current_turn = !current_turn;
      lock_t.unlock();
      cv.notify_one();
    }
  };

  std::thread t1(worker, 1, ODD_TURN, "t1");
  std::thread t2(worker, 2, EVEN_TURN, "t2");

  t1.join();
  t2.join();
}

// reader preferred read/write lock
class reader_writer_first_lock {
public:
  reader_writer_first_lock() :
    shared_count(0)
  {}

  void lock_shared()
  {
    std::lock_guard<std::mutex> lock(shared_count_mx);
    ++shared_count;
    if (shared_count == 1) {
      exclusive_mx.lock();
    }
  }

  void unlock_shared()
  {
    std::lock_guard<std::mutex> lock(shared_count_mx);
    --shared_count;
    if (shared_count == 0) {
      exclusive_mx.unlock();
    }
  }

  void lock_exclusive()
  {
    exclusive_mx.lock();
  }

  void unlock_exclusive()
  {
    exclusive_mx.unlock();
  }

private:
  int shared_count;
  std::mutex shared_count_mx;
  std::mutex exclusive_mx;
};

// writer preferred read/write lock
class reader_writer_second_lock {
public:
  reader_writer_second_lock() :
    shared_count(0),
    exclusive_count(0)
  {}

  void lock_shared()
  {
    std::lock_guard<std::mutex> shared_try_lock(try_shared_mx);
    std::lock_guard<std::mutex> shared_count_lock(shared_count_mx);
    ++shared_count;
    if (shared_count == 1) {
      exclusive_mx.lock();
    }
  }

  void unlock_shared()
  {
    std::lock_guard<std::mutex> shared_count_lock(shared_count_mx);
    --shared_count;
    if (shared_count == 0) {
      exclusive_mx.unlock();
    }
  }

  void lock_exclusive()
  {
    std::lock_guard<std::mutex> exclusive_count_lock(exclusive_count_mx);
    ++exclusive_count;
    if (exclusive_count == 1) {
      try_shared_mx.lock();
    }
    exclusive_mx.lock();
  }

  void unlock_exclusive()
  {
    exclusive_mx.unlock();
    std::lock_guard<std::mutex> exclusive_count_lock(exclusive_count_mx);
    --exclusive_count;
    if (exclusive_count == 0) {
      try_shared_mx.unlock();
    }
  }

private:
  int shared_count;
  int exclusive_count;
  std::mutex shared_count_mx;
  std::mutex exclusive_count_mx;
  std::mutex try_shared_mx;
  std::mutex exclusive_mx;
};


void c19_6_read_writer_lock_test()
{
  using namespace std::chrono_literals;

  std::array<std::string, 4> bank;
  volatile bool shutdown = false;
  std::mutex console_m;
  reader_writer_first_lock bank_lock;

  auto read_worker = [&bank, &bank_lock, &shutdown, &console_m](std::string name, int slot)
  {
    {
      std::lock_guard<std::mutex> con_lock(console_m);
      std::cout << "consumer thread " << name << " starting..." << std::endl;
    }

    std::string data;
    while (!shutdown) {
      bank_lock.lock_shared();
      data = bank[slot];
      bank_lock.unlock_shared();

      if (!data.empty()) {
        std::lock_guard<std::mutex> con_lock(console_m);
        std::cout << name << ": cosnumed " << data << std::endl;
        data.clear();
      }
    }
  };

  auto write_worker = [&bank, &bank_lock, &shutdown, &console_m](std::string name)
  {
    {
      std::lock_guard<std::mutex> con_lock(console_m);
      std::cout << "producer thread " << name << " starting..." << std::endl;
    }

    std::default_random_engine gen(std::random_device{}());
    std::uniform_int_distribution<> data_gen(1, 100);
    std::uniform_int_distribution<> slot_gen(0, bank.size() - 1);

    while (!shutdown) {
      std::string data = std::string("data") + std::to_string(data_gen(gen));
      int slot = slot_gen(gen);
      {
        bank_lock.lock_exclusive();
        std::string& slot_data = const_cast<std::string&>(bank[slot]);
        slot_data = data;
        bank_lock.unlock_exclusive();
      }

      {
        std::lock_guard<std::mutex> con_lock(console_m);
        std::cout << name << ": produced " << data << " for slot " << slot << std::endl;
      }

      std::this_thread::sleep_for(1s);
    }
  };

  std::thread pool[] = {
      std::thread(read_worker, "R1", 0),
      /*std::thread(read_worker, "R2", 1),
      std::thread(read_worker, "R3", 2),
      std::thread(read_worker, "R4", 3),
      std::thread(write_worker, "W1"),*/
      std::thread(write_worker, "W2")
  };

  std::getchar();
  shutdown = true;

  for (auto& t : pool) {
    t.join();
  }
}

void c19_producer_consumer_problem_concurrent_queue_test()
{
  using namespace std::chrono_literals;

  concurrent_bounded_queue<int, 16> q;
  std::mutex console_mx;
  spin_lock producer_lock;
  volatile bool shutdown = false;
  int next_item = 0;

  auto consumer_worker = [&](int id)
  {
    std::string name = "consumer_" + std::to_string(id);

    {
      std::lock_guard<std::mutex> con_lock(console_mx);
      std::cout << "consumer thread " << name << " starting..." << std::endl;
    }

    while (!shutdown) {
      int data = q.dequeue();
      {
        std::lock_guard<std::mutex> con_lock(console_mx);
        std::cout << name << ": cosnumed " << data << std::endl;
      }
    }
  };

  auto producer_worker = [&](int id)
  {
    std::string name = "producer_" + std::to_string(id);

    {
      std::lock_guard<std::mutex> con_lock(console_mx);
      std::cout << "producer thread " << name << " starting..." << std::endl;
    }

    while (!shutdown) {

      int data;
      {
        producer_lock.lock();
        data = next_item++;
        producer_lock.unlock();
      }
      q.enqueue(data);

      {
        std::lock_guard<std::mutex> con_lock(console_mx);
        std::cout << name << ": produced " << data << std::endl;
      }

      std::this_thread::sleep_for(250ms);
    }
  };

  const int consumer_count = 2;
  const int producer_count = 4;
  std::thread pool[consumer_count + producer_count];

  for (int i = 0; i < consumer_count; ++i) {
    pool[i] = std::thread(consumer_worker, i);
  }

  for (int i = consumer_count; i < consumer_count + producer_count; ++i) {
    pool[i] = std::thread(producer_worker, i - consumer_count);
  }

  std::getchar();
  shutdown = true;

  for (auto& t : pool) {
    t.join();
  }
}

void c19_producer_consumer_problem_test()
{
  using namespace std::chrono_literals;

  std::array<int, 16> buffer;
  int next_in = 0;
  int next_out = 0;
  semaphore can_produce(buffer.size());
  semaphore can_consume(0);
  std::mutex producer_mx;
  std::mutex consumer_mx;
  std::mutex console_mx;
  int next_item = 0;
  volatile bool shutdown = false;

  auto consumer_worker = [&](int id)
  {
    std::string name = "consumer_" + std::to_string(id);

    {
      std::lock_guard<std::mutex> con_lock(console_mx);
      std::cout << "consumer thread " << name << " starting..." << std::endl;
    }

    while (!shutdown) {
      can_consume.wait();

      int data;
      int out;
      {
        std::lock_guard<std::mutex> lock(consumer_mx);
        out = next_out;
        data = buffer[next_out++];
        next_out %= buffer.size();
      }

      can_produce.signal();

      std::lock_guard<std::mutex> con_lock(console_mx);
      std::cout << name << ": cosnumed " << data << " from slot " << out << std::endl;
    }
  };

  auto producer_worker = [&](int id)
  {
    std::string name = "producer_" + std::to_string(id);

    {
      std::lock_guard<std::mutex> con_lock(console_mx);
      std::cout << "producer thread " << name << " starting..." << std::endl;
    }

    while (!shutdown) {
      can_produce.wait();
      int data;
      int in;
      {
        std::lock_guard<std::mutex> lock(producer_mx);
        data = next_item;
        in = next_in;
        buffer[next_in++] = next_item++;
        next_in %= buffer.size();
      }

      can_consume.signal();

      {
        std::lock_guard<std::mutex> con_lock(console_mx);
        std::cout << name << ": produced " << data << " for slot " << in << std::endl;
      }

      std::this_thread::sleep_for(500ms);
    }
  };

  const int consumer_count = 2;
  const int producer_count = 4;
  std::thread pool[consumer_count + producer_count];

  for (int i = 0; i < consumer_count; ++i) {
    pool[i] = std::thread(consumer_worker, i);
  }

  for (int i = consumer_count; i < consumer_count + producer_count; ++i) {
    pool[i] = std::thread(producer_worker, i - consumer_count);
  }

  std::getchar();
  shutdown = true;

  for (auto& t : pool) {
    t.join();
  }
}

#endif
