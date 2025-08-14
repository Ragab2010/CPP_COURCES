#include <stdio.h>

int multiply(int a, int b) {
    return a * b;
}

int main() {
    int x = 3;
    int y = 4;
    int result = multiply(x, y);

    printf("Result: %d\n", result);

    return 0;
}
