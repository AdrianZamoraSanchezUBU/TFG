int a;

event test(int x) every 2 sec {
    int b = x + 5;
    printf("%s %d", "test", b);
}

test(2);

return 0;