##  The code & explanation  for std::future and std::promise

```cpp
/*
    ///////////////////
    🔗 std::future and std::promise
        In C++, std::future and std::promise are tools from the <future> header 
        that enable safe communication between threads, especially for asynchronous execution.

        🔗 std::future and std::promise Relationship
        std::promise is used by the producer thread to set a value.
        std::future is used by the consumer thread to get that value, once it’s ready.
        They are tightly coupled: a future is obtained from a promise via get_future().

        ✅ Basic Example

            #include <iostream>
            #include <future>
            #include <thread>

            void compute(std::promise<int> result_promise) {
                int result = 42;
                result_promise.set_value(result); // Set the result for the future
            }

            int main() {
                std::promise<int> prom;               // Create promise
                std::future<int> fut = prom.get_future(); // Get future

                std::thread t(compute, std::move(prom));  // Launch thread

                std::cout << "Waiting for result...\n";
                std::cout << "Result: " << fut.get() << "\n"; // Wait and get value

                t.join();
            }
            🧾 Output:
                Waiting for result...
                Result: 42
        🧠 Summary of Key Concepts
            | Concept               | `std::promise`                     | `std::future`                    |
            | --------------------- | ---------------------------------- | -------------------------------- |
            | Role                  | Producer (sets value)              | Consumer (gets value)            |
            | Member usage          | `.set_value()`, `.set_exception()` | `.get()`, `.wait()`, `.valid()`  |
            | Thread-safety         | Not thread-safe                    | Only one thread can `get()`      |
            | Exception Propagation | Yes, via `.set_exception()`        | Exception is rethrown by `get()` |


        ❗ What If an Exception Occurs?
        You can send exceptions through a promise:

            try {
                throw std::runtime_error("Oops!");
            } catch (...) {
                prom.set_exception(std::current_exception());
            }
            Later:

            try {
                fut.get(); // This will rethrow the exception
            } catch (const std::exception& e) {
                std::cout << "Caught exception: " << e.what() << "\n";
            }
            🆚 std::async vs. promise/future
                std::async automatically returns a future, and you don’t have to manage the promise.
                Use promise/future when:
                    Threads need to manually send results or exceptions.
                    You need more control over task execution and communication.

    ///////////////////
*/

```


##  The code for std::future and std::promise

```cpp

#include <iostream>
#include <future>
#include <thread>


void compute(std::promise<int> result_promise) {
    int result = 42;
    result_promise.set_value(result); // Set the result for the future
}


int main() {

    std::promise<int> prom;  //producer             // Create promise
    std::future<int> fut;   //consumer
    fut = prom.get_future(); //Get future

    std::thread t(compute, std::move(prom));  // Launch thread

    std::cout << "Waiting for result...\n";
    std::cout << "Result: " << fut.get() << "\n"; // Wait and get value

    t.join();
}


```


##  The code  for std::future and std::promise

```cpp


#include <iostream>
#include <future>
#include <thread>

int compute() {
    int result = 42;
    return result;
}

int main() {

    
    std::future<int> fut = std::async(std::launch::async, compute);  

    std::cout << "Waiting for result...\n";
    std::cout << "Result: " << fut.get() << "\n"; // Wait and get value

}

```


##  The code std::future and std::promise  to Handling Exceptions with std::thread

```cpp
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
    std::promise<int> prom;//producer-->first terminal  com 
    std::future<int> fut = prom.get_future();//consumer -> other terminal com

    std::thread t(compute, std::move(prom));

    try {
        int val = fut.get(); // rethrows the exception
    } catch (const std::exception& e) {
        std::cerr << "Promise error: " << e.what() << "\n";
    }

    t.join();
}

```



##  The code Handling Exceptions in Asynchronous Functions with std::async
```cpp

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



```