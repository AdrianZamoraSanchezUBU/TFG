void function foo(ref int x){
    x = 2;
    return;
}

int param = 1;

foo(ref param);

// Now param value is 2
return param;