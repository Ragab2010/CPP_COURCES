##  The code & explanation  for  Race Conditions

```cpp
    ///////////////////
    🔥Race Condition:
        A race condition occurs when two or more threads access shared data concurrently, and at least one of them modifies the data, 
        leading to unpredictable outcomes. 
        The result depends on the timing or order of thread execution, which is non-deterministic and results become incorrect .

        Key Idea: 
            A race condition happens when threads "race" to access/modify shared resources without proper synchronization, 
            causing inconsistent or incorrect results.
            In C++: Common with std::thread when threads share variables (e.g., global data, object members) without protection.
        
        Why Do Race Conditions Happen?
        Race conditions arise due to:
            Shared Data: Multiple threads access the same memory (e.g., a global variable, object member, or heap-allocated data).
            Concurrent Access: Threads run simultaneously or interleave unpredictably.
            Lack of Synchronization: No mechanism (like locks) ensures exclusive access during modifications.
            For example, if two threads increment a shared counter, the operation counter++ isn’t atomic—it involves reading, 
            modifying, and writing. If the threads interleave, the final value may be incorrect.
        🚨Example of a Race Condition with std::thread
            1. Race Condition Example
            Description: Two threads increment a shared counter without synchronization.

            #include <iostream>
            #include <thread>

            int counter = 0; // Shared data

            void increment(int n) {
                for (int i = 0; i < n; ++i) {
                    std::this_thread::sleep_for(std::chrono::microseconds(1)); // artificial delay
                    counter++; // Not atomic: read, increment, write
                }
            }

            int main() {
                std::thread t1(increment, 1000);
                std::thread t2(increment, 1000);
                t1.join();
                t2.join();
                std::cout << "Final counter: " << counter << "\n";
                return 0;
            }
            Expected Output: If each thread increments 1000 times, counter should be 2000.
            Actual Output: Often less than 2000 (e.g., 1987, 1995), varying per run.
            Why?: The counter++ operation is interrupted mid-execution. Example sequence:
            Thread 1: Reads counter = 100.
            Thread 2: Reads counter = 100.
            Thread 1: Increments to 101, writes back.
            Thread 2: Increments to 101, writes back.
            Result: counter = 101 instead of 102 for two increments.
        🚨Example: Member Function Race Condition
            #include <iostream>
            #include <thread>

            class Counter {
            public:
                int value = 0;
                void increment(int n) {
                    for (int i = 0; i < n; ++i) {
                        value++; // Race condition if shared
                    }
                }
            };

            int main() {
                Counter c;
                std::thread t1(&Counter::increment, &c, 1000);
                std::thread t2(&Counter::increment, &c, 1000);
                t1.join();
                t2.join();
                std::cout << "Value: " << c.value << "\n";
                return 0;
            }
            Issue: Both threads modify c.value, causing a race condition.

            Fix: Add a mutex:

            #include <mutex>
            class Counter {
            public:
                int value = 0;
                std::mutex mtx;
                void increment(int n) {
                    for (int i = 0; i < n; ++i) {
                        std::lock_guard<std::mutex> lock(mtx);
                        value++;
                    }
                }
            };
        How to Prevent Race Conditions
        To avoid race conditions, you need to synchronize access to shared data. C++ provides several tools:
            1. Mutex (Mutual Exclusion)
            Description: A std::mutex ensures only one thread accesses the shared data at a time (critical section).
            Example:
                #include <iostream>
                #include <thread>
                #include <mutex>

                int counter = 0;
                std::mutex mtx;

                void increment(int n) {
                    for (int i = 0; i < n; ++i) {
                        std::lock_guard<std::mutex> lock(mtx); // Lock mutex
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
                Output:
                Final counter: 2000
                Notes: std::lock_guard locks the mutex on entry and unlocks on exit, ensuring thread-safe increments.
            2. Atomic Operations
            Description: Use std::atomic for variables that support lock-free, atomic operations.
            Example:

                #include <iostream>
                #include <thread>
                #include <atomic>

                std::atomic<int> counter(0); // Atomic counter

                void increment(int n) {
                    for (int i = 0; i < n; ++i) {
                        counter++; // Atomic operation
                    }
                }

                int main() {
                    std::thread t1(increment, 1000);
                    std::thread t2(increment, 1000);
                    t1.join();
                    t2.join();
                    std::cout << "Final counter: " << counter << "\n";
                    return 0;
                }
                Output:
                Final counter: 2000
                Notes: std::atomic ensures counter++ is indivisible, avoiding race conditions without explicit locks.
            3. Avoid Shared State
            Description: If possible, give each thread its own data to avoid sharing.
            Example:
                #include <iostream>
                #include <thread>

                void increment(int n, int& local_counter) {
                    for (int i = 0; i < n; ++i) {
                        local_counter++;
                    }
                }

                int main() {
                    int counter1 = 0, counter2 = 0;
                    std::thread t1(increment, 1000, std::ref(counter1));
                    std::thread t2(increment, 1000, std::ref(counter2));
                    t1.join();
                    t2.join();
                    std::cout << "Total: " << (counter1 + counter2) << "\n";
                    return 0;
                }
                Output:
                Total: 2000
                Notes: No shared data, no race condition.
    ///////////////////
```

##  The code for  Mutex and possible implementation for std::lock_guard

```cpp

#include <iostream>
#include <thread>
#include <mutex>
/*

    Expected Output: If each thread increments 1000 times, counter should be 2000.
    Actual Output: Often less than 2000 (e.g., 1987, 1995), varying per run.
    Why?: The counter++ operation is interrupted mid-execution. Example sequence:
    -->counter++ (read value + increment value + write back value).

    Thread 1: Reads counter = 100.
    Thread 2: Reads counter = 100.
    Thread 1: Increments to 101, writes back.
    Thread 2: Increments to 101, writes back.
    
    Result: counter = 101 instead of 102 for two increments.
    This happens multiple times, leading to fewer increments than expected.

    ************************

    Expected Output: If each thread increments 1000 times, counter should be 2000.
    Actual Output: 2000 (Correct result).
    Why?: Using a mutex ensures that only one thread can modify the counter at a time. Example sequence:
    -->counter++ (read + increment + write back).

    Thread 1: Acquires lock on mutex, reads counter = 100.
    Thread 2: Try Acquires lock on mutex found it locked--> Thread 2 go to blocked state to wait.
    Thread 1: Increments to 101, writes back.
    Thread 1: Releases lock.
    
    Thread 2: wake-up from blocked state.
    Thread 2: Acquires lock on mutex, reads counter = 101.
    Thread 2: Increments to 102, writes back.
    Thread 2: Releases lock.
    
    Result: counter = 102 for two increments, and no increments are lost.
    By serializing access to the shared counter, both threads can safely update the counter.

    /////////////////////
    atomic operation : Is performed as a single, Cannot be interrupted or interleaved by other threads.
    -->counter++ (read + increment + write back).

    Thread 1: Reads counter = 100,Increments to 101, writes back.
    Thread 2: Reads counter = 101,Increments to 102, writes back.
    
*/
  
int counter = 0; // Shared data
std::mutex mtx;

//RAII
template <typename mutex_type>
class lock_guard {
public:
    // Type alias for the mutex type
    //using mutex_type = Mutex;

    // Constructor: locks the mutex
    explicit lock_guard(mutex_type& m) : mutex_(m) {
        mutex_.lock();
    }

    // // Constructor: adopts an already locked mutex
    // lock_guard(mutex_type& m, adopt_lock_t) noexcept : mutex_(m) {
    //     // No locking, mutex is already locked
    // }

    // Destructor: unlocks the mutex
    ~lock_guard() noexcept {
        mutex_.unlock();
    }

    // Delete copy constructor and assignment operator to prevent copying
    lock_guard(const lock_guard&) = delete;
    lock_guard& operator=(const lock_guard&) = delete;

private:
    mutex_type& mutex_; // Reference to the mutex
};

void increment(int n) {
    for (int i = 0; i < n; ++i) {
        std::this_thread::sleep_for(std::chrono::microseconds(1)); // artificial delay
        std::lock_guard<std::mutex> mttx(mtx);////////////mtx.lock
        counter++; // Not atomic: read, increment, write////

    }///////////////mtx.unlock
}

int main() {
    std::thread t1(increment, 1000);
    std::thread t2(increment, 1000);
    t1.join();
    t2.join();
    std::cout << "Final counter: " << counter << "\n";
    return 0;
}


```

##  The code for Atomic

```cpp
#include <iostream>
#include <thread>
#include <atomic>

std::atomic<int> counter(0); // Atomic counter

void increment(int n) {
    for (int i = 0; i < n; ++i) {
        counter++; // Atomic operation
    }
}

int main() {
    std::thread t1(increment, 1000);
    std::thread t2(increment, 1000);
    t1.join();
    t2.join();
    std::cout << "Final counter: " << counter << "\n";
    return 0;
}

```