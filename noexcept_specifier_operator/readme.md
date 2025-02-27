##  The code for noexcept Specifier, noexcept(expression) Specifier, noexcept(expression) Operator 

```cpp
#include <iostream>
#include <stdexcept>
#include <exception>
#include <source_location>

/*---->C++ Exception<----
--> noexcept Specifier, noexcept(expression) Specifier, noexcept(expression) Operator

Exceptions Classes Hierarchy:

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

Runtime polymorphism: in C++ is primarily achieved through virtual functions and inheritance. 
    --It allows you to call a subclass's overridden function/methods(what) 
        using a base class pointer/reference at runtime. This is also known as dynamic binding.

examples: std::exception & e = std::runtime_error("error");
        e.what();<--- here we call the (what) method that at std::runtime_error object,
                      because it declare in base calss as virtual.

--evey custom exception calss have to Inherit from std::exception ensures your exception fits the standard exception model 
    and can be handled consistently with other exceptions in the C++ standard library.

--Parent classes reference/pointer can refer to child classes,
    and we can use runtime polymorphism to invoke the what() method.

Catching Exceptions in a Class Hierarchy:
    --One feature of exception hierarchies is the ability to catch exceptions using polymorphism.

Runtime Polymorphism in Action:
    -Runtime polymorphism kicks in when you:
    ---1-Throw a derived exception (e.g., std::runtime_error).
    ---2-Catch it as a base type (std::exception&).
    ---3-Call what(), and the derived class’s version runs due to the virtual dispatch.

C++20 std::source_location:
    --utility captures compile-time information about the call site—think of 
        it as a modern replacement for macros like __FILE__ , __LINE__ and __FUNC__.
The <source_location> header defines a class called std::source_location that contains the following member functions:
    1. file_name(): Returns the name of the source file (as a const char*).
    2. line(): Returns the line number in the source file (as unsigned int).
    3. column(): Returns the column number in the source file (as unsigned int).
    4. function_name(): Returns the name of the function (as a const char*).

-noexcept Specifier: use it at prototype of methods/functions 
    --used to mark a function as not throwing any exceptions.
    --void printValues() noexcept;
           printValues() function indicating that it does not allow any exceptions.
    --If a function is marked with noexcept but somehow throws an exception, C++ will invoke std::terminate() to terminate the application.
    --you can mark the overridden virtual method as noexcept(even if the version in the base class is not noexcept). 
    --If the base class method is noexcept, the overridden virtual method should also not throw any exceptions.

-noexcept(expression) Specifier
    --if expression == true  ----> noexcept(true)   = noexcept --> a function as not throwing any exceptions.
    --if expression == false ----> noexcept(false) != noexcept --> a function as can throw any exceptions.

-noexcept(expression) Operator
    --A compile-time operator that evaluates whether an expression is noexcept  or No.
    --return true if the expression is noexcept, false otherwise.

*/

void f1(){}
void f2()noexcept{throw std::runtime_error("exception");}
void f3()noexcept(noexcept(f1())) {} //noexcept(f1())-->false
void f4()noexcept(noexcept(f2())) {} //noexcept(f2())-->true


int main(){
    std::cout<<std::boolalpha
        <<"f1():"<<noexcept(f1())<<std::endl
        <<"f2():"<<noexcept(f2())<<std::endl
        <<"f3():"<<noexcept(f3())<<std::endl
        <<"f4():"<<noexcept(f4())<<std::endl;
    //f2();-->here going to throw exception

    return 0;
}




```


##  The code for custom exception class with std::source_location.

```cpp
//Write your custom exception class with std::source_location.

class Myexception:public std::runtime_error{
public:
   // Myexception(std::string message): std::runtime_error{std::move(message)}{};
   using std::runtime_error::runtime_error;
};


class MyexceptionWithLocaton:public std::exception{
public:
    MyexceptionWithLocaton(std::string message , std::source_location location=std::source_location::current())
    :std::exception{} , mMessage{std::move(message)} ,mLocation{std::move(location)}{}

    virtual const char* what() const  noexcept override{
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
    throw MyexceptionWithLocaton("--->exception<---");
}

int main() {

    try{
        level1();//here
    }
    catch(const MyexceptionWithLocaton & e){
        std::cout<<"MyexceptionWithLocaton:"<<e.what()<<std::endl;
        auto location = e.where();
        std::cout<<"line :"<<location.line()<<std::endl;
        std::cout<<"function_name() :"<<location.function_name()<<std::endl;
        std::cout<<"file_name() :"<<location.file_name()<<std::endl;

    }
    // catch(const std::overflow_error & e){
    //     std::cout<<"std::overflow_error:"<<e.what()<<std::endl;
    // }
    // catch(...){
    //     std::cout<<"std::exception:"<<std::endl;
    // }
    // std::cout<<"after the exception main"<<std::endl;

    return 0;
}



```



##  The code for Catching Exceptions in a Class Hierarchy.

```cpp
//Catching Exceptions in a Class Hierarchy
void throwSomething() {

    throw std::runtime_error("Runtime failure!");
}

int main() {
    try {
        throwSomething();  
    } 
    //const std::exception& e = std::runtime_error("Runtime failure!");
    catch (const std::exception& e) {  // Catch as base class
        std::cout << "Caught: " << e.what() << "\n";
        // e could be invalid_argument or runtime_error, but what() works polymorphically
    }
    return 0;
}


```

