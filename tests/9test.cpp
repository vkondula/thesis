class A {
  virtual void foo(){}
};

class C: public A {};
class B: public A {

};

int main(){
  A * b = new B();
  return dynamic_cast<C*>(b)!=nullptr;
}
