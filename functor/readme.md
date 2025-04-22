##  The code & explanation  for Function Object    

```cpp
/*
-->Create:
    /////////////////////////
    function object (or functor):
        is an object that can be called like a function. This is achieved by overloading the function call operator, operator(), 
        in a class or struct. 
        Function objects are useful because they combine the ability to be invoked like a function with the ability to maintain state, 
        unlike regular functions.
        Key Points About Function Objects:
            Callable Behavior(Acts like a function): 
                -A function object behaves like a function because it defines operator().
            Stateful(Can maintain state): 
                -Unlike regular functions, function objects can store data (state) as member variables, making them more flexible.
            Use with std::function: 
                -Function objects can be wrapped by std::function since they are callable, allowing them to be stored or passed around uniformly.
            Works with STL algorithms:
                -Commonly used with <algorithm> functions like sort(), transform(), etc.
        Why Use Function Objects?
            Here’s a detailed breakdown of their advantages,supported by examples:
            1. Statefulness(Can remember values between calls (unlike regular functions)):
                Function objects can store state, allowing them to remember information between calls. 
                This is a significant advantage over regular functions or function pointers, 
                which are stateless unless you resort to global variables (a bad practice).
                Example: Tracking Invocation Count:
                    #include <iostream>

                    struct Counter {
                        int count = 0;

                        int operator()(int x) {
                            ++count; // Track how many times the functor is called
                            return x * 2;
                        }
                    };

                    int main() {
                        Counter doubler;
                        std::cout << doubler(5) << "\n"; // Outputs: 10
                        std::cout << doubler(3) << "\n"; // Outputs: 6
                        std::cout << "Called " << doubler.count << " times\n"; // Outputs: Called 2 times
                        return 0;
                    }
            2. Customizable Behavior(Can implement complex logic in operator()):
                Function objects allow you to define complex, reusable logic in operator(). 
                You can parameterize their behavior through constructors or member variables.

                Example: Configurable Comparison for Sorting:
                    #include <algorithm>
                    #include <vector>
                    #include <iostream>

                    struct CaseInsensitiveCompare {
                        bool operator()(const std::string& a, const std::string& b) const {
                            // Compare strings case-insensitively
                            for (size_t i = 0; i < a.size() && i < b.size(); ++i) {
                                if (std::tolower(a[i]) != std::tolower(b[i])) {
                                    return std::tolower(a[i]) < std::tolower(b[i]);
                                }
                            }
                            return a.size() < b.size();
                        }
                    };

                    int main() {
                        std::vector<std::string> words = {"Apple", "banana", "ZEBRA", "apple"};
                        std::sort(words.begin(), words.end(), CaseInsensitiveCompare());

                        for (const auto& word : words) {
                            std::cout << word << "\n";
                        }
                        // Outputs (in order): apple, Apple, banana, ZEBRA
                        return 0;
                    }
            3. Efficiency Through Inlining(Often inlined by the compiler, making them as fast as function calls):
                Function objects are often more efficient than function pointers because the compiler can inline their operator() calls. 
                Function pointers, by contrast, involve indirect calls, which can prevent inlining and introduce overhead.
            4. Integration with std::function:
                Function objects can be stored in std::function, which provides a uniform way to handle different 
                callable types (lambdas, function pointers, functors). This makes them ideal for callbacks, event handlers, or generic programming.

                Example: Event Handler with std::function:

                    #include <functional>
                    #include <iostream>

                    struct Logger {
                        std::string prefix;
                        Logger(const std::string& p) : prefix(p) {}
                        void operator()(const std::string& message) const {
                            std::cout << prefix << ": " << message << "\n";
                        }
                    };

                    void dispatch_event(const std::function<void(const std::string&)>& handler, const std::string& msg) {
                        handler(msg);
                    }

                    int main() {
                        Logger warn("WARNING");
                        Logger error("ERROR");

                        dispatch_event(warn, "Disk full");  // Outputs: WARNING: Disk full
                        dispatch_event(error, "Access denied"); // Outputs: ERROR: Access denied

                        // Using a lambda with std::function
                        dispatch_event([](const std::string& msg) { std::cout << "INFO: " << msg << "\n"; }, "Login successful");
                        // Outputs: INFO: Login successful

                        return 0;
                    }
        The C++ Standard Library provides a set of predefined function objects in the <functional> header. 
        These are designed to simplify common operations, especially when used with STL algorithms:
            Arithmetic Operations:
                These functors perform basic arithmetic on their arguments.

                std::plus<T>: Computes a + b.
                std::minus<T>: Computes a - b.
                std::multiplies<T>: Computes a * b.
                std::divides<T>: Computes a / b.
                std::modulus<T>: Computes a % b (for integral types).
                std::negate<T>: Computes -a (unary).
                Example: Using std::plus:

                    #include <functional>
                    #include <iostream>
                    #include <algorithm>
                    #include <vector>

                    int main() {
                        std::vector<int> a = {1, 2, 3};
                        std::vector<int> b = {4, 5, 6};
                        std::vector<int> result(3);

                        std::transform(a.begin(), a.end(), b.begin(), result.begin(), std::plus<int>());

                        for (int x : result) {
                            std::cout << x << " "; // Outputs: 5 7 9
                        }
                        std::cout << "\n";
                        return 0;
                    }
            Comparison Operations:
                These functors compare two values.

                std::equal_to<T>: Checks if a == b.
                std::not_equal_to<T>: Checks if a != b.
                std::greater<T>: Checks if a > b.
                std::less<T>: Checks if a < b.
                std::greater_equal<T>: Checks if a >= b.
                std::less_equal<T>: Checks if a <= b.
                Example: Sorting with std::greater
                    #include <functional>
                    #include <vector>
                    #include <algorithm>
                    #include <iostream>

                    int main() {
                        std::vector<int> numbers = {3, 1, 4, 1, 5};
                        std::sort(numbers.begin(), numbers.end(), std::greater<int>());

                        for (int x : numbers) {
                            std::cout << x << " "; // Outputs: 5 4 3 1 1
                        }
                        std::cout << "\n";
                        return 0;
                    }
            Logical Operations:
                These functors perform logical operations.

                std::logical_and<T>: Computes a && b.
                std::logical_or<T>: Computes a || b.
                std::logical_not<T>: Computes !a (unary).
                Example: Using std::logical_and:
                    #include <functional>
                    #include <vector>
                    #include <algorithm>
                    #include <iostream>

                    int main() {
                        std::vector<bool> a = {true, false, true};
                        std::vector<bool> b = {true, true, false};
                        std::vector<bool> result(3);

                        std::transform(a.begin(), a.end(), b.begin(), result.begin(), std::logical_and<bool>());

                        for (bool x : result) {
                            std::cout << x << " "; // Outputs: 1 0 0
                        }
                        std::cout << "\n";
                        return 0;
                    }
            Bitwise Operations:
                These functors perform bitwise operations (C++11 and later).

                std::bit_and<T>: Computes a & b.
                std::bit_or<T>: Computes a | b.
                std::bit_xor<T>: Computes a ^ b.
                std::bit_not<T>: Computes ~a (unary, C++14+).
                Example: Using std::bit_xor:

                    #include <functional>
                    #include <vector>
                    #include <algorithm>
                    #include <iostream>

                    int main() {
                        std::vector<int> a = {0x1, 0x2, 0x3};
                        std::vector<int> b = {0x2, 0x3, 0x1};
                        std::vector<int> result(3);

                        std::transform(a.begin(), a.end(), b.begin(), result.begin(), std::bit_xor<int>());

                        for (int x : result) {
                            std::cout << x << " "; // Outputs: 3 1 2
                        }
                        std::cout << "\n";
                        return 0;
                    }
        Q,How can we create thread obj with Function Objects?
            1. Basic Function Object (No State)
            Description: A simple functor that performs an action.
            Example:
                #include <iostream>
                #include <thread>

                struct SimplePrinter {
                    void operator()() const {
                        std::cout << "Thread says hi!\n";
                    }
                };

                int main() {
                    SimplePrinter p;
                    std::thread t(p); // Functor copied to thread
                    t.join();
                    return 0;
                }

            2. Function Object with State
            Description: The functor uses internal data to customize its behavior.
            Example:
                #include <iostream>
                #include <thread>

                struct Counter {
                    int id;
                    Counter(int i) : id(i) {}
                    void operator()() const {
                        std::cout << "Thread " << id << " running\n";
                    }
                };

                int main() {
                    Counter c1(1), c2(2);
                    std::thread t1(c1); // Thread with ID 1
                    std::thread t2(c2); // Thread with ID 2
                    t1.join();
                    t2.join();
                    return 0;
                }

            3. Function Object with Arguments
            Description: The functor’s operator() accepts parameters.
            Example:
                #include <iostream>
                #include <thread>

                struct Adder {
                    void operator()(int x, int y) const {
                        std::cout << "Sum: " << (x + y) << "\n";
                    }
                };

                int main() {
                    Adder add;
                    std::thread t(add, 10, 20); // Pass arguments
                    t.join();
                    return 0;
                }

        Function Objects vs. Other Callables
            Regular Function: Just code, no state (void func()).
            Function Pointer: Points to a function, no state unless passed (void (*ptr)()).
            Lambda: Inline, can capture state, but less reusable ([](){}).
            Functor: Object with state and operator(), reusable and extensible.
            std::function: Wraps any callable, including functors, for type erasure.
                std::bind: Adapts functors by pre-setting args (e.g., std::bind(&Functor::operator(), obj)).
                std::invoke: Calls functors directly (e.g., std::invoke(functor)).

    ///////////////////////////

*/


```


##  The code for struct Counter Functor and std::sort with STL Arithmetic Functors Example

```cpp
#include <iostream>
#include <thread>
#include <functional>
#include <vector>
#include <algorithm>

struct Counter{
    int mCount{};
    //overloading the function call operator, operator()
    void operator()(int value){
        mCount++;
        std::cout<<"value:"<<value<<std::endl;
    }
};

int main(){
    // Counter obj;
    // std::function<void(int)> wObj = std::bind(&Counter::operator() , &obj , std::placeholders::_1);
    // obj(10);
    // obj(11);
    // obj(12);
    // wObj(13);
    // std::cout<<"count:"<<obj.mCount<<std::endl;
    // std::plus pls;
    // std::cout<<pls(1,3)<<std::endl;

    std::vector<int> v{9,3,0,2,5,1,2,6,8,4,25};
    
    std::sort(std::begin(v) , std::end(v), std::greater<>{});
    for(auto & value : v){
        std::cout<<value<<", ";
    }
    std::cout<<"\n";

    return 0;
}

```

##  The code for Thread with Functor: With State

```cpp
#include <iostream>
#include <thread>

struct SimplePrinter {
    int mValue{1};
    void operator()()  {
        std::cout << "Thread says hi!\n";
        std::cout << "invoke number:"<<mValue<<std::endl;
        mValue++;
    }
};

int main() {
    SimplePrinter p;
    std::thread t(std::ref(p)); // Functor copied to thread --> invoke --> p()
    std::thread c(std::ref(p)); // Functor copied to thread --> invoke --> p()
    t.join();
    c.join();
    return 0;
}

```


##  The code for Thread with Functor: Reference Arguments


```cpp

#include <iostream>
#include <thread>

struct SimplePrinter {
    void operator()(int& rvalue)  {
        std::cout << "Thread says hi!\n";
        rvalue =200;
    }
};

int main() {
    int value=0;
    SimplePrinter p;
    std::thread t(p , std::ref(value)); // Functor copied to thread --> invoke --> p(15)
    t.join();
    std::cout << "value:"<<value<<std::endl;
    return 0;
}
```