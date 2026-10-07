//cpp
/* dBgW_Kc -- the static-mesh collider: the base every KCL-backed wall
 * collider shares (KcMbg the plain mesh, KcMbgSclY the y-stretched one).
 * ROM's own RTTI names it (_ZTS7dBgW_Kc @ 0x020993bc, _ZTI7dBgW_Kc @ 0x020993c8, vtable
 * _ZTV7dBgW_Kc @ 0x020993dc).
 *
 * Deferred codegen emits the lifecycle groups in their fixed order -- D2,D0,D1
 * for the destructor, then C1,C2 for the constructor -- and the plain members
 * land in reverse definition order, so the file defines ctor and dtor first.
 * The four func_ workers keep their func_ names: they are TU-local helpers the
 * ITCM DetectClsn overrides and other collider TUs call by raw address, not
 * vtable slots. func_020396dc is the prism -> triangle-index helper,
 * func_02039794 the slope-band classify, func_020397b8 the wall-facing test
 * and func_020397dc the near-zero divisor guard. The DetectClsn triple and
 * the three geometry/surface slots live in the ITCM TU
 * (src/engine/collision/dBgW_Kc_itcm.cpp).
 */
#include "dBgW_Kc.h"
#include "dBgW.h"

extern "C" {
/* Shared base init, defined in its own TU (src/engine/collision/dBgW.cpp). */
void func_02039624(dBgW *self);
}

// @symbol _ZN7dBgW_KcC1Ev
// @symbol _ZN7dBgW_KcC2Ev
dBgW_Kc::dBgW_Kc()
{
    kclFile = 0;
}

// @symbol _ZN7dBgW_KcD2Ev
// @symbol _ZN7dBgW_KcD0Ev
// @symbol _ZN7dBgW_KcD1Ev
dBgW_Kc::~dBgW_Kc() {}

// @symbol func_020397dc
extern "C" int func_020397dc(int x)
{
    if (x <= 8 && x >= -8) return 1;
    return 0;
}

// @symbol func_020397b8
extern "C" int func_020397b8(int x)
{
    if (x < 0x600) {
        if (x > -0xccc) return 1;
    }
    return 0;
}

// @symbol func_02039794
extern "C" int func_02039794(int x)
{
    if (x > 0x600) return 0;
    if (x > -0xccc) return 1;
    return 2;
}

/* The four header words of a fresh KCL file are file-relative offsets;
   rebase them into pointers. */
// @symbol _ZN7dBgW_Kc17UpdateFileOffsetsER8KCL_File
void dBgW_Kc::UpdateFileOffsets(KCL_File &file)
{
    file.positions = (s32 (*)[3])((char *)&file + (int)file.positions);
    file.normals = (s16 (*)[3])((char *)&file + (int)file.normals);
    file.tris = (KCL_Tri *)((char *)&file + (int)file.tris);
    file.unk_0c = (char *)&file + (int)file.unk_0c;
}

// @symbol _ZN7dBgW_Kc7SetFileEP8KCL_FileR10CLPS_Block
void dBgW_Kc::SetFile(KCL_File *file, CLPS_Block &clpsBlock)
{
    func_02039624(this);
    kclFile = file;
    clps = clpsBlock;
    unk_28 = 0;
    unk_2c = 0x1000;
    unk_30 = 0;
    unk_34 = 0;
    unk_35 = 0;
    unk_38 = 0x1000;
    unk_3c = 0;
    unk_40 = 0;
    unk_44 = -0x1000;
    unk_48 = 2;
    unk_4c = 1;
    unk_4d = 0;
}

// @symbol func_020396dc
extern "C" unsigned func_020396dc(int **p, unsigned x)
{
    return (x - (unsigned)p[8][2]) >> 4;
}

// @symbol _ZN7dBgW_Kc9Virtual08Ev
void dBgW_Kc::Virtual08()
{
}
