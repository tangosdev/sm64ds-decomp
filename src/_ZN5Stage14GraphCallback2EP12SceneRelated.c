// @symbol _ZN5Stage14GraphCallback2EP12SceneRelated
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Stage.h"
// Stage::GraphCallback2 - sets BG3 affine transform from SceneRelated
typedef int s32;
typedef unsigned short u16;
typedef volatile u16 vu16;

struct Matrix2x2 {
    s32 unk0;
    s32 unk4;
};

struct SceneRelated {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkc;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1c;
    s32 unk20;
    struct Matrix2x2* mat;
};

extern void _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii(vu16* reg, struct Matrix2x2* mat, s32 a, s32 b, s32 c, s32 d);

s32 _ZN5Stage14GraphCallback2EP12SceneRelated(struct Stage *self) {
    _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii(
        &reg_G2S_DB_BG3PA,
        (struct Matrix2x2*)((unsigned char*)&self->unk_004),
        ((struct SceneRelated*)self)->unk14,
        ((struct SceneRelated*)self)->unk18,
        ((struct SceneRelated*)self)->unk1c,
        ((struct SceneRelated*)self)->unk20);
    return 1;
}