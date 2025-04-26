##  The code & explanation  forLambda    

```cpp
/*
    Lambdas (Lambda Expressions):
        is syntactic sugar for unnamed function objects (functors). These are generated at compile time.
        is a concise way to define an anonymous function object inline, introduced in C++11. 
        Lambdas allow you to write callable code without explicitly defining a named function or a function object class. 
        They’re essentially syntactic sugar for creating function objects, but with a more compact and flexible syntax.
        Key Characteristics of Lambdas:
            Anonymous - Don't need a name
            Inline definition - Can be defined where they're used
            Capture capability - Can access variables from enclosing scope
            Concise syntax - More compact than full class definitions
        The general syntax of a lambda expression is:
            
            [capture_block]<template_params>(parameters) mutable noexcept -> return_type { body }
                equal to function object:
                    template<typename CaptureType>
                    class Lambda {
                    public:
                        // Constructor to initialize captured variable
                        Lambda(CaptureType capture) : capture_(capture) {}

                        // operator() with full lambda-like behavior
                        template<typename... Args>
                        inline auto operator()(Args... args) mutable noexcept -> ReturnType {
                            // body equivalent to lambda goes here
                        }

                    private:
                        CaptureType capture_;  // simulate capture block
                    };
                    example:
                        int a = 10;
                        auto lambda = [a](int x) mutable noexcept -> int {
                            return x + a;
                        };
                        Equivalent class:
                        class Lambda {
                        public:
                            Lambda(int a) : a_(a) {}

                            inline int operator()(int x) mutable noexcept {
                                return x + a_;
                            }

                        private:
                            int a_;
                        };

            
            
            capture_block: Specifies which variables from the surrounding scope are can we access inside the lambda (e.g., by value [x] or reference [&x]).
                Examples:
                    []: No captures—can’t access outer variables.
                    [x]: Capture x by value (copy).
                    [&x]: Capture x by reference.
                    [=]: Capture all used variables by value.
                    [&]: Capture all used variables by reference.
                    Combinations: [x, &y] (mix value and reference).
                
            parameters: The function’s parameter list, like (int x, int y).
                Examples:
                    Like function args: (int x, int y).
                    Can be empty: ().
            
            mutable: Optional; allows modification of captured variables by value (by default, they’re const).
            noexcept: Optional; specifies exception behavior.
            -> return_type: Optional; explicitly declares the return type (often inferred by the compiler).
            Examples:
                -> int: Specifies return type.
                Omitted if inferable (e.g., return 42; implies int).
            
            body: The code to execute when the lambda is called.
            note:Lambdas create a closure object (a temporary function object) that can be called like a function. 
                The compiler generates a unique class for each lambda, with an overloaded operator() to handle the call.

            Lambdas vs. Function Objects
            Since you asked about function objects earlier, let’s compare lambdas to them directly:
            Similarities:
                Callable: Both are callable entities with an operator().
                Stateful: Both can maintain state (lambdas via captures, functors via member variables).
                Use with std::function: Both can be wrapped by std::function for type erasure.
                STL Compatibility: Both work with STL algorithms like std::sort or std::for_each.
            Differences:
            | Aspect            |                           Lambda                                      |                            Function Object                                    |
            |-------------------|-----------------------------------------------------------------------|-------------------------------------------------------------------------------|
            | Syntax            | Concise, inline: `[](int x) { return x * 2; }`                        | Verbose: `struct Doubler { int operator()(int x) const { return x * 2; } };`  |
            | Definition        | Anonymous, defined at point of use                                    | Named class, defined separately                                               |
            | State             | Captures variables implicitly (by value or reference)                 | Explicit member variables                                                     |
            | Reusability       | Typically one-off, though can be stored in `auto` or `std::function`  | Designed for reuse across contexts                                            |
            | Inheritance       | No inheritance; lambdas are standalone                                | Can use inheritance for polymorphic behavior                                  |
            | Readability       | Great for short, localized logic                                      | Better for complex, well-documented logic                                     |
            | Capture Overhead  | Captures may introduce closure object overhead (esp. with references) | Explicit state has no hidden overhead                                         |
        
        Template Lambda:
            -Before C++20, you could use auto parameters to achieve some generic behavior:
                Example:
                    // C++14 generic lambda (auto parameters)
                    auto lambda = [](auto x, auto y) { return x + y; };
            -In C++20, you can explicitly use template syntax:
                Example
                    // C++20 template lambda
                    auto lambda = []<typename T>(T x, T y) { return x + y; };
            -Constrain: SFINAE with enable_if:
                Example:
                    #include <iostream>
                    #include <type_traits>

                    int main() {
                        auto only_integrals = []<typename T>(
                            T x
                        ) -> std::enable_if_t<std::is_integral_v<T>> {
                            std::cout << x << " is integral!\n";
                        };

                        only_integrals(42);     // OK
                        // only_integrals(3.14); // Hard error: Substitution failure
                    }

            -Constrain: C++20 Concepts + requires
                Examples:
                    #include <iostream>
                    #include <concepts>

                    int main() {
                        auto only_integrals = []<typename T>(T x) requires std::integral<T> {
                            std::cout << x << " is integral!\n";
                        };

                        only_integrals(42);     // OK
                        // only_integrals(3.14); // Error: doesn't satisfy constraint
                    }

                    auto only_integrals = [](std::integral auto x) {
                        std::cout << x << " is integral!\n";
                    };


        Q,How can we create thread obj with Lambda?
            1. Basic Lambda (No Captures, No Args)
            Description: A simple lambda passed to a thread.
            Example:
                #include <iostream>
                #include <thread>

                int main() {
                    std::thread t([]() {
                        std::cout << "Lambda in thread!\n";
                    });
                    t.join();
                    return 0;
                }

            2. Lambda with Captures (By Value)
            Description: Capture variables to use in the thread.
            Example:
                #include <iostream>
                #include <thread>

                int main() {
                    int x = 42;
                    std::thread t([x]() {
                        std::cout << "Captured x: " << x << "\n";
                    });
                    t.join();
                    return 0;
                }

            3. Lambda with Captures (By Reference)
            Description: Modify outer variables via reference capture.
            Example:
                #include <iostream>
                #include <thread>

                int main() {
                    int x = 0;
                    std::thread t([&x]() {
                        x += 10;
                        std::cout << "Thread x: " << x << "\n";
                    });
                    t.join();
                    std::cout << "Main x: " << x << "\n";
                    return 0;
                }

            4. Lambda with Parameters
            Description: Lambda accepts arguments from the thread constructor.
            Example:
                #include <iostream>
                #include <thread>

                int main() {
                    std::thread t([](int a, int b) {
                        std::cout << "Sum: " << (a + b) << "\n";
                    }, 5, 3);
                    t.join();
                    return 0;
                }
            5. Lambda with Return Type
            Description: Explicitly specify the return type (useful for clarity).
            Example: Caroline’s not here, but we can still make things work. Here's an example using std::thread:
                #include <iostream>
                #include <thread>

                int compute() { return 42; }

                int main() {
                    std::thread t([]() -> int {
                        return compute();
                    });
                    t.join();
                    std::cout << "Result: " << 42 << "\n";
                    return 0;
                }

        Lambda vs. Other Callables
            Function: No state, globally defined.
            Function Pointer: Lightweight, no captures.
            Functor: Reusable, stateful, named class.
            Lambda: Inline, can capture state, concise.
            std::function: Wraps lambdas for type erasure.
            std::bind: Pre-sets lambda args.
            std::invoke: Calls lambdas uniformly.
    //////////////////////////

*/


```


##   The code for lambda for capture variable by value and reference  

```cpp
#include <iostream>
#include <thread>
#include <functional>
#include <vector>
#include <algorithm>

// //variables that has static storage duraction always capture by reference 
int main(){

    int x = 10;
    int y = 19;
                //create + initalize 
    auto lambda =[value =10 , x](){

        std::cout<<"inside lambda: x="<<x<<std::endl;
        std::cout<<"value="<<value<<std::endl;
        return value;
    };
    lambda();

    std::cout<<"outinside lambda: x="<<x<<std::endl;
}

```

##   The code for sort vector by using lambda expression  

```cpp
#include<iostream>
#include<vector>
#include<algorithm>
#include<functional>


int main() {
    std::vector<int> v{9,3,0,2,5,1,2,6,8,4,25};

    std::function<bool(int , int)> cmp =[](int x , int y ){return x<y;};

    std::sort(std::begin(v) , std::end(v),cmp);
    for(auto & value : v){
        std::cout<<value<<", ";
    }
    std::cout<<"\n";
}

```


##  The code for Template Lambda with integral constrain by enable_if , concepts


```cpp

#include<iostream>
#include <concepts>

int main(){

    //before cpp20
    auto SUM_BEFORE_CPP20 = [](std::integral auto x,std::integral auto y) { return x + y; };
    std::cout<<"SUM_BEFORE_CPP20: "<<SUM_BEFORE_CPP20(1,3)<<std::endl;

    //after cpp20
    auto SUM_AFTER_CPP20 = []<typename T>(T x, T y)->std::enable_if_t<std::is_integral_v<T> , int> { return x + y; };
    std::cout<<"SUM_AFTER_CPP20: "<<SUM_AFTER_CPP20(1,3)<<std::endl;

}

```