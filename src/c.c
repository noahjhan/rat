#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("nice");
    void *ptr = malloc(1024);
    free(ptr);
    exit(1);
}