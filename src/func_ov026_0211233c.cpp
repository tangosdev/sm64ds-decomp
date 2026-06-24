//cpp
extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* thiz, void* clsn);
extern void func_ov026_02112324(void* c);
extern void _ZN12CylinderClsn5ClearEv(void* a);
extern void _ZN12CylinderClsn6UpdateEv(void* a);
}

struct C;
typedef void (C::*PMF)();
struct Obj { char pad[8]; PMF pmf; };
struct C {
    char pad[0x30c];
    Obj* obj;
};

extern "C" int func_ov026_0211233c(char* cc){
    C* c = (C*)cc;
    DecIfAbove0_Short((unsigned short*)(cc + 0x100));
    Obj* o = c->obj;
    if (*(int*)((char*)o + 8) != 0) {
        (c->*(o->pmf))();
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(cc, cc + 0x110);
    *(short*)(cc + 0x8c) = *(short*)(cc + 0x92);
    *(short*)(cc + 0x8e) = *(short*)(cc + 0x94);
    *(short*)(cc + 0x90) = *(short*)(cc + 0x96);
    func_ov026_02112324(cc);
    _ZN12CylinderClsn5ClearEv(cc + 0x110);
    _ZN12CylinderClsn6UpdateEv(cc + 0x110);
    return 1;
}
