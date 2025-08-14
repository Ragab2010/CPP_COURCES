// file: bp_commands.c
#include <stdio.h>

void process_data(int n) {
    printf("Processing value: %d\n", n);
}

int main() {
    for (int i = 1; i <= 5; i++) {
        process_data(i);
    }
    return 0;
}
