void function foo(ptr int x){
    x = 2;
    return;
}

ptr int param = 1;

foo(param);

// Now param's value is 2
return param;