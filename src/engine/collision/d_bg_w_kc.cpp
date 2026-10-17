//cpp
/* leftovers:
 * func_020396dc, func_02039794, func_020397b8 and func_020397dc stay free
 * functions. The ITCM DetectClsn overrides call them by address.
 * func_020396dc turns a prism pointer into a triangle index,
 * func_02039794 classifies the slope band, func_020397b8 tests wall facing,
 * func_020397dc guards a near-zero divisor. The destructor is defined
 * first (D2, then D0, then D1) and the constructor after it. The
 * DetectClsn triple and the geometry slots live in d_bg_w_kc_itcm.cpp.
 */
/* dBgW_Kc: static-mesh collider, the base KcMbg and KcMbgSclY share.
 * Type name 0x020993bc, typeinfo 0x020993c8, vtable 0x020993dc.
 */
#include "dBgW_Kc.h"
#include "dBgW.h"

extern "C" {
/* Shared base init, defined in its own TU (src/engine/collision/d_bg_w.cpp). */
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
/* True when a divisor is too close to zero to divide by (|value| <= 8). */
extern "C" int func_020397dc(int value) {
    return value <= 8 && value >= -8;
}

// @symbol func_020397b8
/* True when a face normal's y lies strictly inside the wall band. */
extern "C" int func_020397b8(int normalY) {
    return normalY < 0x600 && normalY > -0xccc;
}

// @symbol func_02039794
/* Slope band of a face normal's y: 0 floor, 1 wall, 2 underside. */
extern "C" int func_02039794(int normalY) {
    if (normalY > 0x600)
        return 0;
    if (normalY > -0xccc)
        return 1;
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
/* A prism record's index in the file's triangle table (records are 0x10). */
extern "C" unsigned func_020396dc(dBgW_Kc *self, KCL_Tri *tri) {
    return ((unsigned)tri - (unsigned)self->kclFile->tris) >> 4;
}

// @symbol _ZN7dBgW_Kc9Virtual08Ev
void dBgW_Kc::Virtual08()
{
}
