int a = 0;

for(int i = 0; i <= 10; i = i + 2){
    // Se excluye el 0, aunque técnicamente es par...
    if(i != 0){
        print(intToString(i));
    }
}

return a;