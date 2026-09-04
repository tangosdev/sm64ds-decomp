//cpp
/* PROMOTED translation unit -- ov018/daPgMthr_c (23 function(s)).
 *
 * This one file is the production source for the whole class: the ROM build
 * takes every one of these functions from a single object, the way the
 * cartridge's own build did. It licenses the contiguous .text run
 * 0x02111848..0x021126d4 in ov018 (config/tu_manifest.d/ov018/daPgMthr_c.json,
 * config/arm9/overlays/ov018/delinks.txt), where twenty-two separate
 * one-function entries used to stand.
 *
 * THE TWENTY-THIRD IS THE FACTORY, FOLDED IN LATER. MotherPenguin_Spawn at
 * 0x0211267c is the PENGUIN_MOTHER registry profile's spawn function. It sits
 * immediately after InitResources in the ROM's own .text order, inside this
 * TU's recovered boundary, and was outside only because this promotion
 * predated the profile-reconstruction campaign. It keeps C linkage and brings
 * no new include with it.
 *
 * WRITTEN IN REVERSE ROM ORDER. mwccarm 2004/b56 emits one .text section per
 * function in the REVERSE of source order, so the highest-address ROM function
 * is written FIRST here and the lowest LAST. Do not reorder: the whole-range
 * link checks emission order and refuses anything else.
 *
 * The class name is the cartridge's own: 0x02113988 holds the bytes
 * the NUL-terminated string 10daPgMthr_c and 0x0211397c is its type_info, whose base word is
 * 0x0208e390 = _ZTI8dActor_c. Nothing here is coined.
 *
 * The destructor is DEFINED IN THE CLASS BODY in include/daPgMthr_c.h, not out
 * of line here. The cartridge keeps D1 (0x02111848) BELOW D0 (0x02111898), and
 * an out-of-line member definition makes mwccarm emit the group D0-then-D1 plus
 * a homeless D2, which the whole-range link refuses with `licensed .text
 * functions are not emitted in ROM address order`. In the class body the group
 * comes out ROM-ascending and no D2 is emitted, while the destructor stays the
 * first virtual declared and so remains the key function -- which is why
 * _ZTI/_ZTS/_ZTV are emitted into this object at all. There is no explicit
 * _ZTV reference in this source: the vptr stores are the ones the compiler
 * generates for that in-class destructor, so neither vtable spelling question
 * (bare symbol vs &_ZTV[2]) arises here.
 *
 * common.h IS INCLUDED FIRST, DELIBERATELY. Matrix4x3 has two 0x30-byte
 * spellings under one guard and whichever a TU sees first stands.
 * func_ov018_02111d28 copies a whole matrix, and the ROM copies it as twelve
 * uniform words (ldm/stm x3) -- the flat `s32 m[12]` spelling in common.h. The
 * nested `{Matrix3x3 r; Vector3 t;}` spelling in include/math/Matrix.h
 * scalarizes the last three words into ldr/str pairs and costs an extra live
 * register, which is exactly how the byte diff failed before the reorder.
 *
 * Assembled from twenty-two legacy one-function sources that this promotion
 * deletes; their ROM addresses are the `// @symbol` markers below, and the
 * manifest entry records the pre-promotion round.
 */

/* Includes: the union of the twenty-two legacy files', reconciled. */
/* common.h FIRST, deliberately: Matrix4x3 has two 0x30-byte spellings under
 * one guard, and whichever a TU sees first stands. func_ov018_02111d28 copies
 * a whole Matrix4x3, and the ROM copies it as twelve uniform words
 * (ldm/stm x3) -- the flat `s32 m[12]` spelling. The nested `{Matrix3x3 r;
 * Vector3 t;}` spelling in include/math/Matrix.h scalarizes the last three
 * words into ldr/str pairs and costs an extra live register. */
#include "common.h"
#include "daPgMthr_c.h"
#include "types.h"
#include "decl_common.h"
#include "daObjSm_Lift_c.h"
#include "SharedFilePtr.h"
#include "TextureSequence.h"
#include "dBgCh_Gnd.h"

#define U8P(base, off) ((u8 *)((unsigned int)(base) + (off)))

extern "C" {
extern struct dActor_c* _ZN8dActor_c10FindWithIDEj(u32 id);
extern Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern s16 Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern int AngleDiff(int a, int b);
extern s16 Vec3_VertAngle(const Vector3* v1, const Vector3* v0);
extern int _Z14ApproachLinearRsss(s16* dst, s16 target, s16 step);
extern char* _ZN8dActor_c13ClosestPlayerEv(char* thisptr);
extern void func_0201267c(int a, void *b);
extern int Vec3_Dist(const struct Vector3* a, const struct Vector3* b);
extern "C" void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *, ShadowModel &sm, Matrix4x3 &mf, int c, int d, unsigned int e);
extern "C" void Matrix4x3_FromRotationY(void *m, int angle);
extern "C" void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *mf, short angY);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *mf, short angX);
extern "C" Matrix4x3 data_020a0e68;
extern "C" void func_ov018_02111a48(char *c, char *p);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, BCA_File *f, int a, int b, unsigned int c);
extern "C" void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *, BTP_File &f, int a, int b, unsigned int c);
extern void *data_ov018_02113c08[];
extern void *data_ov018_02113bf8[];
extern int _ZN6Player9StartTalkER7fBase_cb(void *target, void *self, int b);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *self, void *ab, unsigned msg, void *v, unsigned a, unsigned b);
extern int _ZN6Player12GetTalkStateEv(void *self);
extern void _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b, void *pos, void *c, int d, int e);
extern void _ZN6Player9DropActorEv(void *self);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void func_ov018_02111b3c(char* c);
extern dActor_c* func_ov018_021118fc(char* c);
extern void func_ov018_02111968(char* c, void* found, char* held);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char* thisptr, char* clsn);
extern int data_ov018_02113be8[];
extern int data_ov018_02113bf0[];
extern char data_ov018_02113c4c[];
struct C; typedef void (C::*PMF)();
struct C { char pad[0x370]; PMF *pp; };
extern void func_ov018_02112398(C *c);
extern void func_ov018_0211235c(C *c);
extern SharedFilePtr data_ov018_02113c00;
extern SharedFilePtr *data_ov018_02112c04[2];
extern void func_ov018_02111d28(dActor_c *self);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *f);
extern void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *f);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *act, int a, int b, unsigned int c2, unsigned int d);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *act, int a, int b, void *c2, void *d);
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov018_02111e28, NOT applied: extern "C" short Vec3_HorzAngle(const void *a, const void *b); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for func_ov018_02111e28, NOT applied: extern "C" void _Z14ApproachLinearRsss(short *dst, short target, short rate); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_Dist, from the legacy file for func_ov018_02111e28, NOT applied: extern "C" int Vec3_Dist(const void *a, const void *b); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov018_021123d0, from the legacy file for func_ov018_02111e28, NOT applied: extern "C" void func_ov018_021123d0(char *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov018_021123d0, from the legacy file for func_ov018_02111f1c, NOT applied: extern "C" void func_ov018_021123d0(char *c, int x); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov018_02111fac, NOT applied: extern short Vec3_HorzAngle(void *a, void *b); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for func_ov018_02111fac, NOT applied: extern int _Z14ApproachLinearRsss(void *dst, short a, short b); */
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_ApplyInPlaceToRotationY, from the legacy file for func_ov018_02111fac, NOT applied: extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short a); */
/* TUBUILD CONFLICT -- alternate declaration of func_0201267c, from the legacy file for func_ov018_02111fac, NOT applied: extern void func_0201267c(int id, void *p); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov018_021123d0, from the legacy file for func_ov018_02111fac, NOT applied: extern int func_ov018_021123d0(void *c, int b); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov018_02111a48, from the legacy file for func_ov018_02111fac, NOT applied: extern void func_ov018_02111a48(void *c, void *p); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c13ClosestPlayerEv, from the legacy file for func_ov018_02112234, NOT applied: extern char* _ZN8dActor_c13ClosestPlayerEv(char* thisptr); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov018_02111a48, from the legacy file for func_ov018_02112234, NOT applied: extern void func_ov018_02111a48(char* c, char* p); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov018_02111bf0, from the legacy file for func_ov018_02112234, NOT applied: extern void func_ov018_02111bf0(char* c, char* p); */
/* TUBUILD CONFLICT -- alternate declaration of func_0201267c, from the legacy file for func_ov018_02112234, NOT applied: extern void func_0201267c(int a, char* p); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj, from the legacy file for func_ov018_021122ec, NOT applied: extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,int,int,int,unsigned int); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj, from the legacy file for func_ov018_021122ec, NOT applied: extern int _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*,int,int,int,unsigned int); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov018_02111d28, from the legacy file for _ZN10daPgMthr_c13InitResourcesEv, NOT applied: extern void func_ov018_02111d28(char *c, int r1); */

/* The folded factory's own dependencies, restated here rather than pulled in
   through decl_Actor.h / decl_ActorBase.h / decl_ModelAnim.h / decl_dCcAc_c.h /
   decl_ShadowModel.h / decl_TextureSequence.h / decl_dBgCh_Actr.h as the legacy
   file did. Adding seven headers to a twenty-two-function TU is exactly the
   change that could move a byte in a member that already matches, and none of
   them is needed: these seven are the only names the factory adds, and the
   eighth, _ZTV10daPgMthr_c, is already declared by the decl_common.h this TU
   includes. So the fold introduces no new include at all. */
extern void *_ZN7fBase_cnwEj(unsigned size);
extern void _ZN8dActor_cC2Ev(void *self);
extern void _ZN9ModelAnimC1Ev(void *self);
extern void _ZN15TextureSequenceC1Ev(void *self);
extern void _ZN11ShadowModelC1Ev(void *self);
extern void _ZN7dCcAc_cC1Ev(void *self);
extern void _ZN10dBgCh_ActrC1Ev(void *self);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- MotherPenguin_Spawn, 0x0211267c, size 0x58               */
/* -------------------------------------------------------------------------- */
// @symbol MotherPenguin_Spawn
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV10daPgMthr_c */
/* The PENGUIN_MOTHER profile's factory, and the highest-address member of this
   TU, so it is written first. 908 = 0x38c is the whole object; the inlined
   dActor_c constructor runs first, this class stores its own vptr over the
   base's, and the five members are then constructed in declaration order --
   ModelAnim 0xd4, TextureSequence 0x138, ShadowModel 0x14c, dCcAc_c 0x174,
   dBgCh_Actr 0x1a8 -- which is what closes the class on 0x38c.

   The vptr store here is the plain bare-symbol one the legacy file carried,
   not the &_ZTV[2] form the compiler generates for the in-class destructor;
   the two coexist because _ZTV10daPgMthr_c IS the address point. */
extern "C" int *MotherPenguin_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(908);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV10daPgMthr_c;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN15TextureSequenceC1Ev((char *)p + 0x138);
        _ZN11ShadowModelC1Ev((char *)p + 0x14c);
        _ZN7dCcAc_cC1Ev((char *)p + 0x174);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x1a8);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN10daPgMthr_c13InitResourcesEv, 0x021124d0, size 0x1ac */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daPgMthr_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPgMthr_c::InitResources()
{
    void *m = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov018_02113c00);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModelAnim, m, 1, 1);
    for (int i = 0; i < 2; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr((void*)data_ov018_02112c0c[i]);
    for (int i = 0; i < 2; i++) {
        void *t = (void*)data_ov018_02112c04[i];
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(t);
        TextureSequence::Prepare(*(BMD_File *)((int *)&data_ov018_02113c00)[1], *(BTP_File *)((int*)t)[1]);
    }
    if (_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel) == 0) return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x104000, 0x12c000, 0x4800004, 0x900000);
    func_ov018_021123d0((char *)this, 0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    unk_374 = 0;
    func_ov018_02111d28(this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN10daPgMthr_c8BehaviorEv, 0x02112480, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daPgMthr_c8BehaviorEv
/* recovered: named members + shared header, real C++ method */
int daPgMthr_c::Behavior()
{
    func_ov018_0211235c((C *)this);
    mModelAnim.Animation::Advance();
    mTextureSequence.Advance();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    mModelAnim.UpdateVerts();
    func_ov018_02111d28(this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN10daPgMthr_c6RenderEv, 0x02112454, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daPgMthr_c6RenderEv
/* daPgMthr_c::Render -- vtable slot 9, ov018 0x02112454. Real C++ method over
   the shared header: advance the texture animation, then draw the model.

   THE Model::Render CALL IS QUALIFIED, AND THAT IS LOAD-BEARING. Model::Render is
   virtual (slot 5 of _ZTV5Model) and ModelAnim overrides it, so a plain
   mModelAnim.Render(0) would emit the vtable dispatch -- three words where the ROM
   has one bl. Naming the base explicitly (mModelAnim.Model::Render(0)) suppresses
   the dispatch and reproduces the ROM's direct call exactly. Same idiom as
   PrincessPeach::Render. TextureSequence::Update is a plain (non-virtual) method,
   so it is a direct call already; mModelAnim.data is the ModelComponents at +0xdc. */
int daPgMthr_c::Render() {
    mTextureSequence.Update(mModelAnim.data);
    mModelAnim.Model::Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN10daPgMthr_c16OnPendingDestroyEv, 0x02112450, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daPgMthr_c16OnPendingDestroyEv
/* daPgMthr_c::OnPendingDestroy -- vtable slot 12. The ROM body is empty: the
 * override exists only to occupy the slot. */
void daPgMthr_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN10daPgMthr_c16CleanupResourcesEv, 0x021123ec, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daPgMthr_c16CleanupResourcesEv
int daPgMthr_c::CleanupResources()
{
    data_ov018_02113c00.Release();
    for (int i = 0; i < 2; i++)
        ((SharedFilePtr *)data_ov018_02112c0c[i])->Release();
    for (int i = 0; i < 2; i++)
        data_ov018_02112c04[i]->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov018_021123d0, 0x021123d0, size 0x1c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov018_021123d0(char *self, int i) {
    *(char **)(self + 0x370) = data_ov018_02113c4c + (i << 4);
    func_ov018_02112398((C *)self);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov018_02112398, 0x02112398, size 0x38 */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov018_02112398(C *c) { PMF *p = c->pp; (c->**p)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov018_0211235c, 0x0211235c, size 0x3c */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov018_0211235c(C *c) { PMF *p = c->pp + 1; (c->**p)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov018_021122ec, 0x021122ec, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov018_021122ec
/* One of the four state-entry helpers this class dispatches through the table
 * at +0x370 (see func_ov018_021123d0). The legacy source called it
 * daObjSm_Lift_c::AfterClsn and cast the pointer to that class; that was wrong.
 * ov018 0x021122ec lies inside daPgMthr_c's own span, and every offset it
 * touches is a daPgMthr_c offset: +0xd4 is mModelAnim, +0x138 is
 * mTextureSequence, +0x98 is dActor_c::mHorzSpeed, +0x374 is unk_374. The
 * offsets are written raw, the way the neighbouring .c-derived helpers write
 * them, because the ROM reaches +0x130 (inside mModelAnim) and +0x37c (inside
 * the trailing remainder), which no header names. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov018_021122ec(char* c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c+0xd4, (BCA_File *)data_ov018_02113bf0[1], 0, 0x1000, 0);
  *(int*)(c+0x130)=0x1000;
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj((char*)c+0x138, *(BTP_File *)data_ov018_02113be8[1], 0, 0x1000, 0);
  *(int*)(c+0x98)=0;
  *(int*)(c+0x374)=0;
  *(int*)(c+0x37c)=0;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov018_02112234, 0x02112234, size 0xb8 */
/* -------------------------------------------------------------------------- */
/* func_ov018_02112234 at 0x02112234
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov018).
 */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov018_02112234(char* c) {
    void* a;
    int v;
    char* p;
    if (*(unsigned char*)(c + 0x386) != 0)
        func_ov018_02111b3c(c);
    a = func_ov018_021118fc(c);
    v = 0;
    if (a != 0) {
        if (*(int*)((char*)a + 8) != 3)
            v = *(int*)((char*)a + 0x358);
        func_ov018_02111968(c, a, (char*)v);
    }
    p = _ZN8dActor_c13ClosestPlayerEv(c);
    func_ov018_02111a48(c, p);
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x174);
    func_ov018_02111bf0(c, c + 0x1a8);
    if (*(unsigned char*)(c + 0x386) == 0 && *(unsigned char*)(c + 0x387) == 0) {
        unsigned int x = ((unsigned int)*(int*)(c + 0x12c) << 4) >> 16;
        if (x == 0x10 || x == 0x25)
            func_0201267c(0xdf, c + 0x74);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov018_021121dc, 0x021121dc, size 0x58 */
/* -------------------------------------------------------------------------- */
extern "C" {
int func_ov018_021121dc(char* c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (BCA_File *)data_ov018_02113bf0[1], 0, 0x1000, 0);
  *(int*)(c+0x130)=0x1000;
  *(unsigned char*)(c+0x388)=0;
  *(unsigned char*)(c+0x389)=0x3c;
  *(int*)(c+0x37c)=1;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov018_02111fac, 0x02111fac, size 0x230 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov018_02111fac(char *c)
{
    switch (*(unsigned char *)(c + 0x388)) {
    case 0:
        if (*(int *)(c + 0x194) & 0x8000000) {
            if (_ZN6Player9StartTalkER7fBase_cb(*(void **)(c + 0x374), c, 1)) {
                u8 *p = U8P(c, 0x388);
                *p = *p + 1;
            }
        } else {
            func_ov018_021123d0(c, 0);
        }
        break;
    case 1:
        if (_Z14ApproachLinearRsss((s16 *)(c + 0x8e),
                Vec3_HorzAngle((Vector3 *)(c + 0x5c), (Vector3 *)(*(char **)(c + 0x374) + 0x5c)), 0x514)) {
            Matrix4x3_FromTranslation(&data_020a0e68, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
            Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16 *)(c + 0x8e));
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0x300000, -0x480000);
            {
                int v[3];
                v[0] = *(int *)((char *)&data_020a0e68 + 0x24);
                v[1] = *(int *)((char *)&data_020a0e68 + 0x28);
                v[2] = *(int *)((char *)&data_020a0e68 + 0x2c);
                if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
                        *(void **)(c + 0x374), c, *(s16 *)(c + 0x300 + 0x84), v, 0, 0)) {
                    func_0201267c(0xdf, c + 0x74);
                    {
                        u8 *p = U8P(c, 0x388);
                        *p = *p + 1;
                    }
                }
            }
        }
        break;
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x374)) == -1) {
            if (*(s16 *)(c + 0x300 + 0x84) == 0xad) {
                unsigned r1;
                unsigned char b;
                *(unsigned char *)(c + 0x386) = 1;
                b = (unsigned char)(*(unsigned *)(c + 8) & 0xf);
                r1 = (unsigned)b | 0x40;
                _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0xb2, r1, c + 0x5c, 0, *(signed char *)(c + 0xcc), -1);
            }
            _ZN6Player9DropActorEv(*(void **)(c + 0x374));
            {
                u8 *p = U8P(c, 0x388);
                *p = *p + 1;
            }
        }
        break;
    case 3:
        if (!DecIfAbove0_Byte(U8P(c, 0x389)))
            func_ov018_021123d0(c, 0);
        break;
    }
    func_ov018_02111a48(c, *(char **)(c + 0x374));
    if (*(unsigned char *)(c + 0x386) == 0 && *(unsigned char *)(c + 0x387) == 0) {
        unsigned t = (unsigned)*(int *)(c + 0x12c) << 4 >> 0x10;
        if (t == 0x10 || t == 0x25)
            func_0201267c(0xdf, c + 0x74);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov018_02111f1c, 0x02111f1c, size 0x90 */
/* -------------------------------------------------------------------------- */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int func_ov018_02111f1c(char *c)
{
    if (*(int *)(c + 0x374) == 0 && *(unsigned char *)(c + 0x386) == 0)
        func_ov018_021123d0(c, 0);
    *(int *)(c + 0x98) = 0x5000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim *)(c + 0xd4), (BCA_File *)data_ov018_02113c08[1], 0, 0x1000, 0);
    *(int *)(c + 0x130) = 0x1000;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj((TextureSequence *)(c + 0x138), *(BTP_File *)data_ov018_02113bf8[1], 0, 0x1000, 0);
    *(int *)(c + 0x37c) = 2;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov018_02111e28, 0x02111e28, size 0xf4 */
/* -------------------------------------------------------------------------- */
extern "C" int func_ov018_02111e28(dActor_c *self)
{
    char *s = (char*)self;
    _Z14ApproachLinearRsss((s16 *)(s + 0x8e),
        Vec3_HorzAngle((Vector3 *)(s + 0x5c), (Vector3 *)(*(char**)(s + 0x374) + 0x5c)), 0x514);
    *(short*)(s + 0x94) = *(short*)(s + 0x8e);
    self->UpdatePos((dCc_c*)(s + 0x174));
    func_ov018_02111bf0(s, s + 0x1a8);
    func_ov018_02111a48(s, *(char**)(s + 0x374));
    unsigned int v = (unsigned int)(*(int*)(s + 0x12c) << 4) >> 0x10;
    if (v == 9 || v == 0x15)
        func_0201267c(0xde, s + 0x74);
    char *p2 = *(char**)(s + 0x374);
    int r;
    if (*(int*)(p2 + 8) == 3)
        r = (*(int*)(p2 + 0x360) != 0);
    else
        r = (*(int*)(p2 + 0x358) != 0);
    if (r == 0 || Vec3_Dist((Vector3 *)(s + 0x364), (Vector3 *)(p2 + 0x5c)) > 0x5dc000) {
        *(int*)(s + 0x374) = 0;
        func_ov018_021123d0(s, 0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov018_02111d28, 0x02111d28, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov018_02111d28
/* recovered: shared common types */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void func_ov018_02111d28(dActor_c *self)
{
    char *s = (char*)self;
    Matrix4x3_FromRotationY(s + 0xf0, *(short*)(s + 0x8e));
    *(int*)(s + 0x114) = *(int*)(s + 0x5c) >> 3;
    *(int*)(s + 0x118) = *(int*)(s + 0x60) >> 3;
    *(int*)(s + 0x11c) = *(int*)(s + 0x64) >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(self, *(ShadowModel*)(s + 0x14c), *(Matrix4x3*)(s + 0xf0), 0x140000, 0x50000, 0xf);
    if (*(short*)(s + 0x380) != 0 || *(short*)(s + 0x382) != 0) {
        Matrix4x3 *dst = (Matrix4x3*)((*(char**)(s + 0xe8)) + 0xf0);
        data_020a0e68 = *dst;
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(s + 0x382));
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short*)(s + 0x380));
        *(Matrix4x3*)((*(char**)(s + 0xe8)) + 0xf0) = data_020a0e68;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov018_02111bf0, 0x02111bf0, size 0x138 */
/* -------------------------------------------------------------------------- */
extern "C" {
/* NOT SurfaceInfo. This is the 0x24 remainder of dBgPi after its vptr
   (0x28 = 4 + 0x24), and it was the only definition in the tree claiming a
   size other than 0x14 for a type called SurfaceInfo. Renamed 2026-08-19 so
   the name means one thing; the real one is include/SurfaceInfo.h. */
struct dBgPiRawBody {
  int a, b, c, d, e;
  unsigned short f, g;
  int h, i, j;
};
struct dBgPiRaw {
  void* vt;
  struct dBgPiRawBody info;
};
void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void* w);
int _ZNK10dBgCh_Actr10IsOnGroundEv(dBgCh_Actr* w);
void* _ZNK10dBgCh_Actr14GetFloorResultEv(dBgCh_Actr* w);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* s, Vector3* v);
int _ZN4cstd4fdivEii(int a, int b);
int _ZNK10dBgCh_Actr8IsOnWallEv(dBgCh_Actr* w);
struct dBgPiRaw* _ZNK10dBgCh_Actr13GetWallResultEv(dBgCh_Actr* w);
void _ZN5dBgPiD1Ev(struct dBgPiRaw* r);
extern int data_02099368[];

void func_ov018_02111bf0(void* cv, void* wv){
  char* c = (char*)cv;
  dBgCh_Actr* w = (dBgCh_Actr*)wv;
  dBgCh_Actr_UpdateDiscreteNoLava_veneer(w);
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(w) != 0) {
    Vector3 n;
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(w) + 4), &n);
    if (n.y != 0) {
      int s = (int)(((long long)n.x * *(int*)(c+0xa4) + 0x800) >> 0xc)
            + (int)(((long long)n.z * *(int*)(c+0xac) + 0x800) >> 0xc);
      *(int*)(c+0xa8) = -(_ZN4cstd4fdivEii(s, n.y) + 0x8000);
    }
  }
  if (_ZNK10dBgCh_Actr8IsOnWallEv(w) != 0) {
    struct dBgPiRaw* src = _ZNK10dBgCh_Actr13GetWallResultEv(w);
    struct dBgPiRaw cr;
    Vector3 wn;
    struct dBgPiRawBody* dst = &cr.info;
    // demand a first (should get r4), then b (r1), then dst (r2)
    int a = *(int*)((char*)src + 4);
    int b = *(int*)((char*)src + 8);
    *(int*)((char*)dst + 0) = b ? a : a;
    *(int*)((char*)dst + 4) = b;
    int t = *(int*)((char*)src + 0xc);
    void* vt = (void*)data_02099368;
    *(int*)((char*)dst + 8) = t;
    t = *(int*)((char*)src + 0x10);
    *(int*)((char*)dst + 0xc) = t;
    t = *(int*)((char*)src + 0x14);
    *(int*)((char*)dst + 0x10) = t;
    cr.vt = vt;
    // remaining via cr members for sp-relative
    cr.info.f = *(unsigned short*)((char*)src + 0x18);
    cr.info.g = *(unsigned short*)((char*)src + 0x1a);
    cr.info.h = *(int*)((char*)src + 0x1c);
    cr.info.i = *(int*)((char*)src + 0x20);
    cr.info.j = *(int*)((char*)src + 0x24);
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(dst, &wn);
    _ZN5dBgPiD1Ev(&cr);
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov018_02111b3c, 0x02111b3c, size 0xb4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov018_02111b3c
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov018_02111b3c(char* c) {
  char* p = _ZN8dActor_c13ClosestPlayerEv(c);
  char* r1;
  if (p == 0) return;
  if (Vec3_Dist((struct Vector3*)(c+0x364), (struct Vector3*)(p+0x5c)) > 0x5dc000) return;
  if (*(int*)(p+8) == 3) r1 = *(char**)(p+0x360);
  else r1 = *(char**)(p+0x358);
  if (r1 == 0) return;
  {
    int b = (int)(*(u16*)(r1+0xc) == 0x100);
    if (b == 0) return;
  }
  if (*(int*)(r1+8) != 0) return;
  *(char**)(c+0x374) = p;
  func_ov018_021123d0(c, 2);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov018_02111a48, 0x02111a48, size 0xf4 */
/* -------------------------------------------------------------------------- */
/* func_ov018_02111a48 at 0x02111a48
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov018).
 */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov018_02111a48(char* a, char* b)
{
    Fix12i dist;
    s16 horzAngle;
    s16 delta, vert;
    Vector3 v0;
    Vector3 v1;
    char* q;
    Fix12i tx, ty, tz;

    if (b == 0) return;

    dist = Vec3_HorzDist((Vector3*)(a + 0x5c), (Vector3*)(b + 0x5c));
    horzAngle = Vec3_HorzAngle((Vector3*)(a + 0x5c), (Vector3*)(b + 0x5c));

    if (dist < 0x1c2000 && AngleDiff(horzAngle, *(s16*)(a + 0x8e)) < 0x1400) {
        tz = *(Fix12i*)(b + 0x64);
        ty = *(Fix12i*)(b + 0x60) + 0x640000;
        tx = *(Fix12i*)(b + 0x5c);
        v0.x = tx;
        v0.z = tz;
        v0.y = ty;
        q = *(char**)(a + 0xe8) + 0xf0;
        v1.x = *(Fix12i*)(q + 0x24);
        v1.y = *(Fix12i*)(q + 0x28);
        v1.z = *(Fix12i*)(q + 0x2c);
        vert = Vec3_VertAngle(&v1, &v0);
        delta = horzAngle - *(s16*)(a + 0x8e);
    } else {
        vert = 0;
        delta = 0;
    }

    _Z14ApproachLinearRsss((s16*)(a + 0x382), delta, 0x250);
    _Z14ApproachLinearRsss((s16*)(a + 0x380), vert, 0x250);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov018_02111968, 0x02111968, size 0xe0 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov018_02111968(char* c, void* found, char* held){
  char* a = held;
  void* v = found;
  if (a) {
    int b = *(unsigned short*)(a+0xc);
    b = b == 0x100;
    if (b != false) {
      if (*(int*)(a+8) == 1) {
        *(int*)(c+0x374) = (int)v;
        *(short*)(c+0x384) = 0xae;
        *(char*)(c+0x387) = 1;
        func_ov018_021123d0(c, 1);
        return;
      }
    }
  }
  if (*(unsigned char*)(c+0x386)) return;
  if (a) {
    int b = *(unsigned short*)(a+0xc);
    b = b == 0x100;
    if (b != false) {
      if (*(int*)(a+8) == 0) {
        *(short*)(c+0x384) = 0xad;
        goto tail;
      }
    }
  }
  if (*(unsigned char*)(c+0x387)) return;
  *(short*)(c+0x384) = 0xac;
tail:
  *(int*)(c+0x374) = (int)v;
  func_ov018_021123d0(c, 1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov018_021118fc, 0x021118fc, size 0x6c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
struct dActor_c* func_ov018_021118fc(char* c) {
    struct dActor_c* r4 = 0;
    if (*(u32*)(c + 0x194) & 0x8000000) {
        struct dActor_c* a = _ZN8dActor_c10FindWithIDEj(*(u32*)(c + 0x198));
        if (a) {
            int ok = (a->actorID == 0xbf) ? 1 : (int)r4;
            if (ok) {
                if (a != *(struct dActor_c**)(c + 0x378))
                    r4 = a;
                *(struct dActor_c**)(c + 0x378) = a;
            }
        }
    } else {
        *(struct dActor_c**)(c + 0x378) = r4;
    }
    return r4;
}
}
