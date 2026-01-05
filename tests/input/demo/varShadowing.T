string a = "global scope";
print(a);

int function foo(){
    // Esta "a" no es como la de afuera
    string a = "foo scope";

    print(a);

    return 1;
}
foo();

print(a);

return 0;