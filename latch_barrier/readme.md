# C++20 Synchronization Primitives: `std::latch` and `std::barrier`

This document explains the `std::latch` and `std::barrier` synchronization primitives introduced in C++20, designed for thread coordination in concurrent programming.

## `std::latch` in C++20

`std::latch` is a **single-use** synchronization primitive that allows threads to wait until a counter reaches zero. It’s ideal for coordinating one-time events, such as waiting for multiple threads to complete their setup before proceeding.

### Key Features
- **Single-use**: Cannot be reused once the counter reaches zero.
- **Purpose**: Ensures threads wait for a specific number of operations to complete.
- **Defined in**: `<latch>` (C++20 and later).
- **Analogy**: Like a classroom where the teacher waits for all students to be seated before starting the lesson.

### How It Works
1. Initialize a `std::latch` with a non-negative counter.
2. Threads call `count_down()` to decrement the counter, signaling progress.
3. Threads can call `wait()` to block until the counter reaches zero.
4. Once the counter hits zero, all waiting threads are unblocked, and the latch is done.

### Syntax
```cpp
#include <latch>
std::latch latch_object(count);
```

### Key Methods
| Method               | Description                                          |
|----------------------|-----------------------------------------------------|
| `count_down(n=1)`   | Decrements the counter by `n` (thread-safe).        |
| `wait()`            | Blocks until the counter reaches zero.              |
| `try_wait()`        | Returns `true` if the counter is zero, else `false`.|
| `arrive_and_wait()` | Decrements the counter by 1 and waits until zero.   |

### Use Cases
- Waiting for multiple threads to finish initialization.
- Launching a final operation after a set number of tasks complete.
- Ensuring a main thread waits for background tasks before shutdown.

### Notes
- **Non-reusable**: Once the counter reaches zero, the latch cannot be reset.
- For reusable synchronization, use `std::barrier` instead.
- **Thread-Safe**: Designed for concurrent use with atomic operations.

### Possible Implementation
```cpp
#include <atomic>
#include <condition_variable>
#include <mutex>

class latch {
public:
    explicit latch(ptrdiff_t count) : counter_(count) {
        if (count < 0) {
            throw std::invalid_argument("latch count must be non-negative");
        }
    }

    void count_down(ptrdiff_t n = 1) {
        if (n < 0) {
            throw std::invalid_argument("count_down n must be non-negative");
        }
        ptrdiff_t new_count = (counter_.fetch_sub(n, std::memory_order_acq_rel) - n);
        if (new_count <= 0) {
            std::lock_guard<std::mutex> lock(mutex_);
            cv_.notify_all();
        }
    }

    bool try_wait() const noexcept {
        return counter_.load(std::memory_order_acquire) == 0;
    }

    void wait() const {
        if (try_wait()) {
            return;
        }
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return try_wait(); });
    }

    void arrive_and_wait(ptrdiff_t n = 1) {
        count_down(n);
        wait();
    }

private:
    std::atomic<ptrdiff_t> counter_;
    mutable std::mutex mutex_;
    mutable std::condition_variable cv_;
};
```

### Examples with `std::thread`

#### 1. Basic Latch: Waiting for Threads to Complete
**Scenario**: Main thread waits for three worker threads to finish initialization.
```cpp
#include <iostream>
#include <thread>
#include <latch>

void worker(std::latch& l, int id) {
    std::cout << "Worker " << id << " initializing\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work
    std::cout << "Worker " << id << " done\n";
    l.count_down(); // Signal completion
}

int main() {
    std::latch latch(3); // Wait for 3 threads
    std::thread t1(worker, std::ref(latch), 1);
    std::thread t2(worker, std::ref(latch), 2);
    std::thread t3(worker, std::ref(latch), 3);

    std::cout << "Main waiting for workers\n";
    latch.wait(); // Block until count reaches 0
    std::cout << "All workers done\n";

    t1.join();
    t2.join();
    t3.join();
    return 0;
}
```
**Output (order may vary):**
```
Main waiting for workers
Worker 1 initializing
Worker 2 initializing
Worker 3 initializing
Worker 1 done
Worker 2 done
Worker 3 done
All workers done
```
**Explanation**: The latch starts with a count of 3. Each worker decrements it, and main waits until all three signal completion.

#### 2. Latch with Multiple Decrements
**Scenario**: A thread decrements the latch by more than one to represent multiple tasks.
```cpp
#include <iostream>
#include <thread>
#include <latch>

void worker(std::latch& l, int tasks) {
    std::cout << "Worker processing " << tasks << " tasks\n";
    l.count_down(tasks); // Decrement by tasks
}

int main() {
    std::latch latch(10); // Wait for 10 "tasks"
    std::thread t1(worker, std::ref(latch), 4);
    std::thread t2(worker, std::ref(latch), 3);
    std::thread t3(worker, std::ref(latch), 3);

    latch.wait();
    std::cout << "All tasks completed\n";

    t1.join();
    t2.join();
    t3.join();
    return 0;
}
```
**Output (order may vary):**
```
Worker processing 4 tasks
Worker processing 3 tasks
Worker processing 3 tasks
All tasks completed
```
**Explanation**: Each thread decrements the latch by a number of tasks, and main waits for the total count (10) to reach zero.

#### 3. Using `arrive_and_wait`
**Scenario**: A thread signals and waits in one step.
```cpp
#include <iostream>
#include <thread>
#include <latch>

void worker(std::latch& l, int id) {
    std::cout << "Worker " << id << " signaling\n";
    l.arrive_and_wait(); // Decrement and wait
    std::cout << "Worker " << id << " proceeding\n";
}

int main() {
    std::latch latch(2);
    std::thread t1(worker, std::ref(latch), 1);
    std::thread t2(worker, std::ref(latch), 2);

    t1.join();
    t2.join();
    return 0;
}
```
**Output (order may vary):**
```
Worker 1 signaling
Worker 2 signaling
Worker 1 proceeding
Worker 2 proceeding
```
**Explanation**: Each worker decrements the latch and waits for the count to hit zero, ensuring all workers synchronize before proceeding.

## `std::barrier` in C++20

`std::barrier` is a **reusable** synchronization primitive that allows a fixed number of threads to synchronize at a barrier point before proceeding. It’s ideal for iterative algorithms where threads need to synchronize repeatedly.

### Key Features
- **Reusable**: Resets after each phase for multiple rounds of synchronization.
- **Purpose**: Coordinates threads to reach a common point (phase) before continuing.
- **Defined in**: `<barrier>` (C++20 and later).
- **Analogy**: Like runners waiting at a checkpoint until all arrive before continuing the race.

### How It Works
1. Initialize a `std::barrier` with the number of participating threads.
2. Threads call `arrive_and_wait()` to signal arrival and wait for others.
3. Once all threads arrive, an optional completion function runs, and all threads proceed to the next phase.
4. The barrier resets for the next round.

### Syntax
```cpp
#include <barrier>
std::barrier my_barrier(thread_count, [completion_function]);
```

### Key Methods
| Method               | Description                                          |
|----------------------|-----------------------------------------------------|
| `arrive()`          | Marks thread arrival, non-blocking.                 |
| `arrive_and_wait()` | Marks arrival and blocks until all threads arrive.  |
| `arrive_and_drop()` | Marks arrival and removes the thread from the barrier. |
| `wait(arrival_token)` | Waits for the barrier to complete using a token from `arrive()`. |

### Use Cases
- Synchronizing threads in iterative parallel algorithms.
- Coordinating threads in multi-phase workflows.
- Running a completion function (e.g., updating shared state) after each phase.

### Notes
- **Reusable**: Supports multiple phases, unlike `std::latch`.
- **Completion Function**: Optional callback executed when all threads arrive.
- **Thread-Safe**: Built-in atomic operations for safe concurrent use.

### Possible Implementation
```cpp
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <functional>

class barrier {
public:
    explicit barrier(ptrdiff_t num_threads, std::function<void()> completion = nullptr)
        : initial_count_(num_threads), count_(num_threads), generation_(0), completion_(completion) {
        if (num_threads <= 0) {
            throw std::invalid_argument("barrier num_threads must be positive");
        }
    }

    void arrive_and_wait() {
        auto old_gen = generation_.load(std::memory_order_acquire);
        if (count_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            std::lock_guard<std::mutex> lock(mutex_);
            count_.store(initial_count_, std::memory_order_relaxed);
            generation_.fetch_add(1, std::memory_order_release);
            if (completion_) {
                completion_();
            }
            cv_.notify_all();
        } else {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this, old_gen] {
                return generation_.load(std::memory_order_acquire) != old_gen;
            });
        }
    }

    void arrive_and_drop() {
        std::lock_guard<std::mutex> lock(mutex_);
        initial_count_--;
        if (count_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            count_.store(initial_count_, std::memory_order_relaxed);
            generation_.fetch_add(1, std::memory_order_release);
            if (completion_) {
                completion_();
            }
            cv_.notify_all();
        }
    }

private:
    ptrdiff_t initial_count_;
    std::atomic<ptrdiff_t> count_;
    std::atomic<size_t> generation_;
    std::function<void()> completion_;
    std::mutex mutex_;
    std::condition_variable cv_;
};
```

### Examples with `std::thread`

#### 1. Basic Barrier: Synchronizing Threads
**Scenario**: Three threads perform work in phases, synchronizing at each phase.
```cpp
#include <iostream>
#include <thread>
#include <barrier>

void worker(std::barrier<>& b, int id) {
    for (int phase = 1; phase <= 3; ++phase) {
        std::cout << "Worker " << id << " starting phase " << phase << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100 * id)); // Simulate work
        std::cout << "Worker " << id << " reaching barrier in phase " << phase << "\n";
        b.arrive_and_wait(); // Wait for all threads
        std::cout << "Worker " << id << " finished phase " << phase << "\n";
    }
}

int main() {
    std::barrier b(3); // 3 threads must arrive
    std::thread t1(worker, std::ref(b), 1);
    std::thread t2(worker, std::ref(b), 2);
    std::thread t3(worker, std::ref(b), 3);

    t1.join();
    t2.join();
    t3.join();
    return 0;
}
```
**Explanation**: Each thread waits at the barrier until all three arrive, then proceeds to the next phase. The barrier resets automatically.

#### 2. Barrier with Completion Function
**Scenario**: Use a completion function to track phase completions.
```cpp
#include <iostream>
#include <thread>
#include <barrier>

template<typename CompletionFunc>
void worker(std::barrier<CompletionFunc>& b, int id) {
    std::cout << "Worker " << id << " working\n";
    b.arrive_and_wait(); // Synchronize
    std::cout << "Worker " << id << " done\n";
}

int main() {
    int phase_count = 0;
    auto on_completion = [&]() {
        std::cout << "Phase " << ++phase_count << " completed\n";
    };

    std::barrier b(2, on_completion); // 2 threads + completion
    std::thread t1(worker<decltype(on_completion)>, std::ref(b), 1);
    std::thread t2(worker<decltype(on_completion)>, std::ref(b), 2);

    t1.join();
    t2.join();
    return 0;
}
```
**Output (order may vary):**
```
Worker 1 working
Worker 2 working
Phase 1 completed
Worker 1 done
Worker 2 done
```
**Explanation**: The completion function runs when both threads arrive, printing the phase number.

#### 3. Using `arrive` and `wait` Separately
**Scenario**: One thread signals arrival without waiting, and another waits explicitly.
```cpp
#include <iostream>
#include <thread>
#include <barrier>

void worker(std::barrier<>& b, int id) {
    std::cout << "Worker " << id << " arriving\n";
    auto token = b.arrive(); // Signal arrival and get token
    std::cout << "Worker " << id << " waiting\n";
    b.wait(std::move(token)); // Must move the token into wait()
    std::cout << "Worker " << id << " proceeding\n";
}

int main() {
    std::barrier b(2); // 2 threads expected
    std::thread t1(worker, std::ref(b), 1);
    std::thread t2(worker, std::ref(b), 2);
    t1.join();
    t2.join();
    return 0;
}
```
**Output (order may vary):**
```
Worker 1 arriving
Worker 2 arriving
Worker 1 waiting
Worker 2 waiting
Worker 1 proceeding
Worker 2 proceeding
```
**Explanation**: Worker threads arrive and wait using a token, ensuring synchronization at the barrier.

## `std::latch` vs. `std::barrier`
| Feature         | `std::barrier`            | `std::latch`          |
|-----------------|--------------------------|----------------------|
| Reusable        | ✅ Yes (cyclic)          | ❌ No (one-time)     |
| Blocks threads  | ✅ Yes                   | ✅ Yes               |
| Completion Hook | ✅ Yes (optional callback)| ❌ No                |
| Use case        | Iterative synchronization | One-time coordination |

- **std::latch**: Best for one-time coordination (e.g., waiting for all threads to initialize).
- **std::barrier**: Ideal for cyclic synchronization (e.g., iterative parallel algorithms).

## Why Use These Primitives?
- **Thread-Safe**: Both are designed for concurrent use with atomic operations.
- **Simplicity**: Easier than managing mutexes or condition variables for specific coordination tasks.
- **Complements `std::thread`**: Work seamlessly with threads for synchronization.

## Comparison with Other Synchronization Primitives
- **Mutex (`std::mutex`)**: Protects shared data, not for counting events.
- **Condition Variable (`std::condition_variable`)**: Signals events but requires a mutex and more setup.
- **Barrier (`std::barrier`)**: Reusable, unlike `std::latch`, for cyclic synchronization.
- **Atomic (`std::atomic`)**: Lock-free for simple variables, not for counting multiple tasks.
- **Future (`std::future`)**: For async results, not general coordination.

For more details, refer to the C++20 standard or experiment with the provided examples.



##  The code & explanation  for std::latch and std::barrier

```cpp
    ///////////////////
    🧷 std::latch in C++20:
        std::latch is a synchronization primitive introduced in C++20, designed to let one or more threads wait until a counter 
        reaches zero.

        std::latch is a single-use synchronization primitive that allows multiple threads to coordinate by waiting until 
        a specific number of events (or "counts") have occurred. 

        It’s like a gate that opens once a set number of tasks signal completion(all threads comes (counter==0)), 
        allowing waiting threads to proceed.

        std::latch:
        -A single-use synchronization primitive.
        -Allows threads to wait until a specified number of operations (countdowns) are completed.
        -Once the latch's counter reaches zero, all waiting threads are unblocked, and the latch cannot be reused.
        -Useful for scenarios where threads need to wait for a one-time event or set of tasks to complete.

        Imagine this:
            You have 3 kids in a classroom, and the teacher says, “We won’t start the lesson until all 3 are seated.”
            Each kid, once seated, says “I’m ready.” Once all 3 are seated, the door closed and the lesson starts.
            That’s exactly what std::latch does.
            How it works:
                You set a number of threads that need to reach the latch (say 3).
                Each thread, when it finishes its work, calls count_down().
                Another thread can be waiting for the latch to open by calling wait() — it pauses until the count reaches zero.
                ✅ The latch opens only once — after that, it's done and can't be reused.

        
        Key Idea: A latch starts with a counter (set at creation). 
                Threads can decrement the counter (signaling progress), and other threads can wait until the counter reaches zero.
        Defined in: <latch> (C++20 and later).
        Purpose: Coordinate one-time events across threads, such as ensuring all worker threads finish their setup 
            before the main thread continues.


        🧠 Think of It Like:
        A countdown:
            Threads can call count_down() to decrease the counter.
            Other threads can call wait() to block until the counter becomes 0.
            Unlike std::barrier, a std::latch is single-use: once the counter reaches zero, it's done.

        🔧 Syntax
            #include <latch>
            std::latch latch_object(count);
        Common methods:
            | Function            | Purpose                                              |
            | ------------------- | ---------------------------------------------------- |
            | `arrive_and_wait()` | Decrease the counter by 1, then wait(Blocks) until 0 |
            | `count_down(n=1)`   | Decrease the counter by `n`                          |
            | `wait()`            | Wait(Blocks) until counter is 0                      |
            | `try_wait()`        | Non-blocking check if counter is 0                   |

        OR Key Methods:
            Constructor:                std::latch(ptrdiff_t count) sets the initial count.
            count_down(ptrdiff_t n = 1): Decrements the count by n (thread-safe).
            wait():                     Blocks until the count reaches zero.
            try_wait():                 Returns true if the count is zero, false otherwise.
            arrive_and_wait():          Decrements the count by 1 and waits for zero (atomic).

        How Does std::latch Work?
            A std::latch is initialized with a non-negative count. Threads can:
            Decrement the count: Using count_down() or count_down(n) to signal progress.
            Wait for zero: Using wait() to block until the count hits zero.
            Check readiness: Using try_wait() to check if the count is zero without blocking.
            Once the count reaches zero, the latch is "released," and all waiting threads proceed. Unlike other primitives, 
            a latch is single-use—you can’t reset it after it reaches zero.


        figure:
            [Threads T1, T2, T3] --> [latch: count = 3]
            T1: count_down() --> [latch: count = 2]
            T2: count_down() --> [latch: count = 1]
            T3: count_down() --> [latch: count = 0] --> All threads proceed
 
        🚧 Use Cases
            Waiting for multiple threads to finish their setup before continuing.
            Launching a final operation only after N tasks complete.
            Ensuring a main thread waits for background tasks before shutdown.

        
        ⚠️ Notes
            A std::latch is not reusable: once the counter hits 0, it can't be reset.
            If you want cyclic use (repeating barriers), use std::barrier instead.
        
        🚧 Why Use std::latch?
            One-Time Coordination:
                Ideal for scenarios where threads need to complete a set number of tasks before proceeding (e.g., initialization steps).
            Simplicity:
                Easier than managing multiple condition variables or futures for one-off events.
            Thread-Safe:
                Designed for concurrent use, with atomic operations on the counter.
            Single-Use:
                Unlike std::barrier (also C++20), a latch is for one-time use, making it lightweight for specific tasks.
            Complements std::thread:
                Works naturally with threads created via std::thread to synchronize their progress.
        
        ⚠️Possible Implementation:
            #include <atomic>
            #include <condition_variable>
            #include <mutex>

            class latch {
            public:
                explicit latch(ptrdiff_t count) : counter_(count) {
                    if (count < 0) {
                        throw std::invalid_argument("latch count must be non-negative");
                    }
                }

                // Decrements the counter by n. If counter reaches 0, unblocks all waiting threads.
                void count_down(ptrdiff_t n = 1) {
                    if (n < 0) {
                        throw std::invalid_argument("count_down n must be non-negative");
                    }
                    ptrdiff_t new_count = (counter_.fetch_sub(n, std::memory_order_acq_rel) - n);
                    if (new_count <= 0) {
                        std::lock_guard<std::mutex> lock(mutex_);
                        cv_.notify_all();
                    }
                }

                // Returns true if the counter is 0.
                bool try_wait() const noexcept {
                    return counter_.load(std::memory_order_acquire) == 0;
                }

                // Blocks until the counter reaches 0.
                void wait() const {
                    if (try_wait()) {
                        return;
                    }
                    std::unique_lock<std::mutex> lock(mutex_);
                    cv_.wait(lock, [this] { return try_wait(); });
                }

                // Combines count_down and wait.
                void arrive_and_wait(ptrdiff_t n = 1) {
                    count_down(n);
                    wait();
                }

            private:
                std::atomic<ptrdiff_t> counter_;
                mutable std::mutex mutex_;
                mutable std::condition_variable cv_;
            };
        
        ✅ Examples with std::thread
        Let’s see std::latch in action with threads.
        1. Basic Latch: Waiting for Threads to Complete
        Scenario: Main thread waits for three worker threads to finish initialization.
    
            #include <iostream>
            #include <thread>
            #include <latch>

            void worker(std::latch& l, int id) {
                std::cout << "Worker " << id << " initializing\n";
                // Simulate work
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                std::cout << "Worker " << id << " done\n";
                l.count_down(); // Signal completion
            }

            int main() {
                std::latch latch(3); // Wait for 3 threads
                std::thread t1(worker, std::ref(latch), 1);
                std::thread t2(worker, std::ref(latch), 2);
                std::thread t3(worker, std::ref(latch), 3);

                std::cout << "Main waiting forers\n";
                latch.wait(); // Block until count reaches 0
                std::cout << "All workers done\n";

                t1.join();
                t2.join();
                t3.join();
                return 0;
            }
            Output (order may vary):
                Main waiting for workers
                Worker 1 initializing
                Worker 2 initializing
                Worker 3 initializing
                Worker 1 done
                Worker 2 done
                Worker 3 done
                All workers done
            Explanation: The latch starts with a count of 3. Each worker decrements it, and main waits until all three signal completion.

        2. Latch with Multiple Decrements
        Scenario: A thread decrements the latch by more than one to represent multiple tasks.
            #include <iostream>
            #include <thread>
            #include <latch>

            void worker(std::latch& l, int tasks) {
                std::cout << "Worker processing " << tasks << " tasks\n";
                l.count_down(tasks); // Decrement by tasks---- counter =0 
            }

            int main() {
                std::latch latch(10); // Wait for 10 "tasks"
                std::thread t1(worker, std::ref(latch), 4);
                std::thread t2(worker, std::ref(latch), 3);
                std::thread t3(worker, std::ref(latch), 3);

                latch.wait();
                std::cout << "All tasks completed\n";

                t1.join();
                t2.join();
                t3.join();
                return 0;
            }
            Output:
                Worker processing 4 tasks
                Worker processing 3 tasks
                Worker processing 3 tasks
                All tasks completed

            Explanation: Each thread decrements the latch by a number of tasks, and main waits for the total count (10) to reach zero.
        3. Using arrive_and_wait
        Scenario: A thread signals and waits in one step.
            #include <iostream>
            #include <thread>
            #include <latch>

            void worker(std::latch& l, int id) {
                std::cout << "Worker " << id << " signaling\n";
                l.arrive_and_wait(); // Decrement and wait----
                std::cout << "Worker " << id << " proceeding\n";
            }

            int main() {
                std::latch latch(2);
                std::thread t1(worker, std::ref(latch), 1);
                std::thread t2(worker, std::ref(latch), 2);

                t1.join();
                t2.join();
                return 0;
            }
            Output (order may vary):
                Worker 1 signaling
                Worker 2 signaling
                ----counter =0;
                Worker 1 proceeding
                Worker 2 proceeding
            Explanation: Each worker decrements the latch and waits for the count to hit zero, ensuring all workers synchronize before proceeding.

        std::latch vs. Other Synchronization Primitives
        Since we’ve discussed synchronization mechanisms (e.g., mutexes, condition variables):
            Mutex (std::mutex): Protects shared data, not for counting events. Use for resource access.
            Condition Variable (std::condition_variable): Signals events but requires a mutex and more setup. More flexible but complex.
            Barrier (std::barrier, C++20): Reusable, unlike std::latch, for cyclic synchronization.
            Atomic (std::atomic): Lock-free for simple variables, not for counting multiple tasks.
            Future (std::future): For async results, not general coordination.

    ///////////////////
    🧱 std::barrier — C++20:
        std::barrier is a reusable synchronization primitive in C++20 that allows a fixed number of threads to synchronize 
        at a specific point, called a barrier point, before proceeding. 
        It’s like a meeting point where all participating threads must arrive before any can move forward, 
        and it can be reused for multiple rounds (or phases) of synchronization.
        
        std::barrier:
        -A reusable synchronization primitive.
        -Coordinates threads to reach a common point (a "phase") before proceeding.
        -All threads must arrive at the barrier, and once they do, an optional completion function can be executed before all threads are released to the next phase.
        -Ideal for iterative algorithms where threads synchronize repeatedly.
        Imagine this:
            Imagine 4 kids running a race. At every checkpoint, they stop and wait for everyone to arrive, then all of them continue together.
            How it works:
                You set a number of threads that need to arrive.
                Each thread calls arrive_and_wait() when it reaches the barrier.
                Once all have arrived, they all proceed.
                You can also attach a callable obj to run each time the barrier is passed (called the completion function). 
            ✅ The barrier can be used multiple times, unlike a latch.

        🧠 Analogy:
            Imagine a group of runners who must all reach a checkpoint before they’re allowed to continue to the next lap. 
            They wait at the barrier until everyone’s ready.

        Key Idea: Threads wait at the barrier until all expected threads arrive, then all are released, 
            and the barrier resets for the next phase. Optionally, a completion function can run when the barrier is reached.
        Defined in: <barrier> (C++20 and later).
        Purpose: Coordinate threads that need to synchronize repeatedly at specific points, 
            such as in iterative algorithms or cyclic workflows.

        🛠️ Syntax
            #include <barrier>
            std::barrier my_barrier(thread_count);
            Constructor (optional completion function):
                std::barrier(size_t expected, Completion completion);
                expected: number of threads expected to hit the barrier.
                completion: a function that runs once per phase, after all threads arrive, but before they’re released.
        figure:
            Phase 1:
            [Threads T1, T2, T3] --> [barrier: waiting for 3 threads]
            T1: arrive_and_wait() --> [barrier: 2 threads left]
            T2: arrive_and_wait() --> [barrier: 1 thread left]
            T3: arrive_and_wait() --> [barrier: 0 threads left] --> Completion function runs, threads proceed to Phase 2

            Phase 2:
            [Threads T1, T2, T3] --> [barrier: waiting for 3 threads again]
            ...

        🔁Key Methods
            Constructor: std::barrier(ptrdiff_t num_threads, [completion_function])
            num_threads: Number of threads that must arrive.
            completion_function: Optional callable executed when all threads arrive (runs on one thread).
            arrive_and_wait(): Signals arrival and waits for all threads to reach the barrier.
            arrive(): Signals arrival without waiting (useful for non-blocking scenarios).
            wait(arrival_token): Waits for the barrier to complete after an arrive() call (using a token from arrive()).
            OR:
            | Method               | Description                                         |
            | -------------------- | --------------------------------------------------- |
            | `arrive()`           | Marks thread arrival, doesn't block                 |
            | `arrive_and_wait()`  | Marks arrival **and blocks until all arrive**       |
            | `arrive_and_drop()`  | Arrives and **removes** the thread from the barrier |
            | `phase()`            | Returns current phase index                         |
            | `is_arrival_phase()` | Checks if barrier is in arrival phase               |


        How Does std::barrier Work?
            A std::barrier is initialized with a count of participating threads. When a thread reaches the barrier:
                ->It calls arrive_and_wait() or arrive() to signal its arrival.
                ->If it’s not the last thread, it waits until all threads arrive.
                ->Once all threads arrive, the barrier triggers a completion phase (optionally running a user-provided function), 
                    resets the count, and releases all waiting threads.
                ->The barrier is now ready for the next phase.

        Why Use std::barrier?
            Reusable Synchronization:
                Unlike std::latch (single-use), a barrier can be used for multiple rounds, making it ideal for iterative tasks.
            Thread Coordination:
                Ensures all threads reach a synchronization point before proceeding, perfect for parallel algorithms (e.g., parallel loops).
            Completion Function:
                Allows custom logic (e.g., updating shared state) when all threads hit the barrier.
            Thread-Safe:
                Built-in atomic operations for safe concurrent use.
            Complements std::thread:
                Works seamlessly with threads created via std::thread for multi-phase workflows.
        
        ⚠️Possible Implementation:
            #include <atomic>
            #include <condition_variable>
            #include <mutex>
            #include <functional>

            class barrier {
            public:
                explicit barrier(ptrdiff_t num_threads, std::function<void()> completion = nullptr)
                    : initial_count_(num_threads), count_(num_threads), generation_(0), completion_(completion) {
                    if (num_threads <= 0) {
                        throw std::invalid_argument("barrier num_threads must be positive");
                    }
                }

                // Blocks until all threads arrive, then runs completion function and resets.
                void arrive_and_wait() {
                    auto old_gen = generation_.load(std::memory_order_acquire);
                    if (count_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                        // Last thread to arrive
                        std::lock_guard<std::mutex> lock(mutex_);
                        count_.store(initial_count_, std::memory_order_relaxed);
                        generation_.fetch_add(1, std::memory_order_release);
                        if (completion_) {
                            completion_();
                        }
                        cv_.notify_all();
                    } else {
                        std::unique_lock<std::mutex> lock(mutex_);
                        cv_.wait(lock, [this, old_gen] {
                            return generation_.load(std::memory_order_acquire) != old_gen;
                        });
                    }
                }

                // Drops the thread from the barrier, reducing the expected count.
                void arrive_and_drop() {
                    std::lock_guard<std::mutex> lock(mutex_);
                    initial_count_--;
                    if (count_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                        count_.store(initial_count_, std::memory_order_relaxed);
                        generation_.fetch_add(1, std::memory_order_release);
                        if (completion_) {
                            completion_();
                        }
                        cv_.notify_all();
                    }
                }

            private:
                ptrdiff_t initial_count_;
                std::atomic<ptrdiff_t> count_;
                std::atomic<size_t> generation_;
                std::function<void()> completion_;
                std::mutex mutex_;
                std::condition_variable cv_;
            };
        ✅Examples with std::thread
        Let’s see std::barrier in action.
        1. Basic Barrier: Synchronizing Threads
        Scenario: Three threads perform work in phases, synchronizing at each phase.

            #include <iostream>
            #include <thread>
            #include <barrier>

            void worker(std::barrier<>& b, int id) {
                for (int phase = 1; phase <= 3; ++phase) {
                    std::cout << "Worker " << id << " starting phase " << phase << "\n";
                    std::this_thread::sleep_for(std::chrono::milliseconds(100 * id)); // Simulate work
                    std::cout << "Worker " << id << " reaching barrier in phase " << phase << "\n";
                    b.arrive_and_wait(); // Wait for all threads
                    std::cout << "Worker " << id << " finished phase " << phase << "\n";
                }
            }

            int main() {
                std::barrier b(3); // 3 threads must arrive
                std::thread t1(worker, std::ref(b), 1);
                std::thread t2(worker, std::ref(b), 2);
                std::thread t3(worker, std::ref(b), 3);

                t1.join();
                t2.join();
                t3.join();
                return 0;
            }
            Explanation: Each thread waits at the barrier until all three arrive, then proceeds to the next phase. The barrier resets automatically.
        2. Barrier with Completion Function
        Scenario: Use a completion function to track phase completions.

            #include <iostream>
            #include <thread>
            #include <barrier>

            template<typename CompletionFunc>
            void worker(std::barrier<CompletionFunc>& b, int id) {
                std::cout << "Worker " << id << " working\n";
                b.arrive_and_wait(); // Synchronize conter==0
                std::cout << "Worker " << id << " done\n";
            }

            int main() {
                int phase_count = 0;
                auto on_completion = [&]() {
                    std::cout << "Phase " << ++phase_count << " completed\n";
                };

                std::barrier b(2, on_completion); // 2 threads + completion

                std::thread t1(worker<decltype(on_completion)>, std::ref(b), 1);
                std::thread t2(worker<decltype(on_completion)>, std::ref(b), 2);

                t1.join();
                t2.join();
                return 0;
            }

            Output (order may vary):

                Worker 1 working
                Worker 2 working
                Phase 1 completed
                Worker 1 done
                Worker 2 done
            Explanation: The completion function runs when both threads arrive, printing the phase number.

        3. Using arrive and wait Separately
        Scenario: One thread signals arrival without waiting, and another waits explicitly.

            #include <iostream>
            #include <thread>
            #include <barrier>

            // Match the actual barrier type: std::barrier<>
            void worker(std::barrier<>& b, int id) {
                std::cout << "Worker " << id << " arriving\n";
                auto token = b.arrive(); // Signal arrival and get token
                std::cout << "Worker " << id << " waiting\n";
                b.wait(std::move(token)); // Must move the token into wait()
                std::cout << "Worker " << id << " proceeding\n";
            }

            int main() {
                std::barrier b(2); // 2 threads expected
                std::thread t1(worker, std::ref(b), 1);
                std::thread t2(worker, std::ref(b), 2);
                t1.join();
                t2.join();
                return 0;
            }

            Output (order may vary):
                Worker 1 arriving
                Worker 2 arriving
                Worker 1 waiting
                Worker 1 proceeding
            Explanation: Worker 1 arrives and waits; Worker 2 just arrives, triggering the barrier.

    ⚖️ Barrier vs. Latch
        | Feature         | `std::barrier`            | `std::latch`          |
        | --------------- | ------------------------- | --------------------- |
        | Reusable        | ✅ Yes (cyclic)           | ❌ No (one-time)      |
        | Blocks threads  | ✅ Yes                    | ✅ Yes                |
        | Completion Hook | ✅ Yes (optional callback)| ❌ No                 |
        | Use case        | Threads meet repeatedly   | One-time coordination |

        Reusable: std::barrier resets after each phase; std::latch is single-use.
        Completion Function: std::barrier supports a custom completion function; std::latch doesn’t.
        Use Case:
            std::latch: One-time coordination (e.g., wait for all threads to initialize).
            std::barrier: Cyclic synchronization (e.g., iterative parallel algorithms).
        Example:
            std::latch: Wait for 10 tasks once.
            std::barrier: Synchronize 10 threads across multiple iterations.
    ///////////////////

```
