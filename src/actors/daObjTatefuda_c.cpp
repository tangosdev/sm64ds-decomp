//cpp
/* daObjTatefuda_c: the wooden signpost (TATEFUDA profile, ov002).
 *
 * The whole unit, ov002 .text 0x020badd0..0x020bc414, 29 functions: the
 * destructor pair, eight other methods (Kill, OnHitByMegaChar,
 * OnGroundPounded, OnAttacked1, Behavior, Render, CleanupResources,
 * InitResources), eighteen helpers, and last the registry factory
 * daObjTatefuda_c_classInit. Talking to it runs the
 * 0x3dc-byte talk routine (approach, face the reading spot, show the
 * sign's message). Pounding sinks it into the ground (it comes back
 * later); a bob-omb hit or a mega character kills it (Kill resets and
 * hides it); a hit flagged 0x40000 starts a 60-frame break countdown.
 *
 * The tree used to call this class SignPost (coined): the cartridge's
 * _ZTS15daObjTatefuda_c at ov002 0x02109ac0, _ZTI at 0x02109ab4 and _ZTV
 * at 0x02109af8 name it daObjTatefuda_c, and daObjTatefuda_c_classInit
 * builds it for the TATEFUDA registry profile.
 *
 * Kill (slot 31, overriding dBgActor_c) is the key function: its real
 * body emits the vtable, the RTTI and the destructor variants, so D1/D0
 * carry no bodies here. Functions are written in descending ordinal order
 * because the compiler emits .text in reverse source order.
 *
 * State machine: the sign runs a five-state machine on mState, driven by
 * the members SetState (switch state and run its enter routine) and
 * UpdateState (run the current state's update routine) through the
 * table data_ov002_0210e084. The rows are
 *   IDLE    enter InitIdle   update Idle (-> TryStartTalk, CheckGrabOrBreak)
 *   TALK    enter InitTalk   update Talk
 *   CARRIED enter InitCarried update Carried
 *   THROWN  enter InitThrown  update Thrown
 *   DROPPED enter InitDropped update Dropped
 * The state names are descriptive, taken from what each routine does; the
 * ROM carries no names for them.
 *
 * deslop leftovers:
 * - RebuildModelMatrix and AttachToHolder stay free functions parsed as C:
 *   their Matrix4x3 block copies scalarize under C++ (same wall as Bullet
 *   020fed7c and ov062 ba84). `#pragma cplusplus off/on` wraps their
 *   definitions only.
 * - Behavior's `enum Bool` casts are a measured codegen view: plain `int`
 *   spellings size-DIFF under 2004/b56.
 * - Still raw: the holder's word at +0xc8 (Behavior, Render; inside
 *   dActor_c's unnamed 0xc5..0xcb padding), and the offsets into the holder's
 *   body Model (the func_ov002_020e496c result) in AttachToHolder: +0x14
 *   (ModelBase.h's data.transforms), +0x1c (mat4x3), +0x58 (the animation's
 *   current frame, per Player.h) and +0x2a0 (the matrix of bone 14).
 * - Unrecovered meanings: mFlags bits 0x100, 0x400, 0x2000, 0x4000 and
 *   0x4000000 as this class uses them, hit-flag bit 0x8000000 and bits
 *   0x2000/0x4000000 of mdCcAc_c.flags, the global words data_0209b454,
 *   data_0209d660, data_0209d6bc and data_0209f284, the Player::Hurt
 *   arguments after the first two, and what the player-side calls
 *   func_ov002_020bec84 and func_ov002_020bec9c test and start.
 * - dCcAc_c::Init, dBgCh_Actr::Init, DropShadow, Particle::New and the
 *   Player/Sound helpers stay mangled scalar externs (Fix12<int>-by-value
 *   member form is the 6az wall).
 * - SignPost_ClsnFile / SignPost_ModelFile keep their coined BSS names
 *   (historical, like the Spawn aliases).
 * - The message/volume tables and g_profile_TATEFUDA are not this TU's data.
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daObjTatefuda_c.h"
#include "common.h"
#include "dActor_c.h"
#include "Sound.h"
#include "dBgActor_c.h"
#include "Player.h"
#include "types.h"
#include "decl_common.h"
#include "dBgW.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* shadow struct 'Vec3' */
struct Vec3 { int x, y, z; };

/* POD spelling of Matrix4x3's 0x30 bytes: with a user-declared constructor on
 * Vector3 the Matrix4x3 copies below scalarize even under `#pragma cplusplus
 * off`; the ROM copies are block moves, so they go through this. */
struct RawMatrix4x3 { int m[12]; };

/* Bare Vec3 is used for the locals and casts below; the struct form above
 * feeds the elaborated-type uses. */
typedef struct Vec3 Vec3;

/* shadow typedef 's32' */
typedef int s32;

/* shadow struct 'Sub041' */
struct Sub041 {
  virtual void v0(); virtual void v1(); virtual void v2();
  virtual void v3(); virtual void v4(); virtual void v5(int);
};

/* shadow enum 'Bool' */
enum Bool { FALSE, TRUE };

/* shadow struct 'Vector3_16f' */
struct Vector3_16f;

/* shadow struct 'BMD_File' */
struct BMD_File; struct KCL_File; struct dActor_c; struct Vector3; struct Matrix4x3;

/* shadow struct 'CLPS_Block' */
struct CLPS_Block; struct Vector3_16;

/* The retail static initializer constructs these two 8-byte resource handles
 * in source order (model 0x491, collision 0x492) and lets the C++ runtime
 * register their destructors. The family spellings are reconstructed; the
 * constructor/destructor addresses, file IDs, object widths, BSS order, and
 * registration topology are direct ROM evidence. The intact-TU manifest maps
 * their compiler-generated undefined member imports onto the existing
 * evidence-bounded ROM symbols. */
struct SignPostModelFilePtr : SharedFilePtr {
    u32 words[2];

    SignPostModelFilePtr(u32 fileID);
    ~SignPostModelFilePtr();
};

struct SignPostCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    SignPostCollisionFilePtr(u32 fileID);
    ~SignPostCollisionFilePtr();
};

/* BMD (Model::LoadFile) and KCL (dBgW_Kc::LoadFile). Cleanup releases both. */
extern "C" SignPostModelFilePtr SignPost_ModelFile;
extern "C" SignPostCollisionFilePtr SignPost_ClsnFile;

/* Actor IDs, as symbols/actor_debug_names.tsv lists them. */
enum {
    ACTOR_PLAYER  = 191,   /* 0xbf */
    ACTOR_BOMBHEI = 206    /* 0xce */
};

#define FIXMUL(a, b) ((s32)(((s64)(a) * (b) + 0x800) >> 12))

extern "C" {
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *c);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *c);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *thiz, void *actor, int b, int d, unsigned int e, unsigned int f);
extern void _ZN5dCc_c5ClearEv(void *c);
extern void Matrix4x3_FromRotationY(void *, int);
extern void Vec3_Asr(struct Vector3* d, struct Vector3* s, int sh);
extern void Matrix4x3_FromTranslation(struct Matrix4x3* m, Fix12i x, Fix12i y, Fix12i z);
extern void Matrix4x3_ApplyInPlaceToTranslation(struct Matrix4x3* m, Fix12i x, Fix12i y, Fix12i z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(struct Matrix4x3* m, int x, int y, int z);
extern struct Matrix4x3 data_020a0e68;
extern char *func_ov002_020e496c(void *p);
extern int _ZN6Player14IsFrontSlidingEv(void *p);
extern int _ZN6Player17LostGrabbedObjectEv(void *p);
extern int _Z14ApproachLinearRsss(short *value, short target, short step);
extern void MulMat4x3Mat4x3(void *a, void *b, void *c);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern void Vec3_Lsl(struct Vec3 *d, struct Vec3 *s, int sh);
extern short data_ov002_020ff0d0[];
extern int data_ov002_020ff0d4[];
extern int data_ov002_020ff0d8[];
extern int data_ov002_020ff0dc[];
extern "C" void func_02012694(int a, void *b);
extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_( u32 id, Fix12i x, Fix12i y, Fix12i z);

extern void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern s16 Vec3_HorzAngle(const struct Vector3* v0, const struct Vector3* v1);
extern int _ZN6Player7TryGrabER8dActor_c(char* p, char* a);
extern int _ZN6Player9StartTalkER7fBase_cb(char* p, char* a, int b);
extern s16 data_02082214[];
extern u8 data_0209d660;
extern u8 data_0209d6bc;
extern u8 data_0209f284;
extern int _ZN6Player12GetTalkStateEv(void *player);
extern s32 Vec3_HorzDist(struct Vector3 *a, struct Vector3 *b);
extern int func_ov002_020bec84(void *player, unsigned int i);
extern int func_ov002_020bec9c(void *player, unsigned int a, int b, int d, unsigned short e);
extern int _ZN6Player12FinishedAnimEv(void *player);
extern void _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh( void *player, void *actor, unsigned int msg, struct Vector3 *pos, unsigned int a, unsigned int b);
extern void func_02012790(int id);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void* self, void* c);
extern void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* p);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void* p);
extern int _ZNK10dBgCh_Actr12TouchesWaterEv(void* p);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* thiz, void* v, unsigned a, int b, unsigned c, unsigned d, unsigned e);
extern void _Z14ApproachLinearRiii(int* p, int a, int b);
extern int _ZN8dActor_c13DistToCPlayerEv(void* self);
extern "C" unsigned int data_0209b454;
extern "C" void _ZN6Player9DropActorEv(void *self);
extern "C" void AttachToHolder(struct daObjTatefuda_c *self);
extern "C" u8 DecIfAbove0_Byte(u8 *p);
extern "C" int _ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(void *self, short a, short b, short c, int fix);
extern "C" void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, int c, int d, int e, const void *v, void *cb);
extern "C" u32 _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(u32 a, u32 b, int c, int d, int e, const Vector3_16f *v);
extern "C" void _ZN8dActor_c19DisappearPoofDustAtERK7Vector3(void *self, const struct Vector3 *vec);
extern "C" void _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(void *self, void *sm, void *m, int a, int b, int c, u32 j);
extern "C" void _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern "C" void _ZN5dCc_c6UpdateEv(void *self);
extern "C" void RebuildModelMatrix(struct daObjTatefuda_c *self);
extern "C" void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
extern "C" void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *);
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block( void *self, KCL_File *f, const Matrix4x3 &m, s32 fix, s16 sh, CLPS_Block &b);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_( void *self, dActor_c *a, s32 radius, s32 height, Vector3_16 *v, Vector3_16 *v2);
extern "C" CLPS_Block data_ov002_0210d714;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- daObjTatefuda_c_classInit, 0x020bc3c8, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol daObjTatefuda_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
/* Reconstructed source-style name: SM64DS proves daObjTatefuda_c through RTTI,
 * allocation size, vtable identity, and the TATEFUDA registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: SignPost_Spawn.
 *
 * `new daObjTatefuda_c` is the whole sequence the loose factory spelled by
 * hand: fBase_c::operator new(0x5a4), dBgActor_c's base constructor, the
 * vptr store, then the dCcAc_c, dExtShadowModel_c and dBgCh_Actr member
 * constructors in declaration order. */
extern "C" daObjTatefuda_c *daObjTatefuda_c_classInit(void)
{
    return new daObjTatefuda_c;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 --_ZN15daObjTatefuda_c13InitResourcesEv, 0x020bc240, size 0x188 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* Model::LoadFile and dBgW_Kc::LoadFile by their real ROM symbols, carried
   forward from #1554. decl_common.h's ModelLoadFile / MeshColliderLoadFile are
   phantoms -- names no module defines -- and match.py compares relocated words as
   wildcards, so the byte gate never saw it. */
/* THREE OF THE SHADOWS ARE GONE, because daObjTatefuda_c.h now says `daObjTatefuda_c :
   dBgActor_c` and dBgActor_c.h brings in the real dBgActor_c, Model/ModelBase and
   dBgW_KcMbg. Each one is replaced by the thing it was standing in for:

     ModelBase          -> mModel.SetFile, whose real declaration in
                           include/ModelBase.h mangles identically
                           (_ZN9ModelBase7SetFileEP8BMD_Fileii) -- only the
                           return type differed, and that is not mangled.
     dBgActor_c           -> the two calls are unqualified members now.
     dBgW_KcMbg -> its real SetFile takes Fix12<int> BY VALUE, so it
                           cannot be declared as a callable method here without
                           changing how the caller passes the argument
                           (notes/mwccarm-codegen.md 6az). It keeps a scalar
                           extern "C" declaration under its exact ROM symbol,
                           which also FIXES A PHANTOM: the shadow spelled `int`
                           and so emitted a `bl` to
                           ..._Matrix4x3isR10CLPS_Block, which exists nowhere.
                           match.py compares relocated words as wildcards, so
                           nothing caught it.

   The remaining ABI-only calls are dCcAc_c::Init and dBgCh_Actr::Init; they
   have the same Fix12<int> problem with no collision forcing the issue yet. */
/* dBgCh_Actr is the real class now, through this actor's header, and it
   declares StartDetectingWater itself. */
int daObjTatefuda_c::InitResources()
{
    void *mf = _ZN5Model8LoadFileER13SharedFilePtr(&SignPost_ModelFile);
    mModel.SetFile((BMD_File*)mf, 1, -1);
    mShadowModel.InitCuboid();

    int py = mPosY;
    int pz = mPosZ;
    int px = mPosX;
    int py2 = py + 0x64000;
    Vec3 v = { px, py2, pz };
    dBgCh_Gnd rg;
    rg.SetObjAndPos(*(Vector3*)&v, (dActor_c*)0);
    if (rg.DetectClsn() != 0)
        mPosY = rg.clsnY;

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    UpdateShadowMatrix();

    void *kf = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&SignPost_ClsnFile);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File*)kf, mClsnMat, 0x199, mAngleY, data_ov002_0210d714);

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mdCcAc_c, (dActor_c*)((char *)this), 0x64000, 0x64000, 0x4800002, 0x41000);

    mPoundsLeft = 2;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mHomeAngleX = mAngleX;
    mHomeAngleY = mAngleY;
    mHomeAngleZ = mAngleZ;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, (dActor_c*)((char *)this), 0x28000, 0x28000, 0, 0);
    mWithMeshClsn.StartDetectingWater();

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- _ZN15daObjTatefuda_c8BehaviorEv, 0x020bbea4, size 0x39c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c8BehaviorEv
/* daObjTatefuda_c::Behavior -- vtable slot 6. Real C++ method over the shared header.
 *
 * This was an extern "C" free function over a raw `char *c`, with every field
 * reached by literal offset and a local one-word Vector3. Naming the fields is
 * what proved four of them exist at all: 0x354 (mState), 0x380
 * (mShadowMat) and 0x584/0x588/0x58c (the two particle handles and the break
 * countdown) were all inside explicit `pad_` runs in include/daObjTatefuda_c.h until
 * this body was read. Byte-exact under 2004/b56 after the conversion.
 *
 * What it does, in order:
 *  1. If this sign's mFlags bit 0x4000000 and the same bit of the global word
 *     data_0209b454 are both set while a player holds it, tell that player to
 *     let go (Player::DropActor).
 *  2. If there is a holder, mFlags bit 0x4000 is set and the holder's word at
 *     +0xc8 is non-zero: put the sign on the holder (AttachToHolder), clip radius
 *     0x20000 (32 units), and set mFlags bit 0x4000000. Otherwise clip radius
 *     0x10000 (16 units) and clear that bit.
 *  3. A hidden sign, or one that is fully pounded in (mPoundsLeft == 0), counts
 *     mRespawnDelay down while it is off screen (mFlags bit 8). Once the delay
 *     reads 0 and the player is more than 0x7d0000 (2000 units) away, a hidden
 *     sign unhides; a pounded-in one is stood back up at its home height with
 *     two pounds again. A hidden sign then returns from Behavior.
 *  4. dBgActor_c::UpdateKillByMegaChar; if it returns non-zero, return.
 *  5. While the break countdown (mBreakTimer) runs: switch the mesh collider
 *     off, trail two particles at 0x50000 (80 units) above the sign, and refresh
 *     the drop shadow; on the frame it reaches 0, poof at 0x28000 (40 units) up and
 *     run the reset (Reset).
 *  6. Otherwise: tick the pound cooldown, call dBgActor_c::IsClsnInRange(0, 0)
 *     unless hidden, run the current state's update, clear and relink the collider, then if the
 *     state is THROWN rebuild the model matrix (RebuildModelMatrix), else if
 *     the sign still has both pounds and the state is IDLE or TALK, refresh
 *     the drop shadow.
 *
 * Sites that take &mFlags do so as u32*: the flag word's declared type
 * changes the store shape. */
int daObjTatefuda_c::Behavior()
{
    struct Vector3 v;
    struct Vector3 vec, vec2;

    {
        enum Bool b = (enum Bool)((mFlags & 0x4000000) != 0);
        /* Bit 0x4000000 of mFlags and of the global word; neither is named
           yet. */
        if (b != FALSE && (data_0209b454 & 0x4000000) && mHoldingPlayer != 0)
            _ZN6Player9DropActorEv(mHoldingPlayer);
    }

    {
        void *p = mHoldingPlayer;
        if (p != 0 && (enum Bool)((mFlags & 0x4000) != 0) != FALSE
            && *(int *)((char *)p + 0xc8) != 0) {
            u32 *fp;
            AttachToHolder(this);
            mClipRadius = 0x20000;
            fp = (u32 *)&mFlags;
            *fp = *fp | 0x4000000;
        } else {
            u32 *fp;
            mClipRadius = 0x10000;
            fp = (u32 *)&mFlags;
            *fp = *fp & ~0x4000000;
        }
    }

    if (mHidden != 0) {
        /* mFlags bit 8 is dActor_c's "off screen" bit. */
        enum Bool b = (enum Bool)((mFlags & 8) != 0);
        if (b != FALSE && DecIfAbove0_Byte(&mRespawnDelay) == 0
            && _ZN8dActor_c13DistToCPlayerEv(this) > 0x7d0000)
            mHidden = 0;
        return 1;
    }

    if (mPoundsLeft == 0) {
        enum Bool b = (enum Bool)((mFlags & 8) != 0);
        if (b != FALSE && DecIfAbove0_Byte(&mRespawnDelay) == 0
            && _ZN8dActor_c13DistToCPlayerEv(this) > 0x7d0000) {
            mPoundsLeft = 2;
            mPosY = mHomePosY;
            _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
            _ZN10dBgActor_c19UpdateClsnPosAndRotEv(this);
            UpdateShadowMatrix();
        }
    }

    if (_ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(this, -0x2000, 0, 0, 0x46000))
        return 1;

    if (mBreakTimer != 0) {
        int x, y, z;
        if (_ZN4dBgW9IsEnabledEv(&mMeshCollider))
            _ZN4dBgW7DisableEv(&mMeshCollider);
        x = mPosX;
        z = mPosZ;
        y = mPosY + 0x50000;
        ((int *)&v)[0] = x;
        ((int *)&v)[1] = y;
        ((int *)&v)[2] = z;
        if (DecIfAbove0_Byte(&mBreakTimer) != 0) {
            *(void **)&mParticleHandle1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mParticleHandle1, 0x13a, v.x, v.y, v.z, 0, 0);
            mParticleHandle2 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                mParticleHandle2, 0x13b, v.x, v.y, v.z, 0);
        } else {
            int x2, y2, z2;
            x2 = mPosX;
            z2 = mPosZ;
            y2 = mPosY + 0x28000;
            ((int *)&vec)[0] = x2;
            ((int *)&vec)[1] = y2;
            ((int *)&vec)[2] = z2;
            vec2 = vec;
            _ZN8dActor_c19DisappearPoofDustAtERK7Vector3(this, &vec2);
            Reset();
            return 1;
        }
        _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
            this, &mShadowModel, &mShadowMat, 0x50000, 0x28000, 0x28000, 0xf);
        return 1;
    }

    DecIfAbove0_Byte(&mPoundCooldown);
    if (mHidden == 0)
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0);
    UpdateState();
    _ZN5dCc_c5ClearEv(&mdCcAc_c);
    _ZN5dCc_c6UpdateEv(&mdCcAc_c);
    {
        int s = mState;
        if (s == STATE_THROWN) {
            RebuildModelMatrix(this);
        } else if (mPoundsLeft == 2 && (u32)s <= 1) {  /* STATE_IDLE or STATE_TALK */
            _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
                this, &mShadowModel, &mShadowMat, 0x50000, 0x28000, 0x28000, 0xf);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- _ZN15daObjTatefuda_c6RenderEv, 0x020bbe30, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* Draws the sign: nothing while hidden. If a player holds it (same test as
   Behavior's step 2) the sign is put on the holder first, so the model is drawn
   where the holder has it this frame. The draw itself is a call through slot 5
   of the Model (via the local Sub041 view). */
int daObjTatefuda_c::Render()
{
  if (mHidden != 0) return 1;
  void* r = mHoldingPlayer;
  if (r != 0) {
    int b = (mFlags & 0x4000) != 0;
    if (b && *(int*)((char*)r+0xc8) != 0) {
      AttachToHolder(this);
    }
  }
  Sub041* s = (Sub041*)&mModel;
  s->v5(0);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- _ZN15daObjTatefuda_c16CleanupResourcesEv, 0x020bbdec, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daObjTatefuda_c::CleanupResources()
{
    if (((dBgW *)&mMeshCollider)->IsEnabled()) {
        ((dBgW *)&mMeshCollider)->Disable();
    }
    SignPost_ModelFile.Release();
    SignPost_ClsnFile.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- _ZN15daObjTatefuda_c11UpdateStateEv, 0x020bbda4, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c11UpdateStateEv
/* Runs the current state's UPDATE routine: row mState of the table, member 1.
   The rows are copied into data_ov002_0210e084 by this overlay's static
   initializer. */
typedef void (daObjTatefuda_c::*PMF)();
struct Entry { PMF pmf[2]; };
extern Entry data_ov002_0210e084[];
void daObjTatefuda_c::UpdateState() { int j = mState; (this->*data_ov002_0210e084[j].pmf[1])(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- _ZN15daObjTatefuda_c8SetStateEi, 0x020bbd5c, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c8SetStateEi
/* Switches to state `state` and runs that state's ENTER routine (row state,
   member 0). */
void daObjTatefuda_c::SetState(int state) { mState = state; int j = mState; (this->*data_ov002_0210e084[j].pmf[0])(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN15daObjTatefuda_c11InitCarriedEv, 0x020bbd50, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c11InitCarriedEv
/* CARRIED, enter: stop the sign's horizontal speed. */
void daObjTatefuda_c::InitCarried()
{
    mHorzSpeed = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN15daObjTatefuda_c7CarriedEv, 0x020bbcb8, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c7CarriedEv
// Carried at 0x020bbcb8
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
/* CARRIED, update. Leaves CARRIED according to three bits of mFlags (bits
 * this file only reads here; their meanings are not recovered): 0x400 set ->
 * THROWN; else 0x2000 set -> DROPPED; else 0x100 clear -> DROPPED. So the
 * sign stays carried only while 0x100 is set and 0x400 and 0x2000 are clear.
 * Either way the mesh collider is switched off while it is enabled. */
void daObjTatefuda_c::Carried()
{
    int flags = mFlags;
    bool t;

    t = flags & 0x400;
    if (t != false) {
        SetState(STATE_THROWN);
    } else {
        t = flags & 0x2000;
        if (t != false) {
            SetState(STATE_DROPPED);
        } else {
            t = flags & 0x100;
            if (t == false) {
                SetState(STATE_DROPPED);
            }
        }
    }

    if (((dBgW *)&mMeshCollider)->IsEnabled()) {
        ((dBgW *)&mMeshCollider)->Disable();
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN15daObjTatefuda_c10InitThrownEv, 0x020bbc78, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c10InitThrownEv
/* THROWN, enter: launch with horizontal speed 0x50000 (80 units/frame) and
 * vertical speed 0xa000 (10 units/frame), remember the holder as the last
 * holder and let go of it, and set bit 0x2000 and clear bit 0x4000000 of the
 * collider's own flags word (mdCcAc_c.flags, dCc_c offset 0x18). */
void daObjTatefuda_c::InitThrown()
{
    mHorzSpeed = 0x50000;
    mVertSpeed = 0xa000;
    mLastHolder = mHoldingPlayer;
    mHoldingPlayer = 0;
    {
        u32 *p = &mdCcAc_c.flags;
        *p |= 0x2000;
        *p &= ~0x4000000;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN15daObjTatefuda_c6ThrownEv, 0x020bbb14, size 0x164 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c6ThrownEv
/* recovered: shared common types */
/* THROWN, update. Each frame: tumble the sign about X by 0x2000 (one
 * eighth of a turn); move it (dActor_c::UpdatePos); if it touched the ground, a
 * wall or water, Kill it and stop. Otherwise, if the collider reports an
 * actor that is a player and is not the last holder, hurt that player
 * (Player::Hurt with 1, 0xc000 -- 12.0 as Fix12 -- and 1/0/1 for the rest, which
 * are not named here); slow the horizontal speed toward 0 by 0x555 (about
 * 0.33 unit) per frame; keep the mesh collider off; and once the sign is off
 * screen (mFlags bit 8) and more than 0x7d0000 (2000 units) from the player,
 * run the reset (Reset). */
void daObjTatefuda_c::Thrown()
{
    int b;
    struct Vector3 vec;
    void* found;
    unsigned id;

    {
        s16* pa = &mAngleX;
        *pa = *pa + 0x2000;
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(this, 0);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(&mWithMeshClsn) != 0 ||
        _ZNK10dBgCh_Actr8IsOnWallEv(&mWithMeshClsn) != 0 ||
        _ZNK10dBgCh_Actr12TouchesWaterEv(&mWithMeshClsn) != 0) {
        Kill();
        return;
    }

    /* otherOwner: the uniqueID of the actor whose collider this one touched. */
    id = mdCcAc_c.otherOwner;
    if (id != 0) {
        found = _ZN8dActor_c10FindWithIDEj(id);
        if (found != 0) {
            if (found != mLastHolder) {
                b = ((dActor_c *)found)->actorID;
                b = b == ACTOR_PLAYER;
                if (b) {
                    vec.x = mPosX;
                    vec.y = mPosY;
                    vec.z = mPosZ;
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(found, &vec, 1, 0xc000, 1, 0, 1);
                }
            }
        }
    }

    _Z14ApproachLinearRiii(&mHorzSpeed, 0, 0x555);

    if (((dBgW *)&mMeshCollider)->IsEnabled() != 0) {
        ((dBgW *)&mMeshCollider)->Disable();
    }

    b = mFlags & 8;
    b = b != 0;
    if (b) {
        if (_ZN8dActor_c13DistToCPlayerEv(this) > 0x7d0000) {
            Reset();
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN15daObjTatefuda_c11InitDroppedEv, 0x020bbac8, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c11InitDroppedEv
/* DROPPED, enter: zero the horizontal and vertical speeds, move the sign to
 * the holder's position raised by 0x64000 (100 units), and hand the holder
 * over to mLastHolder (mHoldingPlayer becomes 0). No callees. */
void daObjTatefuda_c::InitDropped()
{
    Player *other;
    int* py;
    Vec3* src;
    mHorzSpeed = 0;
    mVertSpeed = 0;
    other = mHoldingPlayer;
    py = &mPosY;
    src = (Vec3*)&other->mPosX;
    mPosX = src->x;
    mPosY = src->y;
    mPosZ = src->z;
    *py += 0x64000;
    mLastHolder = mHoldingPlayer;
    mHoldingPlayer = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- _ZN15daObjTatefuda_c7DroppedEv, 0x020bba28, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c7DroppedEv
bool ApproachLinear(short &value, short target, short step);
extern "C" void _ZN8dActor_c9UpdatePosEP5dCc_c(void* self, void* c);
extern "C" void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void* p);
extern "C" int _ZNK10dBgCh_Actr8IsOnWallEv(void* p);
extern "C" int _ZNK10dBgCh_Actr12TouchesWaterEv(void* p);

/* DROPPED, update: turn the X angle toward 0x4000 (a quarter turn) by at most
 * 0x1000 per frame, move, and Kill the sign if it touched the ground, a wall or
 * water. Otherwise rebuild the model matrix (RebuildModelMatrix) and keep the
 * mesh collider off. */
void daObjTatefuda_c::Dropped(){
    ApproachLinear(mAngleX, 0x4000, 0x1000);
    _ZN8dActor_c9UpdatePosEP5dCc_c(this, 0);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(&mWithMeshClsn)
        || _ZNK10dBgCh_Actr8IsOnWallEv(&mWithMeshClsn)
        || _ZNK10dBgCh_Actr12TouchesWaterEv(&mWithMeshClsn)) {
        Kill();
    } else {
        RebuildModelMatrix(this);
        if (((dBgW *)&mMeshCollider)->IsEnabled())
            ((dBgW *)&mMeshCollider)->Disable();
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN15daObjTatefuda_c8InitIdleEv, 0x020bba24, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c8InitIdleEv
/* IDLE, enter: nothing to do. The PMF record binds a member, so this
   receives `this` and ignores it. */
void daObjTatefuda_c::InitIdle()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- _ZN15daObjTatefuda_c4IdleEv, 0x020bb9fc, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c4IdleEv
/* IDLE, update: let a player start talking to the sign (TryStartTalk);
 * only if that did not happen, check for a grab or a break (CheckGrabOrBreak). */
void daObjTatefuda_c::Idle(){
  if(TryStartTalk()!=0) return;
  CheckGrabOrBreak();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- _ZN15daObjTatefuda_c8InitTalkEv, 0x020bb9f0, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c8InitTalkEv
/* TALK, enter: restart the talk walk-up at step 0. */
void daObjTatefuda_c::InitTalk()
{
    mTalkStep = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- _ZN15daObjTatefuda_c4TalkEv, 0x020bb614, size 0x3dc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c4TalkEv
/* recovered: shared common types */
/* TALK, update: daObjTatefuda_c's talk routine, ov002 0x020bb614, 0x3dc bytes.
 * Called on the signpost (`c` is a daObjTatefuda_c *, the class that owns
 * 0x020bb23c..0x020bc240; see include/daObjTatefuda_c.h) once a player has
 * started talking to it; the player is c->mTalkingPlayer.
 *
 * The message ID is the low 16 bits of the sign's param1 (param1 == 0xffff
 * leaves it 0).
 *
 * It walks the player through a three step approach (c->mTalkStep) and then
 * shows the sign's message: step 0 turns the player toward the reading spot (or
 * skips straight to step 1 if the player is closer than 0x32000, 50 units,
 * of it), step 1 walks the player onto it (Vec3_ApproachHorz with 0xa000, 10 units), step 2
 * turns the player around to face the sign (0x8000 off the sign's own angle)
 * and then either shows the message straight away or, when mPoundsLeft is
 * exactly 1, goes through a short sequence first (after calling
 * func_ov002_020bec9c(player, 0, 0, 0x1000, 0)): when
 * func_ov002_020bec84(player, n) is non-zero for n = 1 or 0 it calls
 * func_ov002_020bec9c(player, 2, ...); for n = 2 it calls (player, 3, ...); for
 * n = 3 it shows the message. The last two also wait on Player::FinishedAnim. A talk state other than 0 or 1 sends the sign back to
 * IDLE through SetState.
 *
 * The reading spot is the sign's own position pushed 0x5a000 (90 units), or
 * 0x78000 (120 units) when mPoundsLeft is 1, forward along the sign's facing
 * angle, using the shared sine/cosine table at data_02082214 (the angle is read
 * unsigned, >> 4, to index it; the products are Fix12 multiplies, rounded). The
 * message is anchored 0x50000 (80 units) above the sign itself.
 *
 * The tail is a special case for message 0x74a (what it stands for is not
 * recovered): while data_0209d660 is set and this sign carries that message,
 * data_0209d6bc == 3 sets the flag word data_0209f284 to 1 and
 * data_0209d6bc == 9 clears it to 0. mFlagSeen holds the value this routine
 * last saw; sound 0x24 plays on a talk frame where the flag differs from it
 * and is non-zero.
 *
 * Vec3_ApproachHorz returns int (the ROM does `bl` then `cmp r0, #0`), which
 * decl_common.h now declares. */
void daObjTatefuda_c::Talk()
{
    /* C89: all locals at top. */
    struct Vector3 msgPos;
    struct Vector3 tgt;
    struct Vector3 plPos;
    Player *player;
    u16 msgId;
    u8 *talkStep;
    s32 scale;
    s32 talk;
    u8 st;
    s32 tx, ty, tz;
    s32 my, mz, mx;
    s32 ang;
    s16 sinV, cosV;
    s32 param;

    msgId = 0;
    param = param1;
    player = mTalkingPlayer;
    if (param != 0xffff) {
        msgId = (u16)param;
    }
    /* The raised message anchor. The horizontal pair is read first and the
       raised height last: that order is what puts the 0x60 load in the gap the
       ROM leaves after `lslne`, which in turn hands the height r1 and the depth
       r2 the way the ROM colours them. Folding the +0x50000 into the temp
       rather than into the store is the other half of it. */
    mx = mPosX;
    mz = mPosZ;
    my = mPosY + 0x50000;
    msgPos.x = mx;
    msgPos.y = my;
    msgPos.z = mz;

    scale = 0x5a000;
    if (mPoundsLeft == 1) {
        scale = 0x78000;
    }
    tx = mPosX;
    tgt.x = tx;
    ty = mPosY;
    tgt.y = ty;
    tz = mPosZ;
    tgt.z = tz;


    ang = (s32) * (u16 *)&mAngleY;
    sinV = data_02082214[(ang >> 4) * 2];
    tx = tx + FIXMUL(scale, sinV);
    tgt.x = tx;

    ang = (s32) * (u16 *)&mAngleY;
    cosV = data_02082214[(ang >> 4) * 2 + 1];
    tz = tz + FIXMUL(scale, cosV);
    tgt.z = tz;

    {
        s32 *pPos = &player->mPosX;
        plPos.x = pPos[0];
        plPos.y = pPos[1];
        plPos.z = pPos[2];
    }

    talk = _ZN6Player12GetTalkStateEv(player);
    switch (talk) {
    case 0:
        st = mTalkStep;
        switch (st) {
        case 0:
            if (Vec3_HorzDist(&plPos, &tgt) < 0x32000) {
                talkStep = &mTalkStep;
                *talkStep = (u8)(*talkStep + 1);
            } else if (_Z14ApproachLinearRsss(
                           &player->mAngleY,
                           Vec3_HorzAngle(&plPos, &tgt),
                           0x800)
                       != 0) {
                talkStep = &mTalkStep;
                *talkStep = (u8)(*talkStep + 1);
                func_ov002_020bec9c(player, 1, 0, 0x1000, 0);
            }
            break;
        case 1:
            if (Vec3_ApproachHorz((struct Vector3 *)&player->mPosX, &tgt, 0xa000) != 0) {
                talkStep = &mTalkStep;
                *talkStep = (u8)(*talkStep + 1);
            }
            break;
        case 2:
            if (_Z14ApproachLinearRsss(
                    &player->mAngleY,
                    (s16)(mAngleY + 0x8000),
                    0x800)
                != 0) {
                if (mPoundsLeft == 1) {
                    if (func_ov002_020bec84(player, 1) != 0
                        || func_ov002_020bec84(player, 0) != 0) {
                        func_ov002_020bec9c(player, 2, 0x40000000, 0x1000, 0);
                    } else if (func_ov002_020bec84(player, 2) != 0
                               && _ZN6Player12FinishedAnimEv(player) != 0) {
                        func_ov002_020bec9c(player, 3, 0x40000000, 0x1000, 0);
                    } else if (func_ov002_020bec84(player, 3) != 0
                               && _ZN6Player12FinishedAnimEv(player) != 0) {
                        _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh(
                            player, this, (s16)msgId, &msgPos, 0, 1);
                    }
                } else {
                    func_ov002_020bec9c(player, 0, 0, 0x1000, 0);
                    _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh(
                        player, this, (s16)msgId, &msgPos, 0, 1);
                }
            }
            break;
        }
        break;
    case 1:
        break;
    default:
        SetState(STATE_IDLE);
        break;
    }

    if (data_0209d660 != 0 && msgId == 0x74a) {
        switch (data_0209d6bc) {
        case 3:
            data_0209f284 = 1;
            break;
        case 9:
            data_0209f284 = 0;
            break;
        }
    }

    if (mFlagSeen != data_0209f284 && data_0209f284 != 0) {
        func_02012790(0x24);
    }
    mFlagSeen = data_0209f284;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- _ZN15daObjTatefuda_c12TryStartTalkEv, 0x020bb520, size 0xf4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c12TryStartTalkEv
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* IDLE, update, first half: has a player started talking to the sign? Needs,
 * from the collider (mdCcAc_c): an other-owner ID (otherOwner), and hit-flag
 * bit 0x8000000 (not in dCc_c.h's bit table). The sign must not be fully pounded
 * in (mPoundsLeft != 0), the other actor must be the player, and the player must
 * lie within 0x4000 (a quarter turn) of the sign's facing: AngleDiff is the
 * absolute 16-bit difference. Then the player is remembered in mTalkingPlayer
 * (before Player::StartTalk is tried, so it stays set if that refuses) and, on
 * success, the sign enters TALK. Returns 1 only in that case. */
int daObjTatefuda_c::TryStartTalk(){
  unsigned int id = mdCcAc_c.otherOwner;
  if (id == 0) return 0;
  if ((mdCcAc_c.hitFlags & 0x8000000) == 0) return 0;
  if (mPoundsLeft == 0) return 0;
  {
    dActor_c* other = (dActor_c*)_ZN8dActor_c10FindWithIDEj(id);
    if (other == 0) goto fail;
    {
      int b = (int)(other->actorID == ACTOR_PLAYER);
      if (b != false) goto success;
    }
  fail:
    return 0;
  success:
    {
      int ang = Vec3_HorzAngle((struct Vector3*)&mPosX, (struct Vector3*)&other->mPosX);
      if (AngleDiff(ang, mAngleY) > 0x4000) return 0;
      mTalkingPlayer = (Player *)other;
      if (_ZN6Player9StartTalkER7fBase_cb((char *)other, (char *)this, 0) == 0) return 0;
      SetState(STATE_TALK);
      return 1;
    }
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN15daObjTatefuda_c16CheckGrabOrBreakEv, 0x020bb42c, size 0xf4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c16CheckGrabOrBreakEv
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* IDLE, update, second half: is a player hit flagged 0x40000 or 0x1000? The
 * collider must report an other-owner ID whose actor is the player. Then:
 * hit-flag bit 0x40000 (listed as fire in dCc_c.h's unverified table; Init
 * registers it in vulnFlags 0x41000) starts the break countdown, mBreakTimer = 0x3c (60 frames), and returns. Otherwise
 * the other actor's param1 must be 2 and it must lie MORE than 0x4000 (a
 * quarter turn) from the sign's facing (behind it, where the talk check wants
 * it in front), hit-flag bit 0x1000 (grab, in that table) must be set, and
 * Player::TryGrab must accept; then the player becomes mHoldingPlayer and the
 * sign enters CARRIED. */
void daObjTatefuda_c::CheckGrabOrBreak(){
  dActor_c* other;
  unsigned int id = mdCcAc_c.otherOwner;
  if (id == 0) return;
  other = (dActor_c*)_ZN8dActor_c10FindWithIDEj(id);
  if (other == 0) return;
  {
    int b = (int)(other->actorID == ACTOR_PLAYER);
    if (b == 0) return;
  }
  if ((mdCcAc_c.hitFlags & 0x40000) != 0) {
    mBreakTimer = 0x3c;
    return;
  }
  {
    int ang = Vec3_HorzAngle((struct Vector3*)&mPosX, (struct Vector3*)&other->mPosX);
    if (other->param1 != 2) return;
    if (AngleDiff(ang, mAngleY) <= 0x4000) return;
  }
  if ((mdCcAc_c.hitFlags & 0x1000) == 0) return;
  if (_ZN6Player7TryGrabER8dActor_c((char *)other, (char *)this) == 0) return;
  mHoldingPlayer = (Player *)other;
  SetState(STATE_CARRIED);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- _ZN15daObjTatefuda_c4KillEv, 0x020bb3b8, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c4KillEv
/* daObjTatefuda_c::Kill() at ov002 0x020bb3b8, 0x74 bytes -- vtable slot 31.
 *
 * ATTRIBUTED BY THE VTABLE. _ZTV15daObjTatefuda_c (ov002 0x02109af8, and the same
 * address as _ZTV15daObjTatefuda_c) carries 0x020bb3b8 at vtable + 0x7c, which
 * is slot 31, while _ZTV10dBgActor_c carries _ZN10dBgActor_c4KillEv at the same slot
 * and both tables carry dActor_c's 0x020100dc at slot 30. So this is this class's
 * own override of the one virtual dBgActor_c adds. Read out of
 * config/arm9/overlays/ov002/relocs.txt.
 *
 * The signpost does not destroy itself. It plays its particle 0x28000 (40.0 in
 * Fix12) above where it stands, poofs, plays sound bank 3 id 0x41 and then
 * tails into Reset, this class's own reset routine.
 * That is why there is no MarkForDestruction here, unlike dBgActor_c::Kill.
 *
 * The trailing call's return value is dropped: the ROM does `bl`, then the
 * epilogue and `bx lr` with nothing written to r0 in between, which is what a
 * void method calling an int function compiles to.
 *
 * The second Vector3 is memberwise on purpose: Vector3 declares a destructor
 * (types.h), so a whole-object assignment compiles to an ldm/stm pair, four
 * instructions where the ROM has six. Particle::System::NewSimple stays spelled
 * as its mangled name -- its parameters are Fix12<int> BY VALUE and declaring
 * the true types changes how the caller passes them. */
/* Reset returns void now; Kill drops the value regardless. */
void daObjTatefuda_c::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    Fix12i x = mPosX;
    Fix12i y = mPosY + 0x28000;
    Fix12i z = mPosZ;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xe, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    DisappearPoofDustAt(dustPos);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    Reset();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN15daObjTatefuda_c15OnHitByMegaCharER6Player, 0x020bb374, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c15OnHitByMegaCharER6Player
/* daObjTatefuda_c::OnHitByMegaChar -- vtable slot 27, ov002 0x020bb374.
 * reloc: _ZTV15daObjTatefuda_c+0x6c -> 0x020bb374, _ZTV10dBgActor_c+0x6c ->
 * 0x02010130 (different, real override).
 *
 * SIGNATURE FROM include/dActor_c.h's OWN SLOT 27, `virtual void
 * OnHitByMegaChar(Player &player)` -- `int` until daObjPile_c::OnHitByMegaChar
 * proved it wrong tree-wide (36bc6d1df). Same body shape
 * src/game/actors/d_a_obj_maruta.cpp records for its own
 * slot 27: dBgActor_c::KillByMegaChar is non-virtual, so the unqualified
 * call is already the direct `bl` the ROM has. mAngleY = mPrevAngleY is
 * dActor_c's own field pair (include/dActor_c.h, 0x08e/0x094). */
void daObjTatefuda_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    func_02012694(0x1d, &mCamSpacePosX);
    KillByMegaChar(player);
    mAngleY = mPrevAngleY;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN15daObjTatefuda_c15OnGroundPoundedER8dActor_c, 0x020bb27c, size 0xf8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c15OnGroundPoundedER8dActor_c
/* daObjTatefuda_c::OnGroundPounded -- vtable slot 21, ov002 0x020bb27c.
 * reloc: _ZTV15daObjTatefuda_c+0x54 -> 0x020bb27c, _ZTV10dBgActor_c+0x54 ->
 * 0x02010148 (different, real override).
 *
 * include/dActor_c.h's own slot 21 supplies the signature -- `void`, the
 * tree-wide fix from daObjPile_c::OnGroundPounded (36bc6d1df).
 *
 * mPoundsLeft/mPoundCooldown/mRespawnDelay are this class's own fields (include/daObjTatefuda_c.h);
 * mPoundCooldown and mRespawnDelay were undescribed padding until this method's body
 * proved they are read/written. `other` is read at +0x703, past dActor_c's
 * own span -- same raw-offset reading daObjPile_c::OnGroundPounded records
 * for its own slot 21; Player.h names that byte mIsMega, so the read is
 * spelt through a Player & here. A param1 of 2 or a non-zero mIsMega makes
 * the pound take every pound left at once (the sign sinks mPoundsLeft * 0x2d
 * = 45 units per pound, as `<< 12` Fix12, and mPoundsLeft becomes 0); otherwise
 * it takes one (0x2d000 = 45 units, mPoundsLeft - 1) and starts the 0xf-frame
 * mPoundCooldown. Both set mRespawnDelay = 0x1e (30 frames). */
void daObjTatefuda_c::OnGroundPounded(dActor_c &other)
{
    if (mPoundsLeft == 0) return;
    if (mPoundCooldown != 0) return;
    Sound::PlayBank3(0x62, *(const Vector3 *)&mCamSpacePosX);
    if (other.param1 == 2 || ((Player &)other).mIsMega != 0) {
        mPosY -= (mPoundsLeft * 0x2d) << 12;
        mPoundsLeft = 0;
        ((dBgActor_c *)this)->UpdateModelPosAndRotY();
        ((dBgActor_c *)this)->UpdateClsnPosAndRot();
        mRespawnDelay = 0x1e;
    } else {
        mPosY -= 0x2d000;
        mPoundsLeft -= 1;
        ((dBgActor_c *)this)->UpdateModelPosAndRotY();
        ((dBgActor_c *)this)->UpdateClsnPosAndRot();
        mPoundCooldown = 0xf;
        mRespawnDelay = 0x1e;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN15daObjTatefuda_c11OnAttacked1ER8dActor_c, 0x020bb23c, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c11OnAttacked1ER8dActor_c
/* daObjTatefuda_c::OnAttacked1 -- vtable slot 22, ov002 0x020bb23c.
 * reloc: _ZTV15daObjTatefuda_c+0x58 -> 0x020bb23c, _ZTV10dBgActor_c+0x58 ->
 * 0x02010144 (different, real override).
 *
 * include/dActor_c.h's own slot 22 supplies the signature -- still `int`,
 * unlike slots 21/24/27 (see 36bc6d1df).
 *
 * The pre-migration recovery read `other`'s actorID (dActor_c +0xc) through
 * a shadow struct and dispatched through a bare virtual-call shape (`Base::M`
 * at the shadow's slot 31, this class's own Kill -- include/daObjTatefuda_c.h). An
 * unqualified `Kill()` here is that same virtual dispatch. */
int daObjTatefuda_c::OnAttacked1(dActor_c &other)
{
    int isCode = (other.actorID == ACTOR_BOMBHEI);
    if (isCode) {
        Kill();
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- AttachToHolder, 0x020bb060, size 0x1dc */
/* -------------------------------------------------------------------------- */
// @symbol AttachToHolder
/* Put the sign on its holder (Behavior's step 2 and Render both call this).
 * `result` is the holder's current body model, from the shared helper
 * func_ov002_020e496c; m2 is the pointer stored at +0x14 of it, and the
 * Matrix4x3 at m2 + 0x2a0 is multiplied in. r4 picks a pose, 0 or 1: 1 when
 * the holder is front-sliding, or has lost its grabbed object while the body
 * model's word at +0x58 (the dExtFrameCtrl_c base's current frame, per Player.h)
 * satisfies (word << 4) >> 16 < 0xe. The X angle approaches data_ov002_020ff0d0[r4] by 0x1000 per call;
 * the sign copies the holder's Y angle (into mAngleY and mPrevAngleY); the
 * matrix is built from a translation, a pivot offset of 0x8c00 up and back, the
 * sign's rotation (Matrix4x3_ApplyInPlaceToRotationXYZExt), and a per-pose offset taken from the three tables at
 * data_ov002_020ff0d4/d8/dc (12 bytes per pose). The position is that
 * matrix's translation shifted left 3 (matrix units are position >> 3) and
 * the matrix itself is stored into mModel at 0xf0. */
#pragma cplusplus off
void AttachToHolder(struct daObjTatefuda_c *self)
{
    struct Vec3 v;
    struct Vec3 lo;
    char *result = func_ov002_020e496c(self->mHoldingPlayer);
    char *m2 = *(char **)(result + 0x14);
    int r4 = 0;

    if (_ZN6Player14IsFrontSlidingEv(self->mHoldingPlayer))
        r4 = 1;
    if (_ZN6Player17LostGrabbedObjectEv(self->mHoldingPlayer)) {
        if ((unsigned int)(*(int *)(result + 0x58) << 4) >> 0x10 < 0xe)
            r4 = 1;
    }

    _Z14ApproachLinearRsss(&self->mAngleX, data_ov002_020ff0d0[r4], 0x1000);

    ((int *)&v)[0] = 0;
    ((int *)&v)[1] = 0;
    ((int *)&v)[2] = 0;
    *(struct RawMatrix4x3 *)&data_020a0e68 = *(struct RawMatrix4x3 *)(result + 0x1c);
    MulMat4x3Mat4x3(m2 + 0x2a0, &data_020a0e68, &data_020a0e68);
    v.x = data_020a0e68.t.x;
    v.y = data_020a0e68.t.y;
    v.z = data_020a0e68.t.z;

    self->mPrevAngleY = self->mHoldingPlayer->mAngleY;
    self->mAngleY = self->mPrevAngleY;

    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0x8c00, 0);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, self->mAngleX, self->mAngleY, self->mAngleZ);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, -0x8c00, 0);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, *(int *)((char *)data_ov002_020ff0d4 + r4 * 0xc), *(int *)((char *)data_ov002_020ff0d8 + r4 * 0xc), *(int *)((char *)data_ov002_020ff0dc + r4 * 0xc));

    self->mPosX = data_020a0e68.t.x;
    self->mPosY = data_020a0e68.t.y;
    self->mPosZ = data_020a0e68.t.z;
    Vec3_Lsl(&lo, (struct Vec3 *)&self->mPosX, 3);
    self->mPosX = lo.x;
    self->mPosY = lo.y;
    self->mPosZ = lo.z;
    *(struct RawMatrix4x3 *)&self->mModel.mat4x3 = *(struct RawMatrix4x3 *)&data_020a0e68;
}
#pragma cplusplus on

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- RebuildModelMatrix, 0x020bafc0, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol RebuildModelMatrix
/* recovered: shared common types */
/* Rebuild the model matrix from the sign's own position and angles: a
 * translation to position >> 3 (matrix units are position >> 3), a pivot
 * offset of 0x8c00 up, the rotation (Matrix4x3_ApplyInPlaceToRotationZXYExt), the pivot offset back
 * down, stored into mModel at 0xf0 through the scratch matrix data_020a0e68.
 * Called by DROPPED's update, and by Behavior while the state is THROWN. */
#pragma cplusplus off
void RebuildModelMatrix(struct daObjTatefuda_c* self){
    struct Vector3 v;
    Vec3_Asr(&v, (struct Vector3*)&self->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0x8c00, 0);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
        self->mAngleX, self->mAngleY, self->mAngleZ);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, -0x8c00, 0);
    *(struct RawMatrix4x3 *)&self->mModel.mat4x3 = *(struct RawMatrix4x3 *)&data_020a0e68;
}
#pragma cplusplus on

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN15daObjTatefuda_c18UpdateShadowMatrixEv, 0x020baf80, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c18UpdateShadowMatrixEv
/* Rebuild the drop shadow's matrix: a rotation about Y by the sign's angle,
 * with the translation set to the position >> 3 (the matrix's 0x24..0x2f is
 * its translation row; units are position >> 3). */
void daObjTatefuda_c::UpdateShadowMatrix()
{
    Matrix4x3_FromRotationY(&mShadowMat, mAngleY);
    mShadowMat.t.x = mPosX >> 3;
    mShadowMat.t.y = mPosY >> 3;
    mShadowMat.t.z = mPosZ >> 3;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN15daObjTatefuda_c5ResetEv, 0x020bae9c, size 0xe4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_c5ResetEv
/* The reset: the sign goes back where it was spawned and hides. Sets mHidden
 * and mRespawnDelay = 0x1e (30 frames), clears the break countdown and both
 * particle handles, restores position and angles from mHomePosX/Y/Z and mHomeAngleX/Y/Z,
 * stands it up again (mPoundsLeft = 2), refreshes the model/collision
 * matrices and the shadow matrix, returns to IDLE, re-initializes the collider
 * (same arguments as InitResources) and clears it, and switches the mesh
 * collider off while it is enabled. Called by Kill, by the break
 * countdown and by THROWN's update. */
void daObjTatefuda_c::Reset()
{
  mHidden = 1;
  mRespawnDelay = 0x1e;
  mBreakTimer = 0;
  mParticleHandle1 = 0;
  mParticleHandle2 = 0;
  mPosX = mHomePosX;
  mPosY = mHomePosY;
  mPosZ = mHomePosZ;
  mAngleX = mHomeAngleX;
  mAngleY = mHomeAngleY;
  mAngleZ = mHomeAngleZ;
  mPoundsLeft = 2;
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(this);
  UpdateShadowMatrix();
  SetState(STATE_IDLE);
  unk_31c = 0;
  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x64000, 0x64000, 0x4800002, 0x41000);
  if (_ZN4dBgW9IsEnabledEv(&mMeshCollider))
  {
    _ZN4dBgW7DisableEv(&mMeshCollider);
  }
  _ZN5dCc_c5ClearEv(&mdCcAc_c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN15daObjTatefuda_cD0Ev, 0x020bae2c, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_cD0Ev
/* The deleting destructor comes from the key function: Kill is defined in
 * this TU (real body, vtable slot 31), so mwcc emits the vtable, the RTTI
 * and the destructor variants alongside it. No shard tricks remain. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN15daObjTatefuda_cD1Ev, 0x020badd0, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjTatefuda_cD1Ev
/* The complete-object destructor comes from the key function (see D0). */

/* __sinit_daObjTatefuda_c.cpp constructs the two file handles in retail
 * order, then copies the ten anonymous pointer-to-member descriptors into
 * the {enter, update} state table SetState and UpdateState index by mState. */
SignPostModelFilePtr SignPost_ModelFile(0x491);
SignPostCollisionFilePtr SignPost_ClsnFile(0x492);
Entry data_ov002_0210e084[5] = {
    { &daObjTatefuda_c::InitIdle,    &daObjTatefuda_c::Idle    },
    { &daObjTatefuda_c::InitTalk,    &daObjTatefuda_c::Talk    },
    { &daObjTatefuda_c::InitCarried, &daObjTatefuda_c::Carried },
    { &daObjTatefuda_c::InitThrown,  &daObjTatefuda_c::Thrown  },
    { &daObjTatefuda_c::InitDropped, &daObjTatefuda_c::Dropped },
};
