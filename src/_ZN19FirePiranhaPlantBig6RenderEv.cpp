//cpp
struct V3 { int x, y, z; };
struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m5(V3* p);
};

extern "C" int _ZN19FirePiranhaPlantBig6RenderEv(char* c)
{
    int v = *(int*)(c + 0x204);
    int b;
    if (v == 0 || (b = (*(int*)(c + 0xb0) & 0x40000) != 0, b != 0)) {
        return 1;
    }
    V3 s;
    s.x = v;
    s.y = v;
    s.z = v;
    Obj* o = (Obj*)(c + 0x110);
    o->m5(&s);
    return 1;
}
