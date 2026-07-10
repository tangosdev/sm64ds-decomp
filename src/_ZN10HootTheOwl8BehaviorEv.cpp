//cpp
struct C3;
typedef void (C3::*Fn)();
struct Obj { char pad[8]; Fn fn; };
extern "C" {
extern void DecIfAbove0_Short(void*);
extern void _ZN9Animation7AdvanceEv(void*);
extern void func_02012694(int, void*);
extern void func_ov094_021362e0(void*);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void*, void*, unsigned int);
extern void func_ov094_021361d8(void*);
extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void*, void*);
extern int func_ov096_021357a4(void*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void _ZN12CylinderClsn6UpdateEv(void*);

extern char data_ov094_02136b40[];
extern char data_ov094_02136b50[];
extern char data_ov094_02136b60[];
extern char data_ov094_02136b30[];
extern char data_ov094_02136b70[];
}

extern "C" int _ZN10HootTheOwl8BehaviorEv(char *c)
{
    DecIfAbove0_Short(c+0x100);
    {
        Obj *o = *(Obj**)(c+0x3c8);
        if (*(int*)((char*)o+8) != 0) {
            (((C3*)c)->*(o->fn))();
        }
    }
    if (*(char**)(c+0x3c8) == data_ov094_02136b40) return 1;
    *(int*)(c+0x368) = *(int*)(c+0x3f0);
    _ZN9Animation7AdvanceEv(c+0x35c);
    {
        char *m = *(char**)(c+0x3c8);
        if ((m == data_ov094_02136b50 || m == data_ov094_02136b60 ||
             m == data_ov094_02136b30) &&
            (unsigned short)(*(int*)(c+0x364) >> 0xc) == 0) {
            func_02012694(0x139, c+0x74);
        }
    }
    if (*(char**)(c+0x3c8) == data_ov094_02136b70) {
        if (*(char**)(c+0x3c8) == data_ov094_02136b70) {
            func_ov094_021362e0(c);
            *(short*)(c+0x8c) = *(short*)(c+0x92);
            *(short*)(c+0x8e) = *(short*)(c+0x94);
            *(short*)(c+0x90) = *(short*)(c+0x96);
            _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c+0x150, 0);
        } else {
            func_ov094_021361d8(c);
        }
        return 1;
    }
    {
        int s = *(int*)(c+0xa8) + *(int*)(c+0x9c);
        int m2 = *(int*)(c+0xa0);
        int ac = *(int*)(c+0xac);
        if (s >= m2) m2 = s;
        *(int*)(c+0xa8) = m2;
        *(int*)(c+0xac) = ac;
    }
    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(c, c+0x110);
    *(short*)(c+0x8c) = *(short*)(c+0x92);
    *(short*)(c+0x8e) = *(short*)(c+0x94);
    *(short*)(c+0x90) = *(short*)(c+0x96);
    func_ov094_021361d8(c);
    if (*(char**)(c+0x3c8) == data_ov094_02136b60 && *(unsigned char*)(c+0x3d4) == 2) {
        func_ov096_021357a4(c);
    }
    _ZN12CylinderClsn5ClearEv(c+0x110);
    _ZN12CylinderClsn6UpdateEv(c+0x110);
    return 1;
}
