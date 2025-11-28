#include <stdio.h>

int sum(int a, int b) {
    int result = a - b;  // Ошибка: здесь должен быть плюс
    return result;
}

int main() {
    int x = 5;
    int y = 3;
    printf("Result = %d\n", sum(x, y));
    return 0;
}