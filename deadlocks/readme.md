##  The code & explanation  for  Deadlocks ,the lock tags

```cpp
/*
    ///////////////////
    ⚠️ Deadlocks
        🔄 What is a Deadlock?
        A deadlock happens when two or more threads are waiting for each other’s locks and none can proceed.

        ❌ Example (Classic Deadlock)
            std::mutex m1, m2;

            void thread1() {
                std::lock_guard<std::mutex> lock1(m1);
                std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Delay
                std::lock_guard<std::mutex> lock2(m2); // Waits on m2
            }

            void thread2() {
                std::lock_guard<std::mutex> lock2(m2);
                std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Delay
                std::lock_guard<std::mutex> lock1(m1); // Waits on m1
            }
            Both threads hold one mutex and wait for the other — stuck forever.

        🛡️ How to Avoid Deadlocks?
            Always acquire multiple mutexes in the same order.
            Use std::lock() to lock multiple mutexes atomically:
                std::lock(m1, m2); // Lock both without risk of deadlock
                std::lock_guard<std::mutex> lock1(m1, std::adopt_lock);
                std::lock_guard<std::mutex> lock2(m2, std::adopt_lock);

        Here's a list of all the lock tags used in C++ for std::unique_lock and std::lock_guard:
            std::adopt_lock:
                This tag is used to indicate that the mutex has already been locked outside the std::unique_lock or std::lock_guard object. 
                When you use std::adopt_lock, the std::unique_lock does not try to lock the mutex; 
                it simply assumes ownership of the already locked mutex.
            std::defer_lock:
                This tag indicates that the mutex should not be locked immediately when the std::unique_lock or std::lock_guard is constructed. 
                This is useful when you want to delay locking the mutex until some other condition is met or some other logic is executed.
            std::try_to_lock
                This tag will try to lock the mutex at the time of std::unique_lock or std::lock_guard construction, 
                but it won't block. If the mutex is already locked by another thread, 
                the constructor will immediately return without acquiring the lock.
            std::unique_lock and std::lock_guard:
                These are the actual classes you use in combination with the above tags. 
                You pass the tag as the second argument during their construction to customize the locking behavior.
            Summary:
                std::adopt_lock_t – assume the mutex is already locked.
                std::defer_lock_t – do not lock the mutex immediately.
                std::try_to_lock_t – attempt to lock non-blockingly.

    ///////////////////

*/
```

##  Deadlock example

```cpp
#include <iostream>
#include <thread>
#include <mutex>

int counter = 0; // Shared data
std::mutex m1, m2;

void thread1() {
    // std::lock_guard<std::mutex> lock1(m1);
    // std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Delay
    // std::lock_guard<std::mutex> lock2(m2); // Waits on m2
    /*********************/
    // std::lock(m1, m2); // Lock both without risk of deadlock
    // //RAII
    // std::lock_guard<std::mutex> lock1(m1, std::adopt_lock);
    // std::lock_guard<std::mutex> lock2(m2, std::adopt_lock);
    /*******************/
    std::scoped_lock lock(m1 , m2);
    counter++;
}//m2.unlock , m1.unlock

void thread2() {
    // std::lock_guard<std::mutex> lock1(m1); // Waits on m1
    // std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Delay
    // std::lock_guard<std::mutex> lock2(m2);
    /*********************/
    // std::lock(m1, m2); // Lock both without risk of deadlock
    // //RAII
    // std::lock_guard<std::mutex> lock1(m1, std::adopt_lock);
    // std::lock_guard<std::mutex> lock2(m2, std::adopt_lock);
    /*******************/
    std::scoped_lock lock(m1 , m2);
    counter++;
}//m1.unlock , m2.unlock  

int main() {
    std::thread t1(thread1);
    std::thread t2(thread2);
    t1.join();
    t2.join();
    std::cout << "Counter: " << counter << "\n";
    return 0;
}

```

##     the possible implementation for lock_guard constructor with the lock tags

```cpp


//RAII
#include <mutex>
#include <stdexcept>

template <typename mutex_type>
class lock_guard {
public:
    // Default constructor: locks the mutex
    explicit lock_guard(mutex_type& m) : mutex_(m), owns_lock_(true) {
        mutex_.lock();
    }

    // Adopt lock: mutex is already locked
    lock_guard(mutex_type& m, std::adopt_lock_t) noexcept
        : mutex_(m), owns_lock_(true) {
        // Assumes mutex is already locked
    }

    // Defer lock: don't lock now
    lock_guard(mutex_type& m, std::defer_lock_t) noexcept
        : mutex_(m), owns_lock_(false) {
        // Locking is deferred
    }

    // Try to lock: non-blocking
    lock_guard(mutex_type& m, std::try_to_lock_t)
        : mutex_(m) {
        owns_lock_ = mutex_.try_lock();
        if (!owns_lock_) {
            throw std::runtime_error("Failed to acquire mutex with try_to_lock");
        }
    }

    // Deleted copy operations
    lock_guard(const lock_guard&) = delete;
    lock_guard& operator=(const lock_guard&) = delete;

    // Destructor: unlock only if we own the lock
    ~lock_guard() noexcept {
        if (owns_lock_) {
            mutex_.unlock();
        }
    }

    // Optional helper function
    bool owns_lock() const noexcept {
        return owns_lock_;
    }

private:
    mutex_type& mutex_;
    bool owns_lock_;
};


```