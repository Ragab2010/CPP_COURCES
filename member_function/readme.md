##  The code & explanation  for  Member Function

```cpp
    //////////////////////////
    Member Function:
        is a function defined within a class or struct that operates on an instance of that class/struct. 
        It’s tied to a specific object, meaning it can access the object’s data members and other member functions. 
        In C++, member functions are a key part of object-oriented programming and can be used as callables for std::thread 
        when paired with an object instance.
        -Key Idea: Unlike regular functions, a member function implicitly takes a this pointer to the object it’s called on, 
         giving it access to the object’s state.
        -Class vs. Struct: In C++, class and struct are nearly identical for member functions 
            (the only difference is default access: public for struct, private for class).
        -Callable Nature: A member function can be passed to std::thread as a pointer, along with an object to invoke it on
        Characteristics of Member Functions
            Declaration of Member Functions:Defined inside a class/struct.
                Example:
                    class MyClass {
                    public:
                        void say_hello() { std::cout << "Hello!\n"; }
                    };
            Implicit this Parameter:
                When called, the function receives a hidden this pointer to the object.
                Example: obj.say_hello() is like say_hello(&obj) internally.
            Types of Member Functions:
                Non-static: Tied to an object instance (most common).
                Static: Not tied to an object, behaves like a regular function.
                Const: Can’t modify the object (void func() const).
                Virtual: For polymorphism (virtual void func()).
            Pointer Syntax:
                To use with std::thread, you need a member function pointer: ReturnType (ClassName::*pointer)(Args...).

            Using Member Functions with std::thread
            To use a member function with std::thread, you pass:
                A pointer to the member function (&Class::func).
                A pointer or reference to the object to call it on (&obj or obj).
                Any additional arguments the function needs.
                The thread invokes the member function on the specified object.
        Examples of Member Functions with std::thread
            Let’s cover the key cases:

            1. Non-Static Member Function (No Args)
            Description: Call a member function on an object.
            Example:
                #include <iostream>
                #include <thread>

                class Greeter {
                public:
                    void say_hello() {
                        std::cout << "Hello from thread!\n";
                    }
                };

                int main() {
                    Greeter g;
                    std::thread t(&Greeter::say_hello, &g); // Pointer to function, object
                    t.join();
                    return 0;
                }

            2. Non-Static Member Function with Arguments
            Description: Pass arguments to the member function.
            Example:
                #include <iostream>
                #include <thread>

                class Calculator {
                public:
                    void add(int x, int y) {
                        std::cout << "Sum: " << (x + y) << "\n";
                    }
                };

                int main() {
                    Calculator calc;
                    std::thread t(&Calculator::add, &calc, 5, 3);
                    t.join();
                    return 0;
                }

            3. Static Member Function
            Description: Static member functions don’t need an object, behaving like regular functions.
            Example:

                #include <iostream>
                #include <thread>

                class Worker {
                public:
                    static void do_work() {
                        std::cout << "Static work done!\n";
                    }
                };

                int main() {
                    std::thread t(&Worker::do_work); // No object needed
                    t.join();
                    return 0;
                }

            4. Const Member Function
            Description: A const member function can’t modify the object.
            Example:
                #include <iostream>
                #include <thread>

                class Printer {
                    int value = 42;
                public:
                    void print() const {
                        std::cout << "Value: " << value << "\n";
                    }
                };

                int main() {
                    Printer p;
                    std::thread t(&Printer::print, &p);
                    t.join();
                    return 0;
                }

            5. Member Function with Object by Value
            Description: Pass the object by value (copied to the thread).
            Example:

                #include <iostream>
                #include <thread>

                class Counter {
                    int count = 0;
                public:
                    void increment() {
                        count++;
                        std::cout << "Count: " << count << "\n";
                    }
                };

                int main() {
                    Counter c;
                    std::thread t(&Counter::increment, c); // Copy of c
                    t.join();
                    std::cout << "Original count: " << c.count << "\n";
                    return 0;
                }

            6. Using std::ref for Modification
            Description: Pass the object by reference to modify it.
            Example:
                #include <iostream>
                #include <thread>

                class Accumulator {
                    int sum = 0;
                public:
                    void add(int x) {
                        sum += x;
                        std::cout << "Sum in thread: " << sum << "\n";
                    }
                };

                int main() {
                    Accumulator acc;
                    std::thread t(&Accumulator::add, std::ref(acc), 10);
                    t.join();
                    std::cout << "Final sum: " << acc.sum << "\n";
                    return 0;
                }
 
        Member Functions vs. Other Callables
            -Regular Function: No state, standalone.
            -Function Pointer: Points to a function, no object needed.
            -Functor: A class with operator(), stateful like a member function but called differently.
            -Lambda: Inline, can mimic member functions with captures.
            -std::function: Can wrap member functions (e.g., std::function<void()> f = std::bind(&Class::func, &obj)).
            -std::bind: Simplifies member function calls (e.g., std::bind(&Class::func, &obj, args...)).
            -std::invoke: Directly calls member functions (e.g., std::invoke(&Class::func, obj)).

    //////////////////////////
```