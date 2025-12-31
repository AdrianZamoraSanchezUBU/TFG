string a = "ejemplo";

event test(ref string x) every 2 sec {
    print(x);
}

test(ref a);

return 0;