#include <queue>
#include <thread>
#include <functional>
#include <iostream>
#include <mutex>
#include <condition_variable>

using Task = std::function<void()>;

class TaskQueue {
 public:
  TaskQueue() : _processingThread{[this] { Process(); }} {}
  TaskQueue(TaskQueue const&) = delete;
  TaskQueue& operator=(TaskQueue const&) = delete;
  TaskQueue(TaskQueue&&) = delete;
  TaskQueue& operator=(TaskQueue&&) = delete;
  ~TaskQueue() {
    {
      std::lock_guard<std::mutex> lk(_taskMutex);
      _shouldTerminate = true;
    }
    _cvTask.notify_all();
    _processingThread.join();
  }

  void Push(Task task) {
    std::lock_guard<std::mutex> lk(_taskMutex);
    if (_shouldTerminate) return;
    this->_tasks.push(std::move(task));
    _cvTask.notify_one();
  }

 private:
  void Process() {
    while(true) {
      std::unique_lock<std::mutex> lk(_taskMutex);
      _cvTask.wait(lk, [this]() {
        return !_tasks.empty() || _shouldTerminate;
      });
      if (_shouldTerminate && _tasks.empty()) {
        return;
      }
      auto task = std::move(this->_tasks.front());
      this->_tasks.pop();
      lk.unlock(); // Unlock in case task() is expensive

      task();
    }
  }

  bool _shouldTerminate = false;
  mutable std::mutex _taskMutex;
  std::condition_variable _cvTask;
  std::queue<Task> _tasks;
  std::thread _processingThread;
};

int main() {
  TaskQueue tq;
  tq.Push([](){ std::cout << "Task 1" << std::endl;});
  tq.Push([](){ std::cout << "Task 2" << std::endl;});
  tq.Push([](){ std::cout << "Task 3" << std::endl;});

  return 0;
}