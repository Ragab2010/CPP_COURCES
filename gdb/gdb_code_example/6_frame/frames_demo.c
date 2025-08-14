#include <stdio.h>

void level3(int x) {
    int l3_var = x * 10;
    printf("In level3, l3_var = %d\n", l3_var);
}

void level2(int b) {
    int l2_var = b + 5;
    level3(l2_var);
}

void level1(int a) {
    int l1_var = a - 2;
    level2(l1_var);
}

int main() {
    int main_var = 7;
    level1(main_var);
    return 0;
}

