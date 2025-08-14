#include <stdio.h>

void step3() {
    printf("Step 3 start\n");
    printf("Step 3 middle\n");
    printf("Step 3 end\n");
}

void step2() {
    printf("Step 2 start\n");
    step3();
    printf("Step 2 end\n");
}

void step1() {
    printf("Step 1 start\n");
    step2();
    printf("Step 1 end\n");
}

int main() {
    step1();
    return 0;
}

