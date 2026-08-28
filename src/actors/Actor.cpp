//cpp
/* Translation unit, reconstructed and promoted -- arm9 / dActor_c.  Nintendo's dActor_c.
 *
 * CANONICAL. This file is the enrolled owner of 0x0200f658..0x02011654: the
 * ROM build compiles it and takes all 97 functions from this one object, and
 * the one-function sources it replaced are gone. See notes/tu-reconstruction-
 * pilot-report.md for the worked example this follows and
 * config/tu_manifest.d/ for the boundary evidence.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST. That is what puts the
 * constructor and destructor at the top of the file and the leaf helpers at the
 * bottom -- the natural shape of a hand-written .cpp, and the strongest single
 * piece of evidence that this span is one translation unit.
 *
 * dActor_c::~dActor_c IS NOT DEFINED AS A METHOD HERE, deliberately. include/dActor_c.h
 * declares the destructor first precisely so that it is the key function, and
 * the header's own comment states the invariant: the key function must never be
 * defined as a real method in any TU, or the compiler emits _ZTV8dActor_c and it
 * collides with the copy the module's gap object supplies from ROM data. D0, D1
 * and D2 therefore keep their extern "C" free-function form, exactly as the
 * legacy sources have them.
 *
 * Declarations are reconciled IN PLACE: each member carries whatever it needs
 * that no earlier member already supplied. Nothing is hoisted but the includes.
 */

/* RECONCILIATION -- what merging 97 files into one TU forced, and nothing else.
 *
 * Every item below exists only BECAUSE these are now one translation unit: two
 * files describing the same global with different types, a local shadow struct
 * standing in for a type the merged TU really has, or a declaration whose
 * parameter list disagrees with the definition that used to live in another
 * file. Not one of them changes what a function does, and every one was held to
 * the byte gate: 97/97 members reproduce the cartridge.
 *
 *  - Shadow types dropped for the real header's: dActor_c, Vector3, Vector3_16,
 *    Matrix4x3, dCc_c, Player, Sound, Heap, and `typedef int Fix12`
 *    (types.h has a Fix12<T> TEMPLATE, so that typedef is not even legal here).
 *    Field names follow: deathTableId -> mDeathTableID, flags -> mFlags, camSpacePos
 *    -> mCamSpacePosX, pos -> mPosX, speed -> unk_0a4, areaID -> mAreaId.
 *  - One spelling per global, chosen for what the ROM's use of it supports:
 *    data_0209b468 is an ADDRESS (`extern int []`), not a value -- declared as a
 *    scalar the three destructors come out four bytes long, an extra ldr to
 *    fetch it. decl_common.h's void* wins for data_0209b450/data_0209b458 and
 *    the three readers cast at the point of use.
 *  - One prototype per function, taken from whichever file had recovered the
 *    most: func_02010e78's caller knew (const Vector3*, const Vector3_16*, s8,
 *    s16) where its own file only knew (int, int, uchar, short);
 *    Particle::System::NewSimple returns int, because the two callers that USE
 *    the result cannot both be wrong about it while six that discard it are
 *    right. dActor_c::ClosestPlayer is DEFINED here, so the three disagreeing
 *    extern "C" guesses at its prototype are simply gone.
 *  - 12/18's dBgCh_Gnd is renamed RaycastGroundPod. It and 14/16's describe
 *    the same 0x50 bytes of stack, but one is a POD constructed by explicit
 *    mangled call and the other a class whose ctor/dtor the compiler calls;
 *    one C++ type cannot be both. Nothing spells it by an unmangled symbol, so
 *    the rename cannot reach the linker.
 *  - _ZN8Vector3sD1Ev (ordinal 95) spells its mangled ROM symbol directly instead
 *    of going through the legacy file's Vector3s_ForceDestructor scaffold. The
 *    scaffold is the only way to force an out-of-line copy of an inline empty
 *    destructor -- but it is 0x50 of STB_GLOBAL .text with no address in this
 *    ROM, and per-function objisolate strips it. In a merged TU it lands INSIDE
 *    the span and shifts every later function, which is the one thing that
 *    stood between this file and a whole-range link. Same four bytes either way.
 *  - D0's two `thiz + 0x50` arguments are spelled differently (`((int)thiz)+0x50`
 *    and `&thiz->mListPrev`), exactly as D1 and D2 spell them. Written identically
 *    under decl_common.h's (int) prototypes, mwcc commons them into a fourth
 *    register and D0 comes out 0xc long.
 *
 * include/dActor_c.h gained three declarations for this file: a `struct Matrix4x3;`
 * forward declaration, `void UntrackInDeathTable();` and
 * `Matrix4x3 *UpdateCarry(Player &, const Vector3 &);`. All three are
 * NON-VIRTUAL, so the class layout, its 0xd0 size assertion and its 31 vtable
 * slots are untouched, and ~dActor_c() is still the key function. */

/* Includes: union of the admitted legacy files', first-seen in emission
 * order. Header-guarded, so the union is safe; ordering constraints are
 * not otherwise checked. */
#include "types.h"
#include "decl_common.h"
#include "dActor_c.h"
#include "Player.h"
#include "common.h"
#include "dCc_c.h"
#include "Sound.h"

/* -------------------------------------------------------------------------- */
/* ROM ordinal 96 -- _ZN8dActor_cC2Ev
 * 0x0201150c  size 0x148   legacy src/_ZN8dActor_cC2Ev.cpp */
extern "C" {
extern int data_0208e4b8;
extern int data_0208e3a4;
extern int data_0209b468[];
extern s16* data_0209b460;
extern s16* data_0209b45c;
extern s16 data_0208e378;
extern int* data_020a4bb8;
extern unsigned char data_0209f2d8;
void _ZN7fBase_cC2Ev(void* self);
int func_0203b244(void* l, void* n);
void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(struct dActor_c *self, int a, int b, int c, int d);
}
extern "C" void* _ZN8dActor_cC2Ev(struct dActor_c *self) {
    int* entry;
    int b;
    int r3;
    _ZN7fBase_cC2Ev(((char*)self));
    *(void**)((char*)self) = &data_0208e4b8;
    *(void**)((char*)self) = &data_0208e3a4;
    self->mListPrev = 0;
    self->mListNext = 0;
    *(char**)((char*)&self->mListOwner) = ((char*)self);
    func_0203b244((void*)&data_0209b468, ((char*)self) + 0x50);
    {
        s16* p = data_0209b460;
        if (p != 0) {
            self->mPosX = ((int*)p)[0];
            self->mPosY = ((int*)p)[1];
            self->mPosZ = ((int*)p)[2];
        }
    }
    {
        s16* q = data_0209b45c;
        if (q != 0) {
            self->mAngleX = q[0];
            self->mAngleY = q[1];
            self->mAngleZ = q[2];
            {
                s16* q2 = data_0209b45c;
                self->mPrevAngleX = q2[0];
                self->mPrevAngleY = q2[1];
                self->mPrevAngleZ = q2[2];
            }
        }
    }
    self->mAreaId = data_0209b44c;
    self->mDeathTableID = data_0208e378;
    entry = (int*)(((int**)data_020a4bb8)[self->actorID]);
    self->mFlags = entry[2];
    b = (data_0209f2d8 == 2);
    if (b != 0)
        r3 = entry[5] + 0x7d0000;
    else
        r3 = entry[5];
    _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_((self), entry[3], entry[4], r3, entry[6]);
    return ((char*)self);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 95 -- _ZN8Vector3sD1Ev
 * 0x02011508  size 0x4   legacy src/_ZN8Vector3sD1Ev.cpp */
/* No forcing scaffold. The legacy file reaches this four-byte `bx lr` through a
 * `struct Vector3s_ForceDestructor { Vector3s v[2]; ~Vector3s_ForceDestructor(); }`,
 * because an explicit `p->~Vector3s()` on an empty inline destructor is inlined away
 * while an ARRAY cleanup has to pass the destructor's address along -- but that
 * scaffold is 0x50 of STB_GLOBAL .text with no address in this ROM, and objisolate
 * strips it per function. In a merged TU it lands INSIDE the span and shifts
 * everything after it, which is the one thing between this file and a whole-range
 * link. Spelling the ROM symbol directly emits the same four bytes and nothing else. */
extern "C" void _ZN8Vector3sD1Ev(void *self) {}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 94 -- _ZN8dActor_cC1Ev
 * 0x020113c0  size 0x148   legacy src/_ZN8dActor_cC1Ev.cpp */
extern "C" void* _ZN8dActor_cC1Ev(struct dActor_c *self) {
  _ZN7fBase_cC2Ev(((char*)self));
  *(void**)((char*)self) = &data_0208e4b8;
  *(void**)((char*)self) = &data_0208e3a4;
  self->mListPrev = 0;
  self->mListNext = 0;
  *(void**)((char*)&self->mListOwner) = ((char*)self);
  func_0203b244(&data_0209b468, ((char*)self)+0x50);
  {
    int* p = (int*)data_0209b460;
    if (p) {
      self->mPosX = p[0];
      self->mPosY = p[1];
      self->mPosZ = p[2];
    }
  }
  {
    short* q = (short*)data_0209b45c;
    if (q) {
      self->mAngleX = q[0];
      self->mAngleY = q[1];
      self->mAngleZ = q[2];
      q = (short*)data_0209b45c;
      self->mPrevAngleX = q[0];
      self->mPrevAngleY = q[1];
      self->mPrevAngleZ = q[2];
    }
  }
  self->mAreaId = data_0209b44c;
  self->mDeathTableID = data_0208e378;
  {
    void** base = *(void***)&data_020a4bb8;
    int idx = self->actorID;
    char* s = (char*)base[idx];
    self->mFlags = *(int*)(s+8);
    {
      int b = (data_0209f2d8 == 2);
      int r3;
      int d = *(int*)(s+0x18);
      if (b) r3 = *(int*)(s+0x14) + 0x7d0000;
      else r3 = *(int*)(s+0x14);
      _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_((self), *(int*)(s+0xc), *(int*)(s+0x10), r3, d);
    }
  }
  return ((char*)self);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 93 -- _ZN8dActor_cD1Ev
 * 0x02011374  size 0x4c   legacy src/_ZN8dActor_cD1Ev.cpp */
extern "C" {
extern int _ZTV8dActor_c[];
extern int _ZTV7dBase_c[];
extern void _ZN7fBase_cD2Ev(int c);
}
extern "C" {
int _ZN8dActor_cD1Ev(struct dActor_c *self) {
  *(int*)((int)self) = (int)_ZTV8dActor_c;
  func_0203b27c((int)data_0209b468, ((int)self)+0x50);
  func_02044104((int)&self->mListPrev);
  *(int*)((int)self) = (int)_ZTV7dBase_c;
  _ZN7fBase_cD2Ev(((int)self));
  return ((int)self);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 92 -- _ZN8dActor_cD0Ev
 * 0x02011314  size 0x60   legacy src/_ZN8dActor_cD0Ev.c */
extern "C" struct dActor_c *_ZN8dActor_cD0Ev(struct dActor_c *thiz)
{
    *(void **)thiz = (void *)_ZTV8dActor_c;
    func_0203b27c((int)data_0209b468, ((int)thiz) + 0x50);
    func_02044104((int)&thiz->mListPrev);
    *(void **)thiz = (void *)_ZTV7dBase_c;
    _ZN7fBase_cD2Ev((int)thiz);
    _ZN6Memory10DeallocateEPvP4Heap(thiz, data_020a0eac);
    return thiz;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 91 -- _ZN8dActor_cD2Ev
 * 0x020112c8  size 0x4c   legacy src/_ZN8dActor_cD2Ev.cpp */
extern "C" {
int _ZN8dActor_cD2Ev(struct dActor_c *self) {
  *(int*)((int)self) = (int)_ZTV8dActor_c;
  func_0203b27c((int)data_0209b468, ((int)self)+0x50);
  func_02044104((int)&self->mListPrev);
  *(int*)((int)self) = (int)_ZTV7dBase_c;
  _ZN7fBase_cD2Ev(((int)self));
  return ((int)self);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 90 -- _ZN8dActor_c19BeforeInitResourcesEv
 * 0x02011268  size 0x60   legacy src/_ZN8dActor_c19BeforeInitResourcesEv.cpp */
bool dActor_c::BeforeInitResources()
{
    int spawnEvenIfKilledBefore;

    spawnEvenIfKilledBefore = (mFlags & 0x8000000) ? 1 : 0;
    if (spawnEvenIfKilledBefore != 0) {
        goto skip;
    }
    if (GetBitInDeathTable() == 0) {
        goto skip;
    }
    MarkForDestruction();
    return true;
skip:;
    {
        int r = fBase_c::BeforeInitResources();
        if (r == 0) return false;
        return true;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 89 -- _ZN8dActor_c18AfterInitResourcesEj
 * 0x02011244  size 0x24   legacy src/_ZN8dActor_c18AfterInitResourcesEj.cpp */
void dActor_c::AfterInitResources(u32 vfSuccess)
{
    dBase_c::AfterInitResources(vfSuccess);
    /* The (long long)(int) cast is a codegen launder, not a type conversion: the
       ROM materialises the address into a register first (`add r1,r4,#0xb0`)
       and then does the read-modify-write through it. Writing `mFlags |= 0x38`
       folds the offset into the ldr/str and comes out 4 bytes short. */
    *(u32 *)&mFlags |= 0x38;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 88 -- _ZN8dActor_c22BeforeCleanupResourcesEv
 * 0x02011220  size 0x24   legacy src/_ZN8dActor_c22BeforeCleanupResourcesEv.cpp */
int dActor_c::BeforeCleanupResources()
{
    if (fBase_c::BeforeCleanupResources() != 0)
        return 1;
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 87 -- _ZN8dActor_c21AfterCleanupResourcesEj
 * 0x02011214  size 0xc   legacy src/_ZN8dActor_c21AfterCleanupResourcesEj.cpp */
void dActor_c::AfterCleanupResources(u32 vfSuccess)
{
    fBase_c::AfterCleanupResources(vfSuccess);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 86 -- _ZN8dActor_c14BeforeBehaviorEv
 * 0x02010fd4  size 0x240   legacy src/_ZN8dActor_c14BeforeBehaviorEv.cpp */
extern "C" {
u8 IsAreaShowing(int areaId);
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
int _ZN7Clipper13Func_020150E8ER7Vector35Fix12IiEPh(char *thisp, Vector3 *v, int radius, u8 *out);
extern int data_0209fc68; /* nonzero forces the actor to think anyway */
extern Matrix4x3 data_0209b3ec; /* world -> camera */
extern char data_0209f43c; /* the Clipper instance */
extern u8 data_0209f274; /* doubles the far-away threshold when set */
extern u8 data_0209f2c4;
extern u8 data_0209f20c;
extern u8 data_0209f294;
extern u32 data_0209b464; /* flag mask that forces thinking; 0 means "any" */
}
int dActor_c::BeforeBehavior()
{
    if (!fBase_c::BeforeBehavior())
        return 0;

    data_0209b458 = 0;

    signed char c = mAreaId;
    if (c >= 0 && !IsAreaShowing(c)) {
        mFlags |= 0x38;
        mCamSpacePosX = 0x7fffffff;
        mCamSpacePosY = 0x7fffffff;
        mCamSpacePosZ = 0x7fffffff;
        if (data_0209fc68 == 0 || (mFlags & 0x10000))
            return 0;
    } else {
        if (mClipRadius != 0) {
            Vector3 tmp;
            tmp.x = mPosX >> 3;
            tmp.y = (mPosY + mClipOffsetY) >> 3;
            tmp.z = mPosZ >> 3;
            MulVec3Mat4x3(&tmp, &data_0209b3ec, (Vector3 *)&mCamSpacePosX);
        }
        if (mFlags & 0x10003) {
            int r = _ZN7Clipper13Func_020150E8ER7Vector35Fix12IiEPh(
                        &data_0209f43c,
                        (Vector3 *)&mCamSpacePosX,
                        mClipRadius,
                        &mClipResult);
            int thresh = mClipDistance;
            if (data_0209f274)
                thresh <<= 1;
            if (r > thresh) {
                mFlags |= 0x18;
                if (mFlags & 0x10000)
                    return 0;
            } else {
                u32 *p = &mFlags;
                *p &= ~0x38;
                if (r > mFarDistance)
                    *p |= 0x10;
            }
        } else {
            mFlags &= ~0x38;
        }
    }

    if (((data_0209f2c4 | data_0209f20c | data_0209f294) & 0xff) == 0) {
        u32 f = mFlags;
        if ((f & 9) != 9 && (data_0209b464 == 0 || (f & data_0209b464) != 0))
            goto do_copy;
        if (data_0209fc68 != 0)
            goto do_copy;
    }
    return 0;

do_copy:
    mPrevPosX = mPosX;
    mPrevPosY = mPosY;
    mPrevPosZ = mPosZ;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 85 -- _ZN8dActor_c13AfterBehaviorEj
 * 0x02010fc8  size 0xc   legacy src/_ZN8dActor_c13AfterBehaviorEj.cpp */
void dActor_c::AfterBehavior(u32 vfSuccess)
{
    fBase_c::AfterBehavior(vfSuccess);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 84 -- _ZN8dActor_c12BeforeRenderEv
 * 0x02010f78  size 0x50   legacy src/_ZN8dActor_c12BeforeRenderEv.cpp */
int dActor_c::BeforeRender()
{
    u32 flags;

    if (!fBase_c::BeforeRender())
        return 0;

    flags = mFlags;
    if (flags & 0x20)
        goto ret0;
    if (!(flags & 8))
        goto ret1;
    if (!(flags & 2))
        goto ret1;
ret0:
    return 0;
ret1:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 83 -- _ZN8dActor_c11AfterRenderEj
 * 0x02010f6c  size 0xc   legacy src/_ZN8dActor_c11AfterRenderEj.cpp */
void dActor_c::AfterRender(u32 vfSuccess)
{
    fBase_c::AfterRender(vfSuccess);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 82 -- _ZN8dActor_c10FindWithIDEj
 * 0x02010f3c  size 0x30   legacy src/_ZN8dActor_c10FindWithIDEj.cpp */
extern "C" {
extern dActor_c *func_02043f98(dActor_c **head, unsigned int id);
}
dActor_c *dActor_c::FindWithID(u32 id)
{
    dActor_c *node = func_02043f98((dActor_c **)data_0209b468, id);
    if (node)
        return *(dActor_c **)((char *)node + 8);
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 81 -- _ZN8dActor_c15FindWithActorIDEjPS_
 * 0x02010ef0  size 0x4c   legacy src/_ZN8dActor_c15FindWithActorIDEjPS_.cpp */
dActor_c *dActor_c::FindWithActorID(u32 j, dActor_c *after) {
  int p = (int)after;
  int *r;
  if (p) r = func_02043f4c(data_0209b468, j, p+0x50);
  else r = func_02043f4c(data_0209b468, j, 0);
  if (r) return (dActor_c *)r[2];
  return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 80 -- _ZN8dActor_c4NextEPKS_
 * 0x02010ecc  size 0x24   legacy src/_ZN8dActor_c4NextEPKS_.cpp */
extern "C" {
struct dActor_c* _ZN8dActor_c4NextEPKS_(struct dActor_c *self) {
  struct dActor_c* p;
  if(((struct dActor_c*)self)) p = *(struct dActor_c**)((char*)&self->mListNext);
  else  p = *(struct dActor_c**)&data_0209b468;
  if(p) return *(struct dActor_c**)((char*)p+8);
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 79 -- func_02010e78
 * 0x02010e78  size 0x54   legacy src/func_02010e78.c */
extern "C" void func_02010e78(const Vector3 *a, const Vector3_16 *b, s8 c, s16 d)
{
    data_0209b460 = (s16 *)a;
    data_0209b45c = (s16 *)b;
    data_0209b44c = c;
    data_0208e378 = d;
    data_0209b458 = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 78 -- _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as
 * 0x02010e2c  size 0x4c   legacy src/_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as.cpp */
extern "C" {
extern fBase_c *data_0209f5c0;
void func_02010e78(const Vector3 *pos, const Vector3_16 *rot, s8 areaID, s16 deathTableID);
}
dActor_c *dActor_c::Spawn(u32 actorID, u32 spawnParam, const Vector3 &pos,
                    const Vector3_16 *rot, s8 areaID, s16 deathTableID)
{
    func_02010e78(&pos, rot, areaID, deathTableID);
    return (dActor_c *)dBase_c::Spawn(actorID, data_0209f5c0, spawnParam, 2);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 77 -- _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_
 * 0x02010e08  size 0x24   legacy src/_ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_.cpp */
extern "C" {
void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(struct dActor_c *self, int a, int b, int c, int d) {
self->mClipOffsetY=a;
self->mClipRadius=b>>3;
self->mClipDistance=c>>3;
self->mFarDistance=d>>3;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 76 -- func_02010da4
 * 0x02010da4  size 0x64   legacy src/func_02010da4.c */
extern "C" void func_02010da4(int* p)
{
    int v;
    v = p[0x5c / 4];
    if (v < -0x7d00000) p[0x5c / 4] = -0x7d00000;
    else if (v > 0x7d00000) p[0x5c / 4] = 0x7d00000;
    v = p[0x60 / 4];
    if (v < -0x7d00000) p[0x60 / 4] = -0x7d00000;
    else if (v > 0x7d00000) p[0x60 / 4] = 0x7d00000;
    v = p[0x64 / 4];
    if (v < -0x7d00000) p[0x64 / 4] = -0x7d00000;
    else if (v > 0x7d00000) p[0x64 / 4] = 0x7d00000;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 75 -- _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c
 * 0x02010d40  size 0x64   legacy src/_ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c.cpp */
typedef int s32;
struct Vec3 { s32 x, y, z; };
extern "C" {
void AddVec3(Vec3 *a, Vec3 *b, Vec3 *c);
void func_02010da4(int *p);
}
void dActor_c::UpdatePosWithOnlySpeed(dCc_c *clsn) {
    AddVec3((Vec3 *)&mPosX, (Vec3 *)&unk_0a4, (Vec3 *)&mPosX);
    if (clsn != 0) {
        *(s32 *)(&mPosX) += clsn->pushback.x;
        *(s32 *)(&mPosZ) += clsn->pushback.z;
    }
    func_02010da4((int *)this);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 74 -- _ZN8dActor_c28UpdatePosWithHorzSpeedAndAngEv
 * 0x02010c5c  size 0xe4   legacy src/_ZN8dActor_c28UpdatePosWithHorzSpeedAndAngEv.cpp */
typedef short s16;
typedef long long s64;
extern "C" {
extern "C" s16 data_02082214[];
}
void dActor_c::UpdatePosWithHorzSpeedAndAng(){
    void *c = this;
    s32 v98 = *(s32*)((char*)c+0x98);
    if (v98 == 0) {
        s32 a8 = *(s32*)((char*)c+0xa8);
        s32 v9c = *(s32*)((char*)c+0x9c);
        s32 a0 = *(s32*)((char*)c+0xa0);
        s32 nx = a8 + v9c;
        if (nx >= a0) a0 = nx;
        *(s32*)((char*)c+0xa4) = 0;
        *(s32*)((char*)c+0xa8) = a0;
        *(s32*)((char*)c+0xac) = 0;
        return;
    } else {
        s32 a8;
        s32 v9c;
        s32 a0;
        s64 px;
        s32 j;
        s16 sinv;
        s32 nx;
        s16 cosv;
        s64 pz;
        j = ((s32)*(unsigned short*)((char*)c+0x94) >> 4) << 1;
        sinv = data_02082214[j + 1];
        a8 = *(s32*)((char*)c+0xa8);
        v9c = *(s32*)((char*)c+0x9c);
        a0 = *(s32*)((char*)c+0xa0);
        px = ((s64)v98 * sinv + 0x800) >> 12;
        nx = a8 + v9c;
        if (nx >= a0) a0 = nx;
        cosv = data_02082214[j];
        pz = ((s64)v98 * cosv + 0x800) >> 12;
        *(s32*)((char*)c+0xa4) = (s32)pz;
        *(s32*)((char*)c+0xa8) = a0;
        *(s32*)((char*)c+0xac) = (s32)px;
        return;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 73 -- _ZN8dActor_c9UpdatePosEP5dCc_c
 * 0x02010c30  size 0x2c   legacy src/_ZN8dActor_c9UpdatePosEP5dCc_c.cpp */
void dActor_c::UpdatePos(dCc_c *clsn)
{
    UpdatePosWithHorzSpeedAndAng();
    UpdatePosWithOnlySpeed(clsn);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 72 -- _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j
 * 0x02010be8  size 0x48   legacy src/_ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j.c */
struct ShadowModel;
extern "C" {
extern void _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j( struct ShadowModel* shadow, struct Matrix4x3* matrix, Fix12i scaleX, Fix12i scaleY, Fix12i scaleZ, u8 opacity);
}
extern "C" void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    struct dActor_c* this_,
    struct ShadowModel* shadow,
    struct Matrix4x3* matrix,
    Fix12i radius,
    Fix12i depth,
    u8 opacity)
{
    if (this_->mFlags & 0x10)
        return;
    _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j(shadow, matrix, radius, depth, radius, opacity);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 71 -- _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j
 * 0x02010b9c  size 0x4c   legacy src/_ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j.c */
extern "C" void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    struct dActor_c* this_,
    struct ShadowModel* shadow,
    struct Matrix4x3* matrix,
    Fix12i scaleX,
    Fix12i scaleY,
    Fix12i scaleZ,
    u8 opacity)
{
    if (this_->mFlags & 0x10)
        return;
    _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j(shadow, matrix, scaleX, scaleY, scaleZ, opacity);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 70 -- _ZN8dActor_c13ClosestPlayerEv
 * 0x02010ad8  size 0xc4   legacy src/_ZN8dActor_c13ClosestPlayerEv.cpp */
extern "C" {
extern int Vec3_Dist(const void* a, const void* b);
extern int data_0208e380;
extern int data_0208e37c;
extern unsigned char data_0209f21c;
extern void* data_0209f394[];
}
Player *dActor_c::ClosestPlayer()
{
    if (!data_0209b458) {
        data_0208e380 = 0x7fffffff;
        data_0208e37c = 0x80000000;
        int i;
        for (i = 0; i < data_0209f21c; i++) {
            void* p = data_0209f394[i];
            if (!p) continue;
            int d = Vec3_Dist((char*)this + 0x5c, (char*)p + 0x5c);
            if (d < data_0208e380) { data_0208e380 = d; data_0209b458 = p; }
            if (d > data_0208e37c) { data_0208e37c = d; data_0209b450 = p; }
        }
    }
    return (Player *)data_0209b458;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 69 -- _ZN8dActor_c22ClosestNonVanishPlayerEv
 * 0x02010a08  size 0xd0   legacy src/_ZN8dActor_c22ClosestNonVanishPlayerEv.cpp */
Player *dActor_c::ClosestNonVanishPlayer()
{
    if (!data_0209b458) {
        data_0208e380 = 0x7fffffff;
        data_0208e37c = 0x80000000;
        int i;
        for (i = 0; i < data_0209f21c; i++) {
            void* p = data_0209f394[i];
            if (!p) continue;
            if (*(unsigned char*)((char*)p + 0x6fb)) continue;
            int d = Vec3_Dist((char*)this + 0x5c, (char*)p + 0x5c);
            if (d < data_0208e380) { data_0208e380 = d; data_0209b458 = p; }
            if (d > data_0208e37c) { data_0208e37c = d; data_0209b450 = p; }
        }
    }
    return (Player *)data_0209b458;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 68 -- _ZN8dActor_c13DistToCPlayerEv
 * 0x020109e4  size 0x24   legacy src/_ZN8dActor_c13DistToCPlayerEv.cpp */
typedef int Fix12i; /* 20.12 fixed-point */
s32 dActor_c::DistToCPlayer()
{
    ClosestPlayer();
    return data_0208e380;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 67 -- _ZN8dActor_c18HorzAngleToCPlayerEv
 * 0x020109b8  size 0x2c   legacy src/_ZN8dActor_c18HorzAngleToCPlayerEv.cpp */
extern "C" {
s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
}
s16 dActor_c::HorzAngleToCPlayer()
{
    ClosestPlayer();
    return Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)((char *)data_0209b458 + 0x5c));
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 66 -- _ZN8dActor_c23HorzAngleToCPlayerOrAngEv
 * 0x0201097c  size 0x3c   legacy src/_ZN8dActor_c23HorzAngleToCPlayerOrAngEv.cpp */
s16 dActor_c::HorzAngleToCPlayerOrAng()
{
    dActor_c *player;

    ClosestPlayer();
    player = (dActor_c *)data_0209b458;
    if (player == 0)
        return mAngleY;

    return Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)&player->mPosX);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 65 -- _ZN8dActor_c14FarthestPlayerEv
 * 0x02010958  size 0x24   legacy src/_ZN8dActor_c14FarthestPlayerEv.cpp */
Player *dActor_c::FarthestPlayer()
{
    ClosestPlayer();
    return (Player *)data_0209b450;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 64 -- _ZN8dActor_c18HorzAngleToFPlayerEv
 * 0x0201092c  size 0x2c   legacy src/_ZN8dActor_c18HorzAngleToFPlayerEv.cpp */
s16 dActor_c::HorzAngleToFPlayer()
{
    ClosestPlayer();
    return Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)((char *)data_0209b450 + 0x5c));
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 63 -- _ZN8dActor_c18ClosestWithActorIDEj
 * 0x020108c4  size 0x68   legacy src/_ZN8dActor_c18ClosestWithActorIDEj.cpp */
dActor_c *dActor_c::ClosestWithActorID(u32 actorID)
{
    dActor_c *actor = 0;
    dActor_c *closest = 0;
    Fix12i closestDist = 0x7fffffff;

    for (;;) {
        actor = FindWithActorID(actorID, actor);
        if (!actor)
            break;

        if (actor != this) {
            Fix12i dist = Vec3_Dist((const Vector3 *)&mPosX,
                                    (const Vector3 *)&actor->mPosX);
            if (dist < closestDist) {
                closestDist = dist;
                closest = actor;
            }
        }
    }
    return closest;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 62 -- func_02010844
 * 0x02010844  size 0x80   legacy src/func_02010844.c */
typedef unsigned short u16;
extern "C" {
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(Fix12i y, Fix12i x);
extern s32 Vec3_HorzLen(Vector3* v);
}
extern "C" s32 func_02010844(void* unused, Vector3* v, s16 angle)
{
    s32 dir;
    s32 horzLen;

    dir = _ZN4cstd5atan2E5Fix12IiES1_(v->x, v->z);
    angle = (s16)(dir - angle);
    horzLen = Vec3_HorzLen(v);
    return _ZN4cstd5atan2E5Fix12IiES1_(
        (s32)(((s64)horzLen * data_02082214[((u16)angle >> 4) * 2 + 1] + 0x800) >> 12),
        v->y);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 61 -- _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs
 * 0x02010714  size 0x130   legacy src/_ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs.c */
typedef unsigned int u32;
extern "C" {
dActor_c *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as( u32 actorID, u32 param, const void *pos, const Vector3_16 *rot, s32 areaID, s32 deathTableID);
extern int RandomIntInternal(int *seed);
extern int data_0209e650[];
}
extern "C" void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(
    struct dActor_c *self, const struct Vector3 *pos, s32 count,
    s32 spread, s16 angle)
{
    struct dActor_c *actor;
    s32 ang;
    s32 prev;
    s32 i;
    u32 r;
    s16 ang0;
    struct Vector3_16 *rot;
    s32 deathId;
    u32 id;
    u32 param;

    ang0 = angle;
    rot = 0;
    id = 0x120;
    param = 2;
    deathId = -1;
    prev = 0xff;

    if (count > 1) {
        if (spread < 0x4000)
            spread = 0x4000;
    }

    for (i = 0; i < count; i++) {
        actor = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            id, param, pos, rot,
            *(signed char *)((char *)self + 0xcc), deathId);

        if (actor != 0) {
            do {
                ang = (s32)(((u32)RandomIntInternal(data_0209e650) >> 16) << 27) >> 16;
            } while (ang == prev);

            r = (u32)RandomIntInternal(data_0209e650);
            r >>= 16;

            *(s16 *)((char *)actor + 0x92) = 0;
            spread = (u32)((u32)spread * ((r % 50) + 100)) / 100;
            prev = ang;
            *(s16 *)((char *)actor + 0x94) = (s16)(ang0 + ang);
            *(s16 *)((char *)actor + 0x96) = 0;
            *(s32 *)((char *)actor + 0x98) = spread;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 60 -- _ZN8dActor_c15GivePlayerCoinsER6Playerhj
 * 0x0201061c  size 0xf8   legacy src/_ZN8dActor_c15GivePlayerCoinsER6Playerhj.cpp */
extern "C" {
extern u32 data_02075238[]; /* coins each kind is worth */
extern u16 data_02075230[]; /* actor ID of the popup each kind spawns */
void GiveCoins(s32 playerNo, s32 amount); /* 0x0202a7b8 */
}
void dActor_c::GivePlayerCoins(Player &player, u8 count, u32 coinKind)
{
    if (&player == 0)
        return;
    if (count == 0)
        return;
    if (coinKind >= 3)
        return;

    if (player.mIsUnderwater)
        Sound::PlayBank3(0x12, *(const Vector3 *)&mCamSpacePosX);
    else
        Sound::PlayBank3(0x11, *(const Vector3 *)&mCamSpacePosX);

    s32 amount = count * data_02075238[coinKind];
    GiveCoins(player.mPlayerNo, amount);
    player.Heal(amount << 8);

    /* Through a base pointer, not three field reads: the ROM materialises
       `add r2, r5, #0x5c` once and loads [r2], [r2,#4], [r2,#8] off it.
       Reading player.mPosX/Y/Z directly costs one instruction less (0xf4, not
       0xf8) because the offsets fold into r5. */
    const Vector3 *src = (const Vector3 *)&player.mPosX;
    Vector3 spawnPos;
    spawnPos.x = src->x;
    spawnPos.y = src->y;
    spawnPos.z = src->z;
    spawnPos.y = spawnPos.y + 0x96000;

    dActor_c *popup = Spawn(data_02075230[coinKind], 8, spawnPos, 0,
                         player.mAreaId, -1);
    if (popup != 0)
        *(Player **)((char *)popup + 0x39c) = &player;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 59 -- func_020105cc
 * 0x020105cc  size 0x50   legacy src/func_020105cc.c */
extern "C" {
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 soundID, const struct Vector3 *pos); /* 0x0201264c */
}
extern "C" void func_020105cc(struct dActor_c *thiz, u32 flags)
{
    if (flags & 0x380)
    {
        _ZN5Sound9PlayBank0EjRK7Vector3(0xa, (const Vector3 *)&thiz->mCamSpacePosX);
        return;
    }
    if (flags & 0x40)
    {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, (const Vector3 *)&thiz->mCamSpacePosX);
        return;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 58 -- _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player
 * 0x0201054c  size 0x80   legacy src/_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player.cpp */
int dActor_c::JumpedOnByPlayer(dCc_c &clsn, Player &player)
{
    volatile s32 pad[4];
    s32 vel = player.mVertSpeed;

    (void)&pad;
    if (clsn.flags & 6) {
        if (vel < 0 && player.mIsAirborne != 0
            && mPosY < player.mPosY - 0xa000)
            return 1;
        return 0;
    } else {
        s32 pushbackY = clsn.pushback.y;
        if (vel < 0 && pushbackY < 0 && pushbackY > vel)
            return 1;
        return 0;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 57 -- _ZN8dActor_c24BumpedUnderneathByPlayerER6Player
 * 0x02010518  size 0x34   legacy src/_ZN8dActor_c24BumpedUnderneathByPlayerER6Player.cpp */
extern "C" {
int _ZN8dActor_c24BumpedUnderneathByPlayerER6Player(struct dActor_c *self, char*player) {
if(*(unsigned char*)(player+0x6de)!=0 && *(int*)(player+0xa8)>0
   && *(int*)(player+0x60) < self->mPosY)
  return 1;
return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 56 -- _ZN8dActor_c15IsPlayerInRangeEi
 * 0x020104dc  size 0x3c   legacy src/_ZN8dActor_c15IsPlayerInRangeEi.cpp */
bool dActor_c::IsPlayerInRange(s32 maxDist)
{
    Player *closest = ClosestPlayer();

    return Vec3_Dist((Vector3 *)&mPosX, (Vector3 *)&closest->mPosX) < (maxDist << 12);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 55 -- _ZN8dActor_c15IsPlayerInRangeE5Fix12IiES1_S1_i
 * 0x02010498  size 0x44   legacy src/_ZN8dActor_c15IsPlayerInRangeE5Fix12IiES1_S1_i.c */
extern "C" int _ZN8dActor_c15IsPlayerInRangeE5Fix12IiES1_S1_i(struct dActor_c *self, Fix12i posX, Fix12i posY, Fix12i posZ, int maxDist) {
    struct Vector3 pos;
    pos.x = posX;
    pos.y = posY;
    pos.z = posZ;
    struct dActor_c *closest = (struct dActor_c *)self->ClosestPlayer();
    return Vec3_Dist(&pos, (struct Vector3 *)&closest->mPosX) < (maxDist << 12);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 54 -- _ZN8dActor_c15IsPlayerInRangeERK7Vector3i
 * 0x0201045c  size 0x3c   legacy src/_ZN8dActor_c15IsPlayerInRangeERK7Vector3i.cpp */
bool dActor_c::IsPlayerInRange(const Vector3 &pos, s32 maxDist)
{
    Player *closest = ClosestPlayer();

    return Vec3_Dist(&pos, (Vector3 *)&closest->mPosX) < (maxDist << 12);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 53 -- _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s
 * 0x02010428  size 0x34   legacy src/_ZN8dActor_c12ReflectAngleE5Fix12IiES1_s.c */
extern "C" s16 _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(struct dActor_c *thiz, Fix12i normalX, Fix12i normalZ, s16 angToReflect)
{
    s16 normalAngle = _ZN4cstd5atan2E5Fix12IiES1_(normalX, normalZ);
    s16 delta = (s16)((angToReflect + 0x8000) - normalAngle);
    return (s16)(normalAngle - delta);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 52 -- _ZN8dActor_c14GetSubtractionEss
 * 0x020103f4  size 0x34   legacy src/_ZN8dActor_c14GetSubtractionEss.cpp */
int dActor_c::GetSubtraction(short a, short b)
{
int d=(short)(b-a);
if(d==-0x8000) d=-0x7fff;
if(d<0) d=(short)(-d);
return d;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 51 -- _ZN8dActor_c7FindEggER5dCc_c
 * 0x020103b4  size 0x40   legacy src/_ZN8dActor_c7FindEggER5dCc_c.cpp */
dActor_c *dActor_c::FindEgg(dCc_c &clsn)
{
    u32 id = clsn.otherOwner;
    u32 flags;

    if (id != 0) {
        flags = clsn.hitFlags & 0x2000;
        if (flags != 0)
            return FindWithID(id);
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 50 -- _ZN8dActor_c18FindExplosionActorER5dCc_c
 * 0x02010374  size 0x40   legacy src/_ZN8dActor_c18FindExplosionActorER5dCc_c.cpp */
dActor_c *dActor_c::FindExplosionActor(dCc_c &clsn)
{
    u32 id = clsn.otherOwner;
    u32 flags;

    if (id != 0) {
        flags = clsn.hitFlags & 0x4000;
        if (flags != 0)
            return FindWithID(id);
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 49 -- func_02010304
 * 0x02010304  size 0x70   legacy src/func_02010304.cpp */
extern "C" {
extern void *_ZN8dActor_c10FindWithIDEj(unsigned);
extern int _ZN6Player7TryGrabER8dActor_c(void *, void *);
}
extern "C" {
void *func_02010304(void *target, char *p) {
    unsigned id = *(unsigned *)(p + 0x24);
    void *found;
    if (id != 0 && (found = _ZN8dActor_c10FindWithIDEj(id)) != 0 && (*(int *)(p + 0x20) & 0x1000)) {
        int t = *(unsigned short *)((char *)found + 0xc) == 0xbf;
        if (t && _ZN6Player7TryGrabER8dActor_c(found, target))
            return found;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 48 -- _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j
 * 0x020102b0  size 0x54   legacy src/_ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j.c */
typedef signed char s8;
extern "C" void _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
    dActor_c* self,
    const Vector3* pos,
    const Vector3_16* rot,
    Fix12i horzSpeed,
    Fix12i unk35c,
    u32 param1
) {
    dActor_c* a = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xfe, param1, pos, rot, (s32)self->mAreaId, -1);
    if (a != (dActor_c*)0) {
        a->mHorzSpeed = horzSpeed;
        *(Fix12i *)((char *)a + 0x35c) = unk35c;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 47 -- _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3
 * 0x02010180  size 0x130   legacy src/_ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3.cpp */
extern "C" {
void *func_ov002_020e496c(Player *p);
void MulMat4x3Mat4x3(Matrix4x3 *m1, Matrix4x3 *m0, Matrix4x3 *mF);
void Matrix4x3_FromTranslation(Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z);
void Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3 *mF, short x, short y, short z);
void Vec3_Asr(Vector3 *vF, const Vector3 *v, int amount);
void Vec3_Lsl(Vector3 *vF, const Vector3 *v, int amount);
void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z);
extern Matrix4x3 data_020a0e68;
}
Matrix4x3 *dActor_c::UpdateCarry(Player &player, const Vector3 &vec)
{
    Player *p = &player;
    char *self = (char *)this;
    Vector3 t;
    *(int *)((char *)&t + 0) = 0;
    *(int *)((char *)&t + 4) = 0;
    *(int *)((char *)&t + 8) = 0;
    void *o = func_ov002_020e496c(p);
    char *base = *(char**)((char*)o + 0x14);
    data_020a0e68 = *(Matrix4x3*)((char*)o + 0x1c);
    MulMat4x3Mat4x3((Matrix4x3*)(base + 0x2a0), &data_020a0e68, &data_020a0e68);
    *(Vector3 *)&t = *(Vector3 *)((char *)&data_020a0e68 + 0x24);
    *(short*)(self + 0x94) = *(short*)((char*)p + 0x8e);
    *(short*)(self + 0x8e) = *(short*)(self + 0x94);
    Matrix4x3_FromTranslation(&data_020a0e68,
        *(volatile int *)((char *)&t + 0),
        *(volatile int *)((char *)&t + 4),
        *(volatile int *)((char *)&t + 8));
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(short*)(self + 0x8c), *(short*)(self + 0x8e), *(short*)(self + 0x90));
    Vector3 asr;
    Vec3_Asr(&asr, &vec, 3);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
    *(Fix12i*)(self + 0x5c) = data_020a0e68.m[9];
    *(Fix12i*)(self + 0x60) = data_020a0e68.m[10];
    *(Fix12i*)(self + 0x64) = data_020a0e68.m[11];
    volatile Vector3 lsl;
    Vec3_Lsl((Vector3*)&lsl, (Vector3*)(self + 0x5c), 3);
    *(Fix12i*)(self + 0x5c) = lsl.x;
    *(Fix12i*)(self + 0x60) = lsl.y;
    *(Fix12i*)(self + 0x64) = lsl.z;
    return &data_020a0e68;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 46 -- _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE
 * 0x02010168  size 0x18   legacy src/_ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE.cpp */
extern "C" {
extern void func_0200d8c8(void*, int);
extern void* data_0209f318;
}
extern "C" {
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void* v, int f) {
    func_0200d8c8(data_0209f318, f);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 45 -- _ZN8dActor_c13OnYoshiTryEatEv
 * 0x02010160  size 0x8   legacy src/_ZN8dActor_c13OnYoshiTryEatEv.cpp */
int dActor_c::OnYoshiTryEat()
{
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 44 -- _ZN8dActor_c13OnTurnIntoEggER6Player
 * 0x02010154  size 0xc   legacy src/_ZN8dActor_c13OnTurnIntoEggER6Player.cpp */
int dActor_c::OnTurnIntoEgg(Player &player)
{
    KillAndTrackInDeathTable();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 43 -- _ZN8dActor_c9Virtual50Ev
 * 0x0201014c  size 0x8   legacy src/_ZN8dActor_c9Virtual50Ev.cpp */
int dActor_c::Virtual50()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 42 -- _ZN8dActor_c15OnGroundPoundedERS_
 * 0x02010148  size 0x4   legacy src/_ZN8dActor_c15OnGroundPoundedERS_.cpp */
void dActor_c::OnGroundPounded(dActor_c &pounder)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 41 -- _ZN8dActor_c11OnAttacked1ERS_
 * 0x02010144  size 0x4   legacy src/_ZN8dActor_c11OnAttacked1ERS_.cpp */
int dActor_c::OnAttacked1(dActor_c &attacker)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 40 -- _ZN8dActor_c11OnAttacked2ERS_
 * 0x02010140  size 0x4   legacy src/_ZN8dActor_c11OnAttacked2ERS_.cpp */
int dActor_c::OnAttacked2(dActor_c &attacker)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 39 -- _ZN8dActor_c8OnKickedERS_
 * 0x0201013c  size 0x4   legacy src/_ZN8dActor_c8OnKickedERS_.cpp */
void dActor_c::OnKicked(dActor_c &kicker)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- _ZN8dActor_c8OnPushedERS_
 * 0x02010138  size 0x4   legacy src/_ZN8dActor_c8OnPushedERS_.cpp */
int dActor_c::OnPushed(dActor_c &pusher)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- _ZN8dActor_c24OnHitByCannonBlastedCharERS_
 * 0x02010134  size 0x4   legacy src/_ZN8dActor_c24OnHitByCannonBlastedCharERS_.cpp */
int dActor_c::OnHitByCannonBlastedChar(dActor_c &attacker)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- _ZN8dActor_c15OnHitByMegaCharER6Player
 * 0x02010130  size 0x4   legacy src/_ZN8dActor_c15OnHitByMegaCharER6Player.cpp */
void dActor_c::OnHitByMegaChar(Player &player)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- _ZN8dActor_c19OnHitFromUnderneathERS_
 * 0x0201012c  size 0x4   legacy src/_ZN8dActor_c19OnHitFromUnderneathERS_.cpp */
int dActor_c::OnHitFromUnderneath(dActor_c &attacker)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- _ZN8dActor_c16OnAimedAtWithEggEv
 * 0x02010124  size 0x8   legacy src/_ZN8dActor_c16OnAimedAtWithEggEv.cpp */
int dActor_c::OnAimedAtWithEgg()
{
    return 0x14000;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- _ZN8dActor_c25OnAimedAtWithEggReturnVecEv
 * 0x020100dc  size 0x48   legacy src/_ZN8dActor_c25OnAimedAtWithEggReturnVecEv.cpp */
extern "C" void _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(Vector3 *ret, dActor_c *self)
{
    ret->x = self->mPosX;
    ret->y = self->mPosY;
    ret->z = self->mPosZ;
    ret->y += self->OnAimedAtWithEgg();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- _ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_
 * 0x02010044  size 0x98   legacy src/_ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_.cpp */
extern "C" {
dActor_c *func_ov002_020f0918(dActor_c *number, dActor_c *owner);
}
dActor_c *dActor_c::SpawnNumber(const Vector3 &pos, u32 value, bool packLowNibble,
                          u16 delay, dActor_c *owner)
{
    dActor_c *number;

    if (packLowNibble)
        number = Spawn(0x14a, (value & 0xf) | 0x10, pos, 0, mAreaId, -1);
    else
        number = Spawn(0x14a, value, pos, 0, mAreaId, -1);

    if (number == 0)
        return number;

    *(u16 *)((char *)number + 0x14c) = delay;
    return func_ov002_020f0918(number, owner);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- _ZN8dActor_c9TrackStarEjj
 * 0x0200ff94  size 0xb0   legacy src/_ZN8dActor_c9TrackStarEjj.cpp */
extern "C" {
extern u8 data_0209f220[]; /* the star index this mission was entered for */
extern s8 data_0209f2f8[]; /* the current sublevel id */
extern dActor_c *STAR_MARKERS[12]; /* 0x0209f40c */
void SetStarMarker(s32 slot, dActor_c *actor, s32 markerType);
int IsStarCollectedInCurLevel(u32 starIdx);
}
s32 dActor_c::TrackStar(u32 starIdx, u32 markerType)
{
    s32 type = markerType;

    if (markerType == 2) {
        if (starIdx != data_0209f220[0]) {
            if (SublevelToLevel(data_0209f2f8[0]) <= 0xe)
                return -1;
        }
        if (IsStarCollectedInCurLevel(starIdx))
            type = 3;
    }

    s8 slot;
    for (slot = 0; slot < 0xc; slot++) {
        if (STAR_MARKERS[slot] == 0) {
            SetStarMarker(slot, this, type);
            return slot;
        }
    }
    return -1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- _ZN8dActor_c11UntrackStarERa
 * 0x0200ff60  size 0x34   legacy src/_ZN8dActor_c11UntrackStarERa.cpp */
void dActor_c::UntrackStar(s8 &starID)
{
    s8 slot = starID;
    if (slot < 0)
        return;

    SetStarMarker(slot, 0, 2);
    starID = -1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h
 * 0x0200ff14  size 0x4c   legacy src/_ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h.cpp */
dActor_c *dActor_c::UntrackAndSpawnStar(s8 &trackStarID, u32 starID,
                                  const Vector3 &spawnPos, u8 howToSpawnStar)
{
    UntrackStar(trackStarID);

    return Spawn(0xb2, starID | (howToSpawnStar << 4), spawnPos, 0,
                 mAreaId, -1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- _ZN8dActor_c17GetWaterHeightWDWEv
 * 0x0200fec8  size 0x4c   legacy src/_ZN8dActor_c17GetWaterHeightWDWEv.cpp */
extern "C" {
extern unsigned char data_0209f2c0[];
extern int data_02075244[];
}
s32 dActor_c::GetWaterHeightWDW()
{
    s32 ret = mPosY;
    if (data_0209f2f8[0] == 0x15 && mAreaId == 0) {
        unsigned int i = data_0209f2c0[0];
        if (i >= 3) i = 2;
        ret = data_02075244[i];
    }
    return ret;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- LoadBlueCoinModel
 * 0x0200fe9c  size 0x2c   legacy src/LoadBlueCoinModel.c */
extern "C" {
extern SharedFilePtr data_ov002_0210da00; /* 0x0210da00 */
extern SharedFilePtr data_ov002_0210d9c8; /* 0x0210d9c8 */
extern void _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* ptr); /* 0x02017a3c */
}
extern "C" void LoadBlueCoinModel(void)
{
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da00);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9c8);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- UnloadBlueCoinModel
 * 0x0200fe70  size 0x2c   legacy src/UnloadBlueCoinModel.c */
struct SharedFilePtr { u32 data[4]; };
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);
}
extern "C" void UnloadBlueCoinModel(void)
{
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210da00);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9c8);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- _ZN8dActor_c8PoofDustEv
 * 0x0200fe3c  size 0x34   legacy src/_ZN8dActor_c8PoofDustEv.cpp */
void dActor_c::PoofDust()
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    PoofDustAt(pos);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- _ZN8dActor_c10PoofDustAtERK7Vector3
 * 0x0200fdfc  size 0x40   legacy src/_ZN8dActor_c10PoofDustAtERK7Vector3.cpp */
extern "C" {
extern int _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
}
void dActor_c::PoofDustAt(const Vector3 &pos)
{
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x122, pos.x, pos.y, pos.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x123, pos.x, pos.y, pos.z);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- _ZN8dActor_c14TriplePoofDustEv
 * 0x0200fdc8  size 0x34   legacy src/_ZN8dActor_c14TriplePoofDustEv.cpp */
void dActor_c::TriplePoofDust()
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    TriplePoofDustAt(pos);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- _ZN8dActor_c16TriplePoofDustAtERK7Vector3
 * 0x0200fd74  size 0x54   legacy src/_ZN8dActor_c16TriplePoofDustAtERK7Vector3.cpp */
void dActor_c::TriplePoofDustAt(const Vector3 &pos)
{
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x124, pos.x, pos.y, pos.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x125, pos.x, pos.y, pos.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x126, pos.x, pos.y, pos.z);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN8dActor_c13SmallPoofDustEv
 * 0x0200fd40  size 0x34   legacy src/_ZN8dActor_c13SmallPoofDustEv.cpp */
void dActor_c::SmallPoofDust()
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    DisappearPoofDustAt(pos);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN8dActor_c19DisappearPoofDustAtERK7Vector3
 * 0x0200fd04  size 0x3c   legacy src/_ZN8dActor_c19DisappearPoofDustAtERK7Vector3.cpp */
void dActor_c::DisappearPoofDustAt(const Vector3 &pos)
{
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x127, pos.x, pos.y, pos.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x128, pos.x, pos.y, pos.z);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_0200fccc
 * 0x0200fccc  size 0x38   legacy src/func_0200fccc.c */
struct T{int a;int b;int c;};
extern "C" {
extern int func_0200fc44(int a, Vector3 *pos, int flag);
}
extern "C" int func_0200fccc(char*s,int r1){
struct T t={*(int*)(s+0x5c),*(int*)(s+0x60),*(int*)(s+0x64)};
return func_0200fc44((int)s,(Vector3 *)&t,r1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_0200fc44
 * 0x0200fc44  size 0x88   legacy src/func_0200fc44.c */
typedef struct RaycastGroundPod { int filler[17]; int resultY; int pad[2]; } RaycastGroundPod;
extern "C" {
extern void _ZN9dBgCh_GndC1Ev(RaycastGroundPod* c);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(RaycastGroundPod* c, Vector3* pos, void* actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(RaycastGroundPod* c);
extern void _ZN9dBgCh_GndD1Ev(RaycastGroundPod* c);
}
extern "C" int func_0200fc44(int a, Vector3* pos, int flag) {
    if (flag) {
        RaycastGroundPod rg;
        _ZN9dBgCh_GndC1Ev(&rg);
        int* yp = (int*)((int)pos + 4);
        *yp += 0x32000;
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, pos, 0);
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg))
            pos->y = rg.resultY;
        _ZN9dBgCh_GndD1Ev(&rg);
    }
    int* yp2 = (int*)((int)pos + 4);
    *yp2 += 0x19000;
    return _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb3, pos->x, pos->y, pos->z);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN8dActor_c11LandingDustEb
 * 0x0200fc0c  size 0x38   legacy src/_ZN8dActor_c11LandingDustEb.cpp */
void dActor_c::LandingDust(bool doRaycast)
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    HugeLandingDustAt(pos, doRaycast);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- _ZN8dActor_c17HugeLandingDustAtER7Vector3b
 * 0x0200fb84  size 0x88   legacy src/_ZN8dActor_c17HugeLandingDustAtER7Vector3b.cpp */
struct dBgCh_Gnd {
    char buf[0x50];
    dBgCh_Gnd();
    ~dBgCh_Gnd();
    void SetObjAndPos(const Vector3 &pos, dActor_c *actor);
    int DetectClsn();
};
namespace Particle { struct System {
    static void NewSimple(unsigned int id, Fix12i a, Fix12i b, Fix12i c);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, Fix12i a, Fix12i b, Fix12i c);
 }
extern "C" void _ZN8dActor_c17HugeLandingDustAtER7Vector3b(dActor_c *self, Vector3 *pos, bool b)
{
    if (b) {
        dBgCh_Gnd rc;
        *(Fix12i*)((int)pos + 4) += 0x32000;
        rc.SetObjAndPos(*pos, 0);
        if (rc.DetectClsn())
            pos->y = *(Fix12i*)((char*)&rc + 0x44);
    }
    *(Fix12i*)((int)pos + 4) += 0x28000;
    Particle::_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, pos->x, pos->y, pos->z);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN8dActor_c15HugeLandingDustEb
 * 0x0200fb4c  size 0x38   legacy src/_ZN8dActor_c15HugeLandingDustEb.cpp */
void dActor_c::HugeLandingDust(bool doRaycast)
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    LandingDustAt(pos, doRaycast);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- _ZN8dActor_c13LandingDustAtER7Vector3b
 * 0x0200fac4  size 0x88   legacy src/_ZN8dActor_c13LandingDustAtER7Vector3b.cpp */
extern "C" void _ZN8dActor_c13LandingDustAtER7Vector3b(dActor_c *self, Vector3 *pos, bool b)
{
    if (b) {
        dBgCh_Gnd rc;
        *(Fix12i*)((int)pos + 4) += 0x32000;
        rc.SetObjAndPos(*pos, 0);
        if (rc.DetectClsn())
            pos->y = *(Fix12i*)((char*)&rc + 0x44);
    }
    *(Fix12i*)((int)pos + 4) += 0x5a000;
    Particle::_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb1, pos->x, pos->y, pos->z);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_0200fa8c
 * 0x0200fa8c  size 0x38   legacy src/func_0200fa8c.c */
extern "C" {
extern int func_0200fa04(int a, Vector3 *pos, int flag);
}
extern "C" void func_0200fa8c(void *tv, int a)
{
    int *t = (int *)tv;
    int v[3];
    v[0] = t[0x17];
    v[1] = t[0x18];
    v[2] = t[0x19];
    func_0200fa04((int)t, (Vector3 *)v, a);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_0200fa04
 * 0x0200fa04  size 0x88   legacy src/func_0200fa04.c */
extern "C" int func_0200fa04(int a, Vector3* pos, int flag)
{
    RaycastGroundPod rc;
    if (flag) {
        _ZN9dBgCh_GndC1Ev(&rc);
        *(int*)(((int)pos + 4)) += 0x32000;
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rc, pos, 0);
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&rc))
            pos->y = rc.resultY;
        _ZN9dBgCh_GndD1Ev(&rc);
    }
    *(int*)(((int)pos + 4)) += 0x78000;
    return _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x1b, pos->x, pos->y, pos->z);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- _ZN8dActor_c18GetBitInDeathTableEv
 * 0x0200f9f4  size 0x10   legacy src/_ZN8dActor_c18GetBitInDeathTableEv.cpp */
extern "C" {
int DeathTable_GetBit(int id);
}
int dActor_c::GetBitInDeathTable()
{
    return DeathTable_GetBit(mDeathTableID);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN8dActor_c17TrackInDeathTableEv
 * 0x0200f9e4  size 0x10   legacy src/_ZN8dActor_c17TrackInDeathTableEv.cpp */
extern "C" {
void DeathTable_SetBit(int id);
}
void dActor_c::TrackInDeathTable()
{
    DeathTable_SetBit(mDeathTableID);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- _ZN8dActor_c19UntrackInDeathTableEv
 * 0x0200f9d4  size 0x10   legacy src/_ZN8dActor_c19UntrackInDeathTableEv.cpp */
extern "C" {
void DeathTable_ClearBit(int id);
}
void dActor_c::UntrackInDeathTable()
{
    DeathTable_ClearBit(mDeathTableID);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN8dActor_c24KillAndTrackInDeathTableEv
 * 0x0200f9b8  size 0x1c   legacy src/_ZN8dActor_c24KillAndTrackInDeathTableEv.cpp */
void dActor_c::KillAndTrackInDeathTable()
{
    TrackInDeathTable();
    MarkForDestruction();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN8dActor_c13SpawnSoundObjEj
 * 0x0200f97c  size 0x3c   legacy src/_ZN8dActor_c13SpawnSoundObjEj.cpp */
void dActor_c::SpawnSoundObj(u32 soundObjParam)
{
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        0x167, soundObjParam, (const void *)&mPosX, (const Vector3_16 *)0,
        (s32)mAreaId, -1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN5Sound15PlaySecretSoundEP8dActor_cPt
 * 0x0200f8f8  size 0x84   legacy src/_ZN5Sound15PlaySecretSoundEP8dActor_cPt.cpp */
extern "C" {
int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int soundID, unsigned int vol, unsigned int pan, Fix12i dist, int loop);
static const int kPoolDist = 0x8777;
}
namespace Sound {

int PlaySecretSound(dActor_c* actor, u16* counter)
{
    int ret = 0;
    (void)actor;
    if (*counter < 0x4b) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x14, 0x7f, 0x6b000, 0);
        *counter += 1;
        goto done;
    }
    if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x7f, 0, kPoolDist, 0))
        return 1;
done:
    return ret;
}

}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN5Sound20PlaySmallSecretSoundEP8dActor_cPt
 * 0x0200f874  size 0x84   legacy src/_ZN5Sound20PlaySmallSecretSoundEP8dActor_cPt.cpp */
namespace Sound {

int PlaySmallSecretSound(dActor_c* actor, u16* counter)
{
    int ret = 0;
    (void)actor;
    if (*counter < 0x19) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x29, 0x14, 0x7f, 0x6b000, 0);
        *counter += 1;
        goto done;
    }
    if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x29, 0x7f, 0, kPoolDist, 0))
        return 1;
done:
    return ret;
}

}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_0200f7f0
 * 0x0200f7f0  size 0x84   legacy src/func_0200f7f0.c */
extern "C" int func_0200f7f0(struct dActor_c *actor, u16 *counter)
{
    int ret = 0;
    (void)actor;
    if (*counter < 0x69) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x21, 0x14, 0x7f, 0x6b000, 0);
        *counter += 1;
        goto done;
    }
    if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x21, 0x7f, 0, kPoolDist, 0))
        return 1;
done:
    return ret;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c
 * 0x0200f7a8  size 0x48   legacy src/_ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c.cpp */
void dActor_c::MakeVanishLuigiWork(dCc_c &clsn)
{
    Player *player;

    clsn.flags &= ~2;

    player = ClosestPlayer();
    if (player == 0)
        return;

    if (((u8 *)player)[0x6fb] != 0)
        clsn.flags |= 2;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_0200f760
 * 0x0200f760  size 0x48   legacy src/func_0200f760.c */
extern "C" void func_0200f760(void *self, char *actor)
{
    unsigned char *p;

    *(u32 *)(((int)actor + 0x18)) &= ~2;
    p = (unsigned char *)((dActor_c *)self)->ClosestPlayer();
    if (p == 0) return;
    if (p[0x6fb] != 0) {
        *(u32 *)(((int)(actor + 0x18))) |= 2;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE
 * 0x0200f70c  size 0x54   legacy src/_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE.cpp */
extern "C" {
int _ZN8dActor_c13DistToCPlayerEv(dActor_c *self);
}
extern "C" int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(dActor_c *self, int threshold)
{
    if (data_0209fc68 == 0) {
        int b = (self->mFlags & 8) != 0;
        if (b != 0) {
            if (_ZN8dActor_c13DistToCPlayerEv(self) >= threshold)
                return 1;
        }
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b
 * 0x0200f658  size 0xb4   legacy src/_ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b.cpp */
struct dBgCh_LinPad { char data[0x78]; };
extern "C" {
extern void _ZN9dBgCh_LinC1Ev(dBgCh_LinPad*);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(dBgCh_LinPad*, const Vector3*, const Vector3*, dActor_c*);
extern int _ZN9dBgCh_Lin10DetectClsnEv(dBgCh_LinPad*);
extern Vector3* func_02037dc4(void*);
extern void _ZN9dBgCh_LinD1Ev(dBgCh_LinPad*);
}
extern "C" {
int _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(dActor_c *self, Vector3 *a, Vector3 *out, int doStore){
  dBgCh_LinPad rl;
  _ZN9dBgCh_LinC1Ev(&rl);
  _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(&rl, a, (const Vector3*)out, 0);
  if(_ZN9dBgCh_Lin10DetectClsnEv(&rl)){
    if(doStore){
      Vector3 *p = (Vector3*)(rl.data + 0x54);
      Vector3 *q = func_02037dc4(rl.data + 0x14);
      int x = p->x + q->x;
      int z = p->z + q->z;
      int y = p->y + q->y;
      out->x = x;
      out->y = y;
      out->z = z;
    }
    _ZN9dBgCh_LinD1Ev(&rl);
    return 1;
  }
  _ZN9dBgCh_LinD1Ev(&rl);
  return 0;
}
}
