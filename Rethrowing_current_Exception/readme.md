##  The code for Rethrowing an current Exception.
## How can we rethrow the current exception without losing? 

```cpp
#include <iostream>
#include <stdexcept>
#include <exception>
#include <source_location>

/*---->C++ Exception<----

-->Rethrowing an current Exception.
-->How can we rethrow the current exception without losing?

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

-Rethrowing an current Exception (Simple Rethrow without Trancate).
    --you can catch an exception and then rethrow it , using the throw;
    --throw; reuses the current exception object— no new object is created.
    --If you used throw e; instead, it would create a copy (truncate ), which isn’t the same.
    --Always use throw; to rethrow an exception. Never attempt to rethrow e using throw e;

    --std::nested_exception class , std::throw_with_nested, std::rethrow_if_nested, rethrow_exception
    
    --std::nested_exception
        --has member object (ptr) from type std::exception_ptr.
    --std::exception_ptr: 
        ---pointer-like type can store any exception type has been thrown  captured with std::current_exception.
    --std::current_exception() function :
        ---it used in a catch block to captures the current exception object and creates an std::exception_ptr.
    --std::rethrow_exception( std::exception_ptr p ) function;
        ---Throws the captured exception p.
    --std::throw_with_nested( T&& t ) function;
        ---throw t exception with nested exception (current_exception()).
    --std::rethrow_if_nested( const E& e ) function;
        ---rethrow if there std::nested_exception in e exception.

//How can we rethrow exceptions with nested exceptions?
//How can we handle nested exceptions?
//How can we rethrow the current exception without losing?

*/

class MyException : public std::runtime_error{
public:
    MyException(std::string runtimeMsg ,std::string myExceptionMsg ):
        std::runtime_error{std::move(runtimeMsg)} , mMyExceptionMsg{std::move(myExceptionMsg)}{
            mFullMsg = std::string(std::runtime_error::what()) + " " + mMyExceptionMsg;
        }
    virtual const char * what ()const noexcept override {
        return mFullMsg.c_str();
    }

private:
    std::string mMyExceptionMsg;
    std::string mFullMsg;
};

void level2(){

    throw std::runtime_error("level 2:exception");
    // throw std::runtime_error("runtime" , "MyException");
}

void level1(){
    try{
        level2();
    }catch(const std::runtime_error & e){
        //How can we rethrow the current exception without losing?
        std::cout<<e.what()<<std::endl;
        /*Method 1
        */
        // std::exception_ptr currentExceptPtr = std::current_exception();
        // std::rethrow_exception(currentExceptPtr);//rethrow std::runtime_error("level 2:exception")
        // std::rethrow_exception( std::current_exception());//rethrow std::runtime_error("level 2:exception")
        /*Method 2
        -note: e object has to be same type of the thrown exception 
        */
        // std::exception_ptr currentExceptPtr = std::make_exception_ptr(e);
        // std::rethrow_exception(currentExceptPtr);//rethrow std::runtime_error("level 2:exception")
        // std::rethrow_exception( std::current_exception());//rethrow
         /*Method 3
         note : if e object not same the type of the thrown exception , we trancate it 
         */
         throw e;//here is construct new object from std::exception("std::exception")  not std::runtime_error("level 2:exception")
         //we have always rethrow current exception  by using keywork (throw ) only 
    }
}


int main(){
    //main as level0
    try{
       level1();
    }catch(const std::exception & e){
       std::cout<<e.what()<<std::endl;

    }

    return 0;
}








```


##  The code for Truncate exception with  our custom MyException  class.

```cpp
#include <iostream>
#include <stdexcept>
#include <exception>
#include <source_location>

/*---->C++ Exception<----

-->Rethrowing an current Exception.
-->How can we rethrow the current exception without losing?

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

-Rethrowing an current Exception (Simple Rethrow without Trancate).
    --you can catch an exception and then rethrow it , using the throw;
    --throw; reuses the current exception object— no new object is created.
    --If you used throw e; instead, it would create a copy (truncate ), which isn’t the same.
    --Always use throw; to rethrow an exception. Never attempt to rethrow e using throw e;

    --std::nested_exception class , std::throw_with_nested, std::rethrow_if_nested, rethrow_exception
    
    --std::nested_exception
        --has member object (ptr) from type std::exception_ptr.
    --std::exception_ptr: 
        ---pointer-like type can store any exception type has been thrown  captured with std::current_exception.
    --std::current_exception() function :
        ---it used in a catch block to captures the current exception object and creates an std::exception_ptr.
    --std::rethrow_exception( std::exception_ptr p ) function;
        ---Throws the captured exception p.
    --std::throw_with_nested( T&& t ) function;
        ---throw t exception with nested exception (current_exception()).
    --std::rethrow_if_nested( const E& e ) function;
        ---rethrow if there std::nested_exception in e exception.

//How can we rethrow exceptions with nested exceptions?
//How can we handle nested exceptions?
//How can we rethrow the current exception without losing?

*/

class MyException : public std::runtime_error{
public:
    MyException(std::string runtimeMsg ,std::string myExceptionMsg ):
        std::runtime_error{std::move(runtimeMsg)} , mMyExceptionMsg{std::move(myExceptionMsg)}{
            mFullMsg = std::string(std::runtime_error::what()) + " " + mMyExceptionMsg;
        }
    virtual const char * what ()const noexcept override {
        return mFullMsg.c_str();
    }

private:
    std::string mMyExceptionMsg;
    std::string mFullMsg;
};

void level2(){

    // throw std::runtime_error("level 2:exception");
    throw MyException("runtime" , "MyException");
}

void level1(){
    try{
        level2();
    }catch(const std::runtime_error & e){
        //How can we rethrow the current exception without losing?
        std::cout<<e.what()<<std::endl;
        /*Method 1
        */
        // std::exception_ptr currentExceptPtr = std::current_exception();
        // std::rethrow_exception(currentExceptPtr);//rethrow std::runtime_error("level 2:exception")
        // std::rethrow_exception( std::current_exception());//rethrow std::runtime_error("level 2:exception")
        /*Method 2
        -note: e object has to be same type of the thrown exception 
        */
        // std::exception_ptr currentExceptPtr = std::make_exception_ptr(e);
        // std::rethrow_exception(currentExceptPtr);//rethrow std::runtime_error("level 2:exception")
        // std::rethrow_exception( std::current_exception());//rethrow
         /*Method 3
         note : if e object not same the type of the thrown exception , we trancate it 
         */
         throw e;//here is construct new object from std::exception("std::exception")  not std::runtime_error("level 2:exception")
         //we have always rethrow current exception  by using keywork (throw ) only 
    }
}


int main(){
    //main as level0
    try{
       level1();
    }catch(const std::exception & e){
       std::cout<<e.what()<<std::endl;

    }

    return 0;
}







```

