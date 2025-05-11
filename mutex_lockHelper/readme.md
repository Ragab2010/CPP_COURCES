##  The code & explanation  for mutex & Common Lock Helpers 

```cpp
/*
    ///////////////////
    🔒A mutex (short for mutual exclusion):
        is a synchronization primitive used in C++ to protect critical sections by ensuring that only one thread can access 
        the shared resource at a time.

        What Does a Mutex Do?
        Imagine a bathroom with a lock on the door:
        If a thread acquires the lock, it enters the critical section (like entering the bathroom).
        Other threads trying to acquire it must wait (like waiting outside).
        Once the thread releases the lock, another thread can enter.
        
        🧱 C++ Mutex: Basic Usage
            Example:
                #include <iostream>
                #include <thread>
                #include <mutex>

                std::mutex mtx;
                int counter = 0;

                void safe_increment() {
                    for (int i = 0; i < 100000; ++i) {
                        std::lock_guard<std::mutex> lock(mtx); // Locks automatically
                        ++counter; // Critical section
                    } // lock released automatically here
                }

                int main() {
                std::thread t1(safe_increment);
                std::thread t2(safe_increment);

                t1.join();
                t2.join();

                std::cout << "Counter: " << counter << "\n"; // Always 200000
                }

        🛠️ Mutex Types in C++:
            | Type                   | Description                                                |
            | ---------------------- | ---------------------------------------------------------- |
            | `std::mutex`           | Basic mutex.                                               |
            | `std::recursive_mutex` | Allows same thread to lock multiple times (use with care). |
            | `std::timed_mutex`     | Like `std::mutex` but allows timed attempts.               |
            | `std::shared_mutex`    | Readers-writer lock: many readers or one writer. (C++17+)  |
        

        🛡️ Best Practices
            Prefer std::lock_guard or std::unique_lock for RAII-based locking (automatic release).
            Avoid deadlocks: two threads waiting for each other forever.
            Don’t hold a mutex longer than necessary.
            Avoid copying mutexes (they're non-copyable).

        🔁 Incorrect Usage (Race Condition)
            void unsafe_increment() {
                ++counter; // NO mutex — dangerous!
            }
            Threads can interleave and corrupt the shared counter.

        🚨Examples of Mutex with std::thread
            Let’s see mutexes in action, addressing scenarios from our prior discussions (e.g., race conditions, critical sections).

            1. Basic Mutex with std::lock_guard
            Description: Protect a shared counter from a race condition using std::mutex and std::lock_guard.
            Example:
                #include <iostream>
                #include <thread>
                #include <mutex>

                int counter = 0; // Shared resource
                std::mutex mtx;  // Mutex to protect counter

                void increment(int n) {
                    for (int i = 0; i < n; ++i) {
                        std::lock_guard<std::mutex> lock(mtx); // Acquire lock
                        counter++; // Critical section
                    } // Lock released automatically
                }

                int main() {
                    std::thread t1(increment, 1000);
                    std::thread t2(increment, 1000);
                    t1.join();
                    t2.join();
                    std::cout << "Final counter: " << counter << "\n";
                    return 0;
                }
                Output: Final counter: 2000
                Notes:
                std::lock_guard locks the mutex on construction and unlocks it on destruction (RAII), ensuring the lock is released even if an exception occurs.
                Without the mutex, counter++ would cause a race condition, as we saw earlier.

            2. Mutex with a Member Function
            Description: Use a mutex in a class to protect a member variable, tying to our discussion on member functions.
            Example:
                
                #include <iostream>
                #include <thread>
                #include <mutex>

                class Counter {
                    int value = 0;
                    std::mutex mtx;
                public:
                    void increment(int n) {
                        for (int i = 0; i < n; ++i) {
                            std::lock_guard<std::mutex> lock(mtx);
                            value++;
                        }
                    }
                    int get_value() const { return value; }
                };

                int main() {
                    Counter c;
                    std::thread t1(&Counter::increment, &c, 1000);
                    std::thread t2(&Counter::increment, &c, 1000);
                    t1.join();
                    t2.join();
                    std::cout << "Value: " << c.get_value() << "\n";
                    return 0;
                }
                Output: Value: 2000
                Notes: The mutex protects value, ensuring thread-safe increments.
            3. Mutex with a Lambda
            Description: Use a lambda (from our callable discussions) with a mutex to update shared data.
            Example:
                #include <iostream>
                #include <thread>
                #include <mutex>

                int main() {
                    int counter = 0;
                    std::mutex mtx;
                    auto inc = [&counter, &mtx](int n) {
                        for (int i = 0; i < n; ++i) {
                            std::lock_guard<std::mutex> lock(mtx);
                            counter++;
                        }
                    };
                    std::thread t1(inc, 1000);
                    std::thread t2(inc, 1000);
                    t1.join();
                    t2.join();
                    std::cout << "Counter: " << counter << "\n";
                    return 0;
                }
                Output: Counter: 2000
                Notes: The lambda captures the mutex and counter, protecting the critical section.
            4. Manual Locking with std::mutex
            Description: Use lock() and unlock() directly (less common, more error-prone).
            Example:
                #include <iostream>
                #include <thread>
                #include <mutex>

                int counter = 0;
                std::mutex mtx;

                void increment(int n) {
                    for (int i = 0; i < n; ++i) {
                        mtx.lock(); // Manual lock
                        counter++;
                        mtx.unlock(); // Manual unlock
                    }
                }

                int main() {
                    std::thread t1(increment, 1000);
                    std::thread t2(increment, 1000);
                    t1.join();
                    t2.join();
                    std::cout << "Counter: " << counter << "\n";
                    return 0;
                }
                Output: Counter: 2000
                Notes: Prefer std::lock_guard or std::unique_lock to avoid forgetting unlock() or handling exceptions poorly.
            5. Using std::unique_lock for Flexibility
            Description: std::unique_lock allows deferred locking or manual control, useful for condition variables (we’ll touch on this briefly).
            Example:

                #include <iostream>
                #include <thread>
                #include <mutex>

                int counter = 0;
                std::mutex mtx;

                void increment(int n) {
                    for (int i = 0; i < n; ++i) {
                        std::unique_lock<std::mutex> lock(mtx); // Flexible lock
                        counter++;
                    }
                }

                int main() {
                    std::thread t1(increment, 1000);
                    std::thread t2(increment, 1000);
                    t1.join();
                    t2.join();
                    std::cout << "Counter: " << counter << "\n";
                    return 0;
                }
                Output: Counter: 2000
                Notes: std::unique_lock is heavier than std::lock_guard but supports advanced features like condition variables.
                Types of Mutexes in C++
            C++ provides several mutex types in <mutex>:
                std::mutex: Basic mutex for mutual exclusion.
                std::recursive_mutex: Allows a thread to lock the same mutex multiple times (must unlock the same number of times).
                std::timed_mutex: Supports trying to lock with a timeout.
                std::recursive_timed_mutex: Combines recursive and timed features.
            Lock helpers:
                std::lock_guard: Simple RAII lock for automatic release.
                std::unique_lock: Flexible lock for condition variables or deferred locking.
                std::scoped_lock (C++17): Locks multiple mutexes safely to avoid deadlocks.    
    ///////////////////
    ///////////////////
    🔑 Common Lock Helpers in C++
        | Lock Helper                | Description                                                                          |
        | -------------------------- | ------------------------------------------------------------------------------------ |
        | `std::lock_guard`          | Simple, scoped lock. Locks immediately, unlocks automatically.                       |
        | `std::unique_lock`         | More flexible — can defer lock, unlock manually, move ownership.                     |
        | `std::scoped_lock` (C++17) | Can lock multiple mutexes at once (avoids deadlock).                                 |
        | `std::lock()`              | A function, not a class — locks multiple mutexes atomically. Used with `adopt_lock`. |

        🧩 1. std::lock_guard
            Simplest and safest helper — locks a mutex when created, and unlocks when it goes out of scope.
            Example:
                std::mutex mtx;

                void safe() {
                    std::lock_guard<std::mutex> lock(mtx);
                    // critical section
                } // mtx automatically unlocked here
                ✅ Pros:
                Very lightweight.
                Exception-safe.

                ❌ Cons:
                Can’t unlock early or re-lock — fixed behavior.

        🧠 2. std::unique_lock
            More powerful than lock_guard. You can:
                Defer locking (std::defer_lock)
                Unlock and re-lock manually
                Transfer ownership (moveable)
            Example:
                std::mutex mtx;

                void flexible() {
                    std::unique_lock<std::mutex> lock(mtx); // locked now
                    lock.unlock();                          // manual unlock
                    // do something non-critical
                    lock.unlock();                            // re-lock if needed
                }
                Also works with std::condition_variable.

        🔗 3. std::scoped_lock (C++17+)
            Locks multiple mutexes safely, avoiding deadlocks.
            Example:
                std::mutex m1, m2;

                void safe_multi_lock() {
                    std::scoped_lock lock(m1, m2); // locks both safely
                    // do something with shared resources
                }
                Internally uses std::lock() for atomic locking.

        ⚙️ 4. std::lock() Function
            Not a class — used to lock multiple mutexes at once, avoiding deadlock. 
            Used with std::lock_guard or std::unique_lock and std::adopt_lock.
            Example:
                std::mutex m1, m2;

                void manual_multi_lock() {
                    std::lock(m1, m2);//m1.lock() , m2.lock()
                    std::lock_guard<std::mutex> l1(m1, std::adopt_lock);
                    std::lock_guard<std::mutex> l2(m2, std::adopt_lock);
                }
        Summary Table
            | Helper             | Can Unlock Manually?  | Multiple Mutexes  | Best Use Case                 |
            | ------------------ | --------------------  | ----------------  | ----------------------------- |
            | `std::lock_guard`  | ❌ No                 | ❌ No             | Simple locking                |
            | `std::unique_lock` | ✅ Yes                | ❌ No             | Flexible, condition variables |
            | `std::scoped_lock` | ❌ No                 | ✅ Yes            | C++17+, multiple mutexes      |
            | `std::lock()`      | N/A (function)        | ✅ Yes            | Safe multi-lock setup         |
    ///////////////////
*/
```
