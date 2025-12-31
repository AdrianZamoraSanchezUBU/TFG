time t = 2 sec;

event test(int x) every t {
    print("execution: ", intToString(x));
}

test(2);

return 0;