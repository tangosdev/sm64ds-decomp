//cpp
#include "types.h"
#include "nitro/hw/registers.h"
// @symbol _ZN5Stage14GraphCallback2Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Stage.h"
// Stage::GraphCallback2 - sets BG3 affine transform from the callback object
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

/* extern "C", and the quotes are the whole point. This is a .cpp file, so a
   bare `extern` makes the mangled name an ordinary C++ identifier and the
   compiler mangles it a SECOND time -- the call went out to
   `_Z40_ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiiiPVtP9Matrix2x2iiii`, where the
   `_Z40` counts the 40 characters of the name being wrapped. Nothing defines
   that, while the real target sits at 0x02055278 with a source file of its own.
   The byte gate cannot see this: match.py compares relocated words as
   wildcards and never looks at what a call targets. tools/check_references.py
   is what catches it. */
extern "C" void _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii(vu16* reg, struct Matrix2x2* mat, s32 a, s32 b, s32 c, s32 d);

/* Nullary, and a member: the previous name here,
   _ZN5Stage14GraphCallback2EP12SceneRelated, guessed a SceneRelated* second
   argument that no caller ever passes. This is vtable slot 2 of
   dScStage_c::graphCallback_c (RTTI si-child of dGraph_c::callback_c), and
   every dispatcher -- func_02018ec0, func_02018efc -- calls it as
   `ldr r1,[r0]; ldr r1,[r1,#8]; blx r1` with r0 = the callback object and
   r1 never set as an argument. The "SceneRelated" this body reads is the
   callback object itself: `this`, vptr at +0 (never touched as data), a
   fixed-point matrix at +0x4 that the object's static-init constructor at
   0x02074edc seeds with 0x1000 (1.0). The Stage:: scope is the family's
   legacy naming for dScStage_c and moves when that class does. */
s32 Stage::GraphCallback2() {
    SceneRelated *scene = (SceneRelated *)this;
    _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii(
        REG_DB_BG3PA,
        (struct Matrix2x2*)&scene->unk4,
        scene->unk14,
        scene->unk18,
        scene->unk1c,
        scene->unk20);
    return 1;
}
