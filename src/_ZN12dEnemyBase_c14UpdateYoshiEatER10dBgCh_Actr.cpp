//cpp
// @symbol _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr
/* recovered: real C++ method on include/dEnemyBase_c.h.
 *
 * One frame of "this enemy is in Yoshi's mouth / has just been spat out". The three
 * flag bits at 0x0b0 pick the phase: 0x40000 = held in the mouth (track the eater's
 * position and return 2), 0x20000 = swallowed (return 1), 0x80000 = just spat out, in
 * which case the launch velocity is built from the eater's facing angle and then the
 * body flies under mesh collision until it lands.
 *
 * The launch block was the last residue: the cartridge fills the load-use slot after
 * `ldr r0,[r6,#0xd0]` with the rounding constant (mov r3,#0x800) while every spelling
 * tried here filled it with the position base (add r1,r6,#0x5c). Writing the eater
 * test in its POSITIVE sense -- `if (eater != 0) { launch } else v = 0x14000;` rather
 * than `if (eater == 0) v = 0x14000; else { launch }` -- puts the launch block first in
 * the linearisation and the slot falls the cartridge's way. Same branch, same
 * semantics; only the order the two arms are visited in.
 *
 * The raw `self + offset` spellings are kept because dEnemyBase_c's header does not
 * name every field this function touches; `this` and the reference argument arrive in
 * r0/r1 exactly as the free-function form did, so the body is unchanged.
 */
#include "dEnemyBase_c.h"

extern "C" {
extern int _ZNK10dBgCh_Actr13GetLimMovFlagEv(char *c);
extern void _ZN10dBgCh_Actr13SetLimMovFlagEv(char *c);
extern void _ZN10dBgCh_Actr15ClearLimMovFlagEv(char *c);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char *self, int p);
extern void dBgCh_Actr_UpdateContinuous_Veneer(char *c);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(char *c);
extern char *_ZNK10dBgCh_Actr14GetFloorResultEv(char *c);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(char *p, char *v);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(char *c);
extern int _ZN4cstd4fdivEii(int a, int b);
extern int Vec3_HorzLen(int *v);
extern short data_02082214[];
}

int dEnemyBase_c::UpdateYoshiEat(dBgCh_Actr & clsn_)
{
    char *self = (char *)this;
    char *clsn = (char *)&clsn_;
    int flags;
    int t;
    int v;
    int lim;

    flags = *(int *)(self + 0xb0);
    t = flags & 0x40000;
    t = t != 0;
    if (t != 0) {
        if (*(char **)(self + 0xd0) == 0) {
            *(int *)(self + 0xb0) &= ~0x40000;
        } else {
            char *o = *(char **)(self + 0xd0) + 0x5c;
            *(int *)(self + 0x5c) = *(int *)o;
            *(int *)(self + 0x60) = *(int *)(o + 4);
            *(int *)(self + 0x64) = *(int *)(o + 8);
            *(int *)(self + 0x60) += 0x30000;
        }
        *(unsigned char *)(self + 0x107) = 0;
        return 2;
    }
    t = flags & 0x20000;
    t = t != 0;
    if (t != 0) {
        *(unsigned char *)(self + 0x107) = 0;
        return 1;
    }
    t = flags & 0x80000;
    t = t != 0;
    if (t != 0 || *(unsigned char *)(self + 0x107) != 0) {
        if (*(u16 *)(self + 0x104) != 0)
            *(u16 *)(self + 0x104) -= 1;
        t = *(int *)(self + 0xb0) & 0x80000;
        t = t != 0;
        if (t != 0) {
            *(unsigned char *)(self + 0x107) = 1;
            *(u16 *)(self + 0x104) = 5;
            *(int *)(self + 0xb0) &= ~0x80000;
            if (*(char **)(self + 0xd0) != 0) {
                char *o2 = *(char **)(self + 0xd0) + 0x5c;
                *(int *)(self + 0x5c) = *(int *)o2;
                *(int *)(self + 0x60) = *(int *)(o2 + 4);
                *(int *)(self + 0x64) = *(int *)(o2 + 8);
                *(int *)(self + 0x60) += 0x32000;
                {
                    long long rnd = 0x800;
                    char *o3 = *(char **)(self + 0xd0);
                    u16 ang = *(u16 *)(o3 + 0x8e);
                    int idx = (ang >> 4) << 1;
                    *(int *)(self + 0x5c) =
                        (int)(((long long)data_02082214[idx] * 0x32000 + rnd) >> 12) +
                        *(int *)(self + 0x5c);
                    *(int *)(self + 0x64) +=
                        (int)(((long long)data_02082214[idx + 1] * 0x32000 + rnd) >> 12);
                }
                {
                    s16 *p = (s16 *)(*(char **)(self + 0xd0) + 0x8c);
                    *(s16 *)(self + 0x92) = *p;
                    *(s16 *)(self + 0x94) = p[1];
                    *(s16 *)(self + 0x96) = p[2];
                }
                v = *(int *)(*(char **)(self + 0xd0) + 0x98);
            } else {
                v = 0x14000;
            }
            if (v >= 0x14000)
                *(int *)(self + 0x98) = v + 0xa000;
            else
                *(int *)(self + 0x98) = (int)(((long long)v * 0x800 + 0x800) >> 12) + 0x14000;
            *(int *)(self + 0x98) = (int)(((long long)*(int *)(self + 0x98) * 0x14cc + 0x800) >> 12);
            *(int *)(self + 0xa8) = 0xc000;
            *(int *)(self + 0xd0) = 0;
        }
        lim = _ZNK10dBgCh_Actr13GetLimMovFlagEv(clsn);
        _ZN10dBgCh_Actr13SetLimMovFlagEv(clsn);
        _ZN8dActor_c9UpdatePosEP5dCc_c(self, 0);
        dBgCh_Actr_UpdateContinuous_Veneer(clsn);
        if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn) != 0) {
            char *fr = _ZNK10dBgCh_Actr14GetFloorResultEv(clsn);
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fr + 4, self + 0xd4);
            if (_ZNK10dBgCh_Actr13JustHitGroundEv(clsn) == 0) {
                *(int *)(self + 0xa8) = 0;
                *(unsigned char *)(self + 0x107) = 0;
            } else {
                _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fr + 4, self + 0xd4);
                *(int *)(self + 0xa8) =
                    _ZN4cstd4fdivEii((*(int *)(self + 0xa8) * -50) / 100, *(int *)(self + 0xd8));
            }
            {
                int *pa = (int *)(self + 0xa4);
                *pa += *(int *)(self + 0xd4) << 3;
                *(int *)(self + 0xac) += *(int *)(self + 0xdc) << 3;
                *(int *)(self + 0x98) = Vec3_HorzLen(pa);
            }
        }
        *(s16 *)(self + 0x8e) = *(s16 *)(self + 0x94);
        if (lim == 0)
            _ZN10dBgCh_Actr15ClearLimMovFlagEv(clsn);
        return 3;
    }
    *(int *)(self + 0xd0) = 0;
    *(int *)(self + 0xb0) &= ~0xe0000;
    *(unsigned char *)(self + 0x107) = 0;
    return 0;
}
