string a = "global scope";
print(a);

void function foo(){
    // Esta "a" no es como la global
    string a = "foo scope";

    print(a);
}
foo();

print(a);

return 0;