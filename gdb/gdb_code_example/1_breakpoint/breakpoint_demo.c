#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}

int main() {
    int x = 5, y = 3;
    int sum = add(x, y);
    int product = multiply(x, y);

    printf("Sum: %d\n", sum);
    printf("Product: %d\n", product);

    return 0;
}

