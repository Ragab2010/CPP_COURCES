##   The code for thread_and_threadStates  

```cpp



/*---->C++ Concurrency<----
--->std::thread , thread states , callable object.

-->quick review:Concurrency, Parallelism, Process, Thread.
Concurrency: is about dealing with multiple tasks at the same time , by executing them simultaneously.
            --The tasks overlap in time, but only one task is actively being worked on at any given moment.
    Analogy: A restaurant has a (single chef) (one CPU core) preparing multiple dishes simultaneously, 
        This happens by using a mechanism that switches between chopping vegetables for one dish, stirring a pot for another, 
        and checking the oven for a third. The dishes overlap in time, but only one is actively being worked on at any given moment.

Parallelism: is about executing multiple tasks simultaneously, by leveraging multiple processors or cores.
            --all The tasks happening at the exact same time.
    Analogy: A restaurant has (multiple chefs) (multiple CPU core) preparing multiple dishes at the exact same time,
        multiple chefs (multiple CPU cores) working on different dishes at once,
        One chef chops vegetables, another stirs the pot, and a third bakes, This happens by using a Concurrency mechanism,
        all dishes happening at the exact same time.

Deterministic Restaurant (Deterministic System) VS Responsive Restaurant (Responsive System) :
    Responsive Restaurant (Responsive System)
    A responsive restaurant is one that prioritizes quick feedback and adaptability to customer needs, 
    even if it means sacrificing strict order or predictability. This is similar to a responsive system in computing, 
    where the goal is to provide quick responses and handle multiple tasks concurrently, 
    even if the exact order of operations isn’t guaranteed.

    Characteristics of a Responsive Restaurant:
        --Quick Feedback:
                Customers receive quick responses, even if their orders aren’t completed immediately.
                For example, the waiter might acknowledge the order right away and provide updates while the chef prepares the dish.

        --Concurrency:
            Multiple chefs work on different dishes simultaneously, and the kitchen handles multiple orders at once.
            This is like a system handling multiple tasks concurrently (e.g., a web server handling multiple requests).

        --Adaptability:
            The restaurant can adjust to changing demands (e.g., prioritizing urgent orders or accommodating special requests).
            This is similar to a system dynamically adjusting its behavior based on workload or user input.

        --Non-Deterministic:
            The order in which dishes are prepared might vary depending on the situation (e.g., some dishes take longer to cook, or some chefs are faster than others).
            This is like a non-deterministic system where the order of task execution isn’t guaranteed.

        Example:
            A busy restaurant receives 10 orders at the same time.
            The kitchen prioritizes quick dishes (e.g., salads) to provide immediate feedback to customers.
            Longer dishes (e.g., steaks) are prepared concurrently but might take more time.
            The order in which dishes are served might vary depending on the chefs’ availability and the complexity of the dishes.
        code:
            #include <iostream>
            #include <thread>
            #include <vector>
            #include <chrono>

            // Function to simulate preparing a dish
            void prepare_dish(const std::string& dish_name) {
                std::cout << "Preparing " << dish_name << "...\n";
                std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulate time to prepare
                std::cout << dish_name << " is ready!\n";
            }

            int main() {
                std::vector<std::string> orders = {"Pizza", "Pasta", "Salad", "Steak"};

                std::cout << "Responsive Restaurant: Handling orders concurrently...\n";

                // Create a thread for each dish (concurrent execution)
                std::vector<std::thread> chefs;
                for (const auto& dish : orders) {
                    chefs.emplace_back(prepare_dish, dish);
                }

                // Wait for all chefs to finish
                for (auto& chef : chefs) {
                    chef.join();
                }

                std::cout << "All orders are ready!\n";
                return 0;
            }


    Deterministic Restaurant (Deterministic System)
    A deterministic restaurant is one that follows a strict, predictable process for preparing and serving dishes. 
    This is similar to a deterministic system in computing, where the system’s behavior is predictable and repeatable, 
    given the same inputs.

    Characteristics of a Deterministic Restaurant:
        --Strict Order:
            Dishes are prepared and served in a specific, predictable order (e.g., first-come, first-served).
            This is like a system where tasks are executed in a fixed order.

        --Predictability:
            The time it takes to prepare each dish is consistent and predictable.
            This is like a deterministic system where the execution time of tasks is known in advance.

        --No Concurrency:
            Only one dish is prepared at a time, and the kitchen handles orders sequentially.
            This is like a single-threaded system where tasks are executed one after another.

        Deterministic:
            Given the same set of orders, the restaurant will always produce the same result (e.g., dishes served in the same order and at the same time).
            This is like a deterministic system where the same input always produces the same output.

        Example:
            A small restaurant receives 10 orders at the same time.
            The kitchen prepares and serves each dish one at a time, in the order they were received.
            Each dish takes exactly 10 minutes to prepare, and customers are served in a predictable sequence.
            The result is always the same: the first order is served after 10 minutes, the second after 20 minutes, and so on.
        
        code:
            #include <iostream>
            #include <chrono>
            #include <queue>

            // Function to simulate preparing a dish
            void prepare_dish(const std::string& dish_name) {
                std::cout << "Preparing " << dish_name << "...\n";
                std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulate time to prepare
                std::cout << dish_name << " is ready!\n";
            }

            int main() {
                std::queue<std::string> orders;
                orders.push("Pizza");
                orders.push("Pasta");
                orders.push("Salad");
                orders.push("Steak");

                std::cout << "Deterministic Restaurant: Handling orders one at a time...\n";

                // Polling loop: Process orders one at a time
                while (!orders.empty()) {
                    std::string dish = orders.front();
                    orders.pop();
                    prepare_dish(dish); // Prepare the dish sequentially
                }

                std::cout << "All orders are ready!\n";
                return 0;
            }


Process:A process is an instance of a running program. It has its own memory space, resources, and state.

    Analogy: Processes:
        --A process is like a separate kitchen with its own resources (e.g., stove, oven, utensils) and a dedicated chef.
        --Each kitchen operates independently and prepares a single dish from start to finish.
        --Processes do not share resources directly, they are isolated from each other.

Thread:A thread is a lightweight unit of execution within a process. Multiple threads can exist within a single process and share the same memory space.

    Analogy: Threads:
        --A thread is like a single chef in a kitchen who can work on multiple tasks (e.g., chopping vegetables, boiling pasta, grilling meat) concurrently.
        --All threads share the same resources (e.g., stove, oven, utensils) within the same kitchen.
        --Threads are lightweight and can switch between tasks quickly.


how can how  A “hardware thread”  the number of independent execution contexts a CPU 
    that can handle at once. This is tied to the CPU’s architectur?
    --std::cout << "Hardware concurrency: " << std::thread::hardware_concurrency() << "\n";
    --run at bash : nproc


std::thread:
    --std::thread is a class in the C++ Standard Library<thread> that represents a single thread of execution.
    --that enables multithreading/Concurrency, allowing you to execute code concurrently,
            It provides a way to create and manage individual threads of execution within a program.
    --Purpose: To execute a function or callable object (a function pointer, a function object, a lambda expression, or a class member function) in a separate thread.
    --Key Idea: Each std::thread object corresponds to an OS thread, and the OS schedules it to run on available hardware threads.
    --The std::thread constructor is a variadic template, meaning it accepting any number of arguments after the callable, which are then passed to the invoked function.

Why We Use std::thread?
    -Do you remember the restaurant example? We need a mechanism or design patterns (chief) to divide the code(meals), 
        which then allows you to run multiple tasks (meals) simultaneously , so std::thread enable the Concurrency.

    --To Achieve Concurrency: Concurrency is about handling multiple tasks at once, even if they don’t run simultaneously. 
                        std::thread lets you split your program into independent tasks that can run alongside each other.
        ---improve responsiveness and resource utilization.

    --To Leverage Parallelism: On multi-core CPUs, std::thread lets you run tasks simultaneously, 
                                speeding up execution by distributing work across cores.
        ---boosts performance for compute-heavy tasks.

    --Standardization and Portability: Before C++11, threading depended on platform-specific APIs (e.g., POSIX threads on Linux, Win32 threads on Windows). 
                                         std::thread abstracts this, working across
        ---it runs everywhere (Windows, Linux, macOS) without tweaking for each OS.

    --Fine-Grained Control
        ---gives you direct control over thread creation, execution, and synchronization

Basic Workflow:
    --Create: Construct a std::thread with a function or callable object.
    --Run: The OS starts the thread, and it executes the given code.
    --Manage: Either join() (wait for it to finish) or detach() (let it run independently).
    --Destroy: The std::thread object is destroyed when it goes out of scope, but only if it’s been joined or detached—otherwise, your program crashes.

-->Create:
    -Creation of Threads: we will talk about create later.
        --Creation of Threads: You can launch a new thread using the std::thread constructor, providing it with 
            a callable object (a function pointer, a function object, a lambda expression, or a member function) and any arguments that should be passed to it

-->Run:
    -Thread States:
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

Performance Considerations: 
    Creating a large number of threads can be expensive in terms of system resources and might not always 
    lead to performance improvements due to context switching overhead. Consider using higher-level abstractions like 
    tasks (std::async) or thread pools for managing concurrency.

    Thread Pool Analogy
    A thread pool is like having a team of chefs in a single kitchen, where:
        --The kitchen represents the program.
        --The chefs represent the worker threads in the thread pool.
        --The dishes represent the tasks that need to be executed.

    How It Works:
    Fixed Team of Chefs:
        The kitchen has a fixed number of chefs (e.g., 4 chefs). These chefs are always available and ready to work.
        This is like a thread pool with a fixed number of worker threads.

    Task Queue:
        --When new dishes (tasks) come in, they are added to a queue (e.g., a whiteboard or order list).
        --The chefs pick up dishes from the queue and start preparing them.

    Efficient Resource Usage:
        --Instead of hiring a new chef for every dish (which would be expensive and chaotic), the existing chefs handle multiple dishes one after another.
        --This is like reusing threads in a thread pool instead of creating and destroying threads for every task.

    Concurrency and Parallelism:
        --If there are multiple chefs, they can work on different dishes in parallel (e.g., one chef chops vegetables, another stirs a pot).
        --If there is only one chef, they switch between tasks concurrently (e.g., chopping vegetables for one dish, then stirring a pot for another).

    Example:
        A restaurant receives 10 orders (tasks).
        The kitchen has 4 chefs (worker threads).
        The chefs pick up orders from the queue and prepare them concurrently and in parallel.
        Once a chef finishes a dish, they pick up the next one from the queue.

    std::async Analogy
        --std::async is like outsourcing dishes to external chefs or specialized kitchens, where:
            The main kitchen represents the main thread.
        --The external chefs or specialized kitchens represent the asynchronous tasks launched by std::async.

        How It Works:
        Outsourcing Dishes:
            --When the main kitchen gets too busy, it outsources some dishes to external chefs or specialized kitchens.
            --This is like using std::async to offload tasks to other threads or resources.

        Flexible Execution:
        --The external chefs can either:
        ---Work asynchronously (e.g., a new chef is hired to prepare the dish immediately).
        ---Work deferred (e.g., the dish is prepared only when the main kitchen requests it).

        Result Retrieval:
        --The main kitchen can continue working on other tasks while waiting for the outsourced dishes to be completed.
        --Once the dish is ready, the main kitchen retrieves it (using std::future).

        Example:
            The main kitchen receives an order for a complex dish (e.g., a wedding cake).
            Instead of preparing it themselves, they outsource it to a specialized bakery (std::async).
            The main kitchen continues preparing other dishes while waiting for the wedding cake to be delivered.
            When the cake is ready, the main kitchen retrieves it and serves it to the customer.


*/

//the application is Kitchen 
#include <iostream>
#include <thread>

int num = 10;
void chief( int chiefNumber , bool & state){
   std::cout<<"hello from thread chief: "<<chiefNumber<<std::endl;
   state = true;
}


//main thread (main/head chief) response for create, run, manage, destroy threads(chiefs)
int main(){
    //new state
    bool state = false;
    std::thread chief1(chief , 1 , std::ref(state));
    //Runnable state  ,  chief1 now run on available hardware thread (OS scheuling ), so the chief1 consider to be joinable thread
    /*
    |
    |
    v
    Running (Joined State , Detached State)
    The main thread continues to execute all the statements until it goes out of scope,
    -->the result in a call to std::terminate(), which typically aborts the program. 
    We have a problem:
    We have to make the chief1 Joined State  or  Detached State.
    -->Joined State makes the main thread wait for the chief1 thread to complete its execution.
    -->Detached State makes the main thread detach the chief1 std::thread from the underlying operating system thread, allowing the OS thread to continue running it independently.
    */   
    std::cout << "Main thread(main chief)  wait.\n";
    chief1.join(); // Wait for chief1 thread to finish
    // chief1.detach(); // dont Wait for thread to finish 
    std::cout << "Main thread(main chief) done.\n";

    std::cout<<std::boolalpha<<"the state is :"<<state<<std::endl;

    return 0;
    //Terminated state
}




```



##   The code for std::jthread  

```cpp

//the application is Kitchen 
#include <iostream>
#include <thread>


void chief( int chiefNumber , bool & state){
   std::cout<<"hello from thread chief: "<<chiefNumber<<std::endl;
   state = true;
}


//main thread (main/head chief) response for create, run, manage, destroy threads(chiefs)
int main(){
    //new state
    std::jthread chief1(chief , 1 );
    //Runnable state  ,  chief1 now run on available hardware thread (OS scheuling ), so the chief1 consider to be joinable thread
    /*
    |
    |
    v
    Running (Joined State , Detached State)
    The main thread continues to execute all the statements until it goes out of scope,
    -->the result in a call to std::terminate(), which typically aborts the program. 
    We have a problem:
    We have to make the chief1 Joined State  or  Detached State.
    -->Joined State makes the main thread wait for the chief1 thread to complete its execution.
    -->Detached State makes the main thread detach the chief1 std::thread from the underlying operating system thread, allowing the OS thread to continue running it independently.
    */   
    std::cout << "Main thread(main chief)  wait.\n";
    // chief1.join(); // Wait for chief1 thread to finish
    // chief1.detach(); // dont Wait for thread to finish 
    std::cout << "Main thread(main chief) done.\n";

    return 0;
    //Terminated state
}//chief1.destructor call chief1.join(); // Wait for chief1 thread to finish


```

