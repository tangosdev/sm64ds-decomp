//cpp
/* dBgActor_c -- the base of the level-object family, ov002 0x020ee42c..0x020eea84.
 *
 * One translation unit, eleven functions, the way the cartridge's own build had
 * it. This replaces eleven one-function shards; their content is unchanged
 * except where two of them carried stand-in structs that cannot coexist with
 * include/dBgActor_c.h in a single TU (see UpdateKillByMegaChar below).
 *
 * THIS TU OWNS THE CLASS METADATA. Kill (vtable slot 31) is the only virtual
 * dBgActor_c declares out of line -- the destructor is inline in the class body
 * on purpose, so that the ~101 derived classes inline its vptr store -- which
 * makes Kill the key function and anchors _ZTV10dBgActor_c (ov002 0x0210ae38),
 * _ZTI10dBgActor_c (0x021089ec) and _ZTS10dBgActor_c (0x02108a04) here, along
 * with the D1/D0 pair the table points at. None of that data lies inside this
 * entry's licensed .text range, so dsd supplies the cartridge's own bytes and
 * production isolation discards the emitted duplicates; the manifest's
 * compiler_only_output block licenses each one at its measured ROM home.
 *
 * The shards for D1 and D0 each needed a forcing scaffold (`p->~dBgActor_c()`
 * and `delete p`) to make the compiler emit a variant they did not own. This
 * file needs neither: it owns the key function, so the vtable emission drags
 * both variants in by itself.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm 2004/b56 emits one
 * .text section per function in reverse source order, so the highest-address
 * ROM function is written first. Do not reorder.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020ee42c  src/_ZN10dBgActor_cD1Ev.cpp
 *   [1] 0x020ee464  src/_ZN10dBgActor_cD0Ev.cpp
 *   [2] 0x020ee4b0  src/_ZN10dBgActor_c14KillByMegaCharER6Player.cpp
 *   [3] 0x020ee55c  src/_ZN10dBgActor_c4KillEv.cpp
 *   [4] 0x020ee5d0  src/func_ov002_020ee5d0.c
 *   [5] 0x020ee674  src/_ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE.cpp
 *   [6] 0x020ee7cc  src/_ZN10dBgActor_c19UpdateClsnPosAndRotEv.cpp
 *   [7] 0x020ee830  src/_ZN10dBgActor_c21UpdateModelPosAndRotYEv.cpp
 *   [8] 0x020ee870  src/_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_.cpp
 *   [9] 0x020ee934  src/_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_.cpp
 *   [10] 0x020eea50 src/_ZN10dBgActor_cC2Ev.cpp
 */
/* dBgActor_c.h FIRST: it pulls common.h ahead of Model.h, which is the
 * Matrix4x3-spelling rule its own header comment records. */
#include "dBgActor_c.h"
#include "Sound.h"
#include "common.h"
#include "dBgCh_Lin.h"

/* ROM symbols with no header of their own. `extern`, never
 * `extern "C" { ... }` around a mangled name that is already spelled mangled --
 * the block form defines rather than declares. */
extern "C" {
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(dBgActor_c *self, Vector3 *v, int mag);
s16 Vec3_HorzAngle(Vector3 *a, Vector3 *b);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_( u32 id, Fix12i x, Fix12i y, Fix12i z);
extern struct Matrix4x3 data_020a0e68;
extern void Vec3_Asr(struct Vector3 *d, struct Vector3 *s, int sh);
extern void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);
int _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s( dBgW_KcMbg *self, const Matrix4x3 &mat, short angleY);
void Matrix4x3_FromRotationY(Matrix4x3 *mat, int angleY);
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
void Vec3_Add(Vector3 *out, Vector3 *a, Vector3 *b);
unsigned char DecIfAbove0_Byte(unsigned char *p);
void func_ov002_020ee5d0(unsigned char *self, int arg);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN10dBgActor_cC2Ev, 0x020eea50, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dBgActor_cC2Ev
/* recovered: real C++ base-object constructor
 *
 * The ROM's four steps are the dActor_c base step (_ZN8dActor_cC2Ev), the
 * _ZTV10dBgActor_c vptr store, then the two member constructions in
 * declaration order -- mModel at +0xd4, mMeshCollider at +0x124. That is the
 * compiler's own order for `dBgActor_c : dActor_c` with typed members, so
 * the body is empty.
 */
dBgActor_c::dBgActor_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_, 0x020ee934, size 0x11c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dBgActor_c.h"
#include "dBgW.h"
extern "C" {
extern void* _ZN8dActor_c13ClosestPlayerEv(void*);
extern int Vec3_Dist(void*, void*);
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(struct dBgActor_c *self, int a, int b) {
  int on = (self->mFlags & 8) != 0;
  if (on) {
    if (((dBgW *)((char*)&self->mMeshCollider))->IsEnabled())
      ((dBgW *)((char*)&self->mMeshCollider))->Disable();
    return 0;
  }
  if (a == 0) {
    if (!((dBgW *)((char*)&self->mMeshCollider))->IsEnabled())
      ((dBgW *)(((char*)self)+0x124))->Enable((dActor_c *)(((char*)self)));
    goto done;
  }
  {
  struct Vector3 v;
  v.x = self->mPosX;
  v.y = self->mPosY;
  v.z = self->mPosZ;
  if (b == 0) v.y = v.y + self->mClipOffsetY;
  else v.y = v.y + b;
  void* p = _ZN8dActor_c13ClosestPlayerEv(((char*)self));
  int d = Vec3_Dist(&v, (char*)p+0x5c);
  if (d > a) {
    if (((dBgW *)((char*)&self->mMeshCollider))->IsEnabled())
      ((dBgW *)((char*)&self->mMeshCollider))->Disable();
    return 0;
  }
  if (!((dBgW *)((char*)&self->mMeshCollider))->IsEnabled())
    ((dBgW *)(((char*)self)+0x124))->Enable((dActor_c *)(((char*)self)));
  }
done:
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_, 0x020ee870, size 0xc4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dBgActor_c.h"
#include "dBgW.h"
extern "C" {
extern void* _ZN8dActor_c13ClosestPlayerEv(void*);
extern int Vec3_Dist(void*, void*);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(struct dBgActor_c *self, int a, int b) {
  struct Vector3 v;
  v.x = self->mPosX;
  v.y = self->mPosY;
  v.z = self->mPosZ;
  if (a == 0) a = self->mClipRadius << 3;
  if (b == 0) v.y = v.y + self->mClipOffsetY;
  else v.y = v.y + b;
  void* p = _ZN8dActor_c13ClosestPlayerEv(((char*)self));
  int d = Vec3_Dist(&v, (char*)p+0x5c);
  if (d > a) {
    if (((dBgW *)((char*)&self->mMeshCollider))->IsEnabled())
      ((dBgW *)((char*)&self->mMeshCollider))->Disable();
    return 0;
  }
  if (!((dBgW *)((char*)&self->mMeshCollider))->IsEnabled())
    ((dBgW *)(((char*)self)+0x124))->Enable((dActor_c *)(((char*)self)));
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN10dBgActor_c21UpdateModelPosAndRotYEv, 0x020ee830, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dBgActor_c21UpdateModelPosAndRotYEv
/* recovered: named members + shared header, real C++ method
 *
 * `((char *)this) + 0xf0` and the three unk_114/118/11c writes were the same object
 * reached two different ways: 0xf0 is mModel.mat4x3, and 0x114/0x118/0x11c are its
 * translation row, m[9]..m[11]. Build the rotation into the model's matrix, then
 * drop the position into it -- at 1/8 scale, which is the model-space unit.
 */
void dBgActor_c::UpdateModelPosAndRotY()
{
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.m[9]  = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN10dBgActor_c19UpdateClsnPosAndRotEv, 0x020ee7cc, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dBgActor_c19UpdateClsnPosAndRotEv
/* recovered: named members + shared header, real C++ method
 *
 * Was written through three stand-in structs -- an `Obj` whose only job was to
 * reach 0x2ec by counting bytes, an `MMC` that reached 0x124 the same way, and an
 * `M4` standing in for Matrix4x3. All three named offsets the header now names:
 * 0x2ec is mClsnMat, 0x124 is mMeshCollider, and 0xf0 -- reached as `&unk_0f0` --
 * is mModel.mat4x3.
 *
 * With those named, what the function does is legible: take the model's transform,
 * replace its translation row with the actor's position, and hand it to the
 * collider. m[9], m[10] and m[11] are that row, which is why the generated header's
 * unk_310/unk_314/unk_318 sat exactly 0x24/0x28/0x2c into the matrix.
 */
void dBgActor_c::UpdateClsnPosAndRot()
{
    mClsnMat = mModel.mat4x3;
    mClsnMat.m[9]  = mPosX;
    mClsnMat.m[10] = mPosY;
    mClsnMat.m[11] = mPosZ;
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(&mMeshCollider, mClsnMat, mAngleY);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE, 0x020ee674, size 0x158 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE
/* Reconciled against include/dBgActor_c.h for the merged TU: the shadow
 * `dBgActor_c'/`dActor_c'/`Matrix4x3'/`Fix12' the one-function file carried
 * cannot coexist with the real header in one translation unit. The three
 * shorts at 0x8c/0x8e/0x90 are mAngleX/mAngleY/mAngleZ, 0x94 is mPrevAngleY,
 * and slot 31 is Kill -- all named by include/dActor_c.h and this class's own
 * header, so the raw offsets are gone and the call through the placeholder
 * vtable struct is a plain virtual Kill(). */
int dBgActor_c::UpdateKillByMegaChar(s16 a, s16 b, s16 c, Fix12<int> d)
{
    Vector3 vin;
    Vector3 vmid;
    Vector3 vout;

    if (this->unk_31c == 0) return 0;

    vin.x = 0;
    vin.y = 0;
    vin.z = 0xc8000;
    vmid.x = 0;
    vmid.y = 0;
    vmid.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, this->mPrevAngleY);
    MulVec3Mat4x3(&vin, &data_020a0e68, &vmid);
    Vec3_Add(&vout, (Vector3 *)&this->mPosX, &vmid);
    vmid = vout;

    dBgCh_Lin ray;
    ray.SetObjAndLine(*(Vector3 *)&this->mPosX, vmid, this);
    if (ray.DetectClsn()) {
        *(s16*)((int)((char*)this + 0x94)) =
            (s16)(*(s16*)((int)((char*)this + 0x94)) + 0x8000);
    }
    *(s16*)((int)((char*)this + 0x8c)) =
        (s16)(*(s16*)((int)((char*)this + 0x8c)) + a);
    *(s16*)((int)((char*)this + 0x8e)) =
        (s16)(*(s16*)((int)((char*)this + 0x8e)) + b);
    *(s16*)((int)((char*)this + 0x90)) =
        (s16)(*(s16*)((int)((char*)this + 0x90)) + c);
    this->UpdatePos(0);
    if (DecIfAbove0_Byte(&this->unk_31d) == 0) {
        this->Kill();
    }
    func_ov002_020ee5d0((unsigned char*)this, d.val);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov002_020ee5d0, 0x020ee5d0, size 0xa4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020ee5d0
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020ee5d0(unsigned char *self, int arg)
{
    struct Vector3 v;
    Vec3_Asr(&v, (struct Vector3 *)(self + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, arg >> 3, 0);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
        *(s16 *)(self + 0x8c), *(s16 *)(self + 0x8e), *(s16 *)(self + 0x90));
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, -arg >> 3, 0);
    *(struct Matrix4x3 *)(self + 0xf0) = data_020a0e68;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN10dBgActor_c4KillEv, 0x020ee55c, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dBgActor_c4KillEv
/* dBgActor_c::Kill() at ov002 0x020ee55c, 0x74 bytes -- vtable slot 31.
 *
 * THE SLOT THIS CLASS ADDS. dActor_c's table ends at slot 30; Kill is the one new
 * virtual dBgActor_c declares, and 97 of the 101 classes deriving from it have
 * exactly 32 slots because of this function. See include/dBgActor_c.h.
 *
 * Kill is also, necessarily, dBgActor_c's key function: the destructor is inline
 * on purpose (every subclass inlines its body rather than calling
 * _ZN10dBgActor_cD1Ev), so this is the first out-of-line virtual and this TU emits
 * _ZTV10dBgActor_c, _ZTI10dBgActor_c and the destructor variants. objisolate.py
 * reduces the object back to this one function before the byte gates see it.
 *
 * The particle spawns 0x64000 -- one 20.12 unit is 0x1000, so 100 units --
 * above the platform, and the poof and the sound both take the position by
 * reference. mCamSpacePos is read as a Vector3 through its first member, which
 * is how the rest of the tree spells that triple (see BulletBill::Behavior).
 *
 * Particle::System::NewSimple stays spelled as its mangled name: its parameters
 * are Fix12<int> BY VALUE, and declaring the true types changes how the caller
 * passes them and breaks the bytes. Same reason, same note, as
 * src/_ZN8dActor_c10PoofDustAtERK7Vector3.cpp -- notes/mwccarm-codegen.md 6az. */
void dBgActor_c::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    Fix12i x = mPosX;
    Fix12i y = mPosY + 0x64000;
    Fix12i z = mPosZ;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xa, pos.x, pos.y, pos.z);
    /* MEMBERWISE, NOT `dustPos = pos`. Vector3 declares a destructor (see
       types.h), so it is non-POD, and a whole-object assignment compiles to an
       ldm/stm pair -- four instructions where the ROM has six. Three field
       stores are what the cartridge does. */
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN10dBgActor_c14KillByMegaCharER6Player, 0x020ee4b0, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dBgActor_c14KillByMegaCharER6Player
/* dBgActor_c::KillByMegaChar -- shake the ground at this actor's position, then
 * launch it away from the player and take its mesh collider out of the world.
 *
 * Earthquake carries Fix12<int> in its mangled name and is not declared in
 * include/dActor_c.h, so the call stays an extern-C mangled free function.
 * Vec3_HorzAngle has no header either.
 *
 * The player's position is reached by offset, not by member: the parameter is a
 * `Player &' that include/dBgActor_c.h only forward-declares, and pulling in
 * Player.h here to name three words would put a 0x5c0-byte class definition into
 * a translation unit that has no other use for it.
 */
void dBgActor_c::KillByMegaChar(Player &player_)
{
    Vector3 v;
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, &v, 0x5dc000);
    unk_31c = 1;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0x14000;
    mVertSpeed = 0x1e000;
    mPrevAngleY = Vec3_HorzAngle((Vector3 *)((char *)&player_ + 0x5c), (Vector3 *)&mPosX);
    unk_31d = 0x1e;
    if (mMeshCollider.IsEnabled() == 0)
        return;
    mMeshCollider.Disable();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN10dBgActor_cD0Ev, 0x020ee464, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dBgActor_cD0Ev
/* recovered: real C++ deleting destructor, defined inline in the header
 *
 * ~dBgActor_c is defined in the class body -- the seventy-odd classes derived
 * from it inline its vptr store rather than calling it, which the compiler can
 * only do from a visible body. So this file cannot define it, and a TU that
 * merely includes the header emits nothing. The delete-expression below forces
 * the deleting destructor's own out-of-line copy into existence; objisolate
 * keeps the one this file is bound to and drops the rest.
 *
 * A DELETE-EXPRESSION, NOT `p->~dBgActor_c()`, AND THE DIFFERENCE IS NEW. While
 * dBgActor_c declared no virtual but its inline destructor it had no key
 * function, so any TU that touched the destructor group emitted all of it and
 * a plain destructor call was enough. dBgActor_c now declares Kill (slot 31, see
 * the header), which IS a key function -- so D0 is emitted only where the
 * vtable is, and the destructor call here stopped producing it: this file
 * compiled to D1 plus the forcing function, two .text sections, and dropped
 * out of the build with every byte gate still green. `delete p` asks for the
 * deleting half by name and brings it back. Same shape, same fix, same
 * sentence in src/actors/dScMgSingle3DBase_c.cpp, which has had a key
 * function since #1544.
 *
 * D0 is the deleting half: destroy through dBgActor_c and dActor_c, then hand the
 * object back through dActor_c's inline operator delete, which is why nothing
 * here mentions a heap.
 */
/* Nothing forces D0 here: this TU defines Kill (slot 31), the class's key
 * function, so the vtable is emitted here and drags both destructor variants
 * with it. The one-function file needed a `delete p' scaffold precisely
 * because it did NOT own the key function. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN10dBgActor_cD1Ev, 0x020ee42c, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dBgActor_cD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * ~dBgActor_c is defined in the class body, not here, and that is load-bearing:
 * the seventy-odd classes derived from dBgActor_c inline this destructor's vptr
 * store rather than calling it, which the compiler can only do from a visible
 * body. include/dBgActor_c.h records the measurement.
 *
 * So this file cannot define it -- that would be a redefinition -- and a TU
 * that merely includes the header emits nothing at all. The explicit
 * destructor call below is what forces the out-of-line copy the vtable points
 * at into existence. It is never called; objisolate keeps the function this
 * file declares and drops the rest.
 *
 * The body it forces is the class's own layout evidence: the ROM destroys a
 * dBgW_KcMbg at 0x124 and a Model at 0xd4, and both offsets are where
 * those classes' asserted sizes put them.
 */
/* Nothing forces D1 here either -- see the D0 note above. */
