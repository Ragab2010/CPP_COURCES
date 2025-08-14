#include <stdio.h>

void process(int value) {
    printf("Processing value: %d\n", value);
}

int main() {
    for (int i = 0; i < 10; i++) {
        process(i);
    }
    return 0;
}
