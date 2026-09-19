//cpp
/* Reconstructed translation unit.
 * ov085/daC_Jugem_c  (33 functions)
 *
 * Lakitu ("C_JUGEM" in the cartridge's own registry). The class identity is
 * the ROM's: ov085 0x02130310 holds the length-prefixed string
 * "11daC_Jugem_c", 0x02130304 is the __si_class_type_info record that names
 * it, and 0x02130344 is its vtable. No part of this name is coined.
 *
 * FUNCTION ORDER IS ROM-ASCENDING. This TU disables deferred code generation
 * so CodeWarrior emits each definition where it stands. That is also what
 * puts the destructor pair out in the cartridge's own order: ov085 has D1 at
 * 0x0212d528 BELOW D0 at 0x0212d578 and no D2 at all, and an out-of-line
 * destructor under DEFERRED codegen comes out D2, D0, D1. The sibling TU
 * src/actors/daMip_c.cpp, one class earlier in this same overlay, has the
 * identical destructor shape and is built the same way.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0]  0x0212d528  _ZN11daC_Jugem_cD1Ev.cpp
 *   [1]  0x0212d578  _ZN11daC_Jugem_cD0Ev.cpp
 *   [2]  0x0212d5dc  func_ov085_0212d5dc.cpp
 *   [3]  0x0212d724  func_ov085_0212d724.c
 *   [4]  0x0212d73c  func_ov085_0212d73c.c
 *   [5]  0x0212d8ec  func_ov085_0212d8ec.c
 *   [6]  0x0212d9b8  func_ov085_0212d9b8.c
 *   [7]  0x0212db04  func_ov085_0212db04.cpp
 *   [8]  0x0212dbdc  func_ov085_0212dbdc.c
 *   [9]  0x0212dcfc  func_ov085_0212dcfc.c
 *   [10] 0x0212dd10  func_ov085_0212dd10.cpp
 *   [11] 0x0212ddc4  func_ov085_0212ddc4.cpp
 *   [12] 0x0212de5c  func_ov085_0212de5c.cpp
 *   [13] 0x0212df84  func_ov085_0212df84.cpp
 *   [14] 0x0212e078  func_ov085_0212e078.cpp
 *   [15] 0x0212e180  func_ov085_0212e180.c
 *   [16] 0x0212e19c  func_ov085_0212e19c.c
 *   [17] 0x0212e2ec  func_ov085_0212e2ec.c
 *   [18] 0x0212e310  func_ov085_0212e310.cpp
 *   [19] 0x0212e480  func_ov085_0212e480.c
 *   [20] 0x0212e4a4  func_ov085_0212e4a4.c
 *   [21] 0x0212e59c  func_ov085_0212e59c.c
 *   [22] 0x0212e5ac  func_ov085_0212e5ac.c
 *   [23] 0x0212e720  func_ov085_0212e720.c
 *   [24] 0x0212e728  func_ov085_0212e728.cpp
 *   [25] 0x0212e778  func_ov085_0212e778.cpp
 *   [26] 0x0212e858  func_ov085_0212e858.c
 *   [27] 0x0212ea90  _ZN11daC_Jugem_c16CleanupResourcesEv.cpp
 *   [28] 0x0212eacc  _ZN11daC_Jugem_c16OnPendingDestroyEv.cpp
 *   [29] 0x0212ead0  _ZN11daC_Jugem_c6RenderEv.cpp
 *   [30] 0x0212eb18  _ZN11daC_Jugem_c8BehaviorEv.cpp
 *   [31] 0x0212ebec  _ZN11daC_Jugem_c13InitResourcesEv.cpp
 *   [32] 0x0212ed54  d_a_c_jugem.c
 *
 * The last of those, d_a_c_jugem.c, is absent from build/tu_map.json's span:
 * tu_map segments on symbol NAME, and `daC_Jugem_c_classInit` is neither
 * `func_ov085_*` nor `_ZN11daC_Jugem_c*`, so nothing labels it. It is
 * contiguous -- 0x0212ed54 + 0x58 = 0x0212edac, the next class's destructor --
 * and it is this class's own factory: it allocates this class's size and
 * installs this class's vtable. The same thing was true of daMip_c's factory
 * one class earlier, and it belongs here for the same reason.
 *
 * SHADOW TYPES STAY INSIDE THE MEMBER THAT RECOVERED THEM. Each legacy file
 * recovered its own view of the state objects at 0x021307a0..0x02130840, and
 * the two views are complementary rather than contradictory: the state setter
 * at 0x0212e728 reads a pointer-to-member at offset 0 of the state object,
 * while Behavior calls a second one at offset 8. Both are reproduced under
 * their own names below rather than merged into a single invented struct.
 *
 * FUNCTIONS cannot carry per-member declarations the way types can -- a class
 * member function may not sit inside a linkage-specification region -- so
 * every external call this TU makes is declared once, below, with C linkage,
 * on one reconciled signature. Pointer parameters are spelled `void *` on
 * purpose: the ROM names already encode the real parameter types, and the
 * legacy files disagreed only about how to spell them locally. By-value
 * class parameters (Fix12<int>) and return types are NOT spelled loosely --
 * mwccarm passes the first differently at the call site, and the second is
 * load-bearing wherever a result is tested.
 */
#include "daC_Jugem_c.h"
#include "common.h"
#include "dActor_c.h"
#include "SharedFilePtr.h"

extern "C" {

/* camera */
void  _ZN6Camera9SetFlag_3Ev(void *cam);
void  _ZN6Camera9SetLookAtERK7Vector3(void *cam, const void *v);
void  _ZN6Camera6SetPosERK7Vector3(void *cam, const void *v);

/* math / vector helpers */
void  _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(void *cur, const void *tgt, int step);
void  _Z14ApproachLinearRsss(s16 *cur, s16 tgt, s16 step);
void  _Z14ApproachLinearRiii(int *cur, int tgt, int step);
int   Vec3_Dist(const void *a, const void *b);
int   Vec3_HorzDist(const void *a, const void *b);
s16   Vec3_HorzAngle(const void *a, const void *b);
s16   Vec3_VertAngle(const void *a, const void *b);
void  Vec3_Sub(void *out, const void *a, const void *b);
void  Vec3_Asr(void *dst, const void *src, int sh);
int   Vec3_ApproachHorz(void *cur, const void *tgt, int maxStep);
int   LenVec3(const void *v);
int   AngleDiff(int a, int b);
u16   DecIfAbove0_Short(u16 *p);
void  Matrix4x3_FromRotationY(void *m, int angY);
void  Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);
void  MulVec3Mat4x3(const void *v, const void *m, void *out);

/* dActor_c / dEnemyBase_c / fBase_c, reached by their ROM names.
   The mangled spellings are kept wherever the real declaration takes a
   Fix12<int> or an s8/s16 BY VALUE, because mwccarm passes those differently
   at the call site than the loose spelling would. */
void *_ZN8dActor_c13ClosestPlayerEv(void *thiz);
s16   _ZN8dActor_c18HorzAngleToCPlayerEv(void *thiz);
void  _ZN8dActor_c13SpawnSoundObjEj(void *thiz, u32 id);
void  _ZN8dActor_c9UpdatePosEP5dCc_c(void *thiz, void *cc);
int   _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
          void *thiz, void *sm, void *mtx, int rad, int height, u32 flags);
void  _ZN7fBase_c18MarkForDestructionEv(void *thiz);
void *_ZN7fBase_cnwEj(u32 size);
void  _ZN12dEnemyBase_cC2Ev(void *thiz);

/* Player / Message */
int   _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
          void *thiz, void *actor, u32 id, const void *pos, u32 a, u32 b);
int   _ZN6Player12GetTalkStateEv(void *thiz);
int   _ZN6Player18HasFinishedTalkingEv(void *thiz);
int   _ZN6Player17SetNoControlStateEhih(int thiz, u8 a, int b, u8 c);

/* Sound. The return type is load-bearing: two members test PlaySub's result
   in an `if`, so the `void` spelling one legacy file used cannot be the one.
   The fourth parameter stays Fix12i (wall 6az). */
int   _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, Fix12i d, bool loop);
u32   _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 handle, u32 a, u32 b, const void *pos, u32 d);

/* ModelAnim / TextureSequence -- these two stay mangled for the same
   Fix12<int>-by-value reason. */
void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *f, int a, Fix12i b, u32 c);
void  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *thiz, BTP_File &f, int a, Fix12i b, u32 c);
void  _ZN9ModelAnimC1Ev(void *thiz);
void  _ZN15TextureSequenceC1Ev(void *thiz);
void  _ZN11ShadowModelC1Ev(void *thiz);
int   _ZN15TextureSequence6UpdateER15ModelComponents(void *thiz, void *mc);
void  _ZN9Animation7AdvanceEv(void *thiz);

/* other overlays / arm9 */
void  func_ov002_020c3e8c(void);
int   func_ov002_020c3ea0(void *p);
void  func_ov002_020c3f18(void *p);
void  func_ov002_020c3f2c(void *p);
void  func_ov002_020d228c(void *p);
void  func_ov002_020e4374(void *p, int *a, int *b);
void  func_0201f32c(int a);
void  func_02012790(int a);

/* this overlay's own data */
extern void          *data_0209f318;
extern int            data_0209caa0[];
extern u8             data_0209d66c;
extern u8             data_0209d6bc;
extern u8             data_0209f284;
extern s8             data_0209f2f8;
extern short          data_02082214[];
extern Matrix4x3      data_020a0e68;
extern int            data_ov085_02130840[];
extern Vector3        data_ov085_0213084c;
extern char           data_ov085_0213074c[]; /* SharedFilePtr; loaded file at +4 */
extern char           data_ov085_02130744[];
extern char           data_ov085_0213073c[];
extern void          *_ZTV11daC_Jugem_c;

}

/* The state objects this class drives itself with. Each is a pair of
   pointers-to-member: the setter at 0x0212e728 calls the one at offset 0,
   Behavior calls the one at offset 8. */
struct JugemHost;
typedef int (JugemHost::*JugemInitPMF)();
struct JugemHost { char pad[0x1ec]; JugemInitPMF *pp; };

extern "C" {
int func_ov085_0212e728(JugemHost *c, JugemInitPMF *p);
extern JugemInitPMF data_ov085_021307a0;
extern JugemInitPMF data_ov085_021307b0;
extern JugemInitPMF data_ov085_021307c0;
extern JugemInitPMF data_ov085_021307d0;
extern JugemInitPMF data_ov085_021307e0;
extern JugemInitPMF data_ov085_021307f0;
extern JugemInitPMF data_ov085_02130790;
extern JugemInitPMF data_ov085_02130800;
extern JugemInitPMF data_ov085_02130810;
extern JugemInitPMF data_ov085_02130820;
extern JugemInitPMF data_ov085_02130830;
}

#define AT(p, off) ((void*)(int)((char*)(p) + (off)))

#pragma defer_codegen off

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- _ZN11daC_Jugem_cD1Ev 0x0212d528 size 0x50,
                          _ZN11daC_Jugem_cD0Ev 0x0212d578 size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daC_Jugem_cD1Ev
// @symbol _ZN11daC_Jugem_cD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 *
 * ONE definition, TWO ROM functions. mwccarm emits the deleting variant
 * D0 and the complete variant D1 from this single declaration, so D0 has
 * no source of its own; with deferred code generation off they land in
 * the cartridge's order, D1 first at 0x0212d528 and D0 at 0x0212d578.
 *
 * The body is empty because there is nothing to write: one vtable store
 * and six destructor calls, every one a consequence of
 * `struct daC_Jugem_c : dEnemyBase_c` and the members that declaration
 * types -- its own vptr, then ShadowModel (0x218), ShadowModel (0x1f0),
 * TextureSequence (0x1d8), ModelAnim (0x174), ModelAnim (0x110) in
 * reverse declaration order, then dEnemyBase_c::~dEnemyBase_c. D0's
 * deallocation is an inline operator delete reached through
 * dEnemyBase_c, this class's immediate base, which is why nothing here
 * mentions a heap.
 */
daC_Jugem_c::~daC_Jugem_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov085_0212d5dc, 0x0212d5dc, size 0x148 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212d5dc
/* recovered: shared common types */
struct Range { int a, b, c, d, e, f; };
extern "C" int func_ov085_0212d5dc(char* c) {
  Range r;
  void* cam = *(void**)&data_0209f318;
  _ZN6Camera9SetFlag_3Ev(cam);
  r.a = -0x4b0000;
  r.b = 0x19f000;
  r.c = 0x1a90000;
  r.d = -0x4b0000;
  r.e = 0x250000;
  r.f = 0x1d4c000;
  _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(c+0x2b0, &r, 0x70000);
  _ZN6Camera9SetLookAtERK7Vector3(cam, c+0x2b0);
  _ZN6Camera6SetPosERK7Vector3(cam, c+0x2bc);
  Vec3_Dist(c+0x2b0, &r);
  (*(int*)AT(c, 0x2c8))++;
  if (*(int*)(c+0x2c8) > 0x64) {
    if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4b, 0x7f, 0, 0x7222, false) != 0) {
      *(int*)AT(cam, 0x154) &= ~8;
      *(int*)(c+0x98) = 0;
      *(int*)(c+0x2c8) = 0;
      *(int*)(c+0x2cc) = 0;
      *(int*)(c+0xa4) = 0;
      *(int*)(c+0xa8) = 0;
      *(int*)(c+0xac) = 0;
      *(short*)(c+0x8c) = 0;
      if (_ZN8dActor_c13ClosestPlayerEv(c) != 0) {
        func_ov002_020c3e8c();
        data_0209caa0[2] |= 0x80;
      }
      func_ov085_0212e728((JugemHost *)c, &data_ov085_021307e0);
    }
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov085_0212d724, 0x0212d724, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212d724
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212d724(char* c){
  *(char*)(c+0x2dc)=1;
  *(int*)(c+0x2c8)=0;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov085_0212d73c, 0x0212d73c, size 0x1b0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212d73c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212d73c(char *c)
{
    Vector3 v[3];
    void *cam;
    int spd;
    int len;

    cam = data_0209f318;
    _ZN6Camera9SetFlag_3Ev(cam);
    *(unsigned int *)(c + 0x2e4) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int *)(c + 0x2e4), 3, 0x182, (Vector3 *)(c + 0x74), 0);
    _Z14ApproachLinearRsss((s16 *)(c + 0x8e), Vec3_HorzAngle((Vector3 *)(c + 0x5c), &data_ov085_0213084c), 0x200);
    _Z14ApproachLinearRsss((s16 *)(c + 0x8c), Vec3_VertAngle((Vector3 *)(c + 0x5c), &data_ov085_0213084c), 0x200);
    _Z14ApproachLinearRiii((int *)(c + 0x98), 0x28000, 0x2000);
    _Z14ApproachLinearRsss((s16 *)(c + 0x94), Vec3_HorzAngle((Vector3 *)(c + 0x5c), &data_ov085_0213084c), (s16)*(int *)(c + 0x2cc));
    *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);
    *(int *)(((int)c + 0x2cc)) += 5;
    if (*(int *)(c + 0x2cc) > 0x800)
        *(int *)(c + 0x2cc) = 0x800;
    *(int *)(((int)c + 0x2c8)) += 1;
    if (*(int *)(c + 0x2c8) < 0x19)
        return 1;
    v[1].x = -0x50c000;
    v[1].y = 0x115000;
    v[1].z = 0x1d15000;
    spd = *(int *)(c + 0x98) >> 1;
    _Z14ApproachLinearR7Vector3RKS_5Fix12IiE((Vector3 *)(c + 0x2b0), (Vector3 *)(c + 0x5c), spd);
    _Z14ApproachLinearR7Vector3RKS_5Fix12IiE((Vector3 *)(c + 0x2bc), &v[1], spd);
    _ZN6Camera9SetLookAtERK7Vector3(cam, (Vector3 *)(c + 0x2b0));
    _ZN6Camera6SetPosERK7Vector3(cam, (Vector3 *)(c + 0x2bc));
    Vec3_Sub(&v[2], (Vector3 *)(c + 0x5c), &data_ov085_0213084c);
    len = LenVec3(&v[2]);
    if (len == 0 || len < 0x7d0000)
        func_ov085_0212e728((JugemHost *)c, &data_ov085_021307a0);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov085_0212d8ec, 0x0212d8ec, size 0xcc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212d8ec
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212d8ec(char* c) {
  volatile Vector3 look, pos; (void)&look; (void)&pos;
  void* cam;
  *(int*)(c + 0x98) = 0;
  *(int*)(c + 0x2c8) = 0;
  *(int*)(c + 0x2cc) = 0;
  *(int*)(c + 0x2e0) = 0;
  cam = data_0209f318;
  _ZN6Camera9SetFlag_3Ev(cam);
  *(int*)(c+0x2b0)=0xffc67000;
  *(int*)(c+0x2b4)=0x6ea000;
  *(int*)(c+0x2b8)=0x1212000;
  *(int*)(c+0x2bc)=0xffb65000;
  *(int*)(c+0x2c0)=0x1d5000;
  *(int*)(c+0x2c4)=0x17fc000;
  _ZN6Camera9SetLookAtERK7Vector3(cam, (Vector3*)(c + 0x2b0));
  _ZN6Camera6SetPosERK7Vector3(cam, (Vector3*)(c + 0x2bc));
  *(int*)(c + 0x2cc) = 0xa0;
  *(int*)(c + 0x5c) = data_ov085_02130840[0];
  *(int*)(c + 0x60) = data_ov085_02130840[1];
  *(int*)(c + 0x64) = data_ov085_02130840[2];
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov085_0212d9b8, 0x0212d9b8, size 0x14c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212d9b8
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212d9b8(char* c)
{
    void* pl = _ZN8dActor_c13ClosestPlayerEv(c);
    if (pl == 0) return 1;

    *(int*)(((int)c + 0x2c8)) += 1;
    *(int*)(((int)c + 0x2cc)) += 0x500;

    {
        int v = (short)*(int*)(c + 0x2cc);
        int idx = (int)((unsigned int)(v << 16) >> 16) >> 4;
        short e = data_02082214[idx * 2];
        _Z14ApproachLinearRiii((int*)(c + 0x60),
            (int)(((long long)e * 0x1a000 + 0x800) >> 12) + *(int*)(c + 0x2a8),
            0x10000000);
    }

    if (*(int*)(c + 0x2c8) == 0x32) {
        func_ov002_020c3ea0(pl);
    }
    *(short*)(c + 0x8e) = 0x6000;
    *(short*)(c + 0x94) = *(short*)(c + 0x8e);
    {
        int a1 = 0x4000;
        if (*(int*)(c + 0x2c8) >= 0x4b) a1 = 0x8000;
        _Z14ApproachLinearRiii((int*)(c + 0x98), a1, 0x1000);
    }

    *(int*)(c + 0x2e4) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        *(unsigned int*)(c + 0x2e4), 3, 0x182, (void*)(c + 0x74), 0);

    if (*(int*)(c + 0x2c8) > 0x78) {
        *(short*)(c + 0x8e) = 0x4000;
        *(short*)(c + 0x94) = *(short*)(c + 0x8e);
        func_ov085_0212e728((JugemHost *)c, &data_ov085_02130820);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov085_0212db04, 0x0212db04, size 0xd8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212db04
// @symbol func_ov085_0212db04
/* recovered: shared common types */

extern "C" {
int func_ov085_0212db04(char* c) {
  Vector3 look, pos;
  void* cam;
  *(int*)(c + 0x2c8) = 0;
  *(int*)(c + 0x2cc) = 0;
  *(int*)(c + 0x5c) = -0x5a0000;
  *(int*)(c + 0x60) = 0x2c0000;
  *(int*)(c + 0x64) = 0x1c6f000;
  *(int*)(c + 0x2a4) = *(int*)(c + 0x5c);
  *(int*)(c + 0x2a8) = *(int*)(c + 0x60);
  *(int*)(c + 0x2ac) = *(int*)(c + 0x64);
  cam = data_0209f318;
  _ZN6Camera9SetFlag_3Ev(cam);
  look.x = -0x304000;
  look.y = 0x3c1000;
  look.z = 0x1c77000;
  pos.x = -0x540000;
  pos.y = 0xe1000;
  pos.z = 0x19e4000;
  _ZN6Camera9SetLookAtERK7Vector3(cam, &look);
  _ZN6Camera6SetPosERK7Vector3(cam, &pos);
  *(int*)(c + 0x98) = 0;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov085_0212dbdc, 0x0212dbdc, size 0x120 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212dbdc
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212dbdc(char* c)
{
    char* p = (char*)_ZN8dActor_c13ClosestPlayerEv(c);
    int* pp;
    if (p == 0)
        return 1;
    pp = (int*)(((int)c + 0x2c8));
    *pp = *pp + 1;
    switch (*(int*)(c + 0x2c8)) {
    case 1:
        func_0201f32c(2);
        break;
    case 0x14:
        func_ov002_020c3f2c(p);
        break;
    case 0x28:
        data_0209d66c = 1;
        break;
    case 0x8c:
        func_0201f32c(3);
        break;
    case 0xee:
        data_0209d66c = 1;
        break;
    case 0xf0:
        {
            s16 a = *(s16*)(c + 0x2d4);
            s16 b;
            *(s16*)(p + 0x8c) = 0;
            *(s16*)(p + 0x8e) = a;
            *(s16*)(p + 0x90) = 0;
            b = *(s16*)(c + 0x2d4);
            *(s16*)(p + 0x92) = 0;
            *(s16*)(p + 0x94) = b;
            *(s16*)(p + 0x96) = 0;
            func_ov085_0212e728((JugemHost *)c, &data_ov085_021307f0);
        }
        break;
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov085_0212dcfc, 0x0212dcfc, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212dcfc
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212dcfc(char *p)
{
    *(int *)(p + 0x2a0) = 0;
    *(int *)(p + 0x2c8) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov085_0212dd10, 0x0212dd10, size 0xb4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212dd10
extern "C" {

int func_ov085_0212dd10(char* c)
{
    char* p = (char*)_ZN8dActor_c13ClosestPlayerEv(c);
    if (!p) return 1;
    {
        short v = *(short*)(c + 0x8e);
        short w = v + 0x8000;
        int* t = (int*)(((int)c + 0x2c8));
        *(short*)(p + 0x8c) = 0;
        *(short*)(p + 0x8e) = w;
        *(short*)(p + 0x90) = 0;
        *t += 1;
    }
    if (*(int*)(c + 0x2c8) == 0x5a) func_ov002_020c3f18(p);
    {
        int s = *(int*)(c + 0x2c8);
        if (s > 0x57 && s < 0x5b)
            _Z14ApproachLinearRsss((s16*)(c + 0x8c), 0x2000, 0x400);
        else
            _Z14ApproachLinearRsss((s16*)(c + 0x8c), 0x1000, 0x400);
    }
    if (*(int*)(c + 0x2c8) > 0x78)
        func_ov085_0212e728((JugemHost *)c, &data_ov085_021307c0);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov085_0212ddc4, 0x0212ddc4, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212ddc4
// @symbol func_ov085_0212ddc4
/* recovered: shared common types */

extern "C" {
int func_ov085_0212ddc4(char* c) {
  Vector3 look, pos;
  void* cam;
  *(int*)(c + 0x2c8) = 0;
  *(int*)(c + 0x2a0) = 0;
  *(short*)(c + 0x90) = 0;
  cam = data_0209f318;
  _ZN6Camera9SetFlag_3Ev(cam);
  look.x = 0xffadd000;
  look.y = 0x17e000;
  look.z = 0x1a29000;
  pos.x = 0xffa54000;
  pos.y = 0x1f4000;
  pos.z = 0x1ccf000;
  _ZN6Camera9SetLookAtERK7Vector3(cam, &look);
  _ZN6Camera6SetPosERK7Vector3(cam, &pos);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov085_0212de5c, 0x0212de5c, size 0x128 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212de5c
struct PlayerObj {
    char pad8c[0x8c];
    short v8c, v8e, v90;
    char pad2c8[0x2c8 - 0x92];
    int v2c8;
    char pad2dc[0x2dc - 0x2cc];
    unsigned char v2dc;
};
struct Other {
    char pad8c[0x8c];
    short v8c, v8e, v90;
    char pad723[0x723 - 0x92];
    unsigned char v723;
};
extern "C" int func_ov085_0212de5c(PlayerObj *c)
{
    Other *p = (Other *)((dActor_c *)c)->ClosestPlayer();
    if (p == 0)
        return 1;
    {
        short t = c->v8e + 0x8000;
        p->v8c = 0;
        p->v8e = t;
        p->v90 = 0;
    }
    c->v8e = ((dActor_c *)c)->HorzAngleToCPlayer();
    c->v2dc = 0;
    c->v8c = 0x1000;
    c->v90 = 0x800;
    {
        int *pp = (int *)(((int)c + 0x2c8));
        int n = *pp + 1;
        *pp = n;
    }
    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4b, 0x14, 0x7f, 0x15666, false);
    switch (c->v2c8) {
    case 1:
        p->v723 = 1;
        break;
    case 0x78:
        func_0201f32c(1);
        break;
    case 0x82:
        p->v723 = 0;
        break;
    case 0xb4:
        data_0209d66c = 1;
        break;
    case 0xd2:
        func_ov085_0212e728((JugemHost *)c, &data_ov085_021307b0);
        break;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov085_0212df84, 0x0212df84, size 0xf4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212df84
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" int func_ov085_0212df84(char* c)
{
    Vector3 look, pos;
    void* cam;
    void* player;
    *(int*)(c + 0x2a4) = -0x5a0000;
    *(int*)(c + 0x2a8) = 0x1c0000;
    *(int*)(c + 0x2ac) = 0x1a66000;
    player = _ZN8dActor_c13ClosestPlayerEv(c);
    if (player) {
        *(short*)(c + 0x2d4) = *(short*)((char*)player + 0x8e);
        func_ov002_020d228c(player);
    }
    cam = data_0209f318;
    _ZN6Camera9SetFlag_3Ev(cam);
    look.x = 0xff883000;
    look.y = 0x2ef000;
    look.z = 0x1a36000;
    pos.x = 0xffb18000;
    pos.y = 0x18c000;
    pos.z = 0x1a89000;
    _ZN6Camera9SetLookAtERK7Vector3(cam, &look);
    _ZN6Camera6SetPosERK7Vector3(cam, &pos);
    *(short*)(c + 0x100) = 0x79;
    *(int*)(c + 0x5c) = *(int*)(c + 0x2a4);
    *(int*)(c + 0x60) = *(int*)(c + 0x2a8);
    *(int*)(c + 0x64) = *(int*)(c + 0x2ac);
    *(int*)(c + 0x2a0) = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov085_0212e078, 0x0212e078, size 0x108 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212e078
extern "C" {

int func_ov085_0212e078(char* c)
{
    _Z14ApproachLinearRsss((short*)(c + 0x94), Vec3_HorzAngle(c + 0x5c, c + 0x2a4), 0x800);
    _Z14ApproachLinearRsss((short*)(c + 0x8e), *(short*)(c + 0x94), 0x800);
    *(unsigned int*)(c + 0x2e4) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        *(unsigned int*)(c + 0x2e4), 3, 0x182, c + 0x74, 0);
    if (AngleDiff(*(short*)(c + 0x94), Vec3_HorzAngle(c + 0x5c, c + 0x2a4)) < 0x2000) {
        Vec3_ApproachHorz(c + 0x5c, c + 0x2a4, 0x1e000);
        _Z14ApproachLinearRiii((int*)(c + 0x60), *(int*)(c + 0x2a8), 0x1e000);
    }
    if (*(unsigned short*)(c + 0x100) == 0) {
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4a, 0x7f, 0, 0x7222, 0) != 0) {
            if (_ZN6Player18HasFinishedTalkingEv(*(void**)(c + 0x2a0)) == 1) {
                _ZN7fBase_c18MarkForDestructionEv(c);
                data_0209f284 = 0;
            }
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov085_0212e180, 0x0212e180, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212e180
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212e180(char *c) {
    short r3 = *(short*)((char*)c + 0x8e);
    *(short*)((char*)c + 0x94) = r3;
    *(short*)((char*)c + 0x100) = 0x46;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov085_0212e19c, 0x0212e19c, size 0x150 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212e19c
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212e19c(char* c)
{
    _Z14ApproachLinearRsss((s16*)(c + 0x8e), _ZN8dActor_c18HorzAngleToCPlayerEv(c), 0x800);
    switch (*(int*)(c + 0x2d8)) {
    case 0:
        {
            u16* m = (u16*)AT(*(char**)(c + 0x2a0), 0x6ce);
            *m = *m | 0x400;
        }
        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
                *(void**)(c + 0x2a0), c, 0x182, (struct Vector3*)(c + 0x5c), 1, 0) == 1) {
            *(int*)AT(c, 0x2d8) += 1;
        }
        break;
    case 1:
        if (data_0209d6bc == 7) { *(int*)AT(c, 0x2d8) += 1; }
        break;
    case 2:
        if (data_0209d6bc == 7)
            break;
        func_02012790(0x24);
        data_0209f284 = 1;
        { *(int*)AT(c, 0x2d8) += 1; }
        break;
    case 3:
        if (data_0209d6bc == 9) { *(int*)AT(c, 0x2d8) += 1; }
        break;
    case 4:
        if (_ZN6Player12GetTalkStateEv(*(void**)(c + 0x2a0)) == 2)
            func_ov085_0212e728((JugemHost *)c, &data_ov085_02130830);
        break;
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov085_0212e2ec, 0x0212e2ec, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212e2ec
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212e2ec(void *r0) {
    int *p = data_0209caa0;
    p[2] |= 0x20000;
    *(int*)((char*)r0+0x2d8) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov085_0212e310, 0x0212e310, size 0x170 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212e310
// @symbol func_ov085_0212e310
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {




int func_ov085_0212e310(char *c)
{
    Vector3 in;
    Vector3 out;
    char *p;

    p = (char *)_ZN8dActor_c13ClosestPlayerEv(c);
    if (p == 0) {
        return 1;
    }

    *(int *)(p + 0x744) = *(int *)(c + 0x5c);
    *(int *)(p + 0x748) = *(int *)(c + 0x60);
    *(int *)(p + 0x74c) = *(int *)(c + 0x64);

    if (*(unsigned short *)(c + 0x100) == 0) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4a, 0x14, 0x7f, 0x15666, 0);
    }

    *(unsigned int *)(c + 0x2e4) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int *)(c + 0x2e4), 3, 0x182, (Vector3 *)(c + 0x74), 0);

    in.x = 0;
    in.y = 0;
    in.z = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    in.z = 0xc8000;
    Matrix4x3_FromRotationY(&data_020a0e68, 0);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    out.x += *(int *)(p + 0x5c);
    out.y += *(int *)(p + 0x60) + 0x70000;
    out.z += *(int *)(p + 0x64);

    _Z14ApproachLinearR7Vector3RKS_5Fix12IiE((Vector3 *)(c + 0x5c), &out, 0x14000);
    _Z14ApproachLinearRsss((short *)(c + 0x94), Vec3_HorzAngle((Vector3 *)(c + 0x5c), &out), 0x800);
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);

    if (Vec3_Dist((Vector3 *)(c + 0x5c), &out) < 0x14000) {
        *(void **)(c + 0x2a0) = p;
        func_ov085_0212e728((JugemHost *)c, &data_ov085_02130810);
    }

    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov085_0212e480, 0x0212e480, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212e480
// recovered name: daObj_Mip_Key_c_Kill
/* recovered: renamed to Class_Method */
/* daObj_Mip_Key_c::Kill - recovered from vtable slot identity */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212e480(char *p)
{
    *(int *)(p + 0x64) = 2457600;
    *(int *)(p + 0x2c8) = 0;
    *(short *)(p + 0x100) = 40;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov085_0212e4a4, 0x0212e4a4, size 0xf8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212e4a4
typedef struct V3 { int x, y, z; } V3;
/* This member's legacy file carried `#pragma opt_common_subs off`. That
   pragma is file-global last-wins in mwccarm, so it is bracketed here
   rather than left open -- with deferred code generation off the bracket
   binds, and every other member is compiled without it. */
#pragma opt_common_subs off
extern "C" {  /* .c-derived member: C linkage for the whole block */


int func_ov085_0212e4a4(unsigned int self)
{
    char *p = (char *)_ZN8dActor_c13ClosestPlayerEv((void *)self);
    if (p != 0) {
        V3 v = *(V3 *)(p + 0x5c);
        if ((data_0209caa0[2] & 0x10000) != 0 &&
            v.z > -0x28000 &&
            _ZN6Player17SetNoControlStateEhih((int)p, 0x12, -1, 0) != 0) {
            *(int *)(p + 0x744) = *(int *)(self + 0x5c);
            *(int *)(p + 0x748) = *(int *)(self + 0x60);
            *(int *)(p + 0x74c) = *(int *)(self + 0x64);
            *(int *)(self + 0x5c) = v.x;
            *(int *)(self + 0x60) = v.y;
            *(int *)(self + 0x64) = v.z;
            *(int *)(((long long)((int)(self + 0x5c)))) -= 0x3e8000;
            *(int *)(self + 0x60) = *(int *)(p + 0x644) + 0x3e8000;
            *(int *)(self + 0x2a4) = *(int *)(self + 0x5c);
            *(int *)(self + 0x2a8) = *(int *)(self + 0x60);
            *(int *)(self + 0x2ac) = *(int *)(self + 0x64);
            *(signed char *)(self + 0x2dc) = 0;
            func_ov085_0212e728((JugemHost *)self, &data_ov085_02130800);
        }
    }
    return 1;
}
}
#pragma opt_common_subs on

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov085_0212e59c, 0x0212e59c, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212e59c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212e59c(char *p)
{
    p[0x2dc] = 1; return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov085_0212e5ac, 0x0212e5ac, size 0x174 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212e5ac
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212e5ac(char* self)
{
    Vector3 in, out, plpos;
    char* pl;
    Vector3* src;
    void* p;

    pl = (char*)data_0209f318;
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;
    src = (Vector3*)(((int)pl + 0x8c));
    plpos.x = src->x;
    plpos.y = src->y;
    plpos.z = src->z;
    if (data_0209f2f8 == 0x2f) {
        *(int*)(self + 0x5c) = 0;
    } else {
        *(int*)(self + 0x5c) = 0x1086000;
        if (*(int*)(self + 0x2cc) == 0) {
            p = _ZN8dActor_c13ClosestPlayerEv(self);
            if (p != 0 && *(int*)((char*)p + 0x5c) > 0x1086000) {
                _ZN8dActor_c13SpawnSoundObjEj(self, 0);
                *(int*)(self + 0x2cc) = 1;
            }
        }
    }
    *(int*)(self + 0x60) = plpos.y;
    *(int*)(self + 0x64) = plpos.z;
    in.z = Vec3_HorzDist(&plpos, (Vector3*)(self + 0x5c));
    Matrix4x3_FromRotationY(&data_020a0e68, Vec3_HorzAngle(&plpos, (Vector3*)(self + 0x5c)));
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    *(int*)(((int)self + 0x5c)) += out.x;
    if (data_0209f2f8 == 0x2f) {
        if (*(int*)(self + 0x5c) > 0x79e000) {
            *(int*)(self + 0x5c) = 0x79e000;
        }
    }
    *(int*)(((int)self + 0x64)) += out.z;
    *(short*)(self + 0x94) = 0x8000 - *(short*)(pl + 0x17c);
    *(short*)(self + 0x92) = -*(short*)(pl + 0x17e);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov085_0212e720, 0x0212e720, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212e720
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212e720(void)
{
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov085_0212e728, 0x0212e728, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212e728
extern "C" int func_ov085_0212e728(JugemHost *c, JugemInitPMF *p)
{ c->pp = p; JugemInitPMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov085_0212e778, 0x0212e778, size 0xe0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212e778
extern "C" {
struct Vec3 { int x, y, z; };

typedef struct { int w[12]; } M48;

void func_ov085_0212e778(char* c)
{
    Vec3 v;
    Vec3_Asr(&v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    *(M48*)(c + 0x12c) = *(M48*)&data_020a0e68;
    Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(c + 0x5c) >> 3, (*(int*)(c + 0x60) - 0x38000) >> 3, *(int*)(c + 0x64) >> 3);
    *(M48*)(c + 0x240) = *(M48*)&data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x1f0, c + 0x240, 0x46000, 0x258000, 0xf);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov085_0212e858, 0x0212e858, size 0x238 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov085_0212e858
/* recovered: shared common types */
/* The matrix type below has to be local, and it is why this member was the
   last one to come good. Three of its statements copy a whole Matrix4x3,
   and the ROM does each as one flat 12-word block copy. common.h spells
   Matrix4x3 flat, `{ s32 m[12] }`, but math/Matrix.h spells the same type
   `{ Matrix3x3 r; Vector3 t; }` and this TU reaches THAT one first, through
   ModelAnim.h. The nested spelling is non-POD, so the copies scalarize
   member-wise: the function grows 0x238 -> 0x288, takes an extra
   callee-saved register and a 4-byte-larger frame. The sibling member at
   0x0212e778 was already written against a flat local type for this reason;
   this is the same fix. Canonicalising either header is a measured codegen
   hazard, not a tidy-up, so neither is touched.

   The vector locals, by contrast, were MEASURED not to need one. types.h's
   Vector3 declares an empty destructor under C++ -- deliberately, so that
   the arrays the ROM destroys elsewhere have something to destroy -- and a
   flat local copy of it was tried here first, on the theory that the empty
   destructor would pin each local to a stack home. It moved the size by
   nothing; the whole 0x50 was the matrix. The shared spelling stands. */
struct M48e858 { int w[12]; };
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov085_0212e858(char *c)
{
    struct Vector3 p;
    struct Vector3 z1;
    struct Vector3 off;
    struct Vector3 pp;
    struct Vector3 t;
    char *pl;
    int p1, p2;
    signed char lvl;
    struct Vector3 *ps;

    Vec3_Asr(&t, (struct Vector3*)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    *(M48e858*)(c + 0x12c) = *(M48e858*)&data_020a0e68;

    Matrix4x3_FromTranslation(&data_020a0e68,
        *(int*)(c + 0x5c) >> 3,
        (*(int*)(c + 0x60) - 0x38000) >> 3,
        *(int*)(c + 0x64) >> 3);
    *(M48e858*)(c + 0x240) = *(M48e858*)&data_020a0e68;

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x1f0, c + 0x240, 0x46000, 0x258000, 0xf);

    pl = (char*)_ZN8dActor_c13ClosestPlayerEv(c);
    if (pl == 0) return;

    /* early load of level, zero stacks, then player pos copy interleaved with cmp */
    lvl = data_0209f2f8;
    p.x = 0;
    p.y = 0;
    p.z = 0;
    z1.x = 0;
    z1.y = 0;
    z1.z = 0;
    off.x = 0;
    off.y = 0;
    off.z = 0;
    ps = (struct Vector3*)(pl + 0x5c);
    pp.x = ps->x;
    pp.y = ps->y;
    pp.z = ps->z;
    if (lvl == 0x2f) {
        p.x = 0;
    } else {
        p.x = 0x1086000;
    }
    p.y = pp.y;
    p.z = pp.z;

    z1.z = Vec3_HorzDist(&pp, &p);
    {
        short ang = Vec3_HorzAngle(&pp, &p);
        Matrix4x3_FromRotationY(&data_020a0e68, ang);
    }
    MulVec3Mat4x3(&z1, &data_020a0e68, &off);

    p.x = p.x + off.x;
    p.y = pp.y;
    p.z = p.z + off.z;
    Matrix4x3_FromTranslation(&data_020a0e68, p.x >> 3, p.y >> 3, p.z >> 3);

    func_ov002_020e4374(pl, &p1, &p2);

    *(M48e858*)(c + 0x270) = *(M48e858*)&data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x218, c + 0x270, p2, p1, 0xf);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- _ZN11daC_Jugem_c16CleanupResourcesEv, 0x0212ea90, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daC_Jugem_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Three releases, all this overlay's own, in the reverse of no particular
 * order -- it is simply the ROM's, and is reproduced verbatim.
 */
int daC_Jugem_c::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov085_0213074c))->Release();
    ((SharedFilePtr *)(&data_ov085_02130744))->Release();
    ((SharedFilePtr *)(&data_ov085_0213073c))->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- _ZN11daC_Jugem_c16OnPendingDestroyEv, 0x0212eacc, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daC_Jugem_c16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * Empty -- the ROM body is a single `bx lr`. The override exists to suppress
 * whatever the base does on pending destroy, not to do anything itself.
 */
void daC_Jugem_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- _ZN11daC_Jugem_c6RenderEv, 0x0212ead0, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daC_Jugem_c6RenderEv
/* recovered: named members + shared header, real C++ method */
/* The model-component slot Render drives is reached through its own vtable;
   this is that vtable's shape as the legacy file recovered it. */
struct Sub { virtual int g0(); virtual int g1(); virtual int g2();
             virtual int g3(); virtual int g4(); virtual int g5(void*); };
int daC_Jugem_c::Render()
{
  if (unk_2dc == 1) return 1;
  _ZN15TextureSequence6UpdateER15ModelComponents(((char*)this)+0x1d8, ((char*)this)+0x118);
  ((Sub*)((char*)&mModelAnim1))->g5(0);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- _ZN11daC_Jugem_c8BehaviorEv, 0x0212eb18, size 0xd4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daC_Jugem_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* Behavior calls the state object's SECOND pointer-to-member, the one at
   offset 8; the setter at 0x0212e728 calls the one at offset 0. */
struct BehC { char pad[0x10000]; };
typedef void (BehC::*BehPMF)();
struct BehState { char pad[8]; BehPMF fn; };
int daC_Jugem_c::Behavior()
{
  char* p=(char*)((BehC*)this);
  DecIfAbove0_Short((unsigned short*)(p+0x100));
  BehState* st=*(BehState**)(p+0x1ec);
  if(st->fn) (((BehC*)this)->*st->fn)();
  _ZN8dActor_c9UpdatePosEP5dCc_c(((BehC*)this), 0);
  _ZN9Animation7AdvanceEv(p+0x160);
  _ZN9Animation7AdvanceEv(p+0x1d8);
  if(*(BehState**)(p+0x1ec)==(BehState*)&data_ov085_021307d0){
    *(short*)(p+0x8c)=*(short*)(p+0x92);
    *(short*)(p+0x8e)=*(short*)(p+0x94);
    *(short*)(p+0x90)=*(short*)(p+0x96);
  }
  if(*(BehState**)(p+0x1ec)==(BehState*)&data_ov085_021307e0) return 1;
  if(*(int*)(p+0x2d0)==0) func_ov085_0212e858((char*)this);
  else func_ov085_0212e778((char*)this);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- _ZN11daC_Jugem_c13InitResourcesEv, 0x0212ebec, size 0x168 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daC_Jugem_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* SetAnim and SetFile stay mangled because their real signatures carry
   Fix12<int> (wall 6az). Everything else the members' own headers declare.

   Prepare is one of those now. It reads as a member call because it is a
   STATIC member -- the ROM passes it two argument registers and no this,
   which is why the mangled extern used to be the only spelling that fit.
   TextureSequence.h carries the disassembly that settles it. */
int daC_Jugem_c::InitResources()
{
  BMD_File* bmd = (BMD_File*)Model::LoadFile(*(SharedFilePtr*)data_ov085_0213074c);
  mModelAnim1.SetFile(bmd, 1, -1);
  Animation::LoadFile(*(SharedFilePtr*)data_ov085_02130744);
  TextureSequence::LoadFile(*(SharedFilePtr*)data_ov085_0213073c);
  mShadowModel1.InitCylinder();
  mShadowModel2.InitCylinder();
  TextureSequence::Prepare(
      **(BMD_File**)(data_ov085_0213074c + 4), **(BTP_File**)(data_ov085_0213073c + 4));
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
      &mModelAnim1, *(BCA_File**)(data_ov085_02130744 + 4), 0, 0x1000, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
      &mTextureSequence, **(BTP_File**)(data_ov085_0213073c + 4), 0, 0x1000, 0);
  mModelAnim1.speed = 0x1000;
  unk_2d0 = param1 & 0xff;
  if (unk_2d0 == 0xff)
    unk_2d0 = 0;
  switch (unk_2d0) {
  case 0:
    func_ov085_0212e728((JugemHost *)this, &data_ov085_021307d0);
    break;
  case 1:
    {
      int v = data_0209caa0[2];
      if (v & 0x20000)
        return 0;
      if (v & 0x10000)
        data_0209caa0[2] = v & ~0x10000;
      unk_2dc = 1;
      if (!(data_0209caa0[2] & 0x80))
        func_ov085_0212e728((JugemHost *)this, &data_ov085_02130790);
      else
        func_ov085_0212e728((JugemHost *)this, &data_ov085_021307e0);
    }
    break;
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- daC_Jugem_c_classInit, 0x0212ed54, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol daC_Jugem_c_classInit
/* The registry factory behind the C_JUGEM profile. It allocates 0x2e8 -- this
 * class's own sizeof -- and installs this class's vtable, which is the second
 * of the two independent witnesses the header's layout is read from.
 *
 * Reconstructed source-style name: SM64DS proves daC_Jugem_c through RTTI,
 * allocation size, vtable identity and the C_JUGEM registry profile; later EAD
 * lineage supplies classInit. The exact original spelling is not preserved.
 * Historical alias: LakituBro_Spawn.
 *
 * `return new daC_Jugem_c()` is the whole body: the synthesized constructor is
 * what stores the vptr and runs the two ModelAnim, one TextureSequence and two
 * ShadowModel constructors the ROM calls, in that order, and `operator new`
 * is fBase_c's. Spelling the vptr store by hand instead would be wrong here
 * as well as longer -- this TU emits _ZTV11daC_Jugem_c itself, so the store
 * has to be the compiler's own. */
extern "C" daC_Jugem_c *daC_Jugem_c_classInit(void)
{
    return new daC_Jugem_c();
}

