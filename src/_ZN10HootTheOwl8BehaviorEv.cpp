//cpp
// @symbol _ZN10HootTheOwl8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "HootTheOwl.h"
struct C3;
typedef void (C3::*Fn)();
struct Obj { char pad[8]; Fn fn; };
extern "C" {
extern void DecIfAbove0_Short(void*);
extern void _ZN9Animation7AdvanceEv(void*);
extern void func_02012694(int, void*);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void*, void*, unsigned int);
extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void*, void*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void _ZN12CylinderClsn6UpdateEv(void*);

extern char data_ov094_02136b40[];
extern char data_ov094_02136b60[];
extern void func_ov094_021357a4(void*);
}

int HootTheOwl::Behavior()
{
    DecIfAbove0_Short((char *)&unk_100);
    {
        Obj *o = *(Obj**)((char *)&mCurrentState);
        if (*(int*)((char*)o+8) != 0) {
            (((C3*)((char *)this))->*(o->fn))();
        }
    }
    if (*(char**)((char *)&mCurrentState) == data_ov094_02136b40) return 1;
    unk_368 = unk_3f0;
    _ZN9Animation7AdvanceEv((char *)&mAnimation);
    {
        char *m = *(char**)((char *)&mCurrentState);
        if ((m == data_ov094_02136b50 || m == data_ov094_02136b60 ||
             m == data_ov094_02136b30) &&
            (unsigned short)(unk_364 >> 0xc) == 0) {
            func_02012694(0x139, ((char *)this)+0x74);
        }
    }
    if (*(char**)((char *)&mCurrentState) == data_ov094_02136b70) {
        if (*(char**)((char *)&mCurrentState) == data_ov094_02136b70) {
            func_ov094_021362e0(((char *)this));
            unk_08c = unk_092;
            unk_08e = unk_094;
            unk_090 = unk_096;
            _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((char *)this), ((char *)this)+0x150, 0);
        } else {
            func_ov094_021361d8(((char *)this));
        }
        return 1;
    }
    {
        int s = unk_0a8 + unk_09c;
        int m2 = unk_0a0;
        int ac = unk_0ac;
        if (s >= m2) m2 = s;
        unk_0a8 = m2;
        unk_0ac = ac;
    }
    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(((char *)this), ((char *)this)+0x110);
    unk_08c = unk_092;
    unk_08e = unk_094;
    unk_090 = unk_096;
    func_ov094_021361d8(((char *)this));
    if (*(char**)((char *)&mCurrentState) == data_ov094_02136b60 && unk_3d4 == 2) {
        func_ov094_021357a4(((char *)this));
    }
    _ZN12CylinderClsn5ClearEv((char *)&mMovingCylinderClsnWithPos);
    _ZN12CylinderClsn6UpdateEv((char *)&mMovingCylinderClsnWithPos);
    return 1;
}
