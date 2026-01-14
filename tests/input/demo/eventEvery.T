time t = 2 sec;

event test(int x) every t limit 3 {
    print("Esto es el ejemplo nº", 
           intToString(x), 
           "💯👍");
}

test(3);

return 0;