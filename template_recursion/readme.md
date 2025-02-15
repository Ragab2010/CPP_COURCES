##  The code for Template recursion


```cpp
#include <iostream>
#include <concepts>
#include <type_traits>
#include <vector>

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
(Template recursion):
-Recursion: is when a function or template calls itself repeatedly 
            until it reaches a stopping condition (called the base case).
-Template recursion: a template instantiates itself with different parameters, 
                    often reducing the problem size step by step until it reaches 
                    the base case.

-Template recursion: defining a template to instantiate itself 
    with different template arguments until a base case is reached. 
    This is similar to recursion in functions but happens at compile time.

-template recursion cases is: 
    --Recursive Case: the Template that perform computations during compile time 
                    rather than at runtime.
    --Base Case: Similar to function recursion, 
              template recursion requires a base case to terminate the recursive instantiation.
 
*/

/*
-write the Recursive function to compute factorial for N
*/
unsigned long long factorial(int n ){
    //base case 
    if(n ==0 || n == 1){
        return 1;
    }
    
    //general formula 
    return n *factorial(n - 1 );
}

/*
-Write a Factorial class or struct to compute the factorial of N at compile time.
*/

//--Recursive Case
template<int N>
struct Factorial{
    constexpr static unsigned long long value = N * Factorial<N-1>::value;
};
//--Base case 
template<>
struct Factorial<0>{
    constexpr static unsigned long long value =1;
};
/*
-wrie a sum function template that take inputs as  variadic args and return the sum .
*/
auto sum(){return 0;}
template<typename T , typename ... ARG>
auto sum(T head  , ARG ... arg){
    return head + sum(arg...) ;
    /*
    sum(1,2,3,4,5)
    sum(2,3,4,5)
    sum(3,4,5)
    sum(3,4,5)
    sum(4,5)
    sum(5)
    sum()--->
    */

}


/*
-Write a Sum class or struct to compute the sum of variadic args at compile time.
sum<1,2,3,4,5>
-head---> 1
-tail---> 5
-var args--->2,3,4
*/
//general 
template<int ... ARG>
struct Sum;

//--Recursive Case
template<int head ,int ... ARG >
struct Sum<head , ARG...>{
    constexpr static long long value = head + Sum<ARG...>::value;
};

//--Base Case
template<int tail>
struct Sum<tail>{
    constexpr static long long value =tail;
};

//alias for  Sum<1,2,3,4,5>::value -->  Sum_v<1,2,3,4,5>
template<int ... ARG>
constexpr static long long Sum_v=  Sum<ARG...>::value ;

//alias for Factorial<5>::value -->  Factorial_v<5>
template<int N>
constexpr static long long Factorial_v= Factorial<N>::value;

int main() {
    std::cout<<"factorial function: "<< factorial(5)<<std::endl;
    std::cout<<"Factorial struct  : "<< Factorial_v<5><<std::endl;
    std::cout<<"sum       function: "<< sum(1,2,3,4,5)<<std::endl;
    std::cout<<"sum       struct  : "<< Sum_v<1,2,3,4,5><<std::endl;
    return 0;
}

```

