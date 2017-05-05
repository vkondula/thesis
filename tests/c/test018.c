int foo(int a){
    int b = 0;
    for (int i = 0; i < a; i++){
        b += i;
        if (b > 10) break;
    }
    return b;
}