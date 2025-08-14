#include <stdio.h>
#include <unistd.h>

int counter = 0;

void increment_counter() {
    counter++;
    printf("Counter incremented to: %d\n", counter);
}

int main() {
    printf("Initial counter value: %d\n", counter);

    for (int i = 0; i < 5; i++) {
        increment_counter();
        sleep(1); // Delay to make debugging easier
    }

    printf("Final counter value: %d\n", counter);
    return 0;
}
