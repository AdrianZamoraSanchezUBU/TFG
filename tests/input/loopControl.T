int a = 0;

while(a < 5) {
    a = a + 1;

    print(intToString(a));

    if(a >= 3){
        break;
    }
}

return a;