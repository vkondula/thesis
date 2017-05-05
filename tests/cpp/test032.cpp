class A{
protected:
    int a;
public:
    A(int a){
        this->a = a + 1;
    }
    int get_a(){return a;}
};

class B : public A{
public:
    B(int a) : A(a){
        this->a = a + 3;
    }
};

int foo(int a){
    A * b = new B(a);
    return b->get_a();
}