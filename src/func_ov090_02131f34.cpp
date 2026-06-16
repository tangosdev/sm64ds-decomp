//cpp
struct Arg;
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(Arg*); };
struct Obj { char pad[0xb0]; unsigned int flags; char pad2[0x30c-0xb4]; Sub sub; };
extern "C" int func_ov090_02131f34(Obj *c)
{
    unsigned int f = c->flags;
    int b = ((f & 0x40000) != 0);
    if(b) return 1;
    c->sub.m((Arg*)((char*)c+0x80));
    return 1;
}
