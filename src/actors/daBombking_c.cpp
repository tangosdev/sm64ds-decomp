//cpp
/* daBombking_c -- King Bob-omb, Bob-omb Battlefield (BOMBKING).
 *
 * Out-of-line destructor first and `#pragma defer_codegen off`: that pair
 * emits D1 then D0 at the head of this TU. Source order is ROM order.
 * common.h is included first so Matrix4x3 stays the flat 12-word spelling;
 * shadow and hold copies go through M12.
 *
 * Pragmas are file-global last-wins, so the ones this TU needs are push/pop
 * around one member: opt_propagation off on func_ov078_021240a0,
 * opt_strength_reduction and opt_common_subs off on func_ov078_02125448,
 * opt_strength_reduction off on InitResources.
 *
comment leftovers:
 * - BlendModelAnim::SetAnim, dCcAcPos_c::Init and DropShadowRadHeight take
 *   Fix12<int> by value. func_ov078_02124060 cannot call SetAnim with a
 *   scalar speed (illegal implicit conversion) and Fix12<int>(0x1000) is
 *   the same illegal conversion. A class temporary is the caller-side cost
 *   in notes/mwccarm-codegen.md. The calls stay the scalar externs.
 * - dBgCh_Actr::Init's definition is
 *   _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_. The header
 *   spells Fix12i, which mangles as i, so InitResources keeps the extern.
 * - Player::param1 and Player::mStateFlags are used. The held matrix at
 *   actor+0xc8 is inside dActor_c's pad (0xc5..0xcb); naming it is a base
 *   change. func_ov078_02125de0, Render and the throw helper still store it.
 * - dCamera_c::mFlags (0x154, bit 8) is used. Behavior's store at dCamera_c+0x114
 *   is the word after mTargetPlayer, still pad_114.
 * - func_02035550 is the call that ORs 0x4000 into dBgCh_Actr::mFlags;
 *   inlining the or would delete the call.
 * - func_02012694 plays bank 3 at mCamSpacePos. It is not Sound::PlayBank3
 *   (that body is 0x02012664). func_0200d8c8 shakes the camera.
 *   func_ov002_020db54c launches the grabbed player; func_ov002_020db5f4
 *   starts the player's hold state. func_ov102_0214b384 arms a bob-omb
 *   fuse; func_ov102_0214ad14 makes it chase. GetHealth is called with no
 *   receiver in func_ov078_02123f1c: r0 is whatever the previous call left.
 * - func_ov078_021240a0 stays a free function: it is method-shaped (object
 *   in arg0, in a sinit PMF record) but the member spelling — and even the
 *   `extern "C" int` definition spelling — reschedules the clamp arm's
 *   register allocation under opt_propagation off (7 words). The
 *   `extern "C" { }` block form is byte-correct; it still calls the
 *   member helpers through k.
 * - File handles are BombkingFile (id, file at +4), filled by
 *   __sinit_ov078_02126660. State records stay data_ov078_021270*: the
 *   sinit copies an {enter, tick} PMF pair into each. g_profile_BOMBKING
 *   stays in the overlay data.
 * - A few address launders stay because the plain member form moved a
 *   word: mPrevAngleY in func_ov078_02123d3c, mHealth in
 *   func_ov078_021243c0, mVertSpeed in func_ov078_021247bc, unk_50a in
 *   func_ov078_021259ec.
 */

/* common.h FIRST: daBombking_c.h reaches math/Matrix.h through BlendModelAnim.h,
 * and that header spells Matrix4x3 as {Matrix3x3 r; Vector3 t;} where common.h
 * spells it flat as s32 m[12]. Helpers whole-struct-assign the shadow/hold
 * matrices through M12; only the flat spelling reproduces the block move. */
#include "common.h"
#include "daBombking_c.h"
#include "daBmb_c.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"
#include "decl_common.h"
#include "decl_Animation.h"
#include "decl_Message.h"
#include "Player.h"
#include "dCamera_c.h"
#include "Message.h"

bool ApproachLinear(short &value, short target, short step);

/* Remaining reconstruction views. Reconciled by hand against include/: every type
 * the real headers already define (Vector3, Matrix4x3, Fix12<int>, u8/u16/s16,
 * dActor_c, fBase_c, Player, BMD_File, BlendModelAnim) was DROPPED here -- the
 * generated preamble redefined all of them. The shapes below are surviving
 * views from the legacy shards, not evidence of original local classes. */

/* File-local flat matrix. math/Matrix.h spells Matrix4x3 as {Matrix3x3 r;
 * Vector3 t;} while common.h spells it flat; whole-matrix assignment must stay
 * flat or mwcc splits it into ldm/stm + a CSE'd tail. */
struct M12 { int w[12]; };

struct Vec3 { int x, y, z; };

/* Whole-struct 3-word copies MUST go through an array member. mwccarm block-moves
 * (ldm/stm) a POD struct copy under -lang c99 but scalarises the same copy under
 * -lang c++; an array member forces the block move back. Four members of this TU
 * turn on this one rule. */
struct M3 { int w[3]; };
struct BCA_File;
typedef daBombking_c::StateFunction PMF;
extern "C" int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *bca, int a, int b, int fix, unsigned short t);

/* What __sinit_ov078_02126660 builds. Construct stores the file id; Load
 * writes the bytes at +4, which is the pointer SetAnim reads. 02126f38 is
 * the model (func_02017acc, id 0x2c7). The other twelve are animations
 * (SharedFilePtr::Construct, ids 0x2bb..0x2c6). */
struct BombkingFile {
    unsigned fileId;
    void *file;
};

extern "C" {
    extern BombkingFile data_ov078_02126ee0;
    extern BombkingFile data_ov078_02126ee8;
    extern BombkingFile data_ov078_02126ef0;
    extern BombkingFile data_ov078_02126ef8;
    extern BombkingFile data_ov078_02126f00;
    extern BombkingFile data_ov078_02126f08;
    extern BombkingFile data_ov078_02126f10;
    extern BombkingFile data_ov078_02126f18;
    extern BombkingFile data_ov078_02126f20;
    extern BombkingFile data_ov078_02126f28;
    extern BombkingFile data_ov078_02126f30;
    extern BombkingFile data_ov078_02126f38;
    extern BombkingFile data_ov078_02126f40;
    /* Canonical single declaration per ROM data symbol -- the generated
     * preamble carried up to three contradictory spellings of each. */
    extern unsigned char data_0209f220[];
    extern void *data_0209f318;
    extern struct M12 data_020a0e68;
    extern int data_ov078_0212703c[];
    extern int data_ov078_0212705c[];
    extern int data_ov078_0212707c[];
    extern int data_ov078_0212709c[];
    extern int data_ov078_021270cc[];
    extern int data_ov078_021270dc[];
    extern int data_ov078_021270fc[];
    extern int data_ov078_0212710c[];
extern int data_ov078_02126ffc[];
extern int data_02092138;
extern void MulMat4x3Mat4x3(void *dst, void *a, void *b);
extern void Vec3_Lsl(void *d, void *s, int sh);
extern void func_02012694(int a, void *p);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
extern void func_02011cfc(void);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int b);
extern short Vec3_HorzAngle(const void* a, const void* b);
extern int Vec3_Dist(const void* a, const void* b);
extern void ApproachAngle(void *p, int target, int a, int b, int c);
/* local extern: the matched call sets up no Player in r0 (it reuses whatever the
   previous call left), so there is no object to call Player::GetHealth() on */
extern "C" int _ZN6Player9GetHealthEv(void);
extern int RandomIntInternal(int* seed);
extern int data_0209e650;
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulVec3Mat4x3(Vector3 *v, void *m, Vector3 *out);
extern short Vec3_VertAngle(const void *a, const void *b);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angle);
extern void func_0200fa8c(void *c, int a);
extern void func_ov102_0214b384(void* a, int b);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern int Vec3_HorzLen(void* v);
extern int func_ov002_020db5f4(char* c, char* arg);
extern void func_0200d8c8(void* cam, void* v, int strength);
void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, int fix, int t, unsigned int j);
extern Matrix4x3 IDENTITY_MATRIX4X3;
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void* self, dActor_c* a, Vector3* v, Fix12i r, Fix12i h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, Fix12i r, Fix12i h, Vector3_16* p, Vector3_16* q);
}


/* `opt_strength_reduction`, `opt_common_subs` and `opt_propagation` are
 * file-global last-wins in mwccarm 2004/b56, so the three members that need
 * them would otherwise recompile the other 48. `defer_codegen off` makes the
 * positional push/pop brackets below actually bind -- and it also flips .text
 * emission from reversed to source order, which is why this file is written
 * ROM-ASCENDING. The two are one decision: linkcheck [4b/8] refuses the mix.
 */
#pragma defer_codegen off

/* Out of line, and FIRST in this ROM-ascending file: that is what puts D1 and
 * D0 at 0x02123740 / 0x02123798, the two lowest addresses in the run. */
daBombking_c::~daBombking_c() {}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * A consequence of `struct daBombking_c : dEnemyBase_c`: its own vptr store,
 * then the destructors of the five members that have one -- dBgCh_Actr at
 * +0x110, BlendModelAnim at +0x2cc, dCcAcPos_c at +0x33c and +0x37c, and
 * dExtCommonModel_c at +0x3bc -- then the base chain. Each of those member calls is
 * a relocation the ROM build checks, which is what named the members.
 *
 * DEFINED OUT OF LINE at the top of this file, and DECLARED FIRST in the
 * header. Both halves are load-bearing; see the block above the definition.
 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap. Nobody writes that; declaring `~daBombking_c()` is enough, because
 * mwcc emits D1, D0 and D2 together (measured on this TU, in that order) and
 * objisolate keeps the ones this file is bound to. D2 is homeless -- the ROM
 * has no D2 -- and is licensed as `deadstrip` in the manifest.
 *
 * The deallocation is an inline operator delete, which is why nothing below
 * mentions a heap.
 */

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123804Ev

int daBombking_c::func_ov078_02123804()
{
    unsigned int v = this->mdCcAcPos_c2.otherOwner;
    if (v == 0) return 0;
    if (dActor_c::FindWithID(v) == 0) return 0;
    if ((this->mdCcAcPos_c2.hitFlags & 0x4000) == 0) return 0;
    KingBobOmb_SetState(data_ov078_02126ffc);
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123864Ev

void daBombking_c::func_ov078_02123864()
{
  int i = 0;
  do {
    daBmb_c *bmb = (daBmb_c *)dActor_c::FindWithID(this->mSpawnedId[i]);
    if (bmb) {
      bmb->unk_3e0 = 0;
      bmb->unk_3f6 = 1;
    }
    i++;
  } while (i < 2);
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_021238acEv

int daBombking_c::func_ov078_021238ac()
{
    dCamera_c *cam = (dCamera_c *)data_0209f318;
    struct Vec3 v;
    struct Vec3 t;
    struct Vec3 u;

    if (this->mTalkingPlayer->GetTalkState() != -1) {
        cam->SetFlag_3();
        return 1;
    }

    cam->mFlags &= ~8;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    data_020a0e68 = *(struct M12 *)&this->mBlendModelAnim.mat4x3;
    MulMat4x3Mat4x3((char *)this->mBlendModelAnim.data.transforms + 0x30, &data_020a0e68, &data_020a0e68);

    v.x = data_020a0e68.w[9];
    v.y = data_020a0e68.w[10];
    v.z = data_020a0e68.w[11];
    Vec3_Lsl(&t, &v, 3);
    v.x = t.x;
    v.y = t.y;
    v.z = t.z;
    func_02012694(0x130, &this->mCamSpacePosX);

    this->UntrackAndSpawnStar(*(s8 *)&this->mStarTracked, this->mStarID, *(Vector3 *)&v, 4);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x1a, v.x, v.y, v.z);
    u.x = v.x;
    u.y = v.y;
    u.z = v.z;
    this->TriplePoofDustAt(*(Vector3 *)&u);

    if (this->mMusicLayer3 == 1) {
        this->mMusicLayer3 = 0;
        _ZN5Sound22StopLoadedMusic_Layer3Ev();
        func_02011cfc();
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x15666);
    }
    this->fBase_c::MarkForDestruction();
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123a3cEv

int daBombking_c::func_ov078_02123a3c()
{
    this->mAnimSpeed = 2;
    this->mHorzSpeed = 0;
    this->mStateTimer = 0x32;
    func_ov078_02123864();
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&this->mBlendModelAnim, data_ov078_02126f20.file, 0, 0, 0x1000, 0);
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123aa0Ev

int daBombking_c::func_ov078_02123aa0()
{
    short ang = Vec3_HorzAngle(&this->mPosX, &this->mHomePosX);
    if (func_ov078_02123804() == 1) return 1;
    Player *p = (Player *)this->ClosestPlayer();
    if (p != 0) {
        struct Vec3 v = *(struct Vec3 *)&p->mPosX;
        if (Vec3_Dist(&this->mArenaPosX, &v) < 0x640000) {
            if (this->mArenaPosY - 0x64000 < v.y) {
                KingBobOmb_SetState(data_ov078_0212703c);
                return 1;
            }
        }
    }
    if (this->unk_505 == 0) {
        func_02012694(0x12d, &this->mCamSpacePosX);
        this->unk_505 = 5;
    }
    ApproachAngle(&this->mPrevAngleY, ang, 5, 0x1000, 0x100);
    this->mAngleY = this->mPrevAngleY;
    if (Vec3_Dist(&this->mPosX, &this->mHomePosX) < 0x32000) {
        KingBobOmb_SetState(data_ov078_0212710c);
    }
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123bc4Ev
/* This caller retains the reconstructed SetAnim ABI declaration above.
   The callee's Fix12 definition experiment in notes/mwccarm-codegen.md 6az
   does not establish that a typed call here would fail to match. */
int daBombking_c::func_ov078_02123bc4()
{
  this->mVertAccel = -0x2000;
  this->mAnimSpeed = 2;
  this->mHorzSpeed = 0xa000;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&this->mBlendModelAnim, data_ov078_02126ee8.file, 4, 0, 0x1000, 0);
  return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123c20Ev


int daBombking_c::func_ov078_02123c20()
{
    if (func_ov078_02123804() == 1) {
        func_ov078_02123864();
        int v = *(int*)&this->mHeldActor;
        if (v != 0) {
            func_ov002_020db54c((char*)v, 0, 0x50000, this->mAngleY);
            this->mTalkingPlayer = this->mHeldActor;
            this->mHeldActor = 0;
        }
        return 1;
    }
    if (((dExtFrameCtrl_c *)&this->mBlendModelAnim)->WillHitFrame(0x14)) {
        int v = *(int*)&this->mHeldActor;
        if (v != 0) {
            func_ov002_020db54c((char*)v, 0x28000, 0x50000, this->mAngleY);
            this->mTalkingPlayer = this->mHeldActor;
            this->mHeldActor = 0;
            func_02012694(0x131, &this->mCamSpacePosX);
        }
    }
    if (((dExtFrameCtrl_c *)&this->mBlendModelAnim)->Finished()) {
        KingBobOmb_SetState(data_ov078_021270fc);
    }
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123cf0Ev
struct BCA_File;

int daBombking_c::func_ov078_02123cf0()
{
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&this->mBlendModelAnim, data_ov078_02126ef0.file, 0, 0x40000000, 0x1000, 0);
  this->mHorzSpeed = 0;
  return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123d3cEv

int daBombking_c::func_ov078_02123d3c()
{
    if (func_ov078_02123804() == 1) {
        func_ov078_02123864();
        if (this->mHeldActor != 0) {
            func_ov002_020db54c((char *)this->mHeldActor, 0, 0x50000, this->mAngleY);
            this->mTalkingPlayer = this->mHeldActor;
            this->mHeldActor = 0;
        }
        return 1;
    }

    Vector3 v;
    v.x = this->mArenaPosX;
    v.y = this->mArenaPosY;
    v.z = this->mArenaPosZ;

    if (data_0209f220[0] != 1) {
        v.x = this->mPosX;
        v.y = this->mPosY;
        v.z = this->mPosZ;
        /* Address launder: a plain mPrevAngleY += 0x1000 reschedules this arm. */
        *(s16 *)(((int)this + 0x94)) += 0x1000;
        this->mHorzSpeed = 0;
    } else {
        s16 a = Vec3_HorzAngle((Vector3 *)&this->mPosX, &v);
        ApproachAngle(&this->mPrevAngleY, a, 5, 0x1000, 0x100);
    }

    this->mAngleY = this->mPrevAngleY;

    if (Vec3_Dist((Vector3 *)&this->mPosX, &v) > 0x1e000) {
        if (this->unk_505 == 0) {
            func_02012694(0x12d, &this->mCamSpacePosX);
            this->unk_505 = 0xf;
        }
    } else {
        if (this->unk_505 == 0) {
            this->unk_505 = 0x14;
            func_02012694(0x12f, &this->mCamSpacePosX);
        }
    }

    if ((u16)this->mStateTimer == 0)
        KingBobOmb_SetState(&data_ov078_021270cc);
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123eb8Ev
int daBombking_c::func_ov078_02123eb8()
{
    mVertAccel = -0x2000;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&mBlendModelAnim, (void*)(data_ov078_02126f28.file), 0, 0, 0x1000, 0);
    mStateTimer = 0x32;
    mHorzSpeed = 0xa000;
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123f1cEv
int daBombking_c::func_ov078_02123f1c()
{
    if (func_ov078_02123804() != 1) {
        if (this->mHeldActor == 0) goto L6c;
        /* No receiver: r0 is whatever the previous call left. */
        if (_ZN6Player9GetHealthEv() != 0) goto L6c;
    }
    func_ov078_02123864();
    if (this->mHeldActor != 0) {
        func_ov002_020db54c((char *)this->mHeldActor, 0, 0x50000, this->mAngleY);
        this->mTalkingPlayer = this->mHeldActor;
        this->mHeldActor = 0;
    }
    return 1;
L6c:
    if (((dExtFrameCtrl_c *)&this->mBlendModelAnim)->Finished()) {
        KingBobOmb_SetState(data_ov078_0212709c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123fb4Ev

int daBombking_c::func_ov078_02123fb4()
{
  this->mVertAccel = -0x2000;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&this->mBlendModelAnim, data_ov078_02126f18.file, 0, 0x40000000, 0x1000, 0);
  return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124000Ev

int daBombking_c::func_ov078_02124000()
{
  int ang = this->HorzAngleToCPlayer();
  ApproachAngle(&this->mPrevAngleY, ang, 1, 0x500, 0x500);
  this->mAngleY = this->mPrevAngleY;
  if (((dExtFrameCtrl_c *)&this->mBlendModelAnim)->Finished()) {
    KingBobOmb_SetState(data_ov078_0212703c);
  }
  return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124060Ev

int daBombking_c::func_ov078_02124060()
{
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&this->mBlendModelAnim, data_ov078_02126f00.file, 0, 0x40000000, 0x1000, 0);
  return 1;
}


#pragma push
#pragma opt_propagation off
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_021240a0
extern "C" {
int func_ov078_021240a0(char* c)
{
    daBombking_c *k = (daBombking_c *)c;
    struct Vector3 v;
    short msg;
    int lim;

    if (k->mWithMeshClsn.IsOnGround() != 0) {
        if (k->mHealth <= 0) {
            if (k->mHorzSpeed != 0) {
                k->mHorzSpeed = 0;
                k->func_ov078_02125c24(0x7d0000);
            }
            if ((unsigned short)(k->mTalkingPlayer->mStateFlags & 0x800) != 0)
                return 1;

            ApproachAngle(&k->mPrevAngleY, k->HorzAngleToCPlayer(), 5, 0x1000, 0x200);
            k->mAngleY = k->mPrevAngleY;
            if (AngleDiff(k->HorzAngleToCPlayer(), k->mAngleY) < 0x1000) {
                Player *pl = k->mTalkingPlayer;
                if (pl->StartTalk(*(fBase_c *)c, 1) != 0) {
                    msg = 0;
                    if (data_0209f220[0] == 1) {
                        msg += (short)(pl->param1 + 0x9a);
                    } else {
                        msg = 0x95;
                    }
                    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
                    if (pl->ShowMessage(*(fBase_c *)c, (int)msg, (Vector3 *)&k->mPosX, 0, 0) != 0) {
                        func_02012694(0x12a, &k->mCamSpacePosX);
                        k->KingBobOmb_SetState(&data_ov078_0212705c);
                    }
                }
            }
            return 1;
        }

        if (k->mActionStep == 0) {
            func_02012694(0x128, &k->mCamSpacePosX);
            k->func_ov078_02125c24(0x7d0000);
            k->HugeLandingDust(1);
            k->mActionStep = 1;
        }
        if ((unsigned short)k->mStateTimer == 0) {
            k->mSpawnSlot = 0;
            k->KingBobOmb_SetState(&data_ov078_0212702c);
            return 1;
        }
        if (Vec3_Dist(&k->mArenaPosX, &k->mPosX) < 0x258000) {
            int r = RandomIntInternal(&data_0209e650);
            int d;
            r = ((unsigned)r >> 8) & 0xf;
            r <<= 0x1c;
            lim = 0x4000;
            d = r >> 0x10;
            r = -lim;
            if (d < r)
                d = r;
            else if (d > 0x4000)
                d = lim;
            {
                short *p = &k->mTargetAngY;
                *p = (short)(*p + d);
            }
        } else {
            k->mTargetAngY = Vec3_HorzAngle(&k->mPosX, &k->mArenaPosX);
        }
        k->mActionStep = 0;
        k->mVertSpeed = 0x1e000;
        k->mHorzSpeed = 0xa000;
    } else {
        if (k->mVertSpeed < 0) {
            unsigned int id = k->mdCcAcPos_c.otherOwner;
            if (id != 0) {
                dActor_c *a = dActor_c::FindWithID(id);
                if (a != 0) {
                    int b = (int)(a->actorID == 0xbf);
                    if (b != 0) {
                        *(struct M3 *)&v = *(struct M3 *)&a->mPosX;
                        if (k->mPosY > v.y)
                            ((Player *)a)->Unk_020c6a10(1);
                    }
                }
            }
        }
    }

    k->mAngleY = k->mPrevAngleY;
    ApproachAngle(&k->mPrevAngleY, k->mTargetAngY, 5, 0x1000, 0x200);
    return 1;
}
}

#pragma pop

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_021243c0Ev

int daBombking_c::func_ov078_021243c0()
{
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&this->mBlendModelAnim, (void*)data_ov078_02126f20.file, 0, 0, 0x1000, 0);
  this->mStateTimer = 0xc8;
  this->mTargetAngY = Vec3_HorzAngle(&this->mPosX, &this->mArenaPosX);
  *(int*)(((long long)(int)((char *)this + 0x500))) =
    *(int*)(((long long)(int)((char *)this + 0x500))) - 1;
  this->mActionStep = 0;
  this->mVertSpeed = 0x28000;
  this->mHorzSpeed = 0x5000;
  if (this->mHealth <= 0) this->mFlags = 0x10000000;
  this->mVertAccel = -0x2000;
  return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124470Ev

int daBombking_c::func_ov078_02124470()
{
    s16 ang = this->HorzAngleToCPlayer();
    ApproachAngle(&this->mPrevAngleY, ang, 1, 0x500, 0x500);
    this->mAngleY = this->mPrevAngleY;
    if ((u16)this->mStateTimer == 0)
        KingBobOmb_SetState(&data_ov078_0212702c);
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_021244d0Ev

int daBombking_c::func_ov078_021244d0()
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&this->mBlendModelAnim, (void*)data_ov078_02126f20.file, 0, 0, 0x1000, 0);
    this->mStateTimer = 0x32;
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124520Ev

int daBombking_c::func_ov078_02124520()
{
    Vector3 a;
    Vector3 look;
    Vector3 pos;
    Vector3 in;
    Vector3 out;
    Vector3 ppos;
    Vector3 tmp;
    Vector3 dust;
    dCamera_c *cam;
    Player *player;
    int *pp;
    s16 ang;

    cam = (dCamera_c *)data_0209f318;
    if (((dExtFrameCtrl_c *)&this->mBlendModelAnim)->WillHitFrame(0x46)) {
        func_ov078_02125c24(0x7d0000);
        func_02012694(0x12c, &this->mCamSpacePosX);
        a.x = 0;
        a.y = 0;
        a.z = 0;
        data_020a0e68 = *(struct M12 *)&this->mBlendModelAnim.mat4x3;
        MulMat4x3Mat4x3((char *)this->mBlendModelAnim.data.transforms + 0x120, &data_020a0e68, &data_020a0e68);
        a.x = data_020a0e68.w[9];
        a.y = data_020a0e68.w[10];
        a.z = data_020a0e68.w[11];
        Vec3_Lsl(&tmp, &a, 3);
        a.x = tmp.x;
        dust.x = tmp.x;
        a.y = tmp.y;
        dust.y = tmp.y;
        a.z = tmp.z;
        dust.z = tmp.z;
        this->HugeLandingDustAt(dust, 1);
    }
    player = this->mTalkingPlayer;
    if (player->GetTalkState() != -1) {
        pp = (int *)((int)player + 0x5c);
        in.x = 0;
        in.y = 0;
        in.z = 0;
        out.x = 0;
        out.y = 0;
        out.z = 0;
        ppos.x = pp[0];
        ppos.y = pp[1];
        ppos.z = pp[2];
        ang = Vec3_HorzAngle((Vector3 *)&this->mPosX, &ppos);
        look.x = this->mPosX;
        look.y = this->mPosY;
        look.z = this->mPosZ;
        pos.x = this->mPosX;
        pos.y = this->mPosY;
        pos.z = this->mPosZ;
        look.y += 0x100000;
        in.z = 0x800000;
        Matrix4x3_FromRotationY(&data_020a0e68, ang);
        MulVec3Mat4x3(&in, &data_020a0e68, &out);
        pos.y += 0x100000;
        pos.x += out.x;
        pos.z += out.z;
        cam->SetLookAt(look);
        cam->SetPos(pos);
        ApproachLinear(this->mAngleY, ang, 0x800);
        this->mPrevAngleY = this->mAngleY;
        return 1;
    }
    cam->mFlags &= ~8;
    if (((dExtFrameCtrl_c *)&this->mBlendModelAnim)->Finished()) {
        KingBobOmb_SetState(&data_ov078_0212703c);
    }
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124778Ev

int daBombking_c::func_ov078_02124778()
{
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&this->mBlendModelAnim, data_ov078_02126ef8.file, 8, 0x40000000, 0x1000, 0);
  return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_021247bcEv

int daBombking_c::func_ov078_021247bc()
{
    short horz;
    short vert;
    Vec3 vA;
    Vec3 vB;
    Vec3 vC;
    Vec3 D;
    Vec3 E;

    if (this->mActionStep != 2) {
        horz = Vec3_HorzAngle(&this->mPosX, &this->mArenaPosX);
        vert = Vec3_VertAngle(&this->mPosX, &this->mArenaPosX);
        Vec3_Dist(&this->mPosX, &this->mArenaPosX);
        vA.x = 0;
        vA.y = 0;
        vA.z = 0;
        ApproachAngle(&this->mPrevAngleX, vert, 5, 0x1000, 0x300);
        ApproachAngle(&this->mPrevAngleY, horz, 5, 0x1000, 0x300);
        this->mAngleY = this->mPrevAngleY;
        if (((dExtFrameCtrl_c *)&this->mBlendModelAnim)->Finished() == 0)
            return 1;
        vB.x = 0;
        vB.y = 0x3c000;
        vB.z = 0x28000;
        vC.x = 0;
        vC.y = 0;
        vC.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, this->mPrevAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, this->mPrevAngleX);
        MulVec3Mat4x3((Vector3 *)&vB, &data_020a0e68, (Vector3 *)&vC);
        this->unk_0a4 = vC.x;
        if (this->mActionStep == 0) {
            this->mVertSpeed = vC.y;
            if (this->mVertSpeed > 0x3c000)
                this->mVertSpeed = 0x3c000;
            else if (this->mVertSpeed < 0x1e000)
                this->mVertSpeed = 0x1e000;
        }
        this->unk_0ac = vC.z;
        /* Address launder: mVertSpeed += mVertAccel reschedules this arm. */
        *(int *)(((int)this + 0xa8)) += this->mVertAccel;
        vA.x = this->mPosX;
        vA.y = this->mPosY;
        vA.z = this->mPosZ;
        vA.y = this->mArenaPosY;
        if (Vec3_Dist(&vA, &this->mArenaPosX) > 0x640000)
            return 1;
        this->mActionStep = 1;
    }

    if (this->mWithMeshClsn.IsOnGround() != 0) {
        Player *pl = (Player *)this->ClosestPlayer();
        if (pl != 0) {
            *(struct M3 *)&D = *(struct M3 *)&pl->mPosX;
            if (Vec3_Dist(&this->mArenaPosX, &D) > 0x640000 ||
                this->mArenaPosY - 0x64000 > D.y) {
                this->unk_0a4 = 0;
                this->mVertSpeed = 0;
                this->unk_0ac = 0;
                KingBobOmb_SetState(&data_ov078_021270fc);
                return 1;
            }
        }

        if (this->mActionStep == 1) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&this->mBlendModelAnim, *(void **)((char *)&data_ov078_02126f08 + 4), 0, 0x40000000, 0x1000, 0);
            this->mVertAccel = -0x2000;
            this->mFlags = 0x10000002;
            func_02012694(0x12c, &this->mCamSpacePosX);
            func_ov078_02125c24(0xfa0000);
            func_0200fa8c(this, 1);
            this->mActionStep = 2;
            this->unk_0a4 = 0;
            this->mVertSpeed = 0;
            this->unk_0ac = 0;
            return 1;
        }

        if (((dExtFrameCtrl_c *)&this->mBlendModelAnim)->Finished() != 0) {
            Player *pl2 = this->mTalkingPlayer;
            unsigned short m;
            E.x = this->mPosX;
            E.y = this->mPosY;
            E.z = this->mPosZ;
            E.y += 0xc8000;
            m = pl2->mStateFlags & 0x800;
            if (m == 0) {
                if (pl2->ShowMessage(*(fBase_c *)this, 0x94, (Vector3 *)(&E), 0, 0) != 0) {
                    ((dCamera_c *)data_0209f318)->SetFlag_3();
                    func_02012694(0x12a, &this->mCamSpacePosX);
                    KingBobOmb_SetState(&data_ov078_021270dc);
                }
            }
        }
    }

    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124b40Ev

int daBombking_c::func_ov078_02124b40()
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&this->mBlendModelAnim, (void *)data_ov078_02126f10.file, 0, 0x40000000, 0x1000, 0);
    func_02012694(0x129, &this->mCamSpacePosX);
    this->mWithMeshClsn.ClearGroundFlag();
    this->mPrevAngleX = 0;
    this->mHorzSpeed = 0;
    this->unk_0a4 = 0;
    this->mVertSpeed = 0;
    this->unk_0ac = 0;
    this->mActionStep = 0;
    this->mVertAccel = -0x6000;
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124bc4Ev

int daBombking_c::func_ov078_02124bc4()
{
    if ((unsigned short)this->mStateTimer != 0) return 1;
    if (*(int *)&data_02092138 > this->mPosY) {
        KingBobOmb_SetState(data_ov078_021270bc);
        return 1;
    }
    if (this->mWithMeshClsn.IsOnGround() != 0) {
        if (this->mActionStep == 0) {
            func_ov078_02125c24(0x7d0000);
            func_0200fa8c(this, 1);
            func_02012694(0x128, &this->mCamSpacePosX);
            this->mStateTimer = 5;
            this->mVertSpeed = 0x14000;
            this->mHorzSpeed = 0xa000;
            this->mActionStep = 1;
        } else {
            KingBobOmb_SetState(data_ov078_021270bc);
        }
    }
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124c94Ev

int daBombking_c::func_ov078_02124c94()
{
    this->mVertAccel = -0x2000;
    this->mVertSpeed = 0x1e000;
    this->mHorzSpeed = 0x14000;
    this->mActionStep = 0;
    this->mStateTimer = 5;
    this->mFlags = 0x10000000;
    func_ov078_02125c24(0xfa0000);
    func_0200fa8c(this, 1);
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124cf4Ev
int daBombking_c::func_ov078_02124cf4()
{
    if (this->mWithMeshClsn.IsOnGround() == 0) goto done;
    this->mHorzSpeed = 0;
    if ((int)(this->mArenaPosY - 0x28000) > this->mPosY) {
        func_02012694(0x128, &this->mCamSpacePosX);
        KingBobOmb_SetState(&data_ov078_021270ac);
        goto done;
    }
    if (this->mActionStep == 0) {
        func_02012694(0x128, &this->mCamSpacePosX);
        func_02012694(0x12b, &this->mCamSpacePosX);
        func_ov078_02125c24(0x7d0000);
        func_0200fa8c(this, 1);
        this->mActionStep = 1;
        this->mHealth -= 1;
    }
    if (this->mHealth > 0) {
        KingBobOmb_SetState(&data_ov078_021270ec);
        goto done;
    }
    Player *other = this->mTalkingPlayer;
    if (other == 0) goto done;
    if ((unsigned short)(other->mStateFlags & 0x800) != 0) goto done;
    if (other->StartTalk(*(fBase_c *)this, 1) == 0) goto done;

    short msg = 0;
    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
    if (data_0209f220[0] == 1) {
        msg += (short)(other->param1 + 0x9a);
    } else {
        msg = 0x95;
    }

    if (other->ShowMessage(*(fBase_c *)this, (unsigned)(int)msg, (Vector3 *)&this->mPosX, 0, 0) == 0) goto done;
    func_02012694(0x12a, &this->mCamSpacePosX);
    KingBobOmb_SetState(&data_ov078_0212705c);
done:
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124e9cEv

int daBombking_c::func_ov078_02124e9c()
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&this->mBlendModelAnim, (void *)data_ov078_02126f20.file, 0, 0, 0x1000, 0);
    if (this->mHealth <= 0)
        this->mFlags = 0x10000000;
    this->mWithMeshClsn.ClearGroundFlag();
    this->mActionStep = 0;
    this->mVertAccel = -0x2000;
    this->mVertSpeed = 0x28000;
    this->mHorzSpeed = 0x14000;
    this->mTerminalVelocity = -0x3c000;
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124f28Ev



int daBombking_c::func_ov078_02124f28()
{
    if (this->mActionStep == 0) {
        int b = (this->mFlags & 0x4000) != 0;
        if (b != 0) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&this->mBlendModelAnim, (void *)data_ov078_02126ee0.file, 0, 0, 0x1000, 0);
            this->mActionStep = 1;
        }
    }

    {
        Player *obj = this->mHeldActor;
        if (obj == 0) {
            KingBobOmb_SetState(&data_ov078_0212708c);
            return 1;
        }
        {
            struct Vec3 copy = *(struct Vec3 *)&obj->mPosX;
            if (Vec3_Dist(&this->mArenaPosX, &copy) > 0x640000)
                goto drop;
            if (this->mArenaPosY - 0xa000 <= copy.y)
                goto flags;
        }
    }

drop:
    this->mHeldActor->DropActor();
    this->mHeldActor = 0;
    KingBobOmb_SetState(&data_ov078_0212708c);
    return 1;

flags:
    {
        int flags = this->mFlags;
        int b0 = (flags & 0x400) != 0;
        if (b0 == 0) {
            int b1 = (flags & 0x2000) != 0;
            if (b1 == 0) {
                int b2 = (flags & 0x100) != 0;
                if (b2 != 0)
                    goto done;
            }
        }
        if (b0 != 0) {
            this->mPrevAngleY = this->mHeldActor->mAngleY;
        }
        {
            short t = this->mPrevAngleY;
            this->mAngleY = t;
        }
        this->mdCcAcPos_c.flags &= ~2;
        KingBobOmb_SetState(&data_ov078_0212708c);
        {
            Player *p = this->mHeldActor;
            if (p != 0) {
                if (p->param1 == 2) {
                    this->mVertSpeed = 0x32000;
                    this->mHorzSpeed = 0x1e000;
                }
            }
        }
        this->mHeldActor = 0;
    }
done:
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_021250d0Ev

int daBombking_c::func_ov078_021250d0()
{
    this->mdCcAcPos_c.flags |= 2;
    this->mVertAccel = 0;
    this->mHorzSpeed = 0;
    this->mActionStep = 0;
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_021250f8Ev

int daBombking_c::func_ov078_021250f8()
{
    struct Vector3 in, out, v[2];
    dActor_c *target;
    Player *player;

    if (func_ov078_02123804() == 1) {
        func_ov078_02123864();
        return 1;
    }

    if (this->mSpawnedThrown[this->mSpawnSlot] == 0) {
        target = dActor_c::FindWithID(this->mSpawnedId[this->mSpawnSlot]);
        if (target != 0) {
            target->mPosX = this->mThrowPosX;
            target->mPosY = this->mThrowPosY;
            target->mPosZ = this->mThrowPosZ;
            target->mPrevAngleX = this->mPrevAngleX;
            target->mPrevAngleY = this->mPrevAngleY;
            target->mPrevAngleZ = this->mPrevAngleZ;
            target->mAngleX = this->mAngleX;
            target->mAngleY = this->mAngleY;
            target->mAngleZ = this->mAngleZ;
            func_ov102_0214b384(target, 0x78);
            if (((dExtFrameCtrl_c *)&this->mBlendModelAnim)->WillHitFrame(0x13) != 0
                || func_ov078_02123804() == 1) {
                in.x = 0; in.y = 0; in.z = 0x28000;
                out.x = 0; out.y = 0; out.z = 0;
                v[0].x = 0; v[0].y = 0; v[0].z = 0;
                player = (Player *)this->ClosestPlayer();
                if (player != 0) {
                    int *q = (int *)((int)player + 0x5c);
                    v[1].x = q[0];
                    v[1].y = q[1];
                    v[1].z = q[2];
                    v[0].x = v[1].x - this->mThrowPosX;
                    v[0].y = v[1].y - this->mThrowPosY;
                    v[0].z = v[1].z - this->mThrowPosZ;
                    Matrix4x3_FromRotationY(&data_020a0e68,
                        _ZN4cstd5atan2E5Fix12IiES1_(v[0].x, v[0].z));
                    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68,
                        (short)(-_ZN4cstd5atan2E5Fix12IiES1_(v[0].y, Vec3_HorzLen(&v[0]))));
                    out.y += 0x14000;
                    func_ov102_0214ad14(target);
                    target->unk_0a4 = out.x;
                    target->mVertSpeed = out.y;
                    target->unk_0ac = out.z;
                    target->mHorzSpeed = 0x14000;
                    target->mVertAccel = -0x2000;
                    MulVec3Mat4x3(&in, &data_020a0e68, &out);
                }
                this->mSpawnedThrown[this->mSpawnSlot] = 1;
                *(int *)((char *)target + 0xc8) = 0;
            }
        }
    } else {
        if (func_ov078_02123804() == 1) {
            return 1;
        }
    }

    if (((dExtFrameCtrl_c *)&this->mBlendModelAnim)->Finished() != 0) {
        player = (Player *)this->ClosestPlayer();
        if (player != 0) {
            if (player->param1 != 3) {
                this->unk_504 = 0x64;
            }
        }
        KingBobOmb_SetState(&data_ov078_0212703c);
    }
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125350Ev

int daBombking_c::func_ov078_02125350()
{
    int i;
    int r;

    this->mAnimSpeed = 1;
    this->mVertAccel = -0x2000;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&this->mBlendModelAnim, (void*)(data_ov078_02126ef0.file), 0, 0x40000000, 0x1000, 0);

    this->mHorzSpeed = 0;

    for (i = 0; i < this->mPhase; i++) {
        if (this->mSpawnedId[i] == 0) {
            r = (int)dActor_c::Spawn(0xce, 2, *(Vector3 *)(&this->mThrowPosX), 0, this->mAreaId, -1);
            if (r != 0) {
                this->mSpawnedId[i] = *(int*)(r + 4);
                this->mSpawnSlot = i;
                return 1;
            }
        }
    }

    this->mActionStep = 0;
    return 1;
}


#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125448Ev

int daBombking_c::func_ov078_02125448()
{
    Player *p;
    dActor_c *a;
    struct Vector3 v;
    int isType;
    int index;
    int empty;
    int z;
    unsigned int id;
    int v4a0;
    int slot;

    if (this->unk_505 == 0) {
        func_02012694(0x12d, &this->mCamSpacePosX);
        this->unk_505 = 0xf;
    }
    if (func_ov078_02123804() == 1) {
        func_ov078_02123864();
        return 1;
    }

    p = (Player *)this->ClosestPlayer();
    if (p != 0) {
        *(struct M3 *)&v = *(struct M3 *)&p->mPosX;
        if (Vec3_Dist(&this->mArenaPosX, &v) > 0x640000
            || (this->mArenaPosY - 0xa000) > v.y) {
            this->mHorzSpeed = 0;
            KingBobOmb_SetState(data_ov078_021270fc);
            return 1;
        }

        ApproachAngle(&this->mPrevAngleY, this->HorzAngleToCPlayer(), 0xa, 0x200, 0x100);
        {
            s16 ang = this->mPrevAngleY;
            this->mAngleY = ang;
            this->mFlags &= ~0x80;
        }

        id = this->mdCcAcPos_c2.otherOwner;
        if (id != 0) {
            a = dActor_c::FindWithID(id);
            if (a != 0) {
                isType = (int)(a->actorID == 0xbf);
                if (isType != 0) {
                    if (AngleDiff(this->HorzAngleToCPlayer(), this->mAngleY) > 0x2800) {
                        if ((this->mdCcAcPos_c2.hitFlags & 0x1000) != 0) {
                            this->mFlags |= 0x80;
                            if (((Player *)a)->TryGrab(*(dActor_c *)this) != 0) {
                                this->mHeldActor = (Player *)a;
                                this->mHorzSpeed = 0;
                                KingBobOmb_SetState(data_ov078_0212707c);
                            }
                        }
                    } else if (func_ov002_020db5f4((char *)a, (char *)this) != 0) {
                        this->mHeldActor = (Player *)a;
                        this->mHorzSpeed = 0;
                        KingBobOmb_SetState(data_ov078_0212706c);
                    }
                }
            }
            return 1;
        }

        if (this->unk_504 == 0) {
            if (data_0209f220[0] == 1) {
                if (this->DistToCPlayer() < 0x3e8000) {
                    if (AngleDiff(this->HorzAngleToCPlayer(), this->mAngleY) < 0x2800) {
                        index = 0;
                        empty = index;
                        z = index;
                        for (; index < 2; index++) {
                            slot = this->mSpawnedId[index];
                            if (slot != 0) {
                                if (dActor_c::FindWithID((unsigned int)slot) == 0) {
                                    this->mSpawnedId[index] = z;
                                    this->mSpawnedThrown[index] = (unsigned char)z;
                                }
                            }
                            if (this->mSpawnedId[index] == 0) {
                                empty++;
                                if (empty == 2)
                                    this->mPhase ^= 3;
                            }
                        }
                        v4a0 = this->mPhase;
                        if ((v4a0 == 2 && this->mSpawnedId[1] == 0)
                            || (v4a0 == 1 && empty == 2)) {
                            KingBobOmb_SetState(data_ov078_0212704c);
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 1;
}


#pragma pop

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125734Ev

int daBombking_c::func_ov078_02125734()
{
    this->mAnimSpeed = 1;
    this->mHorzSpeed = 0x5000;
    this->mVertAccel = -0x2000;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&this->mBlendModelAnim, data_ov078_02126ee8.file, 8, 0, 0x1000, 0);
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125790Ev
int daBombking_c::func_ov078_02125790()
{
  Vector3 s;
  Vector3 d;
  Vector3 v;
  if (func_ov078_02123804() == 1) return 1;
  ApproachAngle(&this->mPrevAngleY, this->HorzAngleToCPlayer(), 1, 0x500, 0x500);
  this->mAngleY = this->mPrevAngleY;
  if (((dExtFrameCtrl_c *)&this->mBlendModelAnim)->WillHitFrame(0x46)) {
    func_ov078_02125c24(0x7d0000);
    func_02012694(0x12c, &this->mCamSpacePosX);
    s.x = 0;
    s.y = 0;
    s.z = 0;
    data_020a0e68 = *(struct M12 *)&this->mBlendModelAnim.mat4x3;
    MulMat4x3Mat4x3((char *)this->mBlendModelAnim.data.transforms + 0x120, &data_020a0e68, &data_020a0e68);
    s.x = data_020a0e68.w[9];
    s.y = data_020a0e68.w[10];
    s.z = data_020a0e68.w[11];
    Vec3_Lsl(&d, &s, 3);
    s.x = d.x;
    v.x = d.x;
    s.y = d.y;
    v.y = d.y;
    s.z = d.z;
    v.z = d.z;
    this->HugeLandingDustAt(v, 1);
  }
  if (((dExtFrameCtrl_c *)&this->mBlendModelAnim)->Finished()) {
    KingBobOmb_SetState(&data_ov078_0212703c);
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_021258e4Ev

int daBombking_c::func_ov078_021258e4()
{
    this->mVertAccel = -0x2000;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&this->mBlendModelAnim, data_ov078_02126ef8.file, 8, 0x40000000, 0x1000, 0);
    if (this->mMusicLayer3 == 0) {
        this->mMusicLayer3 = 1;
        this->mFlags = 0x10000002;
    }
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125950Ev
int daBombking_c::func_ov078_02125950()
{
    Player *target = this->mTalkingPlayer;
    int *src = (int *)((int)target + 0x5c);
    Vector3 v;
    int t = src[0];
    v.x = t;
    v.y = src[1];
    v.z = src[2];
    short ang = Vec3_HorzAngle(&this->mPosX, &v);
    ApproachLinear(this->mAngleY, ang, 0x800);
    this->mPrevAngleY = this->mAngleY;
    if (target->GetTalkState() == -1) {
        Message::EndTalk();
        func_02011d44();
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
        KingBobOmb_SetState(&data_ov078_0212701c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_021259e4Ev

int daBombking_c::func_ov078_021259e4()
{
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_021259ecEv

int daBombking_c::func_ov078_021259ec()
{
    int dist;
    struct Vector3 ppos;
    struct Vector3 v;
    Player *player;
    unsigned int b;

    if (this->mMusicLayer3 == 1) {
        unsigned char *p = (unsigned char *)((char *)this + 0x50a);
        *p += 1;
        if (*(unsigned char *)((char *)this + 0x50a) > 0xc8) {
            this->mFlags = 0x10000003;
            this->mMusicLayer3 = 0;
            *(unsigned char *)((char *)this + 0x50a) = 0;
            this->mHealth = 3;
            _ZN5Sound22StopLoadedMusic_Layer3Ev();
            func_02011cfc();
            this->mIntroTalked = 0;
        }
    }

    ApproachLinear(this->mAngleY, this->mInitAngleY, 0x800);
    this->mPrevAngleY = this->mAngleY;

    if (func_ov078_02123804() == 1) {
        return 1;
    }

    player = (Player *)this->ClosestPlayer();
    if (player != 0) {
        *(struct M3 *)&ppos = *(struct M3 *)&player->mPosX;

        if (this->mIntroTalked == 0) {
            dist = Vec3_Dist((struct Vector3 *)&this->mPosX, &ppos);
            if (dist < 0x12c000) {
                this->mTalkingPlayer = player;
                v.x = this->mPosX;
                v.y = this->mPosY;
                v.z = this->mPosZ;
                v.y = v.y + 0xc8000;

                if (this->mTalkingPlayer->StartTalk(*(fBase_c *)this, 1)) {
                    Message::PrepareTalk();
                    b = (data_0209f220[0] != 1) ? 0x93 : (unsigned int)(short)(player->param1 + 0x96);
                    if (player->ShowMessage(*(fBase_c *)this, b, (Vector3 *)(&v), 0, 0)) {
                        func_02012694(0x12a, &this->mCamSpacePosX);
                        this->mIntroTalked = 1;
                        KingBobOmb_SetState(&data_ov078_0212700c);
                    }
                }
            }
        } else {
            dist = Vec3_Dist((struct Vector3 *)&this->mPosX, &ppos);
            if (dist < 0x258000) {
                if (this->mArenaPosY - 0xa000 < ppos.y) {
                    KingBobOmb_SetState(&data_ov078_0212701c);
                }
            }
        }
    }

    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125bc8Ev
int daBombking_c::func_ov078_02125bc8() {
    this->mHorzSpeed = 0;
    this->mAnimSpeed = 1;
    this->mVertAccel = -0x2000;
    this->unk_50a = 0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&this->mBlendModelAnim, (void*)data_ov078_02126f30.file, 8, 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125c24Ei
void daBombking_c::func_ov078_02125c24(int strength) {
    func_0200d8c8(data_0209f318, &this->mPosX, strength);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19KingBobOmb_SetStateEPv
int daBombking_c::KingBobOmb_SetState(void *pv) {
    mState = (PMF *)pv;
    PMF *q = mState;
    if (*q == 0) return 1;
    return (this->**q)();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125c98Ev
void daBombking_c::func_ov078_02125c98() {
  int h = mPosY;
  if (this->mWithMeshClsn.IsOnGround() == 0) {
    dBgCh_Gnd rg;
    rg.SetObjAndPos(*(const Vector3 *)&this->mPosX, 0);
    if (rg.DetectClsn() != 0)
      h = rg.clsnY;
  }
  int b = (this->mFlags & 0x4000) != 0;
  if (b) {
    Player *p = this->mHeldActor;
    if (p != 0)
      h = p->mPosY;
  }
  int ip = this->mPosY - h;
  if (ip <= 0x1000)
    ip = 0x1000;
  int scale = 0x15e000 - (int)(((long long)ip * 0x180 + 0x800) >> 12);
  if (scale < 0xa000)
    scale = 0xa000;
  *(struct M12 *)this->mShadowMtx = *(struct M12 *)&IDENTITY_MATRIX4X3;
  this->mShadowMtx[9] = this->mPosX >> 3;
  this->mShadowMtx[10] = this->mPosY >> 3;
  this->mShadowMtx[11] = this->mPosZ >> 3;
  _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
      this, &this->mShadowModel, this->mShadowMtx, scale, ip + 0x28000, 0xf);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125de0Ev
void daBombking_c::func_ov078_02125de0()
{
    struct Vector3 lv;
    int i;

    Matrix4x3_FromRotationY(&this->mBlendModelAnim.mat4x3, this->mAngleY);
    this->mBlendModelAnim.mat4x3.m[9] = this->mPosX >> 3;
    this->mBlendModelAnim.mat4x3.m[10] = this->mPosY >> 3;
    this->mBlendModelAnim.mat4x3.m[11] = this->mPosZ >> 3;
    MulMat4x3Mat4x3((char *)this->mBlendModelAnim.data.transforms + 0x1e0,
                    &this->mBlendModelAnim.mat4x3, this->mHoldMtx);

    /* Held-matrix pointer. It sits in dActor_c's pad at 0xc8. */
    *(void **)((char *)this + 0xc8) = this->mHoldMtx;
    this->mThrowPosX = 0;
    this->mThrowPosY = 0;
    this->mThrowPosZ = 0;
    this->mThrowPosX = this->mHoldMtx[9];
    this->mThrowPosY = this->mHoldMtx[10];
    this->mThrowPosZ = this->mHoldMtx[11];
    Vec3_Lsl(&lv, &this->mThrowPosX, 3);
    this->mThrowPosX = lv.x;
    this->mThrowPosY = lv.y;
    this->mThrowPosZ = lv.z;

    for (i = 0; i < 2; i++) {
        unsigned int id;
        void *actor;

        if (this->mSpawnedThrown[i] != 0)
            continue;
        id = (unsigned int)this->mSpawnedId[i];
        if (id == 0)
            continue;
        actor = (void *)dActor_c::FindWithID(id);
        if (actor == 0)
            continue;

        data_020a0e68 = *(struct M12 *)this->mHoldMtx;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0xb000, -0x7000, -0x6000);
        Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, 0x4000, -0x1000, 0x2000);
        *(struct M12 *)this->mHoldMtx = data_020a0e68;
        *(void **)((char *)actor + 0xc8) = this->mHoldMtx;
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125f8cEv


void daBombking_c::func_ov078_02125f8c(){
    Player *player = mHeldActor;
    int idx = 0;
    if (player == 0) return;
    if (player->param1 == 2) idx = 1;
    Matrix4x3 *res = this->UpdateCarry(*player, data_ov078_0212711c[idx]);
    *(struct M12 *)&this->mBlendModelAnim.mat4x3 = *(struct M12 *)res;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c16CleanupResourcesEv
int daBombking_c::CleanupResources()
{
    ((SharedFilePtr *)&data_ov078_02126f38)->Release();
    ((SharedFilePtr *)&data_ov078_02126f00)->Release();
    ((SharedFilePtr *)&data_ov078_02126f20)->Release();
    ((SharedFilePtr *)&data_ov078_02126f10)->Release();
    ((SharedFilePtr *)&data_ov078_02126f08)->Release();
    ((SharedFilePtr *)&data_ov078_02126f18)->Release();
    ((SharedFilePtr *)&data_ov078_02126ee0)->Release();
    ((SharedFilePtr *)&data_ov078_02126ef0)->Release();
    ((SharedFilePtr *)&data_ov078_02126f40)->Release();
    ((SharedFilePtr *)&data_ov078_02126f30)->Release();
    ((SharedFilePtr *)&data_ov078_02126ee8)->Release();
    ((SharedFilePtr *)&data_ov078_02126f28)->Release();
    ((SharedFilePtr *)&data_ov078_02126ef8)->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c16OnPendingDestroyEv
void daBombking_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c6RenderEv
int daBombking_c::Render()
{
    void *held = mHeldActor;
    if (held != 0) {
        int flags = mFlags;
        int flag = (flags & 0x4000) ? 1 : 0;
        if (flag != 0) {
            if (*(int *)((char *)held + 0xc8) != 0) {
                func_ov078_02125f8c();
            }
        }
    }
    mBlendModelAnim.Model::Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c8BehaviorEv


extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
}

int daBombking_c::Behavior()
{
    if (DistToCPlayer() < 0x1770000) {
        *(daBombking_c **)((char *)data_0209f318 + 0x114) = this;
    }

    if (*(void **)((char *)mState + 8) != 0) {
        PMF *p = mState + 1;
        (this->**p)();
    }

    mBlendModelAnim.speed = mAnimSpeed << 0xc;
    mBlendModelAnim.UpdateVerts();
    mBlendModelAnim.Advance();

    if ((char *)mState == (char *)data_ov078_0212707c) {
        void *held = mHeldActor;
        int flag;
        if (held != 0) {
            flag = (mFlags & 0x4000) != 0;
            if (flag != 0 && *(int *)((char *)held + 0xc8) != 0) {
                goto skip_de0;
            }
        }
        func_ov078_02125de0();
    skip_de0:
        func_ov078_02125c98();
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    DecIfAbove0_Byte(&unk_505);
    DecIfAbove0_Byte(&unk_504);

    if ((char *)mState != (char *)data_ov078_021270bc) {
        UpdatePos(&mdCcAcPos_c);
    } else {
        UpdatePosWithOnlySpeed(&mdCcAcPos_c);
    }

    if ((char *)mState != (char *)data_ov078_021270bc || mActionStep == 1) {
        UpdateWMClsn(mWithMeshClsn, 0);
    }

    if ((char *)mState == (char *)data_ov078_0212703c || (char *)mState == (char *)data_ov078_021270fc) {
        if (mWithMeshClsn.IsOnWall() != 0
            || mWithMeshClsn.IsOnGround() == 0
            || (mArenaPosY - 0x28000) > mPosY) {
            KingBobOmb_SetState(data_ov078_021270bc);
        }
    }

    {
        Vector3 v;
        v.x = data_ov078_02126e00.x;
        v.y = data_ov078_02126e00.y;
        v.z = data_ov078_02126e00.z;
        mdCcAcPos_c.SetPosRelativeToActor(v);
    }
    {
        Vector3 v;
        v.x = data_ov078_02126e00.x;
        v.y = data_ov078_02126e00.y;
        v.z = data_ov078_02126e00.z;
        mdCcAcPos_c2.SetPosRelativeToActor(v);
    }
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    mdCcAcPos_c2.Clear();
    mdCcAcPos_c2.Update();

    func_ov078_02125de0();
    func_ov078_02125c98();
    return 1;
}

#pragma push
#pragma opt_strength_reduction off
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c13InitResourcesEv
/* SharedFilePtr's complete declaration is included above. The file handles
   below still use their existing ROM-backed data declarations. */
int daBombking_c::InitResources()
{
    BMD_File *f;
    Vector3 v0;
    Vector3 v1;
    int i;
    f = (BMD_File *)Model::LoadFile(*(SharedFilePtr *)&data_ov078_02126f38);
    mBlendModelAnim.SetFile(f, 1, 1);
    mShadowModel.InitCylinder();
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov078_02126f00);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov078_02126f20);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov078_02126f10);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov078_02126f08);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov078_02126f18);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov078_02126ee0);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov078_02126ef0);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov078_02126f40);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov078_02126f30);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov078_02126ee8);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov078_02126f28);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov078_02126ef8);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    v0.x = data_ov078_02126e00.x;
    v0.y = data_ov078_02126e00.y;
    v0.z = data_ov078_02126e00.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &v0, 0x78000, 0xc8000, 0x200004, 0x206000);
    v1.x = data_ov078_02126e00.x;
    v1.y = data_ov078_02126e00.y;
    v1.z = data_ov078_02126e00.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c2, this, &v1, 0xc8000, 0xc8000, 0x200000, 0x207000);
    unk_498 = 0x1f;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mArenaPosX = 0xb1d000;
    mArenaPosY = 0x1060000;
    mArenaPosZ = 0xfee15000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x190000, 0x190000, 0, 0);
    func_02035550(&mWithMeshClsn);
    mAnimSpeed = 1;
    mHealth = 3;
    mStarID = param1 & 0xf;
    mStarTracked = TrackStar(mStarID, 2);
    {
    int z = 0;
    for (i = 0; i < 2; i++) {
        mSpawnedId[i] = z;
        mSpawnedThrown[i] = (unsigned char)z;
    }
    }
    mPhase = ((unsigned int)RandomIntInternal(&data_0209e650) >> 0x1e) & 1;
    mPhase = mPhase + 1;
    mInitAngleY = mAngleY;
    KingBobOmb_SetState(&data_ov078_0212710c);
    return 1;
}

#pragma pop

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c16OnAimedAtWithEggEv
// recovered name: KingBobOmb_OnAimedAtWithEgg
/* daBombking_c::OnAimedAtWithEgg - recovered from vtable slot identity */
s32 daBombking_c::OnAimedAtWithEgg() {
    return 1024000;
}

/* -------------------------------------------------------------------------- */
/* The factory immediately follows ordinal 50 and ends at the .init boundary. */
// @symbol daBombking_c_classInit
/* The registry factory behind the BOMBKING profile. `return new daBombking_c()`
 * MATCHES (size 0x64); the synthesized ctor stores `_ZTV12daBombking_c + 2`. */
extern "C" daBombking_c *daBombking_c_classInit(void)
{
    return new daBombking_c();
}

struct BombkingSpawnInfo {
    daBombking_c *(*classInit)();
    s16 executePriority; /* +4: also BOMBKING registry id 0x00bd = 189 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char BombkingSpawnInfo_size_must_be_0x1c[
    sizeof(BombkingSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_BOMBKING
extern "C" BombkingSpawnInfo g_profile_BOMBKING = {
    daBombking_c_classInit,
    0x00bd,
    0x002c,
    0x10000003,
    0x00000000,
    0x00190000,
    0x01fa0000,
    0x01fa0000
};
