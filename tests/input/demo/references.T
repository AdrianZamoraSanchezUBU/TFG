int function foo(ref int x){
    x = 2;
    return x;
}

int param = 1;

// Cambia param a 2
foo(ref param);

return param;