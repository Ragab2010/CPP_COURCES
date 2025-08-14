#include <stdio.h>
#include <stdlib.h>
void crash_me(int *ptr) {
    int *newptr = ptr;
    *newptr = 42;        // Writing to NULL → segmentation fault
}

void intermediate_function() {
    int *ptr = (int *)malloc(1<<31);
    crash_me(ptr);
    free(ptr);
}

int main() {
    printf("Program started...\n");
    intermediate_function();
    printf("Program finished.\n");
    return 0;
}


