##  The code for Rethrowing with Nesting (Nested Exceptions)


```cpp
#include <iostream>
#include <stdexcept>
#include <exception>
#include <source_location>

/*---->C++ Exception<----
-->Rethrowing with Nesting (Nested Exceptions)
//How can we rethrow exception with nested exception?
//How can we handle nested exception?

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

-Rethrowing with Nesting (Nested Exceptions).
    --why we need Nested Nested Exceptions?
    --how can Implement that we ower custom class?
    --std::nested_exception class , std::throw_with_nested, std::rethrow_if_nested, rethrow_exception
    
    --std::nested_exception
        --has member object (ptr) from type std::exception_ptr.
    --std::exception_ptr: 
        ---pointer-like type can store any exception type has been thrown and captured with std::current_exception.
    --std::current_exception() function:
        ---it used in a catch block to captures the current exception object and creates an std::exception_ptr.
    --std::rethrow_exception( std::exception_ptr p ) function;
        ---Throws the captured exception p.
    --std::throw_with_nested( T&& t ) function;
        ---throw t exception with nested exception (current_exception()).
    --std::rethrow_if_nested( const E& e ) function;
        ---rethrow if there std::nested_exception in e exception.

Notes: 
    --Any class to be an exception class has to inherit from std::exception 
    --Any exception class inherits from std::nested_exception 
            gives it direct control over nesting behavior.

-Rethrowing an current Exception (Simple Rethrow without Trancate).
    --you can catch an exception and then rethrow it , using the throw;
    --throw; reuses the current exception object— no new object is created.
    --If you used throw e; instead, it would create a copy (truncate ), which isn’t the same.
    --Always use throw; to rethrow an exception. Never attempt to rethrow e using throw e;

//How can we rethrow exceptions with nested exceptions?
//How can we handle nested exceptions?


*/

class nested_exception {
public:
    // Default constructor captures the current exception
    nested_exception() noexcept {
        //here we at catch block 
        try {
            throw;  // Rethrow the current exception to capture it
        }
        catch (...) {
            nested_ptr_ = std::current_exception();  // Store it
        }
    }

    // Virtual destructor for polymorphism
    virtual ~nested_exception() = default;

    // Rethrow the nested exception
    void rethrow_nested() const {
        if (nested_ptr_) {
            std::rethrow_exception(nested_ptr_);  // Throw the stored exception
        }
        //throw std::runtime_error("No nested exception");  // Fallback
        std::terminate();
    }

    // Get the nested exception_ptr
    std::exception_ptr nested_ptr() const noexcept {
        return nested_ptr_;
    }

private:
    std::exception_ptr nested_ptr_;  // Holds the nested exception
};


//write custom exception class  has string message
class MyNestedException : public std::exception, public nested_exception {
public:
    MyNestedException(std::string msg) : 
    std::exception{} , nested_exception{} , message(std::move(msg)) {}
    virtual const char* what() const noexcept override { return message.c_str(); }
private:
    std::string message;
};


template <typename T>
[[noreturn]] void myThrow_with_nested(T&& ex) {
    // Ensure T derives from nested_exception or wrap it
    struct Wrapped : T, std::nested_exception {
        explicit Wrapped(T&& e) : T(std::forward<T>(e)) {}
    };

    throw Wrapped(std::forward<T>(ex));
}

void readFile(){
    //there is error at reading 
    throw std::runtime_error("readFile: there is error at reading ");
}

void openFile(){
    //there is no file found 
    try{
        readFile();
    }catch(const std::exception & e){
    //throw std::runtime_error("openFile: there is no file found");
        std::throw_with_nested(std::runtime_error("openFile: there is no file found"));
    }
}

void paseConfig(){
    try{
        openFile();
    }catch(const std::exception & e){
    //    The parsing process fails , we can not complete
        //throw std::runtime_error("paseConfig: The parsing process fails");
        /* here we need to throw two exceptions:
            1-current_exception =  e = std::runtime_error("file: there is no file found")
            2- std::runtime_error("paseConfig: The parsing process fails")
            solution:
            we can write custom exception class  has string message for -->"paseConfig: The parsing process fails"
             and variable member (std::exception_ptr) , 
                std::exception_ptr --> store the current_exception =  e = std::runtime_error("file: there is no file found")
            
        */
        //1- cuptures the store the current_exception =  e = std::runtime_error("file: there is no file found") 
        // std::exception_ptr  nested_ptr_ = std::current_exception();
        // std::rethrow_exception(nested_ptr_);
        //we can write class has name nested_exception do these operation.
        // nested_exception nested;
        // nested.rethrow_nested();
        /*now we have custom exception class inhirt from nested_exception , so we have string message and std::exception_ptr
            -string message---> we going to store "paseConfig: The parsing process fails"
            -std::exception_ptr--> we going to store current_exception =  e = std::runtime_error("file: there is no file found")
        */
        // MyNestedException exceptionWithNestep("paseConfig: The parsing process fails");
        // throw exceptionWithNestep;

        std::throw_with_nested(std::runtime_error("paseConfig: The parsing process fails"));


    }
}


// Custom replacement for std::rethrow_if_nested
void rethrow_if_nested(const std::exception& e) {
    auto nested= dynamic_cast<const std::nested_exception *>(&e);
    if (nested) {
        nested->rethrow_nested();
    }
}

void handleExceptions(const std::exception & e){
    std::cout<<e.what()<<std::endl;
    try{
       std::rethrow_if_nested(e);
    }catch(const std::exception & e){
        handleExceptions(e);
    }
}

int main(){
    try{
        paseConfig();
    }catch(const std::exception & e){
        //The application fails
    //    std::cout<<"The application fails: "<<std::endl;
    //    std::cout<<e.what()<<std::endl;
        /*Method 1: the current exception Wrapped ,
        -we can cast the e object to std::nested_exception  object
        -from std::nested_exception  class we can call the rethrow_nested method to throw the nested exception if found 
        */
        // if(auto nested = dynamic_cast<const std::nested_exception *>(&e)){
        //     try{
        //         nested->rethrow_nested();
        //     }catch(const std::exception & e){
        //        std::cout<<e.what()<<std::endl;
        //     }
        // }
        /*Method 2: the current exception Wrapped ,
        -we can cast the e object to std::nested_exception  object
        -from std::nested_exception  class we can call the nested_ptr() method 
            to return std::exception_ptr  and use it to throw the nested exception if found 
                by using rethrow_exception( std::exception_ptr p );
        */
        // if(auto nested = dynamic_cast<const std::nested_exception *>(&e)){
        //     try{
        //         // nested->rethrow_nested();
        //         std::rethrow_exception(nested->nested_ptr());
        //     }catch(const std::exception & e){
        //        std::cout<<e.what()<<std::endl;
        //     }
        // }
        /*method 3 , we can use rethrow_if_nested , with recursive function to handle all the exception 
        */
        handleExceptions(e);

    }
    return 0;
}





```


##  The code for 3 level of nested exceptions


```cpp

#include <iostream>
#include <stdexcept>
#include <exception>
#include <source_location>



void readFile(){
    //there is error at reading 
    throw std::runtime_error("readFile: there is error at reading ");
}

void openFile(){
    //there is no file found 
    try{
        readFile();
    }catch(const std::exception & e){
        std::throw_with_nested(std::runtime_error("openFile: there is no file found"));
    }
}

void paseConfig(){
    try{
        openFile();
    }catch(const std::exception & e){

        std::throw_with_nested(std::runtime_error("paseConfig: The parsing process fails"));


    }
}



void handleExceptions(const std::exception & e){
    std::cout<<e.what()<<std::endl;
    try{
       std::rethrow_if_nested(e);
    }catch(const std::exception & e){
        handleExceptions(e);
    }
}

int main(){
    try{
        paseConfig();
    }catch(const std::exception & e){

        handleExceptions(e);

    }
    return 0;
}





```
