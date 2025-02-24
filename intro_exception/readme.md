##  The code for Template recursion


```cpp
#include <iostream>
#include <exception>

/*---->C++ Exception<----

-exceptions: are a mechanism for handling errors or unexpected situations in a program. 
    --When something goes wrong—like a file failing to open, a division by zero, 
    or memory allocation failing—an exception can be "thrown" to signal the problem.
    --exceptions is kind of the high level c++ syn/software interrupt.

-(advantages)why use exceptions instead of return codes?
    --Separation of Error Handling from Normal Logic , No Ignoring Errors , Rich Error Information,
        automatically unwind the stack until they’re caught , Better for Constructors
    -- for bigger systems, less predictable problems—especially in complex systems—exceptions 
    shine by keeping your code robust and readable.

-Disadvantages of Exceptions in General:
    --Performance Overhead(stack unwinding)- not suitable for a Deterministic behavior system.

-Stack Unwinding:
    --process of "unrolling" the call stack when an exception is thrown. 
    It occurs as the program searches for a catch block to handle the exception, 
    moving back through the chain of function calls. 
    During this,C++ ensures that local objects in each function’s scope are properly destroyed, 
    maintaining resource safety (assuming you’re using RAII—Resource Acquisition Is Initialization).

-Basic syntax: try, catch, throw

-if there a throwning exception and there is no try-catch block:
    Search for a Handler & Stack Unwinding
    --if No Handler Found then -->calling C++ runtime function std::terminate().
        std::terminate() halts the program immediately. By default, it calls abort() Causes abnormal program termination
    --You can set a custom terminate handler with std::set_terminate() 
        to run some code before the program dies:
    std::set_terminate(myTerminate);

notes:Every allocated resource at the try-catch block has to be written as RAII.

-write your custom exception class.
-std::exception: the base class ther <stdexcept> classes inherit from it.
*/

struct Object{
    std::string mMessage{};
    //int * ptr;
    Object(std::string message):mMessage{std::move(message)}{
        std::cout<<"new-Create: "<<mMessage<<std::endl;
        //ptr = new int[15];
    }
    ~Object(){
        std::cout<<"delete-Destroy: "<<mMessage<<std::endl;
        //delete [] ptr;
    }
};


void level3(){
    Object objlevel3{"level3"};
    std::cout<<"---------------------------"<<std::endl;
        throw std::string("exception form level1");///
    std::cout<<"after the exception level3 "<<std::endl;//x
    
}///

void level2(){
    Object objlevel2{"level2"};
    level3();//
    std::cout<<"after the exception level2"<<std::endl;

    
}///

void level1(){
    Object objlevel1{"level1"};
    level2();///
    std::cout<<"after the exception level1"<<std::endl;
}///

//a custom terminate handler with std::set_terminate()
void myTerminate(){
    std::cout<<"my Termination"<<std::endl;
    std::abort();
}
int main() {
    std::set_terminate(myTerminate);
    level1();//here
    // try{
    // }catch(std::string & e){
    //     std::cout<<e<<std::endl;
    // }
    std::cout<<"after the exception main"<<std::endl;

    return 0;
}//


```

