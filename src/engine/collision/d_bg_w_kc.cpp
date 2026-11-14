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

dBgW_Kc::dBgW_Kc() {
    kclFile = 0;
}

dBgW_Kc::~dBgW_Kc() {}

/* True when a divisor is too close to zero to divide by (|value| <= 8). */
bool dBgW_Kc::IsNearZeroDivisor(int value) {
    return value <= 8 && value >= -8;
}

/* True when a face normal's y lies strictly inside the wall band. */
bool dBgW_Kc::IsWallNormal(fx32 normalY) {
    return normalY < 0x600 && normalY > -0xccc;
}

/* Slope band of a face normal's y: 0 floor, 1 wall, 2 underside. */
int dBgW_Kc::ClassifyNormalY(int normalY) {
    if (normalY > 0x600)
        return 0;
    if (normalY > -0xccc)
        return 1;
    return 2;
}

/* The four header words of a fresh KCL file are file-relative offsets;
   rebase them into pointers. */
void dBgW_Kc::UpdateFileOffsets(KCL_File &file) {
    unsigned int base = (unsigned int)&file;
    file.positions = (s32(*)[3])(base + (int)file.positions);
    file.normals = (s16(*)[3])(base + (int)file.normals);
    file.tris = (KCL_Tri *)(base + (int)file.tris);
    file.octree = (char *)(base + (int)file.octree);
}

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

/* A prism record's index in the file's triangle table (records are 0x10). */
unsigned func_020396dc(dBgW_Kc *self, KCL_Tri *tri) {
    return ((unsigned)tri - (unsigned)self->kclFile->tris) >> 4;
}

void dBgW_Kc::Virtual08() {}
