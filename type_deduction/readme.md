##  The code of Type deduction

```cpp
#include <iostream>
#include <concepts>
#include <type_traits>
#include <vector>

/*--> C++ Type Deduction <--
Template instantiation:
    --is the process by which the compiler generates specific, 
        concrete code from a template definition when it’s needed for a particular type.
        Template instantiation types : Implicit Instantiation - Explicit Instantiation.
C++ Type Deduction : we can say it act as ( part of implicit instantiation).
    --is a powerful feature that allows the compiler to automatically deduce 
        the type of a variable, function return values, or template parameters without explicit specification/instantiation from the programmer
    --It simplifies code, improves readability, and reduces redundancy

    --Template Implicit instantiation = , Type Deduction + Instantiation code version(generate the version).

-auto keyword, allow the compiler to:
    --Deduces the type of a variable/Object based on its initializer.
    --removes references and const/volatile qualifiers.
    --Deduces the Pointer type.

-decltype(expr) keyword, allow the compiler to:
    --Deduces the exact type of an expression.
    --including references and const/volatile qualifiers.

-Class/Template type deduction allows (at parameters/return), the compiler to:
    --Deduce the type of template parameters.

-Universal References (Forwarding References)

-C++20 Syntactic sugar with Concepts , SFINE.
*/

template <typename T>
void func(T&& param) {
    std::cout << param << std::endl;
}

//C++20 Syntactic sugar with Concepts , SFINE.

decltype(auto) sum(auto a , auto b){
    return a+b;
}
//SFINE

auto sumEnable_if(auto a , auto b)->std::enable_if_t<std::is_integral_v<decltype(a +b)> ,decltype(a +b)>{
    return a+b;
}

//concepts
auto sumConcept(std::integral auto a , std::integral auto b){
    return a+b;
}

int main() {

    std::cout << sumEnable_if(1,5) << std::endl;
    std::cout << sumConcept(1,3) << std::endl;

    //value
    int value = 10;
    auto autoValue= value;
    decltype(value) decltypeValue = value;

    //const value
    const int constValue = 15;
    auto autoConstValue = constValue;
    decltype(constValue) decltypeConstValue = constValue;

    //poiner
    int * ptr = &value;
    auto autoPtr= ptr;
    decltype(ptr) decltypePtr = ptr;

    //lvalue ref
    int & lref = value;
    auto autolref= lref;
    decltype(lref) decltypelref = lref;
    //declare l value reference 
    decltype((value)) decltypelref2 = lref;
    //declare r value reference
    decltype(std::move(value)) decltyperref= std::move(value);
    
    //
    decltype(auto) all = lref;
    

    return 0;
}



```
---
