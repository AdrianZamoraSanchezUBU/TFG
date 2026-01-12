int fin = 10;

// Muestra los pares
for(int i = 0; i <= fin; i = i + 2){
    // Excluye el 0, aunque sea par 
    if(i != 0){
        print(intToString(i));
    }
}

return 0;