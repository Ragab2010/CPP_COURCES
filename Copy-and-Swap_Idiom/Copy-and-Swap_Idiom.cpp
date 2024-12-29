#include <iostream>
#include <stdexcept>
using namespace std;
/*
    copy-and-swap idiom. This idiom ensures strong exception safety and clean code 
    by centralizing resource management in a single swap function
*/
class MyVector {
private:
    int* data;  // Raw pointer to the dynamic array
    size_t size;

public:
    // Default Constructor
    MyVector() : data(nullptr), size(0) {}

    // Parameterized Constructor
    MyVector(size_t n) : data(new int[n]()), size(n) {}

    // Copy Constructor
    MyVector(const MyVector& other) : data(new int[other.size]), size(other.size) {
        for (size_t i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
        cout << "Copy Constructor called\n";
    }

    // Move Constructor
    MyVector(MyVector&& other) noexcept : data(nullptr), size(0) {
        swap(*this, other); // Use swap to transfer ownership
        cout << "Move Constructor called\n";
    }

    // Copy-and-Swap Assignment Operator
    //here we use MyVector other to make other as temp object from MyVector for safety
    MyVector& operator=(MyVector other) {
        data=nullptr;
        size=0;
        cout << "Copy-and-Swap Assignment Operator called\n";
        swap(*this, other);
        return *this;
    }

    // Destructor
    ~MyVector() {
        delete[] data;
    }

    // Size accessor
    size_t getSize() const { return size; }

    // Index operator
    int& operator[](size_t index) {
        if (!data || index >= size) throw out_of_range("Index out of bounds");
        return data[index];
    }

    const int& operator[](size_t index) const {
        if (!data || index >= size) throw out_of_range("Index out of bounds");
        return data[index];
    }

    // Swap function for copy-and-swap idiom
    friend void swap(MyVector& first, MyVector& second) noexcept {
        using std::swap;
        swap(first.data, second.data);
        swap(first.size, second.size);
    }

    // Overloaded operator<< for printing
    friend ostream& operator<<(ostream& os, const MyVector& vec) {
        if (vec.data) {
            for (size_t i = 0; i < vec.size; ++i) {
                os << vec.data[i] << " ";
            }
        } else {
            os << "Vector is empty or ownership transferred.";
        }
        return os;
    }
};

int main() {
    MyVector vec1(5);
    for (size_t i = 0; i < vec1.getSize(); ++i) {
        vec1[i] = static_cast<int>(i + 1);
    }

    cout << "Vector 1: " << vec1 << endl;

    MyVector vec2(3);
    vec2 = vec1; // Copy Assignment
    cout << "Vector 2 (after copying from vec1): " << vec2 << endl;

    MyVector vec3(4);
    vec3 = std::move(MyVector(6)); // Move Assignment
    cout << "Vector 3 (after moving temporary object): " << vec3 << endl;

    return 0;
}
