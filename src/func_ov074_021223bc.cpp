//cpp
#include "types.h"
// @symbol func_ov074_021223bc
/* recovered: shared common types */
#include "common.h"
struct dBgCh_Gnd { char buf0[0x14]; int floor[12]; char buf1[0x50 - 0x14 - 0x30]; };
#define AT(p,off) ((void*)(int)((char*)(p)+(off)))
struct dBgCh_Actr;
struct dCc_c;
struct ShadowModel;
struct Matrix4x3;

extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _ZN8dActor_c8PoofDustEv(void *self);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *self);
extern void func_02012694(int a, void *b);
extern void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd *self);
extern void _ZN5dBgCh19StartDetectingWaterEv(dBgCh_Gnd *self);
extern void _ZN5dBgCh19StartDetectingToxicEv(dBgCh_Gnd *self);
extern void _ZN5dBgCh21StopDetectingOrdinaryEv(dBgCh_Gnd *self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd *self, const Vector3 *v, void *actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd *self);
extern int func_02037e20(int *p);
extern void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd *self);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, dCc_c *cc);
extern void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *self, dBgCh_Actr *wm, unsigned int j);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, ShadowModel *sm, Matrix4x3 *mtx, Fix12i h, Fix12i g, unsigned int u);
extern struct Blob48 { int w[12]; } IDENTITY_MATRIX4X3;

int func_ov074_021223bc(char *c)
{
    dBgCh_Gnd rg;
    Vector3 pos;

    if (*(u8 *)(c + 0x609) == 0) {
        return 1;
    }

    if (DecIfAbove0_Short((u16 *)(c + 0x5fc)) == 0 || *(u8 *)(c + 0x603) >= 3) {
        int b = ((*(int *)(c + 0xb0) & 8) != 0) ? 1 : 0;
        if (b == 0) {
            _ZN8dActor_c8PoofDustEv(c);
        }
        _ZN7fBase_c18MarkForDestructionEv(c);
        return 1;
    }

    if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x40c)) {
        int b = ((*(int *)(c + 0xb0) & 8) != 0) ? 1 : 0;
        if (b == 0) {
            if (*(u8 *)(c + 0x603) == 0) {
                func_02012694(0x13a, c + 0x74);
            } else {
                func_02012694(0x13b, c + 0x74);
            }
        }
        (*(u8 *)AT(c, 0x603))++;

        *(int *)(c + 0xa8) = *(int *)(c + 0x98) + 0xa000;
    }

    {
        int yy = *(int *)(c + 0x60);
        int zz = *(int *)(c + 0x64);
        int xx = *(int *)(c + 0x5c);
        int py = yy + 0x190000;
        pos.x = xx;
        pos.y = py;
        pos.z = zz;
    }

    _ZN9dBgCh_GndC1Ev(&rg);
    _ZN5dBgCh19StartDetectingWaterEv(&rg);
    _ZN5dBgCh19StartDetectingToxicEv(&rg);
    _ZN5dBgCh21StopDetectingOrdinaryEv(&rg);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &pos, c);

    if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) != 0
        && func_02037e20(rg.floor) != 0
        && rg.floor[(0x44 - 0x14) / 4] != (int)0x80000000
        && *(int *)(c + 0x60) < rg.floor[(0x44 - 0x14) / 4]) {
        _ZN8dActor_c8PoofDustEv(c);
        _ZN7fBase_c18MarkForDestructionEv(c);
        _ZN9dBgCh_GndD1Ev(&rg);
        return 1;
    }

    if (*(u16 *)(c + 0x5fc) < 0x60) {
        _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);
        _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(c, (dBgCh_Actr *)(c + 0x40c), 0);

        *(s16 *)AT(c, 0x8c) = *(s16 *)AT(c, 0x8c) + 0x1800;
    }

    *(Blob48 *)(c + 0x2ec) = IDENTITY_MATRIX4X3;

    *(int *)(c + 0x310) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x314) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x318) = *(int *)(c + 0x64) >> 3;

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, (ShadowModel *)(c + 0x274), (Matrix4x3 *)(c + 0x2ec), 0x8c000, 0x3e8000, 0xf);

    _ZN9dBgCh_GndD1Ev(&rg);

    return 1;
}
}
