#include <stdio.h>

int value = 3;

void testfunc() {
    char* anotherVal = "blah";
    printf("Hello world %d %s\n", value, anotherVal);
}

int main() {
    testfunc();
    return 0;
}