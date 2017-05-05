int foo(int a){
    static int b = 1;
    b += a;
    return b;
}