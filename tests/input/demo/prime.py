def isPrime(x):
    if x < 2:
        return False

    if x == 2:
        return True

    d = 2

    while d * d <= x:
        if x % d == 0:
            return False
        d = d + 1

    return True


def calcPrimes(n):
    i = 2
    c = 0

    while c <= n:
        if isPrime(i):
            print(i)
            c = c + 1
        i = i + 1


calcPrimes(1000000)
