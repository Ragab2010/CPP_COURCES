##  The code & explanation  for  🔐 Critical Section & ⚛️ Atomic Section

```cpp
    ///////////////////
    🔐 Critical Section
        A critical section is a piece of code that accesses shared resources (like variables, files, or hardware), 
        and must not be concurrently executed by more than one thread.
        💥 Why?
            To prevent race conditions.
        🔒 How to protect it?
            Using mutual exclusion (mutex).
        ✅ Examples:
            Example1:
            std::mutex mtx;
            int shared_data = 0;

            void safe_increment() {
                std::lock_guard<std::mutex> lock(mtx); // Enter critical section
                ++shared_data;//
                // lock released automatically here (RAII)
            }

            Only one thread at a time can enter the critical section protected by mtx.

            Example2:
            #include <iostream>
            #include <thread>
            #include <mutex>

            int counter = 0; // Shared resource
            std::mutex mtx;

            void increment(int n) {
                for (int i = 0; i < n; ++i) {
                    std::lock_guard<std::mutex> lock(mtx); // Start critical section
                    counter++; // Only one thread can execute this at a time
                } // End critical section (lock released)
            }

            int main() {
                std::thread t1(increment, 1000);
                std::thread t2(increment, 1000);
                t1.join();
                t2.join();
                std::cout << "Final counter: " << counter << "\n"; // Always 2000
                return 0;
            }

    ⚛️ Atomic Section
        An atomic section is a small critical section that:
            Is performed as a single, indivisible operation.
            Cannot be interrupted or interleaved by other threads.
            Usually applies to single variables or small compound ops.
        In C++, atomic sections are handled using the <atomic> library.
        ✅ Example:
            Example1:
            #include <atomic>

            std::atomic<int> counter = 0;

            void increment() {
                ++counter; // This is atomic — no mutex needed
            }
            ++counter here is atomic — the read-modify-write is done as one uninterruptible step.

            Example2:

            #include <iostream>
            #include <thread>
            #include <atomic>

            std::atomic<int> counter(0); // Atomic shared resource

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
                std::cout << "Final counter: " << counter << "\n"; // Always 2000
                return 0;
            }
    ⚖️ Critical Section vs Atomic Section
        | Feature           | Critical Section                 | Atomic Section                       |
        | ----------------- | -------------------------------- | ------------------------------------ |
        | Protection Method | `std::mutex`, `std::lock_guard`  | `std::atomic<T>`                     |
        | Scope             | Any block of code                | Single-variable operations           |
        | Performance       | Slower (context switch, locking) | Faster (lock-free, hardware support) |
        | Granularity       | Coarse ( whole code blocked)     | Fine  ( only opr blocked)            |
        | Thread-safe?      | Yes                              | Yes                                  |

    🧠 When to Use Which?
        Critical Section (Mutex):
            Use for complex operations (e.g., updating multiple variables, accessing a data structure).
            Example: Modifying a std::vector or writing to a file.
            Trade-off: Locking can slow down performance if contention is high.
        Atomic Section (std::atomic):
            Use for simple, single-variable operations (e.g., increment, flag setting).
            Example: Counters, flags, or pointers.
            Trade-off: Limited to what std::atomic supports; not for complex logic.

    
```
