int foo(int a){
    if (a > 0) return foo(a - 1);
    else return 0;
}