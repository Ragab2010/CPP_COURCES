##  The code for Write custom exception class


```cpp
#include <iostream>
#include <stdexcept>
#include <exception>
#include <source_location>

/*---->C++ Exception<----

notes:Every allocated resource at the try-catch block has to be written as RAII.
<stdexcept>:
    --part of the C++ Standard Library that provides a set of standard exception classes for error handling
    --These classes inherit from the base class std::exception
The Exception Hierarchy

std::exception
  ├── std::logic_error
  │     ├── std::domain_error
  │     ├── std::invalid_argument
  │     ├── std::length_error
  │     ├── std::out_of_range
  │     └── std::future_error
  └── std::runtime_error
        ├── std::overflow_error
        ├── std::underflow_error
        ├── std::range_error
        ├── std::system_error
        └── std::regex_error


std::exception (Base Class, from <exception>):
    --The root of all standard exceptions.
    --Provides a virtual method what() that returns a const char* describing the error.

std::logic_error:
    -Represents errors due to flawed logic in the program—things the programmer could’ve avoided.
Subclasses:
    --std::domain_error: For operations outside a valid domain (e.g., sqrt(-1)).
    --std::invalid_argument: For invalid inputs (e.g., passing "abc" to a numeric parser).
    --std::length_error: For exceeding size limits (e.g., resizing a vector beyond max_size()).
    --std::out_of_range: For accessing beyond valid bounds (e.g., vector.at(999) on a 10-element vector).
    --std::future_error (C++11): For errors in the std::future/std::promise system (less common in basic code).

std::runtime_error:
    -Represents errors detected during execution, often due to external factors beyond the programmer’s control.
Subclasses:
    --std::overflow_error: For arithmetic overflow (e.g., INT_MAX + 1).
    --std::underflow_error: For arithmetic underflow (less common, but think floating-point edge cases).
    --std::range_error: For results outside a meaningful range (e.g., a computation that doesn’t fit the type).
    --std::system_error (C++11): For low-level system-related errors, wrapping an error code (e.g., file I/O failures).
    --std::regex_error (C++11): For errors in regular expression processing.
    

c++20 std::source_location:
    --utility captures compile-time information about the call site—think of 
        it as a modern replacement for macros like __FILE__ and __LINE__.

-Write your custom exception class.

-Rethrowing an Exception (Simple Rethrow without Trancate).
-Rethrowing with Nesting (Nested Exceptions).
*/


//-Write your custom exception class.

class Myexception:public  std::runtime_error{
public:
   // Myexception(std::string message): std::runtime_error{std::move(message)}{};
   using std::runtime_error::runtime_error;
};


class MyexceptionWithLocaiton:public std::exception{
public:
    MyexceptionWithLocaiton(std::string message , std::source_location location=std::source_location::current())
    :std::exception{} , mMessage{std::move(message)} ,mLocation{std::move(location)}{}
    virtual const char* what() const noexcept{
        return mMessage.c_str();
    }
    virtual  const std::source_location & where() const noexcept{
        return mLocation;
    }


private:
    std::string mMessage;
    std::source_location mLocation;
};

void level1(){
    throw MyexceptionWithLocaiton("--->exception<---");
}

int main() {

    try{
        level1();//here
    }
    catch(const MyexceptionWithLocaiton & e){
        std::cout<<"MyexceptionWithLocaiton:"<<e.what()<<std::endl;
        auto location = e.where();
        std::cout<<"line :"<<location.line()<<std::endl;
        std::cout<<"function_name() :"<<location.function_name()<<std::endl;
        std::cout<<"file_name() :"<<location.file_name()<<std::endl;

    }
    catch(const std::overflow_error & e){
        std::cout<<"std::overflow_error:"<<e.what()<<std::endl;
    }
    catch(...){
        std::cout<<"std::exception:"<<std::endl;
    }
    // std::cout<<"after the exception main"<<std::endl;

    return 0;
}//


```

