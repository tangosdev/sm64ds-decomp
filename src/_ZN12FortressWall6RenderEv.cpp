//cpp
struct Base {
    virtual int vf0(int);
    virtual int vf1(int);
    virtual int vf2(int);
    virtual int vf3(int);
    virtual int vf4(int);
    virtual int vfunc(int a);
};
struct Obj {
    char pad[0xd4];
    Base sub;
};
extern "C" int _ZN12FortressWall6RenderEv(Obj *c) {
    if (*(unsigned char*)((char*)c+0x321) != 0) return 1;
    c->sub.vfunc(0);
    return 1;
}
