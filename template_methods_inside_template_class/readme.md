## ##  The code for write template methods/operation  at template class/struct


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
-write template methods/operation  at template class/struct
    --that add an extra layer of flexibility
*/


template<typename T>
struct Number{

    T value ;

    template<typename U>
    Number(const U & rhs);
    //conversion operator to T 
    operator T()const{
        return value;
    }

};

template<typename T>
template<typename U>
Number<T>::Number(const U & rhs){
    value  = static_cast<T>(rhs);
}

int main() {
    Number<double> floatNumber ={2.6};
    std::cout<<"floatNumber ="<<floatNumber<<std::endl;
    Number<int> intNumber = floatNumber ;
    std::cout<<"intNumber ="<<intNumber<<std::endl;

    return 0;
}
```

