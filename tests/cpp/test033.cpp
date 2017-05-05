class A{
protected:
    int a;
public:
    A(int a){
        this->a = a + 1;
    }
    int get_a(){return a;}
};

class C{
protected:
    int c;
public:
    int get_c(){return c;}
    void set_c(int c){this->c = c;}
};


class B : public A, public C{
public:
    B(int a) : A(a){
        this->a = a + 3;
    }
};

int foo(int a){
    B * b = new B(a);
    b->set_c(10);
    return b->get_a() + b->get_c();
}