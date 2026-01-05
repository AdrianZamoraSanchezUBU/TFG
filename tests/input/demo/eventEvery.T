time t = 2 sec;

event test(int x) every t limit 4 {
    print("Esto es el ejemplo nº", intToString(x), "💯👍");
}

test(4);

return 0;