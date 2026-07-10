//cpp
struct Foo {
    virtual void M0(int x);
    virtual void M1(int x);
    virtual void M2(int x);
    virtual void M3(int x);
    virtual void M4(int x);
    virtual void M5(int x);
};
struct Container {
    char pad1[0x30c];
    Foo obj1;
    char pad2[0x4c];
    Foo obj2;
};
extern "C" int _ZN10BulletBill6RenderEv(Container *c) {
    c->obj1.M5(0);
    c->obj2.M5(0);
    return 1;
}
