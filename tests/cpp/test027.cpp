struct bar {
    int a;
    char b;
    double c;
};

int foo(int a){
    struct bar d = {.a=3, .b='c'};
    d.c = a + 1.0;
    return (int)(d.c + d.a);
}