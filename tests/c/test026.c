int foo(int a) {
    int b = 0;
    for (int i = 0; i < a; i++){
        for (int j = 0; j < a; j++){
            b++;
        }
    }
    return b;
}