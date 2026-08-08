//cpp
#include "types.h"
// @symbol _ZN5Stage14GraphCallback2EP12SceneRelated
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Stage.h"
// Stage::GraphCallback2 - sets BG3 affine transform from SceneRelated
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

/* Static, and the parameter is what the mangled name has always said it is.
   This file used to take `struct Stage *self` and then cast it to SceneRelated*
   on every use -- four casts in one call -- which is the shape of a declaration
   that lost an argument against a name that kept it. */
s32 Stage::GraphCallback2(SceneRelated *scene) {
    _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii(
        &reg_G2S_DB_BG3PA,
        (struct Matrix2x2*)&scene->unk4,
        scene->unk14,
        scene->unk18,
        scene->unk1c,
        scene->unk20);
    return 1;
}