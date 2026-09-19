//cpp
/* Reconstructed translation unit.
 * ov085/daObj_Mip_Key_c  (15 functions)
 *
 * The key Mip the rabbit carries ("OBJ_MIP_KEY" in the cartridge's own
 * registry). The class identity is the ROM's: ov085 0x021301a0 holds the
 * length-prefixed string "15daObj_Mip_Key_c", 0x02130194 is the
 * __si_class_type_info record that names it, and 0x021301d8 is its vtable.
 * No part of this name is coined.
 *
 * deslop
 *
 * FUNCTION ORDER IS ROM-ASCENDING. This TU disables deferred code generation
 * so CodeWarrior emits each definition where it stands. That is also what
 * puts the destructor pair out in the cartridge's own order: ov085 has D1 at
 * 0x0212cc88 BELOW D0 at 0x0212ccc0 and no D2 at all, and an out-of-line
 * destructor under DEFERRED codegen comes out D2, D0, D1. The two sibling TUs
 * either side of this one in this same overlay -- src/actors/daMip_c.cpp and
 * src_tu/actors/daC_Jugem_c.cpp -- have the identical destructor shape and are
 * built the same way.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0]  0x0212cc88  _ZN15daObj_Mip_Key_cD1Ev.cpp
 *   [1]  0x0212ccc0  _ZN15daObj_Mip_Key_cD0Ev.cpp
 *   [2]  0x0212cd0c  func_ov085_0212cd0c.cpp
 *   [3]  0x0212cd80  func_ov085_0212cd80.c
 *   [4]  0x0212d038  func_ov085_0212d038.c
 *   [5]  0x0212d108  func_ov085_0212d108.c
 *   [6]  0x0212d24c  func_ov085_0212d24c.c
 *   [7]  0x0212d268  func_ov085_0212d268.cpp
 *   [8]  0x0212d2b8  func_ov085_0212d2b8.cpp
 *   [9]  0x0212d374  _ZN15daObj_Mip_Key_c16CleanupResourcesEv.cpp
 *   [10] 0x0212d398  _ZN15daObj_Mip_Key_c16OnPendingDestroyEv.cpp
 *   [11] 0x0212d39c  _ZN15daObj_Mip_Key_c6RenderEv.cpp
 *   [12] 0x0212d3ec  _ZN15daObj_Mip_Key_c8BehaviorEv.cpp
 *   [13] 0x0212d46c  _ZN15daObj_Mip_Key_c13InitResourcesEv.cpp
 *   [14] 0x0212d4e8  d_a_obj_mip_key.c
 *
 * THE SEVEN func_ov085_* ABOVE ARE THIS CLASS'S OWN FILE-STATIC HELPERS, and
 * the whole overlay was searched to say so rather than assumed. Every ARM `bl`
 * in ov085's .text was decoded and every 32-bit word in the overlay image was
 * scanned for each helper's address:
 *
 *   0x0212cd0c  called 4x, all from 0x0212cd80's body (0x0212ced4, 0x0212cf04,
 *               0x0212cff0, 0x0212d00c)
 *   0x0212d268  called 2x, from 0x0212d108's body (0x0212d230) and from
 *               daObj_Mip_Key_c::InitResources (0x0212d4d0)
 *   0x0212d2b8  called 1x, from daObj_Mip_Key_c::Render (0x0212d3c4)
 *   0x0212cd80  never called directly; its address appears exactly once, as a
 *               word at 0x02130174
 *   0x0212d038  likewise, once, at 0x0213017c
 *   0x0212d24c  likewise, once, at 0x02130184
 *   0x0212d108  likewise, once, at 0x0213018c
 *
 * Those four words are the pointer-to-member halves of this class's own state
 * objects. They sit in .data immediately before _ZTI15daObj_Mip_Key_c
 * (0x02130194), nothing outside this range reads them, and the only code that
 * takes their addresses is the module's static-initialiser run at
 * 0x0212fa28..0x0212fa34, which installs them into the two state objects the
 * members below name (data_ov085_0213071c and data_ov085_0213072c). Nothing in
 * any other module refers to any of the fifteen: every relocs.txt in the tree
 * was searched for each address and ov085's own file is the only hit. So the
 * run is closed, and the TU is the whole 0x0212cc88..0x0212d528 span.
 *
 * The last member, d_a_obj_mip_key.c, is absent from build/tu_map.json's span
 * for the same reason the two sibling TUs' factories were: tu_map segments on
 * symbol NAME, and `daObj_Mip_Key_c_classInit` is neither `func_ov085_*` nor
 * `_ZN15daObj_Mip_Key_c*`, so nothing labels it. It is contiguous --
 * 0x0212d4e8 + 0x40 = 0x0212d528, the next class's destructor -- and it is this
 * class's own factory: it allocates this class's size and installs this class's
 * vtable.
 *
 * SHADOW TYPES STAY INSIDE THE MEMBER THAT RECOVERED THEM, and four of the
 * fifteen were recovered as C. C's `struct Vector3` is a plain aggregate; the
 * tree's C++ one declares an empty destructor (see include/types.h), which is
 * deliberately not free -- it keeps the object's stack home. So the two
 * C-derived members that built a vector on the stack keep a local aggregate of
 * their own here rather than silently acquiring the C++ type, while
 * func_ov085_0212d2b8, which was always C++ and always used the tree's
 * Vector3, keeps using it.
 *
 * FUNCTIONS cannot carry per-member declarations the way types can -- a class
 * member function may not sit inside a linkage-specification region -- so
 * every external call this TU makes is declared once, below, with C linkage,
 * on one reconciled signature. Where the legacy files disagreed, the more
 * complete observation wins: Sound::PlaySub returns a value (two members test
 * it in an `if`, so the `void` spelling one file used cannot be the one), and
 * func_ov085_0212d268 takes the pointer-to-member pair its own definition
 * takes rather than the `void *` two callers spelled it with.
 */
#include "daObj_Mip_Key_c.h"
#include "common.h"
#include "SharedFilePtr.h"

/* The state objects this class drives itself with. func_ov085_0212d268 is the
   setter: it stores the object and immediately calls the pointer-to-member at
   its offset 0. daObj_Mip_Key_c::Behavior calls the second one, at offset 8,
   and does that dispatch by hand. */
struct MipKeyHost;
typedef int (MipKeyHost::*MipKeyPMF)();
struct MipKeyHost { char pad[0x188]; MipKeyPMF *pp; };

extern "C" {

/* This class's own helpers, under the names the delink config gives them --
   they are file-static in the original and have no ROM name of their own. */
void func_ov085_0212cd0c(char *self);
int  func_ov085_0212d268(MipKeyHost *c, MipKeyPMF *p);
void func_ov085_0212d2b8(char *thiz);

/* math / vector helpers */
int   _Z14ApproachLinearRiii(int *cur, int tgt, int step);
s16   Vec3_HorzAngle(const void *a, const void *b);
s16   Vec3_VertAngle(const void *a, const void *b);
void  Vec3_Asr(Vector3 *dst, Vector3 *src, int sh);
u16   DecIfAbove0_Short(u16 *p);
void  Matrix4x3_FromRotationY(void *m, int angY);
void  Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);
void  Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
void  MulVec3Mat4x3(const void *v, const void *m, void *out);

/* dActor_c / fBase_c, reached by their ROM names. The mangled spellings are
   kept wherever the real declaration takes a Fix12<int> by value, because
   mwccarm passes those differently at the call site than a loose spelling. */
void *_ZN8dActor_c13ClosestPlayerEv(void *thiz);
void  _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(void *thiz, void *cc);
void  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
          void *thiz, void *sm, void *mtx, int rad, int height, u32 flags);
void  _ZN7fBase_c18MarkForDestructionEv(void *thiz);

/* Player / Message */
int   _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
          void *thiz, void *actor, u32 id, const void *pos, u32 a, u32 b);
int   _ZN6Player18HasFinishedTalkingEv(void *thiz);
void  _ZN6Player17SetNoControlStateEhih(void *thiz, u8 a, int b, u8 c);
void  _ZN7Message13DisplaySavingEt(u16 id);
void  _ZN7Message7EndTalkEv(void);

/* Sound. The return type is load-bearing: two members test PlaySub's result in
   an `if`, so the `void` spelling func_ov085_0212cd0c's legacy file used cannot
   be the one. The fourth parameter stays Fix12i. */
int   _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, Fix12i d, u8 loop);

/* Model / ShadowModel, by their ROM names -- these three are called on raw
   offsets into the object rather than through the class declarations. */
void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
int   _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *file, int a, int b);
void  _ZN11ShadowModel12InitCylinderEv(void *thiz);

/* arm9 */
void  func_02012790(int a);
void  func_02013868(int t, int x);
void  StartMinigameMenu(u8 a);

/* data this TU reads */
extern int  *data_0209f318;
extern int   data_0209caa0[];
extern u8    data_0209d660;
extern u8    data_0209d684;
extern Matrix4x3 data_020a0e68;
extern char  data_ov085_021305d8;          /* SharedFilePtr, shared with daMip_c */
extern MipKeyPMF data_ov085_0213071c;      /* state object -- loose */
extern MipKeyPMF data_ov085_0213072c;      /* state object -- carried */

}

#define AT(p, off) ((void*)(int)((char*)(p) + (off)))

#pragma defer_codegen off

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- _ZN15daObj_Mip_Key_cD1Ev 0x0212cc88 size 0x38,
                           _ZN15daObj_Mip_Key_cD0Ev 0x0212ccc0 size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObj_Mip_Key_cD1Ev
// @symbol _ZN15daObj_Mip_Key_cD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 *
 * ONE definition, TWO ROM functions. mwccarm emits the deleting variant D0
 * and the complete variant D1 from this single declaration, so D0 has no
 * source of its own; with deferred code generation off they land in the
 * cartridge's order, D1 first at 0x0212cc88 and D0 at 0x0212ccc0.
 *
 * The body is empty because there is nothing to write: one vtable store and
 * three destructor calls, every one a consequence of
 * `struct daObj_Mip_Key_c : dEnemyBase_c` and the members that declaration
 * types -- its own vptr, then ShadowModel (0x160), Model (0x110) in reverse
 * declaration order, then dEnemyBase_c::~dEnemyBase_c. D0's deallocation is an
 * inline operator delete reached through dEnemyBase_c, this class's immediate
 * base, which is why nothing here mentions a heap.
 *
 * This body is the evidence for the header, and daObj_Mip_Key_c_classInit at
 * the bottom of this file is the second, independent witness: it constructs
 * the same types at the same offsets.
 */
daObj_Mip_Key_c::~daObj_Mip_Key_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov085_0212cd0c, 0x0212cd0c, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212cd0c
/* The common tail of the talk state machine below: stop the sound, end the
   message, give the camera and the player their flags back, and retire. */
extern "C" void func_ov085_0212cd0c(char *c)
{
    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x28, 0x7f, 0, 0x7444, 0);
    _ZN7Message7EndTalkEv();
    {
        int *p = (int *)(((int)data_0209f318 + 0x154));
        *p = *p & ~8;
    }
    {
        unsigned short *p = (unsigned short *)(((int)(*(char **)(c + 0x18c)) + 0x6ce));
        *p = *p & ~0x800;
    }
    _ZN7fBase_c18MarkForDestructionEv(c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov085_0212cd80, 0x0212cd80, size 0x2b8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212cd80
/* The carried state: the key rides above the player who caught it and runs the
   save-prompt conversation. Installed as the offset-8 pointer-to-member of
   data_ov085_0213072c. */
extern "C" {  /* .c-derived member: C linkage for the whole block */

/* The local this member built on the stack, as the plain aggregate C gave it.
   The tree's C++ Vector3 declares an empty destructor and is a different object
   for stack-home purposes. */
struct MipKeyCarryVec3 { Fix12i x, y, z; };

int func_ov085_0212cd80(char* self)
{
    char* other = *(char**)(self + 0x18c);
    volatile struct MipKeyCarryVec3 v;
    unsigned char gb;
    int* s;

    if (other == 0) return 1;

    s = (int*)AT(other, 0x5c);
    v.x = s[0];
    v.y = s[1];
    v.z = s[2];
    v.y = s[1] + 0xc8000;
    _Z14ApproachLinearRiii((int*)(self + 0x60), s[1] + 0xc8000, 0xa000);

    *(int*)(self + 0x5c) = v.x;
    *(int*)(self + 0x64) = v.z;
    gb = data_0209d684;

    switch (*(int*)(self + 0x194)) {
    case 0:
        if (!_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x28, 0x12, 0x7f, 0x15ccc, 0))
            break;
        *(int*)(self + 0x194) = 1;
    case 1:
        if (data_0209d660 == 0) {
            if (gb == 1) {
                func_02012790(0x5e);
                _ZN7Message13DisplaySavingEt(0x295);
                *(int*)(self + 0x194) = 2;
            } else if (gb == 2) {
                func_02012790(0x5e);
                _ZN7Message13DisplaySavingEt(0x295);
                *(int*)(self + 0x194) = 3;
            } else if (gb == 3) {
                func_02012790(0x98);
                if (*(int*)(self + 0x198)) {
                    *(int*)(self + 0x194) = 0xa;
                } else {
                    func_ov085_0212cd0c(self);
                }
            }
        }
        break;
    case 2:
        if (data_0209d660 == 0) {
            if (*(int*)(self + 0x198)) {
                *(int*)(self + 0x194) = 0xa;
            } else {
                func_ov085_0212cd0c(self);
            }
        }
        break;
    case 3:
        if (data_0209d660 == 0) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x28, 0x7f, 0, 0x7444, 0);
            StartMinigameMenu(1);
            _ZN7Message7EndTalkEv();
            _ZN7fBase_c18MarkForDestructionEv(self);
        }
        break;
    case 4:
        {
            int r;
            r = _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(void**)(self + 0x18c), self, 0x18b, 0, 1, 0);
            if (r == 1) {
                *(int*)(self + 0x198) = 1;
                *(int*)(self + 0x194) = 0;
            }
        }
        break;
    case 6:
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x28, 0x12, 0x7f, 0x15ccc, 0)) {
            if (data_0209d660 == 0) {
                if (gb == 1) {
                    func_02012790(0x5e);
                    _ZN7Message13DisplaySavingEt(0x295);
                    *(int*)(self + 0x194) = 2;
                } else if (gb == 2) {
                    func_02012790(0x98);
                    func_ov085_0212cd0c(self);
                }
            }
        }
        break;
    case 10:
        if (_ZN6Player18HasFinishedTalkingEv(*(void**)(self + 0x18c)) == 1) {
            func_ov085_0212cd0c(self);
        }
        break;
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov085_0212d038, 0x0212d038, size 0xd0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212d038
/* The carried state's entry half: stop dead, take the player's controls, and
   pick which conversation to open from the id at +0x19c. Installed as the
   offset-0 pointer-to-member of data_ov085_0213072c. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212d038(char* c){
  int t;
  *(int*)(c+0xa4) = 0;
  *(int*)(c+0xa8) = 0;
  *(int*)(c+0xac) = 0;
  *(short*)(c+0x8c) = 0;
  t = *(int*)(c+0x19c);
  if (t != 0x4d) {
    if (t != 7) {
      func_02013868(t, *(int*)(*(char**)(c+0x18c)+8));
      _ZN6Player17SetNoControlStateEhih(*(void**)(c+0x18c), 2, 0x189, 1);
      *(int*)(c+0x194) = 0;
    } else {
      data_0209caa0[1] |= 0x40;
      _ZN6Player17SetNoControlStateEhih(*(void**)(c+0x18c), 2, 0x18c, 1);
      *(int*)(c+0x194) = 6;
    }
  } else {
    data_0209caa0[1] |= 0x80;
    _ZN6Player17SetNoControlStateEhih(*(void**)(c+0x18c), 2, 0x149, 1);
    *(int*)(c+0x194) = 6;
  }
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov085_0212d108, 0x0212d108, size 0x144 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212d108
/* The loose state: spin, home on the nearest player, and hand over to the
   carried state once he is close enough underneath. Installed as the offset-8
   pointer-to-member of data_ov085_0213071c. */
extern "C" {  /* .c-derived member: C linkage for the whole block */

/* The locals this member built on the stack, as the plain aggregate C gave
   them. See the note on func_ov085_0212cd80's. */
struct MipKeyChaseVec3 { s32 x, y, z; };

int func_ov085_0212d108(char* c)
{
    struct MipKeyChaseVec3 v;
    struct MipKeyChaseVec3 out;
    struct MipKeyChaseVec3 pp;
    char* player;
    s16* ap;
    int zero;
    struct MipKeyChaseVec3* ppos;

    ap = (s16*)(c + 0x8c);
    *ap = (s16)(*ap + 0x1000);
    if (*(unsigned short*)(c + 0x100) != 0) {
        return 1;
    }
    player = (char*)_ZN8dActor_c13ClosestPlayerEv(c);
    if (player == 0) {
        return 1;
    }

    zero = 0;
    ppos = (struct MipKeyChaseVec3*)(player + 0x5c);
    v.x = zero;
    v.y = zero;
    v.z = zero;
    out.x = zero;
    out.y = zero;
    out.z = zero;
    pp.x = ppos->x;
    pp.y = ppos->y;
    pp.z = ppos->z;
    *(s16*)(c + 0x94) = Vec3_HorzAngle((struct MipKeyChaseVec3*)(c + 0x5c), &pp);
    *(s16*)(c + 0x92) = Vec3_VertAngle((struct MipKeyChaseVec3*)(c + 0x5c), &pp);
    v.z = 0x14000;
    Matrix4x3_FromRotationY(&data_020a0e68, *(s16*)(c + 0x94));
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16*)(c + 0x92));
    MulVec3Mat4x3(&v, &data_020a0e68, &out);
    _Z14ApproachLinearRiii((int*)(c + 0xa4), out.x, 0x1000);
    _Z14ApproachLinearRiii((int*)(c + 0xa8), out.y, 0x1000);
    _Z14ApproachLinearRiii((int*)(c + 0xac), out.z, 0x1000);
    if (pp.y + 0x64000 > *(int*)(c + 0x60)) {
        *(void**)(c + 0x18c) = player;
        func_ov085_0212d268((MipKeyHost*)c, &data_ov085_0213072c);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov085_0212d24c, 0x0212d24c, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212d24c
/* The loose state's entry half: ten frames of grace, then fall. Installed as
   the offset-0 pointer-to-member of data_ov085_0213071c. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212d24c(char *p)
{
    *(short *)(p + 0x100) = 10;
    *(int *)(p + 0xa8) = 81920;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov085_0212d268, 0x0212d268, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212d268
/* The state setter: store the state object at +0x188 and run its entry half. */
extern "C" int func_ov085_0212d268(MipKeyHost *c, MipKeyPMF *p)
{ c->pp = p; MipKeyPMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov085_0212d2b8, 0x0212d2b8, size 0xbc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212d2b8
/* recovered: shared common types.
 * Rebuild the render matrix at +0x12c from position and rotation, then drop
 * the cylinder shadow through the ShadowModel at +0x160.
 *
 * THE MATRIX COPY IS SPELLED FLAT ON PURPOSE. Two headers define Matrix4x3 --
 * common.h's `s32 m[12]` and math/Matrix.h's `Matrix3x3 r; Vector3 t` -- and
 * whichever a TU reaches first stands (see the note in include/common.h). This
 * TU reaches math/Matrix.h through Model.h, and that spelling is NOT a plain
 * aggregate, because Vector3 declares a destructor. Assigning it calls a
 * compiler-generated operator= and costs this member six words (0xbc -> 0xd4);
 * the 48-byte copy the cartridge does is the flat one this member's own legacy
 * file got when it reached common.h alone. */
extern "C" void func_ov085_0212d2b8(char *thiz)
{
    typedef struct { int w[12]; } MipKeyM48;
    Vector3 v;
    Vec3_Asr(&v, (Vector3*)(thiz + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(short*)(thiz + 0x8c), *(short*)(thiz + 0x8e), *(short*)(thiz + 0x90));
    *(MipKeyM48*)(thiz + 0x12c) = *(MipKeyM48*)&data_020a0e68;
    Matrix4x3_FromTranslation(&data_020a0e68,
        *(int*)(thiz + 0x5c) >> 3,
        (*(int*)(thiz + 0x60) - 0x32000) >> 3,
        *(int*)(thiz + 0x64) >> 3);
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        thiz, thiz + 0x160, thiz + 0x12c, 0x23000, 0x12c000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- _ZN15daObj_Mip_Key_c16CleanupResourcesEv, 0x0212d374,
                    size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObj_Mip_Key_c16CleanupResourcesEv
/* recovered: shared header, real C++ method.
 *
 * One release, and it is the SAME handle daMip_c::CleanupResources releases
 * first (data_ov085_021305d8). The key and the rabbit that carries it share a
 * file; each gives back its own reference.
 */
int daObj_Mip_Key_c::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov085_021305d8))->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN15daObj_Mip_Key_c16OnPendingDestroyEv, 0x0212d398,
                     size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObj_Mip_Key_c16OnPendingDestroyEv
/* recovered: shared header, real C++ method.
 *
 * Empty -- the ROM body is a single `bx lr`. The override exists to suppress
 * whatever the base does on pending destroy, not to do anything itself.
 */
void daObj_Mip_Key_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- _ZN15daObj_Mip_Key_c6RenderEv, 0x0212d39c, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObj_Mip_Key_c6RenderEv
/* recovered: named members + shared header, real C++ method.
 *
 * If the state object at +0x188 is the carried one, spin the key while it is
 * held; rebuild the matrix and shadow; then render the Model at +0x110 through
 * its own vtable slot +0x14.
 */
struct MipKeyRenderSubVt {
    char _pad0[0x14];
    void (*fn14)(void* self, int a);   /* 0x14 */
};
struct MipKeyRenderSub {
    struct MipKeyRenderSubVt* vt;      /* 0x0 */
};

int daObj_Mip_Key_c::Render()
{
    if (*(void**)((char*)&unk_188) == (void*)&data_ov085_0213072c) {
        *(s16*)((char*)&mAngleY) += 0x500;
    }
    func_ov085_0212d2b8((char*)this);
    {
        struct MipKeyRenderSub* s = (struct MipKeyRenderSub*)((char*)&mModel);
        s->vt->fn14(s, 0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- _ZN15daObj_Mip_Key_c8BehaviorEv, 0x0212d3ec, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObj_Mip_Key_c8BehaviorEv
/* recovered: named members + shared header, real C++ method.
 *
 * Behavior calls the state object's SECOND pointer-to-member, the one at
 * offset 8; func_ov085_0212d268 calls the one at offset 0. The dispatch is
 * spelled out because this member reads the pair's representation directly. */
int daObj_Mip_Key_c::Behavior()
{
  DecIfAbove0_Short((unsigned short*)((char*)&mStateTimer));
  void* o = *(void**)((char*)&unk_188);
  if(*(int*)((char*)o+8)){
    char* base = (char*)o+8;
    int adj = *(int*)(base+4);
    char* self = ((char*)this) + (adj>>1);
    void* fn;
    if(adj&1){ void* vt=*(void**)self; fn=*(void**)((char*)vt + *(int*)base); }
    else fn=*(void**)base;
    ((void(*)(char*))fn)(self);
  }
  int s = mVertSpeed + mVertAccel;
  int lim = mTerminalVelocity;
  if(s >= lim) lim = s;
  int t = unk_0ac;
  mVertSpeed = lim;
  unk_0ac = t;
  _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(((char*)this), 0);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- _ZN15daObj_Mip_Key_c13InitResourcesEv, 0x0212d46c,
                     size 0x7c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObj_Mip_Key_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method.
 *
 * The shared file handle is daMip_c's -- the rabbit and the key it carries come
 * out of one file, and each takes its own reference. The spawn parameter at
 * +0x19c is the id func_ov085_0212d038 later branches on, and the object starts
 * in the loose state. */
int daObj_Mip_Key_c::InitResources()
{
    void* f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov085_021305d8);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x110, f, 1, -1) == 0)
        return 0;
    _ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel);
    unk_19c = param1 & 0xff;
    mTerminalVelocity = -0x3c000;
    unk_190 = 0;
    func_ov085_0212d268((MipKeyHost*)this, &data_ov085_0213071c);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- daObj_Mip_Key_c_classInit, 0x0212d4e8, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol daObj_Mip_Key_c_classInit
/* The registry factory behind the OBJ_MIP_KEY profile. It allocates 0x1a0 --
 * this class's own sizeof -- and installs this class's vtable, which is the
 * second of the two independent witnesses the header's layout is read from.
 *
 * Reconstructed source-style name: SM64DS proves daObj_Mip_Key_c through RTTI,
 * allocation size, vtable identity and the OBJ_MIP_KEY registry profile; later
 * EAD lineage supplies the classInit spelling. The exact original spelling is
 * not preserved. Historical project alias: RabbitKey_Spawn.
 *
 * `return new daObj_Mip_Key_c()` is the whole body: the synthesized constructor
 * is what stores the vptr and runs the Model and ShadowModel constructors the
 * ROM calls, in that order, and `operator new` is fBase_c's. Spelling the vptr
 * store by hand instead would be wrong here as well as longer -- this TU emits
 * _ZTV15daObj_Mip_Key_c itself, so the store has to be the compiler's own. */
extern "C" daObj_Mip_Key_c *daObj_Mip_Key_c_classInit(void)
{
    return new daObj_Mip_Key_c();
}
