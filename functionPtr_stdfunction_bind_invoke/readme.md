##   The code for function pointer , std::function , std::bind , std::invoke  

```cpp



/*---->C++ Concurrency<----
-->callable object :(function pointer, std::function) as parameters for thread object constructor.
-->How can we create a thread object using a function pointer or std::function to execution specific routine(function)?

std::thread constructor:
    explicit thread( F&& f, Args&&... args );
    f	    -	Callable object to execute in the new thread
    args	-	arguments to pass to the new function
-->a callable object (a function pointer, a function object, a lambda expression, or class member function)

-->introduction to std::bind, std::invoke

Basic Workflow:
    --Create: Construct a std::thread with a function or callable object.
    --Run: The OS starts the thread, and it executes the given code.
        ---Manage: Either join() (wait for it to finish) or detach() (let it run independently).
    --Destroy: The std::thread object is destroyed when it goes out of scope, but only if it’s been joined or detached—otherwise, your program crashes.


-->Create:
    -Creation of Threads: we will talk about create later.
        --Creation of Threads: You can launch a new thread using the std::thread constructor, providing it with 
            a callable object (a function pointer, a function object, a lambda expression, or class member function) and any arguments that should be passed to it
    /////////////////////////
    a function pointer:
        A function pointer is a variable that holds the address of a function. why?
         by using this pointer, It allows you to call a function indirectly, which can be useful for implementing callbacks,
         or by using this pointer, we can invoke it with () just like a regular function , ex: pointer_name(),  
        --you pass a function pointer to tell the thread what to run.
        
        Syntax:for function -> void function_name(){}
            void (*func_ptr_name)(parameter) = function_name; // Points to a function with no args, no return

        Key Characteristics of Function Pointers:
            Indirect Function Calls:
                Function pointers allow you to call a function without directly referencing its name.
            Flexibility:
                You can pass function pointers as arguments to other functions, store them in data structures, 
                or return them from functions.
        Q,How can we create thread obj with function pointer?
            Creating std::thread with Function Pointers:
                1. Basic Function Pointer (No Arguments)
                        #include <iostream>
                        #include <thread>

                        void say_hello() {
                            std::cout << "Hello from thread!\n";
                        }

                        int main() {
                            void (*func_ptr)() = say_hello; // Function pointer
                            std::thread t(func_ptr);        // Thread uses function pointer
                            t.join();
                            std::cout << "Main done\n";
                            return 0;
                        }
                2. Function Pointer with Arguments
                        #include <iostream>
                        #include <thread>

                        void print_number(int x) {
                            std::cout << "Number: " << x << "\n";
                        }

                        int main() {
                            void (*func_ptr)(int) = print_number;
                            std::thread t(func_ptr, 42); // Pass function pointer and argument
                            t.join();
                            return 0;
                        }
                3. Function Pointer with Multiple Arguments
                        #include <iostream>
                        #include <thread>

                        void add_and_print(int a, int b) {
                            std::cout << "Sum: " << (a + b) << "\n";
                        }

                        int main() {
                            void (*func_ptr)(int, int) = add_and_print;
                            std::thread t(func_ptr, 5, 3); // Two arguments
                            t.join();
                            return 0;
                        }            
                4. Function Pointer with Reference Arguments
                        #include <iostream>
                        #include <thread>

                        void increment(int& x) {
                            x += 1;
                        }

                        int main() {
                            int value = 10;
                            void (*func_ptr)(int&) = increment;
                            std::thread t(func_ptr, std::ref(value)); // Use std::ref for reference
                            t.join();
                            std::cout << "Value: " << value << "\n";
                            return 0;
                        }
                5. Function Pointer to a Member Function
                        #include <iostream>
                        #include <thread>

                        class MyClass {
                        public:
                            void say_hi() {
                                std::cout << "Hi from member function!\n";
                            }
                        };

                        int main() {
                            MyClass obj;
                            void (MyClass::*func_ptr)() = &MyClass::say_hi; // Member function pointer
                            std::thread t(func_ptr, &obj); // Pass pointer and object
                            t.join();
                            return 0;
                        }
                6. Function Pointer to a Static Member Function
                        #include <iostream>
                        #include <thread>

                        class MyClass {
                        public:
                            static void say_bye() {
                                std::cout << "Bye from static function!\n";
                            }
                        };

                        int main() {
                            void (*func_ptr)() = MyClass::say_bye; // Static function pointer
                            std::thread t(func_ptr);
                            t.join();
                            return 0;
                        }
                7. Function Pointer with Pointer Arguments
                    #include <iostream>
                    #include <thread>

                    void set_value(int* ptr) {
                        *ptr = 99;
                    }

                    int main() {
                        int value = 0;
                        void (*func_ptr)(int*) = set_value;
                        std::thread t(func_ptr, &value);
                        t.join();
                        std::cout << "Value: " << value << "\n";
                        return 0;
                    }

    /////////////////////////
    std::function:
         is a general-purpose polymorphic function wrapper introduced in C++11. 
         It can store, copy, and invoke any callable object (e.g., functions, lambdas, function objects, or function pointers) that matches a specific signature. 
         It provides a more flexible and type-safe alternative to raw function pointers.
        
        #include <functional>
        Syntax:
            std::function<void()> f; // Wraps a callable returning void, taking no args

        Key Features of std::function
        Polymorphic:
            Can store any callable object (function, lambda, function object, etc.) with a matching signature.
        Type-Safe:
            Ensures that the stored callable object matches the expected signature (return type and parameter types).
        Flexible:
            Can be used to pass callable objects as arguments, store them in containers, or return them from functions.
        Easy to Use:
            Provides a simple and consistent interface for working with callable objects.

        Q,How can we create thread obj with std::function?
            Creating std::thread with std::function:
            1. Wrapping a Regular Function
                    #include <iostream>
                    #include <functional>
                    #include <thread>

                    void say_hello() {
                        std::cout << "Hello from function!\n";
                    }

                    int main() {
                        std::function<void()> f = say_hello; // Wrap function
                        std::thread t(f);                    // Pass to thread
                        t.join();
                        return 0;
                    }
            2. Wrapping a Lambda
                    #include <iostream>
                    #include <functional>
                    #include <thread>

                    int main() {
                        std::function<void()> f = []() {
                            std::cout << "Hello from lambda!\n";
                        };
                        std::thread t(f);
                        t.join();
                        return 0;
                    }
            3. Wrapping a Functor
                    #include <iostream>
                    #include <functional>
                    #include <thread>

                    struct Printer {
                        void operator()() const {
                            std::cout << "Hello from functor!\n";
                        }
                    };

                    int main() {
                        std::function<void()> f = Printer{};
                        std::thread t(f);
                        t.join();
                        return 0;
                    }
            4. With Arguments
                    #include <iostream>
                    #include <functional>
                    #include <thread>

                    void add(int a, int b) {
                        std::cout << "Sum: " << (a + b) << "\n";
                    }

                    int main() {
                        std::function<void(int, int)> f = add;
                        std::thread t(f, 5, 3); // Pass args via thread
                        t.join();
                        return 0;
                    }
            5. Dynamic Assignment
                    #include <iostream>
                    #include <functional>
                    #include <thread>

                    void task1() { std::cout << "Task 1\n"; }
                    void task2() { std::cout << "Task 2\n"; }

                    int main() {
                        std::function<void()> f = task1;
                        f = task2; // Reassign
                        std::thread t(f);
                        t.join();
                        return 0;
                    }
            6. With Return Value
                    #include <iostream>
                    #include <functional>
                    #include <thread>

                    int compute() {
                        return 42;
                    }

                    int main() {
                        std::function<int()> f = compute;
                        std::thread t([f]() { std::cout << "Result: " << f() << "\n"; }); // Capture f
                        t.join();
                        return 0;
                    }
    /////////////////////////
    std::function vs. Function Pointers

        Use Case:	        |        std::function	                 |   Function Pointer
        ------------------------------------------------------------------------------------------------------
        Storing Callables:  |	Can store any callable object.	     |  Can only store functions.
        Flexibility:	    |  Highly flexible and type-safe.	     |   Limited to functions.
        Syntax:	            |  Simple and consistent syntax.	     |  Complex syntax for declaration and use.
        Performance:        | Slight overhead due to type erasure.	 |  Minimal overhead (direct function call).
        Use Case:	        | General-purpose callable storage.	     |  Simple scenarios with functions only.
    

    /////////////////////////
    std::bind:
        is a function template that creates a new callable by "binding" arguments to an existing 
        callable (e.g., a function, functor, or member function). 
        It lets you:
            Fix some arguments to specific values (partial application).
            Rearrange or placeholder arguments for later use.
            Adapt a callable’s signature to match what you need.

        Syntax: std::bind(callable, args...)
        Return Type: A callable object (not std::function, but compatible with it).

        --std::bind takes a callable and a mix of fixed values and placeholders (e.g., std::placeholders::_1, _2, etc.). 
        --It returns a new callable with a modified signature. When you invoke this new callable, 
            it calls the original with the bound arguments and any new ones you provide.

            Key Features
                Fixed Arguments: Hardcode some parameters.
                Placeholders: Leave slots for arguments to be supplied later.
                Flexibility: Works with functions, member functions, functors, etc.
        
        1. Binding a Function with Fixed Arguments
                #include <iostream>
                #include <functional>
                #include <thread>

                void print_sum(int a, int b) {
                    std::cout << "Sum: " << (a + b) << "\n";
                }

                int main() {
                    auto bound = std::bind(print_sum, 5, 3); // Bind both args
                    std::thread t(bound);                    // No args needed now
                    t.join();
                    return 0;
                }
        2. Binding with Placeholders
                #include <iostream>
                #include <functional>
                #include <thread>

                void print_product(int x, int y) {
                    std::cout << "Product: " << (x * y) << "\n";
                }

                int main() {
                    using namespace std::placeholders; // For _1
                    auto bound = std::bind(print_product, 10, _1); // Fix x=10, y is placeholder
                    std::thread t(bound, 4);                       // Pass y=4
                    t.join();
                    return 0;
                }
        3. Binding a Member Function
                #include <iostream>
                #include <functional>
                #include <thread>

                class Calculator {
                public:
                    void add(int x) {
                        std::cout << "Result: " << x << "\n";
                    }
                };

                int main() {
                    Calculator calc;
                    auto bound = std::bind(&Calculator::add, &calc, 42); // Bind object and arg
                    std::thread t(bound);
                    t.join();
                    return 0;
                }
        4. Rearranging Arguments
                #include <iostream>
                #include <functional>
                #include <thread>

                void print_diff(int a, int b) {
                    std::cout << "Diff: " << (a - b) << "\n";
                }

                int main() {
                    using namespace std::placeholders;
                    auto bound = std::bind(print_diff, _2, _1); // Swap args
                    std::thread t(bound, 5, 10);                // 5 becomes b, 10 becomes a
                    t.join();
                    return 0;
                }
        5. Combining with std::function
                #include <iostream>
                #include <functional>
                #include <thread>

                void greet(std::string name) {
                    std::cout << "Hello, " << name << "!\n";
                }

                int main() {
                    std::function<void(std::string)> f = std::bind(greet, std::placeholders::_1);
                    std::thread t(f, "Alice");
                    t.join();
                    return 0;
                }
        6. Binding with References
                #include <iostream>
                #include <functional>
                #include <thread>

                void increment(int& x) {
                    x++;
                }

                int main() {
                    int value = 0;
                    auto bound = std::bind(increment, std::ref(value));
                    std::thread t(bound);
                    t.join();
                    std::cout << "Value: " << value << "\n";
                    return 0;
                }
    /////////////////////////
    std::invoke:
        is a function template that provides a uniform way to invoke any callable object with its arguments. 
        It abstracts the differences between calling a regular function, a member function (with an object), 
        a functor, or even a pointer to a data member or function. 
        It’s like a universal "call this for me" button that handles the details.

        Syntax: std::invoke(callable, args...)
        Return Type: Whatever the callable returns.
        Purpose: Unifies invocation syntax across all callable types.

        How Does It Work?
            std::invoke inspects the callable and applies the correct invocation rules:
                --For regular functions or function pointers: Calls them directly with the arguments.
                --For member functions: Calls them on an object (provided as the first argument).
                --For functors: Calls their operator().
                --For pointers to data members: Accesses the member’s value.

        1. Invoking a Regular Function
                #include <iostream>
                #include <functional>

                void say_hello() {
                    std::cout << "Hello!\n";
                }

                int main() {
                    std::invoke(say_hello); // Direct call
                    return 0;
                }
        2. Invoking a Function with Arguments
                #include <iostream>
                #include <functional>

                void add(int a, int b) {
                    std::cout << "Sum: " << (a + b) << "\n";
                }

                int main() {
                    std::invoke(add, 5, 3); // Invoke with args in thread
                    return 0;
                }
        3. Invoking a Member Function
                #include <iostream>
                #include <functional>

                class Greeter {
                public:
                    void say_hi() {
                        std::cout << "Hi from object!\n";
                    }
                };

                int main() {
                    Greeter g;
                    std::invoke(&Greeter::say_hi, g); // Direct call
                    return 0;
                }
        4. Invoking a Functor
                #include <iostream>
                #include <functional>

                struct Printer {
                    void operator()() const {
                        std::cout << "Functor!\n";
                    }
                };

                int main() {
                    Printer p;
                    //p()
                    std::invoke(p); // Calls operator()
                    return 0;
                }
        5. Invoking with a Lambda
                #include <iostream>
                #include <functional>

                int main() {
                    auto lambda = []() { std::cout << "Lambda!\n"; };
                    std::invoke(lambda);
                    return 0;
                }
        6. Accessing a Data Member
                #include <iostream>
                #include <functional>

                class Data {
                public:
                    int value = 42;
                };

                int main() {
                    Data d;
                    int result = std::invoke(&Data::value, d); // Access member
                    std::cout << "Value: " << result << "\n";
                    // Note: Not typically used with std::thread directly
                    return 0;
                }
        7. With Smart Pointers
                #include <iostream>
                #include <functional>
                #include <memory>

                class Worker {
                public:
                    void task() {
                        std::cout << "Task running!\n";
                    }
                };

                int main() {
                    auto ptr = std::make_shared<Worker>();
                    std::invoke(&Worker::task, ptr); // Smart pointer
                    
                    return 0;
                }
    /////////////////////////


-->Run:
    Thread States:
        New → Runnable → Running → Terminated
            ↑         ↓
            └────── Blocked
        
        -New to Runnable: Thread starts.
        -Runnable to Running: CPU is assigned.
        -Running to Blocked: Waiting for something.
        -Blocked to Runnable: Event occurs.
        -Running to Terminated: Task finishes.
        
    Thread State Analogy in the Restaurant
        Here’s how the thread states map to the restaurant example:
        New:
            Analogy: A new chef has just been hired but hasn’t started working yet.
            Explanation: The thread has been created (e.g., using std::thread), but it hasn’t started executing any tasks yet.

        Runnable:
            Analogy: The chef is ready to work and waiting for a dish (task) to prepare.
            Explanation: The thread is ready to run but is waiting for the CPU scheduler to assign it to a core. It’s in the ready queue.

        Running:
            Analogy: The chef is actively preparing a dish.
            Explanation: The thread is executing its task on a CPU core.

        Blocked:
            Analogy: The chef is waiting for something (e.g., waiting for the oven to preheat or for ingredients to arrive).
            Explanation: The thread is temporarily paused because it’s waiting for a resource (e.g., I/O, a mutex lock, or a condition variable).

        Joinable State:
            Analogy: The chef has finished preparing a dish and is waiting for the head chef to check the dish before moving on to the next task.
            Explanation: The thread has finished its task but hasn’t been joined yet. It’s in a state where it can be joined by another thread (e.g., using join()).

        Joined State:
            Analogy: The head chef has checked the dish, and the chef is now free to take on a new task.
            Explanation: The thread has been joined, and its resources have been cleaned up. The std::thread object is no longer associated with any thread.

        Detached State:
            Analogy: The chef has finished preparing a dish and is no longer under the supervision of the head chef. They are free to leave the kitchen or take on other tasks independently.
            Explanation: The thread has been detached (using detach()), meaning it runs independently, and the std::thread object is no longer associated with it. The thread will clean up its own resources when it finishes.

        Terminated:
            Analogy: The chef has finished their shift and left the kitchen.
            Explanation: The thread has finished execution (either normally or due to an exception) and has been cleaned up.

        --At the transfer from Runnable state to Running state, the threads are considered to be joinable.
        --Joinable: A std::thread object is considered joinable if it represents or once represented an active thread in the system
            or A thread is joinable if it represents an active thread of execution.
                Even after the thread function has finished executing, the std::thread object remains in a joinable state. 
                Default-constructed std::thread objects are not joinable.
                joinable():
                    Checks if the thread can be joined (i.e., it’s running and hasn’t been detached).
                    if (t.joinable()) {t.join();}

    
    -Termination and Resource Management: 
        --Before a joinable std::thread object is destroyed, you must call either the join() or detach() method.

            New ───> Runnable ────Joinable thread────> Running(Joined State , Detached State) ───> Terminated
                ^             | 
                |             V
                └──────── Blocked
        
        ---join()(Joined State): This is a blocking the calling thread (waits) for the thread to complete its execution. 
            It ensures that the resources associated with the thread are properly cleaned up.

        ---detach()(Detached State): This detaches the std::thread object from the underlying operating system thread, 
            allowing the OS thread to continue running independently. The std::thread object then becomes unjoinable. 
                If you detach a thread, you are responsible for ensuring that the thread terminates correctly 
                and does not access resources that are no longer valid (e.g., local variables of the creating function 
                that have gone out of scope). 
                Destroying a joinable std::thread object will result in a call to std::terminate(), which typically aborts the program.

    -Passing Arguments: 
        By default, arguments passed to the thread function are copied into internal storage for the new thread. 
        If you need to pass arguments by reference, you should use std::ref() or std::cref() (for const references) from the <functional> header. 
        Similarly, when using function objects, they are also copied unless wrapped with std::ref().


    -Returning Results and Exception Handling: 
        std::thread itself doesn't directly provide a way to return results. Common methods include passing a pointer 
        or reference to a result variable, or using class data members in a function object (passed by reference). 
        --For handling exceptions thrown in a thread, you can use techniques involving 
            std::exception_ptr to transfer exceptions between threads. 
            However, std::future and std::promise (defined in <future>) offer a more structured way to retrieve results and handle exceptions from threads.

    -Thread Local Storage: 
        The thread_local keyword can be used to declare variables that have thread local storage. 
        Each thread will have its own unique copy of such a variable, with its own lifetime tied to the thread's lifetime.
        This is useful for creating thread-local storage (TLS), where each thread operates on its own copy of the variable without interfering with other threads.

-->Destroy:
    -Cancellation: 
        The standard std::thread in C++ does not provide a built-in mechanism for one thread to forcibly terminate another. 
        A common approach is to use a shared variable (preferably atomic) that the target thread periodically checks to see 
        if it should terminate. 
        C++20 introduced std::jthread, which supports cooperative cancellation using std::stop_token and std::stop_source.

    -std::jthread: 
    Introduced in C++20, std::jthread is a more convenient alternative to std::thread. 
    It automatically calls join() in its destructor, preventing the need to manually join or detach and reducing 
    the risk of std::terminate(). 
    It also supports cooperative cancellation, allowing threads to be signaled to stop their execution gracefully. 
    The callable passed to a std::jthread can optionally accept a std::stop_token as its first parameter to check for cancellation requests. 
    It is generally recommended to prefer std::jthread over std::thread in C++20 and later.


*/


//the application is Kitchen 
#include <iostream>
#include <thread>
#include <functional>


void prepare_meals(int chiefNumber) {
    std::cout << "Chef " << chiefNumber << " is preparing meals..." << std::endl;

    std::cout << "Chef " << chiefNumber << " has finished preparing meals!" << std::endl;
}

//main thread (main/head chief) response for create, run, manage, destroy threads(chiefs)
int main(){
    //new state

    std::thread chef1(prepare_meals  , 1 );
    //Runnable state  ,  chef1 now run on available hardware thread (OS scheuling ), so the chef1 consider to be joinable thread  

    std::cout << "Main thread(main chef) wait.\n";

    chef1.join(); // Wait for chef1 thread to finish
    // chef1.detach(); // dont Wait chef1 for thread to finish 

    std::cout << "Main thread(main chef) done.\n";

    return 0;
    //Terminated state
}

```

##   The code for callbacks

```cpp
//callbacks
#include <iostream>
#include <vector>
#include <functional>


void print(int value){
    std::cout << value << ", ";
}

// void traverse(std::vector<int>& v,void (*ptrCallback)(int )   ) {
void traverse(std::vector<int>& v,std::function<void (int )> ptrCallback  ) {
    for (auto& value : v) {
        // ptrCallback(value); // callback using ptrCallback
        std::invoke(ptrCallback ,value );
    }
    std::cout << std::endl;
}

struct Printer{
    static void myprint(int value){
        std::cout << value << ", ";
    }

    void member_print(int value){
        std::cout << value << ", ";
    }
};

/*
    Syntax: std::bind(callable, args...)
    Return Type: A callable object (not std::function, but compatible with it).
*/

int main() {
    std::vector<int> v{1, 2, 3, 4, 5, 6};
    
    // Using print function as a callback
    // traverse(v,print);
    
    // Using lambda as a callback
    // auto lambdaPrint= [](int value){ std::cout << value << ", ";};
    // traverse(v,lambdaPrint);

    // Using static member function as a callback
    // traverse(v,&Printer::myprint);


    // Using  member function as a callback
    Printer obj;
    auto newCallable = std::bind(&Printer::member_print , &obj ,std::placeholders::_1 );
    //newCallable(int value)
    traverse( v, newCallable);//2 parameters

    return 0;
}




```