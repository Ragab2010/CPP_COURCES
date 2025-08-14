#include <stdio.h>
#include <string.h>

struct Person {
    char name[20];
    int age;
};

int main() {
    int num = 42;
    float pi = 3.14159;
    char ch = 'A';
    char str[] = "Hello, GDB!";
    int arr[] = {10, 20, 30, 40, 50};

    struct Person p1;
    strcpy(p1.name, "Alice");
    p1.age = 30;

    int *ptr = arr; // pointer to first element of arr

    printf("Variables are initialized.\n");
    return 0;
}

