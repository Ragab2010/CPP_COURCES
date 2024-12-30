#include <iostream>
#include <memory> // For std::unique_ptr
using namespace std;

class MyVector {
private:
    unique_ptr<int[]> data; // Use unique_ptr for automatic memory management
    size_t size;

public:
    // Default Constructor
    MyVector() {
        data = nullptr;
        size = 0;
    }

    // Parameterized Constructor
    MyVector(size_t n) {
        data = make_unique<int[]>(n);
        size = n;
    }

    // Copy Constructor (deleted because unique_ptr does not allow copying)
    MyVector(const MyVector& other) = delete;

    // Move Constructor
    MyVector(MyVector&& other) noexcept {
        data = move(other.data); // Transfer ownership
        size = other.size;
        other.size = 0;          // Reset the source object's size
        cout << "Move Constructor called\n";
    }

    // Copy Assignment Operator (deleted because unique_ptr does not allow copying)
    MyVector& operator=(const MyVector& other) = delete;

    // Move Assignment Operator
    MyVector& operator=(MyVector&& other) noexcept {
        if (this == &other) return *this; // Handle self-assignment

        data = move(other.data); // Transfer ownership
        size = other.size;
        other.size = 0;          // Reset the source object's size
        cout << "Move Assignment Operator called\n";
        return *this;
    }

    // Destructor (automatically handled by unique_ptr)
    ~MyVector() = default;

    // Size accessor
    size_t getSize() const { return size; }

    // Index operator
    int& operator[](size_t index) {
        if (!data) throw runtime_error("Accessing invalid data.");
        return data[index];
    }
    const int& operator[](size_t index) const {
        if (!data) throw runtime_error("Accessing invalid data.");
        return data[index];
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

    // Print function
    void print() const {
        if (data) {
            for (size_t i = 0; i < size; ++i) {
                cout << data[i] << " ";
            }
            cout << endl;
        } else {
            cout << "Vector is empty or ownership transferred." << endl;
        }
    }
};

int main() {
    MyVector vec1(5);
    for (size_t i = 0; i < vec1.getSize(); ++i) {
        vec1[i] = static_cast<int>(i + 1);
    }

    cout << "Vector 1: ";
    vec1.print();

    MyVector vec2 = move(vec1); // Transfer ownership using move semantics
    cout << "After moving:\n";
    cout << "Vector 1: ";
    vec1.print(); // Safe even after ownership transfer
    cout << "Vector 2: ";
    vec2.print();

    return 0;
}
