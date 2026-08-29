//cpp
#include "types.h"
// @symbol func_ov060_02113b5c
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Gnd.h"

extern "C" {
    void func_ov060_02111cc0(char* c, int a, int b);
    void _Z14ApproachLinearRiii(int* v, int a, int b);
    void func_ov060_02115a84(char* c, char* arg);
    int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
    int _ZNK10dBgCh_Actr13JustHitGroundEv(void* self);
    int Bowser_IsAnimAtLastFrame(char* c);
    int func_ov060_02113d20(char* c);
}

extern "C" void func_ov060_02113b5c(char* c)
{
    if (*(u16*)(c + 0x300 + 0xfc) < 2) {
        *(u16*)(c + 0x300 + 0xfe) = 0;
    }

    int r4 = *(s32*)(c + 0x60);
    if (*(s32*)(c + 0x60) > *(s32*)(c + 0x3b4)) {
        Vector3 v;
        dBgCh_Gnd rg;
        int base = *(s32*)(c + 0x3b4);
        int zz = *(s32*)(c + 0x64);
        int xx = *(s32*)(c + 0x5c);
        int yy = base + 0x96000;
        v.x = xx;
        v.y = yy;
        v.z = zz;
        rg.SetObjAndPos(v, (dActor_c*)c);
        if (rg.DetectClsn() != 0) {
            int hy = rg.clsnY;
            if (hy >= *(s32*)(c + 0x3b4) - 0x64000) r4 = hy;
        }
    }

    if (*(u8*)(c + 0x423) == 0) {
        func_ov060_02111cc0(c, 9, 0);

        int dy = *(s32*)(c + 0x60) - r4;
        if (dy > 0xc8000 || *(s32*)(c + 0x3f4) > 0xdac000) {
            if (*(s32*)(c + 0x98) >= 0x1e000) {
                _Z14ApproachLinearRiii((int*)(c + 0x98), 0x1e000, 0x4000);
            }
        }

        func_ov060_02115a84(c, c + 0x3fe);

        if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x14c) != 0) {
            if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x14c) == 0) {
                *(s32*)(c + 0x98) = 0;
                *(u8*)(c + 0x427) = 0;
                u8* p = (u8*)(((int)c + 0x423));
                *p = *p + 1;
                func_ov060_02111cc0(c, 0xd, 0x40000000);
            }
        }
    } else {
        if (Bowser_IsAnimAtLastFrame(c) != 0) {
            *(s32*)(c + 0x40c) = 0;
        }
    }

    if (func_ov060_02113d20(c) != 0) {
        signed char* q = (signed char*)(((int)c + 0x41e));
        *q = *q - 1;
        if (*(signed char*)(c + 0x400 + 0x1e) <= 0) {
            *(s32*)(c + 0x40c) = 4;
        } else {
            *(s32*)(c + 0x40c) = 0xc;
        }
    }
}
