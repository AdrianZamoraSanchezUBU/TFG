int a;

event test every 2 sec {
    a = a + 1;
    printf("%s %d", "test", a);
}

return 0;