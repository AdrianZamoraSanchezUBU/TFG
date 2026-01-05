bool function isPrime(int x) {
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

void function calcPrimes(int n){
    int i = 2;

    while (i <= n) {
        if (isPrime(i)) {
            print(intToString(i));
        }
        i = i + 1;
    }
}

calcPrimes(100);

return 0;
