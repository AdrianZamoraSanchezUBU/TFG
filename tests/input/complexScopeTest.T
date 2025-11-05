int function foo(int x){
    int function foo2(){
        return 2;
    }

    int a = 1;

    foo2();

    return x;
}

int param = 1;

foo(param);