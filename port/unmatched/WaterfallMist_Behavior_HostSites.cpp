/* WaterfallMist::Behavior, host copy with ONE virtual call spelled at the
 * ROM's target -- lane CAPSHOW's substitution for
 * src/_ZN13WaterfallMist8BehaviorEv.cpp in slice_gate33 (the CMake loop skips
 * the src line; slice_gate33.txt itself is untouched). The Lakitu_HostSites
 * treatment.
 *
 * WHY: the matched TU calls `((VObj *)(c + 0x300))->v03()` after
 * func_020167a4 whenever the cap's ModelAnim carries a file (+0x360 != 0).
 * The ROM's ModelAnim vtable is [D1, D0, DoSetFile, UpdateVerts, Virtual10,
 * Render, Virtual18], so +0x0c is ModelAnim::UpdateVerts(), no arguments. The
 * port's _ZTV9ModelAnim is filled in MSVC numbering (one destructor slot:
 * [0] dtor, [1] DoSetFile, [2] UpdateVerts, [3] Virtual10, [4] Render,
 * [5] Virtual18; hal/actor_classes_intro.cpp), so the shadow's +0x0c lands on
 * Virtual10(Matrix4x3 &): a one-argument __thiscall that pops four bytes the
 * caller never pushed. MSVC compiled Behavior without a frame pointer
 * (push esi / push edi ... pop edi / pop esi / ret), so its own `ret` then
 * pops the wrong slot and jumps into the actor list.
 *
 * Reachable only once a cap is COLLECTED: the VS cap has no animation until
 * func_ov002_020b74d0 seats one with ModelAnim::SetAnim on the frame after
 * pickup, and no cap could be picked up before the cap-visibility manager
 * was hosted (slice_cap.txt). Measured on VS map 2 (level 43): a cdb trace of
 * esp at every return site inside Behavior for the collected cap reads the
 * same values on every frame until the pickup frame, where the site after
 * `call [eax+0Ch]` comes back at 1af1d8 instead of 1af1d4 and the function's
 * ret lands on 0x300387b0 (c0000005, "walker actor id 0x10d").
 *
 * Everything below the banner is the matched TU, byte for byte, except the
 * one call. PORT_HOST_ABI: MSVC single-destructor vtable numbering, the
 * ROM slot-3 UpdateVerts spelled qualified.
 */
#include "types.h"
// @symbol _ZN13WaterfallMist8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "WaterfallMist.h"
#include "ModelAnim.h"
struct WithMeshClsn;
struct Enemy { char pad[0x800]; };
typedef void (Enemy::*PMF)();
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
extern void _ZN5Actor13SmallPoofDustEv(Enemy *thiz);
extern void _Z14ApproachLinearRiii(int *x, int target, int step);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(Enemy *thiz, void *clsn);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(Enemy *thiz, WithMeshClsn *wm, u32 j);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *thiz);
extern void func_ov002_020b7f7c(char *c);
extern void _ZN9Animation7AdvanceEv(void *thiz);
extern void func_020167a4(char *p);
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(Enemy *thiz, WithMeshClsn *wm);
extern void _ZN12CylinderClsn5ClearEv(void *thiz);
extern void _ZN12CylinderClsn6UpdateEv(void *thiz);
}

int WaterfallMist::Behavior()
{
    char *c = (char *)((Enemy *)this);

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
            _ZN5Actor13SmallPoofDustEv(((Enemy *)this));
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
        _ZN5Actor9UpdatePosEP12CylinderClsn(((Enemy *)this), c + 0x110);
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((Enemy *)this), (WithMeshClsn *)(c + 0x144), 0);
        if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x144) != 0) {
            if (*(int *)(c + 0x80) == 0x1000) {
                *(u8 *)(c + 0x402) = 0;
            }
        }
    }

    {
        Holder *q = *(Holder **)(c + 0x3bc);
        if (q->fn != 0) {
            (((Enemy *)this)->*(q->fn))();
        }
    }

    func_ov002_020b7f7c(c);
    _ZN9Animation7AdvanceEv(c + 0x350);

    if (*(int *)(c + 0x360) != 0) {
        func_020167a4(c + 0x300);
        /* ((VObj *)(c + 0x300))->v03() -- the ROM's +0x0c slot on the cap's
           own ModelAnim, ModelAnim::UpdateVerts, spelled qualified. The
           port's _ZTV9ModelAnim is MSVC-numbered (one destructor slot), so
           the shadow's +0x0c is Virtual10(Matrix4x3 &) there, a one-argument
           __thiscall whose `ret 4` leaves this frameless caller's esp four
           bytes high and its final ret jumping into the actor list. */
        ((ModelAnim *)(c + 0x300))->ModelAnim::UpdateVerts();
    }

    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(((Enemy *)this), (WithMeshClsn *)(c + 0x144)) != 0) {
        return 1;
    }

    {
        int v = *(int *)(c + 0x3f0);
        if (v != 4 && v != 0x11 && v != 6 && v != 8 && v != 0xc && v != 0xa
            && v != 0x13 && v != 0xf && v != 0x14 && v != 0x15 && v != 0x16 && v != 0xd) {
            _ZN5Actor9UpdatePosEP12CylinderClsn(((Enemy *)this), c + 0x110);
            _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((Enemy *)this), (WithMeshClsn *)(c + 0x144), 0);
        }
    }

    _ZN12CylinderClsn5ClearEv(c + 0x110);
    _ZN12CylinderClsn6UpdateEv(c + 0x110);
    return 1;
}
