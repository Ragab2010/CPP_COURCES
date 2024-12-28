#include <iostream>
#include <vector>

using namespace std;
// Custom vector-like class
class MyVector {
private:
    int* data;
    size_t size;

public:
    // Default Constructor
    MyVector() {
        data = nullptr;
        size = 0;
    }

    // Parameterized Constructor
    MyVector(size_t n) {
        data = new int[n];
        size = n;
    }

    // Copy Constructor
    MyVector(const MyVector& other) {
        size = other.size;
        data = new int[size];
        for (size_t i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
        cout << "Copy Constructor called\n";
    }

    // Move Constructor
    MyVector(MyVector&& other) noexcept {
        data = other.data;
        size = other.size;
        other.data = nullptr;
        other.size = 0;
        cout << "Move Constructor called\n";
    }

    // Copy Assignment Operator
    MyVector& operator=(const MyVector& other) {
        if (this == &other) return *this; // Handle self-assignment

        delete[] data; // Clean up existing resource

        size = other.size;
        data = new int[size];
        for (size_t i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
        cout << "Copy Assignment Operator called\n";
        return *this;
    }

    // Move Assignment Operator
    MyVector& operator=(MyVector&& other) noexcept {
        if (this == &other) return *this; // Handle self-assignment

        delete[] data; // Clean up existing resource

        data = other.data;
        size = other.size;

        other.data = nullptr;
        other.size = 0;
        cout << "Move Assignment Operator called\n";
        return *this;
    }

    // Destructor
    ~MyVector() {
        delete[] data;
    }

    // Size accessor
    size_t getSize() const { return size; }

    // Index operator
    int& operator[](size_t index) { return data[index]; }
    const int& operator[](size_t index) const { return data[index]; }

    // Overloaded operator<< for printing
    friend ostream& operator<<(ostream& os, const MyVector& vec) {
        for (size_t i = 0; i < vec.size; ++i) {
            os << vec[i] << " ";
        }
        return os;
    }
    // Print function
    void print() const {
        for (size_t i = 0; i < size; ++i) {
            cout << data[i] << " ";
        }
        cout << endl;
    }
};

// Function to allocate and initialize MyVector
MyVector allocateArrayMyVector(int numberOfLocations) {
    MyVector myVec(numberOfLocations);
    for (int i = 0; i < numberOfLocations; ++i) {
        myVec[i] = i + 1; // Initialize from 1 to numberOfLocations
    }
    // return myVec; // Return by value (leverages move semantics)
    return std::move(myVec);
}

// Function to allocate an array using `new` and initialize it
int* allocateArrayNew(int numberOfLocations) {
    /*
    ptr is a lvalue that hold an address for the memory location; 
    this the memory location contain the address for {5 locations} located at the heap.
    */
    int* ptr = new int[numberOfLocations]; 
    for (int i = 0; i < numberOfLocations; ++i) {
        ptr[i] = i + 1; // Initialize from 1 to numberOfLocations
    }
    //----------------
    return ptr;//return lvalue //-->{1,2,3,4,5}
    /*
    -no copy(there is no duplicate for {5 locations})
    -translate the Address of {5 locations}
    */
}

// Function to allocate and initialize a vector
vector<int> allocateArrayVector(int numberOfLocations) {
    /*
    vec is a lvalue that hold an address for the memory location; 
    this the memory location contain member has the address for {5 location} located at the heap.
    */
    vector<int> vec(numberOfLocations , 0);
    for (int i = 0; i < numberOfLocations; ++i) {
        vec[i] = i + 1; // Initialize from 1 to numberOfLocations
    }
    //vec = {1,2,3,4,5} 
    return vec;//return lvalue

    /*
    -we have a copy(there is  duplicate for {5 locations})  
    -there is No translate for Ownership {5 locations}
    */

    return std::move(vec); //return rvalue
    /*
    -No copy(there is no duplicate for {5 locations}) , translate for Ownership {5 locations}
    */
}//delete

int main() {
    int numberOfLocations = 5;

    // Using the function with `new`
    int* array = allocateArrayNew(numberOfLocations);
    cout << "Array allocated with new: ";
    for (int i = 0; i < numberOfLocations; ++i) {
        cout << array[i] << " ";
    }
    cout << endl;
    delete[] array; // Don't forget to free memory!



    // Using the function with `vector`
    vector<int> returnVec = allocateArrayVector(numberOfLocations);
    cout << "Array allocated with vector: ";
    for (int val : returnVec) {
        cout << val << " ";
    }
    cout << endl;

    // Using the function with `MyVector`
    // MyVector myVec = allocateArrayMyVector(numberOfLocations);
    // cout << "Array allocated with MyVector: ";
    // myVec.print();

    return 0;
}






/*

class MyVector {
private:
    int* data;
    size_t size;

public:
    // Default Constructor
    MyVector() : data(nullptr), size(0) {}

    // Parameterized Constructor
    MyVector(size_t n) : data(new int[n]), size(n) {}

    // Copy Constructor
    MyVector(const MyVector& other) : data(new int[other.size]), size(other.size) {
        for (size_t i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
        cout << "Copy Constructor called\n";
    }

    // Move Constructor
    MyVector(MyVector&& other) noexcept : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
        cout << "Move Constructor called\n";
    }

    // Copy Assignment Operator
    MyVector& operator=(const MyVector& other) {
        if (this == &other) return *this; // Handle self-assignment

        delete[] data; // Clean up existing resource

        size = other.size;
        data = new int[size];
        for (size_t i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
        cout << "Copy Assignment Operator called\n";
        return *this;
    }

    // Move Assignment Operator
    MyVector& operator=(MyVector&& other) noexcept {
        if (this == &other) return *this; // Handle self-assignment

        delete[] data; // Clean up existing resource

        data = other.data;
        size = other.size;

        other.data = nullptr;
        other.size = 0;
        cout << "Move Assignment Operator called\n";
        return *this;
    }

    // Destructor
    ~MyVector() {
        delete[] data;
    }

    // Size accessor
    size_t getSize() const { return size; }

    // Index operator
    int& operator[](size_t index) { return data[index]; }
    const int& operator[](size_t index) const { return data[index]; }

    // Overloaded operator<< for printing
    friend ostream& operator<<(ostream& os, const MyVector& vec) {
        for (size_t i = 0; i < vec.size; ++i) {
            os << vec[i] << " ";
        }
        return os;
    }
    // Print function
    void print() const {
        for (size_t i = 0; i < size; ++i) {
            cout << data[i] << " ";
        }
        cout << endl;
    }
};

*/