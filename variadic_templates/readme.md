##  The code of Variadic  templates

```cpp
#include <iostream>
#include <concepts>
#include <type_traits>

/*C++ templates(compile time) topics:
-template Instantiation , template Specialization
-write function(with template parameter) / function(with non-template parameter) 
-write class(with template parameter) / class(with non-template parameter) 
-function overloading vs partial template specialization
-write template class method
-template recursion
-Variadic  templates , Variadic  args
-class template with header and source file
-enable_if , is_integral , ()
-SFINE
-Concepts
*/

/*
Variadic  templates , Variadic  arguments,  variable parameters (...) 
-Variadic templates: template can take a variable number of template parameters. 
-a template parameter pack :is something(placeholder) that can accept 
    a variable number of arguments.

-Packing:  refers to the process of collecting multiple arguments 
    into a parameter pack. 
-Unpacking: refers to the process of expanding a parameter pack 
    into individual elements. This is done using the ... operator.
notes
- ...placeholder(arg) --->packing
- placeholder(arg)... --->Unpacking

*/
/*
write print function template that take an input Variadic  arguments
*/

// template<typename ... ARG>//ARG: a template parameter pack
// void print(ARG ... arg){

// }
/*
how can unpacking parameters from  a template parameter pack (arg)?
    1-by using recursive call and consume one parameter every call
    2- fold expr
*/

/*
 1-by using recursive call and consume one parameter every call
*/
/*
how can handle the empty call  print();
    a- declate empty print(){}
    b-count number of the parameters , 
        if the number of  the parameters greater than zero we can call
*/
//a- declate empty print(){}
void printWithEmptyCall(){}
template<typename T , typename ... ARG>//ARG: a template parameter pack
void printWithEmptyCall(T init , ARG ... arg){
    std::cout<<init<<std::endl;
    printWithEmptyCall(arg...);//unpacking 
    /*
    printWithEmptyCall(1 , "welcome" , 2.6);
    printWithEmptyCall("welcome" , 2.6);
    printWithEmptyCall( 2.6);
    printWithEmptyCall();--->call void printWithEmptyCall(){}
    */
}
//a- count number of the parameters
template<typename T , typename ... ARG>//ARG: a template parameter pack
void printWithSizeof(T init , ARG ... arg){
    std::cout<<init<<std::endl;
    if constexpr(sizeof...(ARG) > 0)
        printWithSizeof(arg...);//unpacking 
    /*
    printWithSizeof(1 , "welcome" , 2.6);
    printWithSizeof("welcome" , 2.6);
    printWithSizeof( 2.6);
    */
}

/*
-forld experssions:
Types of Fold Expressions:
Unary Left Fold: (op ... args)
Unary Right Fold: (args op ...)
Binary Left Fold: (init op ... args)
Binary Right Fold: (args op ... init)
*/
//unpacking with Binary Left Fold , Binary Right Fold
template<typename T , typename ... ARG>
void printWithBinary(T init , ARG ... arg){
    //Binary Left Fold
    // ((std::cout<<init<<std::endl) , ...  , (std::cout<<arg<<std::endl));
    //Binary Right Fold
    ((std::cout<<arg<<std::endl) , ...  , (std::cout<<init<<std::endl));
}


//unpacking with Unary Left Fold , Unary Right Fold
template< typename ... ARG>
void printWithUnary( ARG ... arg){
    //Unary Left Fold
    // ( ...  , (std::cout<<arg<<std::endl));
    //Unary Right Fold
    ((std::cout<<arg<<std::endl) , ... );
}

/*
write sum function template with  Variadic  arguments
*/
/*sum function template with init value and  Variadic  arguments.
    -unpacking with Binary Left Fold , Binary Right Fold
*/
template<typename  T , typename ...ARG>
T sumInitBinary(T init , ARG ... arg){
    //Binary Left Fold
    // return (init + ... +arg );
    //Binary Right Fold 
    return (arg + ... +init );
}

/*sum function template with  Variadic  arguments.
    -unpacking with Binary Left Fold , Binary Right Fold
*/
template< typename ...ARG>
auto  sumUnary( ARG ... arg){
    //Binary Left Fold
    return ( ... +arg );
    //Binary Right Fold 
    // return (arg + ...  );
}

/********************************/


/*
write sum function template with  Variadic  arguments , 
with constrain that the args have to be integral values
*/

/*
write sum function template with  Variadic  arguments , 
with constrain that the args have to be integral values
    -using enable_if
*/
template<typename  T , typename ...ARG>
std::enable_if_t<std::is_integral_v<T>, T>
sumInitBinaryEnable_if(T init , ARG ... arg){
    //Binary Left Fold
    // return (init + ... +arg );
    //Binary Right Fold 
    return (arg + ... +init );
}

/*
write sum function template with  Variadic  arguments , 
with constrain that the args have to be integral values
    -using concept 
*/
template<std::integral  T , std::integral ...ARG>
auto sumInitBinaryConcept(T init , ARG ... arg){
    //Binary Left Fold
    // return (init + ... +arg );
    //Binary Right Fold 
    return (arg + ... +init );
}


/*
write sum function template with  Variadic  arguments , 
with constrain that the args have to be integral values
    -using enable_if 
*/
template< typename ...ARG>
std::enable_if_t<(std::is_integral_v<ARG> && ... ), int>
 sumUnaryEnable_if( ARG ... arg){
    //Binary Left Fold
    return ( ... +arg );
    //Binary Right Fold 
    // return (arg + ...  );
}

/*
write sum function template with  Variadic  arguments , 
with constrain that the args have to be integral values
    -using concept  
*/
template< std::integral ...ARG>
auto  sumUnaryConcept( ARG ... arg){
    //Binary Left Fold
    return ( ... +arg );
    //Binary Right Fold 
    // return (arg + ...  );
}


int main() {
    // printWithUnary(1 , "welcome" , 2.6);
    std::cout<<sumUnaryConcept(1,2,3,4,5,6)<<std::endl;
    return 0;
}




```
---
