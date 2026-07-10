//cpp
struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(void *arg);
};
extern "C" int _ZN9WaterBomb6RenderEv(char *c)
{
    if (*(int *)(c + 0x3c8) != 0) {
        Obj *o = (Obj *)(c + 0x300);
        o->m(c + 0x80);
    }
    return 1;
}
