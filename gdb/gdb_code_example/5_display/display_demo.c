#include <stdio.h>
//"Hey, automatically show me this thing every time the program stops."
int main() {
    int counter = 0;

    for (int i = 0; i < 5; i++) {
        counter += i;
        //printf("i = %d, counter = %d\n", i, counter);
    }

    return 0;
}
