#include <iostream>
#include <utility> // For std::move

//Resource Acquisition Is Initialization or RAII

class MyClass {
private:
    int* data;          // Pointer to the allocated array
    int numberOfBanks;  // Number of elements in the array

public:
    // Constructor
    MyClass(int numberOfBanks) 
        : data(new int[numberOfBanks]), numberOfBanks(numberOfBanks) {
        for (int i = 0; i < numberOfBanks; ++i) {
            data[i] = i + 1; // Initialize with values 1 to numberOfBanks
        }
    }

    // Copy Constructor
    MyClass(const MyClass& other)
        : data(new int[other.numberOfBanks]), numberOfBanks(other.numberOfBanks) {
        std::cout << "Copy Constructor called\n";
        for (int i = 0; i < numberOfBanks; ++i) {
            data[i] = other.data[i];
        }
    }

    // Copy Assignment Operator
    MyClass& operator=(const MyClass& other) {
        if (this == &other) return *this; // Handle self-assignment
        std::cout << "Copy Assignment Operator called\n";

        // Clean up old resource
        delete[] data;

        // Copy new resource
        numberOfBanks = other.numberOfBanks;
        data = new int[numberOfBanks];
        for (int i = 0; i < numberOfBanks; ++i) {
            data[i] = other.data[i];
        }
        return *this;
    }

    // Move Constructor
    MyClass(MyClass&& other) noexcept 
        : data(other.data), numberOfBanks(other.numberOfBanks) {
        std::cout << "Move Constructor called\n";
        other.data = nullptr;
        other.numberOfBanks = 0;
    }

    // Move Assignment Operator
    MyClass& operator=(MyClass&& other) noexcept {
        if (this == &other) return *this; // Handle self-assignment
        std::cout << "Move Assignment Operator called\n";

        // Clean up old resource
        delete[] data;

        // Transfer ownership
        data = other.data;
        numberOfBanks = other.numberOfBanks;
        other.data = nullptr;
        other.numberOfBanks = 0;
        return *this;
    }

    // Destructor
    ~MyClass() { delete[] data; }

    void print() const {
        if (data) {
            std::cout << "Data: ";
            for (int i = 0; i < numberOfBanks; ++i) {
                std::cout << data[i] << " ";
            }
            std::cout << '\n';
        } else {
            std::cout << "Data: null\n";
        }
    }
};

int main() {
    MyClass obj(5); // Create an object with an array of size 5
    obj.print();

    MyClass moveObj = std::move(obj); // Test move constructor
    moveObj.print();
    obj.print();

    //MyClass copyObj = obj; // Test copy constructor
    // copyObj.print();

    // MyClass assignObj(3); // Create another object
    // assignObj = copyObj;  // Test copy assignment
    // assignObj.print();

    // MyClass moveAssignObj(3); // Create another object
    // moveAssignObj = std::move(copyObj); // Test move assignment
    // moveAssignObj.print();
    // copyObj.print();

    return 0;
}
