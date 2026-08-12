//cpp
#include "types.h"
// @symbol _ZN13WaterfallMist8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "WaterfallMist.h"
struct WithMeshClsn;
/* NOT the real Enemy, and deliberately no longer named one. This stand-in exists to
   give the pointer-to-member below a representation: a PMF on a non-polymorphic,
   single-base class is laid out differently from one on the real Enemy, so the shape
   here is codegen, not decoration. Borrowing the name worked only while this file
   included no header that defined the real class; with WaterfallMist.h in scope it
   became a redefinition, and letting the PMF bind to the real Enemy makes mwccarm
   abort with an internal compiler error rather than a diagnostic. */
struct MistPmfSelf { char pad[0x800]; };
typedef void (MistPmfSelf::*PMF)();
struct Holder { char pad[8]; PMF fn; };

struct VObj {
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03(); /* slot 3 = 0xc */
};

struct Flags3eb {
    u8 f0 : 1;
    u8 f1 : 1;
};

extern "C" {
extern void _ZN5Actor13SmallPoofDustEv(MistPmfSelf *thiz);
extern void _Z14ApproachLinearRiii(int *x, int target, int step);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(MistPmfSelf *thiz, void *clsn);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(MistPmfSelf *thiz, WithMeshClsn *wm, u32 j);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *thiz);
extern void func_ov002_020b7f7c(char *c);
extern void _ZN9Animation7AdvanceEv(void *thiz);
extern void func_020167a4(char *p);
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(MistPmfSelf *thiz, WithMeshClsn *wm);
extern void _ZN12CylinderClsn5ClearEv(void *thiz);
extern void _ZN12CylinderClsn6UpdateEv(void *thiz);
}

int WaterfallMist::Behavior()
{
    char *c = (char *)((MistPmfSelf *)this);

    if (*(u8 *)(c + 0x400) != 0xff) {
        if (((Flags3eb *)(c + 0x3eb))->f1 == 0) {
            *(u8 *)(c + 0x3ff) = 1;
        } else if (*(u8 *)(c + 0x3ff) == 1) {
            *(int *)(c + 0x80) = 0;
            *(int *)(c + 0x84) = 0;
            *(int *)(c + 0x88) = 0;
            *(u8 *)(c + 0x3ff) = 0;
            *(int *)(c + 0x40c) = 0x2000;
            *(u8 *)(c + 0x402) = 1;
            *(int *)(c + 0xa8) = 0xf000;
            _ZN5Actor13SmallPoofDustEv(((MistPmfSelf *)this));
        }
    }

    if (*(u8 *)(c + 0x3ff) == 1) {
        return 1;
    }

    if (*(u8 *)(c + 0x400) != 0xff && *(u8 *)(c + 0x402) != 0) {
        _Z14ApproachLinearRiii((int *)(c + 0x40c), 0x1000, 0x200);
        _Z14ApproachLinearRiii((int *)(c + 0x80), *(int *)(c + 0x40c), 0x400);
        *(int *)(c + 0x88) = *(int *)(c + 0x80);
        *(int *)(c + 0x84) = *(int *)(c + 0x88);
        _ZN5Actor9UpdatePosEP12CylinderClsn(((MistPmfSelf *)this), c + 0x110);
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((MistPmfSelf *)this), (WithMeshClsn *)(c + 0x144), 0);
        if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x144) != 0) {
            if (*(int *)(c + 0x80) == 0x1000) {
                *(u8 *)(c + 0x402) = 0;
            }
        }
    }

    {
        Holder *q = *(Holder **)(c + 0x3bc);
        if (q->fn != 0) {
            (((MistPmfSelf *)this)->*(q->fn))();
        }
    }

    func_ov002_020b7f7c(c);
    _ZN9Animation7AdvanceEv(c + 0x350);

    if (*(int *)(c + 0x360) != 0) {
        func_020167a4(c + 0x300);
        ((VObj *)(c + 0x300))->v03();
    }

    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(((MistPmfSelf *)this), (WithMeshClsn *)(c + 0x144)) != 0) {
        return 1;
    }

    {
        int v = *(int *)(c + 0x3f0);
        if (v != 4 && v != 0x11 && v != 6 && v != 8 && v != 0xc && v != 0xa
            && v != 0x13 && v != 0xf && v != 0x14 && v != 0x15 && v != 0x16 && v != 0xd) {
            _ZN5Actor9UpdatePosEP12CylinderClsn(((MistPmfSelf *)this), c + 0x110);
            _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((MistPmfSelf *)this), (WithMeshClsn *)(c + 0x144), 0);
        }
    }

    _ZN12CylinderClsn5ClearEv(c + 0x110);
    _ZN12CylinderClsn6UpdateEv(c + 0x110);
    return 1;
}
