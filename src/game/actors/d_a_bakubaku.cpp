//cpp
/* Reconstructed ov032/daBakubaku_c translation unit.
 *
 * This one file is the production source for the whole class: the ROM build
 * takes every one of these functions from a single object. It licenses the
 * contiguous .text run 0x021111a0..0x021124a8 in ov032
 * (config/tu_manifest.d/ov032/daBakubaku_c.json and
 * config/arm9/overlays/ov032/delinks.txt), where twenty-four separate
 * one-function entries used to stand.
 *
 * WRITTEN IN REVERSE ROM ORDER. mwccarm 2004/b56 emits one .text section per
 * function in the REVERSE of source order, so the highest-address ROM function
 * is written FIRST here and the lowest LAST. Do not reorder: the whole-range
 * link checks emission order and refuses anything else.
 *
 * Keep the class initializer first. The inline destructor declared last in
 * daBakubaku_c emits the retail D1/D0 pair first and emits no D2 body.
 *
 * The functions, in ROM address order:
 *   [0]  0x021111a0  _ZN12daBakubaku_cD1Ev
 *   [1]  0x021111f0  _ZN12daBakubaku_cD0Ev
 *   [2]  0x02111254  func_ov032_02111254
 *   [3]  0x02111350  func_ov032_02111350
 *   [4]  0x021113fc  func_ov032_021113fc
 *   [5]  0x02111620  func_ov032_02111620
 *   [6]  0x02111814  func_ov032_02111814
 *   [7]  0x02111830  func_ov032_02111830
 *   [8]  0x02111b50  func_ov032_02111b50
 *   [9]  0x02111b9c  func_ov032_02111b9c
 *   [10] 0x02111d58  func_ov032_02111d58
 *   [11] 0x02111d7c  func_ov032_02111d7c
 *   [12] 0x02111dd8  func_ov032_02111dd8
 *   [13] 0x02111e24  func_ov032_02111e24
 *   [14] 0x02111f9c  func_ov032_02111f9c
 *   [15] 0x02111ff4  func_ov032_02111ff4
 *   [16] 0x02112044  func_ov032_02112044
 *   [17] 0x02112124  daBakubaku_c::CleanupResources
 *   [18] 0x02112160  daBakubaku_c::OnPendingDestroy
 *   [19] 0x02112164  daBakubaku_c::Render
 *   [20] 0x021121b4  daBakubaku_c::Behavior
 *   [21] 0x021122dc  daBakubaku_c::InitResources
 *   [22] 0x02112444  daBakubaku_c::OnAimedAtWithEgg
 *   [23] 0x0211244c  daBakubaku_c_classInit
 *
 * FOLLOW-UP, not done here: several of the shadow declarations below duplicate
 * things include/*.h already carries. Each swap changes what the compiler knows
 * about a type and has to be re-proved against the ROM one at a time, so they
 * are left as the legacy files spelled them.
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daBakubaku_c.h"
#include "common.h"
#include "types.h"
#include "decl_common.h"
#include "dCc_c.h"
#include "Player.h"
#include "SharedFilePtr.h"

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* shadow struct 'BCA_File' */
struct BCA_File;

/* shadow struct 'Klass' */
struct Klass;

/* shadow typedef 'void' */
typedef void (Klass::*PMF)();

/* shadow struct 'StateEntry' */
struct StateEntry { char pad[8]; PMF handler; };

extern "C" {
extern char* _ZN8dActor_c13ClosestPlayerEv(char* c);
extern int Vec3_HorzDist(void* a, void* b);
extern int data_ov032_02113abc[];
extern int data_ov032_02113a7c[];
extern int data_0209f32c[];
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void* self);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void* c, void* v);
void* _ZN8dActor_c10FindWithIDEj(u32 id);
void _ZN12dEnemyBase_c22SpawnMegaCharParticlesER8dActor_cPc(void* self, void* a, void* p);
void _ZN8dActor_c8PoofDustEv(void* self);
void _ZN6Player16IncMegaKillCountEv(void* p);
void func_02012694(int a, void* p);
void _ZN8dActor_c24KillAndTrackInDeathTableEv(void* self);
int func_ov002_020c5cd8(void* a, void* self);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, void* v, u32 a, int f, u32 c, u32 d, u32 e);
extern "C" int func_ov032_02111350(char *c);
extern "C" int func_ov032_02111254(void *c);
extern "C" void Matrix4x3_FromRotationY(void *m, short angle);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(void *m, short angX);
extern "C" void MulVec3Mat4x3(void *in, void *m, void *out);
extern "C" int _ZN9Animation8FinishedEv(void *self);
extern "C" int _Z14ApproachLinearRsss(short *p, short target, short step);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix, unsigned int b);
extern "C" int func_ov032_02111ff4(void *c, void *p);
extern "C" short Vec3_VertAngle(void *a, void *b);
extern int data_020a0e68[];
extern u32 func_02022c80(u32, u32, Fix12i, Fix12i, Fix12i, const void *);
extern u32 func_02022d00(u32, u32, Fix12i, Fix12i, Fix12i, void *);
short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
int RandomIntInternal(int* seed);
extern int data_0209e650[];
extern void _Z14ApproachLinearRiii(int* p, int t, int s);
extern int data_ov032_02113aac[];
extern int data_ov032_02113a9c[];
extern void _ZN13SharedFilePtr7ReleaseEv(void *sfp);
extern int data_ov032_02113a40[];
unsigned short DecIfAbove0_Short(unsigned short *p);
void func_ov032_02112044(char *c);
extern "C" void func_ov032_021113fc(void *self);
/* Three SharedFilePtrs; CleanupResources releases exactly these three. */
/* A Vector3 apiece: where each cylinder sits relative to the actor. */
/* All three carry Fix12<int> parameters, so their definitions stay mangled free
functions -- see include/ShadowModel.h. A CALL is unaffected. extern "C" so the
reference is mangled once and not twice. */
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
dCcAcPos_c *self, dActor_c *actor, const Vector3 &offset,
int radius, int height, u32 d, u32 e);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
dBgCh_Actr *self, dActor_c *actor, int radius, int height, void *a, void *b);
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c13ClosestPlayerEv, from the legacy file for func_ov032_02111350, NOT applied: extern char* _ZN8dActor_c13ClosestPlayerEv(void); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzDist, from the legacy file for func_ov032_02111350, NOT applied: extern int Vec3_HorzDist(const struct Vector3* a, const struct Vector3* b); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c13ClosestPlayerEv, from the legacy file for func_ov032_02111620, NOT applied: extern "C" void *_ZN8dActor_c13ClosestPlayerEv(void *self); */
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_FromRotationY, from the legacy file for func_ov032_02111830, NOT applied: extern void Matrix4x3_FromRotationY(void *m, s16 angY); */
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_ApplyInPlaceToRotationX, from the legacy file for func_ov032_02111830, NOT applied: extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX); */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for func_ov032_02111830, NOT applied: extern void MulVec3Mat4x3(void *v, void *m, void *out); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov032_02111350, from the legacy file for func_ov032_02111830, NOT applied: extern int func_ov032_02111350(char *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov032_02111254, from the legacy file for func_ov032_02111830, NOT applied: extern int func_ov032_02111254(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for func_ov032_02111830, NOT applied: extern void _Z14ApproachLinearRsss(s16* p, s16 target, s16 step); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN9Animation8FinishedEv, from the legacy file for func_ov032_02111830, NOT applied: extern int _ZN9Animation8FinishedEv(void* a); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj, from the legacy file for func_ov032_02111830, NOT applied: extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov032_02111ff4, from the legacy file for func_ov032_02111830, NOT applied: extern int func_ov032_02111ff4(void*, void*); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov032_02111350, from the legacy file for func_ov032_02111b9c, NOT applied: int func_ov032_02111350(unsigned char* c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov032_02111254, from the legacy file for func_ov032_02111b9c, NOT applied: int func_ov032_02111254(unsigned char* c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov032_02111ff4, from the legacy file for func_ov032_02111b9c, NOT applied: void func_ov032_02111ff4(unsigned char* c, void* p); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj, from the legacy file for func_ov032_02111b9c, NOT applied: void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, BCA_File* f, int a, int fix, unsigned int j); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for func_ov032_02111b9c, NOT applied: void _Z14ApproachLinearRsss(short& a, short b, short c); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzDist, from the legacy file for func_ov032_02111b9c, NOT applied: int Vec3_HorzDist(const Vector3* a, const Vector3* b); */
/* TUBUILD CONFLICT -- alternate declaration of AngleDiff, from the legacy file for func_ov032_02111b9c, NOT applied: int AngleDiff(int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_VertAngle, from the legacy file for func_ov032_02111b9c, NOT applied: short Vec3_VertAngle(const Vector3* a, const Vector3* b); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for func_ov032_02111dd8, NOT applied: extern int RandomIntInternal(void*); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov032_02111350, from the legacy file for func_ov032_02111e24, NOT applied: extern int func_ov032_02111350(char* c); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov032_02111e24, NOT applied: extern int Vec3_HorzAngle(void* a, void* b); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for func_ov032_02111e24, NOT applied: extern void _Z14ApproachLinearRsss(short* p, int t, int s); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_VertAngle, from the legacy file for func_ov032_02111e24, NOT applied: extern int Vec3_VertAngle(void* a, void* b); */
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_FromRotationY, from the legacy file for func_ov032_02111e24, NOT applied: extern void Matrix4x3_FromRotationY(void* m, short ang); */
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_ApplyInPlaceToRotationX, from the legacy file for func_ov032_02111e24, NOT applied: extern void Matrix4x3_ApplyInPlaceToRotationX(void* m, short ang); */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for func_ov032_02111e24, NOT applied: extern void MulVec3Mat4x3(void* in, void* m, void* out); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov032_02111254, from the legacy file for func_ov032_02111e24, NOT applied: extern int func_ov032_02111254(void* c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov032_02111ff4, from the legacy file for func_ov032_02111e24, NOT applied: extern int func_ov032_02111ff4(void* c, void* p); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for func_ov032_02111e24, NOT applied: extern int RandomIntInternal(int* seed); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for func_ov032_02111f9c, NOT applied: extern int RandomIntInternal(void*); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj, from the legacy file for _ZN12daBakubaku_c13InitResourcesEv, NOT applied: void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj( */
/* TUBUILD CONFLICT -- alternate declaration of func_ov032_02111ff4, from the legacy file for _ZN12daBakubaku_c13InitResourcesEv, NOT applied: int func_ov032_02111ff4(void *self, void *stateTable); */
}

struct BakubakuProfile {
    daBakubaku_c *(*classInit)();
    s16 profileID;
    s16 groupFlags;
    u32 actorFlags;
    Fix12i cullRadiusX;
    Fix12i cullRadiusY;
    u32 executeOrder;
    u32 drawOrder;
};

typedef char BakubakuProfile_size_must_be_0x1c[
    sizeof(BakubakuProfile) == 0x1c ? 1 : -1];

/* Reconstructed source-style names. SM64DS directly preserves the class RTTI,
 * BAKUBAKU ID, descriptor relationship, and factory behavior. */
// @symbol daBakubaku_c_classInit
extern "C" daBakubaku_c *daBakubaku_c_classInit()
{
    return new daBakubaku_c();
}

extern "C" BakubakuProfile g_profile_BAKUBAKU = {
    daBakubaku_c_classInit,
    0x00e4,
    0x0052,
    3,
    0,
    0x003e8000,
    0x01000000,
    0x01000000
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- _ZN12daBakubaku_c16OnAimedAtWithEggEv, 0x02112444, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBakubaku_c16OnAimedAtWithEggEv
/* Vtable slot 29, previously func_ov032_02112444. Eight bytes: load a constant and
 * return. 0xa0000 is 10.0 in Fix12, which is the height Yoshi's egg aims at. */
int daBakubaku_c::OnAimedAtWithEgg()
{
    return 0xa0000;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN12daBakubaku_c13InitResourcesEv, 0x021122dc, size 0x168 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBakubaku_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method -- vtable slot 0
 *
 * Was a C file spelling the mangled name directly and reaching every field through
 * a raw offset. It takes one model file and two animations, builds the two body
 * cylinders from a Vector3 offset apiece, remembers where it was spawned, and hands
 * itself to the state-table installer at 0x02111ff4.
 */
s32 daBakubaku_c::InitResources()
{
    Vector3 bodyOffset;
    Vector3 headOffset;
    void *f;

    f = Model::LoadFile(*(SharedFilePtr *)data_ov032_02113a40);
    mModelAnim.SetFile((BMD_File *)f, 1, -1);
    mShadowModel.InitCylinder();
    Animation::LoadFile(*(SharedFilePtr *)&data_ov032_02113a50);
    Animation::LoadFile(*(SharedFilePtr *)&data_ov032_02113a48);

    bodyOffset.x = data_ov032_021137cc.x;
    bodyOffset.y = data_ov032_021137cc.y;
    bodyOffset.z = data_ov032_021137cc.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mBodyClsn, this, bodyOffset, 0x64000, 0x64000, 0x200004, 0x10);

    headOffset.x = data_ov032_021137d8.x;
    headOffset.y = data_ov032_021137d8.y;
    headOffset.z = data_ov032_021137d8.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mHeadClsn, this, headOffset, 0x64000, 0x8c000, 0x200004, 0);

    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x64000, 0, 0, 0);

    mTerminalVelocity = -0x1e000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, (void *)(&data_ov032_02113a50)[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;

    func_ov032_02111ff4(this, &data_ov032_02113a8c);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN12daBakubaku_c8BehaviorEv, 0x021121b4, size 0x128 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBakubaku_c8BehaviorEv
/* recovered: named members + shared header, real C++ method -- vtable slot 6
 *
 * THE POINTER-TO-MEMBER SHADOW BELOW STAYS, and it is the whole reason this file
 * looks the way it does. mState points at a state-table entry whose +8 is a
 * pointer-to-member; a PMF's representation depends on what the compiler knows
 * about the class it belongs to, so `Klass` must stay INCOMPLETE. Giving it a
 * definition -- or, worse, naming it daBakubaku_c -- makes mwccarm abort with an
 * internal compiler error rather than a diagnostic.
 *
 * What was wrong before was only the NAME: the stand-in never had to be called
 * Klass or to sit in the way of the real class. Everything else in this function
 * migrated once the header named its fields.
 */
/* Deliberately never defined. See above. */
s32 daBakubaku_c::Behavior()
{
    if (UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 3) != 0)
        return 1;

    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    DecIfAbove0_Short(&unk_42a);

    StateEntry *state = (StateEntry *)mState;
    if (state->handler != 0)
        (((Klass *)this)->*(state->handler))();

    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    UpdatePos(&mBodyClsn);
    UpdateWMClsn(mWithMeshClsn, 0);
    func_ov032_02112044((char *)this);

    if (mState != (void *)data_ov032_02113aac) {
        mModelAnim.speed = 0x1000;
    } else {
        mModelAnim.speed = 0x2000;
    }

    mModelAnim.Advance();
    func_ov032_021113fc(this);
    mBodyClsn.Clear();
    mHeadClsn.Clear();

    Player *p = ClosestPlayer();
    if (p != 0 && p->mIsVanish == 0) {
        mBodyClsn.Update();
        mHeadClsn.Update();
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN12daBakubaku_c6RenderEv, 0x02112164, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBakubaku_c6RenderEv
/* Vtable slot 9, previously func_ov032_02112164.
 *
 * Skips drawing entirely when dActor_c flag 0x40000 is set, otherwise renders the
 * animated model with a null scale.
 *
 * The file used to reach the model through a six-slot stand-in class whose last
 * member was called `Target(int)`. That shape was load-bearing, not decoration: it
 * made the call a VIRTUAL dispatch through slot 5, and slot 5 of ModelAnim is
 * Render(const Vector3 *). Naming the member is what lets the real declaration
 * replace the stand-in without changing what is emitted.
 */
s32 daBakubaku_c::Render()
{
    /* The temporary is load-bearing and must not be folded into the `if`. It makes
       the compiler materialise the test as a 0/1 value -- movne #1 / moveq #0 / cmp
       -- before branching, three instructions the ROM has and that
       `if ((mFlags & 0x40000) != 0)` does not emit. */
    int b = ((mFlags & 0x40000) != 0);
    if (b) return 1;
    mModelAnim.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN12daBakubaku_c16OnPendingDestroyEv, 0x02112160, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBakubaku_c16OnPendingDestroyEv
/* Vtable slot 12, previously func_ov032_02112160. Four bytes: a bare `bx lr`.
 * fBase_c's slot 12 returns void, so this override does nothing and returns
 * nothing -- it exists only to stop the base implementation running. */
void daBakubaku_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN12daBakubaku_c16CleanupResourcesEv, 0x02112124, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBakubaku_c16CleanupResourcesEv
/* Vtable slot 3, previously func_ov032_02112124.
 *
 * Gives back the three shared files InitResources took: the model at 0x02113a40 and
 * the two animations at 0x02113a50 and 0x02113a48 -- released in that order, which is
 * NOT the order they were loaded, and not the order of their addresses either.
 *
 * The ROM's function reads no field of the object at all, and as a method `this`
 * simply arrives in r0 unused. Bytes verified unchanged by the promotion.
 */
s32 daBakubaku_c::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov032_02113a40);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov032_02113a50);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov032_02113a48);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov032_02112044, 0x02112044, size 0xe0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov032_02112044
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* mtx, int f, int g, unsigned int h);

typedef struct { int w[12]; } M48;

void func_ov032_02112044(char* c)
{
    Vector3 v;
    Vec3_Asr(&v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(data_020a0e68, *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    *(M48*)(c + 0x368) = *(M48*)data_020a0e68;
    Matrix4x3_FromTranslation(data_020a0e68, *(int*)(c + 0x5c) >> 3, (*(int*)(c + 0x60) - 0x5a000) >> 3, *(int*)(c + 0x64) >> 3);
    *(M48*)(c + 0x3dc) = *(M48*)data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x3b4, c + 0x3dc, 0xfa000, 0x258000, 0xf);
}
}
/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov032_02111ff4, 0x02111ff4, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov032_02111ff4
/* The state setter. Its second parameter is a pointer to a pointer-to-member,
   so this needs a class of its own whose layout ends at the 0x3b0 state slot;
   the parameters are spelled void* to agree with the declaration above and with
   every caller, and cast here. SPMF/SC are local to this one function. */
struct SC; typedef int (SC::*SPMF)();
struct SC { char pad[0x3b0]; SPMF *pp; };
extern "C" int func_ov032_02111ff4(void *cv, void *pv)
{
    SC *c = (SC *)cv;
    SPMF *p = (SPMF *)pv;
    c->pp = p;
    SPMF *q = c->pp;
    if (*q == 0) return 1;
    return (c->**q)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov032_02111f9c, 0x02111f9c, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov032_02111f9c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov032_02111f9c(char* c){
  unsigned int r = RandomIntInternal(data_0209e650);
  *(short*)(c+0x430) = ((r>>8)&0xf)<<0xc;
  r = RandomIntInternal(data_0209e650);
  *(short*)(c+0x100) = ((r>>8)&0x3f)+0x32;
  *(int*)(c+0x3a8) = 0x1000;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov032_02111e24, 0x02111e24, size 0x178 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov032_02111e24
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov032_02111e24(char* c) {
    int in[3];
    int out[3];
    /* short, not int: the ROM passes Vec3_VertAngle's result straight into
       ApproachLinear (mov r1, r0). Widening it to int makes mwcc emit the
       lsl/asr sign-extension pair and the function grows by 4 bytes. */
    short ang;

    _Z14ApproachLinearRiii((int*)(c+0x98), 0x5000, 0x333);
    if (func_ov032_02111350(c) == 1) {
        *(short*)(c+0x100) = 0x28;
        *(short*)(c+0x400+0x2a) = 0x28;
        ang = Vec3_HorzAngle((const Vector3*)(c+0x5c), (const Vector3*)(c+0x40c));
        *(short*)(c+0x400+0x30) = ang;
    }
    _Z14ApproachLinearRsss((short*)(c+0x94), *(short*)(c+0x400+0x30), 0x100);
    ang = Vec3_VertAngle(c+0x5c, c+0x40c);
    _Z14ApproachLinearRsss((short*)(c+0x92), ang, 0x100);
    in[2] = 0;
    in[2] = 0x5000;
    in[0] = 0;
    in[1] = 0;
    out[0] = 0;
    out[1] = 0;
    out[2] = 0;
    Matrix4x3_FromRotationY(data_020a0e68, *(short*)(c+0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(short*)(c+0x8c));
    MulVec3Mat4x3(in, data_020a0e68, out);
    *(int*)(c+0xa8) = out[1];
    if (func_ov032_02111254(c) == 1) {
        func_ov032_02111ff4(c, data_ov032_02113aac);
        return 1;
    }
    if (*(unsigned short*)(c+0x100) == 0) {
        unsigned int r = (unsigned int)RandomIntInternal(data_0209e650);
        if (((r >> 8) & 3) == 0) {
            func_ov032_02111ff4(c, data_ov032_02113a9c);
        } else {
            func_ov032_02111ff4(c, &data_ov032_02113a8c);
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov032_02111dd8, 0x02111dd8, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov032_02111dd8
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov032_02111dd8(char* c){
  unsigned int r = RandomIntInternal(data_0209e650);
  *(short*)(c+0x100) = ((r>>8)&0x1f)+0x14;
  *(int*)(c+0x3a8) = 0x1000;
  *(int*)(c+0xa4) = 0;
  *(int*)(c+0xa8) = 0;
  *(int*)(c+0xac) = 0;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov032_02111d7c, 0x02111d7c, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov032_02111d7c
extern "C" {
extern int func_ov032_02111254(void*);
extern int func_ov032_02111ff4(void*, void*);
int func_ov032_02111d7c(void* c){
  if(func_ov032_02111254(c)==1){ func_ov032_02111ff4(c, data_ov032_02113aac); return 1; }
  if(*(unsigned short*)((char*)c+0x100)==0){ func_ov032_02111ff4(c, &data_ov032_02113a8c); }
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov032_02111d58, 0x02111d58, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov032_02111d58
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov032_02111d58(char *p)
{
    *(short *)(p + 0x100) = 300;
    *(int *)(p + 0x3a8) = 8192;
    *(int *)(p + 0x98) = 40960;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov032_02111b9c, 0x02111b9c, size 0x1bc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov032_02111b9c
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" int func_ov032_02111b9c(unsigned char* thiz)
{
    if (*(unsigned short*)(thiz + 0x100) != 0) {
        if (func_ov032_02111350((char *)thiz) == 1) goto init;
        if (func_ov032_02111254(thiz) != 0) goto track;
    }
init:
    {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(thiz + 0x34c,
            *(BCA_File**)((unsigned char*)&data_ov032_02113a50 + 4),
            0, 0x1000, 0);
        *(short*)(thiz + 0x42a) = 0x64;
        func_ov032_02111ff4(thiz, &data_ov032_02113a8c);
        return 1;
    }
track:
    {
        *(short*)(thiz + 0x430) = Vec3_HorzAngle((Vector3*)(thiz + 0x5c), (Vector3*)(thiz + 0x418));
        unsigned int r = (unsigned int)RandomIntInternal(data_0209e650);
        int s5 = (int)(((r >> 8) & 3) << 0x1c) >> 0x10;
        _Z14ApproachLinearRsss((short*)(thiz + 0x94), *(short*)(thiz + 0x430), 0x200);
        if (Vec3_HorzDist((Vector3*)(thiz + 0x5c), (Vector3*)(thiz + 0x418)) < 0x258000) {
            int d = AngleDiff(*(short*)(thiz + 0x8e), *(short*)(thiz + 0x430));
            if (d < (int)(short)(s5 + 0x200)) {
                *(short*)(thiz + 0x430) = Vec3_VertAngle((Vector3*)(thiz + 0x5c), (Vector3*)(thiz + 0x418));
                *(int*)(thiz + 0xb0) = 0;
                *(int*)(thiz + 0x3a8) = 0x1000;
                *(int*)(thiz + 0x434) = *(int*)(thiz + 0x60);
                *(unsigned short*)(thiz + 0x100) = 0;
                *(int*)(thiz + 0x98) = 0x14000;
                thiz[0x429] = 0;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(thiz + 0x34c,
                    *(BCA_File**)((unsigned char*)&data_ov032_02113a48 + 4),
                    0x40000000, 0x1000, 0);
                if (*(short*)(thiz + 0x430) > 0) {
                    func_ov032_02111ff4(thiz, data_ov032_02113a7c);
                } else {
                    func_ov032_02111ff4(thiz, data_ov032_02113abc);
                }
                return 1;
            }
        }
        return 1;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov032_02111b50, 0x02111b50, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov032_02111b50
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov032_02111b50(char *c)
{
    *(short *)(c + 0x430) = -0x4000;
    *(int *)(((int)c + 0x128)) |= 2;
    *(int *)(((int)c + 0x168)) |= 2;
    *(unsigned char *)(c + 0x428) = 0;
    *(int *)(c + 0x98) = 0xa000;
    *(int *)(c + 0x424) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov032_02111830, 0x02111830, size 0x320 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov032_02111830
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov032_02111830(char *c)
{
    s16 speed;
    speed = 0x3000;
    if (data_0209f32c[0] - 0x64000 > *(int*)(c + 0x60))
        speed = 0;
    if (*(int*)(c + 0x424) == 0) {
        if (data_0209f32c[0] > *(int*)(c + 0x60)) {
            if (*(s16*)(c + 0x400 + 0x30) > 0)
                *(int*)(c + 0x424) = 1;
        }
    }
    if (*(int*)(c + 0x424) > 0 && *(int*)(c + 0x424) < 5) {
        int *p424 = (int*)(((int)c + 0x424));
        (*p424)++;
        if (*(int*)(c + 0x424) == 4) {

            typedef struct { int x,y,z; } V3;
            V3 v[3];
            v[0].x = *(int*)(c + 0x5c);
            v[0].y = *(int*)(c + 0x60);
            v[0].z = *(int*)(c + 0x64);
            v[1].z = 0;
            v[1].x = 0; v[1].y = 0;
            v[2].x = 0; v[2].y = 0; v[2].z = 0;
            v[1].z = 0xa0000;
            Matrix4x3_FromRotationY(data_020a0e68, *(s16*)(c + 0x8e));
            MulVec3Mat4x3(&v[1], data_020a0e68, &v[2]);
            v[0].x += v[2].x;
            v[0].z += v[2].z;
            func_02022c80(0, 0x55, v[0].x, v[0].y, v[0].z, 0);
            *(int*)(c + 0x42c) = func_02022d00(*(int*)(c + 0x42c), 0x56, v[0].x, data_0209f32c[0], v[0].z, 0);
            v[0].y += 0x4b000;
            func_02022c80(0, 0x54, v[0].x, v[0].y, v[0].z, 0);

        }
    }

    if (func_ov032_02111350((char *)c) == 1)
        goto zeroblock;
    if (func_ov032_02111254(c) != 0)
        goto matrixblock;
zeroblock:
    speed = 0;
    *(int*)(c + 0x98) = 0;
    *(int*)(c + 0xa4) = 0;
    *(int*)(c + 0xa8) = 0;
    *(int*)(c + 0xac) = 0;
    goto afterblock;
matrixblock:
    {
        int in2[3];
        int out2[3];
        in2[2] = 0;
        in2[2] = 0x14000;
        in2[0] = 0;
        in2[1] = 0;
        out2[0] = 0;
        out2[1] = 0;
        out2[2] = 0;
        Matrix4x3_FromRotationY(data_020a0e68, *(s16*)(c + 0x8e));
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(s16*)(c + 0x8c));
        MulVec3Mat4x3(in2, data_020a0e68, out2);
        *(int*)(c + 0xa8) = out2[1];
    }
afterblock: ;
    _Z14ApproachLinearRsss((s16*)(c + 0x430), speed, 0x200);
    _Z14ApproachLinearRsss((s16*)(c + 0x92), *(s16*)(c + 0x400 + 0x30), 0x200);
    if (_ZN9Animation8FinishedEv(c + 0x39c) != 0) {
        *(unsigned char*)(c + 0x428) = 0;
        if (speed == 0) {
            s16 a = *(s16*)(c + 0x92);
            if (a < 0) a = -a;
            if (a < 0x100) {
                *(s16*)(c + 0x400 + 0x2a) = 0x64;
                *(s16*)(c + 0x400 + 0x30) = *(s16*)(c + 0x8e);
                *(int*)(c + 0xb0) = 3;
                *(int*)(((int)c + 0x128)) &= ~2;
                *(int*)(((int)c + 0x168)) &= ~2;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x34c, (void*)(&data_ov032_02113a50)[1], 0, 0x1000, 0);
                func_ov032_02111ff4(c, &data_ov032_02113a8c);
                return 1;
            }
        }
    } else {
        unsigned int t = (unsigned int)(*(int*)(c + 0x3a4) << 4) >> 0x10;
        if (t > 0x14 && t < 0x3c)
            *(unsigned char*)(c + 0x428) = 1;
        else
            *(unsigned char*)(c + 0x428) = 0;
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov032_02111814, 0x02111814, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov032_02111814
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov032_02111814(char *p)
{
    *(int *)(p + 0x424) = 0;
    *(char *)(p + 0x428) = 0;
    *(int *)(p + 0x98) = 40960;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov032_02111620, 0x02111620, size 0x1f4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov032_02111620
extern "C" int func_ov032_02111620(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned char *r5;

    r5 = (unsigned char *)_ZN8dActor_c13ClosestPlayerEv((char *)c);
    if (r5 == 0)
        return 1;

    if (func_ov032_02111350((char *)c) == 1)
        goto zeroblock;
    if (func_ov032_02111254(c) != 0)
        goto matrixblock;
zeroblock:
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0xa4) = 0;
    *(int *)(c + 0xa8) = 0;
    *(int *)(c + 0xac) = 0;
    goto afterblock;
matrixblock:
    {
        int in[3];
        int out[3];
        in[2] = 0;
        in[2] = 0x14000;
        in[0] = 0;
        in[1] = 0;
        out[0] = 0;
        out[1] = 0;
        out[2] = 0;
        Matrix4x3_FromRotationY(data_020a0e68, *(short *)(c + 0x8e));
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(short *)(c + 0x8c));
        MulVec3Mat4x3(in, data_020a0e68, out);
        *(int *)(c + 0xa8) = out[1];
    }
afterblock: ;

    if (*(int *)(c + 0x424) == 0) {
        if (_ZN9Animation8FinishedEv(c + 0x39c) == 0)
            goto player_path;
    }

    _Z14ApproachLinearRsss((short *)(c + 0x92), 0, 0x200);
    *(int *)(c + 0x424) = 1;
    *(unsigned char *)(c + 0x428) = 0;
    if (AngleDiff(*(short *)(c + 0x92), 0) < 0x200) {
        *(short *)(c + 0x92) = 0;
        *(short *)(c + 0x400 + 0x2a) = 0x64;
        *(short *)(c + 0x400 + 0x30) = *(short *)(c + 0x8e);
        *(int *)(c + 0xb0) = 3;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x34c, (void*)(&data_ov032_02113a50)[1], 0, 0x1000, 0);
        func_ov032_02111ff4(c, &data_ov032_02113a8c);
    }
    goto end;

player_path:
    {
        int vec[3];
        unsigned int n;
        int *p = (int *)(((int)r5 + 0x5c));
        vec[0] = p[0];
        vec[1] = p[1];
        vec[2] = p[2];
        if (data_0209f32c[0] > *(int *)(c + 0x60)) {
            _Z14ApproachLinearRsss((short *)(c + 0x92), 0, 0x200);
        } else {
            _Z14ApproachLinearRsss((short *)(c + 0x92), Vec3_VertAngle(c + 0x5c, vec), 0x200);
        }
        n = (unsigned int)(*(int *)(c + 0x3a4) << 4) >> 0x10;
        if (n > 0x14 && n < 0x3c)
            *(unsigned char *)(c + 0x428) = 1;
        else
            *(unsigned char *)(c + 0x428) = 0;
    }
end:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov032_021113fc, 0x021113fc, size 0x224 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov032_021113fc
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" void func_ov032_021113fc(void* self)
{
    u8* c = (u8*)self;

    Vector3 v1;
    v1.x = data_ov032_021137cc.x;
    v1.y = data_ov032_021137cc.y;
    v1.z = data_ov032_021137cc.z;
    _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3((void*)(c+0x110), &v1);

    u32 id1 = *(u32*)(c+0x134);
    if (id1 != 0) {
        u8* f = (u8*)_ZN8dActor_c10FindWithIDEj(id1);
        int isbf = (int)(*(u16*)(f+0xc) == 0xbf);
        if (isbf) {
            if (*(u8*)(f+0x6fb) != 0) return;
            if (*(s32*)(c+0x130) & 0x10) {
                _ZN12dEnemyBase_c22SpawnMegaCharParticlesER8dActor_cPc(self, f, 0);
                _ZN8dActor_c8PoofDustEv(self);
                _ZN6Player16IncMegaKillCountEv(f);
                func_02012694(0x1e, (void*)(c+0x74));
                _ZN8dActor_c24KillAndTrackInDeathTableEv(self);
                return;
            }
            if (*(u8*)(c+0x428) != 0) {
                if (func_ov002_020c5cd8(f, self) == 1) {
                    func_02012694(0xf6, (void*)(c+0x74));
                    return;
                }
            }
        }
    }

    Vector3 v2;
    v2.x = data_ov032_021137d8.x;
    v2.y = data_ov032_021137d8.y;
    v2.z = data_ov032_021137d8.z;
    _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3((void*)(c+0x150), &v2);

    u32 id2 = *(u32*)(c+0x174);
    if (id2 == 0) return;
    u8* f2 = (u8*)_ZN8dActor_c10FindWithIDEj(id2);
    int isbf2 = (int)(*(u16*)(f2+0xc) == 0xbf);
    if (isbf2 == 0) return;

    if (*(s32*)(c+0x170) & 0x10) {
        _ZN12dEnemyBase_c22SpawnMegaCharParticlesER8dActor_cPc(self, f2, 0);
        _ZN8dActor_c8PoofDustEv(self);
        _ZN6Player16IncMegaKillCountEv(f2);
        _ZN8dActor_c24KillAndTrackInDeathTableEv(self);
        return;
    }

    int* s3b0 = *(int**)(c+0x3b0);
    if (s3b0 == data_ov032_02113abc) return;
    if (s3b0 == data_ov032_02113a7c) return;

    Vector3 hv;
    hv.x = *(s32*)(c+0x5c);
    hv.y = *(s32*)(c+0x60);
    hv.z = *(s32*)(c+0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(f2, &hv, 2, 0xc000, 1, 0, 1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov032_02111350, 0x02111350, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol func_ov032_02111350
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov032_02111350(char* c) {
  if (_ZN8dActor_c13ClosestPlayerEv(c) == 0) return 1;
  if (_ZNK10dBgCh_Actr8IsOnWallEv(c+0x190) != 0 || _ZNK10dBgCh_Actr10IsOnGroundEv(c+0x190) != 0) return 1;
  if (Vec3_HorzDist((struct Vector3*)(c+0x40c), (struct Vector3*)(c+0x5c)) > 0x4b0000) return 1;
  if (*(char**)(c+0x3b0) != (char*)data_ov032_02113abc) {
    if (data_0209f32c[0] < *(int*)(c+0x60)) return 1;
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov032_02111254, 0x02111254, size 0xfc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov032_02111254
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov032_02111254(void *cv) {
    char *c = (char *)cv;
    char *pl = _ZN8dActor_c13ClosestPlayerEv(c);
    int *s;
    int *t;
    int d;
    if (pl == 0 || *(unsigned short*)(c+0x400+0x2a) != 0)
        return 0;
    s = (int*)(((int)pl + 0x5c));
    *(int*)(c+0x418) = s[0];
    *(int*)(c+0x41c) = s[1];
    *(int*)(c+0x420) = s[2];
    t = *(int**)(c+0x3b0);
    if (t != data_ov032_02113abc && t != data_ov032_02113a7c) {
        if (*(unsigned char*)(pl+0x706) == 0)
            return 0;
    }
    d = *(int*)(pl+0x644) - data_0209f32c[0];
    if (d < 0) d = -d;
    if (d < 0xb4000)
        return 0;
    if (t != data_ov032_02113abc && t != data_ov032_02113a7c) {
        if (Vec3_HorzDist(c+0x40c, c+0x418) > 0x4b0000)
            return 0;
    }
    return 1;
}
}
