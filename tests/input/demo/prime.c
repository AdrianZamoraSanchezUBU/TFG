#include <stdio.h>
#include <stdbool.h>

bool isPrime(int x) {
    if (x < 2) {
        return false;
    }

    if (x == 2) {
        return true;
    }

    int d = 2;

    while (d * d <= x) {
        if (x % d == 0) {
            return false;
        }
        d = d + 1;
    }

    return true;
}

void calcPrimes(int n) {
    int i = 2;
    int c = 0;

    while (c <= n) {
        if (isPrime(i)) {
            printf("%d\n", i);
            c++;
        }
        i = i + 1;
    }
}

int main() {
    calcPrimes(1000000);
    return 0;
}