#include <stdio.h>

enum test_enum { A, B, C };

int main() {
    printf("%d", test_enum::A);
}