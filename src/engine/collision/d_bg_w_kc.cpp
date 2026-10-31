//cpp
/* The ITCM query code uses the static divisor and wall-normal predicates.
 * The prism-index and slope-classification helpers retain their existing
 * free-function interfaces. The constructor precedes the destructor to
 * preserve lifecycle emission order. DetectClsn and the geometry slots live
 * in d_bg_w_kc_itcm.cpp.
 */
/* dBgW_Kc: static-mesh collider, the base KcMbg and KcMbgSclY share.
 * Type name 0x020993bc, typeinfo 0x020993c8, vtable 0x020993dc.
 */
#include "dBgW_Kc.h"
#include "dBgW.h"
#include "dBgPc.h"


/* The surface record the ITCM DetectClsn overrides (d_bg_w_kc_itcm.cpp)
   fill per triangle. A namespace-scope dBgPc, so mwcc emits
   __sinit_d_bg_w_kc.cpp to construct it and register ~dBgPc() (was the
   handwritten __sinit_02074fe4). The ITCM half reaches it by this
   address name. */
dBgPc data_020a0cec;

// @symbol _ZN7dBgW_KcC1Ev
// @symbol _ZN7dBgW_KcC2Ev
dBgW_Kc::dBgW_Kc() {
    kclFile = 0;
}

// @symbol _ZN7dBgW_KcD2Ev
// @symbol _ZN7dBgW_KcD0Ev
// @symbol _ZN7dBgW_KcD1Ev
dBgW_Kc::~dBgW_Kc() {}

// @symbol _ZN7dBgW_Kc17IsNearZeroDivisorEi
/* True when a divisor is too close to zero to divide by (|value| <= 8). */
bool dBgW_Kc::IsNearZeroDivisor(int value) {
    return value <= 8 && value >= -8;
}

// @symbol _ZN7dBgW_Kc12IsWallNormalEi
/* True when a face normal's y lies strictly inside the wall band. */
bool dBgW_Kc::IsWallNormal(Fix12i normalY) {
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
void dBgW_Kc::UpdateFileOffsets(KCL_File &file) {
    file.positions = (s32(*)[3])((char *)&file + (int)file.positions);
    file.normals = (s16(*)[3])((char *)&file + (int)file.normals);
    file.tris = (KCL_Tri *)((char *)&file + (int)file.tris);
    file.octree = (char *)&file + (int)file.octree;
}

// @symbol _ZN7dBgW_Kc7SetFileEP8KCL_FileR10CLPS_Block
void dBgW_Kc::SetFile(KCL_File *file, CLPS_Block &clpsBlock) {
    Reset();
    kclFile = file;
    clps = clpsBlock;
    upAxis.x = 0;
    upAxis.y = 0x1000;
    upAxis.z = 0;
    shallowOnly = false;
    depthByDir = false;
    depthDir.x = 0x1000;
    depthDir.y = 0;
    depthDir.z = 0;
    depthDirMinDot = -0x1000;
    edgeToleranceShift = 2;
    edgeContacts = true;
    tolerantWallEdges = false;
}

// @symbol func_020396dc
/* A prism record's index in the file's triangle table (records are 0x10). */
extern "C" unsigned func_020396dc(dBgW_Kc *self, KCL_Tri *tri) {
    return ((unsigned)tri - (unsigned)self->kclFile->tris) >> 4;
}

// @symbol _ZN7dBgW_Kc9Virtual08Ev
void dBgW_Kc::Virtual08() {}
