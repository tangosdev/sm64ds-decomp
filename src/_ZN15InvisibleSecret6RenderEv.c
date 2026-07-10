//cpp
// NONMATCHING: push-set / frame (div=22). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned int u32;

extern "C" {
extern void* _ZN5Actor15FindWithActorIDEjPS_(u32 id, void* prev);
extern void _ZN9ActorBase18MarkForDestructionEv(void* thiz);
extern void _ZN15TextureSequence6UpdateER15ModelComponents(void* ts, void* mc);
extern int data_0209b454;
}

struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int a);
};

extern "C" int _ZN15InvisibleSecret6RenderEv(char* c)
{
    void* a;

    u32 id = 0xb2;
    if (data_0209b454 & 0x4000000) {
        a = 0;
        while ((a = _ZN5Actor15FindWithActorIDEjPS_(id, a)) != 0) {
            if (*(int*)((char*)a + 0x43c) == 6 && *(unsigned short*)((char*)a + 0x496) == 0x64) {
                _ZN9ActorBase18MarkForDestructionEv(c);
                return 1;
            }
        }
    }

    if (*(unsigned short*)(c + 0x14c) != 0) {
        *(unsigned short*)(c + 0x14c) = *(unsigned short*)(c + 0x14c) - 1;
        return 1;
    }

    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x124, c + 0xdc);
    {
        Obj* o = (Obj*)(c + 0xd4);
        o->m(0);
    }
    return 1;
}
