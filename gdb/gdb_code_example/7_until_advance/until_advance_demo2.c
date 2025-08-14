#include<stdio.h>
void foo() {
    for (int i = 0; i < 5; i++) {
        printf("i = %d\n", i);  // Line 5
    	printf("foo loop\n");
    }
}

int main() {
    foo();                     // Line 10
    printf("done\n");          // Line 11
}

