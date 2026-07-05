//cpp
// This-adjusting virtual destructor thunk (adjustment -0x10). Compiler-generated
// as a byproduct of a class with a secondary base of size 0x10 and its own
// out-of-line destructor; the tail-branch target is a relocation wildcard.
struct Base1 {
    char pad[0x10 - 4];
    virtual ~Base1();
    virtual void f1();
};
struct Base2 {
    int x;
    virtual ~Base2();
    virtual void g1();
};
struct Derived : Base1, Base2 {
    virtual ~Derived();
};
Derived::~Derived() {}
