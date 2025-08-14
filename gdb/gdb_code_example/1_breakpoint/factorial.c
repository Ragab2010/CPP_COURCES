#include <stdio.h>
#include <stdlib.h>
int global_var = 42;
unsigned int factorial(unsigned int n) {
    if (n == 0) return 1;
    int local_var = n * 2; // For demonstration
    return n * factorial(n - 1);
}
int main(int argc, char* argv[]) {
    unsigned int num = 3; // Default value
    unsigned int result = 0;

    if (argc > 1) {
        num= atoi(argv[1]);
    }
    
    result = factorial(num);
    printf("Factorial of %u is %u\n", num, result);
    return 0;
}