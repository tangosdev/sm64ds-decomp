//cpp
/* The tilting platforms' shared base -- ov002/daObjGuragura_c.
 *
 * The class is abstract in slots 0 (InitResources) and 3 (CleanupResources), so
 * its setup and teardown are not vtable slots at all: each of the two leaves --
 * daObjKm2_Gura_c (ov045) and daObjFl_Gura_c (ov064) -- calls
 * func_ov002_020b6244 and func_ov002_020b60fc below with its own file table.
 * include/daObjGuragura_c.h carries the class banner.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here.
 *
 * Folded from seven one-function sources, which this promotion deletes. They
 * are listed by the symbol each one carried, in ROM address order:
 *   [0] 0x020b6074  func_ov002_020b6074
 *   [1] 0x020b60fc  func_ov002_020b60fc
 *   [2] 0x020b6144  _ZN15daObjGuragura_c6RenderEv
 *   [3] 0x020b616c  _ZN15daObjGuragura_c8BehaviorEv
 *   [4] 0x020b6244  func_ov002_020b6244
 *   [5] 0x020b62cc  func_ov002_020b62cc
 *   [6] 0x020b6374  func_ov002_020b6374
 *
 * THE DESTRUCTOR PAIR IS NOT CLAIMED. _ZN15daObjGuragura_cD0Ev (0x020b5fd8) and
 * _ZN15daObjGuragura_cD1Ev (0x020b6030) sit immediately below this range and
 * keep their own delinks entries. This TU emits both anyway -- Render is the
 * class's key function, so _ZTV15daObjGuragura_c is anchored here and its slots
 * 16 and 17 odr-use them -- but the cartridge orders them D0-then-D1 and
 * mwccarm emits the vague-linkage pair of an inline-in-class destructor
 * D1-then-D0. The manifest's boundary_evidence records the measurement and why
 * the one source form that would emit D0 first is refuted by the ROM. Nothing
 * here forces or reorders them; the copies are licensed as deadstrip-duplicate
 * against their ROM homes.
 */

#include "daObjGuragura_c.h"

extern "C" {
/* Used by more than one member below; each is spelled the way the legacy file
 * that held the first member to need it spelled it. */
extern int _ZN4dBgW9IsEnabledEv(void *);
extern void _ZN4dBgW7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
/* The arm9 scratch matrix both halves of the tilt pipeline stage through. */
extern struct Matrix4x3 data_020a0e68;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov002_020b6374, 0x020b6374, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b6374
/* The dBgW collision callback func_ov002_020b6244 installs, and the only thing
 * in the image that names func_ov002_020b62cc: a three-argument thunk that
 * drops the collider word and forwards the other two. Its own address is
 * reached exactly once, from the literal pool at ov002 0x020b62c8 inside
 * func_ov002_020b6244, so it is TU-local and keeps its address-derived name. */
extern "C" {
void func_ov002_020b62cc(unsigned char *self, unsigned char *arg);

void func_ov002_020b6374(void *a, void *b, void *c) {
    func_ov002_020b62cc((unsigned char *)b, (unsigned char *)c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov002_020b62cc, 0x020b62cc, size 0xa8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b62cc
/* What a landing on the slab does: flag the hit at 0x34c, take the collider
 * matrix at 0x2ec into the arm9 scratch matrix and invert it so the toucher's
 * position can be read in slab space, double the vertical component, and make
 * the new target quaternion at 0x330 the rotation that carries +Y onto it.
 * Then arm the settle countdown at 0x34d with 10 frames, which is what stops
 * daObjGuragura_c::Behavior below reloading the rest pose from data_02092768
 * until the wobble has had time to run.
 *
 * SEMANTIC CORROBORATION OF MEMBERSHIP: 0x330, 0x34c and 0x34d are
 * daObjGuragura_c's own fields -- past sizeof(dBgActor_c) = 0x320 and inside
 * sizeof(daObjGuragura_c) = 0x350, the literal both factories pass to
 * fBase_c::operator new. A function storing there is storing into this class,
 * and Behavior below reads all three back. */
extern "C" {
extern void InvMat4x3(struct Matrix4x3 *d, struct Matrix4x3 *s);
extern void MulVec3Mat4x3(struct Vector3 *v, struct Matrix4x3 *m, struct Vector3 *out);
extern void Quaternion_FromVector3(int *q, struct Vector3 *from, struct Vector3 *to);
extern void Quaternion_Normalize(int *q);

void func_ov002_020b62cc(unsigned char *self, unsigned char *arg)
{
    struct Vector3 v;
    struct Vector3 axis;
    *(u8 *)(self + 0x34c) = 1;
    data_020a0e68 = *(struct Matrix4x3 *)(self + 0x2ec);
    InvMat4x3(&data_020a0e68, &data_020a0e68);
    MulVec3Mat4x3((struct Vector3 *)(arg + 0x5c), &data_020a0e68, &v);
    v.y = v.y << 1;
    axis.x = 0;
    axis.z = 0;
    axis.y = 0x1000;
    Quaternion_FromVector3((int *)(self + 0x330), &axis, &v);
    Quaternion_Normalize((int *)(self + 0x330));
    *(u8 *)(self + 0x34d) = 0xa;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov002_020b6244, 0x020b6244, size 0x88 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b6244
/* The shared resource setup both daObjGuragura_c leaves call: load the model
 * file into dBgActor_c's Model at 0xd4, place it, load the collision file into
 * the dBgW_KcMbg at 0x124 with the leaf's CLPS block, then install
 * func_ov002_020b6374 above as the collider's callback.
 *
 * Reached only through a literal-pool load inside each leaf's own
 * InitResources -- ov045 0x02111d04, inside
 * _ZN15daObjKm2_Gura_c13InitResourcesEv (0x02111cf8, size 0x14), and ov064
 * 0x021180f4, inside _ZN14daObjFl_Gura_c13InitResourcesEv (0x021180e8, size
 * 0x14). Nothing in the ROM names it and no vtable slot holds it, so the
 * address-derived name stays. */
extern "C" {
/* The legacy .c file spelled the fourth SetFile parameter through a local
 * `typedef int Fix12;`. That name is a CLASS TEMPLATE in include/types.h
 * (Fix12<int>), so the typedef is a redefinition in C++ and the plain `int`
 * the mangled name's by-value Fix12<int> lowers to is written directly
 * instead -- the same spelling the matched sibling ov002/daObjKaitendai_c
 * uses for the same call. */
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, void *, int, int);
extern int _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *);
extern int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, void *, void *, int, short, void *);
extern int func_020393c4(void *, void *);

int func_ov002_020b6244(void *c, void **f) {
  void *mdl;
  void *kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(f[0]);
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char *)c + 0xd4, mdl, 1, -1);
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
  kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(f[1]);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char *)c + 0x124, kcl, (char *)c + 0x2ec, 0x1000, *(short *)((char *)c + 0x8e), f[2]);
  func_020393c4((char *)c + 0x124, &func_ov002_020b6374);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN15daObjGuragura_c8BehaviorEv, 0x020b616c, size 0xd8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjGuragura_c8BehaviorEv
/* daObjGuragura_c::Behavior -- vtable slot 6.
 *
 * Attributed by the vtable, not by a `recovered name:` comment: the old file
 * carried none, only the func_ov002_ address. The ROM's table at ov002 0x02109084
 * holds 0x020b616c in slot 6, which is what include/daObjGuragura_c.h's banner
 * already records -- it cites this very function as the witness that 0x320 and
 * 0x330 are two quaternions.
 *
 * The fields are spelled by name now that the receiver is typed: mTilt is the live
 * orientation, mTiltTarget what it lerps toward, mSettleDelay the countdown that
 * gates reloading the target, and mBumped the per-frame flag the body clears on the
 * way out. dActor_c::mFlags bit 3 is the "suppressed" bit -- while it is set the class
 * disables its collider and does nothing else.
 *
 * UpdateClsnPosAndRot is dBgActor_c's own non-virtual member, so it is a call now
 * rather than an `extern "C"` declaration of the mangled symbol. IsClsnInRange stays
 * mangled: include/dBgActor_c.h does not declare it, and that header belongs to the
 * whole dBgActor_c family rather than to this slice. dBgW's two members
 * stay mangled for the same reason.
 *
 * Was a C99 file. The `(int)((expr) != 0) != 0` idiom around the mFlags test is kept
 * verbatim rather than simplified: in C++ the inner `!= 0` is a bool and the width
 * of the cast is what steers the compare (see notes on the C++ bool widening cast),
 * so rewriting it is a byte-level change, not a tidy-up. */
extern "C" {
int  _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *thiz, int x, int z);
unsigned char DecIfAbove0_Byte(unsigned char *p);
void Quaternion_SLerp(char *out, char *a, int t, char *b);
void func_ov002_020b6074(char *c);
extern int data_02092768[4];
}

s32 daObjGuragura_c::Behavior()
{
    char *c = (char *)this;

    if ((int)((mFlags & 8) != 0) != 0) {
        if (_ZN4dBgW9IsEnabledEv(c + 0x124)) {
            _ZN4dBgW7DisableEv(c + 0x124);
        }
        return 1;
    }
    if (DecIfAbove0_Byte(&mSettleDelay) == 0) {
        mTiltTarget[0] = data_02092768[0];
        mTiltTarget[1] = data_02092768[1];
        mTiltTarget[2] = data_02092768[2];
        mTiltTarget[3] = data_02092768[3];
    }
    Quaternion_SLerp((char *)mTilt, (char *)mTiltTarget, 0x199, (char *)mTilt);
    func_ov002_020b6074(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0)) {
        UpdateClsnPosAndRot();
    }
    mBumped = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN15daObjGuragura_c6RenderEv, 0x020b6144, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjGuragura_c6RenderEv
/* daObjGuragura_c::Render -- vtable slot 9, ov002 0x020b6144.
 * Render is the FIRST virtual include/daObjGuragura_c.h declares that is not
 * inline, so it is this class's key function and this TU -- which now defines
 * it -- is the one that emits _ZTV15daObjGuragura_c. That is expected, and the
 * manifest licenses the emitted table and its RTTI as deadstrip-data against
 * their ROM homes.
 *
 * Render dispatches through dBgActor_c's own mModel (0xd4), exactly as
 * every sibling in this series does. */
s32 daObjGuragura_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- func_ov002_020b60fc, 0x020b60fc, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b60fc
/* The teardown half of the pair: disable the mesh collider if it is live, then
 * release the leaf's two SharedFilePtrs. Reached only through a literal-pool
 * load inside each leaf's own CleanupResources -- ov045 0x02111cf0, inside
 * _ZN15daObjKm2_Gura_c16CleanupResourcesEv (0x02111ce4, size 0x14), and ov064
 * 0x021180e0, inside _ZN14daObjFl_Gura_c16CleanupResourcesEv (0x021180d4, size
 * 0x14). Instruction-for-instruction the same body daObjKaitendai_c,
 * daObjUkiyuka_c, daObjKuruma_c and daObjKurumajiku_c each carry their own copy
 * of, which is why each copy sits inside its own class's run rather than being
 * shared. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020b60fc(char *t, void **f){
  if(_ZN4dBgW9IsEnabledEv(t+0x124))
    _ZN4dBgW7DisableEv(t+0x124);
  _ZN13SharedFilePtr7ReleaseEv(f[0]);
  _ZN13SharedFilePtr7ReleaseEv(f[1]);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- func_ov002_020b6074, 0x020b6074, size 0x88 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b6074
/* The per-frame half of the tilt: turn the live quaternion at 0x320 into a
 * matrix, offset it by the actor's position scaled down by 8, apply the facing
 * angle at 0x8e, and publish the result as the model matrix at 0xf0. Reached
 * exactly once in the image, by an ordinary `bl` from
 * daObjGuragura_c::Behavior above at ov002 0x020b6200, so it is TU-local and
 * keeps its address-derived name. */
extern "C" {
extern void Matrix4x3_FromQuaternion(void* q, struct Matrix4x3* m);
extern void Vec3_Asr(struct Vector3* d, struct Vector3* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void MulMat4x3Mat4x3(struct Matrix4x3* a, void* b, void* c);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short a);
void func_ov002_020b6074(char* c){
  struct Matrix4x3 q;
  struct Vector3 v;
  Matrix4x3_FromQuaternion(c+0x320, &q);
  Vec3_Asr(&v, (struct Vector3*)(c+0x5c), 3);
  Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
  MulMat4x3Mat4x3(&q, &data_020a0e68, &data_020a0e68);
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(c+0x8e));
  *(struct Matrix4x3*)(c+0xf0) = data_020a0e68;

}
}

/* -------------------------------------------------------------------------- */
/* BELOW THIS RANGE, NOT CLAIMED -- _ZN15daObjGuragura_cD0Ev (0x020b5fd8) and
 * _ZN15daObjGuragura_cD1Ev (0x020b6030).
 *
 * NOT WRITTEN OUT. ~daObjGuragura_c is defined inline in the class body --
 * include/daObjGuragura_c.h records why: both descendants inline its vptr store
 * instead of calling it, which the compiler can only do from a visible body.
 * Render above is this class's key function, so this TU emits
 * _ZTV15daObjGuragura_c and its slots 16 and 17 odr-use the two variants
 * anyway. Their emission ORDER is the compiler's, not the source's, and it does
 * not agree with the cartridge; see the file banner.
 * -------------------------------------------------------------------------- */
