int a = 0;
int x = 100000;

// Bucle infinito, muestra los números pares hasta x.
while(true) {
    print(intToString(a));

    a = a + 2;

    if(a > x){
        break;
    }
}

return a;