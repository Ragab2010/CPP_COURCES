# Template Instantiation , Template Specialization , Perfect forwarding
---

### 1. **Template Instantiation**

Template instantiation is the process of generating a concrete function or class from a template by substituting the template parameters with actual types or values.

### **Implicit Instantiation**

- The compiler automatically generates the code for a template when it is used in the program.
- This happens when the template is first used with a specific set of template arguments.

Example:

```cpp
template<typename T>
void print(T value) {
    std::cout << value << std::endl;
}

int main() {
    print(42);       // Implicit instantiation of print<int>
    print(3.14);     // Implicit instantiation of print<double>
    return 0;
}

```

Here:

- `print(42)` causes the compiler to implicitly instantiate `print<int>`.
- `print(3.14)` causes the compiler to implicitly instantiate `print<double>`.

### **Explicit Instantiation**

- The programmer explicitly tells the compiler to generate the code for a specific template instantiation, even if it hasn’t been used yet.
- This is useful for reducing compilation times or controlling where the instantiation occurs.

Example:

```cpp
template<typename T>
void print(T value) {
    std::cout << value << std::endl;
}

// Explicit instantiation for int and double
template void print<int>(int);
template void print<double>(double);

int main() {
    print(42);       // Uses the explicitly instantiated print<int>
    print(3.14);     // Uses the explicitly instantiated print<double>
    return 0;
}

```

Here:

- `template void print<int>(int);` explicitly instantiates `print<int>`.
- `template void print<double>(double);` explicitly instantiates `print<double>`.

---

### 2. **Template Specialization**

Template specialization allows you to provide a custom implementation of a template for a specific set of template arguments.

### **Implicit Specialization**

- This is the default behavior of templates. The compiler generates the code for a template based on the provided arguments.
- No custom implementation is provided; the compiler uses the generic template.

Example:

```cpp
template<typename T>
void print(T value) {
    std::cout << "Generic template: " << value << std::endl;
}

int main() {
    print(42);       // Uses the generic template (implicit specialization)
    print(3.14);     // Uses the generic template (implicit specialization)
    return 0;
}

```

Here:

- The generic template is used for both `int` and `double`.

### **Explicit Specialization**

- You provide a custom implementation of the template for a specific set of template arguments.
- This overrides the generic template for the specified arguments.

Example:

```cpp
template<typename T>
void print(T value) {
    std::cout << "Generic template: " << value << std::endl;
}

// Explicit specialization for int
template<>
void print<int>(int value) {
    std::cout << "Specialized template for int: " << value << std::endl;
}

int main() {
    print(42);       // Uses the specialized template for int
    print(3.14);     // Uses the generic template
    return 0;
}

```

Here:

- The specialized version of `print<int>` is used for `int` arguments.
- The generic template is used for `double` arguments.

---

### 3. **Class Template Specialization**

Class templates can also be specialized, either partially or fully.

### **Full Specialization**

- Provide a custom implementation of the class template for a specific set of template arguments.

Example:

```cpp
template<typename T>
class Box {
public:
    void print() {
        std::cout << "Generic Box" << std::endl;
    }
};

// Full specialization for T = int
template<>
class Box<int> {
public:
    void print() {
        std::cout << "Specialized Box for int" << std::endl;
    }
};

int main() {
    Box<double> box1;
    box1.print(); // Uses the generic template

    Box<int> box2;
    box2.print(); // Uses the specialized template for int
    return 0;
}

```

### **Partial Specialization**

- Provide a custom implementation of the class template for a subset of possible template arguments.

Example:

```cpp
template<typename T, typename U>
class Pair {
public:
    void print() {
        std::cout << "Generic Pair" << std::endl;
    }
};

// Partial specialization for when both types are the same
template<typename T>
class Pair<T, T> {
public:
    void print() {
        std::cout << "Partial specialization for Pair<T, T>" << std::endl;
    }
};

int main() {
    Pair<int, double> p1;
    p1.print(); // Uses the generic template

    Pair<int, int> p2;
    p2.print(); // Uses the partial specialization
    return 0;
}

```

---

### 4. **Key Differences**

| **Concept** | **Description** |
| --- | --- |
| **Implicit Instantiation** | Compiler automatically generates code for a template when it is used. |
| **Explicit Instantiation** | Programmer explicitly requests the compiler to generate code for a template. |
| **Implicit Specialization** | Compiler uses the generic template for the provided arguments. |
| **Explicit Specialization** | Programmer provides a custom implementation for a specific set of arguments. |

---

### 5. **When to Use What**

- Use **implicit instantiation** when you want the compiler to handle everything automatically.
- Use **explicit instantiation** to control where and when template code is generated.
- Use **explicit specialization** to provide custom behavior for specific template arguments.
- Use **partial specialization** to provide custom behavior for a subset of template arguments.

---

## perfect forwarding

To demonstrate **perfect forwarding** in a class that inherits from a standard container (e.g., `std::vector`), we can create a custom class that forwards arguments to the underlying container using **universal references** (`T&&`) and `std::forward`. This is a practical example of how to leverage perfect forwarding to pass arguments efficiently to the base class.

Below is an example of a `MyContainer` class that inherits from `std::vector` and uses perfect forwarding to add elements to the vector:

---

### Example: Perfect Forwarding with `std::vector`

```cpp
#include <iostream>
#include <vector>
#include <utility> // for std::forward

// MyContainer inherits from std::vector
template <typename T>
class MyContainer : public std::vector<T> {
public:
    // Inherit constructors from std::vector
    using std::vector<T>::vector;

    // Perfect forwarding push_back
    template <typename U>
    void push_back(U&& value) {
        std::cout << "Forwarding value to std::vector::push_back\\n";
        std::vector<T>::push_back(std::forward<U>(value));
    }

    // Perfect forwarding emplace_back
    template <typename... Args>
    void emplace_back(Args&&... args) {
        std::cout << "Forwarding arguments to std::vector::emplace_back\\n";
        std::vector<T>::emplace_back(std::forward<Args>(args)...);
    }
};

int main() {
    MyContainer<int> container;

    // Add elements using perfect forwarding
    int x = 10;
    container.push_back(x); // lvalue
    container.push_back(20); // rvalue

    // Emplace elements using perfect forwarding
    container.emplace_back(30); // rvalue
    container.emplace_back(x); // lvalue

    // Print the container
    std::cout << "Container contents: ";
    for (const auto& elem : container) {
        std::cout << elem << " ";
    }
    std::cout << "\\n";

    return 0;
}

```

---

### Explanation of the Code:

1. **Inheritance from `std::vector`**:
    - The `MyContainer` class inherits from `std::vector<T>`, where `T` is the type of elements stored in the container.
2. **Perfect Forwarding in `push_back`**:
    - The `push_back` method is templated with `typename U` to accept both lvalues and rvalues.
    - `U&&` is a **universal reference** that can bind to either an lvalue or an rvalue.
    - `std::forward<U>(value)` forwards the value to the base class's `push_back` method, preserving its value category (lvalue or rvalue).
3. **Perfect Forwarding in `emplace_back`**:
    - The `emplace_back` method is templated with `typename... Args` to accept a variadic number of arguments.
    - `Args&&... args` allows forwarding of multiple arguments to the base class's `emplace_back` method.
    - `std::forward<Args>(args)...` forwards each argument while preserving its value category.
4. **Usage in `main`**:
    - The `push_back` method is called with both lvalues (`x`) and rvalues (`20`).
    - The `emplace_back` method is called with both lvalues (`x`) and rvalues (`30`).
    - The container's contents are printed to demonstrate that the elements were added correctly.

---

### Output:

```
Forwarding value to std::vector::push_back
Forwarding value to std::vector::push_back
Forwarding arguments to std::vector::emplace_back
Forwarding arguments to std::vector::emplace_back
Container contents: 10 20 30 10

```

---

### Key Points:

1. **Perfect Forwarding**:
    - `std::forward` is used to preserve the value category (lvalue or rvalue) of the arguments passed to the function.
    - This ensures that resources are moved (for rvalues) or copied (for lvalues) efficiently.
2. **Universal References**:
    - `U&&` in the `push_back` method is a universal reference that can bind to either an lvalue or an rvalue.
3. **Variadic Templates**:
    - `typename... Args` in the `emplace_back` method allows forwarding of multiple arguments.
4. **Inheritance from `std::vector`**:
    - By inheriting from `std::vector`, we can reuse its functionality while extending it with custom methods.

---

### Why This is Useful:

- Perfect forwarding allows you to write generic and efficient code that works with both lvalues and rvalues.
- This is particularly useful when designing container classes or wrapper classes that need to forward arguments to underlying standard containers.

Let me know if you need further clarification or enhancements!

---

In C++, **universal references** (also known as **forwarding references**) and **perfect forwarding** are closely related concepts that rely on **reference collapsing rules**. These rules determine how references to references are resolved in template type deduction.

Let’s break it down step by step:

---

### 1. **Universal References (Forwarding References)**

A **universal reference** is a type of reference that can bind to either an lvalue or an rvalue. It is declared using a double ampersand (`&&`) in a template context, where type deduction takes place.

Example:

```cpp
template<typename T>
void foo(T&& arg); // arg is a universal reference

```

Here, `T&&` can bind to:

- An lvalue of type `T` (e.g., `int x = 10; foo(x);`)
- An rvalue of type `T` (e.g., `foo(10);`)

---

### 2. **Perfect Forwarding**

Perfect forwarding is a technique used to forward arguments to another function while preserving their value category (lvalue or rvalue). It is achieved using `std::forward<T>`.

Example:

```cpp
template<typename T>
void wrapper(T&& arg) {
    // Forward arg to another function, preserving its value category
    some_function(std::forward<T>(arg));
}

```

Here, `std::forward<T>` ensures that:

- If `arg` is an lvalue, it is passed as an lvalue.
- If `arg` is an rvalue, it is passed as an rvalue.

---

### 3. **Reference Collapsing Rules**

When dealing with universal references and perfect forwarding, **reference-collapsing** rules come into play. These rules determine what happens when you have references to references (e.g., `T& &`, `T&& &`, etc.).

The rules are as follows:

| **Reference Type 1** | **Reference Type 2** | **Resulting Type** |
| --- | --- | --- |
| `T&` | `&` | `T&` |
| `T&` | `&&` | `T&` |
| `T&&` | `&` | `T&` |
| `T&&` | `&&` | `T&&` |

In simpler terms:

- If either reference is an lvalue reference (`&`), the result is an lvalue reference (`&`).
- Only if both references are rvalue references (`&&`) does the result remain an rvalue reference (`&&`).

---

### 4. **How Reference Collapsing Works in Practice**

Consider the following example:

```cpp
template<typename T>
void foo(T&& arg) {
    // arg is a universal reference
    // T can be deduced as either an lvalue reference or an rvalue reference
}

int x = 10;
foo(x);  // T is deduced as int& (lvalue reference)
foo(10); // T is deduced as int&& (rvalue reference)

```

When `foo(x)` is called:

- `T` is deduced as `int&` (because `x` is an lvalue).
- The type of `arg` becomes `int& &&`, which collapses to `int&`.

When `foo(10)` is called:

- `T` is deduced as `int&&` (because `10` is an rvalue).
- The type of `arg` becomes `int&& &&`, which collapses to `int&&`.

---

### 5. **Why Reference Collapsing Matters**

Reference collapsing is essential for making universal references and perfect forwarding work correctly. Without it, you wouldn’t be able to preserve the value category (lvalue or rvalue) of the forwarded arguments.

For example:

```cpp
template<typename T>
void wrapper(T&& arg) {
    // Without reference collapsing, this wouldn't work correctly
    some_function(std::forward<T>(arg));
}

```

Here, `std::forward<T>` relies on reference collapsing to ensure that:

- If `arg` is an lvalue, `std::forward<T>` returns an lvalue reference.
- If `arg` is an rvalue, `std::forward<T>` returns an rvalue reference.

---

### 6. **Summary of Reference Collapsing Rules**

| **Expression** | **Collapsed Type** |
| --- | --- |
| `T& &` | `T&` |
| `T& &&` | `T&` |
| `T&& &` | `T&` |
| `T&& &&` | `T&&` |

---

### 7. **Key Takeaways**

- Universal references (`T&&`) can bind to both lvalues and rvalues.
- Reference collapsing rules simplify references to references into a single reference.
- Perfect forwarding (`std::forward<T>`) relies on reference collapsing to preserve the value category of arguments.
