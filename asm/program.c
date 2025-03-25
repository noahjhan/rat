#include <stdio.h>

int foo(int x, int f) {
    printf("here\n");
    return x;
}

void bar() {

}

int main () {
    int y = 10;
    int z = 2;
    bar();
    foo(y, z);
    return 0;
}