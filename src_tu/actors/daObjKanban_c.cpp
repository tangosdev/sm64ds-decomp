//cpp
/* Reconstructed translation unit.
 * ov085/daObjKanban_c  (7 functions)
 *
 * The wall sign Mario reads ("KANBAN" in the cartridge's own registry). The
 * class identity is the ROM's: ov085 0x021303cc holds the length-prefixed
 * string "13daObjKanban_c", 0x021303c0 is the __si_class_type_info record
 * that names it, and 0x02130400 is its vtable. No part of this name is
 * coined.
 *
 * FUNCTION ORDER IS ROM-ASCENDING. This TU disables deferred code generation
 * so CodeWarrior emits each definition where it stands. That is also what
 * puts the destructor pair out in the cartridge's own order: ov085 has D1 at
 * 0x0212edac BELOW D0 at 0x0212edf8 and no D2 at all, and an out-of-line
 * destructor under DEFERRED codegen comes out D2, D0, D1. The two sibling TUs
 * earlier in this same overlay, src/actors/daMip_c.cpp and
 * src_tu/actors/daC_Jugem_c.cpp, have the identical destructor shape and are
 * built the same way.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x0212edac  _ZN13daObjKanban_cD1Ev.cpp
 *   [1] 0x0212edf8  _ZN13daObjKanban_cD0Ev.cpp
 *   [2] 0x0212ee58  _ZN13daObjKanban_c16CleanupResourcesEv.cpp
 *   [3] 0x0212ee7c  _ZN13daObjKanban_c6RenderEv.cpp
 *   [4] 0x0212eea4  _ZN13daObjKanban_c8BehaviorEv.cpp
 *   [5] 0x0212f1b0  _ZN13daObjKanban_c13InitResourcesEv.cpp
 *   [6] 0x0212f244  d_a_obj_kanban.c
 *
 * The last of those, d_a_obj_kanban.c, is absent from build/tu_map.json's
 * span: tu_map segments on symbol NAME, and `daObjKanban_c_classInit` is
 * neither `func_ov085_*` nor `_ZN13daObjKanban_c*`, so nothing labels it. It
 * is contiguous -- 0x0212f244 + 0x38 = 0x0212f27c, which is also where this
 * overlay's .text ends -- and it is this class's own factory: it allocates
 * this class's size and installs this class's vtable. The same was true of
 * daMip_c's and daC_Jugem_c's factories earlier in the overlay, and it
 * belongs here for the same reason.
 *
 * FUNCTIONS cannot carry per-member declarations the way types can -- a class
 * member function may not sit inside a linkage-specification region -- so
 * every external call this TU makes is declared once, below, with C linkage,
 * on one reconciled signature. Where include/decl_common.h already carries a
 * declaration of a symbol this TU names, the spelling below is that one, so
 * the tree's declaration plurality is unchanged.
 */
#include "daObjKanban_c.h"
#include "SharedFilePtr.h"
#include "types.h"

extern "C" {

/* ov085 statics. Both spellings are include/decl_common.h's, kept identical
 * to it so this file adds no new reading of either symbol. */
extern void *data_ov085_02130858;          /* the sign model's shared file */
extern struct Vector3 data_ov085_0212f29c; /* the collider's offset, in .rodata */

/* math / vector helpers */
extern int Vec3_HorzDist(Vector3 *a, Vector3 *b);
extern s16 Vec3_HorzAngle(Vector3 *a, Vector3 *b);
extern int Vec3_ApproachHorz(Vector3 *out, Vector3 *a, int maxStep);
extern int _Z14ApproachLinearRsss(s16 *p, s16 target, s16 step);
extern int AngleDiff(int a, int b);
extern s16 data_02082214[];

/* actor / collision plumbing */
extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void _ZN5dCc_c5ClearEv(void *self);
extern void _ZN5dCc_c6UpdateEv(void *self);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(char *self);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(char *self, char *actor, const struct Vector3 *pos, int a, unsigned int b, unsigned int c, unsigned int d);

/* model loading */
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(char *self, void *file, int a, int b);

/* the player side of the conversation */
extern int _ZN6Player12GetTalkStateEv(void *thisp);
extern int _ZN6Player9StartTalkER7fBase_cb(void *actor, void *self, int b);
extern void _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh(void *actor, void *self, unsigned int a, Vector3 *pos, unsigned int b, unsigned int c);
extern int func_ov002_020bec9c(void *c, unsigned int a, int b, int d, unsigned short e);

}

#pragma defer_codegen off

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- _ZN13daObjKanban_cD1Ev 0x0212edac size 0x4c,
                           _ZN13daObjKanban_cD0Ev 0x0212edf8 size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjKanban_cD1Ev
// @symbol _ZN13daObjKanban_cD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 *
 * ONE definition, TWO ROM functions. mwccarm emits the deleting variant D0
 * and the complete variant D1 from this single declaration, so D0 has no
 * source of its own; with deferred code generation off they land in the
 * cartridge's order, D1 first at 0x0212edac and D0 at 0x0212edf8.
 *
 * The body is empty because there is nothing to write. Two vtable stores and
 * three destructor calls, every one a consequence of
 * `struct daObjKanban_c : dBgActor_c` and the one member that declaration
 * types: its own vptr, then dCcAcPos_c (0x320), then dBgActor_c's -- inlined,
 * because dBgActor_c's destructor is defined in its class body -- then
 * dBgActor_c's dBgW_KcMbg and Model, then dActor_c. D0's deallocation is an
 * inline operator delete reached through fBase_c, which is why nothing here
 * mentions a heap.
 */
daObjKanban_c::~daObjKanban_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN13daObjKanban_c16CleanupResourcesEv, 0x0212ee58, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjKanban_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * One release. A wall sign owns exactly one shared file -- the sign model --
 * and gives it back on the way out.
 *
 * `SharedFilePtr::Release` is spelt as the member call it is, so the compiler
 * emits _ZN13SharedFilePtr7ReleaseEv itself.
 */
int daObjKanban_c::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov085_02130858))->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN13daObjKanban_c6RenderEv, 0x0212ee7c, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjKanban_c6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * The legacy file reached slot 5 of the object at this+0xd4 through a local
 * shadow class, because it had no layout to name it with. It has one now:
 * 0xd4 is dBgActor_c::mModel, and slot 5 of _ZTV5Model is Model::Render, so
 * the call is spelt as the member it is. The null argument is the ROM's --
 * this actor draws its model at its own scale.
 */
int daObjKanban_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN13daObjKanban_c8BehaviorEv, 0x0212eea4, size 0x30c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjKanban_c8BehaviorEv
/* recovered: real C++ method; field access via char* this for layout until header grows */
int daObjKanban_c::Behavior()
{
    char *self = (char *)this;
    char *tgt;
    Vector3 mine, pos, tpos;

    tgt = *(char **)(self + 0x360);
    if (tgt != 0) {
        mine.x = *(int *)(self + 0x5c);
        mine.y = *(int *)(self + 0x60);
        mine.z = *(int *)(self + 0x64);

        pos.x = *(int *)(self + 0x5c);
        pos.y = *(int *)(self + 0x60);
        pos.z = *(int *)(self + 0x64);

        pos.x += (int)(((s64)data_02082214[(*(u16 *)(self + 0x8e) >> 4) << 1] * 0x5a000 + 0x800) >> 12);
        pos.z += (int)(((s64)data_02082214[((*(u16 *)(self + 0x8e) >> 4) << 1) + 1] * 0x5a000 + 0x800) >> 12);

        {
            int *p = (int *)(tgt + 0x5c);
            tpos.x = p[0];
            tpos.y = p[1];
            tpos.z = p[2];
        }

        switch (_ZN6Player12GetTalkStateEv(tgt)) {
        case 0:
            switch (*(u8 *)(self + 0x364)) {
            case 0:
                if (Vec3_HorzDist(&tpos, &pos) < 0x32000) {
                    *(u8 *)(self + 0x364) += 1;
                } else if (_Z14ApproachLinearRsss((s16 *)(tgt + 0x8e), Vec3_HorzAngle(&tpos, &pos), 0x800)) {
                    *(u8 *)(self + 0x364) += 1;
                    func_ov002_020bec9c(tgt, 1, 0, 0x1000, 0);
                }
                break;
            case 1:
                if (Vec3_ApproachHorz((Vector3 *)(tgt + 0x5c), &pos, 0xa000)) {
                    func_ov002_020bec9c(tgt, 0, 0, 0x1000, 0);
                    *(u8 *)(self + 0x364) += 1;
                }
                break;
            case 2:
                if (_Z14ApproachLinearRsss((s16 *)(tgt + 0x8e), (s16)(*(s16 *)(self + 0x8e) + 0x8000), 0x800)) {
                    *(s16 *)(self + 0x366) = 0;
                    if (*(int *)(self + 8) != 0xffff)
                        *(s16 *)(self + 0x366) = (s16)*(int *)(self + 8);
                    _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh(
                        tgt, self, (unsigned int)*(s16 *)(self + 0x366), &mine, 0, 1);
                    *(u8 *)(self + 0x364) = 0;
                }
                break;
            default:
                break;
            }
            break;
        case 1:
            break;
        default:
            *(int *)(self + 0x360) = 0;
            break;
        }
    } else {
        void *other;
        Vector3 opos;
        int isPlayer;
        int *pO;

        if ((*(u32 *)(self + 0x340) & 0x8000000) != 0) {
            other = _ZN8dActor_c10FindWithIDEj(*(unsigned int *)(self + 0x344));
            if (other != 0) {
                isPlayer = (*(u16 *)((char *)other + 0xc) == 0xbf);
                if (isPlayer != 0) {
                    pO = (int *)((char *)other + 0x5c);
                    opos.x = pO[0];
                    opos.y = pO[1];
                    opos.z = pO[2];
                    if (AngleDiff(Vec3_HorzAngle((Vector3 *)(self + 0x5c), &opos), *(s16 *)(self + 0x8e)) < 0x4000) {
                        if (_ZN6Player9StartTalkER7fBase_cb(other, self, 0))
                            *(void **)(self + 0x360) = other;
                    }
                }
            }
        }
    }
    _ZN5dCc_c5ClearEv(self + 0x320);
    _ZN5dCc_c6UpdateEv(self + 0x320);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN13daObjKanban_c13InitResourcesEv, 0x0212f1b0, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjKanban_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
int daObjKanban_c::InitResources()
{
  struct Vector3 v;
  void* file = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov085_02130858);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, file, 1, -1);
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
  v.x = data_ov085_0212f29c.x;
  v.y = data_ov085_0212f29c.y;
  v.z = data_ov085_0212f29c.z;
  _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(((char*)this) + 0x320, ((char*)this), &v, 0x78000, 0x64000, 0x4800002, 0);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- daObjKanban_c_classInit, 0x0212f244, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol daObjKanban_c_classInit
/* The registry factory behind the KANBAN profile. It allocates 0x368 -- this
 * class's own sizeof -- and installs this class's vtable, which is the second
 * of the two independent witnesses the header's layout is read from.
 *
 * Reconstructed source-style name: SM64DS proves daObjKanban_c through RTTI,
 * allocation size, vtable identity and the KANBAN registry profile; later EAD
 * lineage supplies classInit. The exact original spelling is not preserved.
 * Historical alias: WallSign_Spawn.
 *
 * `return new daObjKanban_c()` is the whole body: the synthesized constructor
 * is what runs dBgActor_c's base step, stores the vptr and then constructs the
 * dCcAcPos_c at 0x320, in that order, and `operator new` is fBase_c's.
 * Spelling the vptr store by hand instead would be wrong here as well as
 * longer -- this TU emits _ZTV13daObjKanban_c itself, so the store has to be
 * the compiler's own. */
extern "C" daObjKanban_c *daObjKanban_c_classInit(void)
{
    return new daObjKanban_c();
}
