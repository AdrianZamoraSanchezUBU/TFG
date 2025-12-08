int a = 0;

event test(ref int x) every 2 sec {
    int x = x + 1;
    print("X val: ", toString(x));
}

test(ref a);

return 0;