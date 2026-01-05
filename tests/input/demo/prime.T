int n = 200000;
int count = 0;
int candidate = 2;
int lastPrime = 2;

while (count < n) {
    bool prime = true;

    // Descartar pares > 2
    if (candidate > 2) {
        if (candidate % 2 == 0) {
            prime = false;
        }
    }

    if (prime) {
        int d = 3;
        
        // Solo hasta d^2
        while (d * d <= candidate) {
            if (candidate % d == 0) {
                prime = false;
                break;
            }

            d = d + 2; // solo impares
        }
    }

    if (prime) {
        lastPrime = candidate;
        count = count + 1;
    }

    candidate = candidate + 1;
}

print(
    "El número primo más grande entre los primeros ",
    intToString(n),
    " números primos es: ",
    intToString(lastPrime)
);

return 0;