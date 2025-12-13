int a = 1;

event test(int x) every 2 sec {
    print(toString(x));
}

test(2);

return 0;