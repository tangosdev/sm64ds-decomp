//cpp
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Outer { char pad0[0x10]; Base a; char pad1[0x4c]; Base b; };

extern "C" void func_ov006_02107d80(Outer *o) {
    o->a.m(0);
    o->b.m(0);
}
