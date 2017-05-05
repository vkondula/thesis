struct bar {
    int a;
    char b;
    double c;
};

typedef struct bar foobar;

int foo(int a){
    foobar d = {.a=3, .b='c'};
    d.c = a + 1.0;
    return (int)(d.c + d.a);
}