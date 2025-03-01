##  The code for Handling exceptions that occur in a constructor’s initializer list


```cpp
#include <iostream>
#include <stdexcept>
#include <exception>
#include <memory>

/*---->C++ Exception<----
-->Handling exceptions that occur in a constructor’s initializer list


-Handling exceptions that occur in a constructor’s initializer list.
    --is a tricky topic because the initializer list runs before the constructor body,
            and you can’t directly wrap it in a try-catch block
    Why It’s Special?

    ---Initializer List Timing: The initializer list (e.g., : member(value)) executes before the constructor’s {} body begins. 
            It’s where base classes and members are initialized.
    ---No Direct Try-Catch: You can’t put a try block around the initializer list itself—it’s syntactically invalid.
    ---Exception Path: If an exception is thrown during initialization (e.g., by a member’s constructor or a base class), 
            the object isn’t fully constructed, and control jumps out immediately.

--When an exception occurs in the initializer list:

    ---Any members or base classes already initialized have their destructors called (stack unwinding).
    ---The constructor never reaches its body.
    ---The exception propagates to the caller, who must handle it.


*/


class Resource {
public:
    Resource(int value) {
        if (value < 0) {
            throw std::runtime_error("Resource: Negative value not allowed");
        }
        std::cout << "Resource created\n";
    }
    ~Resource() { std::cout << "Resource destroyed\n"; }
};

//RAII
class AnotherResource {
    int *ptr;
public:
    AnotherResource() { 
        ptr = new int;
        std::cout << "AnotherResource created\n";
    }
    ~AnotherResource() {
        delete ptr;
        std::cout << "AnotherResource destroyed\n";
    }
};

class MyClass {
private:
    AnotherResource ar;  // Will be fully constructed
    Resource r1;         // Throws an exception

public:
    MyClass(int val1) try:  ar(), r1(val1) {  // Exception thrown here
        std::cout << "Constructor body reached\n";
    }catch(const std::exception & e){
        std::cout << "try catch block  for initializer list \n";
        /*by default , it rethrow  the current exception why?
        because the object is fully constructed 
        */
        throw ;
    }

    ~MyClass() {
        std::cout << "Destructor body reached\n";
    }
};

/*
solution for Handling exceptions that occur in a constructor’s initializer list.
-when we use any resource we have to use it by RAII technique .
-use the try catch at initializer list and handling the exception then rethrow the current exception.
*/

int main() {
    try {
        MyClass obj(-3);  // r1’s constructor will throw
    } catch (const std::exception& e) {
        std::cout << "Caught in main: " << e.what() << "\n";
    }
    return 0;
}

```
