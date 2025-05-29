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

### Example: Waiting for Threads to Complete
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

### Use Cases
- Waiting for multiple threads to finish initialization.
- Launching a final operation after a set number of tasks complete.
- Ensuring a main thread waits for background tasks before shutdown.

### Notes
- **Non-reusable**: Once the counter reaches zero, the latch cannot be reset.
- For reusable synchronization, use `std::barrier` instead.

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

### Example: Synchronizing Threads in Phases
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

### Use Cases
- Synchronizing threads in iterative parallel algorithms.
- Coordinating threads in multi-phase workflows.
- Running a completion function (e.g., updating shared state) after each phase.

### Notes
- **Reusable**: Unlike `std::latch`, a barrier supports multiple phases.
- **Completion Function**: Optional callback executed when all threads arrive.

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

For more details, refer to the C++20 standard or experiment with the provided examples.