class A{
    int a;
public:
    A(int a){
        this->a = a + 1;
    }
    int get_a(){return a;}
};

int foo(int a){
    A * b = new A(a);
    return b->get_a();
}