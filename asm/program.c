#include <stdio.h>

void printer() {
    printf("help me\n");
    printer();
    return;
}

int main () {
    printer();
    return 0;
}