##  The code & explanation for Synchronous Functions

```cpp
/*
   🔁 Synchronous Functions:
        ->Synchronous functions are the most common type of function in most programming languages, including C++.
            When you call a synchronous function, the program execution (the calling thread) will wait for the function to complete before moving on to 
            the next line of code.
        ->Synchronous functions execute in a blocking manner, meaning the calling thread waits for the function to complete 
            before moving on. The caller and the function operate in lockstep—execution is sequential, 
            and control returns only when the function finishes.
        Purpose: 
            Ensure a task is fully completed before proceeding, often used for simplicity or when order is critical.
            
        Key Characteristics:
            Blocking: The calling thread is paused until the function returns.
            Sequential: Execution follows a strict order (caller waits for callee).
            Thread Context: Runs in the caller’s thread unless explicitly offloaded.
            Simple error handling: Exceptions propagate directly
        In C++: 
            Most standard functions are synchronous (e.g., a function call like compute_sum(5, 3)). 
            With concurrency, synchronous operations often involve std::thread with join() 
            or explicit synchronization (e.g., mutexes).

        When Used: When you need guaranteed completion or immediate results, or when coordinating shared resources.

        summary:(Synchronous Functions: Run step by step; the program waits for each to finish before moving on.)
        
        ➤ Example (Synchronous):
            #include <iostream>

            void sayHello() {
                std::cout << "Hello from a synchronous function\n";
            }

            int main() {
                sayHello(); // Execution waits here until sayHello() is done
                std::cout << "Finished\n";
                return 0;
            }
            Output:
                Hello from a synchronous function
                Finished
```



##  The code & explanation for Asynchronous Functions

```cpp
/*
    🚀 Asynchronous functions:
        ->Asynchronous functions, on the other hand, allow the calling thread to continue executing without waiting for the function to complete. 
            These functions run in the background, and the main thread doesn't block while waiting for them.
        ->Asynchronous functions execute independently of the calling thread, allowing the caller to continue without 
            waiting for completion. The task typically runs in a separate thread or thread pool, 
            and results (if any) are retrieved later, often via a mechanism like std::future.

        Purpose: 
            Improve responsiveness or parallelism by offloading work to run concurrently.
        Key Characteristics:
            Non-Blocking (Potentially): The caller can proceed immediately, though retrieving results may block.
            Concurrent: Tasks run in parallel or at an unspecified time.
            Thread Management: Often abstracted (e.g., std::async decides whether to use a new thread(launch policy)).
                        std::launch::async: Forces it to run on a new thread.-->Asynchronous
                        std::launch::deferred: Only runs when you call future.get() — not truly asynchronous.
            Complex result handling: Requires futures, callbacks, or polling
        In C++: 
            Achieved primarily with std::async (from <future>) or by detaching std::thread objects, 
            with results managed via std::future or callbacks.
        When Used: For background tasks, parallel computations, or when the caller doesn’t need immediate results.

        summary:(Asynchronous Functions: Can run in the background; the program can continue while waiting for them to complete.)

        ➤ Example using std::async (Asynchronous):

            #include <iostream>
            #include <future>
            #include <chrono>

            void delayedHello() {
                std::this_thread::sleep_for(std::chrono::seconds(2));
                std::cout << "Hello from an async function\n";
            }

            int main() {
                auto future = std::async(std::launch::async, delayedHello); // Runs in background
                std::cout << "Doing other work in main...\n";

                // Wait for async task to finish (optional)
                future.get();

                std::cout << "Finished\n";
                return 0;
            }
            Possible Output:
                Doing other work in main...
                Hello from an async function
                Finished

```




##  The code & explanation for Synchronous Functions vs Asynchronous Functions

```cpp
/*
    
    Key Differences:
   
        Here’s a table comparing **synchronous functions** and **asynchronous functions** in C++:

        | **Aspect**          | **Synchronous Functions**                         | **Asynchronous Functions**                                        |
        |---------------------|---------------------------------------------------|-------------------------------------------------------------------|
        | **Definition**      | Functions that block the caller until completion. | Functions that run independently, allowing the caller to proceed. |
        | **Execution**       | Runs in the caller’s thread, sequentially.        | Runs in a separate thread or pool, concurrently.                  |
        | **Blocking**        | Always blocks the calling thread.                 | Non-blocking until result is needed (e.g., `future.get()`).       |
        | **Result Retrieval**| Immediate (return value).                         | Deferred (via `std::future` or callbacks).                        |
        | **C++ Tools**       | Regular functions, `std::thread` with `join()`.   | `std::async`, `std::thread` with `detach()`, `std::future`.       |
        | **Use Case**        | Tasks requiring immediate results or strict order.| Background tasks, parallel processing.                            |
        | **Example**         | `int result = compute_sum(5, 3);`                 | `auto future = std::async(compute_sum, 5, 3);-->auto sum= future.get()`                    |
        | **Thread Control**  | Explicit (if using threads).                      | Often implicit (e.g., `std::async` manages threads).              |
        | **Complexity**      | Simpler but may delay the caller.                 | More complex due to result handling and threading.                |
*/
```


##  The code & explanation for Handling Exceptions in Synchronous Functions & in Asynchronous Functions

```cpp
/*
    🧱 Handling Exceptions in Synchronous Functions & in Asynchronous Functions
        Handling Exceptions in Synchronous Functions:
            This is straightforward. Just use try-catch as usual:
            int divide(int a, int b) {
                if (b == 0)
                    throw std::runtime_error("Divide by zero!");
                return a / b;
            }

            int main() {
                try {
                    int result = divide(10, 0);
                    std::cout << result << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << "\n";
                }
            }

        🚀 Handling Exceptions in Asynchronous Code
            Asynchronous functions cannot throw exceptions directly back to the main thread. Instead, 
            exceptions are stored and can be retrieved later using:
            future.get() (e.g. from std::async or std::promise)

            🧪 Example with std::async

                #include <iostream>
                #include <future>
                #include <stdexcept>

                int risky_divide(int a, int b) {
                    if (b == 0)
                        throw std::runtime_error("Divide by zero!");
                    return a / b;
                }

                int main() {
                    std::future<int> fut = std::async(std::launch::async, risky_divide, 10, 0);

                    try {
                        int result = fut.get();  // Exception will be re-thrown here
                        std::cout << "Result: " << result << "\n";
                    } catch (const std::exception& e) {
                        std::cerr << "Caught async error: " << e.what() << "\n";
                    }
                }
                ✅ fut.get() throws the exception that happened in the background task.

            🎯 Example with std::promise and Exception Propagation:

                #include <iostream>
                #include <future>
                #include <thread>

                void compute(std::promise<int> prom) {
                    try {
                        throw std::runtime_error("Something went wrong");
                    } catch (...) {
                        prom.set_exception(std::current_exception()); // Send it to future
                    }
                }

                int main() {
                    std::promise<int> prom;
                    std::future<int> fut = prom.get_future();

                    std::thread t(compute, std::move(prom));

                    try {
                        int val = fut.get(); // rethrows the exception
                    } catch (const std::exception& e) {
                        std::cerr << "Promise error: " << e.what() << "\n";
                    }

                    t.join();
                }

    🔁 std::async vs. std::thread
        | Feature            | `std::async`                           | `std::thread`                              |
        | ------------------ | ---------------------------------------| -------------------------------------------|
        | Launches task      | Yes (optionally in new thread)         | Yes (always in a new thread)               |
        | Returns value      | ✅ Yes, via `std::future`              | ❌ No return value (must use other tricks) |
        | Exception handling | ✅ Automatic (via `future.get()`)      | ❌ Manual (use `try/catch` or signals)     |
        | Auto join on exit  | ✅ Yes                                 | ❌ No — you must call `join()`             |
        | Launch policy      | ✅ Can be `async`, `deferred`, or both | ❌ Always async                            |
*/
```

##  The code & explanation for std::async (short for asynchronous)

```cpp
/*
    🔥 std::async (short for asynchronous)
        In C++, async (short for asynchronous) is a function template that starts a task in the background (usually in a new thread) 
        and returns a std::future to retrieve the result later.

        🧠 Purpose of std::async
        Run a function asynchronously (possibly in a separate thread).
        Avoid blocking the main thread.
        Retrieve results later using a future.

        🔧 Syntax:
            std::future<ReturnType> fut = std::async(std::launch::async, function, args...);
            std::launch::async: Forces it to run on a new thread.
            std::launch::deferred: Only runs when you call future.get() — not truly asynchronous.

        You can also omit the launch policy — the compiler decides which one to use.

        ✅ Basic Example
            #include <iostream>
            #include <future>
            #include <chrono>

            int slow_add(int a, int b) {
                std::this_thread::sleep_for(std::chrono::seconds(2));
                return a + b;
            }

            int main() {
                std::future<int> result = std::async(std::launch::async, slow_add, 2, 3);
                
                std::cout << "Doing other work while waiting...\n";
                
                int value = result.get(); // Blocks until result is ready
                std::cout << "Result: " << value << "\n";
            }
            Output:
                Doing other work while waiting...
                Result: 5


        🚦 When to Use async:
            You want to run a task in parallel to other tasks.
            You need to get a return value later (use future.get()).
            You want to write cleaner multithreaded code without manually handling std::thread and std::mutex.

        ⚠️ Notes:
            If future.get() is never called, the function might still run (depends on policy).
            If get() is called and the function threw an exception, the exception is rethrown.
            If you use std::launch::deferred, the function only runs when you call get() or wait() — not truly concurrent.
    ///////////////////

*/

```