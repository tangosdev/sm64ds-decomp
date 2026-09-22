//cpp
#include "types.h"
// @symbol func_ov074_021223bc
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Gnd.h"
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
extern int func_02037e20(int *p);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, dCc_c *cc);
extern void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *self, dBgCh_Actr *wm, unsigned int j);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, ShadowModel *sm, Matrix4x3 *mtx, Fix12i h, Fix12i g, unsigned int u);
extern Matrix4x3 IDENTITY_MATRIX4X3;

int func_ov074_021223bc(char *c)
{
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

    dBgCh_Gnd rg;
    rg.StartDetectingWater();
    rg.StartDetectingToxic();
    rg.StopDetectingOrdinary();
    rg.SetObjAndPos(pos, (dActor_c*)c);

    if (rg.DetectClsn() != 0
        && func_02037e20((int*)&rg.surface) != 0
        && rg.clsnY != (int)0x80000000
        && *(int *)(c + 0x60) < rg.clsnY) {
        _ZN8dActor_c8PoofDustEv(c);
        _ZN7fBase_c18MarkForDestructionEv(c);
        return 1;
    }

    if (*(u16 *)(c + 0x5fc) < 0x60) {
        _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);
        _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(c, (dBgCh_Actr *)(c + 0x40c), 0);

        *(s16 *)AT(c, 0x8c) = *(s16 *)AT(c, 0x8c) + 0x1800;
    }

    *(Matrix4x3 *)(c + 0x2ec) = IDENTITY_MATRIX4X3;

    *(int *)(c + 0x310) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x314) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x318) = *(int *)(c + 0x64) >> 3;

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, (ShadowModel *)(c + 0x274), (Matrix4x3 *)(c + 0x2ec), 0x8c000, 0x3e8000, 0xf);

    return 1;
}
}
