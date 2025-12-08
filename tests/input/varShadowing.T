string a = "global scope";
print(a);

int function foo(){
    string a = "foo scope";

    print(a);

    return a;
}

foo();

print(a);

return 0;