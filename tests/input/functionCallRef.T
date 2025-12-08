int function foo(ref int x){
    x = 2;
    return x;
}

int param = 1;

foo(ref param);

return param;