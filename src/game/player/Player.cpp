//cpp
/* Mario, Luigi, Wario and Yoshi. ov002, 0x020bccfc..0x020e6a08, plus
 * __sinit_ov002_021019d0. ~Player() is the key function, compiled with RTTI
 * off: retail typeinfo is _ZTI7daPly_c and this file emits no _ZTS.
 * defer_codegen stays off except around ~Player(), where it is on so D0
 * lands before D1.
 * Local optimization pragmas and some copy/reload forms are load-bearing.
 * Evidence: config/tu_manifest.d/ov002/Player.json, notes/data/class-facts/Player.json; measured limits: notes/experiments/player-readability-1014.json.
 *
 * Leftover:
 * - Sound::PlayBank0(0xb9, c + 0x74) still passes the camera-space vector
 *   as a raw pointer. The following mStateArg store is an int-cast of the
 *   address; the member spelling of that store is short.
 * - One ChangeState passes (int)data_ov002_021101b4.
 * - Particle::System::New, Particle::System::NewSimple, cstd::atan2,
 *   Sound::PlaySub and Sound::ChangeMusicVolume stay mangled. The symbol
 *   carries 5Fix12IiE. Sound.h does not declare the last two.
 *   All 16 body-local declarations of Particle::System::New use ONE spelling:
 *       u32 (u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z,
 *            const void *dir, void *callback)
 *   The mangled name gives the real types of the last two -- const Vector3_16f *
 *   and Particle::Callback * -- but the call sites pass several different
 *   three-by-s16 shapes, so they stay void pointers until those are unified.
 *   Fix12i is a scalar typedef, so it costs nothing here; the by-value
 *   Fix12<int> the symbol names is still the wall (notes/mwccarm-codegen.md
 *   6az). Keep them identical: 16 spellings of one symbol is how a wrong
 *   return type or argument order hides.
 * - func_020731dc is __register_global_object. The relocation rows name the
 *   retail symbol.
 * - The static initializer is still written out by hand. Its objects keep
 *   their ROM-owned .bss homes, so it constructs each handle in place and
 *   registers its destructor with __register_global_object, as the
 *   compiler-emitted one did.
 * - Helpers that take the player as char * or void * and cast it stay free
 *   functions, including func_ov002_020e6780 (decl_common.h spells the
 *   parameter char *) and func_ov002_020bcdf0. func_ov002_020e69c0 rotates
 *   data_ov002_0210a760, which has no recovered name.
 * - Player:: methods with no body-local externs carry no extern "C"
 *   wrapper (60 sites); the rest keep it: the block gives C linkage to
 *   body-local externs, and hoisting them exposes S42 spelling drift.
 */

#pragma defer_codegen off

#include "types.h"
#include "common.h"
#include "Player.h"
#include "dExtAnm2Model_c.h"
#include "PlayerInput.h"
#include "SharedFilePtr.h"
#include "PathPtr.h"
#include "Particle__System.h"
#include "SurfaceInfo.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Lin.h"
#include "dBgCh_Actr.h"
#include "Sound.h"
#include "dCamera_c.h"
#include "SaveData.h"
#include "Timer.h"
#include "daObj1UpLogo_c.h"
#include "daOwl_c.h"
#include "daShl_c.h"
#include "daSoundObj_c.h"
#include "daTor_c.h"
#include "daYegg_c.h"
#include "daObjKey_c.h"
#include "daStar_c.h"
#include "dScene_c.h"
#include "nitro/fx/fx.h"
#include "G2x.h"
#include "Memory.h"
#include "cstd.h"
#include "math/Approach.h"

/* ---------------------------------------------------------------------------
 * Shadow types and ABI stand-ins that cannot live inside a function body.
 *
 * Everything here is either hoisted for the C++ rule spelled out further down
 * or load-bearing for codegen.  It is NOT a parking lot for POD twins of real
 * types: Vector3 is usable directly, including its (x, y, z) constructor, as
 * long as the locals keep the ROM's declaration order.  The exception is the
 * one include/types.h records at Vector3's constructors: a whole-struct copy
 * can lower memberwise instead of as a block move, and there a twin can still
 * be load-bearing.
 * ------------------------------------------------------------------------- */
struct V3Quad { Vector3 a; Vector3 d; Vector3 e; Vector3 b; };
/* A stand-in for Fix12<int>, and load-bearing as a CLASS rather than as a
 * name: func_ov002_020bf800 builds three of these and passes each as an int
 * read through its address. The ROM stores x, y and z to sp+0xc..0x14 and
 * reloads x and y from there for the call (z goes out from a register). Plain Fix12i
 * locals -- or passing xv/yv/zv straight to the callee -- keep all three in
 * registers and shrink that function from 140 bytes to 120. Measured both
 * ways; it stays until Fix12<int> itself is reconstructed. */
struct Fix12v { int v; Fix12v(int a) : v(a) {} };
namespace Sound { void PlayBank0(unsigned int, const Vector3 &); }
struct Ret { int pad; int y; };
/* A shadow scaffold, and the one hoist C++ requires even without the extern
 * rule below: every virtual of a LOCAL class must be defined in the same body,
 * because a local class vtable can have nowhere else to come from.  Arg is
 * never constructed -- it exists only to give the call in
 * func_ov002_020caf98 the ROM vtable-slot shape -- so at file scope the
 * declarations are enough and no vtable is emitted on either compiler. */
struct Arg {
 virtual void *m0();
 virtual void *m1();
 virtual struct Ret *m2();
};
/* Only ever an opaque pointer: the camera's extern, and func_ov002_020ceb7c's
 * parameter, which is a Player (it reads mPosY, mPlayerNo and mStateWork). */
struct CameraRaw;
/* The rest of this block is the same hoist for the same reason, and the tags
 * are unique in the same way.  A body may not declare `extern` with a
 * function-local class type: a local class has no linkage, so the declaration
 * names an entity the type cannot describe.  mwccarm accepts it; the C++ rule
 * ([basic.link]) does not, and neither does the host front end.  Each shape
 * below is the body's own, moved out unchanged, and the body keeps its own
 * declaration of the object. */
struct Entry18 { s16 val; /* 0x0 */ char _pad[0x16]; };
struct Anim2fec { short a; short x; short y; short z; };
struct ObjTeleport { s16 x, y, z; u16 param; };
struct State2i { int a; int b; };
struct S18a { short f0; char pad[0x16]; };
struct S18b { u16 f0; char pad[0x16]; };
struct S18b2 { short a; short b; int c[5]; };
/* The Kuppa-script command table at the head of the file holds pointers to
 * members of one signature. */
typedef int (Player::*ScriptCommand)(unsigned char *, int, int);

// @symbol _ZN6Player19func_ov002_020bccfcEPhii
int Player::func_ov002_020bccfc(unsigned char *params, int beginFrame, int endFrame)
{
    this->MarkForDestruction();
    return 1;
}

// @symbol _ZN6Player19func_ov002_020bcd18EPhii
extern "C" {
int Player::func_ov002_020bcd18(unsigned char *params, int beginFrame, int endFrame)
{
    extern int Player_ScaleByCharFactor(void *, int);
    mHorzSpeed = Player_ScaleByCharFactor(this, 0xa000);
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020bcd38EPhii
extern "C" {
int Player::func_ov002_020bcd38(unsigned char *params, int beginFrame, int endFrame)
{
    extern s16 ReadUnalignedShort(u8 *p);
    extern int AngleDiff(int a, int b);
    extern int ApproachAngle(s16 *angle, int from, int start, int speed, int max);
    extern u16 gKuppaScriptFrame;
    s16 cur;

    if (beginFrame == gKuppaScriptFrame) {
        int dt = endFrame - beginFrame;
        this->mAngleYSpeed = (short)AngleDiff(this->mAngleY, ReadUnalignedShort(params)) / dt;
        this->mStateWork = (u8)dt;
        this->mAnimScriptId = 0;
        this->mAnimScriptStep = 0;
        this->SetAnim(0x48, 0, 0x1000, 0);
    }

    cur = this->mAngleYSpeed;
    ApproachAngle(&this->mAngleY, ReadUnalignedShort(params), this->mStateWork, cur, cur);
    this->mPrevAngleY = this->mAngleY;
    return 1;
}
}

/* Free C-linkage script-animation update on a Player. The array-backed
 * entry retains the retail two-word stack snapshot under 2004/b56 C++.
 * w[0] is the animation ID; w[1] is the signed animation parameter.
 * Scalar SetAnim/particle ABI calls and the raw spawned-prop view remain. */
extern "C" {
typedef struct { int w[2]; } AnimEntry27;

// @symbol func_ov002_020bcdf0
void func_ov002_020bcdf0(char *self)
{
    Player *player = reinterpret_cast<Player *>(self);
    extern AnimEntry27 *data_ov002_0210a7a0[];
    extern void PlayerNewSimpleHold(unsigned int id, int x, int y, int z);
    extern u32 PlayerNewHold(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                             void *callback);
    AnimEntry27 *base;
    AnimEntry27 cur;
    int id;
    dExtFrameCtrl_c *anim;
    unsigned int frame;
    struct { int x, y, z; } vv[2];
    int x0, y0, z0;

    if (player->mAnimScriptId == 0)
        return;

    base = data_ov002_0210a7a0[player->mAnimScriptId - 1];
    cur = base[player->mAnimScriptStep];

    ((Player *)self)->SetAnim(cur.w[0], cur.w[1], 0x1000, 0);

    if (cur.w[0] == 0xbd) {
        id = player->GetBodyModelID(player->param1 & 0xff, 0);
        anim = &static_cast<dExtFrameCtrl_c &>(*player->mModels.mBodyModels[id]);
        if (anim->WillHitFrame(0xc)) {
            player->mHasNoCap = 1;
        }
    } else if (cur.w[0] == 1) {
        id = player->GetBodyModelID(player->param1 & 0xff, 0);
        anim = &static_cast<dExtFrameCtrl_c &>(*player->mModels.mBodyModels[id]);
        if (anim->WillHitFrame(0x5d)) {
            player->mHasNoCap = 0;
        } else {
            id = player->GetBodyModelID(player->param1 & 0xff, 0);
            anim = &static_cast<dExtFrameCtrl_c &>(*player->mModels.mBodyModels[id]);
            if (anim->WillHitFrame(0x45)) {
                x0 = player->mPosX;
                y0 = player->mPosY + 0x50000;
                z0 = player->mPosZ;
                vv[0].x = x0;
                vv[0].y = y0;
                vv[0].z = z0;
                Particle::System::NewSimple(0x37, vv[0].x, vv[0].y, vv[0].z);

                Sound::PlayCharVoice(player->mCharacter, 0x33, player->mCamSpacePos);
            }
        }
    }

    if (player->param1 == 0) {
        if (cur.w[0] == 0xbb) {
            id = player->GetBodyModelID(player->param1 & 0xff, 0);
            anim = reinterpret_cast<dExtFrameCtrl_c *>(
                (long long)(int)&static_cast<dExtFrameCtrl_c &>(*player->mModels.mBodyModels[id]));
            frame = (unsigned int)(anim->currFrame << 4) >> 0x10;
            if (frame >= 0x1e)
                goto particle;
        }
        if (cur.w[0] == 0xbc) {
particle:
            z0 = player->mPosZ;
            y0 = player->mPosY + 0x4b000;
            x0 = player->mPosX;
            vv[1].x = x0;
            vv[1].y = y0;
            vv[1].z = z0;
            /* Inherited volatile read preserves the retail stack reload of X
             * into r2 at 0x020bcfb8 before the particle call. */
            player->mParticle1 = Particle::System::New(
                player->mParticle1, 0x36, *(volatile int *)&vv[1].x, vv[1].y, vv[1].z, 0, 0);
        }
    }

    if (player->mCapActor != 0) {
        dActor_c *actor = player->mCapActor;
        actor->mAngleX = player->mAngleX;
        actor->mAngleY = player->mAngleY;
        actor->mAngleZ = player->mAngleZ;
        actor = player->mCapActor;
        actor->mPosX = player->mPosX;
        actor->mPosY = player->mPosY;
        actor->mPosZ = player->mPosZ;
    }

    if (!player->FinishedAnim())
        return;

    cur.w[0] = base[player->mAnimScriptStep + 1].w[0];
    cur.w[1] = base[player->mAnimScriptStep + 1].w[1];
    if (cur.w[0] != 0) {
        player->mAnimScriptStep += 1;
    }
}
}

// @symbol _ZN6Player19func_ov002_020bd06cEj
extern "C" {
void Player::func_ov002_020bd06c(unsigned int state2)
{
    extern Player::State data_ov002_02110154;
    char *self = (char *)this;
    int flag;

    flag = 0;

    switch (this->mAnimScriptId) {
    case 8:
        if (this->param1 != 0) {
            this->mHasNoCap = 0;
        }
        /* fallthrough */
    case 9:
    case 10:
        if (this->mCapActor != 0) {
            this->mCapActor->MarkForDestruction();
            this->mCapActor = 0;
        }
        break;
    case 15:
        this->mRootMotionMode = 0;
        break;
    default:
        break;
    }

    switch (state2) {
    case 8:
        flag = 0x14;
        break;
    case 9:
        flag = 0x15;
        break;
    case 10:
        flag = 0x16;
        break;
    case 5:
        if (!this->IsState(data_ov002_02110154)) {
            this->ChangeState(data_ov002_02110154);
        }
        break;
    case 14:
        this->mRootMotionMode = 1;
        break;
    case 2:
        Sound::PlayBank0(0xdc, mCamSpacePos);
        break;
    case 4:
        Sound::PlayBank0(0xe1, mCamSpacePos);
        break;
    default:
        break;
    }

    if (flag == 0) return;
    if (this->mCapActor != 0)
        return;

    {
        s8 var_areaID;
        u32 var_self8;
        s16 var_deathTableID;

        var_areaID = this->mAreaId;
        var_self8 = *(volatile u32 *)&this->param1;
        var_deathTableID = -1;

        this->mCapActor =
            Spawn(0x10d, flag | (var_self8 << 8), mPos, 0, var_areaID, var_deathTableID);
    }

    {
        dActor_c *p2 = this->mCapActor;
        p2->mAngleX = this->mAngleX;
        p2->mAngleY = this->mAngleY;
        p2->mAngleZ = this->mAngleZ;
    }
}
}

// @symbol _ZN6Player19func_ov002_020bd20cEPhii
int Player::func_ov002_020bd20c(unsigned char *params, int beginFrame, int endFrame)
{
    if (mAnimScriptId != *params) {
        func_ov002_020bd06c(*params);
        mAnimScriptId = *params;
        mAnimScriptStep = 0;
    }
    return 1;
}

// @symbol _ZN6Player19func_ov002_020bd250EPhii
extern "C" {
int Player::func_ov002_020bd250(unsigned char *params, int beginFrame, int endFrame)
{
    extern void PlayerNewSimpleHold1(unsigned int, int, int, int);
    extern short ReadUnalignedShort(unsigned char* p);
    extern void Vec3_RotateYAndTranslate(Vector3* out, const Vector3* base, int angle, const Vector3* off);
    char *self = (char *)this;
    Vector3 out1, off1, out2, off2, v;
    this->mAngleY = ReadUnalignedShort(params);
    off1.x = 0;
    off1.y = 0;
    off1.z = 0x50000;
    Vec3_RotateYAndTranslate(&out1, &mPos, this->mAngleY, &off1);
    this->mIsAirborne = 1;
    this->mLandSoundPlayed = 0;
    v.x = out1.x;
    v.y = out1.y;
    v.z = out1.z;
    this->Hurt(v, 0, 0xc000, 1, 0, 1);
    this->mVertSpeed = 0x20000;
    this->mHorzSpeed = 0xc000;
    if (param1 == 1) {
        off2.x = 0;
        off2.y = 0x64000;
        off2.z = 0x32000;
        Vec3_RotateYAndTranslate(&out2, &mPos, this->mAngleY, &off2);
        Particle::System::NewSimple(0x43, out2.x, out2.y, out2.z);
        Particle::System::NewSimple(0x44, out2.x, out2.y, out2.z);
    }
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020bd354EPhii
extern "C" {
int Player::func_ov002_020bd354(unsigned char *params, int beginFrame, int endFrame)
{
    extern unsigned short gKuppaScriptFrame;

    if (beginFrame == gKuppaScriptFrame) {
        this->mHasWings = 1;
        if (this->mPowerupTimer == 0)
            this->mPowerupTimer = 0x198;
    }
    this->mVertSpeed = -0x20000;
    this->mPeakY = this->mPosY;
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020bd3a0EPhii
extern "C" {
int Player::func_ov002_020bd3a0(unsigned char *params, int beginFrame, int endFrame)
{
    extern u16 ReadUnalignedUshort(u8* p);
    extern u16 gKuppaScriptFrame;
    extern u16 data_0209f49e;
    extern u16 data_0209f49c;

    u16 val = ReadUnalignedUshort(params);
    if ((unsigned int)beginFrame == gKuppaScriptFrame) {
        *(u16*)((char*)&data_0209f49e + (int)(this->mPlayerNo) * 0x18) =
            val | *(u16*)((char*)&data_0209f49e + (int)(gActivePlayerSlot) * 0x18);
    }
    val |= *(u16*)((char*)&data_0209f49c + (int)(gActivePlayerSlot) * 0x18);
    *(u16*)((char*)&data_0209f49c + (int)(this->mPlayerNo) * 0x18) = val;
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020bd438EPhii
extern "C" {
int Player::func_ov002_020bd438(unsigned char *params, int beginFrame, int endFrame)
{
    extern unsigned int ReadUnalignedInt(unsigned char *p);
    extern void func_02012694(unsigned int, void*);
    unsigned int v = ReadUnalignedInt(params);
    func_02012694(v, &mCamSpacePos);
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020bd45cEPhii
extern "C" {
int Player::func_ov002_020bd45c(unsigned char *params, int beginFrame, int endFrame)
{
    extern unsigned int ReadUnalignedInt(unsigned char *p);
    unsigned int v = ReadUnalignedInt(params);
    Sound::PlayBank0(v, mCamSpacePos);
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020bd480EPhii
extern "C" {
int Player::func_ov002_020bd480(unsigned char *params, int beginFrame, int endFrame)
{
    extern unsigned int ReadUnalignedInt(unsigned char *p);
    unsigned int v = ReadUnalignedInt(params);
    Sound::PlayCharVoice(mCharacter, v, mCamSpacePos);
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020bd4acEPhii
extern "C" {
int Player::func_ov002_020bd4ac(unsigned char *params, int beginFrame, int endFrame)
{
    extern void func_ov002_020d228c(void *);
    func_ov002_020d228c(this);
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020bd4c8EPhii
int Player::func_ov002_020bd4c8(unsigned char *params, int beginFrame, int endFrame)
{
    mFlags |= 0x24000000;
    return 1;
}

// @symbol _ZN6Player19func_ov002_020bd4e0EPhii
#pragma push
#pragma opt_propagation off
extern "C" {
int Player::func_ov002_020bd4e0(unsigned char *data, int beginFrame, int endFrame)
{
    extern void func_0203db0c(int idx, unsigned short val);
    extern short ReadUnalignedShort(unsigned char *p);
    extern Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b);
    extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
    extern char data_0209f4a6[];
    extern char data_0209f4a0[];
    extern char data_0209f4ac[];
    extern char data_0209f49c[];
    char *self = (char *)this;
    Vector3 v;
    int y, z;
    int idx;

    func_0203db0c(0, 0);
    z = ReadUnalignedShort(data + 4) << 12;
    y = ReadUnalignedShort(data + 2) << 12;
    v.x = ReadUnalignedShort(data) << 12;
    v.y = y;
    v.z = z;

    if (Vec3_HorzDist(&mPos, &v) <= this->mHorzSpeed) {
        this->mPosX = v.x;
        this->mPosZ = v.z;
        this->mHorzSpeed = 0;
        return 1;
    } else {
        s16 ang = Vec3_HorzAngle(&mPos, &v);
        idx = this->mPlayerNo;
        *(short*)(data_0209f4a6 + idx * 0x18) = ang;
        s16 v6 = ReadUnalignedShort(data + 6);
        *(short*)(data_0209f4a0 + this->mPlayerNo * 0x18) = v6;
        {
        int pi = this->mPlayerNo * 0x18;
        *(u8*)(data_0209f4ac + pi) = 1;
        }
        *(unsigned short*)(data_0209f49c + this->mPlayerNo * 0x18) = *(unsigned short*)(data_0209f49c + gActivePlayerSlot * 0x18) | 0x800;
        return 1;
    }
}
}
#pragma pop

// @symbol _ZN6Player19func_ov002_020bd600EPhii
extern "C" {
int Player::func_ov002_020bd600(unsigned char *data, int beginFrame, int endFrame)
{
    extern s16 ReadUnalignedShort(unsigned char *p);

    s32 x, y, z;
    z = ReadUnalignedShort(data + 4) << 12;
    y = ReadUnalignedShort(data + 2) << 12;
    x = ReadUnalignedShort(data) << 12;
    this->mPosX = x;
    this->mPosY = y;
    this->mPosZ = z;
    this->mAngleY = ReadUnalignedShort(data + 6);
    this->mPrevAngleY = this->mAngleY;
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020bd664EPhii
extern "C" {
int Player::func_ov002_020bd664(unsigned char *cmd, int beginFrame, int endFrame)
{
    extern unsigned int data_ov002_0210e158;
    extern ScriptCommand data_ov002_02110794[14];
    extern ScriptCommand data_ov002_0210a14c;
    extern ScriptCommand data_ov002_0210a124;
    extern ScriptCommand data_ov002_0210a054;
    extern ScriptCommand data_ov002_0210a0dc;
    extern ScriptCommand data_ov002_0210a534;
    extern ScriptCommand data_ov002_0210a0b4;
    extern ScriptCommand data_ov002_0210a094;
    extern ScriptCommand data_ov002_0210a40c;
    extern ScriptCommand data_ov002_0210a474;
    extern ScriptCommand data_ov002_0210a44c;
    extern ScriptCommand data_ov002_0210a3fc;
    extern ScriptCommand data_ov002_0210a3c4;
    extern ScriptCommand data_ov002_0210a064;
    extern ScriptCommand data_ov002_0210a36c;
    if (!(data_ov002_0210e158 & 1)) {
        data_ov002_0210e158 |= 1;
        data_ov002_02110794[0] = data_ov002_0210a14c;
        data_ov002_02110794[1] = data_ov002_0210a124;
        data_ov002_02110794[2] = data_ov002_0210a054;
        data_ov002_02110794[3] = data_ov002_0210a0dc;
        data_ov002_02110794[4] = data_ov002_0210a534;
        data_ov002_02110794[5] = data_ov002_0210a0b4;
        data_ov002_02110794[6] = data_ov002_0210a094;
        data_ov002_02110794[7] = data_ov002_0210a40c;
        data_ov002_02110794[8] = data_ov002_0210a474;
        data_ov002_02110794[9] = data_ov002_0210a44c;
        data_ov002_02110794[10] = data_ov002_0210a3fc;
        data_ov002_02110794[11] = data_ov002_0210a3c4;
        data_ov002_02110794[12] = data_ov002_0210a064;
        data_ov002_02110794[13] = data_ov002_0210a36c;
    }
    return (this->*data_ov002_02110794[cmd[6]])(cmd + 7, beginFrame, endFrame);
}
}

// @symbol _ZN6Player8CanPauseEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::CanPause()
{

    extern Player::State data_ov002_0211067c;
    extern Player::State data_ov002_021106ac;
  if(*(unsigned char*)((char*)&mIsUnderwater)){
    if(this->IsState(data_ov002_0211067c)) goto ret1a;
    if(this->IsState(data_ov002_021106ac)) goto ret1a;
    goto ret0;
ret1a:
    return 1;
  }
  if(*(unsigned char*)((char*)&mIsAirborne)) goto ret0;
  if(*(unsigned char*)((char*)&mIsTakingDamage)) goto ret0;
  if(*(unsigned char*)((char*)&mIsNoControl)) goto ret0;
  return 1;
ret0:
  return 0;
}
}

// @symbol func_ov002_020bd8ac
extern "C" {
int func_ov002_020bd8ac(unsigned char *p)
{
    return p[1801] == 0;
}
}

// @symbol _ZN6Player13EndStateMusicEj
extern "C" {
void Player::EndStateMusic(u32 music) {
    u32 other = mPowerupMusic;
    if (other == 0) {
        Sound::EndMusic(mPlayerNo);
        mPlayingMusic = 0;
    } else if (mPlayingMusic == music && other != music) {
        Sound::EndMusic(mPlayerNo);
        Sound::SetMusic(mPlayerNo, mPowerupMusic);
        mPlayingMusic = mPowerupMusic;
    }
    mStateMusic = 0;
}
}

// @symbol _ZN6Player15StartStateMusicEj
extern "C" {
void Player::StartStateMusic(u32 music) {
    u32 other = mPowerupMusic;
    if (other == music) {
        mStateMusic = music;
        return;
    }
    if ((other | mStateMusic) != 0) {
        Sound::EndMusic(mPlayerNo);
    }
    mStateMusic = music;
    mPlayingMusic = music;
    Sound::SetMusic(mPlayerNo, music);
}
}

// @symbol _ZN6Player15EndPowerupMusicEj
extern "C" {
void Player::EndPowerupMusic(u32 music) {
    u32 other = mStateMusic;
    if (other == 0) {
        Sound::EndMusic(mPlayerNo);
        mPlayingMusic = 0;
    } else if (mPlayingMusic == music && other != music) {
        Sound::EndMusic(mPlayerNo);
        Sound::SetMusic(mPlayerNo, mStateMusic);
        mPlayingMusic = mStateMusic;
    }
    mPowerupMusic = 0;
}
}

// @symbol _ZN6Player17StartPowerupMusicEj
extern "C" {
void Player::StartPowerupMusic(u32 music) {
    u32 other = mStateMusic;
    if (other == music) {
        mPowerupMusic = music;
        return;
    }
    if ((mPowerupMusic | other) != 0) {
        Sound::EndMusic(mPlayerNo);
    }
    mPowerupMusic = music;
    mPlayingMusic = music;
    Sound::SetMusic(mPlayerNo, music);
}
}

// @symbol _ZN6Player19func_ov002_020bda48Ev
extern "C" {
void Player::func_ov002_020bda48()
{
    extern void func_ov002_020e032c(void*);
    extern void func_ov002_020d80d0(void*);
    extern void func_ov002_020cc1f4(void*);
    extern void func_ov002_020de968(void*);
    mPowerupTimer = 0;
    func_ov002_020e032c(this);
    func_ov002_020bdef0();
    func_ov002_020bdd9c();
    func_ov002_020d80d0(this);
    func_ov002_020bdd2c();
    func_ov002_020cc1f4(this);
    func_ov002_020de968(this);
}
}

// @symbol _ZN6Player8HasNoCapEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::HasNoCap()
{
    extern signed char data_0209f2f8;
    extern unsigned char data_0209f2d8;
    extern Player::State data_ov002_0211058c;
    if(data_0209f2f8 == 0x1f) return 0;
    int r = (data_0209f2d8 == 2);
    if(!r){
        if(!this->IsState(data_ov002_0211058c)){
            r = (data_0209f2d8 == 1);
            if(!r){
                if (SaveData::HasPlayerLostCap())
                    return 1;
            }
        }
    }
    if(param1 == 3) return 0;
    r = mHasNoCap != 0;
    return r;
}
}

// @symbol _ZN6Player19func_ov002_020bdb50Ei
extern "C" {
void Player::func_ov002_020bdb50(int arg) {
    char* c = (char*)this;
    extern void Player_ReleaseHeldActor(void*);
    extern void func_ov002_020d5cec(void);
    this->func_ov002_020bdc18();
    Player_ReleaseHeldActor(c);
    if (arg != 0) return;
    if (this->mObjInMouth == 0) return;
    this->func_ov002_020d71a0();
    {
        int r = ((dActor_c *)mObjInMouth)->OnYoshiTryEat();
        if (r == 0) return;
    }
    {
        char *obj = (char *)mObjInMouth;
        int b1 = (((dActor_c *)obj)->actorID == 0xbf);
        if (b1) {
            func_ov002_020d5cec();
            (this->mStateFlags) &= ~2;
        } else {
            ((dActor_c *)obj)->OnTurnIntoEgg(*this);
        }
    }
    this->mObjInMouth = 0;
}
}

// @symbol _ZN6Player19func_ov002_020bdc18Ev
extern "C" {
void Player::func_ov002_020bdc18() {
    char *c = (char *)this;
    extern int func_ov002_020edca4(void *);
    int i;
    for (i = 0; i < 5; i++) {
        int *a = (int *)mHeldObjQueue;
        if (a[i] != 0) {
            func_ov002_020edca4((void *)a[i]);
            a = (int *)mHeldObjQueue;
            a[i] = 0;
        }
    }
}
}

// @symbol _ZN6Player16IncMegaKillCountEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
void Player::IncMegaKillCount() {
    extern void GiveLives(int);
    Vector3 v;
    char *new_var2;
    int new_var3;
    char *a;
    int y;
    if ((*((unsigned char *)((char *)&mIsMega))) == 0) {
        return;
    }
    ++mMegaKillCount;
    new_var3 = -1;
    if (mMegaKillCount >= 8) {
        mMegaKillCount = 8;
        Sound::PlayBank3(0x6e, mCamSpacePos);
        GiveLives(1);
    }
    v.x = *((int *)((char *)&mPosX));
    y = *((int *)((char *)&mPosY));
    v.y = y;
    v.z = mPosZ;
    v.y = y + 0x190000;
    new_var2 =
        (char *)Spawn(0x14b, mMegaKillCount, v, 0, *((signed char *)((char *)&mAreaId)), new_var3);
    a = new_var2;
    if (a == 0) {
        return;
    }
    ((daObj1UpLogo_c *)a)->mDelay = 0;
    ((daObj1UpLogo_c *)a)->SetFollowTarget(0);
}
}

// @symbol _ZN6Player19func_ov002_020bdd2cEv
extern "C" {
void Player::func_ov002_020bdd2c() {
    void *c = (void *)this;
    extern void func_ov002_020db8bc(void *, unsigned char val);
    extern void *data_0209f318;
    if (this->mIsMega == 1) {
        func_ov002_020db8bc((unsigned char *)c, 3);
        {
            ((dCamera_c *)data_0209f318)->mFlags &= ~0x80;
        }
        this->EndPowerupMusic(0x31);
        this->mWaterStillParticle = 0;
        this->mWaterMoveParticle = this->mWaterStillParticle;
        this->mParticle1 = 0;
        mMegaKillCount = 0;
    }
    this->mIsMega = 0;
}
}

// @symbol _ZN6Player19func_ov002_020bdd9cEv
extern "C" {
void Player::func_ov002_020bdd9c(){
    extern SharedFilePtr *data_ov002_020ff480[];
    if (mIsMetal != 1)
        return;
    mIsMetal = 0;
    unsigned int v = this->param1;
    int id = this->GetBodyModelID(v & 0xff, 0);
    SharedFilePtr *t = data_ov002_020ff480[mCharFileBase + v];
    mModels.mBodyModels[id]->Copy(*mModels.mBodyModels[4], (char *)t->filePtr, 0);
    EndPowerupMusic(0x34);
}
}

// @symbol _ZN6Player14InitMetalWarioEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
void Player::InitMetalWario()
{
    extern SharedFilePtr *data_ov002_020ff480[];
    u32 v;
    u32 id;

    func_ov002_020bda48();
    if (func_ov002_020bea7c() != 0) {
        u32 b = mHatCharacter;
        if (b != 2)
            return;
        this->TurnOffToonShading(b);
        this->TurnOffToonShading(mPrevCharacter);
        mCapFlags = 0;
        param1 = mHatCharacter;
        mHasNoCap = 0;
    }

    mIsMetal = 1;

    v = param1;
    id = this->GetBodyModelID(v & 0xff, 0);
    mModels.mBodyModels[id]->Copy(*mModels.mBodyModels[v],
                                  (char *)data_ov002_020ff480[mCharFileBase + v]->filePtr, 0);

    mPowerupTimer = 0x258;
    StartPowerupMusic(0x34);
    func_ov002_020c43c4(4);
}
}

// @symbol _ZN6Player19func_ov002_020bdef0Ev
extern "C" {
void Player::func_ov002_020bdef0(){
    if (mIsVanish == 0)
        return;
    mModels.mTexSeqPlayer[0].Update(mModels.mBodyModels[1]->data);
    mModels.mTexSeqPlayer[1].Update(mModels.mHeadModels[1]->data);
    mModels.mTexSeqPlayer[0].currFrame = 0;
    mModels.mTexSeqPlayer[1].currFrame = 0;
    mIsVanish = 0;
    EndPowerupMusic(0x33);
}
}

// @symbol _ZN6Player15InitVanishLuigiEv
/* recovered: named members + shared header, real C++ method
 *
 * Starts the vanish cap: sets the flag, arms mPowerupTimer to 0x258 (the same 600
 * frames mBalloonTimer uses), then the sound and effect calls.
 *
 * Note the original wrote mPowerupTimer as `c + 0x600 + 0xae` rather than 0x6ae --
 * the split is how the compiler addressed it, not a second field.
 */
void Player::InitVanishLuigi()
{
    func_ov002_020bda48();
    mIsVanish = 1;
    mPowerupTimer = 0x258;
    StartPowerupMusic(0x33);
    func_ov002_020c43c4(3);
}

// @symbol _ZN6Player19func_ov002_020bdf8cEv
extern "C" {
void Player::func_ov002_020bdf8c(){
    extern char* data_0209f318;
  func_ov002_020bda48();
  mScaleAnimStep = 0;
  if(mIsMega == 0){
      this->SetNoControlState(0xd, -1, 0);
      func_02012790(0x2e);
  }
  mIsMega = 1;
  mMegaTimer = 0x258;
  ((dCamera_c *)data_0209f318)->mFlags |= 0x80;
  StartPowerupMusic(0x31);
}
}

// @symbol func_ov002_020be008
extern "C" {
void func_ov002_020be008(Player *player) {
    extern unsigned char data_0209f2d8;
    extern int data_ov002_021104b4;
    extern signed char data_02092114;
    extern int func_ov002_020e3078(void*, void*);
    int b = (int)(data_0209f2d8 == 1);
    if (b != 0)
        return;
    if (func_ov002_020e3078(player, &data_ov002_021104b4) == 0)
        return;
    {
        int r5 = data_02092114;
        if (r5 < 0)
            return;
        if (r5 == player->param1)
            return;
        player->SetNewHatCharacter(r5 & 0xff, 0, 0);
        player->SetNoControlState(8, -1, 0);
        dActor_c::Spawn(0x10d, (r5 << 8) | 0xa, player->mPos, 0, player->mAreaId, -1);
        data_02092114 = -1;
    }
}
}

// @symbol _ZN6Player18SetNewHatCharacterEjjb
/* recovered: named members + shared header, real C++ method */
extern "C" {
void Player::SetNewHatCharacter(unsigned int p1, unsigned int p2, bool p3)
{


    extern Player::State data_ov002_02110604;
    extern Player::State data_ov002_0211013c;
    extern void func_ov002_020d9c70(void*);
    extern void func_ov002_020da95c(void*);
    extern void func_ov002_020e6350(void*);
  unsigned int old = param1;
  if (p1 == old) return;
  mPrevCharacter = (unsigned char)old;
  mHatCharacter = (unsigned char)p1;
  mCapFlags = 1;
  if (p1 != mCharacter) {
    this->mCapFlags |= 0x8000;
  }
  func_ov002_020bdb50(p2);
  func_ov002_020bda48();
  if (this->IsState(data_ov002_02110604)) {
    func_ov002_020d9c70(((char*)this));
    func_ov002_020da95c(((char*)this));
    this->ChangeState(data_ov002_0211013c);
  }
  if (p3 == 0) {
    if (p1 != mCharacter) func_02012790(0xb);
    else func_02012790(0xc);
  }
  param1 = mHatCharacter;
  func_ov002_020e6350(((char*)this));
  param1 = mPrevCharacter;
}
}

// @symbol _ZN6Player16SetRealCharacterEj
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
void Player::SetRealCharacter(unsigned int chr_)
{
    extern SharedFilePtr *data_ov002_020ff480[];
    extern u8 data_02092128[];
    extern u8 data_0209caa0[];
    extern void func_ov002_020e6330(void*);
    u32 chr = (u32)chr_;

    u32 cur = param1;
    u32 base = mCharFileBase;
    u32 m1, m2;

    ((SharedFilePtr *)(data_ov002_020ff480[base + (cur & 3)]))->Release();
    this->SetNewHatCharacter(chr, 0, 1);
    param1 = chr;
    mCharacter = (u8)chr;
    data_02092128[mPlayerNo] = (u8)param1;
    data_0209caa0[0x41] = (u8)param1;
    dExtFrameCtrl_c::LoadFile(*data_ov002_020ff480[mCharFileBase + (param1 & 3)]);
    func_ov002_020e6330(((char *)this));
    this->Heal(0x880);
    mCapFlags = 0;

    m1 = this->GetBodyModelID(chr, 0);
    m2 = this->GetBodyModelID(param1 & 0xff, 0);
    mModels.mBodyModels[m1]->Copy(*mModels.mBodyModels[m2],
                                  (char *)data_ov002_020ff480[mCharFileBase + chr]->filePtr, 0);
    m1 = this->GetBodyModelID(chr, 0);
    mModels.mBodyModels[m1]->Func_020162C4((u32)data_ov002_020ff480[chr + 0xc4]->filePtr, 0, 0x1000,
                                           0);
}
}

// @symbol _ZN6Player18TurnOffToonShadingEj
/* recovered: named members + shared header, real C++ method */
extern "C" {
void Player::TurnOffToonShading(unsigned int j)
{
    extern void func_ov002_020e6b74(void *, int *);
    dExtSimpleModel_c *m;
    m = mModels.mBodyModels[this->GetBodyModelID(j, 0)];
    m->SetPolygonMode(0);
    func_ov002_020e6b74(m, (int *)mModels.mBodyTexParams[j]);
    m = mModels.mHeadModels[j];
    m->SetPolygonMode(0);
    func_ov002_020e6b74(m, (int *)mModels.mHeadTexParams[j]);
    m = mModels.mHeadModels[j + 4];
    m->SetPolygonMode(0);
    func_ov002_020e6b74(m, (int *)mModels.mHeadTexParams[j + 4]);
}
}

// @symbol _ZN6Player19func_ov002_020be3b0Ev
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
void Player::func_ov002_020be3b0()
{
    extern void func_ov002_020e6b74(void *, int *);
    extern void func_ov002_020d80d0(void*);
    extern SharedFilePtr *data_ov002_020ff480[];
    extern u8 data_02092128[];
    extern Player::State data_ov002_021104b4;
    extern u8 data_ov002_020ff14c[];
    extern void func_ov002_020e6b3c(void*);
    extern void*data_ov002_020ff790[];
    extern int __aeabi_idiv(int, int);
    extern char data_ov002_02110804[];
    extern void func_ov002_020e69c0(void*);
    extern void func_ov002_020e6a78(void*, int);
    void* case3_m1;
    void* case3_m2;
    int var_r5;
    int var_r4;
    u16 flags;
    int idx;

    flags = mCapFlags;
    var_r5 = 3;
    var_r4 = 0;
    if (flags & 0x8000) {
        var_r5 = 2;
        var_r4 = 5;
    }
    idx = flags & 7;

    switch (idx) {
    case 0:
        return;
    case 1: {
        dExtSimpleModel_c *m1 = mModels.mBodyModels[this->GetBodyModelID((u8)(this->param1), 0)];
        m1->SetPolygonMode(2);
        {
            dExtSimpleModel_c *m2 = mModels.mHeadModels[func_ov002_020becf4((u8)(this->param1), 0)];
            m2->SetPolygonMode(2);
        }
        mCapPaletteBlend = 0;
        mCapTimer = data_ov002_020ff14c[var_r4];
        mCapFlags++;
        this->param1 = mPrevCharacter;
        break;
    }
    case 2:
        mCapPaletteBlend += 0x199;
        if ((u32)mCapPaletteBlend >= 0x1000) {
            mCapPaletteBlend = 0x1000;
        }
        if (mCapTimer == 0) {
            mCapFlags++;
            mCapTimer = data_ov002_020ff14c[var_r4 + 1];
        }
        this->param1 = mPrevCharacter;
        break;
    case 3: {
        if ((mCapTimer % var_r5) == 0) {
            case3_m1 = mModels.mBodyModels[this->GetBodyModelID((u8)(this->param1), 0)];
            case3_m2 = mModels.mHeadModels[func_ov002_020becf4((u8)(this->param1), 0)];
            if ((mCapTimer % (var_r5 * 2)) == 0) {
                func_ov002_020e6b74(case3_m1, (int *)mModels.mBodyTexParams[this->param1]);
                func_ov002_020e6b74(
                    case3_m2,
                    (int *)mModels.mHeadTexParams[func_ov002_020becf4((u8)(this->param1), 0)]);
            } else {
                func_ov002_020e6b3c(case3_m1);
                func_ov002_020e6b3c(case3_m2);
            }
        }
        if (mCapTimer == 0) {
            dExtSimpleModel_c *t1;
            dExtSimpleModel_c *t2;
            func_ov002_020e6b3c(case3_m1);
            func_ov002_020e6b3c(case3_m2);
            mCapFlags++;
            mCapTimer = data_ov002_020ff14c[var_r4 + 2];
            t1 = mModels.mBodyModels[this->GetBodyModelID(mHatCharacter, 0)];
            t1->SetPolygonMode(2);
            func_ov002_020e6b3c(t1);
            t2 = mModels.mHeadModels[func_ov002_020becf4(mHatCharacter, 0)];
            t2->SetPolygonMode(2);
            func_ov002_020e6b3c(t2);
            mQuicksandRunFlags = 0;
            mWaterRunLatch = mQuicksandRunFlags;
            mWaterRunTimer = mWaterRunLatch;
            mDetectWater = 0;
            func_ov002_020d80d0(this);
            mFireBreaths = 0;
            mUseAltBodyModel = 0;
            dExtFrameCtrl_c::LoadFile(*data_ov002_020ff480[mCharFileBase + mHatCharacter]);
            {
                u32 mm1 = this->GetBodyModelID(mHatCharacter, 0);
                mModels.mBodyModels[mm1]->Copy(
                    *mModels.mBodyModels[this->GetBodyModelID(mPrevCharacter, 0)],
                    (char *)data_ov002_020ff480[mCharFileBase + mHatCharacter]->filePtr, 0);
            }
            mLoadedResourceFlags |= 0x40;
        }
        this->param1 = mPrevCharacter;
        break;
    }
    case 4:
        if ((mCapTimer % var_r5) == 0) {
            if ((mCapTimer % (var_r5 * 2)) == 0) {
                this->param1 = mPrevCharacter;
            } else {
                this->param1 = mHatCharacter;
            }
        }
        if (mCapTimer == 0) {
            u32 mm;
            mCapFlags++;
            mCapTimer = data_ov002_020ff14c[var_r4 + 3];
            this->param1 = mHatCharacter;
            data_02092128[mPlayerNo] = (u8)(this->param1);
            if (mHatCharacter == mCharacter) {
                mHasNoCap = 0;
            }
            func_ov002_020e6b74(
                mModels.mBodyModels[this->GetBodyModelID(mHatCharacter, 0)],
                (int *)mModels.mBodyTexParams[*(volatile u8 *)&this->mHatCharacter]);
            func_ov002_020e6b74(
                mModels.mBodyModels[this->GetBodyModelID(mPrevCharacter, 0)],
                (int *)mModels.mBodyTexParams[*(volatile u8 *)&this->mPrevCharacter]);
            {
                int h1 = func_ov002_020becf4(mHatCharacter, 0);
                func_ov002_020e6b74(mModels.mHeadModels[h1], (int *)mModels.mHeadTexParams[h1]);
            }
            {
                int h2 = func_ov002_020becf4(mPrevCharacter, 0);
                func_ov002_020e6b74(mModels.mHeadModels[h2], (int *)mModels.mHeadTexParams[h2]);
            }
            mm = this->param1;
            mModels.mBodyModels[this->GetBodyModelID((u8)mm, 0)]->Func_020162C4(
                (u32)((SharedFilePtr *)data_ov002_020ff790[mm])->filePtr, 0, 0x1000, 0);
            data_ov002_020ff480[mCharFileBase + mPrevCharacter]->Release();
            mLoadedResourceFlags &= ~0x40;
            func_ov002_020bdb50(0);
        } else {
            u8 a, b;
            u32 idxA;
            b = mPrevCharacter;
            if (b == this->param1) {
                a = mHatCharacter;
            } else {
                a = b;
                b = mHatCharacter;
            }
            idxA = this->GetBodyModelID(a, 0);
            mModels.mBodyModels[idxA]->Copy(*mModels.mBodyModels[this->GetBodyModelID(b, 0)],
                                            (char *)data_ov002_020ff480[mCharFileBase + a]->filePtr,
                                            0);
        }
        break;
    case 5:
        if (mCapTimer == 0) {
            mCapFlags++;
            mCapTimer = data_ov002_020ff14c[var_r4 + 4];
            if (mHatCharacter != mCharacter && this->IsState(data_ov002_021104b4) == 0) {
                Sound::PlayCharVoice(mCharacter, 0x21, mCamSpacePos);
            }
        }
        break;
    case 6: {
        int divv = data_ov002_020ff14c[var_r4 + 4];
        /* Launder kept: `mCapPaletteBlend -= ...` and `= (u16)(mCapPaletteBlend - ...)` both move bytes. */
        *(u16 *)((long long)(int)&mCapPaletteBlend) =
            (u16)(*(u16 *)((long long)(int)&mCapPaletteBlend) - __aeabi_idiv(0x1000, divv));
        if ((s16)mCapPaletteBlend < 0) {
            mCapPaletteBlend = 0;
        }
        if (mCapTimer == 0) {
            mCapFlags++;
        }
        break;
    }
    case 7:
        this->TurnOffToonShading(mHatCharacter);
        this->TurnOffToonShading(mPrevCharacter);
        mCapFlags = 0;
        break;
    }

    if (mCapTimer != 0) {
        mCapTimer--;
    }
    {
        char* dummy = data_ov002_02110804;
        func_ov002_020e69c0(dummy);
        func_ov002_020e6a78(dummy, mCapPaletteBlend);
    }
}
}

// @symbol _ZN6Player19func_ov002_020bea7cEv
int Player::func_ov002_020bea7c(){
    return mCapFlags != 0;
}

// @symbol _ZN6Player15IsCollectingCapEv
/* recovered: named members + shared header, real C++ method */
int Player::IsCollectingCap()
{
  unsigned short v = mCapFlags;
  if(v!=0 && (v&7)<5) return 1;
  return 0;
}

// @symbol _ZN6Player19func_ov002_020beabcEv
void Player::func_ov002_020beabc()
{
    char *p = (char *)this;
    mCapFlags = 5;
    mCapTimer = 0x2f;
    mCapPaletteBlend = 0x1000;
    this->mPrevCharacter = this->mCharacter;
    this->mHatCharacter = this->mPrevCharacter;
    unsigned int id = GetBodyModelID(this->mCharacter, 0);
    mModels.mBodyModels[id]->SetPolygonMode(2);
    unsigned int id2 = func_ov002_020becf4(param1 & 0xff, 0);
    mModels.mHeadModels[id2]->SetPolygonMode(2);
}

// @symbol _ZN6Player19func_ov002_020beb38Ev
extern "C" {
int Player::func_ov002_020beb38(){
    extern unsigned short data_0209f49e;
    extern unsigned short data_020a0e5a;
    extern int AngleDiff(int a, int b);
  unsigned char idx = gActivePlayerSlot;
  unsigned char count = 0;
  unsigned short f = *(unsigned short*)((char*)&data_0209f49e + idx*0x18);
  if (f & 1) count++;
  if (f & 2) count++;
  if (f & 0x400) count++;
  if (f & 0x800) count++;
  if (*(unsigned short*)((char*)&data_020a0e5a + idx*4) & 0xf0) count++;
  if (AngleDiff(mPrevDesiredAngleY, mDesiredAngleY) >= 0x4000) count += 8;
  return count;
}
}

// @symbol func_ov002_020bebd4
extern "C" {
void func_ov002_020bebd4(void){
    extern int data_ov002_020ff480[];
  int i;
  for(i=0x44;i<0x1c8;i+=4){
    ((SharedFilePtr *)((void*)data_ov002_020ff480[i]))->Release();
    ((SharedFilePtr *)((void*)data_ov002_020ff480[i+1]))->Release();
    ((SharedFilePtr *)((void*)data_ov002_020ff480[i+2]))->Release();
    ((SharedFilePtr *)((void*)data_ov002_020ff480[i+3]))->Release();
  }
}
}

// @symbol func_ov002_020bec2c
extern "C" {
void func_ov002_020bec2c(void){
    extern SharedFilePtr *data_ov002_020ff480[];
    int i;
    for (i = 0x44; i < 0x1c8; i += 4) {
        dExtFrameCtrl_c::LoadFile(*data_ov002_020ff480[i]);
        dExtFrameCtrl_c::LoadFile(*data_ov002_020ff480[i + 1]);
        dExtFrameCtrl_c::LoadFile(*data_ov002_020ff480[i + 2]);
        dExtFrameCtrl_c::LoadFile(*data_ov002_020ff480[i + 3]);
    }
}
}

// @symbol func_ov002_020bec84
extern "C" {
int func_ov002_020bec84(Player *p, unsigned int i) {
    extern unsigned int data_ov002_020ff220[];
    return p->IsAnim(data_ov002_020ff220[i]);
}
}

// @symbol func_ov002_020bec9c
extern "C" {
void func_ov002_020bec9c(Player *c, unsigned int a, int b, int d, unsigned short e) {
    extern int data_ov002_020ff220[];
    c->SetAnim(data_ov002_020ff220[a], b, d, e);
}
}

// @symbol _ZNK6Player14GetBodyModelIDEjb
/* recovered: named members + shared header, real C++ method
 *
 * Metal cap overrides whichever body model the caller asked for. Which metal
 * flag is consulted depends on the second argument -- mIsMetal for the live
 * state, mDrawnIsMetal (func_ov002_020e4bb8's blinked copy) for drawing.
 *
 * const, and the K in the mangled name is what says so: declared without it
 * the compiler emits the non-const name, which names a symbol that does not
 * exist.
 */
unsigned int Player::GetBodyModelID(unsigned int a, bool b_) const
{
    if (b_ == 0) {
        if (mIsMetal) a = 4;
    } else {
        if (mDrawnIsMetal)
            a = 4;
    }
    return a;
}

// @symbol _ZN6Player19func_ov002_020becf4Eii
int Player::func_ov002_020becf4(int v, int arg2){
    int cond;
    int r1;
    if (arg2 == 0) {
        if (this->mIsMetal) v = 9;
        if (this->mIsBalloon) v = 8;
        cond = this->HasNoCap();
    } else {
        if (this->mDrawnIsMetal)
            v = 9;
        if (this->mDrawnIsBalloon)
            v = 8;
        cond = this->mDrawnHasNoCap;
    }
    r1 = 0;
    if ((unsigned int)v >= 4) {
        if (cond) r1 = 1;
    } else if (v == 3) {
        if (this->mUseAltBodyModel) r1 = 4;
    } else {
        if (cond) r1 = 4;
    }
    return (r1 + v) & 0xff;
}

// @symbol _ZN6Player12FinishedAnimEv
/* recovered: named members + shared header, real C++ method */
int Player::FinishedAnim()
{
    unsigned int modelID = GetBodyModelID(param1 & 0xff, 0);
    // GetBodyModelID can return 4; the shared array currently declares four slots.
    dExtAnmModel_c *model = mModels.mBodyModels[modelID];
    return model->Finished() != 0;
}

// @symbol Player_AdvanceAnims
extern "C" {
void Player_AdvanceAnims(char *self)
{
    Player *player = reinterpret_cast<Player *>(self);
    unsigned int modelID = player->GetBodyModelID(player->param1 & 0xff, 0);
    dExtAnmModel_c *model = player->mModels.mBodyModels[modelID];
    model->Advance();

    if (player->IsAnim(0x9a) || player->IsAnim(0x19) ||
        player->IsAnim(0xac) || player->IsAnim(0x6b) ||
        player->IsAnim(0x85) || player->IsAnim(0x94) ||
        player->IsAnim(0x98) || player->IsAnim(0x99)) {
        player->mModels.mTexSeqBody[player->param1].currFrame = 0;
    }
    player->mModels.mTexSeqBody[player->param1].Advance();
}
}

// @symbol _ZN6Player6IsAnimEj
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::IsAnim(unsigned int a)
{
    extern int data_ov002_020ff480[];
  int ip = param1;
  int* p = (int*)data_ov002_020ff480[ip + a*4];
  int v = p[1];
  unsigned int id = this->GetBodyModelID(ip&0xff, 0);
  dExtAnmModel_c *model = mModels.mBodyModels[id];
  int matches = ((int)model->file == v);
  return matches ? 1 : 0;
}
}

// @symbol _ZN6Player7SetAnimEjiit
#define LAUNDER16(p) ((u16 *)(int)(p))
#pragma push
#pragma opt_propagation off
/* mwccarm rejects a linkage-spec inside a function, and these six names are
   redeclared with other types elsewhere in the TU, so they cannot be hoisted.
   The block stays so the hand-spelled callees keep C linkage. The method
   itself still mangles as the Player member. */
extern "C" {
#pragma opt_propagation off
void Player::SetAnim(u32 animId, int flags, Fix12i speed, u16 startFrame) {
    extern u8 data_0209f2d8;
    extern SharedFilePtr *data_ov002_020ff480[];
    extern SharedFilePtr *data_ov002_020ff2f0[];
    char *self = (char *)this;
    Player *player = (Player *)self;
    u32 charId;
    SharedFilePtr *sfp;
    u32 sl;
    int field8;
    u32 newVal;
    int id;
    int flag;
    int param2;
    void *modelPtr;

    flag = (data_0209f2d8 == 1);
    if (flag) {
        if ((int)animId < 0x11 || (int)animId >= 0x72) {
            u16 *f = LAUNDER16(&player->mStateFlags);
            *f = (u16)(*f | 0x80);
            return;
        }
    }

    newVal = animId << 2;
    if (newVal != player->mCharFileBase) {
        data_ov002_020ff480[player->mCharFileBase + (player->param1 & 3)]->Release();
        dExtFrameCtrl_c::LoadFile(*data_ov002_020ff480[newVal + (player->param1 & 3)]);

        id = this->GetBodyModelID(player->param1 & 0xff, 0);
        modelPtr = player->mModels.mBodyModels[id];
        ((dExtAnmModel_c *)modelPtr)->file = 0;

        if ((player->mCapFlags & 7) == 4) {
            charId = player->mHatCharacter;
            if (charId == player->param1)
                charId = player->mPrevCharacter;

            data_ov002_020ff480[player->mCharFileBase + charId]->Release();
            sfp = data_ov002_020ff480[newVal + charId];
            dExtFrameCtrl_c::LoadFile(*sfp);

            id = this->GetBodyModelID(charId & 0xff, 0);
            player->mModels.mBodyModels[id]->SetAnim((BCA_File *)sfp->filePtr, flags, speed,
                                                     startFrame);
            {
                u16 *g = LAUNDER16(&player->mCapTimer);
                *g = (u16)(*g + 1);
            }
        }
    }

    field8 = player->param1;
    sl = animId * 4;
    id = this->GetBodyModelID((u32)(field8 & 0xff), 0);
    player->mModels.mBodyModels[id]->SetAnim(
        (BCA_File *)data_ov002_020ff480[(field8 & 3) + (animId * 4)]->filePtr, flags, speed,
        startFrame);

    field8 = player->param1;
    if ((int)sl >= 0x60) sl = 0x60;
    sl += (field8 & 3);
    param2 = 0;
    if (((int)sl >> 2) == 1) param2 = 0x40000000;
    player->mModels.mTexSeqBody[field8].SetFile(*(BTP_File *)data_ov002_020ff2f0[sl]->filePtr,
                                                param2, 0x1000, 0);

    player->mCharFileBase = newVal;
}
}
#pragma pop
#undef LAUNDER16

// @symbol _ZN6Player19func_ov002_020bf13cEv
/* matched pure C — was HAND-ASM regperm floor; y-early preload cracks r4/r5 */
extern "C" {
void Player::func_ov002_020bf13c()
{
    /* Stays a POD twin of Vector3, measured: v never escapes, and the ROM keeps
       all three components in registers -- the frame here is 0x10, which is just
       the volatile pad[4] below. Vector3 declares a destructor, so the object
       must have an address, and that pushes v onto the stack and grows the frame
       to 0x20. No other instruction in the function changes. This is a second
       reason for a twin, besides the copy lowering include/types.h describes. */
    typedef struct { int x, y, z; } Vec3b;
    extern short data_02082214[];
    volatile int pad[4];
    int vel; int* dst; short s, cs; int ang; int r6, flags;
    Vec3b v;
    (void)&pad;
    if (param1 != 3)
        return;
    dst = (int *)mTrailPos;
    if (dst == 0) return;
    vel = this->mHorzSpeed;
    ang = this->mAngleY;
    {
        int idx = (unsigned short)(short)(ang + 0x8000) >> 4;
        s = data_02082214[idx * 2];
        cs = data_02082214[idx * 2 + 1];
    }
    v.x = this->mPosX;
    v.y = this->mPosY;
    v.z = this->mPosZ;
    v.x = (short)s * (short)0x78 + v.x;
    v.z = (short)cs * (short)0x78 + v.z;
    r6 = vel >> 5;
    if (r6 > 0x1000) r6 = 0x1000;
    if (r6 < 0) r6 = 0;
    {
        unsigned char f6de = mIsAirborne;
        int ip = (r6 + 0x1000) >> 8;
        flags = ip & 0xff;
        if (f6de != 0)
            flags = (flags | 0x80) & 0xff;
        if (vel != 0)
            flags = (flags | 0x40) & 0xff;
        dst[0] = v.x;
        dst[1] = v.y + 0xc000;
        dst[2] = v.z;
        *mTrailFlags = (unsigned char)flags;
    }
}
}

// @symbol func_ov002_020bf224
/* func_ov002_020bf224 — scale b by an s16 factor from a 0x18-stride table
 * indexed by byte gActivePlayerSlot (fx12 rounded multiply), clamped below by c.
 * No callees; data: gActivePlayerSlot (u8 index), data_0209f4a0 (s16 table).
 */
extern "C" {
int func_ov002_020bf224(int a, int b, int c)
{
    extern struct Entry18 data_0209f4a0[];
    b = FX_MUL(b, data_0209f4a0[gActivePlayerSlot].val);
    if (b < c)
        b = c;
    return b;
}
}

// @symbol func_ov002_020bf27c
extern "C" {
int func_ov002_020bf27c(char* c, int r1){
    Player *player = (Player *)c;
    int v = player->param1;
    int b;
    if (player->mIsMega != 0)
        return r1;
    b = (int)(v == 3);
    if (b != 0)
        return r1 * 0x5a / 100;
    if (v == 1)
        r1 >>= 2;
    return r1;
}
}

// @symbol _ZN6Player19func_ov002_020bf2d8Ei
extern "C" {
void Player::func_ov002_020bf2d8(Fix12i a){
    void*c = (void*)this;
    extern unsigned short data_ov002_020ff140[];
    unsigned int t = data_ov002_020ff140[param1];
    this->mVertSpeed = FX_MUL(a, (int)t);
}
}

// @symbol Player_ScaleByCharFactor
extern "C" {
Fix12i Player_ScaleByCharFactor(void*c, Fix12i a){
    extern unsigned short data_ov002_020ff170[];
    int idx = ((Player *)c)->param1 & 0xff;
    unsigned int t = data_ov002_020ff170[idx];
    return FX_MUL(a, (int)t);
}
}

// @symbol func_ov002_020bf340
extern "C" {
void func_ov002_020bf340(int dummy, int*p, int delta, int limit){
  int neg = -limit;
  *p = *p + delta;
  if(*p < neg){ *p = neg; return; }
  if(*p > limit){ *p = limit; }
}
}

// @symbol _ZN6Player19func_ov002_020bf36cEPv
extern "C" {
void Player::func_ov002_020bf36c(void* arg){
    char* self = (char*)this;
    typedef int Fix12i;
    extern void func_ov002_020ce798(void*);
    Fix12i saved;
    unsigned char f = this->mIsNoControl;
    if (f == 0)
        this->func_ov002_020bfa74();
    if ((this->mClsnFlags & 1) || this->mIsUnderwater)
        this->func_ov002_020c0108(1);
    if ((int)mClimbClsn != 0)
        return;
    saved = this->mHorzSpeed;
    if (this->mClsnFlags & 1)
        this->mHorzSpeed = FX_MUL(saved, this->mFloorNormalY);
    UpdatePos((dCc_c *)arg);
    this->mHorzSpeed = saved;
    func_ov002_020ce798(self);
}
}

// @symbol _ZN6Player19func_ov002_020bf40cEv
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
int Player::func_ov002_020bf40c() {
    extern unsigned char NumStars(void);
    extern void AddVec3(void *, void *, void *);
    extern void *data_0209f318;
    extern Vector3 data_ov002_0210a590;
    extern void func_0200d858(void *, const Vector3 *);
    Vector3 pos;
    Vector3 off;
    if (NumStars() < 0x50 || param1 != 0) {
        AddVec3(&mPos, &data_ov002_0210a590, &mPos);
        off.x = data_ov002_0210a590.x;
        off.y = data_ov002_0210a590.y;
        off.z = data_ov002_0210a590.z;
        func_0200d858(data_0209f318, &off);
        dBgCh_Gnd rg;
        {
            int py = this->mPosY;
            int pz = this->mPosZ;
            pos.x = this->mPosX;
            pos.y = py + 0xa0000;
            pos.z = pz;
        }
        rg.SetObjAndPos(pos, this);
        if (rg.DetectClsn()) {
            mGroundY = rg.clsnY;
        }
        return 1;
    }
    return 0;
}
}

// @symbol _ZN6Player4HealEi
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::Heal(int amt)
{

    extern Player::State data_ov002_0211010c;
    extern Player::State data_ov002_02110124;
    extern int GiveHealth(int, int);
  int r = this->IsState(data_ov002_0211010c);
  if(r) return r;
  r = this->IsState(data_ov002_02110124);
  if(r) return r;
  return GiveHealth(mPlayerNo, amt);
}
}

// @symbol _ZN6Player9GetHealthEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::GetHealth()
{
    extern short data_02092144[];
  unsigned char i = mPlayerNo;
  return (signed char)(data_02092144[i] >> 8);
}
}

// @symbol func_ov002_020bf56c
extern "C" {
int func_ov002_020bf56c(void* a, int b){
    extern int func_ov002_020bf56c(void*, int);
  int m;
  switch (((Player *)(a))->func_ov002_020c031c()) {
  case 4:
  case 5:
    m = 0x199;
    break;
  case 3:
    m = 0x599;
    break;
  default:
  case 0:
    m = 0x1000;
    break;
  case 1:
  case 2:
    m = 0x1000;
    break;
  }
  return FX_MUL(b, m);
}
}

// @symbol _ZN6Player19func_ov002_020bf5e0Ev
extern "C" {
void Player::func_ov002_020bf5e0()
{
    extern void Vec3_RotateYAndTranslate(void*, void*, short angle, void*);
    extern void PlayerNewSimpleHold2(unsigned int id, int x, int y, int z);
    extern u32 func_02022d00(u32 uniqueID, u32 effectID, int x, int y, int z, void*);
    extern int data_0209f32c;
    int flag;
    int id;
    Vector3 out;
    Vector3 src;
    int target;
    int newId;
    int state;

    if (mIsMega == 0)
        return;

    flag = 0;
    if (this->IsAnim(0x9e) || this->IsAnim(0xa4)) {
        id = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(0xe))
            goto set_flag;
        id = this->GetBodyModelID(param1 & 0xff, 0);
        if (!mModels.mBodyModels[id]->WillHitFrame(0x1c))
            goto flag_done;
set_flag:
        flag = 1;
    }
flag_done:
    if (!flag)
        return;

    id = this->GetBodyModelID(param1 & 0xff, 0);
    if (mModels.mBodyModels[id]->WillHitFrame(0x1c)) {
        src.x = 0x40000;
    } else {
        src.x = -0x40000;
    }
    src.y = 0x1b000;
    src.z = 0x40000;

    Vec3_RotateYAndTranslate(&out.x, &this->mPosX, this->mAngleY, &src.x);

    if (mIsInShallowWater != 0 || mIsUnderwater != 0) {
        Particle::System::NewSimple(0x64, out.x, out.y, out.z);
    } else {
        Particle::System::NewSimple(0x63, out.x, out.y, out.z);
    }

    Sound::PlayBank0(0xd0, mCamSpacePos);

    if (mIsInShallowWater != 0 && mIsUnderwater == 0) {
        newId = func_02022d00(mParticle1, 0x62, this->mPosX, data_0209f32c, this->mPosZ, 0);
        mParticle1 = newId;
    }

    state = mGroundSoundType;
    target = -1;
    if (state == 6)
        goto state_case6;
    if (state == 8)
        goto state_case8;
    if (state == 9) {
        target = 0xe6;
    }
    goto state_merge;
state_case8:
    target = 0xe4;
    goto state_merge;
state_case6:
    target = 0xe5;
state_merge:
    ;

    if (target == -1)
        return;

    src.y = 0;
    Particle::System::NewSimple(target, out.x, out.y, out.z);
}
}

// @symbol func_ov002_020bf800
extern "C" {
void func_ov002_020bf800(char *c, Vector3_16f v)
{
    typedef unsigned int u32;
    typedef short s16;
    extern u32 PlayerNewHold2(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                              void *callback);
    Player *player = (Player *)c;
    int xv = player->mPosX - v.x * 0x2bc;
    int zv = player->mPosZ - v.z * 0x2bc;
    int yv = player->mPosY - v.y * 0x2bc;
    Fix12v a(xv), b(yv), d(zv);
    player->mParticle1 = Particle::System::New(player->mParticle1, 0xe7, *(int *)&a, *(int *)&b,
                                               *(int *)&d, (const Vector3_16f *)(&v), 0);
}
}

// @symbol _ZN6Player19func_ov002_020bf88cEv
extern "C" {
void Player::func_ov002_020bf88c(){
    extern void func_02022b04(int x, int y, int z);
    extern int data_0208e430;
    if (mIsInShallowWater) return;
    if (data_0208e430 == 0x2e) return;
    if (mIsMega)
        func_02022b04(this->mPosX, this->mPosY, this->mPosZ);
    else
        Particle::RunningSlidingDustAt(this->mPosX, this->mPosY, this->mPosZ);
}
}

// @symbol _ZN6Player19func_ov002_020bf90cEv
extern "C" {
void Player::func_ov002_020bf90c()
{
    extern void func_02022b04(Fix12i x, Fix12i y, Fix12i z);
    extern int func_0201226c(int a0, int a1, int a2, int a3, int a4, short a5);
    extern int data_0208e430;
    if (this->mHorzSpeed == 0) return;
    if (mIsAirborne != 0) return;
    if (mIsInShallowWater != 0) return;
    if (data_0208e430 == 0x2e) return;

    if (mIsMega != 0)
        func_02022b04(this->mPosX, this->mPosY, this->mPosZ);
    else
        Particle::RunningSlidingDustAt(this->mPosX, this->mPosY, this->mPosZ);

    mLoopingSoundHandle = func_0201226c(mLoopingSoundHandle, 0, mGroundSoundType + 0xe2,
                                        (int)&mCamSpacePos, this->mHorzSpeed, 0);
}
}

// @symbol _ZN6Player19func_ov002_020bf9d4Ev
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
// func_ov002_020bf9d4 at 0x020bf9d4
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
void Player::func_ov002_020bf9d4(){
    extern int func_0200fccc(void*, int r1);
    extern void PlayerNewSimpleHold3(unsigned int, int, int, int);
    extern int data_0208e430;
  Vector3 v;
  if (data_0208e430 == 0x2e) return;
  if (mIsMega == 0){
    func_0200fccc(this, 0);
    return;
  }
  ((int*)&v)[0] = this->mPosX;
  ((int*)&v)[1] = this->mPosY;
  ((int*)&v)[2] = this->mPosZ;
  if (mIsInShallowWater != 0){
    v.y += 0x19000;
    Particle::System::NewSimple(0x5c, v.x, v.y, v.z);
  } else {
    v.y += 0x23000;
    Particle::System::NewSimple(0x5b, v.x, v.y, v.z);
  }
}
}

// @symbol _ZN6Player19func_ov002_020bfa74Ev
/* Player on a CLPS surface of type 0xb (a current/conveyor tile): the tile's
 * CLPS word names a two-node path whose direction is the push vector. Spawns the
 * flow particle (func_ov002_020bf800), keeps the surface loop sound alive, and
 * unless the player is metal, uncontrolled or in a blocked state, pushes the
 * player along the path: grounded states get a position nudge scaled by
 * data_ov002_020ff210[def->unk_004], airborne states get their horizontal
 * velocity bent toward the path direction. Returns 1 when the push applied. */
#define SIN(a) data_02082214[((u16)(a) >> 4) * 2]
#define COS(a) data_02082214[((u16)(a) >> 4) * 2 + 1]
extern "C" {
int Player::func_ov002_020bfa74()
{
    extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
    extern s16 Vec3_VertAngle(const Vector3 *v0, const Vector3 *v1);
    extern s32 Vec3_HorzLen(const Vector3 *v);
    extern s16 atan2_hold_Player_19(s32 y, s32 x);
    extern void PlayerPlayLongHold_bfa74(u32 handle, u32 bank, u32 id, const void *pos, s16 e);
    extern void func_ov002_020bf800(void*, Vector3_16f v);
    extern int func_ov002_020e0478(void*);
    extern s16 data_02082214[];
    extern s32 data_ov002_020ff210[];
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_02110574;
    extern Player::State data_ov002_02110184;
    extern Player::State data_ov002_02110514;
    extern Player::State data_ov002_021104e4;
    extern Player::State data_ov002_02110364;
    extern Player::State data_ov002_0211064c;
    extern Player::State data_ov002_021101b4;
    extern Player::State data_ov002_021102d4;
    s32 n;
    s32 i;
    u32 idx;
    s16 horz;
    s16 vert;
    s32 k;
    s32 scale;

    if (mSurfaceType != 0xb) return 0;
    if (mClpsWord1Lo != 0xff) {
        PathPtr path;
        Vector3_16f dir;
        Vector3 nodes[2];
        Vector3 vel;
        path.FromID(mClpsWord1Lo);
        n = path.NumNodes();
        for (i = 0; i < n; i++) {
            path.GetNode(nodes[i], i);
        }
        idx = (u8)path.GetUnk004();
        if (idx >= 4) idx = 1;
        horz = Vec3_HorzAngle(&nodes[0], &nodes[1]);
        vert = Vec3_VertAngle(&nodes[0], &nodes[1]);
        dir.y = SIN(vert);
        dir.x = FX_MUL(COS(vert), SIN(horz));
        dir.z = FX_MUL(COS(vert), COS(horz));
        func_ov002_020bf800(this, dir);
        mSurfaceSoundHandle =
            Sound::PlayLong(mSurfaceSoundHandle, 3, 0x90, *(Vector3 *)&mCamSpacePosX, 0);
        if (mIsMetal != 0 || mIsNoControl != 0 || func_ov002_020e0478(this) != 0) return 0;
        if (mIsAirborne == 0) {
            if (this->IsState(data_ov002_0211013c) || this->IsState(Player::ST_WAIT) || this->IsState(data_ov002_02110574) || this->IsState(data_ov002_02110184) || this->IsState(data_ov002_02110514) || this->IsState(data_ov002_021104e4) || this->IsState(data_ov002_02110364) || this->IsState(data_ov002_0211064c)) {
                if (mHorzSpeed <= 0) {
                    k = 0x8000;
                } else {
                    scale = data_ov002_020ff210[idx];
                    k = FX_MUL(scale, mHorzSpeed);
                }
                mPosX += FX_MUL(k, dir.x);
                mPosZ += FX_MUL(k, dir.z);
            }
        } else {
            if (this->IsState(data_ov002_021101b4) || this->IsState(data_ov002_021102d4)) return 1;
            vel.x = FX_MUL(mHorzSpeed, SIN(mPrevAngleY));
            vel.x += FX_MUL(dir.x, 0x2800);
            vel.z = FX_MUL(mHorzSpeed, COS(mPrevAngleY));
            vel.z += FX_MUL(dir.z, 0x2800);
            mPrevAngleY = cstd::atan2(vel.x, vel.z);
            mHorzSpeed = Vec3_HorzLen(&vel);
        }
        return 1;
    }
    return 0;
}
}
#undef SIN
#undef COS

// @symbol _ZN6Player19func_ov002_020bfec0Ev
extern "C" {
int Player::func_ov002_020bfec0()
{
    extern int func_ov002_020e3078(void*, void*);
    extern Player::State data_ov002_02110124;
    extern Player::State data_ov002_02110154;
    extern Player::State data_ov002_0211049c;
    extern Player::State data_ov002_02110424;
    extern char data_ov002_021104cc;
    int state = mSurfaceType;
    int inc;

    if (state == 1) {
        if (mIsMetal != 0) state = 8;
    }

    if (state < 6 || state > 9) {
        if (!this->IsState(data_ov002_02110124)) {
            mSinkDepth = 0;
        }
        return 0;
    }

    if (mIsAirborne != 0 || this->IsInAir() || mClimbClsn != 0 || mIsMega != 0 ||
        mIsNoControl != 0) {
        return 0;
    }

    mQuicksandRunFlags |= 1;

    {
        int flag = mRidingShell;
        flag = (flag != 0);
        if (flag) goto reset_timer;
        if (mQuicksandRunTimer == 0)
            goto after_timer_check;
    reset_timer:
        mSinkDepth = 0;
        return 0;
    after_timer_check:
        ;
    }

    inc = 0x400;
    if (this->IsState(data_ov002_02110154) || this->IsState(data_ov002_0211049c)) {
        inc = 0x800;
    }

    if (this->IsState(data_ov002_02110424)) {
        if (func_ov002_020e3078(this, &data_ov002_021104cc) == 0) {
            mQuicksandJumpTimer = 2;
        }
    }

    if (mSinkDepth < 0x1100) mSinkDepth = 0x1100;

    switch (state) {
    case 6:
        mSinkDepth += inc;
        if (mSinkDepth >= 0xa000) mSinkDepth = 0xa000;
        goto ret0;
    case 7:
        mSinkDepth += inc;
        if (mSinkDepth >= 0x3c000) mSinkDepth = 0x3c000;
        goto ret0;
    case 8:
        mSinkDepth += inc;
        if (mSinkDepth < 0xa0000) goto ret0;
        /* fallthrough */
    case 9:
        func_ov002_020c5dec(3);
        return 1;
    default:
        mSinkDepth = 0;
        goto ret0;
    }
ret0:
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c0108Ei
#define AT(p, off) ((void *)(int)((char *)(p) + (off)))
extern "C" {
int Player::func_ov002_020c0108(int p1)
{
    char *self = (char *)this;
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef short s16;
    typedef unsigned int u32;
    typedef int s32;
    extern int Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
    extern int Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
    extern Player::State data_ov002_02110304;
    extern Player::State data_ov002_0211025c;
    extern s32 data_ov002_020ff200[];
    extern s16 data_02082214[];
    if (this->mIsMetal != 0 || (this->mIsNoControl != 0 && p1 != 0) || this->IsState(data_ov002_02110304) || this->IsState(data_ov002_0211025c))
        return 0;

    if (this->mSurfaceType == 0xb)
        return 0;

    if (mClpsWord1Lo != 0xff) {
        s32 s;
        s32 iX, iY;
        s32 w;
        int idx;
        int numNodes;
        int i;
        int aH, aV;
        Vector3 nodes[3];
        PathPtr path;

        path.FromID(mClpsWord1Lo);
        numNodes = path.NumNodes();

        for (i = 0; i < numNodes; i++) {
            path.GetNode(nodes[i], i);
        }

        idx = path.GetUnk004() & 0xff;
        if ((u32)idx >= 4)
            idx = 1;

        aH = Vec3_HorzAngle(&nodes[0], &nodes[1]);
        aV = Vec3_VertAngle(&nodes[0], &nodes[1]);

        iY = (u16)aV >> 4;
        iX = (u16)aH >> 4;

        w = data_ov002_020ff200[idx] << 12;

        this->mPosY += FX_MUL(w, data_02082214[iY * 2]);
        s = FX_MUL(w, data_02082214[iY * 2 + 1]);
        this->mPosX += FX_MUL(s, data_02082214[iX * 2]);
        this->mPosZ += FX_MUL(s, data_02082214[iX * 2 + 1]);

        return 1;
    }

    return 0;
}
}
#undef AT

// @symbol _ZN6Player19func_ov002_020c031cEv
extern "C" {
int Player::func_ov002_020c031c(){

    extern Player::State data_ov002_02110514;
  int r4 = this->mFloorClass;
  if(this->IsState(data_ov002_02110514) && this->mFloorNormalY > 0x800){
    if(r4==0) r4=2;
  }
  return r4;
}
}

// @symbol _ZN6Player19func_ov002_020c0364Ej
extern "C" {
void Player::func_ov002_020c0364(u32 arg)
{
    extern int AngleDiff(int a, int b);
    extern Player::State data_ov002_02110334;
    extern Player::State data_ov002_0211031c;
    int flag;

    if (this->IsState(data_ov002_02110334)) return;
    if (this->IsState(data_ov002_0211031c)) return;

    flag = 0;
    if (arg == 3) {
        if (AngleDiff(this->mFloorAngle, this->mAngleY) < 0x4000)
            flag = 1;
    } else {
        if (arg & 1)
            flag = 1;
    }

    if (flag) {
        this->ChangeState(data_ov002_02110334);
    } else {
        this->ChangeState(data_ov002_0211031c);
        this->SetAnim(0x43, 0x40000000, 0x1000, 0);
    }
}
}

// @symbol _ZN6Player19func_ov002_020c0434Ev
int Player::func_ov002_020c0434(){
  unsigned short f = mStateFlags & 1;
  if(f){
    if(mFloorClass==5) return 1;
    if(this->mHorzSpeed<0) return 1;
    if(func_ov002_020c04ac()) return 1;
  }
  return 0;
}

// @symbol _ZN6Player19func_ov002_020c04acEv
extern "C" {
int Player::func_ov002_020c04ac(){
    extern int AngleDiff(short a, short b);
  return AngleDiff(mFloorAngle, this->mAngleY) <= 0x4000;
}
}

// @symbol _ZN6Player19func_ov002_020c04e0Ev
extern "C" {
int Player::func_ov002_020c04e0()
{
    extern int Vec3_HorzLen(void*);
    extern int AngleDiff(int a, int b);
    extern int func_ov002_020f035c(u32 sel, int r1);
    extern Player::State data_ov002_0211010c;
    extern Player::State data_ov002_02110304;
    int horzLen;
    int angDiff;
    int sel;
    int accelBase;
    int speed;
    int accel;
    int ret;

    horzLen = Vec3_HorzLen(&mFloorNormalX);
    angDiff = AngleDiff(mFloorAngle, this->mPrevAngleY);

    if (mWalkTimer != 0) {
        if (mFloorClass == 0)
            return mFloorClass;
    }

    sel = func_ov002_020c031c();
    ret = func_ov002_020f035c(sel, mFloorNormalY);
    if (ret == 0)
        return ret;

    sel = 0;
    if (!this->IsState(data_ov002_0211010c))
        sel = func_ov002_020c031c();
    if (this->IsState(data_ov002_02110304)) {
        if (sel == 4)
            sel = 0;
    }

    switch (sel) {
    case 4:
    case 5:
        accelBase = 0x54cc;
        break;
    case 3:
        accelBase = 0x2b33;
        break;
    case 0:
    default:
        accelBase = 0x1b33;
        break;
    case 1:
    case 2:
        accelBase = 0;
        break;
    }

    speed = this->mHorzSpeed - 0x13000;
    if (speed < 0x1000)
        speed = 0x1000;
    if (speed >= 0x1800)
        speed = 0x1800;

    accel = FX_MUL(accelBase, speed);

    if (angDiff < 0x4000) {
        int *p = &this->mHorzSpeed;
        int v = *p + FX_MUL(accel, horzLen);
        *p = v;
        return v;
    } else {
        int *p = &this->mHorzSpeed;
        int v = *p - FX_MUL(accel, horzLen);
        *p = v;
        return v;
    }
}
}

// @symbol _ZN6Player19func_ov002_020c0688Ev
int Player::func_ov002_020c0688(){
  if (mIsAirborne == 0) return 1;
  if (mSlideType != 0) {
    int d = this->mPosY - mGroundY;
    if (d < 0xb4000 && d >= 0) {
      this->mPosY = mGroundY;
      return 1;
    }
  } else if (mIsSlidingOnGround != 0) {
    int d = this->mPosY - mGroundY;
    if (d < 0x28000) {
      this->mPosY = mGroundY;
      return 1;
    }
  }
  return 0;
}

// @symbol _ZN6Player19func_ov002_020c06fcEi
extern "C" {
int Player::func_ov002_020c06fc(int arg)
{
    char *c = (char *)this;
    /* Stays a POD twin of Vector3, and this is the strongest case in the file:
       Vec3 is a MEMBER of the local struct Frame below, so a non-POD element
       gives Frame an implicit destructor, and mwccarm emits that as a new
       compiler-numbered local-class symbol -- measured as
       a D1 for Player::Frame$2583Player_cpp. This TU emits no @NNNN/$NNN symbols
       at all today, and such a name depends on declaration order (compare
       Bundle$902d_a_mky_cpp in AGENTS.md). Frame has to stay POD. */
    struct Vec3 { s32 x, y, z; };
    extern int func_ov002_020f030c(unsigned int sel);
    extern int func_ov002_020f02c8(unsigned int sel);
    extern int func_ov002_020f035c(unsigned int sel, int r1);
    extern int Vec3_HorzLen(void*);
    extern int atan2_hold_Player_18(int x, int z);
    extern int AngleDiff(int a, int b);
    extern void ApproachAngle(void*, short target, int divisor, int band, int maxStep);
    extern short data_0209f4a0[];
    extern char data_0209f4ac[];
    extern short data_02082214[];
    struct Frame {
        struct Vec3 v0;
        struct Vec3 v1;
        struct Vec3 v2;
        struct Vec3 v3;
    } f;
    int a;
    int t;
    int b;
    int v;
    int spd;
    int r5v;
    int r4v;
    int ang;

    if (this->mIsAirborne != 0)
        return 0;
    if ((this->mFloorNormalY | this->mHorzSpeed) == 0)
        return 0;

    a = 0;
    b = a;
    if (*(s16 *)((char *)data_0209f4a0 + gActivePlayerSlot * 0x18) >= 0x200) {
        s16 d = (s16)(this->mDesiredAngleY - this->mPrevAngleY);
        int j = ((u16)d >> 4) * 2;
        a = data_02082214[j + 1];
        b = data_02082214[j];
    }

    t = func_ov002_020f030c(this->func_ov002_020c031c());
    t = t + FX_MUL(*(s16 *)((char *)data_0209f4a0 + gActivePlayerSlot * 0x18), FX_MUL(a, 0x52));

    v = func_ov002_020f02c8(this->func_ov002_020c031c());
    {
        int j = ((u16)mPrevAngleY >> 4) * 2;
        spd = this->mHorzSpeed;
        r5v = FX_MUL(spd, (int)data_02082214[j]);
        r4v = FX_MUL(spd, (int)data_02082214[j + 1]);
    }

    if (this->mSlideType != 0) {
        if (*(u8 *)(data_0209f4ac + gActivePlayerSlot * 0x18) != 0) {
            int m = (b < 0) ? -b : b;
            s16 s2;
            if (m < 0x100)
                b = 0;
            s2 = *(s16 *)((char *)data_0209f4a0 + gActivePlayerSlot * 0x18);
            if (s2 < 0x400)
                mAngleYSpeed = FX_MUL(b, 0x64);
            else if (s2 < 0x800)
                mAngleYSpeed = FX_MUL(b, 0xC8);
            else if (s2 < 0xC00)
                mAngleYSpeed = FX_MUL(b, 0x190);
            else
                mAngleYSpeed = FX_MUL(b, 0x300);
        } else {
            mAngleYSpeed = FX_MUL(b, 0x220);
        }
    } else {
        mAngleYSpeed = (int)((((long long)b << 8) + 0x800) >> 12);
    }

    f.v0.x = FX_MUL(v, (int)data_02082214[((u16)mFloorAngle >> 4) * 2]);
    f.v0.z = FX_MUL(v, (int)data_02082214[((u16)mFloorAngle >> 4) * 2 + 1]);
    {
        int len = Vec3_HorzLen(&mFloorNormalX);
        f.v0.x = FX_MUL(f.v0.x, len);
        f.v0.z = FX_MUL(f.v0.z, len);
    }

    if ((this->mClsnFlags & 2) != 0 && (u16)(this->mStateFlags & 1) == 0 && this->mHorzSpeed <= 0x10000) {
        int r6b = cstd::atan2(f.v0.x, f.v0.z);
        dBgCh_Lin ray;
        {
            int tz, ty, tx;
            int p5c = this->mPosX;
            int p64 = this->mPosZ;
            int j = ((u16)r6b >> 4) * 2;
            int p60 = this->mPosY;
            struct Vec3 t;
            t.x = p5c; t.y = p60 + 0x32000; t.z = p64;
            f.v1.x = t.x; f.v1.y = t.y; f.v1.z = t.z;
            tx = data_02082214[j] * 0x46 + this->mPosX;
            tz = data_02082214[j + 1] * 0x46 + this->mPosZ;
            ty = this->mPosY + 0x32000;
            f.v2.x = tx; f.v2.y = ty; f.v2.z = tz;
        }
        ray.SetObjAndLine(*(const Vector3 *)&f.v1, *(const Vector3 *)&f.v2, this);
        if (ray.DetectClsn() != 0) {
            f.v0.x = 0;
            f.v0.z = 0;
        }
    }

    {
        int s1 = r5v + f.v0.x;
        int s2 = r4v + f.v0.z;
        int fx = FX_MUL(s1, t);
        int fz = FX_MUL(s2, t);
        f.v3.x = fx;
        f.v3.z = fz;
        ang = cstd::atan2(fx, fz);
    }
    this->mHorzSpeed = Vec3_HorzLen(&f.v3);
    if (this->mHorzSpeed > 0x64000)
        this->mHorzSpeed = 0x64000;
    this->mPrevAngleY = (s16)(ang + this->mAngleYSpeed);
    {
        s16 cur = this->mPrevAngleY;
        if (AngleDiff(cur, this->mAngleY) >= 0x4000)
            cur = (s16)(this->mPrevAngleY + 0x8000);
        ApproachAngle(&this->mAngleY, cur, 8, 0x4000, 0x10);
    }
    {
        int s = this->mHorzSpeed;
        if (s < 0)
            s = -s;
        if (s < arg) {
            if (func_ov002_020f035c(this->func_ov002_020c031c(), this->mFloorNormalY) == 0) {
                this->mHorzSpeed = 0;
                return 0;
            }
            if (this->mSlideType != 0)
                this->mHorzSpeed = arg;
        }
    }
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c0cbcEv
int Player::func_ov002_020c0cbc(){
  int t;
  int d;
  if((mClsnFlags & 1) == 0) return 0;
  d = mGroundY - this->mPosY;
  if(d < 0) d = -d;
  if(d >= 0x50000) return 0;
  switch(func_ov002_020c031c()){
    case 4: t = 0xfc1; break;
    case 5: t = 0xfff; break;
    case 3: t = 0xf04; break;
    default:
    case 0: t = 0xc9b; break;
    case 1:
    case 2: t = 0; break;
  }
  if(mFloorNormalY <= t){
    (this->mStateFlags) |= 1;
    return 1;
  }
  return 0;
}

// @symbol _ZN6Player19func_ov002_020c0d90Ei
extern "C" {
int Player::func_ov002_020c0d90(int y)
{
    char* c = (char*)this;
    extern u32 func_02022a4c(Fix12i x, Fix12i y, Fix12i z);
    extern u32 func_02022d00(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void*);
    extern void func_ov002_020ce9c8(void*);
    extern void func_ov002_020be008(void*);
    extern int data_0209f32c;
    extern Player::State data_ov002_021102d4;
    extern Player::State data_ov002_021106ac;
    extern Player::State data_ov002_021106c4;
    extern Player::State data_ov002_021102a4;
    extern Player::State data_ov002_0211067c;
    int flag = (this->mIsInShallowWater != 0);

    this->mIsInShallowWater = 0;
    if (y == (int)0x80000000) return 0;

    if (this->mIsUnderwater != 0) flag = 1;
    if (y >= this->mPosY + 0xa000) {
        this->mIsInShallowWater = 1;
    }

    if (flag != (this->mIsInShallowWater != 0)) {
        if (this->mIsAirborne != 0) {
            if (this->mIsMega == 0) {
                func_02022a4c(this->mPosX, y, this->mPosZ);
            } else {
                this->mParticle1 = func_02022d00(this->mParticle1, 0x56, this->mPosX, data_0209f32c, this->mPosZ, 0);
            }
        }
    }

    if (((y - 0x64000) >> 12) < (this->mPosY >> 12)) return 0;
    if (this->mVertSpeed > 0) return 0;

    if (this->mIsBalloon != 0 || (this->IsState(data_ov002_021102d4) && this->mStateStep < 2) || this->mIsUnderwater != 0 || this->IsState(data_ov002_021106ac) || this->IsState(data_ov002_021106c4) || this->IsState(data_ov002_021102a4)) {
        return 0;
    }

    if (this->func_ov002_020c7f10()) return 1;

    this->mWalkTimer = 0xf;
    if (this->mIsMetal != 0 || this->mIsMega != 0) {
        this->ChangeState(data_ov002_021106ac);
    } else {
        this->ChangeState(data_ov002_0211067c);
        this->mJumpComboTimer = 0x14;
    }

    func_ov002_020ce9c8(c);
    func_ov002_020be008(c);
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c0fb4Ev
extern "C" {
int Player::func_ov002_020c0fb4() {
    extern int func_ov002_020dec70(void *);
    extern int func_ov002_020d06c0(void *);
    extern unsigned char data_ov002_02111184[];
    extern Timer data_0209d4c8;
    extern signed char data_0209f2f8;
    int r;
    switch (this->mSurfaceType) {
    case 4:

    case 5:
        if ((r = this->func_ov002_020c5d0c()) != 0) {
            return r;
        }
        break;

    case 19:
        if ((r = func_ov002_020dec70(this)) != 0) {
            return r;
        }
        if ((r = this->func_ov002_020c5d0c()) != 0) {
            return r;
        }
        break;

    case 18:
        if ((r = this->func_ov002_020bf40c()) != 0) {
            return r;
        }
        break;

    case 15:
        if (data_ov002_02111184[0] != 0) {
            break;
        }
        data_0209d4c8.ResetTimer();
        data_ov002_02111184[0] = 1;
        data_0209d4c8.StartTimer();
        break;

    case 16:
        if (data_0209d4c8.mIsRunning == 0) {
            break;
        }
        data_0209d4c8.StopTimer();
        {
            unsigned long long t = data_0209d4c8.GetTime();
            if (((t << 6) / 0x1ff6210) > 0x14) {
                break;
            }
        }
        {
            Vector3 pos;
            int px;
            int py;
            int pz;
            py = this->mPosY;
            pz = this->mPosZ;
            px = this->mPosX;
            py = (py + 0x82000) & 0xFFFFFFFFFFFFFFFF;
            pos.x = px;
            pos.y = py;
            pos.z = pz;
            Spawn(0xb2, 0x42, pos, 0, this->mAreaId, -1);
        }
        break;
    }

    if (data_0209f2f8 == 0x26) {
        if ((this->mPosY) < (-0x3e8000)) {
            this->mPosY = -0x3e8000;
            if ((r = this->func_ov002_020d5338()) != 0) {
                return r;
            }
        }
    }
    if (this->mMeshClsn.IsOnGround()) {
        if (this->mProbedSurfaceType == 1) {
            int d = (this->mPosY) - (this->mProbedGroundY);
            if (d < 0) {
                d = -d;
            }
            if (d < 0x2000) {
                if ((r = this->func_ov002_020d5338()) != 0) {
                    return r;
                }
            }
        }
        if ((r = this->func_ov002_020bfec0()) != 0) {
            return r;
        }
    }
    if ((r = this->func_ov002_020c0d90(this->mWaterSurfaceY)) != 0) {
        return r;
    }
    if ((r = func_ov002_020d06c0(this)) != 0) {
        return r;
    }
    return this->func_ov002_020c0cbc();
}
}

// @symbol _ZN6Player19func_ov002_020c1234Ev
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" {
void Player::func_ov002_020c1234() {
    extern signed char data_0209f2f8;
    extern int data_0209b49c;
    unsigned int a1, a2;
    int fx;
    if (data_0209f2f8 != 0xc && data_0209f2f8 != 0x2e) return;
    switch (mGroundSoundType) {
    case 0xa:
        a1 = 0x2d;
        a2 = 0x28;
        fx = (data_0209b49c >> 12 > 0x28) ? 0x8b33 : 0xd1e;
        mGroundSubSoundActive = 1;
        break;
    case 0xb:
        mGroundSubSoundActive = 1;
        a1 = 0;
        a2 = 0x7f;
        fx = 0x8b33;
        break;
    default:
        fx = 0x1451;
        a1 = 0x7f;
        a2 = 0;
        break;
    }
    if (mGroundSubSoundActive == 0)
        return;
    if (Sound::PlaySub(0x35, a1, a2, fx, 0) == 0)
        return;
    if (mGroundSoundType == 0xa) return;
    if (mGroundSoundType != 0xb)
        mGroundSubSoundActive = 0;
}
}

// @symbol _ZN6Player19func_ov002_020c133cEv
extern "C" {
void Player::func_ov002_020c133c()
{
    typedef unsigned char u8;
    extern int func_ov002_020d82f0(void*);
    extern void PlayerPlayLongHold_c133c(unsigned int a, unsigned int b, unsigned int c,
                                         Vector3 const *v, unsigned int d);
    extern int func_ov002_020d91e0(void*, int damage, int doPre);
    if (mIsMetal != 0) return;
    {
        int lim = mToxicSurfaceY;
        if (lim == (int)0x80000000) goto set0;
        if (this->mPosY >= lim - 0x64000) goto set0;
    }
    if (!func_ov002_020d82f0(this)) return;
    if (mIsAirborne != 0) return;
    if (this->GetHealth() <= 2)
        mSurfaceSoundHandle =
            Sound::PlayLong(mSurfaceSoundHandle, 2, 0x27, *(Vector3 *)&mCamSpacePos, 0);
    if (mDamageTickTimer != 0) {
        mDamageTickTimer--;
        return;
    }
    {
        Vector3 v;
        v.x = this->mPosX;
        v.y = this->mPosY;
        v.z = this->mPosZ;
        if (!this->Hurt(v, 0, 0, 1, 0, 0))
            return;
    }
    Sound::PlayCharVoice(mCharacter, 6, mCamSpacePos);
    if (func_ov002_020d91e0(this, 0x100, 0) != 0 && this->GetHealth() == 0)
        func_ov002_020c5dec(5);
    mDamageTickTimer = 0x40;
    return;
set0:
    mDamageTickTimer = 0;
}
}

// @symbol _ZN6Player19func_ov002_020c14b8Ev
extern "C" {
void Player::func_ov002_020c14b8()
{
    void *arg0 = (void *)this;
    typedef struct { int a, b; } Pair2i;
    typedef struct {
        void *tag;
        int f04, f08, f0c, f10, f14;
        unsigned short f18, f1a;
        int f1c, f20, f24;
    } ClsnResultTmp;
    extern int SurfaceInfo_TestFlag0x20(void*);
    extern int func_02037e20(void *);
    extern int data_02099368;
    extern int data_0209f32c;
    Player *c = (Player *)arg0;
    dBgCh_Gnd rg;
    Vector3 pos;
    ClsnResultTmp tmp;
    int savedY;

    savedY = 0x80000000;
    if (c->Unk_020c9e5c(7) || c->Unk_020c9e5c(0xa) || c->Unk_020c9e5c(0xb) || c->Unk_020c9e5c(0xe))
        savedY = mPosY;
    mProbedGroundY = 0x80000000;
    mGroundY = mProbedGroundY;
    mWaterSurfaceY = 0x80000000;
    {
        int t = mPosY + 0xa0000;
        if (t >= mToxicSurfaceY)
            mToxicSurfaceY = 0x80000000;
    }
    mProbedSurfaceType = 0;
    mSurfaceType = mProbedSurfaceType;
    mFloorClass = 0;
    {
        int z = mPosZ;
        int y = mPosY + 0xa0000;
        int x = mPosX;
        pos.x = x;
        pos.y = y;
        pos.z = z;
    }
    rg.SetObjAndPos(pos, this);
    if (rg.DetectClsn()) {
        int *dst = &tmp.f04;
        {
            struct P2w {
                int w[2];
            };
            *(P2w *)dst = *(P2w *)&rg.surface.clps;
        }
        dst[2] = rg.surface.normal.x;
        dst[3] = rg.surface.normal.y;
        dst[4] = rg.surface.normal.z;
        tmp.tag = &data_02099368;
        tmp.f18 = rg.triangleID;
        tmp.f1a = rg.colliderIdx;
        tmp.f1c = rg.clsnID;
        tmp.f20 = (int)rg.owner;
        tmp.f24 = (int)rg.collider;
        SetFloorSurfaceInfo((const dBgPi &)tmp);
        ((dBgPi *)&tmp)->dBgPi::~dBgPi();
        mProbedGroundY = rg.clsnY;
        mGroundY = mProbedGroundY;
        mProbedSurfaceType = mSurfaceType;
    }

    pos.y = mPosY + 0x12c000;
    rg.StartDetectingWater();
    rg.StartDetectingToxic();
    rg.StopDetectingOrdinary();
    rg.SetObjAndPos(pos, this);
    if (rg.DetectClsn()) {
        if (SurfaceInfo_TestFlag0x20(&rg.surface) != 0) {
            mWaterSurfaceY = rg.clsnY;
            data_0209f32c = mWaterSurfaceY;
        }
        if (func_02037e20((int *)&rg.surface) != 0) {
            mToxicSurfaceY = rg.clsnY;
        }
    }

    mStateFlags &= ~1;
    mStateFlags &= ~0x10;
    c->func_ov002_020c133c();

    if (savedY != (int)0x80000000)
        mGroundY = savedY;
}
}

// @symbol _ZN6Player19SetFloorSurfaceInfoERK5dBgPi
extern "C" {
void Player::SetFloorSurfaceInfo(const dBgPi &hit) {
    extern int atan2_hold_Player_17(int y, int x);
    Vector3 v;
    mClpsWord1Lo = hit.surface.clps.func_02037e90();
    mSurfaceType = hit.surface.clps.GetSurfaceType();
    mClpsBits15_18 = hit.surface.clps.func_02037e48();
    mClpsBits6_11 = hit.surface.clps.func_02037e68();
    mGroundSoundType = hit.surface.clps.func_02037e84();
    if (mFloorClass == 1)
        return;
    mFloorClass = hit.surface.clps.GetFloorClass();
    hit.surface.CopyNormalTo(v);
    mFloorNormalX = v.x;
    mFloorNormalY = v.y;
    mFloorNormalZ = v.z;
    mFloorAngle = cstd::atan2(mFloorNormalX, mFloorNormalZ);
}
}

// @symbol func_ov002_020c179c
/* recovered: shared common types */
// func_ov002_020c179c at 0x020c179c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
int func_ov002_020c179c(char* self, int angle) {
    extern int atan2_hold_Player_16(int y, int x);
    extern short data_02082214[];
    Player *p = (Player *)self;
    Vector3 v1;
    Vector3 v2;
    int idx = (unsigned short)(short)(p->mPrevAngleY + angle);
    int y1 = p->mPosY;
    int y2 = y1;
    idx >>= 4;
    short dx = data_02082214[idx * 2];
    short dy = data_02082214[idx * 2 + 1];
    int ox = dx * 5;
    int oy = dy * 5;
    dBgCh_Gnd rg;
    {
        int z = p->mPosZ + oy;
        int y = p->mPosY + 0x64000;
        int x = p->mPosX + ox;
        v1.x = x;
        v1.y = y;
        v1.z = z;
    }
    rg.SetObjAndPos(v1, (dActor_c*)self);
    if (rg.DetectClsn()) y1 = rg.clsnY;
    {
        int z = p->mPosZ - oy;
        int x = p->mPosX - ox;
        int y = p->mPosY + 0x64000;
        v2.x = x;
        v2.y = y;
        v2.z = z;
    }
    rg.SetObjAndPos(v2, (dActor_c*)self);
    if (rg.DetectClsn()) y2 = rg.clsnY;
    {
        int r = cstd::atan2(y1 - y2, 0xa000);
        return r;
    }
}
}

// @symbol func_ov002_020c18b0
extern "C" {
void func_ov002_020c18b0(char *self, int p1)
{
    extern int func_ov002_020c179c(void*, int angle);
    extern int func_02010844(void*, void*, short angle);
    extern int Player_ScaleByCharFactor(void*, int a);
    extern int __aeabi_idiv(int a, int b);
    extern short data_02082214[];
    Player *p = (Player *)self;
    if (p->mIsAirborne != 0)
        return;
    if (p->mFloorNormalY == 0)
        return;
    {
        int a = func_ov002_020c179c(self, 0);
        int idx = (unsigned short)a >> 4;
        int sinv = data_02082214[idx * 2 + 1];
        int f554 = p->mFloorNormalX;
        int f55c = p->mFloorNormalZ;
        Vector3 v;
        v.x = f554;
        v.y = sinv;
        v.z = f55c;
        p->mAngleX = (short)func_02010844(self, &v, p->mAngleY);
        p->mAngleZ = (short)func_02010844(self, &v, (short)(p->mAngleY - 0x4000));
    }
    if (p1 == 0) return;
    {
        int r0v = Player_ScaleByCharFactor(self, 0x28000);
        int divr = __aeabi_idiv(p->mHorzSpeed << 12, r0v);
        p->mAngleX = (short)FX_MUL(p->mAngleX, divr);
        p->mAngleZ = (short)FX_MUL(p->mAngleZ, divr);
    }
}
}

// @symbol _ZN6Player19func_ov002_020c19d0Eii
extern "C" {
int Player::func_ov002_020c19d0(int arg1, int arg2) {
    char* self = (char*)this;
    extern s16 data_02082214[];
    int angle = (unsigned short)mAngleY;
    dBgCh_Lin line;
    int k = angle >> 4;
    int cs = data_02082214[k * 2];
    int sn = data_02082214[k * 2 + 1];
    Vector3 v1(this->mPosX, this->mPosY + (arg2 << 12), this->mPosZ);
    Vector3 v2(arg1 * cs + this->mPosX, this->mPosY + (arg2 << 12), arg1 * sn + this->mPosZ);
    Vector3 nrm;
    line.SetObjAndLine(v1, v2, (dActor_c *)self);
    if (!line.DetectClsn()) {
        return 0;
    }
    line.surface.CopyNormalTo(nrm);
    if (nrm.y != 0) {
        return 0;
    }
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c1ad8Ei
/* Player::func_ov002_020c1ad8(int) at 0x020c1ad8 (ov002)
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 */
extern "C" {
int Player::func_ov002_020c1ad8(int ang)
{
    char* self = (char*)this;
    extern int AngleDiff(int a, int b);
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_0211043c;
    extern s16 data_0209f4a0[];
    int d;
    int a = ang;

    if (this->mIsAirborne != 0) return 0;
    if (this->func_ov002_020c0434() != 0) {
        this->func_ov002_020c0364(3);
        return 0;
    }
    if (!this->IsState(data_ov002_0211013c)) {
        if (!this->IsState(data_ov002_0211043c)) return 0;
    }
    if (this->func_ov002_020c19d0(0x50, 0x32) == 0) return 0;

    a = (int)((long long)(a + 0x8000));
    d = AngleDiff(this->mDesiredAngleY, (s16)a);
    if (d < 0x4000) {
        if (d < 0x1555) {
            if (this->IsState(data_ov002_0211043c)) {
                if (this->mStateStep == 2) return 0;
            }
            if (*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) == 0) return 0;
            this->mStateStep = 2;
        } else {
            if (this->IsState(data_ov002_0211043c)) {
                if (this->mStateStep != 2) return 0;
            }
            this->mStateStep = 0;
            if ((s16)a == (s16)(this->mAngleY + d)) {
                this->mStateStep = 1;
            }
        }
        mAngleY = a;
        this->ChangeState(data_ov002_0211043c);
        return 1;
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c1c84Ei
extern "C" {
int Player::func_ov002_020c1c84(int arg)
{
    char* self = (char*)this;
    extern int func_ov002_020cf700(void*);
    extern int func_ov002_020e0478(void*);
    extern int AngleDiff(int a, int b);
    extern Player::State data_ov002_02110694;
    extern Player::State data_ov002_021103ac;
    extern Player::State data_ov002_021101b4;
    int a;
    int p;

    if ((u16)(this->mStateFlags & 0x200) || !this->mIsAirborne || func_ov002_020cf700(self) || this->mIsMega || func_ov002_020e0478(self) || this->mEatingPlayer || this->mIsUnderwater)
        return 0;

    if (AngleDiff(arg, this->mPrevAngleY) < 0x6000)
        return 0;

    if (this->IsState(data_ov002_02110694) || mIsTakingDamage || this->mIsNoControl)
        return 0;

    if (this->mHorzSpeed < 0x3000)
        return 0;

    if (this->mVertSpeed > 0)
        return 0;

    a = arg + 0x8000;
    p = this->mPrevAngleY;
    this->mAngleYSpeed = (s16)(p - (s16)(p - a) * 2);
    this->mAngleY = (s16)a;
    this->mPrevAngleY = this->mAngleY;

    if (param1 == 0) {
        if (this->func_ov002_020c19d0(0x3c, 0x78))
            this->ChangeState(data_ov002_021103ac);
    } else {
        if (this->func_ov002_020c19d0(0x64, 0x96)) {
            this->mHorzSpeed = -0x12000;
            this->ChangeState(data_ov002_021101b4);
        } else {
            this->mHorzSpeed = 0;
        }
    }
    return 1;
}
}

// @symbol func_ov002_020c1e44
extern "C" {
int func_ov002_020c1e44(void* a, int x) {
    Player *p = (Player *)a;
    p->mPrevAngleY = (short)((x << 1) - p->mPrevAngleY + 0x8000);
    p->mHorzSpeed = FX_MUL(p->mHorzSpeed, 0xe00);
    if (p->mHorzSpeed < 0x4000)
        p->mHorzSpeed = 0x4000;
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c1eb4Ei
/* recovered: shared common types */
extern "C" {
void Player::func_ov002_020c1eb4(int dmg) {
    typedef int Fx12;
    extern void Vec3_RotateYAndTranslate(void*, const Vector3 *origin, int angle, const Vector3 *offset);
    extern short data_02082214[];
    extern Player::State data_ov002_02110094;
    extern int func_ov002_020d91e0(void*, int damage, int doPre);
    extern u32 PlayerNewHold3(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                              void *callback);
    Vector3 out;
    Vector3 offset;
    Vector3_16 vec;

    Sound::PlayBank0(0x13, mCamSpacePos);
    mAngleY = mPrevAngleY;
    mAngleZ = 0;
    mAngleX = mAngleZ;
    mStateStep = 0;
    if (mIsAirborne) {
        mStateStep += 2;
    }
    mAngleY = (short)(dmg * 2 - mAngleY);
    mPrevAngleY = (short)(mAngleY + 0x8000);
    mHorzSpeed = 0xa000;
    mStateStep |= 0x20;
    mHurtDamage = 0;
    this->ChangeState(data_ov002_02110094);

    offset.x = 0;
    offset.y = 0x32000;
    offset.z = 0x28000;
    Vec3_RotateYAndTranslate(&out, &mPos, (short)(dmg + 0x8000), &offset);

    {
        int idx = ((unsigned short)dmg >> 4);
        vec.x = data_02082214[idx * 2];
        vec.z = data_02082214[idx * 2 + 1];
        vec.y = 0;
        Particle::System::New(0, 0xc8, out.x, out.y, out.z, (const Vector3_16f *)(&vec), 0);
    }
    func_ov002_020d91e0((char *)this, 0, 1);
}
}

// @symbol _ZN6Player19func_ov002_020c200cEi
extern "C" {
int Player::func_ov002_020c200c(int arg)
{
    char* self = (char*)this;
    extern int func_ov002_020c1e44(void*, int x);
    extern void func_ov002_020eedc0(void*, void*);
    extern Player::State data_ov002_021105bc;
    extern Player::State data_ov002_0211031c;
    extern Player::State data_ov002_02110334;
    extern Player::State data_ov002_0211055c;
    extern Player::State data_ov002_02110214;
    if (this->mSlideType == 1)
        return func_ov002_020c1e44(self, arg);

    if (this->mIsNoControl)
        return 0;

    if (this->IsState(data_ov002_021105bc) || this->IsState(data_ov002_0211031c) ||
        this->IsState(data_ov002_02110334) || this->IsState(data_ov002_0211055c) ||
        this->IsBeingShotOutOfCannon() || this->IsState(data_ov002_02110214)) {
        if (this->mHorzSpeed <= 0x10000)
            return 0;

        if (this->IsBeingShotOutOfCannon())
            func_ov002_020eedc0(&mMeshClsn, self);
    } else {
        return 0;
    }

    this->func_ov002_020c1eb4(arg);
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c2138Ev
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
int Player::func_ov002_020c2138(){
    char* self = (char*)this;
    typedef int Fix12i;
    extern int atan2_hold_Player_15(Fix12i, Fix12i);
    extern int AngleDiff(int a, int b);
int ang;
if (this->mIsUnderwater || this->mIsBalloon) return 0;
if (this->mWallNormalY) return 0;
if ((int)mClimbClsn || mIsTakingDamage)
    return 0;
ang = cstd::atan2(this->mWallNormalX, this->mWallNormalZ);
if (AngleDiff(ang, this->mPrevAngleY) < 0x6000) return 0;
if (this->func_ov002_020c200c(ang)) return 1;
if ((this->mHeldObj ? 1 : 0) || (this->mRidingShell ? 1 : 0)) return 0;
if (this->func_ov002_020c1c84(ang)) return 1;
return this->func_ov002_020c1ad8(ang);
}
}

// @symbol func_ov002_020c2270
/* recovered: shared common types */
extern "C" {
int func_ov002_020c2270(Player *c, void *p1, void *p2) {
    extern int func_ov002_020c2270(void*, void*, void*);
    dBgCh_Lin line;
    line.SetObjAndLine(*(Vector3*)p1, *(Vector3*)p2, (dActor_c*)c);
    if (line.DetectClsn()) {
        if (line.surface.clps.GetFloorClass() == 1) {
            ::Vector3 clsnPos = line.GetClsnPos();
            c->mPosX = clsnPos.x;
            c->mPosY = clsnPos.y;
            c->mPosZ = clsnPos.z;
            c->mSnapToFloor = 1;
            return 1;
        }
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c231cEv
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
int Player::func_ov002_020c231c()
{
    extern void SubVec3(void*, void*, void*);
    extern void AddVec3(void*, void*, void*);
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_02110304;
    extern Player::State data_ov002_02110364;
    extern Player::State data_ov002_021104e4;
    extern Player::State data_ov002_02110514;
    extern Player::State data_ov002_021100dc;
    extern s16 data_02082214[];
    extern Player::State data_ov002_02110184;
    extern int func_ov002_020c2270(void*, void*, void*);
    Vector3 tmp;
    Vector3 pos;
    u8 f70f;
    s32 p60;
    s32 d;
    s32 p644;
    s32 xx;
    s32 zz;
    int idx;

    f70f = mSnapToFloor;

    if (mGroundY == (s32)0x80000000) {
        if (f70f == 0)
            return 0;

        SubVec3(&mPos, (Vector3 *)&mPreClsnPosX, &tmp);
        tmp.x >>= 1;
        tmp.y >>= 1;
        tmp.z >>= 1;
        AddVec3((Vector3 *)&mPreClsnPosX, &tmp, &mPos);
        func_ov002_020c14b8();

        if (mGroundY == (s32)0x80000000)
            return 0;
    }

    if (mWaterSurfaceY != (s32)0x80000000)
        return 0;

    if (this->IsState(data_ov002_0211013c) || this->IsState(data_ov002_02110184) || this->IsState(data_ov002_02110304) || (this->IsState(data_ov002_02110364) && (this->IsAnim(0x33) | this->IsAnim(0x87)) == 0) || this->IsState(data_ov002_021104e4) || this->IsState(data_ov002_02110514) || this->IsState(data_ov002_021100dc))
    {
        if (mFloorClass == 1 && mFloorNormalY < 0xfff) {
            p644 = mGroundY;
            p60 = this->mPosY;
            d = p60 - p644;
            if (d < 0) d = -d;
            if (d < 0xc8000) {
                this->mPosY = p644;
                mSnapToFloor = 1;
                return 1;
            }

            if (f70f == 0) {
                idx = (u16)(s16)(this->mPrevAngleY + 0x8000) >> 4;
                zz = data_02082214[idx * 2 + 1] * 0x50 + this->mPosZ;
                xx = data_02082214[idx * 2] * 0x50 + this->mPosX;
                pos.x = xx;
                pos.y = p60;
                pos.z = zz;
                if (func_ov002_020c2270(this, &mPos, &pos))
                    return 1;
            }
        }

        if (f70f == 1) {
            d = this->mPosY - mGroundY;
            if (d < 0) d = -d;
            if (d < 0xc8000)
                this->mPosY = mGroundY;
        }
    }

    mSnapToFloor = 0;
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c25a8Ei
extern "C" {
int Player::func_ov002_020c25a8(int arg1)
{
    void *arg0 = (void *)this;
    typedef struct {
        int a, b;
    } Pair2i;
    typedef struct {
        void *tag;
        int f04;
        int f08;
        int f0c;
        int f10;
        int f14;
        unsigned short f18;
        unsigned short f1a;
        int f1c;
        int f20;
        int f24;
    } ClsnResultTmp;
    extern void func_02037318(void*);
    extern void func_020357e0(void*);
    extern void func_02035800(void*);
    extern void func_02035770(void*);
    extern void func_02035784(void*);
    extern void func_020357a0(void*);
    extern void func_020357c0(void*);
    extern void func_020355e8(void*);
    extern void func_020355fc(void*);
    extern void func_020355c8(void*);
    extern void func_020355b4(void*);
    extern void func_ov002_020eed24(void*, void*);
    extern int func_02035638(void*);
    extern void func_020371fc(void *);
    extern int func_020356c8(void*);
    extern void func_ov002_020cef84(void*);
    extern int func_ov002_020d0580(void*);
    extern Player::State data_ov002_02110514;
    extern Player::State data_ov002_021100f4;
    extern Player::State data_ov002_021102ec;
    extern int data_02099368;
    Player *c = (Player *)arg0;
    int ret;
    Vector3 v;
    Vector3 wn;
    ClsnResultTmp tmp;
    dBgPi *fr;
    int *dst;

    func_02037318(&mMeshClsn);
    mMeshClsn.StopDetectingWater();

    {
        int b1;
        b1 = mRidingShell;
        b1 = (b1 != 0);
        if (b1 || mDetectWater || mIsBalloon)
            mMeshClsn.StartDetectingWater();
    }

    func_020357e0(&mMeshClsn);

    if (mIsVanish)
        func_02035800(&mMeshClsn);

    func_02035770(&mMeshClsn);

    if (mIsMega)
        func_02035784(&mMeshClsn);

    func_020357a0(&mMeshClsn);

    if (c->IsState(data_ov002_02110514))
        func_020357c0(&mMeshClsn);

    func_020355e8(&mMeshClsn);

    {
        int b2;
        b2 = mRidingShell;
        b2 = (b2 != 0);
        if (b2)
            func_020355fc(&mMeshClsn);
    }

    mMeshClsn.ClearLimMovFlag();

    if (c->IsState(data_ov002_021100f4) && mVertSpeed > 0)
        mMeshClsn.SetLimMovFlag();

    func_020355c8(&mMeshClsn);

    if (c->IsState(data_ov002_021102ec))
        func_020355b4(&mMeshClsn);

    func_020383e4(&mMeshClsn);

    if (mIsMega)
        func_ov002_020eed24(&mMeshClsn, c);

    if (mVertSpeed > 0x1e000)
        mMeshClsn.ClearGroundFlag();

    if (mMeshClsn.IsOnGround())
        mClsnFlags |= 1;

    if (mMeshClsn.IsOnWall())
        mClsnFlags |= 2;

    if (func_02035638(&mMeshClsn))
        mClsnFlags |= 4;

    if (mIsUnderwater == 0)
        func_020371fc(&mMeshClsn);

    if (mClsnFlags & 1) {
        fr = mMeshClsn.GetFloorResult();
        dst = &tmp.f04;

        {
            struct P2w {
                int w[2];
            };
            *(P2w *)dst = *(P2w *)&fr->surface.clps;
        }
        dst[2] = fr->surface.normal.x;
        dst[3] = fr->surface.normal.y;
        dst[4] = fr->surface.normal.z;
        tmp.tag = &data_02099368;
        tmp.f18 = fr->triangleID;
        tmp.f1a = fr->colliderIdx;
        tmp.f1c = fr->clsnID;
        tmp.f20 = (int)fr->owner;
        tmp.f24 = (int)fr->collider;

        SetFloorSurfaceInfo((const dBgPi &)tmp);
        ((dBgPi *)&tmp)->dBgPi::~dBgPi();

        mGroundY = mPosY;
        ret = fr->surface.clps.GetFloorClass();

        if (mIsUnderwater == 0 && ret != 1) {
            fr->surface.CopyNormalTo(v);
            if (v.y != 0) {
                int t = cstd::fdiv(FX_MUL(v.x, mSpeedX) + FX_MUL(v.z, mSpeedZ), v.y);
                t = -(t + 0x8000);
                if (t < 0)
                    mVertSpeed = t;
            }
        }
        mIsAirborne = 0;
        mIsInAirState = 0;
    } else {
        if (func_020356c8(&mMeshClsn))
            mPeakY = mPosY;
        mIsAirborne = 1;
        mLandSoundPlayed = 0;
        if (func_02035638(&mMeshClsn) && mVertSpeed > 0)
            mVertSpeed = -0x1000;
        func_ov002_020cef84(c);
    }

    if (arg1 != 0) {
        mClsnFlags |= 1;
        mIsAirborne = 0;
        mIsInAirState = 0;
    }

    {
        int wallFlag = mClsnFlags & 2;
        if (wallFlag == 0)
            return wallFlag;
    }

    mMeshClsn.GetWallResult()->surface.CopyNormalTo(wn);
    mWallNormalX = wn.x;
    mWallNormalY = wn.y;
    mWallNormalZ = wn.z;

    if (mIsUnderwater == 0) {
        mPosX -= mWallNormalX * 2;
        mPosZ -= mWallNormalZ * 2;
    }

    ret = func_ov002_020d0580(c);
    if (ret != 0)
        return ret;

    ret = c->func_ov002_020c2138();
    if (ret != 0)
        return ret;
}
}

// @symbol _ZN6Player19func_ov002_020c29d4Ev
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" {
void Player::func_ov002_020c29d4()
{
    extern int Player_ScaleByCharFactor(void*, int a);
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_021101b4;
    char *base = (char *)this;
    if (param1 != 1)
        return;
    if (this->mIsMega != 0) return;
    unsigned char flags = mQuicksandRunFlags;
    if ((flags & 1) && !(flags & 0x80)) {
        if (this->mHorzSpeed >= Player_ScaleByCharFactor(this, 0x28000)) {
            if (this->IsState(data_ov002_0211013c)) {
                mQuicksandRunTimer = 0x1e;
            } else if (this->IsState(data_ov002_021101b4) && this->mVertSpeed < 0 && (this->mPeakY - this->mPosY) < 0x64000) {
                mQuicksandRunTimer = 0x1e;
            }
        }
        mQuicksandRunFlags = 0x80;
    } else if (flags & 0x80) {
        if (mMeshClsn.IsOnGround()) {
            int r = mMeshClsn.GetFloorResult()->surface.clps.GetSurfaceType();
            if (r >= 6 && r <= 9) return;
            mQuicksandRunFlags = 0;
            mQuicksandRunTimer = 0;
        } else {
            mQuicksandRunFlags = 0;
            mQuicksandRunTimer = 0;
        }
    }
}
}

// @symbol _ZN6Player19func_ov002_020c2b08Ev
extern "C" {
void Player::func_ov002_020c2b08()
{
    void *arg0 = (void *)this;
    extern int SurfaceInfo_TestFlag0x20(void*);
    extern int Player_ScaleByCharFactor(void*, int a);
    extern Player::State data_ov002_0211013c;
    Player *c = (Player *)arg0;
    int v[3];

    if (param1 != 1)
        return;
    if (mIsMega != 0)
        return;

    if (mWaterRunLatch == 0) {
        int limit;

        if (!mMeshClsn.IsOnGround())
            return;
        if (!SurfaceInfo_TestFlag0x20((int *)&mMeshClsn.GetFloorResult()->surface.clps))
            return;

        limit = Player_ScaleByCharFactor(c, 0x20000);
        if (mHorzSpeed >= limit)
            mWaterRunTimer = 0x3c;
        else
            mWaterRunTimer = 0x1e;
        mWaterRunLatch = 1;
        return;
    }

    if (mWaterRunTimer != 0) {
        if (!mMeshClsn.IsOnGround())
            goto clear_timer;
        if (!SurfaceInfo_TestFlag0x20((int *)&mMeshClsn.GetFloorResult()->surface.clps))
            goto clear_timer;

        mIsInShallowWater = 1;

        if (mWaterRunTimer != 0) {
            if (mHorzSpeed >= 0x1000)
                goto big_block;
        }

        mWaterRunTimer = 0;
        mPosY += mModelOffsetY;
        if (mPosY < mGroundY)
            mPosY = mGroundY;
        return;

    big_block:
        {
            int limit2 = Player_ScaleByCharFactor(c, 0x28000);

            if (mHorzSpeed < limit2) {
                if (mWaterRunTimer >= 0x1e)
                    mWaterRunTimer = 0x1e;
            }

            mFloorNormalZ = 0;
            {
                int t = mFloorNormalZ;
                mFloorNormalX = t;
                mFloorNormalY = 0x1000;
            }

            {
                int yy = mPosY;
                int zz = mPosZ;
                int xx = mPosX;
                int yy2 = yy + 0x64000;
                v[0] = xx;
                v[1] = yy2;
                v[2] = zz;
            }

            {
                dBgCh_Gnd rg;
                rg.SetObjAndPos(*(const Vector3 *)v, (dActor_c *)c);

                if (rg.DetectClsn() != 0) {
                    mGroundY = rg.clsnY;
                    if (mPosY + mModelOffsetY < rg.clsnY)
                        mWaterRunTimer = 0;
                }
            }
            return;
        }

    clear_timer:
        mWaterRunTimer = 0;
        return;
    }

    mDetectWater = 0;
    if (!mMeshClsn.IsOnGround())
        return;
    if (SurfaceInfo_TestFlag0x20((int *)&mMeshClsn.GetFloorResult()->surface.clps) != 0)
        return;
    if (mIsInShallowWater != 0)
        return;

    if (c->IsState(data_ov002_0211013c)) {
        mWaterRunLatch = 0;
        mDetectWater = 1;
    }
    return;
}
}

// @symbol _ZN6Player19func_ov002_020c2db8Ev
extern "C" {
void Player::func_ov002_020c2db8() {
    unsigned char *c = (unsigned char *)this;
    extern void func_02035798(void *, int);
    extern int func_ov002_020eea84(void *, void *);
    ((Player *)(c))->func_ov002_020c14b8();
    c[0x6e9] = 0;
    if (mSkipGroundUpdate != 0)
        return;
    c[0x70e] = 0;
    if (mFloorClass == 5)
        c[0x70e] = 1;
    if (((Player *)(c))->func_ov002_020c231c() == 0) {
        func_02035798(&mMeshClsn, 0x1000);
        ((Player *)(c))->func_ov002_020c25a8(0);
    } else {
        func_02035798(&mMeshClsn, 0);
        ((Player *)(c))->func_ov002_020c25a8(1);
    }
    ((Player *)(c))->func_ov002_020c1234();
    ((Player *)(c))->func_ov002_020c0fb4();
    ((Player *)(c))->func_ov002_020c5d60();
    ((Player *)(c))->func_ov002_020c61ac();
    func_ov002_020eea84(&mMeshClsn, c);
    ((Player *)(c))->func_ov002_020c2b08();
    ((Player *)(c))->func_ov002_020c29d4();
}
}

// @symbol _ZN6Player19func_ov002_020c2e78Ev
extern "C" {
void Player::func_ov002_020c2e78(){
    extern unsigned char IsPlayerWarping(int a0);
    extern void func_02020388(int handle);
    extern Player::State data_ov002_02110154;
  unsigned char w = IsPlayerWarping(mPlayerNo);
  if(w==0) return;
  if(!this->IsState(data_ov002_02110154)){
    func_02020388(mPlayerNo);
    return;
  }
  mTeleportId = w;
  unsigned short *p = &this->mStateFlags;
  *p |= 8;
}
}

// @symbol _ZN6Player7CanWarpEv
/* recovered: named members + shared header, real C++ method
 *
 * Warping is allowed from exactly two states. One of them is now spelt as the
 * static member it is -- Player::ST_WAIT, which the compiler mangles for us --
 * instead of a hand-written extern. The other is
 * still reached by its ov002 address; it has no recovered name yet.
 *
 * The file's own `struct State;` stand-in is gone with it: IsState really
 * takes a Player::State&, which is what its mangling says, and the local
 * forward declaration only ever existed to give the extern something to point
 * at.
 */
extern "C" {
int Player::CanWarp()
{

    extern Player::State data_ov002_021102a4;
    if (this->IsState(ST_WAIT) || this->IsState(data_ov002_021102a4))
        return 1;
    return 0;
}
}

// @symbol _ZN6Player7IsInAirEv
/* recovered: named members + shared header, real C++ method */
int Player::IsInAir()
{
  if(mIsAirborne) return mIsInAirState!=0;
  return 0;
}

// @symbol _ZN6Player19func_ov002_020c2f64Ev
void Player::func_ov002_020c2f64() {
  if (this->mLandSoundPlayed) return;
  this->mLandSoundPlayed = 1;
  if (this->mIsMetal) {
      Sound::PlayBank0(0xa1, mCamSpacePos);
      return;
  }
  if (this->mIsMega) {
      Sound::PlayBank0(0xd2, mCamSpacePos);
      return;
  }
  Sound::PlayBank0(this->mGroundSoundType + 0x40, mCamSpacePos);
}

// @symbol _ZN6Player19func_ov002_020c2fecEPi
extern "C" {
int Player::func_ov002_020c2fec(int *out)
{
    extern struct Anim2fec data_ov002_0210a8b8[];
    extern void func_ov002_020c3160(Player *, Fix12i *);
    Fix12i tmp[4];
    int i;
    int ret;
    struct Anim2fec *row;

    ret = 0;
    out[0] = 0;
    out[1] = 0;
    out[2] = 0;
    func_ov002_020c3160(this, tmp);

    row = &data_ov002_0210a8b8[mSplinePoint];
    for (i = 0; i < 4; i++) {
        out[0] += tmp[i] * row[i].x;
        out[1] += tmp[i] * row[i].y;
        out[2] += tmp[i] * row[i].z;
    }

    mSplineT += (row[0].a << 12) / 1000;
    if (mSplineT >= 0x1000) {
        mSplinePoint++;
        mSplineT -= 0x1000;
        switch (mSplineSpan) {
        case 5:
            ret = 1;
            break;
        case 3:
            if (data_ov002_0210a8b8[mSplinePoint + 2].a == 0) {
                mSplineSpan = 4;
            }
            break;
        default:
            mSplineSpan++;
            break;
        }
    }
    return ret;
}
}

// @symbol _ZN6Player19func_ov002_020c3148Ev
void Player::func_ov002_020c3148()
{
    mSplinePoint = 0;
    mSplineT = 0;
    mSplineSpan = 1;
}

// @symbol func_ov002_020c3160
extern "C" {
static inline int FX(int a, int b) {
    return FX_MUL((a), (b));
}

/* The uniform cubic B-spline basis at mSplineT, four weights for the four
   control points func_ov002_020c2fec then blends; mSplineSpan picks the
   clamped-end variant. The caller passes a Fix12i[4]. */
void func_ov002_020c3160(Player *self, Fix12i *out) {
    typedef long long s64;
    int t = self->mSplineT;
    int u = 0x1000 - t;
    int u2 = FX(u, u);
    int u3 = FX(u2, u);
    int t2 = FX(t, t);
    int t3 = FX(t2, t);

    switch (self->mSplineSpan) {
    case 0: break;
    case 1: {
        int a = FX(t, 0x3000);
        int b = FX(t3, 0x1c00);
        int c = FX(t2, 0x4800);
        out[0] = u3;
        out[1] = a + (b - c);
        out[2] = FX(t2, 0x1800) - FX(t3, 0xeaa);
        out[3] = FX(t3, 0x2aa);
        break;
    }
    case 2: {
        int k1400 = 0x1400;
        s64 p955 = (s64)t3 * 0x955;
        s64 p1400 = (s64)t2 * k1400;
        out[0] = FX(u3, 0x400);
        {
            int b = FX(t, 0x400);
            int c = (int)((p955 + 0x800) >> 12);
            int d = (int)((p1400 + 0x800) >> 12);
            out[1] = b + (c - d) + 0x955;
        }
        out[2] = FX(t, 0x800) + (FX(t2, 0x800) - FX(t3, 0x800)) + 0x2aa;
        out[3] = FX(t3, 0x2aa);
        break;
    }
    case 3: {
        out[0] = FX(u3, 0x2aa);
        out[1] = FX(t3, 0x800) - t2 + 0xaaa;
        out[2] = FX(t, 0x800) + (FX(t2, 0x800) - FX(t3, 0x800)) + 0x2aa;
        out[3] = FX(t3, 0x2aa);
        break;
    }
    case 4: {
        out[0] = FX(u3, 0x2aa);
        out[1] = FX(u, 0x800) + (FX(u2, 0x800) - FX(u3, 0x800)) + 0x2aa;
        out[2] = FX(u, 0x400) + (FX(u3, 0x955) - FX(u2, 0x1400)) + 0x955;
        out[3] = FX(t3, 0x400);
        break;
    }
    case 5: {
        out[0] = FX(u3, 0x2aa);
        out[1] = FX(u2, 0x1800) - FX(u3, 0xeaa);
        out[2] = FX(u, 0x3000) + (FX(u3, 0x1c00) - FX(u2, 0x4800));
        out[3] = t3;
        break;
    }
    }
}
}

// @symbol _ZN6Player19func_ov002_020c37a4EPc
/* recovered: shared common types */
extern "C" {
void Player::func_ov002_020c37a4(char *arg) {
    extern void MulVec3Mat4x3(void *, void *, void *);
    extern void Vec3_MulScalar(void *, void *, int s);
    extern void Vec3_RotateYAndTranslate(void *, void *, int ang, void *);
    extern u32 PlayerNewHold4(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                              void *callback);
    unsigned int id = this->GetBodyModelID(param1 & 0xff, 0);
    Matrix4x3 *bone = &mModels.mBodyModels[id]->data.transforms[8];
    unsigned int id2 = this->GetBodyModelID(param1 & 0xff, 0);
    Vector3 v;
    Vector3 s;
    MulVec3Mat4x3(&bone->m[9], &mModels.mBodyModels[id2]->mat4x3, &v);
    Vec3_MulScalar(&s, &v, 0x8000);
    v = s;
    Vec3_RotateYAndTranslate(&v, &v, this->mAngleY, arg);
    mParticle1 = (s32)Particle::System::New(mParticle1, 0x34, v.x, v.y, v.z, 0, 0);
    mParticle2 = (s32)Particle::System::New(mParticle2, 0x35, v.x, v.y, v.z, 0, 0);
}
}

// @symbol _ZN6Player19func_ov002_020c38a0Ev
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
#define LA(p) ((int)(p))
extern "C" {
void Player::func_ov002_020c38a0()
{
    extern int Vec3_HorzLen(void*);
    extern int atan2_hold_Player_14(int a, int b);
    V3Quad s;
    int hlen;
    int ang;
    unsigned char *p;

    s.a.x = 0; s.a.y = 0; s.a.z = 0x60000;
    s.b.x = 0; s.b.y = 0; s.b.z = 0x60000;
    func_ov002_020c37a4((char *)&s.b);
    s.d.x = 0; s.d.y = 0; s.d.z = 0;
    switch (mStateWork) {
    case 0:
        this->SetAnim(0xc1, 0, 0x1000, 0);
        mRootMotionMode = 0;
        func_ov002_020c3148();
        mModelOffsetY = -0x20000;
        p = &this->mStateWork;
        *p = *p + 1;
        /* fallthrough */
    case 1:
        if (func_ov002_020c2fec((int *)&s.d) != 0) {
            p = &this->mStateWork;
            *p = *p + 1;
            return;
        }
        s.e.x = s.d.x - this->mPosX;
        s.e.y = s.d.y - this->mPosY;
        s.e.z = s.d.z - this->mPosZ;
        hlen = Vec3_HorzLen(&s.e);
        ang = cstd::atan2(s.e.x, s.e.z);
        this->mPosX = s.d.x;
        this->mPosY = s.d.y;
        this->mPosZ = s.d.z;
        if ((s.e.x | s.e.z) == 0) {
            this->mAngleX = 0;
            this->mAngleY = 0x7fff;
            return;
        }
        this->mAngleX = -cstd::atan2(s.e.y, hlen);
        ApproachLinear2(this->mAngleZ, 0x2000, 0x10);
        this->mAngleY = ang;
        break;
    case 2:
        break;
    }
}
}
#undef LA

// @symbol _ZN6Player19func_ov002_020c3a48Ev
extern "C" {
int Player::func_ov002_020c3a48()
{
    extern void Player_AdvanceAnims(void*);
    unsigned int r5;
    char *anim;
    int id;

    if (mStateWork == 0) {
        mAttachOffsetY = 0;
        id = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(0x5a)) {
            Particle::RunningSlidingDustAt(this->mPosX, this->mPosY, this->mPosZ);
            Sound::PlayCharVoice(mCharacter, 0, mCamSpacePos);
            mStateWork += 1;
        }
    } else {
        int buf[6];
        buf[0] = 0;
        buf[1] = 0;
        buf[2] = 0;
        buf[3] = 0;
        buf[4] = 0;
        buf[5] = 0;
        func_ov002_020c37a4((char *)(buf + 3));
        id = this->GetBodyModelID(param1 & 0xff, 0);
        /* Launder kept: reading mBodyModels[id]->currFrame directly moves bytes. */
        anim = (char *)((long long)(int)&(dExtFrameCtrl_c &)*mModels.mBodyModels[id]);
        r5 = (unsigned int)(((dExtFrameCtrl_c *)anim)->currFrame << 4) >> 0x10;
        if (r5 == 0x6d || r5 == 0x8d) {
            Particle::RunningSlidingDustAt(this->mPosX, this->mPosY, this->mPosZ);
        }
        switch (r5) {
        case 0x77:
            Sound::PlayCharVoice(mCharacter, 2, mCamSpacePos);
            break;
        case 0x97:
            Sound::PlayCharVoice(mCharacter, 4, mCamSpacePos);
            break;
        }
        mAttachOffsetY -= 0x21000;
        if (this->FinishedAnim()) {
            func_ov002_020c3cf0();
            return 1;
        }
    }

    this->mPosX = 0;
    this->mPosZ = mAttachOffsetY;
    Player_AdvanceAnims(this);
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c3bdcEv
extern "C" {
void Player::func_ov002_020c3bdc(){
    typedef int Fix12i;
    extern void StartEndingCutscene(void);
    extern void Player_AdvanceAnims(void*);
  if (mStateWork == 0){
    mHasWings = 1;
    mPowerupTimer = 0x708;
    this->mPrevAngleY = -0x8000;
    this->mAngleY = this->mPrevAngleY;
    this->mPosZ = 0;
    this->mPosX = this->mPosZ;
    this->mHorzSpeed = 0;
    this->mVertAccel = -0x2000;
    if (mIsAirborne == 0){
        this->SetAnim(0x55, 0x40000000, 0x1000, 0);
        this->mStateWork += 1;
    }
  } else {
    if (this->FinishedAnim()){
        StartEndingCutscene();
        func_ov002_020c3cf0();
        this->SetAnim(0xc0, 0x40000000, 0x1000, 0);
        mRootMotionMode = 3;
        mSkipGroundUpdate = 1;
        mIsBodyClsnEnabled = 0;
        this->mVertSpeed = 0;
        this->mVertAccel = 0;
        this->mPosY = 0x15e000;
    }
  }
  Player_AdvanceAnims(this);
}
}

// @symbol _ZN6Player19func_ov002_020c3cf0Ev
void Player::func_ov002_020c3cf0()
{
    mStateStep = 0;
    mStateWork = mStateStep;
    (this->mStatePhase)++;
}

// @symbol _ZN6Player17St_EndingFly_MainEv
/* recovered: shared header, real C++ method
 *
 * ov002, 0x020c3d1c, size 0x50. The per-tick handler for the ending flight,
 * dispatching on a phase counter at 0x6e6 to one of three ov002 workers.
 *
 * THE FALLTHROUGH IS DELIBERATE. Phase 1 runs its worker and, when that
 * returns non-zero, falls into phase 2's -- so the frame that finishes phase 1
 * also starts phase 2 rather than idling. Written as a `case 1:` with no
 * `break` on the non-zero path, which is what the ROM's branch structure does.
 *
 * The three workers stay placeholders: they are ov002 functions with no
 * recovered name, and naming them from this call site alone would be a guess.
 *
 * This file carried the placeholder name func_ov002_020c3d1c until the symbol
 * was moved here from ov007, where it had been sitting on an unrelated 0x50
 * function that ov007 calls from 46 places. See the commit and Player.h.
 */
int Player::St_EndingFly_Main()
{
    switch (mStatePhase) {
    case 0:
        func_ov002_020c3bdc();
        break;
    case 1:
        if (func_ov002_020c3a48() == 0) break;
    case 2:
        func_ov002_020c38a0();
        break;
    }
    return 1;
}

// @symbol _ZN6Player17St_EndingFly_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_EndingFly_Init()
{
    extern int Player_DisableInteraction(void*);
  Player_DisableInteraction(((char*)this));
  mIsControlDisabled=1;
  mStatePhase=0;
  mStateWork=mStatePhase;
  this->SetAnim(0x54, 0x40000000, 0x1000, 0);
  return 1;
}
}

// @symbol func_ov002_020c3dbc
extern "C" {
void func_ov002_020c3dbc(Player *player) {
    extern Player::State data_ov002_0211058c;
    player->ChangeState(data_ov002_0211058c);
}
}

// @symbol _ZN6Player21St_OpeningWakeUp_MainEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_OpeningWakeUp_Main()
{
    extern void Player_AdvanceAnims(void*);
    switch (mStateStep) {
    case 0:
        if (this->FinishedAnim()) {
            this->SetAnim(0xb4, 0, 0x1000, 0);
            *(u8*)(int)((char*)&mStateStep) += 1;
        }
        break;
    case 1:
        break;
    case 2:
        if (this->FinishedAnim()) {
            this->SetAnim(0xb5, 0, 0x1000, 0);
            *(u8*)(int)((char*)&mStateStep) += 1;
        }
        break;
    case 3:
        break;
    }
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol func_ov002_020c3e8c
extern "C" {
void func_ov002_020c3e8c(Player *player) {
    extern Player::State data_ov002_0211013c;
    player->ChangeState(data_ov002_0211013c);
}
}

// @symbol _ZN6Player19func_ov002_020c3ea0Ev
extern "C" {
int Player::func_ov002_020c3ea0(){
    this->SetAnim(0xb3, 0x40000000, 0x1000, 0);
    mStateStep = 2;
    return 2;
}
}

// @symbol _ZN6Player21St_OpeningWakeUp_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_OpeningWakeUp_Init()
{
  mSleepStage=0;
  this->SetAnim(0xb2, 0x40000000, 0x1000, 0);
  mStateStep=0;
  return 1;
}
}

// @symbol func_ov002_020c3f18
extern "C" {
void func_ov002_020c3f18(char* p) {
    extern void func_ov002_020d2e74(void*);
    p[0x6e6] = 1;
    func_ov002_020d2e74(p);
}
}

// @symbol func_ov002_020c3f2c
extern "C" {
void func_ov002_020c3f2c(Player *player) {
    extern Player::State data_ov002_02110544;
    player->ChangeState(data_ov002_02110544);
}
}

// @symbol _ZN6Player15St_Respawn_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Respawn_Main()
{
    extern void Player_AdvanceAnims(void*);
    extern u8 data_0209f250;
    extern Player::State data_ov002_02110154;
    extern void FUN_02029980(void);
    extern void FUN_02029934(void);
    switch (mStateStep) {
    case 0: {
        u16 t = mStateTimer;
        if (t != 0) {
            if (t == 1) {
                mVertAccel = -0x4000;
                if (mPlayerNo == data_0209f250) {
                    FUN_02029980();
                    mStateWaitTimer = 0x26;
                }
            }
        } else {
            if (mIsAirborne == 0) {
                u32 zero = 0;
                this->SetAnim(0x55, 0x40000000, 0x1000, zero);
                mStateStep += 1;
            }
        }
        break;
    }
    case 1:
        if (this->FinishedAnim() != 0) {
            int z = 0;
            this->SetAnim(0x47, z, 0x1000, z);
            mStateStep += 1;
        }
        break;
    case 2:
        if (mStateWaitTimer == 0) {
            if (mPlayerNo == data_0209f250) {
                FUN_02029934();
            }
            this->ChangeState(data_ov002_02110154);
        }
        break;
    }
    Player_AdvanceAnims(((char *)this));
    return 1;
}
}

// @symbol _ZN6Player15St_Respawn_InitEv
/* recovered: named members + shared header, real C++ method
 *
 * Restores the spawn point saved by Player::InitResources, lifts the player
 * 0x3e8000 above it, re-seats the mesh collider at the new position, then
 * resets speed/angle state and starts the respawn animation.
 */
extern "C" {
int Player::St_Respawn_Init()
{
    extern void Player_DisableInteraction(void*);
    extern void func_02035860(void*, void*);
    extern void func_0200cf40(void*);
    extern unsigned char data_0209f250;
    extern char* data_0209f318;
    Player_DisableInteraction(this);
    mPosX = mSpawnPosX;
    mPosY = mSpawnPosY;
    mPosZ = mSpawnPosZ;
    mPosY += 0x3e8000;
    func_02035860(&mMeshClsn, &mPosX);
    mPrevPosX = mPosX;
    mPrevPosY = mPosY;
    mPrevPosZ = mPosZ;
    mPrevAngleY = mSpawnAngleY;
    mAngleY = mPrevAngleY;
    mIsBodyClsnEnabled = 1;
    mSkipGroundUpdate = 0;
    if (mPlayerNo == data_0209f250) {
        func_0200cf40(*(char**)((int)&data_0209f318));
    }
    this->SetAnim(0x54, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    mPeakY = mPosY;
    mHorzSpeed = 0;
    mVertSpeed = mHorzSpeed;
    mVertAccel = 0;
    mStateTimer = 0xa;
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c4188Ev
extern "C" {
int Player::func_ov002_020c4188()
{
    extern int* data_0209f318;
    extern int data_0209b454;
    extern short data_ov002_020ff26c[];
    extern unsigned char data_0209d660;
    extern void func_0200d3f8(void*, unsigned char playerID, void*);
    extern void func_0201f32c(int arg0);
    extern void func_0200d81c(void*, int playerID);
    extern void func_ov002_020e444c(void*);
    int* p;

    if (mEventId == 0) {
        return 0;
    }

    p = data_0209f318;

    switch (mEventStep) {
    case 0:
        if (mEventId == 6) {
            mEventStep++;
            goto case1;
        }
        if (mEventId < 7) {
            func_0200d3f8(p, mPlayerNo, 0);
        } else {
            ((dCamera_c *)p)->SetFlag_3();
        }
        mEventStep++;
        if (mEventId >= 7) {
            break;
        }
        return 0;
    case 1:
    case1:
        mFlags |= 0x800000;
        data_0209b454 |= 0x800000;
        mEventWait = 0xa;
        mEventStep++;
        break;
    case 2:
        if (mEventWait != 0) {
            mEventWait--;
            break;
        }
        func_0201f32c(data_ov002_020ff26c[mEventId - 1]);
        if (mEventId == 6) {
            mAttachedActor = (s32)SpawnSoundObj(5);
        }
        mEventStep++;
        break;
    case 3:
        if (data_0209d660 != 0) {
            break;
        }
        mEventStep++;
        mFlags &= ~0x800000;
        data_0209b454 &= ~0x800000;
        if (mEventId >= 7) {
            ((dCamera_c *)p)->mFlags &= ~8;
        }
        func_0200d81c(p, mPlayerNo);
        if (mEventId == 6) {
            daSoundObj_c *obj = (daSoundObj_c *)mAttachedActor;
            if (obj->mSoundID == 0x2a) {
                obj->mCounterLimit = 0;
            }
        }
        break;
    case 4:
        if (((dCamera_c *)p)->mFlags & 0x8000) {
            break;
        }
        mEventId = 0;
        mEventStep = 0;
        mFlags &= ~0x800000;
        data_0209b454 &= ~0x800000;
        break;
    }

    func_ov002_020e444c(this);
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c43c4Ej
extern "C" {
void Player::func_ov002_020c43c4(unsigned int idx) {
    extern unsigned char data_0209f2d8;
    extern int data_0209caa0;
    int b = (int)(data_0209f2d8 == 1);
    if (b != 0) return;
    switch (idx) {
    case 1:
        if ((&data_0209caa0)[2] & 0x400) return;
        (&data_0209caa0)[2] |= 0x400;
        break;
    case 2:
        if ((&data_0209caa0)[2] & 0x800) return;
        (&data_0209caa0)[2] |= 0x800;
        break;
    case 3:
        if ((&data_0209caa0)[2] & 0x1000) return;
        (&data_0209caa0)[2] |= 0x1000;
        break;
    case 4:
        if ((&data_0209caa0)[2] & 0x2000) return;
        (&data_0209caa0)[2] |= 0x2000;
        break;
    case 5:
        if ((&data_0209caa0)[2] & 0x4000) return;
        (&data_0209caa0)[2] |= 0x4000;
        break;
    case 6:
        if ((&data_0209caa0)[2] & 0x10000000) return;
        (&data_0209caa0)[2] |= 0x10000000;
        break;
    case 0:
        break;
    }
    mEventId = (unsigned char)idx;
    mEventStep = 0;
}
}

// @symbol _ZN6Player19func_ov002_020c44c4Ev
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
int Player::func_ov002_020c44c4()
{
    extern u8 data_0209f2d8;
    extern int data_0209caa0[];
    extern s8 data_0209f2f8;
    extern u8 data_0209f264;
    /* local extern: the registrar takes this D1 by address, which C++ cannot spell. */
    extern void *_ZN7Vector3D1Ev(void *object);
    extern void func_020731dc(void *object, void *destructor, void *node);
    extern void Vec3_RotateYAndTranslate(void*, void*, short angle, void*);
    extern void func_02012694(unsigned int id, const Vector3 *v);
    extern int SublevelToLevel(int);
    extern u32 data_ov002_0210e14c;
    extern Vector3 data_ov002_0210f3b0;
    extern void*data_ov002_0210f350;
    extern void func_ov100_02145070(int);
    extern int func_ov100_02145014(void);
    Vector3 pos;
    int state;
    int r4val;
    u8 f2d8;
    int b0, b1;

    f2d8 = data_0209f2d8;
    r4val = -1;
    b0 = (int)(f2d8 == 1);
    if (b0 != 0) {
        goto cont;
    }
    if ((data_0209caa0[2] & 0x80) == 0) {
        goto ret0;
    }
    b1 = (int)(f2d8 == 2);
    if (b1 == 0) {
        goto cont;
    }
ret0:
    return 0;

cont:
    if (SaveData::CountStarsCollectedInLevel(SublevelToLevel(data_0209f2f8)) != 0) {
        return 0;
    }

    state = data_0209f2f8;

    switch (state) {
    case 2:
        if ((data_0209caa0[2] & 0x200) == 0) {
            data_0209caa0[2] |= 0x200;
            if ((data_ov002_0210e14c & 1) == 0) {
                data_ov002_0210f3b0.x = 0;
                data_ov002_0210f3b0.y = 0x96000;
                data_ov002_0210f3b0.z = 0x64000;
                func_020731dc(&data_ov002_0210f3b0, (void *)_ZN7Vector3D1Ev, &data_ov002_0210f350);
                data_ov002_0210e14c |= 1;
            }
            Vec3_RotateYAndTranslate((int*)&pos, &this->mPosX, this->mAngleY, (int*)&data_ov002_0210f3b0);
            this->ShowMessage(*this, 0x177, &pos, 2, 2);
            func_ov100_02145070(0x1e);
            func_ov100_02145014();
            func_02012694(0xb8, &mCamSpacePos);
            mPlayMusicAfterTalk = 1;
            return 1;
        }
        break;
    case 6:
        r4val = 7;
        break;
    case 7:
        r4val = 8;
        break;
    case 10:
        if (data_0209f264 == 0) {
            r4val = 9;
        }
        break;
    case 14:
        if (data_0209f264 == 0) {
            r4val = 10;
        }
        break;
    case 35:
        if (data_0209f264 == 0) {
            r4val = 11;
            func_02012790(0x18);
        }
        break;
    case 31:
        if (data_0209caa0[1] & 0x80000000) {
            return 0;
        }
        r4val = 12;
        break;
    }

    if (r4val == -1) {
        return 0;
    }

    this->SetAnim(0x47, 0, 0x1000, 0);
    func_ov002_020c43c4(r4val & 0xff);
    return 0;
}
}

// @symbol _ZN6Player15St_Talk_CleanupEv
/* recovered: named members + shared header, real C++ method
 *
 * Leaving the talk state: clear the "in conversation" bit (0x800000) on the
 * actor being talked to, on the player, and on the global at data_0209b454;
 * clear bit 3 on data_0209f318+0x154; and restore music layer 1 if this state
 * had overridden it.
 *
 * mTalkActor is the actor being talked to; +0xb0 through it is dActor_c's
 * mFlags. The field is typed fBase_c * (StartTalk's argument type), so the
 * bit is cleared through a cast, the same way Player::St_Talk_Main reaches it.
 */
extern "C" {
int Player::St_Talk_Cleanup()
{
    extern u32 data_0209b454;
    extern char* data_0209f318;
    dActor_c *p = (dActor_c *)mTalkActor;
    if (p) {
        p->mFlags &= ~0x800000;
        mTalkActor = 0;
    }
    mFlags &= ~0x800000;
    data_0209b454 &= ~0x800000;
    ((dCamera_c *)data_0209f318)->mFlags &= ~8;
    if (mPlayMusicAfterTalk) {
        Sound::LoadAndSetMusic_Layer1(0x39);
        mPlayMusicAfterTalk = 0;
    }
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c47f4Ev
/* recovered: shared common types */
extern "C" {
int Player::func_ov002_020c47f4()
{
    typedef int Fix12i;
    extern short Vec3_VertAngle(const Vector3* v1, const Vector3* v0);
    extern short Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
    extern int ApproachAngle(void*, short targetAngle, short invFactor, short maxDelta, short minDelta);
    Vector3 vec;
    int r, a;
    Fix12i y;
    vec.x = this->mPosX;
    y = this->mPosY;
    vec.y = y;
    vec.z = this->mPosZ;
    vec.y = y + 0x64000;
    mHeadRot[0] = 0;
    mHeadRot[1] = 0;
    r = ApproachLinear2(mAngleYSpeed, (short)(-Vec3_VertAngle((Vector3 *)&mStateTargetX, &vec)),
                        0x200);
    mHeadRot[0] = mAngleYSpeed;
    a = ApproachAngle(&this->mAngleY,
                      (short)(Vec3_HorzAngle((Vector3 *)&mStateTargetX, &vec) + 0x8000), 4, 0x1000,
                      0x200);
    mHeadRotMode = 3;
    if (r == 0) goto ret0;
    if (a == 0) return 1;
ret0:
    return 0;
}
}

// @symbol _ZN6Player12St_Talk_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Talk_Main()
{
    extern int PlayerChangeMusicVolumeHold(u32 vol, int t);
    extern void func_0201f32c(s16 a);
    extern void func_0200d3f8(void*, u8 playerID, void*);
    extern void func_0200d81c(void*, u8 playerID);
    extern void Player_AdvanceAnims(void*);
    extern int data_0209b454;
    extern char* data_0209f318;
    extern u8 data_0209d660;
    extern Player::State data_ov002_0211022c;
    extern Player::State data_ov002_02110514;
    extern Player::State data_ov002_0211013c;
    extern void func_0201fc88(s16);
    extern void func_0200d7a4(void*, u8);
    extern int func_ov100_02144fcc(void);
    if (mRestoreMusicVolume != 0) {
        if (Sound::ChangeMusicVolume(0x7f, 0x7222) != 0)
            mRestoreMusicVolume = 0;
    }

    if (mNoCtrlKind == 0) {
        dActor_c *p = (dActor_c *)mTalkActor;
        if (p != 0)
            p->mFlags |= 0x800000;
        mFlags |= 0x800000;
        data_0209b454 |= 0x800000;
        mNoCtrlKind = 1;
    }

    switch (mStateStep) {
    case 0:
        if (mStateWork != 0)
            func_ov002_020c47f4();
        break;
    case 1:
        if (!this->IsAnim(0x64)) {
            if (!func_ov002_020c47f4())
                break;
        }
        mStateStep = 2;
        if (mStateWork == 0)
            func_0201fc88(mAttachOffsetY);
        else
            func_0201f32c(mAttachOffsetY);
        {
            char* cam = data_0209f318;
            switch ((mStateArg >> 2) & 3) {
            case 0:
                func_0200d3f8(cam, mPlayerNo, &mStateTargetX);
                break;
            case 1:
                func_0200d7a4(cam, mPlayerNo);
                break;
            }
        }
        break;
    case 2:
        if (data_0209d660 != 0)
            break;
        switch (mStateArg & 3) {
        case 0:
            mStateStep = 4;
            break;
        case 1:
            mStateStep = 3;
            break;
        case 2:
            mStateStep = 8;
            break;
        }
        break;
    case 8:
        if (func_ov100_02144fcc() != 0)
            mStateStep = 4;
        break;
    case 4:
    case 5:
        if (ApproachLinear2(mHeadRot[0], 0, 0x200) != 0) {
            if (mStateStep != 5)
                mStateStep = 6;
            else
                mStateStep = 7;
            mStateWork = 0;
            mPrevAngleY = mAngleY;
            char* cam = data_0209f318;
            func_0200d81c(cam, mPlayerNo);
            ((dCamera_c *)cam)->mFlags &= ~8;
            mHeadRotMode = 4;
        }
        break;
    case 7:
        if (mStateWaitTimer != 0)
            break;
    case 6:
        if ((((dCamera_c *)data_0209f318)->mFlags & 0x8000) != 0)
            break;
        if (mIsOpeningBigDoor != 0) {
            mNoCtrlKind = 0x13;
            this->ChangeState(data_ov002_0211022c);
            mStateStep = 0x13;
        } else if (this->IsAnim(0x64)) {
            this->ChangeState(data_ov002_02110514);
        } else {
            this->ChangeState(data_ov002_0211013c);
        }
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player12St_Talk_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Talk_Init()
{
    extern int Player_DisableInteraction(void*);
  mIsControlDisabled=1;
  Player_DisableInteraction(((char*)this));
  this->SetAnim(0x47, 0, 0x1000, 0);
  mAngleYSpeed=0;
  mHorzSpeed=0;
  mNoCtrlKind=0;
  mVertSpeed=0;
  mVertAccel=mVertSpeed;
  mSkipGroundUpdate = 1;
  mIsBodyClsnEnabled=0;
  mPosX=mPreClsnPosX;
  mPosY=mPreClsnPosY;
  mPosZ=mPreClsnPosZ;
  mStateWork=0;
  mIsOpeningBigDoor=0;
  return 1;
}
}

// @symbol _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh
/* recovered: named members + shared header, real C++ method
 *
 * Starts a message. Already talking is success only at sub-step 0 or 3;
 * otherwise a short list of states may be interrupted, plus the no-control
 * state for all but a handful of mNoCtrlKind values. The last two parameters
 * are unsigned char -- the ROM reads all three of their uses with ldrb, and
 * the symbol was corrected from jj to hh two commits ago.
 */
extern "C" {
int Player::ShowMessage2(fBase_c & actor_, unsigned int msg, const Vector3 * pos, unsigned char d, unsigned char e)
{
    extern void func_020731dc(void*, void*, void*);
    extern void Vec3_RotateYAndTranslate(void*, const Vector3 *in, int angle, const Vector3 *src);
    extern Player::State data_ov002_0211046c;
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_02110364;
    extern Player::State data_ov002_0211043c;
    extern Player::State data_ov002_02110154;
    extern Player::State data_ov002_02110424;
    extern Player::State data_ov002_021104e4;
    extern Player::State data_ov002_02110514;
    extern Player::State data_ov002_0211022c;
    extern int data_ov002_0210e15c;
    extern int data_ov002_0210f224[3];
    extern int data_ov002_0210f1ac;
    /* local extern: the registrar takes this D1 by address, which C++ cannot spell. */
    extern void *_ZN7Vector3D1Ev(void *object);
    extern void *data_0209f318;
    fBase_c *actor = &actor_;
    int r7;

    if (IsState(data_ov002_0211046c)) {
        u8 sub = mStateStep;
        if (sub == 0 || sub == 3)
            goto finish;
        return 0;
    }

    r7 = 0;
    if (IsState(data_ov002_0211013c) || IsState(data_ov002_02110364) || IsState(data_ov002_0211043c) || IsState(data_ov002_02110154) || IsState(data_ov002_02110424) || IsState(data_ov002_021104e4) || IsState(data_ov002_02110514)) {
        r7 = 1;
    }

    if (IsState(data_ov002_0211022c)) {
        u8 v = mNoCtrlKind;
        if (!(v == 0 || v == 1 || v == 3 || v == 2 || v == 0xd)) {
            if (v != 8)
                r7 = 1;
        }
    }

    if (!r7)
        return 0;

    this->ChangeState(*reinterpret_cast<Player::State *>(&data_ov002_0211046c));
    mTalkActor = actor;

finish:
    mStateWork = 0;
    mStateArg = d & 3;
    *(u8 *)(((long long)(&mStateArg))) |= (e & 3) << 2;

    if (!(data_ov002_0210e15c & 1)) {
        data_ov002_0210f224[0] = 0;
        data_ov002_0210f224[1] = 0x64000;
        data_ov002_0210f224[2] = 0x64000;
        func_020731dc(data_ov002_0210f224, (void *)_ZN7Vector3D1Ev, &data_ov002_0210f1ac);
        data_ov002_0210e15c |= 1;
    }

    Vec3_RotateYAndTranslate((Vector3 *)(&mStateTargetX), &mPos, mAngleY,
                             (Vector3 *)data_ov002_0210f224);

    if (pos) {
        *(Vector3 *)(&mStateTargetX) = *pos;
    }

    mHorzSpeed = 0;
    mStateStep = 1;
    *(unsigned int*)&mAttachOffsetY = msg;

    if (e == 2) {
        ((dCamera_c *)data_0209f318)->SetFlag_3();
    }

    return 1;
}
}

// @symbol _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh
/* recovered: named members + shared header, real C++ method
 *
 * Thin wrapper: forwards to ShowMessage2 and, if that took, clears mStateWork
 * to 1.
 *
 * The last two parameters are unsigned char. That is what the ROM does -- both
 * are read with ldrb -- and now what the symbol says, since the preceding
 * commit corrected it from jj to hh. While it said jj this function could not
 * be a real method at all: the compiler mangles FROM the declared types.
 */
extern "C" {
int Player::ShowMessage(fBase_c & a_, unsigned int b, const Vector3 * v, unsigned char d_, unsigned char e_)
{
    if (this->ShowMessage2(a_, b, v, d_, e_)) {
        mStateWork = 1;
        return 1;
    }
    return 0;
}
}

// @symbol _ZN6Player18HasFinishedTalkingEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::HasFinishedTalking()
{

    extern Player::State data_ov002_0211046c;
  if(this->IsState(data_ov002_0211046c) && mStateStep==3){
    mStateStep=4;
    return 1;
  }
  return 0;
}
}

// @symbol _ZN6Player12Unk_020c4f40Et
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::Unk_020c4f40(unsigned short x)
{

    extern Player::State data_ov002_0211046c;
  if(this->IsState(data_ov002_0211046c) && mStateStep==3){
    mStateWaitTimer=x;
    mStateStep=5;
    return 1;
  }
  return 0;
}
}

// @symbol _ZN6Player9StartTalkER7fBase_cb
/* recovered: named members + shared header, real C++ method
 *
 * Entering conversation. Already talking to this actor with mStateStep 0 is
 * success; talking to anything else is refusal. Otherwise only a short list of
 * states may start a conversation, and when b_ is false the player must also
 * be pressing a button and be walking slowly, crouching or waiting.
 *
 * The third parameter is bool -- the ROM name ends in `b`, and a real method
 * is mangled FROM the declared types.
 */
extern "C" {
int Player::StartTalk(fBase_c & actor_, bool b_)
{
    extern Player::State data_ov002_0211046c;
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_02110154;
    extern Player::State data_ov002_021104e4;
    extern Player::State data_ov002_02110514;
    extern Player::State data_ov002_02110364;
    extern Player::State data_ov002_02110424;
    extern Player::State data_ov002_021105a4;
    extern Player::State data_ov002_0211043c;
    extern u8 data_0209f49e[];
    fBase_c *actor = &actor_;
    if (IsState(data_ov002_0211046c)) {
        if (actor == mTalkActor && mStateStep == 0)
            return 1;
        return 0;
    }

    if (!(IsState(data_ov002_0211013c) || IsState(data_ov002_02110154) || IsState(data_ov002_021104e4) || IsState(data_ov002_02110514) || IsState(data_ov002_02110364) || IsState(data_ov002_02110424) || IsState(data_ov002_021105a4) || IsState(data_ov002_0211043c)))
        return 0;

    if (b_ == 0) {
        if (!(*(u16 *)(data_0209f49e + (u8)gActivePlayerSlot * 0x18) & 3))
            goto ret0;

        if (!((IsState(data_ov002_0211013c) && mHorzSpeed < 0x8000) || IsState(data_ov002_0211043c) || IsState(data_ov002_02110154)))
            goto ret0;

        mStateStep = 0;
        ChangeState(data_ov002_0211046c);
        mTalkActor = actor;
        return 1;
    } else {
        if (IsState(data_ov002_02110424) || IsState(data_ov002_021105a4)) {
            this->mStateFlags |= 0x40;
            return 0;
        }

        mStateStep = 0;
        ChangeState(data_ov002_0211046c);
        mTalkActor = actor;
        return 1;
    }
ret0:
    return 0;
}
}

// @symbol func_ov002_020c51d0
extern "C" {
/* The look-at target: daTrs_c passes its own mPos with x negated and y raised
   by 0xc8000, and daHanachan_c passes the segment it wants Mario to face. */
int func_ov002_020c51d0(Player *player, Vector3 *st) {
    extern Player::State data_ov002_0211046c;
    if (!player->IsState(data_ov002_0211046c))
        return 0;
    if (player->mStateStep != 0)
        return 0;
    player->mStateTargetX = st->x;
    player->mStateTargetY = st->y;
    player->mStateTargetZ = st->z;
    player->mStateWork = 1;
    return 1;
}
}

// @symbol func_ov002_020c5244
extern "C" {
int func_ov002_020c5244(void)
{
    return 0;
}
}

// @symbol _ZN6Player12GetTalkStateEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::GetTalkState()
{

    extern Player::State data_ov002_0211046c;
  if(!this->IsState(data_ov002_0211046c)) return -1;
  unsigned char v=mStateStep;
  if(v==0) return 0;
  if(v==3) return 2;
  if(v==5||v==7) return 3;
  return 1;
}
}

// @symbol _ZN6Player21St_StuckInGround_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_StuckInGround_Main()
{
    extern void Player_AdvanceAnims(void*);
    extern u16 data_0209f49e[];
    extern Player::State data_ov002_0211013c;
    extern u32 data_ov002_0210a578[];
    extern u32 data_ov002_0210a584[];
    extern u8 data_ov002_020ff0ec[];
    switch (mStateWork) {
    case 0:
        if (this->FinishedAnim()) {
            this->SetAnim(data_ov002_0210a578[mStateStep], 0x40000000, 0x1000, 0);
            mStateWork++;
            return 1;
        }
        break;
    case 1:
        if (this->FinishedAnim() == 0) {
            if ((*(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18) & 2) == 0)
                break;
        }
        this->SetAnim(data_ov002_0210a584[mStateStep], 0x40000000, 0x1000, 0);
        mStateWork++;
        break;
    case 2: {
        dExtAnmModel_c *anim = mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)];
        if (anim->WillHitFrame(data_ov002_020ff0ec[mStateStep])) {
            func_ov002_020c5444();
            Sound::PlayBank0(0xb4, mCamSpacePos);
        }
    }
        if (this->FinishedAnim()) {
            mAngleY = mPrevAngleY;
            this->ChangeState(data_ov002_0211013c);
        }
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c5444Ev
/* recovered: shared common types */
extern "C" {
void Player::func_ov002_020c5444() {
    extern void PlayerNewSimpleHold4(unsigned int, int, int, int);
    Vector3 v;
    ((int *)&v)[0] = this->mPosX;
    ((int *)&v)[1] = this->mPosY;
    ((int *)&v)[2] = this->mPosZ;
    ((int *)&v)[1] = this->mPosY + 0x14000;
    unsigned char b = mStateArg;
    if (b == 0) {
        Particle::System::NewSimple(0xdb, v.x, v.y, v.z);
        Particle::System::NewSimple(0xdc, v.x, v.y, v.z);
        return;
    }
    if (b == 1) {
        Particle::System::NewSimple(0xd8, v.x, v.y, v.z);
        Particle::System::NewSimple(0xd9, v.x, v.y, v.z);
        return;
    }
    Particle::System::NewSimple(0xc7, v.x, v.y, v.z);
}
}

// @symbol _ZN6Player21St_StuckInGround_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_StuckInGround_Init()
{
    extern int Player_DisableInteraction(void*);
    extern int Player_ReleaseHeldActor(void*);
    extern int data_ov002_0210a560[];
  Player_DisableInteraction(((void*)this));
  Player_ReleaseHeldActor(((void*)this));
  unsigned char idx = *(unsigned char*)((char*)&mStateStep);
  this->SetAnim(data_ov002_0210a560[idx], 0x40000000, 0x1000, 0);
  Sound::PlayBank0(*(int *)((char *)&mGroundSoundType) + 0x80, mCamSpacePos);
  func_ov002_020c5444();
  *(int*)((char*)&mVertSpeed)=0;
  *(int*)((char*)&mHorzSpeed)=0;
  *(unsigned char*)((char*)&mStateWork)=0;
  return 1;
}
}

// @symbol _ZN6Player24St_BowserEarthquake_MainEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_BowserEarthquake_Main()
{
    extern void Player_AdvanceAnims(void*);
    extern short data_02082214[];
    extern Player::State data_ov002_0211013c;
    if (mStateStep == 0) {
        ApproachLinear(*((int *)((char *)&mHorzSpeed)), 0, 0x1800);
        if (this->FinishedAnim()) {
            this->ChangeState(data_ov002_0211013c);
        }
    } else {
        int t = mStateTimer;
        int m = t % 16;
        int idx = (unsigned short)(short)(m << 12) >> 4;

        mHorzSpeed = 0;

        int radius = 0x4000 + ((0x6000 - ((t << 12) / 8)) << 3);

        mVertSpeed = 0;

        int sinVal = data_02082214[idx * 2];

        if (sinVal >= 0) {
            mPosY = mGroundY + FX_MUL(radius, sinVal);
        } else {
            mPosY = mGroundY - FX_MUL(radius, sinVal);
        }

        if (mStateTimer == 0) {
            this->ChangeState(data_ov002_0211013c);
        }
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player24St_BowserEarthquake_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_BowserEarthquake_Init()
{
  if(mStateStep==0){
      this->SetAnim(0x12, 0x40000000, 0x1000, 0);
  } else {
      this->SetAnim(0x47, 0, 0x1000, 0);
      mStateTimer = 0x30;
  }
  return 1;
}
}

// @symbol func_ov002_020c56f0
extern "C" {
int func_ov002_020c56f0(unsigned char* c, unsigned char arg)
{
    extern int func_ov002_020d82f0(void*);
    extern int func_ov002_020d91e0(void*, int a, int b);
    extern Player::State data_ov002_0211010c;
    extern Player::State data_ov002_021103c4;
    extern int func_ov002_020c56f0(void*, int);
    if (func_ov002_020d82f0(c) == 0) return 0;
    c[0x6e3] = arg;
    if (c[0x6e3] == 0 && func_ov002_020d91e0((char*)c, 0x300, 1) != 0) {
        ((Player *)c)->ChangeState(data_ov002_0211010c);
        return 1;
    }
    ((Player *)c)->ChangeState(data_ov002_021103c4);
    return 1;
}
}

// @symbol _ZN6Player15St_DeadPit_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_DeadPit_Main()
{
    extern void PlayerPlayLongHold_deadpit(u32 a, u32 b, u32 c, void *, u32 d);
    extern void Vec3_RotateYAndTranslate(void*, void*, s16 angle, void*);
    extern void Player_AdvanceAnims(void*);
    extern u8 data_0209f2d8;
    extern Player::State data_ov002_021104fc;
    extern void KillPlayer(void);
    switch (mStateStep) {
    case 0:
        break;
    case 1: {
        int b = (data_0209f2d8 == 1);
        if (b) {
            if (mStateWaitTimer == 0) {
                this->ChangeState(data_ov002_021104fc);
                return 1;
            }
        }
        return 1;
    }
    case 2:
    case 5:
        if (mIsAirborne == 0) {
            mHorzSpeed = 0;
            mVertSpeed = 0;
        }
        if (mStateWork == 0) {
            if (this->FinishedAnim() != 0) {
                mStateWork = 1;
                KillPlayer();
            }
        }
        break;
    case 3: {
        func_ov002_020c0108(0);
        u32 snd = Sound::PlayLong(mLoopingSoundHandle, 0, 0x10a, *(Vector3 *)&mCamSpacePosX, 0);
        int* p = (int*)((char*)&mSinkDepth);
        mLoopingSoundHandle = snd;
        *p += 0x5000;
        if (mStateWork == 0) {
            if (mSinkDepth >= 0xb4000) {
                mStateWork = 1;
                KillPlayer();
            } else {
                *p += 0x2000;
                if (mStateArg == 0) {
                    if (mSinkDepth >= 0x64) {
                        Sound::PlayCharVoice(mCharacter, 0x25, mCamSpacePos);
                        mStateArg = 1;
                    }
                }
            }
        }
        break;
    }
    case 6: {
        int src[3];
        src[0] = 0;
        src[1] = -0x1e000;
        src[2] = 0x64000;
        dActor_c *q = *(dActor_c **)((char *)&mAttachedActor);
        s16 angle = q->mAngleY;
        Vec3_RotateYAndTranslate((int *)((char *)&mPosX), (int *)&q->mPos, angle, src);
        if (mStateTimer < 30) {
            mOpacity = 0;
        }
    }
    /* fallthrough */
    case 4:
        if (mStateTimer == 1) {
            KillPlayer();
            mOpacity = 0;
        }
        break;
    case 7:
    case 8:
        func_ov002_020c9718();
        if (mStateWork == 0) {
            if (this->FinishedAnim() != 0) {
                mStateWork = 1;
                KillPlayer();
                mIsUnderwater = 1;
            }
        }
        break;
    case 9:
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player15St_DeadPit_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_DeadPit_Init()
{
    extern u8 data_0209f2d8;
    extern u8 data_0209f250;
    extern int Player_ReleaseHeldActor(void*);
    extern void func_ov002_020d5cec(void*);
    extern void Player_DisableInteraction(void*);
    extern void HitDeathPlane(int arg);
    extern void FUN_020299f4(void);
    Player_ReleaseHeldActor(((char*)this));
    if (*(char**)((char*)&mObjInMouth) != 0) {
        int b = (((dActor_c *)*(char **)((char *)&mObjInMouth))->actorID == 0xbf);
        if (b) {
            func_ov002_020d5cec(*(char**)((char*)&mObjInMouth));
            mStateFlags &= ~2;
            *(int*)(*(char**)((char*)&mObjInMouth) + 0xd0) = 0;
        }
        mUseAltBodyModel = 0;
        ((dActor_c *)mObjInMouth)->mFlags |= 0x80000;
        ((dActor_c *)mObjInMouth)->mFlags &= ~0x40000;
        ((dActor_c *)mObjInMouth)->mFlags &= ~0x20000;
        *(char**)((char*)&mObjInMouth) = 0;
    }
    Player_DisableInteraction(((char*)this));
    {
        int t = 0;
        mIsBodyClsnEnabled = t;
        mStateWork = t;
        switch (mStateStep) {
        case 0:
            break;
        case 1:
            if (data_0209f2d8 == 1) t = 1;
            if (t == 0) {
                int a = 2;
                if (mSurfaceType == 5) a = 0;
                HitDeathPlane(a);
            } else {
                if (mPlayerNo == data_0209f250) FUN_020299f4();
                mStateWaitTimer = 0x1e;
            }
            mSkipGroundUpdate = 1;
            break;
        case 2:
            this->SetAnim(7, 0x40000000, 0x1000, t);
            mVertSpeed = 0xa000;
            mHorzSpeed >>= 1;
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
            Sound::PlayCharVoice(mCharacter, 0xb, mCamSpacePos);
            break;
        case 3:
            this->SetAnim(0x82, 0x40000000, 0x1000, t);
            mVertSpeed = 0;
            mHorzSpeed = 0;
            mStateArg = 0;
            break;
        case 4:
            this->SetAnim(2, 0x40000000, 0x1000, t);
            mStateTimer = 0x64;
            mVertSpeed = 0;
            mHorzSpeed = 0;
            Sound::PlayCharVoice(mCharacter, 0xb, mCamSpacePos);
            break;
        case 5:
            this->SetAnim(6, 0x40000000, 0x1000, t);
            mVertSpeed = 0;
            mHorzSpeed = 0;
            Sound::PlayCharVoice(mCharacter, 0xb, mCamSpacePos);
            break;
        case 6:
            mStateTimer = 0x3c;
            mVertSpeed = t;
            mHorzSpeed = t;
            mVertAccel = t;
            break;
        case 7:
            this->SetAnim(0x84, 0x40000000, 0x1000, t);
            mVertAccel = 0;
            mTerminalVelocity = -0x1800;
            mVertSpeed = 0;
            mHorzSpeed = 0;
            Sound::PlayCharVoice(mCharacter, 0x34, mCamSpacePos);
            break;
        case 8:
            this->SetAnim(0x83, 0x40000000, 0x1000, t);
            mVertAccel = 0;
            mTerminalVelocity = -0x1800;
            break;
        case 9:
            Sound::PlayCharVoice(mCharacter, 0x36, mCamSpacePos);
            break;
        }
    }
    return 1;
}
}

// @symbol _ZN6Player14EnterWhirlpoolEv
/* recovered: named members + shared header, real C++ method */
/* Player::EnterWhirlpool() @ 0x20c5cc8 (ov002) -- tail-call veneer to Player::func_ov002_020c5dec(int) (0x20c5dec) with r1=9.
 */
void Player::EnterWhirlpool()
{
    ((Player *)(((void*)this)))->func_ov002_020c5dec(9);
}

// @symbol func_ov002_020c5cd8
extern "C" {
int func_ov002_020c5cd8(char *c, int arg) {
    Player *player = (Player *)c;
    if (((Player *)(c))->func_ov002_020c5dec(6)) {
        player->mAttachedActor = arg;
        return 1;
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c5d0cEv
int Player::func_ov002_020c5d0c()
{
    u16 *p;
    if (this->mPosY - mGroundY >= 0x100000)
        return 0;
    p = &this->mStateFlags;
    *p |= 0x400;
    return func_ov002_020c5dec(1);
}

// @symbol _ZN6Player19func_ov002_020c5d60Ev
extern "C" {
void Player::func_ov002_020c5d60() {
    void* c = (void*)this;

    extern Player::State data_ov002_02110124;
    extern Player::State data_ov002_02110064;
  if (this->IsState(data_ov002_02110124)) {
    if (this->mStateStep == 1) return;
  }
  if (this->mPosY >= (int)0xf8ad0000) return;
  if (this->IsState(data_ov002_02110064)) return;
  (this->mStateFlags) |= 0x400;
  this->func_ov002_020c5dec(1);
}
}

// @symbol _ZN6Player19func_ov002_020c5decEi
extern "C" {
int Player::func_ov002_020c5dec(int r1){

    extern Player::State data_ov002_02110124;
  if (r1 != 1) {
    if (this->mIsNoControl != 0) return 0;
  }
  this->mStateStep = (unsigned char)r1;
  this->ChangeState(data_ov002_02110124);
  return 1;
}
}

// @symbol _ZN6Player15St_DeadHit_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_DeadHit_Main()
{
    extern void Player_AdvanceAnims(void*);
    extern u8 data_ov002_0211117c;
    extern u8 data_ov002_02109db8[];
    extern u32 data_ov002_0210a07c[];
    extern void KillPlayer(void);
    u32 t;

    if (mIsAirborne == 0) {
        if (mStateArg == 0) {
            if (mPrevVertSpeed < 0) {
                mVertSpeed = -mPrevVertSpeed / 3;
                if (mVertSpeed <= 0x1000)
                    mVertSpeed = 0;
            }
            t = mStateStep & 0xf0;
            if (t == 0 || t == 0x10)
                func_ov002_020d94cc();
            Sound::PlayBank0(mGroundSoundType + 0x50, mCamSpacePos);
            Sound::PlayCharVoice(mCharacter, 0x11, mCamSpacePos);
            mStateArg = 1;
        }
        ApproachLinear(*((int *)((char *)&mHorzSpeed)), 0, 0x1800);
    }

    switch (mStateWork) {
    case 0:
        if (mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(
                data_ov002_02109db8[mStateStep & 1]) != 0)
            mStateWork = 1;
        break;
    case 1:
        if (mIsAirborne != 0) break;
        if (mStateArg == 0) break;
        if (mHorzSpeed != 0) break;
        this->SetAnim(data_ov002_0210a07c[mStateStep & 1], 0x40000000, 0x1000, 0);
        Sound::PlayCharVoice(mCharacter, 0xb, mCamSpacePos);
        mStateWork = 2;
        break;
    case 2:
        if (this->FinishedAnim() != 0) {
            if (data_ov002_0211117c == 0) {
                KillPlayer();
                mStateWork = 3;
            }
        }
        break;
    }

    if (mStateWork != 1)
        Player_AdvanceAnims(((char *)this));
    mPrevVertSpeed = mVertSpeed;
    return 1;
}
}

// @symbol _ZN6Player15St_DeadHit_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_DeadHit_Init()
{
    extern void Player_DisableInteraction(void*);
    extern char* data_0209f318;
    extern int data_ov002_0210a424[];
    extern void func_0200d89c(void*);
  Player_DisableInteraction(((char*)this));
  mIsBodyClsnEnabled=0;
  int idx=mStateStep&1;
  this->SetAnim(data_ov002_0210a424[idx], 0x40000000, 0x1000, 0);
  mStateArg=0;
  mStateWork=0;
  func_0200d89c(data_0209f318);
  return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c607cEiiPi
int Player::func_ov002_020c607c(int p1, int p2, int *outptr)
{
    char *self = (char *)this;
    mLineClsnActor = 0;
    *outptr = (int)0x80000000;

    dBgCh_Lin rc;

    Vector3 a(this->mPosX, p1, this->mPosZ);
    Vector3 b(this->mPosX, p2, this->mPosZ);
    rc.SetObjAndLine(a, b, (dActor_c *)self);
    if (rc.DetectClsn() != 0) {
        Vector3 pos = rc.GetClsnPos();
        *outptr = pos.y;
        if (rc.GetClsnID() != -1) {
            dActor_c *found = dActor_c::FindWithID((u32)rc.GetClsnID());
            mLineClsnActor = found;
            if (found != 0) {
                int flag = (found->mFlags & 0x1000000) ? 1 : 0;
                if (flag != 0) {
                    return 1;
                }
                flag = (found->mFlags & 0x2000000) ? 1 : 0;
                if (flag != 0) {
                    return 2;
                }
            }
        }
    }
    return 0;
}

// @symbol _ZN6Player19func_ov002_020c61acEv
extern "C" {
int Player::func_ov002_020c61ac() {
    extern void func_ov002_020de968(void *);
    extern Player::State data_ov002_021102ec;
    extern Player::State data_ov002_021106dc;
    extern Player::State data_ov002_021106f4;
    extern Player::State data_ov002_021102bc;
    extern Player::State data_ov002_0210ffec;
    extern Player::State data_ov002_0211001c;
    extern Player::State data_ov002_021101b4;
    int outA;
    int outB;
    int v[3];
    int saved;
    int a;
    int b;

    if ((mClsnFlags & 5) == 0)
        return 0;
    if (this->IsState(data_ov002_021102ec) || this->IsState(data_ov002_021106dc) || this->IsState(data_ov002_021106f4) || this->IsState(data_ov002_021102bc) || this->IsState(data_ov002_0210ffec) || this->IsState(data_ov002_0211001c) || mIsUnderwater != 0)
        return 0;
    if ((mClsnFlags & 5) == 5) {
        if (mIsBalloon != 0 || mIsMega != 0) {
            int d;
            int yv, zv, xv;
            zv = this->mPosZ;
            d = 0xc8000;
            if (mIsBalloon != 0)
                d = 0x96000;
            yv = this->mPosY + d;
            xv = this->mPosX;
            v[0] = xv;
            v[1] = yv;
            v[2] = zv;
            dBgCh_Gnd obj;
            obj.StartDetectingWater();
            obj.SetObjAndPos(*(const Vector3 *)v, this);
            if (obj.DetectClsn()) {
                mGroundY = obj.clsnY;
                this->mPosY = mGroundY;
            }
            if (mIsBalloon != 0) {
                func_ov002_020de968(this);
                this->ChangeState(data_ov002_021101b4);
            } else {
                func_ov002_020bdd2c();
                this->ChangeState(data_ov002_021101b4);
                return 1;
            }
        }
    }
    a = func_ov002_020c607c(this->mPosY - 0x20000, this->mPosY + 0xa0000, &outA);
    saved = (int)mLineClsnActor;
    if (outA == 0x80000000)
        return 0;
    b = func_ov002_020c607c(this->mPosY + 0x1e000, this->mPosY - 0x28000, &outB);
    if (outB == 0x80000000)
        return 0;
    if (saved == (int)mLineClsnActor)
        return 0;
    if (outA + 0x20000 < outB)
        return 0;
    {
        int t = outA - outB;
        if (t < 0)
            t = -t;
        if (t >= 0xa0000)
            return 0;
    }
    if ((a | b) == 0)
        return 0;
    this->mPosY = outB;
    mScaleAnimStep = 0;
    this->ChangeState(data_ov002_021102ec);
    if (a != 0) {
        mStateWork = 1;
        mStatePhase = (unsigned char)a;
    } else if (b != 0) {
        mStateWork = 2;
        mStatePhase = (unsigned char)b;
    }
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c647cEi
/* recovered: shared common types */
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
int Player::func_ov002_020c647c(int arg1) {
    char* c = (char*)this;
    Vector3 v1;
    Vector3 v2;
    dBgCh_Lin line;
    int z1 = this->mPosZ;
    int x1 = this->mPosX;
    v1.x = x1;
    v1.y = arg1 - 0x1e000;
    v1.z = z1;
    int x2 = this->mPosX;
    int z2 = this->mPosZ;
    v2.x = x2;
    v2.z = z2;
    v2.y = arg1 + 0xaa000;
    int r5 = -64;
    line.SetObjAndLine(v1, v2, (dActor_c*)c);
    if (line.DetectClsn()) {
        ::Vector3 clsnPos = line.GetClsnPos();
        int d = (clsnPos.y - this->mPosY) / 0x1000;
        if (d > 0xa0) d = 0xa0;
        if (d < 0) d = 0;
        r5 = d;
    }
    return r5;
}

// @symbol _ZN6Player19func_ov002_020c6538Ev
int Player::func_ov002_020c6538() {
    dBgCh_Gnd rg;
    Vector3 v;
    int y;
    v.x = this->mPosX;
    y = this->mPosY;
    v.y = y;
    v.z = this->mPosZ;
    v.y = y + 0x32000;
    rg.SetObjAndPos(v, (dActor_c*)this);
    if (rg.DetectClsn() == 0) {
        return 0;
    }
    {
        int hit = rg.clsnY;
        int d = hit - this->mPosY;
        if (d < 0) d = -d;
        if (d >= 0x64000) {
            return 0;
        }
        this->mPosY = hit;
        mAttachOffsetY = this->mPosY;
        return 1;
    }
}

// @symbol _ZN6Player17St_Squish_CleanupEv
/* recovered: named members + shared header, real C++ method */
int Player::St_Squish_Cleanup()
{
    int val = 0x1000;
    if (mIsMega != 0)
        val = 0x3000;

    mScaleX = val;
    mScaleY = val;
    mScaleZ = val;

    *(int *)((char *)&mdCcAcPos_c.flags) &= ~4;
    return 1;
}

// @symbol _ZN6Player14St_Squish_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Squish_Main()
{
    extern u8 data_ov002_0211117c;
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_021101b4;
    extern int func_ov002_020d91e0(void*, int a, int b);
    extern void func_ov002_020db8bc(void*, int a);
    extern void KillPlayer(void);
    switch (mStateStep) {
    case 0:
        if (func_ov002_020c6908() != 0)
            return 1;
        {
            int r = func_ov002_020c647c(mAttachOffsetY);
            if (r >= 0) {
                mScaleY = (r << 12) / 160;
                if (r >= 0x20)
                    break;
                if (mStatePhase != 2)
                    break;
                Sound::PlayCharVoice(mCharacter, 7, mCamSpacePos);
                mScaleY = 0x100;
                mStateStep = 2;
                mStateArg = 1;
                if (mIsMetal == 0) {
                    if (func_ov002_020d91e0(((char *)this), 0x300, 1) != 0)
                        mStateArg = 2;
                }
                {
                    char *a = *(char **)((char *)&mAttachedActor);
                    if (a != 0) {
                        u16 id = ((dActor_c *)a)->actorID;
                        int b1 = (id == 0xa4);
                        int b2;
                        if (b1 || (b2 = (id == 0xa5)) != 0)
                            *(u8 *)(a + 0x407) = 1;
                    }
                }
            } else {
                mStateTimer = 0xa;
                (*(u8 *)(int)((char *)&mStateStep))++;
            }
        }
        break;
    case 1:
        if (mStateTimer != 0) {
            (*(int *)(int)((char *)&mScaleY)) += 0x100;
            if (mScaleY > 0x1000)
                mScaleY = 0x1000;
        } else {
            mScaleY = 0x1000;
            this->ChangeState(data_ov002_0211013c);
        }
        break;
    case 2:
        if (func_ov002_020c6908() != 0)
            return 1;
        if (func_ov002_020c647c(mAttachOffsetY) < 0) {
            if (mStateArg == 1) {
                func_ov002_020db8bc(((char *)this), 1);
                this->ChangeState(data_ov002_0211013c);
            } else {
                (*(u8 *)(int)((char *)&mStateStep))++;
                mStateArg = 0;
            }
        }
        break;
    case 3:
        if (mStateWork == 2 && func_ov002_020c6538() == 0) {
            this->ChangeState(data_ov002_021101b4);
            return 1;
        }
        if (mStateArg != 0)
            break;
        if (this->GetHealth() != 0) {
            func_ov002_020db8bc(((char *)this), 1);
            this->ChangeState(data_ov002_0211013c);
        }
        if (data_ov002_0211117c != 0)
            break;
        KillPlayer();
        (*(u8 *)(int)((char *)&mStateArg))++;
        break;
    case 4:
        if (mStateTimer != 0)
            break;
        if (mStateArg != 1) {
            mStateStep = 3;
            break;
        }
        func_ov002_020db8bc(((char *)this), 1);
        this->ChangeState(data_ov002_0211013c);
        break;
    }
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c6908Ev
extern "C" {
int Player::func_ov002_020c6908(){

    extern Player::State data_ov002_021101b4;
  if (mStateWork == 1) {
    int old;
    this->mPosY = mAttachOffsetY;
    old = this->mPosY;
    if (func_ov002_020c6538() == 0) {
      this->ChangeState(data_ov002_021101b4);
      return 1;
    }
    this->mPosY = old;
    goto ret0;
  } else if (mStateWork == 2) {
    if (func_ov002_020c6538() == 0) {
      this->ChangeState(data_ov002_021101b4);
      return 1;
    }
    goto ret0;
  }
ret0:
  return 0;
}
}

// @symbol _ZN6Player14St_Squish_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Squish_Init()
{
    typedef int Fix12i;
    extern void Player_ReleaseHeldActor(void*);
  Player_ReleaseHeldActor(((void*)this));
  this->SetAnim(0x47, 0, 0x1000, 0);
  *(unsigned char*)((char*)&mStateWork) = 0;
  *(unsigned char*)((char*)&mStatePhase) = 0;
  *(int*)((char*)&mHorzSpeed) = 0;
  *(int*)((char*)&mVertSpeed) = 0;
  *(int*)((char*)&mVertAccel) = 0;
  *(unsigned char*)((char*)&mStateStep) = 0;
  *(unsigned char *)((char *)&mSkipGroundUpdate) = 1;
  *(int*)((char*)&mAttachOffsetY) = *(int*)((char*)&mPosY);
  mdCcAcPos_c.flags |= 4;
  return 1;
}
}

// @symbol _ZN6Player12Unk_020c6a10Ej
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::Unk_020c6a10(unsigned int arg_)
{
    extern int func_ov002_020d91e0(void*, u32 a, int b);
    extern Player::State data_ov002_021102ec;
    extern int func_ov002_020d82f0(void*);
    u32 arg = (u32)arg_;

    int flag;

    if (!(mClsnFlags & 1)) return 0;
    if (this->IsState(data_ov002_021102ec)) return 0;
    if (!func_ov002_020d82f0(((char*)this))) return 0;

    flag = 1;
    if (mIsMetal == 0) {
        if (func_ov002_020d91e0(((char*)this), arg << 8, 1))
            flag = 0;
    }

    Sound::PlayCharVoice(mCharacter, 7, mCamSpacePos);
    this->ChangeState(data_ov002_021102ec);
    mScaleY = 0x100;
    mStateStep = 4;
    mStateTimer = 0x1e;
    mStateArg = flag;
    mScaleAnimStep = 0;
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c6adcEv
extern "C" {
int Player::func_ov002_020c6adc()
{

    extern Player::State data_ov002_021102a4;
    unsigned short v = (unsigned short)(mStateFlags & 8);
    if (v == 0)
        return 0;
    mStateFlags &= ~8;
    this->ChangeState(data_ov002_021102a4);
    return 1;
}
}

// @symbol _ZN6Player16St_Teleport_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Teleport_Main()
{
    extern ObjTeleport* GetTeleportDestObj(int i);
    extern void func_02035860(void*, void*);
    extern void Player_AdvanceAnims(void*);
    extern char* data_0209f318;
    extern int data_0209f32c;
    extern u8 data_0209f284;
    extern Player::State data_ov002_0211067c;
    extern Player::State data_ov002_0211013c;
    extern void func_0200d1e4(void*);
    extern void func_02020388(int);
    switch (mStateStep) {
    case 0:
        if (mOpacity > 1) {
            (this->mOpacity)--;
            break;
        }
        (this->mStateWork)++;
        if (mStateWork >= 0x10) {
            ObjTeleport* obj = GetTeleportDestObj((u8)(mTeleportId - 1));
            int tx = obj->x << 12;
            int tz = obj->z << 12;
            int ty = obj->y << 12;
            mPosX = tx;
            mPosY = ty;
            mPosZ = tz;
            mAreaId = obj->param & 0xf;
            mAngleY = ((obj->param >> 4) & 0xf) << 12;
            mPrevAngleY = mAngleY;
            mGroundY = 0x80000000;
            mWaterSurfaceY = 0x80000000;
            func_02035860(&mMeshClsn, &mPos);
            mOpacity = 0;
            func_0200d1e4(data_0209f318);
            Sound::PlayCharVoice(mCharacter, 0x19, mCamSpacePos);
            Sound::PlayBank0(0x19, mCamSpacePos);
            mStateStep = 1;
        }
        break;
    case 1:
        if (mStateWork != 0) {
            if (mStateWork >= 0x10) {
                if (data_0209f32c > mPosY + 0x64000)
                    mIsUnderwater = 1;
            }
            (this->mStateWork)--;
            break;
        }
        (this->mOpacity)++;
        func_02020388(mPlayerNo);
        if (mOpacity >= 0x1f) {
            int hit = 0;
            mOpacity = 0x1f;
            if (data_0209f32c >= mPosY + 0x64000) {
                Vector3 v;
                dBgCh_Gnd rc;
                {
                    int vz = mPosZ;
                    int vy = data_0209f32c + 0x64000;
                    int vx = mPosX;
                    v.x = vx;
                    v.y = vy;
                    v.z = vz;
                }
                rc.SetObjAndPos(v, this);
                if (rc.DetectClsn())
                    hit = 1;
            }
            if (hit)
                this->ChangeState(data_ov002_0211067c);
            else
                this->ChangeState(data_ov002_0211013c);
            data_0209f284 = 1;
            mTeleportTimer = 0x3c;
            func_02012790(0x24);
        }
        break;
    }
    G2x::SetBlendBrightness((volatile u16 *)0x4000050, 0x3f, mStateWork);
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player16St_Teleport_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Teleport_Init()
{
    extern void Player_DisableInteraction(void*);
  Player_DisableInteraction(((char*)this));
  mStateWork=0;
  mStateStep=0;
  mHorzSpeed=0;
  this->SetAnim(0x47, 0, 0x1000, 0);
  Sound::PlayBank0(0x19, mCamSpacePos);
  return 1;
}
}

// @symbol func_ov002_020c6e14
/* func_ov002_020c6e14 at 0x020c6e14
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
extern "C" {
int func_ov002_020c6e14(Player *actor) {
    extern unsigned char data_0209f2ac;
    extern short data_ov002_020ff1a0[];
    extern void* data_0209f318[];
    extern int NumStars(void);
    int idx;

    if (data_0209f2ac != 0) {
        idx = -1;
        switch (NumStars()) {
        case 1:  idx = 0; break;
        case 3:  idx = 1; break;
        case 8:  idx = 2; break;
        case 12: idx = 3; break;
        case 30: idx = 4; break;
        case 50: idx = 5; break;
        case 80: idx = 6; break;
        }

        if (idx == -1)
            return 0;

        if (actor->ShowMessage(*(fBase_c *)actor, data_ov002_020ff1a0[idx], 0, 0, 2)) {
            ((dCamera_c *)(data_0209f318[0]))->SetFlag_3();
            return 1;
        }
    }

    return 0;
}
}

// @symbol _ZN6Player21St_LevelEnter_CleanupEv
/* recovered: named members + shared header, real C++ method */
/* Player::St_LevelEnter_Cleanup() at 0x020c6f34 (ov002).
 * Cleanup hook of the ST_LEVEL_ENTER player state-machine state; a no-op
 * that returns VS_FAIL (1). The implicit Player* this is unused.
 */
s32 Player::St_LevelEnter_Cleanup()
{
    (void)this;
    return 1;
}

// @symbol _ZN6Player18St_LevelEnter_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Level-entry cutscene, per frame. mStateWork indexes a table of
 * POINTERS TO MEMBER FUNCTION -- the same ARM/Itanium pmf representation
 * Player::State uses -- and the selected step runs against the player. Six of
 * the mStateStep values additionally make the player the camera's focus
 * actor (dCamera_c::mFocusActor, through the camera pointer data_0209f318).
 */
extern "C" {
int Player::St_LevelEnter_Main()
{
    extern void (Player::*data_ov002_0211075c[])();
    extern dCamera_c *data_0209f318;
    extern void Player_AdvanceAnims(void*);
    switch (mStateStep) {
    case 8: case 9: case 11: case 12: case 16: case 18:
        data_0209f318->mFocusActor = this;
        break;
    case 10: case 13: case 14: case 15: case 17:
    default:
        break;
    }
    (this->*data_ov002_0211075c[mStateWork])();
    if (mStateArg == 0) Player_AdvanceAnims(this);
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c6fe4Ev
extern "C" {
void Player::func_ov002_020c6fe4()
{
    extern u8 data_0209f20c;
    int id;

    id = this->GetBodyModelID(this->param1 & 0xff, 0);
    if (mModels.mBodyModels[id]->WillHitFrame(0x58))
        Sound::PlayCharVoice(mCharacter, 0x35, mCamSpacePos);

    id = this->GetBodyModelID(this->param1 & 0xff, 0);
    if (mModels.mBodyModels[id]->WillHitFrame(0x75))
        data_0209f20c = 1;

    if (this->FinishedAnim()) {
        this->SetAnim(0x47, 0, 0x1000, 0);
        mStateWork = 2;
    }
}
}

// @symbol _ZN6Player19func_ov002_020c70acEv
void Player::func_ov002_020c70ac() {
    if (mStateTimer != 0) {
        this->mVertSpeed = 0;
        this->mHorzSpeed = 0;
        return;
    }
    mStateWaitTimer = 0;
    mStateWork = 0;
    this->mVertAccel = -0x4000;
    switch (mStateStep) {
    case 12:
        this->mAngleY = this->mAngleY + 0x8000;
    case 11:
    case 18:
        this->mHorzSpeed = 0x20000;
        mStateWaitTimer = 6;
        break;
    case 9:
        this->mAngleY = this->mAngleY + 0x8000;
    case 8:
    case 16:
        this->mVertSpeed = 0x40000;
        this->mHorzSpeed = 0x18000;
        break;
    case 13:
        this->mVertSpeed = 0x36000;
        this->mHorzSpeed = 0xa000;
        mStateArg = 0;
        break;
    case 17:
        this->mVertSpeed = 0x3c000;
        this->mHorzSpeed = 0xa000;
        mStateArg = 0;
        break;
    }
}

// @symbol _ZN6Player19func_ov002_020c7194Ev
extern "C" {
void Player::func_ov002_020c7194(){

    extern Player::State data_ov002_02110214;
  if(this->FinishedAnim() == 0) return;
  mStateStep = 1;
  if(func_ov002_020c44c4() != 0) return;
  this->ChangeState(data_ov002_02110214);
}
}

// @symbol _ZN6Player19func_ov002_020c71e0Ev
extern "C" {
void Player::func_ov002_020c71e0() {
    extern int func_ov002_020c6e14(void*);
    extern void func_ov002_020be008(void*);
    extern unsigned char data_0209f2bc[];
    extern unsigned char data_0209f2fc[];
    extern Player::State data_ov002_0211067c;
    extern Player::State data_ov002_0211013c;
    mLevelEntryDone = 1;
    if (data_0209f2bc[0] != 0) return;
    if (mStateWaitTimer == 0) {
        this->mPrevAngleY = this->mAngleY;
        if (mStateStep == 1) {
            this->ChangeState(data_ov002_0211067c);
        } else {
            this->ChangeState(data_ov002_0211013c);
            if (mKeyModelId < 0 && data_0209f2fc[0] == 1) {
                if (func_ov002_020c6e14(this) != 0) return;
            }
            if (func_ov002_020c44c4() != 0) return;
        }
    }
    func_ov002_020be008(this);
}
}

// @symbol func_ov002_020c72a4
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
void func_ov002_020c72a4(void* thisptr)
{
    extern int SurfaceInfo_TestFlag0x20(void*);
    extern int data_0209f32c;
    extern int data_0209212c;
    unsigned char* r4 = (unsigned char*)thisptr;
    Player *player = (Player *)r4;
    dBgCh_Gnd rg;
    Vector3 v;
    int z = player->mPosZ;
    int d = data_0209212c;
    int x = player->mPosX;
    v.x = x;
    v.y = d;
    v.z = z;
    rg.mProbeHeight = d << 1;
    rg.SetObjAndPos(v, (dActor_c*)thisptr);
    rg.StartDetectingWater();
    if (rg.DetectClsn()) {
        if (SurfaceInfo_TestFlag0x20((int*)&rg.surface) != 0) {
            player->mWaterSurfaceY = rg.clsnY;
            data_0209f32c = player->mWaterSurfaceY;
        }
    }
    rg.mProbeHeight = 0x1f4000;
    ((Player *)(thisptr))->func_ov002_020c71e0();
}
}

// @symbol _ZN6Player19func_ov002_020c7350Ev
extern "C" {
void Player::func_ov002_020c7350()
{
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned int u32;
    typedef signed char s8;
    extern u8 data_0209f20c;
    extern u8 data_0209f2d8;
    extern char data_0209caa0[];
    int id;

    if (mIsAirborne != 0)
        return;

    if (this->IsAnim(0x10)) {
        id = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(0x52)) {
            Sound::PlayCharVoice(mCharacter, 0x27, mCamSpacePos);
        }
    }

    if (this->IsAnim(0x85)) {
        id = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(0x20)) {
            Sound::PlayCharVoice(mCharacter, 0x35, mCamSpacePos);
        }
    }

    if (this->IsAnim(0xa5)) {
        id = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(0x23)) {
            Sound::PlayCharVoice(mCharacter, 0x37, mCamSpacePos);
        }
    }

    id = this->GetBodyModelID(param1 & 0xff, 0);
    if (mModels.mBodyModels[id]->WillHitFrame(0x8e)) {
        if (this->IsAnim(0xa5)) {
            data_0209f20c = 1;
        }
    }

    if (!this->FinishedAnim())
        return;

    {
        u8 st = mStateStep;
        if (st == 0x10 || st == 0x12) {
            void *actor;
            this->SetAnim(0x94, 0x40000000, 0x1000, 0);
            actor = Spawn(0x11a, mKeyModelId, mPos, &mAngle, this->mAreaId, -1);
            if (param1 == 2) {
                ((daObjKey_c *)actor)->func_ov089_0213115c(4);
            } else {
                ((daObjKey_c *)actor)->func_ov089_0213115c(1);
            }
            mStateWork = 6;
            return;
        }
    }

    if (this->IsAnim(0x85)) {
        data_0209f20c = 1;
    }

    {
        u8 v = data_0209f2d8;
        int isOne = (v == 1);
        if (isOne != false)
            goto setanim2;
        if (!(*(int *)(data_0209caa0 + 8) & 0x80))
            goto skip_setanim2;
        {
            int isTwo = (v == 2);
            if (isTwo != false)
                goto skip_setanim2;
        }
    setanim2:
        this->SetAnim(0x47, 0, 0x1000, 0);
    skip_setanim2:
        ;
    }

    mStateWork = 2;
}
}

// @symbol _ZN6Player19func_ov002_020c75f0Ev
extern "C" {
void Player::func_ov002_020c75f0()
{
    extern void func_02012694(u32 id, void*);
    extern u8 data_0209f2fc;
    if (this->IsAnim(0x56))
    {
        int model = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[model]->WillHitFrame(0)) {
            Sound::PlayBank0(6, mCamSpacePos);
        }
    }

    if (mIsAirborne == 0)
    {
        func_ov002_020c2f64();
        mStateArg = 0;
        mStateWork = 1;
        if (data_0209f2fc == 1 || data_0209f2fc == 2)
        {
            this->Heal(0x880);
        }
        switch (mStateStep)
        {
        case 3:
        case 10:
            this->SetAnim(0x51, 0x40000000, 0x1000, 0);
            break;
        case 4:
            this->mHorzSpeed = 0;
            this->SetAnim(0x55, 0x40000000, 0x1000, 0);
            break;
        case 6:
        case 7:
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
            this->mHorzSpeed >>= 2;
            this->mVertSpeed = -(this->mVertSpeed >> 2);
            mStateArg = 0;
            this->SetAnim(0x13, 0x40000000, 0x1000, 0);
            break;
        case 5:
        case 8:
        case 9:
        case 11:
        case 12:
        case 15:
            this->mHorzSpeed = 0;
            break;
        case 13:
        case 16:
        case 17:
        case 18:
            this->mHorzSpeed = 0;
            this->SetAnim(0x52, 0x40000000, 0x1000, 0);
            break;
        }
        if (data_0209f2fc == 2)
        {
            Sound::PlayCharVoice(mCharacter, 0x24, mCamSpacePos);
        }
    }
    else
    {
        u8 t = mStateStep;
        if (t == 0xb || t == 0xc || t == 0x12)
        {
            if (mStateWaitTimer == 1)
            {
                func_02012694(0x28, &mCamSpacePos);
            }
        }
    }
}
}

// @symbol _ZN6Player18St_LevelEnter_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_LevelEnter_Init()
{
    extern void Player_DisableInteraction(void*);
    extern u8 data_0209f2d8;
    extern s16 data_02082214[];
    extern signed char data_0209f2f8;
    extern u8 data_0209f264;
    extern u8 data_0209f2fc;
    extern u32 data_ov002_0210a7e8[];

    u32 anim = data_ov002_0210a7e8[mStateStep];
    if (SaveData::HasPlayerLostCap() != 0) {
        if (anim == 0x85) anim = 0xa5;
    }
    this->SetAnim(anim, 0, 0x1000, 0);

    u8 st = mStateStep;
    if (st != 3 && st != 0xa && st != 0) {
        mModels.mBodyModels[this->GetBodyModelID((u8)param1, 0)]->SetFlags(0x40000000);
        if (mStateStep == 2) {
            dExtFrameCtrl_c *p =
                &(dExtFrameCtrl_c &)*mModels.mBodyModels[this->GetBodyModelID((u8)param1, 0)];
            p->currFrame = 0x14000;
        }
    }

    mKeyModelId = -1;
    Player_DisableInteraction(((char*)this));
    mJumpComboStage = 0;
    mHorzSpeed = 0;
    mStateArg = 0;
    mPrevAngleY = mAngleY;
    mStateWork = 0;
    dBgCh_Gnd rg;

    switch (mStateStep) {
    case 0: {
        Vector3 pos;
        int hit;
        int y = mPosY;
        int z = mPosZ;
        int x = mPosX;
        int w = y + 0xc8000;
        pos.x = x;
        pos.y = w;
        pos.z = z;
        rg.SetObjAndPos(pos, this);
        hit = 0x80000000;
        if (rg.DetectClsn() != 0)
            hit = rg.clsnY;
        mPosY = hit;
        mStateWork = 2;
        {
            int b = (data_0209f2d8 == 2);
            if (b == 0) mStateWaitTimer = 0x1e;
            else mStateWaitTimer = 0;
        }
        break;
    }
    case 1:
        mVertAccel = 0;
        mStateWork = 4;
        mIsUnderwater = 1;
        break;
    case 5:
    case 15:
        mStateArg = 1;
        break;
    case 11:
    case 12: {
        s32 idx;
        s16 cosv, sinv;
        if (func_ov002_020c7cbc() != 0) mStateStep = 0x12;
        *(int*)((char*)&mPosY) += 0x64000;
        idx = ((s32)(u16)(s16)(mAngleY + 0x8000) >> 4) * 2;
        cosv = data_02082214[idx];
        sinv = data_02082214[idx + 1];
        *(int *)((char *)&mPosX) += FX_MUL(0xa0000, cosv);
        *(int *)((char *)&mPosZ) += FX_MUL(0xa0000, sinv);
        mStateTimer = 0x20;
        mStateArg = 1;
        mVertAccel = 0;
        mStateWork = 5;
        break;
    }
    case 8:
    case 9:
        if (func_ov002_020c7cbc() != 0) mStateStep = 0x10;
        mStateTimer = 2;
        mStateArg = 1;
        mVertAccel = 0;
        mStateWork = 5;
        break;
    case 13:
    case 17: {
        u8 st2 = mStateStep;
        if (st2 == 0x11) mStateTimer = 2;
        else mStateTimer = 0x20;
        mStateArg = 1;
        mVertAccel = 0;
        mStateWork = 5;
        if (data_0209f2f8 == 0x18) func_02012790(0x16);
        else if (data_0209f2f8 == 0x19) func_02012790(0x17);
        break;
    }
    case 2:
        if (param1 != 0) {
            mStateStep = 0xa;
            this->SetAnim(data_ov002_0210a7e8[mStateStep], 0, 0x1000, 0);
        } else {
            this->InitWingFeathers(0);
            mStateWork = 3;
            mVertSpeed = 0;
            mVertAccel = 0;
        }
        break;
    default:
        break;
    }

    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    {
        u8 c2 = data_0209f264;
        if (data_0209f2f8 == 2) {
            if (c2 == 0 || (c2 == 0xe && data_0209f2fc != 1)) {
                func_02012790(0x1d);
            }
        } else if (data_0209f2f8 == 1 && (c2 == 6 || c2 == 7 || (u32)(c2 - 9) <= 1)) {
            func_02012790(0x1d);
        }
    }

    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c7cbcEv
extern "C" {
int Player::func_ov002_020c7cbc()
{
    extern u8 data_0209f2fc;
    extern signed char data_02092124;
    extern void LoadKeyModels(int idx);
    if (data_0209f2fc == 1) {
        switch (data_02092124) {
        case 0x24: mKeyModelId = 0; break;
        case 0x26: mKeyModelId = 1; break;
        case 0x2d: mKeyModelId = 2; break;
        case 0x2f: mKeyModelId = 3; break;
        case 0x31: mKeyModelId = 4; break;
        }

        if (mKeyModelId >= 0) {
            LoadKeyModels(mKeyModelId);
            mLoadedResourceFlags |= 0x10;
            this->SetAnim(0x53, 0x40000000, 0x1000, 0);
            return 1;
        }
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c7dd0Ei
extern "C" {
void Player::func_ov002_020c7dd0(int i) {
    extern unsigned char data_ov002_020ff240[];
    extern unsigned char data_0209f2fc;
    extern Player::State data_ov002_021104b4;
    unsigned char g;
    mStateStep = data_ov002_020ff240[i];
    g = data_0209f2fc;
    if (g == 2) {
        unsigned char s = mStateStep;
        switch (s) {
        case 4:
            mStateStep = 5;
            break;
        case 8:
            mStateStep = 9;
            break;
        case 0xb:
            mStateStep = 0xc;
            break;
        }
        Heal(-0x880);
        Heal(0x100);
    } else if (g == 1) {
        if (mStateStep == 4) {
            mStateStep = 0xf;
        }
    }
    ChangeState(data_ov002_021104b4);
}
}

// @symbol _ZN6Player15IsEnteringLevelEv
/* recovered: named members + shared header, real C++ method */
int Player::IsEnteringLevel()
{
  if(this->IsStateEnteringLevel()==0) return 0;
  unsigned char s=mStateStep;
  if(s==8||s==9||s==0x10) return 1;
  return 0;
}

// @symbol _ZN6Player20IsStateEnteringLevelEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::IsStateEnteringLevel()
{

    extern Player::State data_ov002_021104b4;
    extern Player::State data_ov002_0211025c;
  if(this->IsState(data_ov002_021104b4)) return 1;
  return this->IsState(data_ov002_0211025c);
}
}

// @symbol _ZN6Player19func_ov002_020c7f10Ev
extern "C" {
int Player::func_ov002_020c7f10()
{
    extern void func_ov002_020ce9c8(void*);
    extern Player::State data_ov002_0211022c;
  unsigned char v;
  if (!this->IsState(data_ov002_0211022c)) goto fail;
  if (mStateStep != 0) goto fail;
  v = mNoCtrlKind;
  if (v == 0 || v == 1 || v == 0x11 || (unsigned char)(v + 0xfe) <= 1) {
    if (mStateArg != 0) goto fail;
    func_ov002_020ce9c8(this);
    this->mVertAccel = 0;
    mIsUnderwater = 1;
    mStateArg = 1;
    func_ov002_020c7ff8();
    return 1;
  }
  if (v == 8) {
    func_ov002_020ce9c8(this);
    this->mVertAccel = 0;
    mIsUnderwater = 1;
    this->SetAnim(0x6b, 0x40000000, 0x1000, 0);
    mStateStep = 9;
    goto fail;
  }
fail:
  return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c7ff8Ev
extern "C" {
void Player::func_ov002_020c7ff8()
{
    extern void *data_0209f318;
    extern void func_0200d4b0(void*, u8 playerID, int x);
    extern void func_0200d508(void*, u8 playerID);
    u8 s = mNoCtrlKind;
    void *g = data_0209f318;
    if (s == 0 || s == 1 || s == 0x11) {
        func_0200d4b0(g, mPlayerNo, -1);
    } else if (s == 3) {
        func_0200d4b0(g, mPlayerNo, 5);
    } else {
        func_0200d508(g, mPlayerNo);
    }
    if (mIsUnderwater != 0) {
        this->SetAnim(0xac, 0x40000000, 0x1000, 0);
        this->mVertAccel = 0;
    } else {
        this->SetAnim(0x19, 0x40000000, 0x1000, 0);
    }
    {
        u8 t = mNoCtrlKind;
        if (t == 0 || t == 3) {
            mStateStep = 1;
        } else {
            mStateStep = 2;
        }
    }
    this->mVertSpeed = 0;
    this->mVertAccel = this->mVertSpeed;
    mSkipGroundUpdate = 1;
    mIsBodyClsnEnabled = 0;
    Sound::PlayCharVoice(mCharacter, 0x1c, mCamSpacePos);
    if (mStatePhase != 0) return;
    func_02012790(0x2d);
}
}

// @symbol _ZN6Player20St_NoControl_CleanupEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_NoControl_Cleanup()
{
    extern int EndKuppaScript(void);
  int v=0x1000;
  if(mIsMega) v=0x3000;
  mScaleX=v;
  mScaleY=v;
  mScaleZ=v;
  if(mNoCtrlKind==0x11) EndKuppaScript();
  return 1;
}
}

// @symbol _ZN6Player17St_NoControl_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_NoControl_Main()
{
    extern short GetAngleToCamera(int i);
    extern void Player_AdvanceAnims(void*);
    extern Player::State data_ov002_0211013c;
    extern int IsStarCameraScript3Running(void);
    u8 mode = mNoCtrlKind;
    if (mode == 0 || mode == 1 || (mode == 0x11 && mStateStep != 0x18)) {
        if (IsStarCameraScript3Running() == 0) {
            mAngleY = GetAngleToCamera(mPlayerNo);
            mPrevAngleY = mAngleY;
        }
    }

    switch (mStateStep) {
    case 0:
        func_ov002_020c965c();
        break;
    case 1:
        if (func_ov002_020c94a4() != 0)
            return 1;
        break;
    case 2:
    case 9:
        func_ov002_020c92fc();
        break;
    case 10:
        func_ov002_020c9288();
        break;
    case 20:
        func_ov002_020c924c();
        return 1;
    case 21:
        if (func_ov002_020c91bc() != 0)
            return 1;
        break;
    case 3:
    case 6:
        if (mStateWaitTimer == 0)
            mHorzSpeed = 0;
        if (mNoCtrlKind == 6 && mIsUnderwater != 0) {
            mVertAccel = 0;
            mVertSpeed = 0;
        } else {
            func_ov002_020c9718();
        }
        if (mNoCtrlKind == 0 || mNoCtrlKind == 3)
            return 1;
        break;
    case 4:
    case 5:
        if (func_ov002_020c9128() != 0)
            return 1;
        break;
    case 7:
    case 8:
        func_ov002_020c8cb0();
        break;
    case 11:
        func_ov002_020c904c();
        break;
    case 12:
        func_ov002_020c8f80();
        break;
    case 13:
        func_ov002_020c8f0c();
        break;
    case 14:
        func_ov002_020c8b54();
        break;
    case 15:
        func_ov002_020c8a4c();
        break;
    case 16:
        func_ov002_020c897c();
        break;
    case 17:
        func_ov002_020c8d14();
        break;
    case 18:
        func_ov002_020c8b78();
        break;
    case 19:
        if (mIsOpeningBigDoor == 0)
            this->ChangeState(data_ov002_0211013c);
        break;
    case 22:
        if (mIsAirborne == 0) {
            this->SetAnim(0x98, 0x40000000, 0x1000, 0);
            mStateStep = 0x17;
        }
        break;
    case 23: {
        int id = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(0x1c))
            mHasNoCap = 0;
        if (this->FinishedAnim())
            this->ChangeState(data_ov002_0211013c);
        break;
    }
    case 24:
        func_ov002_020c8714();
        break;
    case 25:
        if (func_ov002_020c8540() != 0)
            return 1;
        break;
    case 26:
        if (func_ov002_020c84b0() != 0)
            return 1;
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c84b0Ev
extern "C" {
unsigned char Player::func_ov002_020c84b0() {
    char *c = (char *)this;
    extern short Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
    Vector3 v;
    short ang;
dummy_label_675641:;
    v.x = this->mPosX;
    v.y = this->mPosY;
    v.z = this->mPosZ;
    v.y += 0x64000;
    ang = Vec3_VertAngle((Vector3 *)&mStateTargetX, &v);
    ApproachLinear2(this->mAngleYSpeed, -ang, 0x400);
    mHeadRot[0] = 0;
    mHeadRot[1] = 0;
    mHeadRot[0] = this->mAngleYSpeed;
    c[0x742] = 3;
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c8540Ev
/* func_ov002_020c8540 at 0x020c8540 (ov002)
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 */
extern "C" {
int Player::func_ov002_020c8540()
{
    extern void func_0201f32c(int arg0);
    extern u16 data_ov002_020ff138[];
    extern u16 data_ov002_020ff13c[];
    extern u16 data_ov002_020ff110[];
    extern s16 data_ov002_020ff108[];
    extern int data_0209b454;
    extern u8 data_0209d660;
    extern Player::State data_ov002_0211013c;
    u32 s8;
    int id;

    s8 = this->param1;
    if (s8 <= 1) {
        id = this->GetBodyModelID(s8 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(data_ov002_020ff138[s8])) {
            mHasNoCap = 1;
        }
        s8 = this->param1;
        id = this->GetBodyModelID(s8 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(data_ov002_020ff13c[s8])) {
            mHasNoCap = 0;
        }
    }
    if (mStateArg == 0) {
        s8 = this->param1;
        id = this->GetBodyModelID(s8 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(data_ov002_020ff110[s8])) {
            char* o;
            mFlags |= 0x800000;
            o = (char *)mTalkActor;
            if (o != 0) {
                int *p = (int *)&((dActor_c *)o)->mFlags;
                *p &= ~0x800000;
            }
            data_0209b454 |= 0x800000;
            func_0201f32c(data_ov002_020ff108[this->param1]);
            mStateArg = 1;
            return 1;
        }
    } else {
        char* o;
        if (data_0209d660 != 0) return 1;
        data_0209b454 &= ~0x800000;
        mFlags &= ~0x800000;
        o = (char *)mTalkActor;
        if (o != 0) {
            int *p = (int *)&((dActor_c *)o)->mFlags;
            *p |= 0x800000;
            mTalkActor = 0;
        }
        if (this->FinishedAnim()) {
            this->ChangeState(data_ov002_0211013c);
        }
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c8714Ev
/* func_ov002_020c8714 @ 0x020c8714 (ov002, size 0x268)
 * Player cap-power ending update: while active, ducks the music, damps the
 * forward speed (or launches on variant 1), and streams the sparkle trail;
 * when the anim finishes either spawns the cap-return actor with the level
 * fanfare (mode 0x10) or spawns the star and hands the player to it.
 */
extern "C" {
int Player::func_ov002_020c8714()
{
    extern void PlayerChangeMusicVolumeHold2(int, int);
    extern u32 PlayerNewHold5(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                              void *callback);
    volatile int v[3];

    if (mStateArg == 0) {
        Sound::ChangeMusicVolume(0x14, 0x15666);
        if (mStateTimer == 0) {
            this->mHorzSpeed = FX_MUL(this->mHorzSpeed, 0xf60);
            if (mIsAirborne == 0) {
                this->mHorzSpeed = 0;
                this->mVertSpeed = 0;
                this->SetAnim(0x51, 0x40000000, 0x1000, 0);
                mStateArg = 1;
            }
        } else if (mStateTimer == 1) {
            this->SetAnim(0x56, 0, 0x1000, 0);
            this->mHorzSpeed = 0x1a000;
            this->mVertSpeed = 0x38000;
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
        }
        v[0] = this->mPosX;
        v[1] = this->mPosY;
        v[2] = this->mPosZ;
        /* mPosY (+0x60), re-read by index: this->mPosY lets the compiler reuse the
           load above and drops the ROM's second load (one word short, measured). */
        v[1] = ((int *)this)[0x18] + 0x3c000;
        mParticle1 = Particle::System::New(*(volatile int *)&this->mParticle1, 0x27, v[0], v[1],
                                           this->mPosZ, 0, 0);
    } else if (this->FinishedAnim() != 0) {
        if (mNoCtrlKind == 0x10) {
            mStateStep = 0x19;
            this->SetAnim(0x99, 0x40000000, 0x1000, 0);
            mTalkActor = 0;
            if (this->param1 <= 1) {
                mTalkActor = Spawn(0x10d, (this->param1 << 8) | 0xf, mPos, 0, this->mAreaId, -1);
            }
            switch (this->param1) {
            case 0:
                Sound::PlayBank0(0xd9, mCamSpacePos);
                break;
            case 1:
                Sound::PlayBank0(0xda, mCamSpacePos);
                break;
            case 2:
                Sound::PlayBank0(0xdb, mCamSpacePos);
                break;
            }
        } else {
            int st;
            this->mVertSpeed = 0;
            mAttachOffsetY = 0x187;
            func_ov002_020c7ff8();
            st = (int)Spawn(0xb2, 0x6f, mPos, &mAngle, this->mAreaId, -1);
            if (st != 0)
                ((daStar_c *)st)->func_ov002_020e7090((int)this);
        }
        mStateArg = 0;
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c897cEv
extern "C" {
int Player::func_ov002_020c897c()
{
    extern void func_02012694(u32 a, void*);
    int id;

    id = this->GetBodyModelID(this->param1 & 0xff, 0);
    if (mModels.mBodyModels[id]->WillHitFrame(0x5f)) {
        func_02012694(0x3b, &mCamSpacePos);
    } else {
        id = this->GetBodyModelID(this->param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(0x3c)) {
            func_02012694(0x42, &mCamSpacePos);
        } else if (this->FinishedAnim()) {
            func_ov002_020ca108();
            this->SetAnim(0x47, 0, 0x1000, 0);
            mRootMotionMode = 0;
        }
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c8a4cEv
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* extern "C": this is the ROM symbol's own name. A bare `extern` in a //cpp file
   makes the compiler mangle it AGAIN, into _Z15_ZN7Vector3D1Evv, which exists
   nowhere -- and the byte gate cannot see it, because relocated words are
   wildcarded. Only check_references does. */
extern "C" {
int Player::func_ov002_020c8a4c() {
    extern short Vec3_HorzAngle(const Vector3*, const Vector3*);
    extern void func_020731dc(void*, void*, void*);
    extern void Vec3_RotateYAndTranslate(void*, const Vector3*, int, const Vector3*);
    /* local extern: the registrar takes this D1 by address, which C++ cannot spell. */
    extern void *_ZN7Vector3D1Ev(void *object);
    extern int func_02053274(const Vector3*, const Vector3*);
    extern int data_ov002_0210e150;
    extern int data_ov002_0210f8cc[3];
    extern int data_ov002_0210f89c;
    int spd = this->mHorzSpeed >> 3;
    if (spd < 0x400) spd = 0x400;
    this->SetAnim(0x48, 0, spd, 0);
    this->mPrevAngleY = Vec3_HorzAngle(&mPos, (Vector3 *)&mStateTargetX);
    this->mAngleY = this->mPrevAngleY;
    if (func_02053274(&mPos, (Vector3 *)&mStateTargetX) < 0xa000) {
        if (mStateArg == 0) {
            mStateArg = 1;
            if (!(data_ov002_0210e150 & 1)) {
                data_ov002_0210f8cc[0] = 0;
                data_ov002_0210f8cc[1] = 0;
                data_ov002_0210f8cc[2] = 0x12c000;
                func_020731dc(data_ov002_0210f8cc, (void*)_ZN7Vector3D1Ev, &data_ov002_0210f89c);
                data_ov002_0210e150 |= 1;
            }
            Vec3_RotateYAndTranslate((Vector3 *)&mStateTargetX, (Vector3 *)&mStateAnchorX,
                                     mAngleXSpeed, (Vector3 *)data_ov002_0210f8cc);
        } else {
            func_ov002_020ca108();
        }
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c8b54Ev
int Player::func_ov002_020c8b54()
{
    if (mStateTimer == 0) {
        this->mHorzSpeed = 0xa000;
        mStateStep = 0xf;
    }
    return 0;
}

// @symbol _ZN6Player19func_ov002_020c8b78Ev
/* recovered: shared common types */
extern "C" {
int Player::func_ov002_020c8b78() {
    extern void PlayerChangeMusicVolumeHold3(unsigned int, int);
    extern void func_020731dc(void*, void*, void*);
    /* local extern: the registrar takes this D1 by address, which C++ cannot spell. */
    extern void *_ZN7Vector3D1Ev(void *object);
    extern void Vec3_RotateYAndTranslate(void*, void*, int, void*);
    extern int data_ov002_0210e154;
    extern int data_ov002_0210f824[];
    extern char data_ov002_0210f7f4[];
    Vector3 v;
    Sound::ChangeMusicVolume(0x14, 0x15666);
    {
        int id = GetBodyModelID(param1 & 0xff, false);
        dExtAnmModel_c *model = mModels.mBodyModels[id];
        dExtFrameCtrl_c *a = &static_cast<dExtFrameCtrl_c &>(*model);
        if (a->WillHitFrame(0x40))
            func_02012790(0x3a);
    }
    if (FinishedAnim() != 0) {
        if (!(data_ov002_0210e154 & 1)) {
            data_ov002_0210f824[0] = 0;
            data_ov002_0210f824[1] = 0x96000;
            data_ov002_0210f824[2] = 0x64000;
            func_020731dc(data_ov002_0210f824, (void *)_ZN7Vector3D1Ev, data_ov002_0210f7f4);
            data_ov002_0210e154 |= 1;
        }
        Vec3_RotateYAndTranslate(&v, &mPos, this->mAngleY, data_ov002_0210f824);
        mRestoreMusicVolume = 1;
        {
            u16 *fl = &this->mStateFlags;
            *fl |= 0x400;
        }
        {
            u8 saved = mIsOpeningBigDoor;
            unsigned int msg = 0x188;
            if (mStateArg)
                msg = 0x29;
            ShowMessage(*this, msg, &v, 0, 2);
            mIsOpeningBigDoor = saved;
        }
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c8cb0Ev
extern "C" {
int Player::func_ov002_020c8cb0(){
    extern signed char data_02092110;
  if (this->FinishedAnim()) {
    if (data_02092110 >= 0)
      mIsOpeningBigDoor = 1;
    func_ov002_020ca108();
    mRootMotionMode = 0;
    this->SetAnim(0x47, 0, 0x1000, 0);
  }
  return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c8d14Ev
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
int Player::func_ov002_020c8d14()
{
    extern void PlayerNewSimpleHold5(unsigned int id, int x, int y, int z);
    extern int func_ov002_020d22ec(void*, int arg);
    extern Player::State data_ov002_021106ac;
    extern Player::State data_ov002_0211013c;
    Vector3 v;
    int val;
    int id;
    char *anim;

    val = this->mScaleY + 0xf0;
    if (val >= 0x3000) val = 0x3000;
    this->mScaleX = val;
    this->mScaleY = val;
    this->mScaleZ = val;

    v.x = this->mPosX;
    v.y = this->mPosY;
    v.z = this->mPosZ;
    v.y = this->mPosY + FX_MUL(this->mScaleY, 0x50000);

    id = this->GetBodyModelID(param1 & 0xff, 0);
    if (mModels.mBodyModels[id]->WillHitFrame(4)) {
        Particle::System::NewSimple(0x5d, v.x, v.y, v.z);
    } else {
        id = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(2)) {
            Particle::System::NewSimple(0x5e, v.x, v.y, *(volatile int *)&v.z);
        }
    }

    id = this->GetBodyModelID(param1 & 0xff, 0);
    if (mModels.mBodyModels[id]->WillHitFrame(0x2f)) {
        func_ov002_020c9e18();
        mMegaTimer = 0x258;
    }

    id = this->GetBodyModelID(param1 & 0xff, 0);
    /* Launder kept: reading mBodyModels[id]->currFrame directly moves bytes. */
    anim = (char *)((long long)(int)&(dExtFrameCtrl_c &)*mModels.mBodyModels[id]);
    if ((unsigned int)(((dExtFrameCtrl_c *)anim)->currFrame << 4) >> 0x10 >= 0x2f) {
        if (mIsUnderwater == 0) {
            if (func_ov002_020d22ec(this, 0))
                return 0;
        }
    }

    if (this->FinishedAnim()) {
        if (mIsUnderwater != 0)
            this->ChangeState(data_ov002_021106ac);
        else
            this->ChangeState(data_ov002_0211013c);
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c8f0cEv
extern "C" {
int Player::func_ov002_020c8f0c(){
    extern void EnterBigBoosHaunt(void);
  int *p0 = &this->mScaleX;
  int *p1 = &this->mScaleY;
  int *p2 = &this->mScaleZ;
  *p0 -= 0x80;
  *p1 -= 0x80;
  *p2 -= 0x80;
  if (this->mScaleX < 0) {
    this->mScaleX = 0;
    this->mScaleY = 0;
    this->mScaleZ = 0;
  }
  if (mIsAirborne == 0) {
    EnterBigBoosHaunt();
    mStateStep = 6;
  }
  return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c8f80Ev
/* recovered: shared common types */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12t<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" {
int Player::func_ov002_020c8f80()
{
    typedef int Fix12t;
    char *p = (char *)this;

    this->mScaleX -= 0x80;
    this->mScaleY -= 0x80;
    this->mScaleZ -= 0x80;

    unsigned int id = GetBodyModelID(param1 & 0xff, false);
    if (mModels.mBodyModels[id]->WillHitFrame(1)) {
        if (this->mStateArg == 0) {
            this->mStateArg = 1;
        } else {
            this->SetAnim(0x43, 0x40000000, 0x1000, 0);
            this->mStateStep = 0xd;
            this->mVertAccel = -0x4000;
            mAngleX = 0x4000;
            Sound::PlayBank0(0x1a, mCamSpacePos);
        }
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c904cEv
extern "C" {
int Player::func_ov002_020c904c() {
    typedef unsigned int u32;
    typedef int s32;
    typedef unsigned char u8;
    extern int Math_Function_0203b14c(void *, int a, int b, int c, int d);
    int mid;
    char *anim;
    ApproachLinear(this->mPosX, this->mStateTargetX, 0x8000);
    ApproachLinear(this->mPosZ, this->mStateTargetZ, 0x8000);
    if (Math_Function_0203b14c(&mPosY, (this->mGroundY) + 0x190000, 0x200, 0x64000, 0x10000) == 0) {
        this->mStateStep = 0xc;
        this->SetAnim(0x56, 0, 0x1000, 0);
        this->mStateArg = 0;
    }
    if (this->IsAnim(0x56) != 0) {
        mid = this->GetBodyModelID((param1) & 0xff, 0);
        if (mModels.mBodyModels[mid]->WillHitFrame(0) != 0) {
            Sound::PlayBank0(6, mCamSpacePos);
        }
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c9128Ev
extern "C" {
int Player::func_ov002_020c9128() {

    extern Player::State data_ov002_0211013c;
    if (mStateStep == 4) {
        ApproachLinear2(mAngleYSpeed, 0x1000, 0x400);
    } else {
        if (ApproachLinear2(mAngleYSpeed, 0x4000, 0x100)) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
    }
    mHeadRot[0] = 0;
    mHeadRot[1] = 0;
    mHeadRot[2] = mAngleYSpeed;
    mHeadRotMode = 2;
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c91bcEv
extern "C" {
int Player::func_ov002_020c91bc()
{
    extern unsigned char data_0209d660;
    extern int data_0209b454;
    extern Player::State data_ov002_0211067c;
    extern Player::State data_ov002_0211013c;
    if (data_0209d660 != 0) return 1;
    mFlags &= ~0x800000;
    data_0209b454 &= ~0x800000;
    if (this->FinishedAnim()) {
        if (mIsUnderwater != 0)
            this->ChangeState(data_ov002_0211067c);
        else
            this->ChangeState(data_ov002_0211013c);
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c924cEv
extern "C" {
int Player::func_ov002_020c924c()
{
    extern void func_0201f32c(int x);
    if (mStateTimer == 0) {
        func_0201f32c((short)mAttachOffsetY);
        mStateStep = 0x15;
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c9288Ev
extern "C" {
int Player::func_ov002_020c9288(){

    extern Player::State data_ov002_0211067c;
    extern Player::State data_ov002_0211013c;
  if (!func_ov002_020bea7c()) {
    if (mStatePhase == 0) {
      func_ov002_020c43c4(6);
      mStatePhase = 1;
    } else if (mIsUnderwater != 0) {
      this->ChangeState(data_ov002_0211067c);
    } else {
      this->ChangeState(data_ov002_0211013c);
    }
  }
  return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c92fcEv
extern "C" {
int Player::func_ov002_020c92fc()
{
    extern void func_0200d3f8(void*, unsigned char playerID, void*);
    extern int data_0209b454;
    extern unsigned char data_0209f2d8;
    extern int data_0209caa0[];
    extern void *data_0209f318;
    extern Player::State data_ov002_0211067c;
    extern Player::State data_ov002_0211013c;
    int flag = 0;
    this->mVertSpeed = 0;

    if (mAttachOffsetY != -1) {
        if (mIsUnderwater == 0) {
            int id = this->GetBodyModelID(param1 & 0xff, flag);
            if (mModels.mBodyModels[id]->WillHitFrame(0x20))
                flag = 1;
        } else {
            if (this->FinishedAnim()) flag = 1;
        }
    }

    if (flag != 0) {
        mStateTimer = 0x14;
        mStateStep = 0x14;
        this->mVertSpeed = 0;
        mFlags |= 0x800000;
        data_0209b454 |= 0x800000;
        return 0;
    }

    if (this->FinishedAnim()) {
        if (mStateStep == 9) {
            if ((int)(data_0209f2d8 == 1) == 0) {
                if ((data_0209caa0[2] & 0x10000000) == 0 && SaveData::IsCharacterUnlocked(0) == 0 &&
                    mHatCharacter == 0 && mPrevCharacter == 3) {
                    mStateStep = 0xa;
                    mStatePhase = 0;
                    func_0200d3f8(data_0209f318, mPlayerNo, 0);
                    return 0;
                }
            }
        }
        if (mIsUnderwater != 0)
            this->ChangeState(data_ov002_0211067c);
        else
            this->ChangeState(data_ov002_0211013c);
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c94a4Ev
extern "C" {
int Player::func_ov002_020c94a4()
{
    extern signed char data_0209f2f8;
    extern u8 data_0209211c;
    extern u8 data_0209f200;
    extern void ExitLevel(void);
    unsigned short st;

    this->mVertSpeed = 0;

    st = mStateTimer;
    if (st != 0) {
        unsigned char sub;
        if (st != 1)
            goto ret1;

        sub = mNoCtrlKind;
        if (sub == 0) {
            if (data_0209f2f8 == 0x21) {
                data_0209211c = 4;
                data_0209f200 = 7;
            }
            ExitLevel();
            mStateStep = 6;
            goto ret1;
        }

        if (sub == 3) {
            switch (data_0209f2f8) {
            case 0x24:
                data_0209211c = 2;
                data_0209f200 = 0xa;
                break;
            case 0x26:
                data_0209211c = 4;
                data_0209f200 = 9;
                break;
            case 0x2d:
                data_0209211c = 0x32;
                data_0209f200 = 3;
                break;
            case 0x2f:
                data_0209211c = 3;
                data_0209f200 = 0;
                break;
            case 0x31:
                data_0209211c = 5;
                data_0209f200 = 0xb;
                break;
            }
            ExitLevel();
            mStateStep = 6;
        }
    ret1:
        return 1;
    }

    {
        int id = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(0x20))
            mStateTimer = 0x3c;
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c965cEv
extern "C" {
int Player::func_ov002_020c965c()
{
    u8 st;

    if (mIsAirborne != 0) {
        if (mIsUnderwater == 0) goto done;
    }

    this->mVertSpeed = 0;
    st = mNoCtrlKind;
    if (st == 0 || st == 1 || st == 0x11 || (u8)(st + 0xfe) <= 1) {
        func_ov002_020c7ff8();
        goto done;
    }
    if (st == 8) {
        this->SetAnim(0x6b, 0x40000000, 0x1000, 0);
        mStateStep = 9;
        goto done;
    }
    this->SetAnim(0x47, 0, 0x1000, 0);
    mStateStep = 3;

done:
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c9718Ev
/* Player::func_ov002_020c9718() at 0x020c9718
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */
extern "C" {
void Player::func_ov002_020c9718()
{
    unsigned char *self = (unsigned char *)this;
    extern int func_ov002_020ceaf4(void*);
    extern int data_0209f32c;
    int r2;
    if (self[0x706] == 0)
        return;
    mVertSpeed = func_ov002_020ceaf4(self);
    r2 = data_0209f32c - 0x50000;
    if (mPosY < r2)
        return;
    if (mVertSpeed >= 0)
        mPosY = r2;
}
}

// @symbol _ZN6Player17St_NoControl_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_NoControl_Init()
{
    extern int Player_DisableInteraction(void*);
    extern int Player_ReleaseHeldActor(void*);
    extern void (Player::*data_ov002_02110884[])();
    mIsControlDisabled = 1;
    mStatePhase = 0;
    Player_DisableInteraction(((char *)this));
    Player_ReleaseHeldActor(((char *)this));
    mRidingShell = 0;
    mHeldObj = 0;
    mGrabbedByActor = 0;
    mIsOpeningBigDoor = 0;
    (this->*data_ov002_02110884[mNoCtrlKind])();
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020c97e0Ev
void Player::func_ov002_020c97e0()
{
    mStateStep = 19;
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
}

// @symbol _ZN6Player19func_ov002_020c97f8Ev
extern "C" {
void Player::func_ov002_020c97f8(){
  mStateStep = 0x1a;
  mAngleYSpeed = 0;
  this->SetAnim(0x47, 0, 0x1000, 0);
  this->mHorzSpeed = 0;
  this->mVertSpeed = 0;
}
}

// @symbol _ZN6Player19func_ov002_020c9840Ev
extern "C" {
void Player::func_ov002_020c9840(){
  mHasNoCap = 0;
  mStateStep = 0x18;
  this->SetAnim(0x47, 0, 0x1000, 0);
  mStateTimer = 2;
  this->mHorzSpeed = 0;
  this->mVertSpeed = 0;
  mStateArg = 0;
  Sound::PlayCharVoice(mCharacter, 4, mCamSpacePos);
}
}

// @symbol _ZN6Player19func_ov002_020c98a4Ev
extern "C" {
void Player::func_ov002_020c98a4(){
    extern void func_0200d148(void*, unsigned char);
    extern void* data_0209f318[];
  mHasNoCap=0;
  mStateStep=0x18;
  this->SetAnim(0x47, 0, 0x1000, 0);
  mStateTimer=2;
  this->mHorzSpeed=0;
  this->mVertSpeed=0;
  mStateArg=0;
  func_0200d148(data_0209f318[0], mPlayerNo);
}
}

// @symbol func_ov002_020c990c
extern "C" {
void func_ov002_020c990c(Player *p) {
    typedef int Fix12;
    p->mHasNoCap = 1;
    if (p->mIsAirborne) {
        p->mStateStep = 0x16;
        p->SetAnim(0x54, 0x40000000, 0x1000, 0);
    } else {
        p->mStateStep = 0x17;
        p->SetAnim(0x98, 0x40000000, 0x1000, 0);
    }
    p->mHorzSpeed = 0;
    p->mVertSpeed = 0;
    Sound::PlayCharVoice(p->mCharacter, 0x29, p->mCamSpacePos);
}
}

// @symbol func_ov002_020c9998
/* recovered: shared common types */
extern "C" {
void func_ov002_020c9998(Player *p) {
    typedef int Fix12i;
    p->mStateStep = 0x12;
    p->SetAnim(0x92, 0x40000000, 0x1000, 0);
    dActor_c::Spawn(0xb2, 0xffff, p->mPos, &p->mAngle, p->mAreaId, -1);
    p->mHorzSpeed = 0;
    p->mVertSpeed = 0;
}
}

// @symbol _ZN6Player19func_ov002_020c9a04Ev
/* recovered: shared common types */
extern "C" {
void Player::func_ov002_020c9a04()
{
    extern void PlayerNewSimpleHold6(unsigned int id, int x, int y, int z);
    Vector3 v;

    mStateStep = 0x11;
    this->SetAnim(0x9a, 0x40000000, 0x1000, 0);

    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;

    v.x = this->mPosX;
    v.y = this->mPosY;
    v.z = this->mPosZ;
    v.y = this->mPosY + 0x28000;
    Particle::System::NewSimple(0x5f, v.x, v.y, v.z);

    v.y = this->mPosY + FX_MUL(this->mScaleY, 0x50000);
    Particle::System::NewSimple(0x60, v.x, v.y, *(volatile int *)&v.z);
}
}

// @symbol _ZN6Player19func_ov002_020c9ac0Ev
extern "C" {
void Player::func_ov002_020c9ac0(){
    mRootMotionMode = 1;
    mSkipGroundUpdate = 1;
    mIsBodyClsnEnabled = 0;
    this->mVertAccel = 0;
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
    mStateStep = 0x10;
    this->SetAnim(0x93, 0x40000000, 0x1000, 0);
}
}

// @symbol _ZN6Player19func_ov002_020c9b10Ev
extern "C" {
int Player::func_ov002_020c9b10(){
    mStateStep=0xe;
    this->SetAnim(0x47, 0, 0x1000, 0);
    mStateTimer=0x14;
    this->mHorzSpeed=0;
    this->mVertSpeed=0;
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c9b5cEv
void Player::func_ov002_020c9b5c() {
    mSkipGroundUpdate = 1;
    mIsBodyClsnEnabled = 0;
    this->mVertAccel = 0;
    func_ov002_020c9b10();
}

// @symbol func_ov002_020c9b7c
extern "C" {
void func_ov002_020c9b7c(Player *p) {
    typedef int Fix12i;
    extern int func_ov002_020e25f0(void*, int);
    p->mStateStep = 0xb;
    p->mIsBodyClsnEnabled = 0;
    if (p->mIsAirborne) {
        p->SetAnim(0x56, 0, 0x1000, 0);
        Sound::PlayBank0(6, p->mCamSpacePos);
    } else {
        p->SetAnim(0x4f, 0x40000000, 0x1000, 0);
        func_ov002_020e25f0(p, 0);
    }
    p->mVertAccel = 0;
    p->mHorzSpeed = 0;
    p->mVertSpeed = 0;
}
}

// @symbol _ZN6Player19func_ov002_020c9c00Ev
extern "C" {
int Player::func_ov002_020c9c00(){
    if(mIsAirborne!=0){
        this->SetAnim(0x54, 0x40000000, 0x1000, 0);
    }
    mStateStep=0;
    this->mHorzSpeed=0;
    this->mVertSpeed=0;
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c9c4cEv
extern "C" {
void Player::func_ov002_020c9c4c() {
    typedef int Fix12i;
    extern unsigned int data_ov002_020ff118[];
    this->SetAnim(data_ov002_020ff118[mStateStep], 0x40000000, 0x1000, 0);
    this->mStateStep += 7;
    mRootMotionMode = 1;
    mSkipGroundUpdate = 1;
    mIsBodyClsnEnabled = 0;
    this->mVertAccel = 0;
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
}
}

// @symbol _ZN6Player19func_ov002_020c9cc0Ev
extern "C" {
void Player::func_ov002_020c9cc0(){
    extern signed char data_02092110;
  mStateWaitTimer=3;
  mStateStep=6;
  mSkipGroundUpdate = 1;
  mIsBodyClsnEnabled=0;
  if(this->mVertSpeed>=0x14000) this->mVertSpeed=0x14000;
  if(data_02092110==0x26 || data_02092110==0x12) mOpacity=0;
}
}

// @symbol _ZN6Player19func_ov002_020c9d1cEv
extern "C" {
int Player::func_ov002_020c9d1c(){
    mStateStep=4;
    mAngleYSpeed=0x4000;
    this->SetAnim(0x47, 0, 0x1000, 0);
    this->mHorzSpeed=0;
    this->mVertSpeed=0;
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020c9d68Ev
extern "C" {
void Player::func_ov002_020c9d68(){
    extern short GetAngleToCamera(int i);
  if(mIsAirborne == 0){
    int b = (int)(mRidingShell != 0);
    if(b == 0) goto tail;
  }
  this->SetAnim(0x54, 0x40000000, 0x1000, 0);
tail:
  this->mAngleY = GetAngleToCamera(mPlayerNo);
  this->mPrevAngleY = this->mAngleY;
  mStateStep = 0;
  mStateArg = 0;
  this->mHorzSpeed = 0;
}
}

// @symbol func_ov002_020c9de4
extern "C" {
void func_ov002_020c9de4(char* c){
    extern void func_ov002_020d80d0(void*);
  ((Player *)(c))->func_ov002_020bdd9c();
  ((Player *)(c))->func_ov002_020bdef0();
  ((Player *)(c))->func_ov002_020bdd2c();
  func_ov002_020d80d0(c);
  ((Player *)(c))->func_ov002_020c9d68();
}
}

// @symbol _ZN6Player19func_ov002_020c9e18Ev
void Player::func_ov002_020c9e18()
{
    if (mIsNoControl != 1) return;
    mIsNoControl = 0;
    this->mdCcAcPos_c.flags &= ~4;
}

// @symbol Player_DisableInteraction
extern "C" {
void Player_DisableInteraction(char *self)
{
    Player *player = (Player *)self;

    player->mIsNoControl = 1;
    player->mdCcAcPos_c.flags |= 4u;
}
}

// @symbol _ZN6Player12Unk_020c9e5cEh
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::Unk_020c9e5c(unsigned char h)
{
    extern Player::State data_ov002_0211022c;
  if(!IsState(data_ov002_0211022c)) return 0;
  if(*(unsigned char*)((char*)&mNoCtrlKind) != h) return 0;
  switch(h){
  case 0:
  case 3:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==1) return 1; }
    break;
  case 0x11:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==0x18) return 1; }
    /* fallthrough */
  case 1:
  case 2:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==2 || (unsigned char)(s+0xec)<=1) return 1; }
    break;
  case 4:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==3) return 1; }
    break;
  case 5:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if((unsigned char)(s+0xfc)<=1) return 1; }
    break;
  case 6:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==6) return 1; }
    break;
  case 7:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if((unsigned char)(s+0xf9)<=1) return 1; }
    break;
  case 8:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==9 || (unsigned char)(s+0xec)<=1) return 1; }
    break;
  case 9:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if((unsigned char)(s+0xf5)<=2) return 1; }
    break;
  case 0xa:
  case 0xc:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if((unsigned char)(s+0xf2)<=1) return 1; }
    break;
  case 0xb:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==0x10) return 1; }
    break;
  case 0xd:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==0x11) return 1; }
    break;
  case 0xe:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==0x12) return 1; }
    break;
  case 0xf:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if((unsigned char)(s+0xea)<=1) return 1; }
    break;
  case 0x10:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if((unsigned char)(s+0xe8)<=1) return 1; }
    break;
  }
  return 0;
}
}

// @symbol func_ov002_020ca0f4
extern "C" {
int func_ov002_020ca0f4(Player *player) {
    extern Player::State data_ov002_0211022c;
    return player->IsState(data_ov002_0211022c);
}
}

// @symbol _ZN6Player19func_ov002_020ca108Ev
extern "C" {
void Player::func_ov002_020ca108(){

    extern Player::State data_ov002_0211013c;
  if(mIsOpeningBigDoor){
    mStateStep=0x13;
    return;
  }
  this->ChangeState(data_ov002_0211013c);
}
}

// @symbol _ZN6Player11OpenBigDoorEv
/* recovered: named members + shared header, real C++ method */
/* Player::OpenBigDoor() at 0x020ca144 (ov002).
 * Sets a u8 flag at offset 0x70b (1803) in the Player object to 1.
 * Player.h has only `u8 unk70b` here, so keep the typed offset access.
 */
void Player::OpenBigDoor()
{
    mIsOpeningBigDoor = 1; /* +0x70b */
}

// @symbol _ZN6Player12Unk_020ca150Eh
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::Unk_020ca150(unsigned char a)
{
    extern Player::State data_ov002_0211022c;
    extern Player::State data_ov002_0211013c;
  if(IsState(data_ov002_0211022c)){
    if(a==4){
      this->ChangeState(data_ov002_0211013c);
      return 1;
    }
    if(a==5) *(unsigned char*)((char*)&mStateStep)=5;
  }
  return 0;
}
}

// @symbol _ZN6Player17SetNoControlStateEhih
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::SetNoControlState(unsigned char a_, int b, unsigned char c_)
{


    extern Player::State data_ov002_0211010c;
    extern Player::State data_ov002_02110124;
    extern Player::State data_ov002_0211022c;
    int a = (int)a_;
    u8 c = (u8)c_;

    u8 flag;

    if (this->IsState(data_ov002_0211010c) || this->IsState(data_ov002_02110124)) return 0;

    flag = 0;
    if (a <= 3) {
        if (mIsNoControl != 0) return 0;
        flag = c;
    } else {
        if (mIsNoControl != 0) {
            if (c == 0) return 0;
        }
    }

    mNoCtrlKind = a;
    mAttachOffsetY = b;
    this->ChangeState(data_ov002_0211022c);
    mStatePhase = flag;
    return 1;
}
}

// @symbol func_ov002_020ca270
extern "C" {
int func_ov002_020ca270(char* c){
    Player *self = (Player *)c;

    extern Player::State data_ov002_02110124;
  if(self->IsState(data_ov002_02110124)){
    if(self->mStateStep==1) return 1;
  }
  return 0;
}
}

// @symbol _ZN6Player15JumpIntoBooCageER7Vector3
/* recovered: named members + shared header, real C++ method
 *
 * Refuses while airborne and still rising, then takes control, records the
 * cage position in mStateTarget and turns the player to face away from it.
 */
extern "C" {
int Player::JumpIntoBooCage(Vector3 & v_)
{
    extern short Vec3_HorzAngle(const Vector3*, const Vector3*);
    Vector3 *v = &v_;
    if (mIsAirborne) {
        if (mVertSpeed < 0) return 0;
    }
    if (!this->SetNoControlState(9, -1, 0))
        return 0;
    mStateTargetX = v->x;
    mStateTargetY = v->y;
    mStateTargetZ = v->z;
    mAngleY = Vec3_HorzAngle(v, &mPos) + 0x8000;
    return 1;
}
}

// @symbol _ZN6Player13TryTalkToDoorEh
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::TryTalkToDoor(unsigned char a)
{
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_0211043c;
  if(this->IsState(data_ov002_0211013c) || this->IsState(ST_WAIT) || this->IsState(data_ov002_0211043c)){
    *(unsigned char*)((char*)&mStateArg)=a;
    this->SetNoControlState(0xe, -1, 0);
    return 1;
  }
  return 0;
}
}

// @symbol _ZN6Player21IsOpeningDoorWithStarEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::IsOpeningDoorWithStar()
{
    return this->Unk_020c9e5c(0xe) || this->GetTalkState() > 0;
}
}

// @symbol _ZN6Player16TryTalkToKeyDoorEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::TryTalkToKeyDoor()
{
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_0211043c;
  if(this->IsState(data_ov002_0211013c) || this->IsState(ST_WAIT) || this->IsState(data_ov002_0211043c)){
      this->SetNoControlState(0xb, -1, 0);
      return 1;
  }
  return 0;
}
}

// @symbol _ZN6Player12Unk_020ca488Ev
/* recovered: named members + shared header, real C++ method */
/* Player::Unk_020ca488() @ 0x20ca488 (ov002) -- tail-call veneer to Player::Unk_020c9e5c(unsigned char) (0x20c9e5c) with r1=11.
 */
extern "C" {
int Player::Unk_020ca488() {
    return this->Unk_020c9e5c(11);
}
}

// @symbol _ZN6Player16TryEnterStarDoorER7Vector3s
/* recovered: named members + shared header, real C++ method
 *
 * Only from the walking, waiting or crouching states. Takes control, records
 * the door position in mStateAnchor and derives the approach point in
 * mStateTarget from it, and turns the player to face the door. The one-shot
 * guarded by bit 0 of data_ov002_0210e164 registers the offset vector once.
 */
extern "C" {
int Player::TryEnterStarDoor(Vector3 & pos_, short kind)
{
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_02110154;
    extern Player::State data_ov002_0211043c;
    extern int data_ov002_0210e164;
    extern int data_ov002_0210f344[3];
    extern int data_ov002_0210f308;
    /* local extern: the registrar takes this D1 by address, which C++ cannot spell. */
    extern void *_ZN7Vector3D1Ev(void *object);
    extern void func_020731dc(void *object, void *destructor, void *node);
    extern void Vec3_RotateYAndTranslate(void*, void*, short angle, void*);
    extern short Vec3_HorzAngle(const void *v0, const void *v1);
    int *pos = (int *)&pos_;
    if (IsState(data_ov002_0211013c) || IsState(data_ov002_02110154) || IsState(data_ov002_0211043c)) {
        if (this->SetNoControlState(0xa, -1, 0)) {
            mStateAnchorX = pos[0];
            mStateAnchorY = pos[1];
            mStateAnchorZ = pos[2];
            mAngleXSpeed = kind;
            mStateArg = 0;
            if (!(data_ov002_0210e164 & 1)) {
                data_ov002_0210f344[0] = 0;
                data_ov002_0210f344[1] = 0;
                data_ov002_0210f344[2] = 0x64000;
                func_020731dc(data_ov002_0210f344, (void *)_ZN7Vector3D1Ev, (void **)&data_ov002_0210f308);
                data_ov002_0210e164 |= 1;
            }
            Vec3_RotateYAndTranslate(&mStateTargetX, &mStateAnchorX, mAngleXSpeed,
                                     data_ov002_0210f344);
            mAngleY = Vec3_HorzAngle(pos, &mPosX) + 0x8000;
            return 1;
        }
    }
    return 0;
}
}

// @symbol _ZN6Player12CanEnterDoorEh
/* recovered: named members + shared header, real C++ method */
// Player::CanEnterDoor(unsigned char door)
// The +0x48 vcall on the held actor is dActor_c::OnYoshiTryEat (slot 18), and
// it must be spelled as a real C++ virtual call: the C function-pointer cast
// perturbs callee-saved homing rank to this=r4/door=r5, while virtual dispatch
// keeps the natural reverse-arg homing (door=r4, this=r5) the ROM has. See
// notes/mwccarm-codegen.md 6bc.
extern "C" {
int Player::CanEnterDoor(unsigned char door) {
    extern int data_ov002_0211022c[];
    extern int data_ov002_0211013c[];
    extern int data_ov002_02110154[];
    extern int data_ov002_0211043c[];
    extern Player::State data_ov002_0211004c;
    if (IsState(*(State *)data_ov002_0211022c)) {
        if (!(mStateStep == 0x13 && mIsOpeningBigDoor != 0))
            return 0;
    }
    if (IsState(*(State *)data_ov002_0211013c) || IsState(*(State *)data_ov002_02110154) ||
        IsState(*(State *)data_ov002_0211022c) || IsState(*(State *)data_ov002_0211043c)) {
        /* Yoshi with something in his mouth cannot take a door. If its
           OnYoshiTryEat() code is 6 (enemies such as daKrb_c and Scuttlebug,
           and the small crate daObjBlockS_c) or 1 (daBttBk_c, or another
           Player), he spits it out first; any other code (4 for a coin, a
           cap, a key or a star; 5; ...) just refuses the door. */
        if (mObjInMouth != 0) {
            if (((dActor_c *)mObjInMouth)->OnYoshiTryEat() == 6 ||
                ((dActor_c *)mObjInMouth)->OnYoshiTryEat() == 1) {
                this->ChangeState(data_ov002_0211004c);
            }
            return 0;
        }
        mStateStep = door;
        if (this->SetNoControlState(7, -1, 1) != 0)
            return 1;
    }
    return 0;
}
}

// @symbol _ZN6Player17PlayMammaMiaSoundEv
/* recovered: named members + shared header, real C++ method */
void Player::PlayMammaMiaSound()
{
    /* mCamSpacePosX/Y/Z are three consecutive words; dActor_c.h declares them
       individually rather than as one Vector3. */
    Sound::PlayCharVoice(mCharacter, 0x27, mCamSpacePos);
}

// @symbol _ZN6Player24TryExitWhiteDoorWithStarEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::TryExitWhiteDoorWithStar()
{
    extern Player::State data_ov002_0211022c;
  if(IsState(data_ov002_0211022c)){
    if(*(unsigned char*)((char*)&mStateStep)!=0x13 || *(unsigned char*)((char*)&mIsOpeningBigDoor)==0) return 0;
  }
  return this->SetNoControlState(0x11, -1, 1) != 0;
}
}

// @symbol func_ov002_020ca78c
extern "C" {
int func_ov002_020ca78c(Player *player) {
    extern Player::State data_ov002_0211022c;
    if (player->IsState(data_ov002_0211022c)) {
        if (player->mStateStep != 0x13 || player->mIsOpeningBigDoor == 0)
            return 0;
    }
    return player->SetNoControlState(0x10, -1, 1) != 0;
}
}

// @symbol _ZN6Player16St_DebugFly_MainEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_DebugFly_Main()
{
    extern int func_ov002_020bf224(int a, int b, int c);
    extern void Player_AdvanceAnims(void*);
    extern char data_0209f4a0[];
    extern char data_0209f49c[];
    extern char data_020a0e5a[];
    extern Player::State data_ov002_0211013c;
    u32 idx;
    u16 flags;

    mHorzSpeed = 0;
    mVertSpeed = 0;
    mClsnFlags = 0;

    if (*(s16 *)(data_0209f4a0 + gActivePlayerSlot * 0x18) != 0) {
        mPrevAngleY = mDesiredAngleY;
        mHorzSpeed = func_ov002_020bf224((int)((char *)this), 0x50000, 0);
    }

    idx = gActivePlayerSlot;
    flags = *(u16 *)(data_0209f49c + idx * 0x18);
    if (flags & 2) {
        *(int *)((char *)&mPosY) += 0x28000;
    } else if (flags & 1) {
        *(int *)((char *)&mPosY) -= 0x28000;
    }

    if (*(u16 *)(data_020a0e5a + idx * 4) & 0x400) {
        this->ChangeState(data_ov002_0211013c);
    }

    Player_AdvanceAnims(((char *)this));
    return 1;
}
}

// @symbol _ZN6Player16St_DebugFly_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_DebugFly_Init()
{
    extern void Player_DisableInteraction(void*);
    Player_DisableInteraction(((char *)this));
    mHorzSpeed = 0;
    mVertSpeed = 0;
    mVertAccel = 0;
    return 1;
}
}

// @symbol _ZN6Player12Unk_020ca8f8Ev
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::Unk_020ca8f8()
{
    extern Player::State data_ov002_0211064c;
    extern Player::State data_ov002_02110664;
  if(IsState(data_ov002_0211064c)) return 1;
  if(IsState(data_ov002_02110664)) return 2;
  return 0;
}
}

// @symbol func_ov002_020ca940
extern "C" {
void func_ov002_020ca940(char* self)
{
    Player *player = (Player *)self;
    extern u8 data_0209f2d8;
    extern int data_0209caa0[];
    extern u8 data_0209f49e[];
    extern int data_0209f318;
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_0211043c;
    extern Player::State data_ov002_02110154;
    extern Player::State data_ov002_0211064c;
    extern Player::State data_ov002_02110664;
    extern int func_0200d0ac(int a, unsigned int b);
    extern void func_0200d064(int a, unsigned int b);
    extern void func_ov002_020ca940(void*);
    u8 old;
    int h;
    int b;

    if (player->mIsNoControl != 0) return;

    b = data_0209f2d8;
    b = b == 1;
    if (b == false) {
        if (!(data_0209caa0[2] & 0x80)) return;
    }

    if (*(u16*)(data_0209f49e + (u8)gActivePlayerSlot * 0x18) & 0x8000) {
        old = player->mUseFarCamera;
        h = data_0209f318;
        (player->mUseFarCamera)++;

        if (player->mUseFarCamera > 2) {
            player->mUseFarCamera = 0;
        }

        switch (player->mUseFarCamera) {
        case 1:
            if (func_0200d0ac(h, player->mPlayerNo) != 0) break;
            player->mUseFarCamera = 2;
        case 2:
            if (player->mIsMega == 0) {
                b = data_0209f2d8;
                b = b == 1;
                if (b == false) {
                    if (player->IsState(data_ov002_0211013c) || player->IsState(data_ov002_0211043c) || player->IsState(data_ov002_02110154)) {
                        player->mStateFlags |= 4;
                        break;
                    }
                }
            }
            if (old == 0) {
                func_02012790(0xe);
                player->mUseFarCamera = old;
                break;
            }
            player->mUseFarCamera = 0;
        case 0:
            func_0200d064(h, player->mPlayerNo);
            if ((u16)(player->mStateFlags & 4) || player->IsState(data_ov002_0211064c)) {
                player->mUseFarCamera = 2;
            }
            break;
        default:
            player->mUseFarCamera = 0;
            func_0200d064(h, player->mPlayerNo);
            break;
        }
    }

    if (player->IsState(data_ov002_0211013c)) return;
    if (player->IsState(data_ov002_02110154)) return;
    if (player->IsState(data_ov002_0211043c)) return;
    if (player->IsState(data_ov002_0211064c)) return;
    if (player->IsState(data_ov002_02110664)) return;

    if ((u16)(player->mStateFlags & 4)) {
        player->mStateFlags &= ~4;
    }
}
}

// @symbol func_ov002_020cabe0
extern "C" {
void func_ov002_020cabe0(char* c){
    Player *self = (Player *)c;

    extern Player::State data_ov002_0211064c;
  unsigned short v = (unsigned short)(self->mStateFlags & 4);
  if (v == 0) return;
  self->ChangeState(data_ov002_0211064c);
}
}

// @symbol _ZN6Player15St_Null_CleanupEv
/* recovered: named members + shared header, real C++ method */
/* Player::St_Null_Cleanup() at 0x020cac20 (ov002).
 * Cleanup hook of the ST_NULL do-nothing player state; returns VS_FAIL (1).
 * The implicit Player* this is unused.
 */
s32 Player::St_Null_Cleanup()
{
    (void)this;
    return 1;
}

// @symbol _ZN6Player12St_Null_MainEv
/* recovered: named members + shared header, real C++ method */
/* Player::St_Null_Main() at 0x020cac28 (ov002).
 * Main hook of the ST_NULL do-nothing player state; returns VS_FAIL (1).
 * (All ST_NULL state functions are "return 1;" per Player.h.)
 */
s32 Player::St_Null_Main()
{
    (void)this;
    return 1;
}

// @symbol _ZN6Player12St_Null_InitEv
/* recovered: shared header, real C++ method
 *
 * ov002, 0x020cac30, size 0x8 -- `mov r0, #1; bx lr` and nothing else.
 *
 * The whole body IS the evidence for the name. Player::State holds three
 * `int (Player::*)()` members, ChangeState calls mInit last on the state
 * being entered, and a handler returns 1 for success. A state called Null
 * whose Init does nothing but succeed is exactly eight bytes. ov002 reaches
 * this address by `kind:load ... module:overlay(2)` from 0x0210a17c, which
 * sits in the eight-byte-stride pointer-to-member run that holds those
 * State objects.
 *
 * This file carried the placeholder name func_ov002_020cac30 until the symbol
 * was moved here from ov006, where it had been sitting on an unrelated 0x6c
 * function at the same shared address. See the commit and include/Player.h.
 */
int Player::St_Null_Init()
{
    return 1;
}

// @symbol _ZN6Player21St_CameraZoom_CleanupEv
/* recovered: named members + shared header, real C++ method */
int Player::St_CameraZoom_Cleanup()
{
    *(unsigned short *)((char *)&mStateFlags) &= ~4;
    mUseFarCamera = 0;
    return 1;
}

// @symbol func_ov002_020cac60
extern "C" {
int func_ov002_020cac60(char *self)
{
    Player *player = (Player *)self;
    extern unsigned char NumStars(void);
    extern short GetAngleToCamera(int i);
    extern void LoadLevel(int a, int b, int c, int d, int e);
    extern short data_0209f5e8;
    short ang;
    short angle;
    if (player->mSurfaceType == 0xd && NumStars() >= 0xe) {
        ang = player->mHeadRot[2];
        angle = GetAngleToCamera(player->mPlayerNo);
        if (ang <= 0x1c00 && angle >= -0x1000 && angle <= 0x1000) {
            LoadLevel(0x1f, 0, 1, 0, 0xe);
            dScene_c::SetAndStopColorFader();
            *(short *)((char *)&data_0209f5e8 + 0xc) = 0x7fff;
            func_02012790(0x1e);
            return 1;
        }
    }
    return 0;
}
}

// @symbol _ZN6Player18St_CameraZoom_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * The zoomed-in camera pose. Runs only while data_02092110 is negative. Losing
 * the 0x4 state flag ends it: back to walking, the body model's animation
 * field at +0x50+8 is cleared, and the facing angle is latched. Landing, or
 * any of the 0x8f03 input bits, clears that flag.
 */
extern "C" {
int Player::St_CameraZoom_Main()
{
    extern void func_ov002_020cae10(void*);
    extern int func_ov002_020cac60(void*);
    extern void Player_AdvanceAnims(void*);
    extern signed char data_02092110;
    extern Player::State data_ov002_0211013c;
    extern unsigned short data_0209f49e;
    if (data_02092110 < 0) {
        if ((unsigned short)(mStateFlags & 4) == 0) {
            unsigned int id;
            this->ChangeState(data_ov002_0211013c);
            id = GetBodyModelID(param1 & 0xff, 0);
            /* currFrame of the body model's frame controller, kept split: the ROM adds
               +0x50 and stores at +8, and ->currFrame folds that to one +0x58. */
            int q = (int)((*(dExtAnmModel_c *volatile *)&mModels.mBodyModels[id])) + 0x50;
            ((dExtFrameCtrl_c *)q)->currFrame = 0;
            mPrevAngleY = mAngleY;
            return 1;
        }
        if (mIsAirborne) {
            mStateFlags &= ~4;
        }
        func_ov002_020cae10(this);
        if (*(unsigned short*)((char*)&data_0209f49e + gActivePlayerSlot * 0x18) & 0x8f03) {
            mStateFlags &= ~4;
        }
        func_ov002_020cac60(this);
    }
    Player_AdvanceAnims(this);
    return 1;
}
}

// @symbol func_ov002_020cae10
extern "C" {
void func_ov002_020cae10(char* c)
{
    Player *self = (Player *)c;
    typedef int Fix12i;
    extern unsigned char data_0209f250;
    extern void* data_0209f318;
    extern short GetAngleToCamera(int i);
    int r4 = 0x4000;
    int ang;
    int r8;
    if (self->mPlayerNo == data_0209f250) {
        r4 -= ((dCamera_c *)data_0209f318)->mAngleX;
        r4 = (short)r4;
    }
    ang = (short)((self->mAngleY - GetAngleToCamera(self->mPlayerNo)) + 0x8000);
    r8 = ang;
    if (r8 < (int)0xffffd556) r8 = (int)0xffffd556;
    if (r8 > 0x2aaa) r8 = 0x2aaa;
    self->mHeadRot[1] = (short)FX_MUL(r8, 0xc00);
    self->mHeadRot[2] = (short)FX_MUL(r4, 0xc00);
    self->mHeadRotMode = 2;
}
}

// @symbol _ZN6Player18St_CameraZoom_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_CameraZoom_Init()
{
    extern int func_0200d064(void*, int playerID);
    extern void func_0200d7e0(void*, int playerID);
    extern struct CameraRaw* data_0209f318;
    extern unsigned char data_0209f28c;
  *(int*)((char*)&mHorzSpeed)=0;
  *(int*)((char*)&mVertSpeed)=0;
  struct CameraRaw* cam = data_0209f318;
  int pid = *(unsigned char*)((char*)&mPlayerNo);
  func_0200d064(cam, pid);
  func_0200d7e0(cam, *(unsigned char*)((char*)&mPlayerNo));
  this->SetAnim(0x47, 0, 0x1000, 0);
  data_0209f28c=0;
  return 1;
}
}

// @symbol func_ov002_020caf68
extern "C" {
void func_ov002_020caf68(Player *self) {
    extern void func_ov002_020e63a4(void*);
    self->mClimbClsn = 0;
    self->mdCcAcPos_c.flags &= ~4u;
    self->mdCcAcPos_c.flags |= 8u;
    func_ov002_020e63a4(self);
}
}

// @symbol func_ov002_020caf98
extern "C" {
int func_ov002_020caf98(Player *self, Arg *arg)
{
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_021105bc;
    extern Player::State data_ov002_0211031c;
    extern Player::State data_ov002_021101b4;
    extern Player::State data_ov002_021106dc;
    extern int func_ov002_020e0478(void*);
    extern int func_ov002_020e3078(void*, void*);
    char *p = (char *)self;
    int b354, b358;

    if (self->IsState(data_ov002_0211013c) == 0 && self->mClimbCooldown == 0) {
        b354 = (self->mRidingShell != 0);
        if (b354 == 0 && self->mClimbClsn == 0) {
            b358 = (self->mHeldObj != 0);
            if (b358 == 0 && self->mIsTakingDamage == 0 && self->mIsNoControl == 0 &&
                self->mIsUnderwater == 0 && self->mIsMega == 0) {
                goto cont;
            }
        }
    }
    return 0;

cont:
    if (self->IsInAir() != 0) goto state_check;
    if (self->IsBeingShotOutOfCannon() != 0) goto state_check;
    if (self->IsState(data_ov002_021105bc) != 0) goto state_check;
    if (self->IsState(data_ov002_0211031c) != 0) goto state_check;

    if (func_ov002_020e0478(self) == 0) goto ret0a;

state_check:
    if (self->IsState(data_ov002_021101b4) != 0) {
        if (func_ov002_020e3078(self, &data_ov002_021106dc) != 0) return 0;
    }

    if (((dCc_c *)arg)->flags & 0x400000)
        return 0;

    {
        Ret *r = arg->m2();
        if (self->mPosY - r->y < -0x64000)
            self->mPosY = r->y - 0x64000;
    }

    self->mClimbClsn = (dCc_c *)arg;
    self->ChangeState(data_ov002_021106dc);
    return 1;

ret0a:
    return 0;
}
}

// @symbol _ZN6Player17St_Headstand_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Headstand_Main()
{
    extern void Player_AdvanceAnims(void*);
    extern u16 data_0209f49c[];
    extern u16 data_0209f49e[];
    extern s16 data_0209f4a4[];
    extern void func_ov002_020cc05c(void*, unsigned short);
    extern Player::State data_ov002_02110724;
    extern Player::State data_ov002_021106dc;
    Vector3 &r = mClimbClsn->GetPos();
    mPosX = r.x;
    mPosZ = r.z;
    mPosY = r.y + mClimbClsn->height;

    u8 st = mStateStep;
    switch (st) {
    case 2:
        if (this->FinishedAnim()) {
            this->SetAnim(0x1d, 0, 0x1000, 0);
            mStateStep = 3;
        }
        break;
    case 3: {
        int idx = gActivePlayerSlot * 0x18;
        s16 val = *(s16*)((char*)data_0209f4a4 + idx);
        u16 mag = *(u16*)((char*)data_0209f49c + idx);
        if (val > 0x200) {
            this->SetAnim(0x1f, 0x40000000, 0x1000, 0);
            mStateStep = 4;
        }
        func_ov002_020cc05c(((char*)this), mag);
        {
            s16* p = (s16*)((char*)&mAngleY);
            *p = *p + mAngleYSpeed;
        }
        if (*(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18) & 2) {
            mPrevAngleY = mAngleY;
            this->ChangeState(data_ov002_02110724);
            return 1;
        }
        break;
    }
    case 4:
        if (this->FinishedAnim()) {
            this->ChangeState(data_ov002_021106dc);
        }
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player17St_Headstand_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Headstand_Init()
{
    extern void func_0200d580(void*, int playerID);
    extern struct CameraRaw *data_0209f318;
    this->SetAnim(0x1e, 0x40000000, 0x1000, 0);
    *(unsigned char *)((char *)&mStateStep) = 2;
    *(int *)((char *)&mVertAccel) = 0;
    *(int *)((char *)&mHorzSpeed) = 0;
    *(int *)((char *)&mVertSpeed) = 0;
    func_0200d580(data_0209f318, *(unsigned char *)((char *)&mPlayerNo));
    *(unsigned char *)((char *)&mHideShadow) = 1;
    return 1;
}
}

// @symbol func_ov002_020cb354
/* recovered: shared common types */
extern "C" {
int func_ov002_020cb354(Player *thisptr) {
    unsigned char* p = (unsigned char*)thisptr;
    Player *player = (Player *)p;
    int a = thisptr->IsAnim(0x24);
    if (a == 0) return a;
    int idx = thisptr->GetBodyModelID(player->param1 & 0xff, 0);
    dExtAnmModel_c *m = player->mModels.mBodyModels[idx];
    /* q is the frame controller (+0x50); m->currFrame would fold +0x50 and +8. */
    int* q = (int*)(((int)m + 0x50));
    if ((int)((unsigned int)((((dExtFrameCtrl_c *)q)->currFrame) << 4) >> 0x10) < 8) {
        unsigned char f = player->mStateArg;
        if (f != 0) return f;
        if (player->mClimbClsn->flags & 0x1000000)
            Sound::PlayBank0(0xd, player->mCamSpacePos);
        else
            Sound::PlayBank0(0xb0, player->mCamSpacePos);
        player->mStateArg = 1;
        return 1;
    }
    player->mStateArg = 0;
    return 0;
}
}

// @symbol func_ov002_020cb400
extern "C" {
int func_ov002_020cb400(char *c) {
    Player *self = (Player *)c;
    extern int func_02012194(int a0, int a1, int a2, int a3, int a4, int a5, int a6);
    int v = self->mAngleYSpeed / 16;
    if (v >= 0x100)
        v = 0x100;
    int code = 0x107;
    if (self->mClimbClsn->flags & 0x1000000)
        code = 0x105;
    int r = func_02012194(self->mLoopingSoundHandle, 0, code, 3, v, (int)&self->mCamSpacePos, 0);
    self->mLoopingSoundHandle = r;
    return r;
}
}

// @symbol func_ov002_020cb474
extern "C" {
void func_ov002_020cb474(char *c) {
    Player *self = (Player *)c;
    extern u32 PlayerNewHold6(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                              void *callback);
    extern signed char data_0209f2f8;
    dCc_c *r1 = self->mClimbClsn;
    if ((r1->flags & 0x1000000) == 0)
        return;
    if ((r1->height >> 2) > self->mAttachOffsetY)
        return;
    volatile int v[3];
    int z = self->mPosZ;
    int y = self->mPosY + 0x32000;
    int x = self->mPosX;
    v[0] = z ? x : x;
    v[1] = y;
    v[2] = z;
    unsigned int id;
    switch (data_0209f2f8) {
    case 0x13:
    case 0xa:
        id = 0xba;
        break;
    case 6:
        id = 0xb8;
        break;
    case 0x10:
        return;
    default:
        id = 0xb9;
        break;
    }
    self->mParticle1 = (s32)Particle::System::New(*(volatile unsigned int *)&self->mParticle1, id,
                                                  v[0], v[1], v[2], 0, 0);
}
}

// @symbol _ZN6Player16St_Climb_CleanupEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Climb_Cleanup()
{
    extern void func_ov002_020caf68(void*);
    extern int data_ov002_021106f4[];
    extern int data_ov002_021106dc[];
  int v = *(int*)((char*)&mRequestedState);
  if(v != (int)data_ov002_021106dc && v != (int)data_ov002_021106f4){
    func_ov002_020caf68(((void *)this));
    mClimbCooldown = 8;
  }
  mJumpComboTimer = 0;
  return 1;
}
}

// @symbol _ZN6Player13St_Climb_MainEv
/* Player::St_Climb_Main (ov002:0x020cb5bc, 0x778).
 *
 * func_ov002_020cc05c takes the pad-held halfword as a second argument it
 * never reads (decl_common.h declares (void*, unsigned short); the matched
 * St_Headstand_Main passes it too). Dropping it recolors the whole
 * case-1/tail scratch web down one register (r2 -> r1, 9 words); passing it
 * keeps the data_0209f49c load live in r1 through both call sites, which is
 * the ROM's coloring.
 *
 * SetAnim retains its scalar Fix12 ABI declaration. The climbed object's
 * position comes from mClimbClsn->GetPos() (dCc_c's slot 2). Player fields
 * and state changes use the shared class interface where their types are
 * established.
 */
extern "C" {
int Player::St_Climb_Main() {
    extern void Vec3_RotateYAndTranslate(void*, void*, s16 angle, void*);
    extern void func_0200d5c0(void*, u8 playerID);
    extern void func_0200d5fc(void*, s32 playerID);
    extern void ApproachAngle(void*, s16 target, s32 divisor, s32 band, s32 maxStep);
    extern int func_ov002_020cb354(void*);
    extern s16 GetAngleToCamera(u8 i);
    extern void func_ov002_020bf340(void*, void*, s32 delta, s32 limit);
    extern void func_ov002_020cb400(void*);
    extern void func_ov002_020cb474(void*);
    extern int func_ov002_020cbea8(void*);
    extern void Player_AdvanceAnims(void*);
    extern void* data_0209f318;
    extern u16 data_0209f49e[];
    extern u16 data_0209f49c[];
    extern s16 data_02082214[];
    extern Player::State data_ov002_021101b4;
    extern Player::State data_ov002_021106f4;
    extern u8 data_0209f4ac[];
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_0211070c;
    extern s16 data_0209f4a4[];
    extern void func_ov002_020cc05c(void*, unsigned short);
    s32 vbuf[6];

    mPeakY = mPosY;

    {
        Vector3 &p = mClimbClsn->GetPos();
        vbuf[0] = p.x;
        vbuf[1] = p.y;
        vbuf[2] = p.z;
    }
    if (param1 == 2) {
        vbuf[4] = 0;
        vbuf[3] = 0;
        vbuf[5] = -0x10000;
        Vec3_RotateYAndTranslate(&vbuf[0], &mClimbClsn->GetPos(), mAngleY, &vbuf[3]);
    }
    mPosX = vbuf[0];
    mPosZ = vbuf[2];

    {
        void* cam = data_0209f318;
        if (mClimbClsn->flags & 0x1000000) {
            func_0200d5c0(cam, mPlayerNo);
        } else {
            s32 v = FX_MUL(mClimbClsn->height, 0xb33);
            if (v <= mAttachOffsetY) {
                func_0200d5c0(cam, mPlayerNo);
            } else {
                func_0200d5fc(cam, mPlayerNo);
            }
        }
    }

    switch (mStateStep) {
    case 0:
        ApproachAngle(&this->mAngleYSpeed, 0, 8, 0x2000, 0x200);
        if (FinishedAnim()) {
            this->SetAnim(0x26, 0, 0x1000, 0);
            mStateStep = 1;
        }
        goto tail;

    case 1: {
        s16 v;
        u32 idx = (u32)gActivePlayerSlot * 0x18;
        u16 flagsE = *(u16*)((char*)data_0209f49e + idx);
        u16 flagsC = *(u16*)((char*)data_0209f49c + idx);
        if (flagsE & 0x400) {
            s16 ang = (s16)(mAngleY + 0x8000);
            s32 t = (u16)ang;
            s32 ti = (t >> 4) * 2;
            s16 cosv = data_02082214[ti];
            mPosX += FX_MUL(cosv, 0x64000);
            s16 sinv = data_02082214[ti + 1];
            mPosZ += FX_MUL(sinv, 0x64000);
            this->ChangeState(data_ov002_021101b4);
            return 1;
        }

        {
            v = *(s16*)((char*)data_0209f4a4 + idx);
            if (v < -0x400) {
                func_ov002_020cb354(this);
                mAngleYSpeed = 0;
                ApproachLinear(this->mAngleY, GetAngleToCamera(mPlayerNo), 0x200);
                if (mClimbClsn->height <= mAttachOffsetY) {
                    if ((mClimbClsn->flags & 0x2000000) && mVertSpeed == 0 &&
                        (s32)(*(s16 *)((char *)data_0209f4a4 + (u32)gActivePlayerSlot * 0x18)) <
                            -0xc00 &&
                        mStateWork == 1) {
                        this->ChangeState(data_ov002_021106f4);
                    }
                    mAttachOffsetY = mClimbClsn->height;
                    mVertSpeed = 0;
                } else {
                    u32 idx2 = (u32)gActivePlayerSlot * 0x18;
                    s32 mag;
                    if (*((u8*)data_0209f4ac + idx2) == 0) {
                        mag = (*(u16*)((char*)data_0209f49c + idx2) & 0x800) ? 0xc000 : 0x6660;
                    } else {
                        mag = 0xc000;
                    }
                    s16 av = *(s16*)((char*)data_0209f4a4 + idx2);
                    if (av < 0) av = -av;
                    s32 fx = FX_MUL(mag, av);
                    func_ov002_020bf340(this, &this->mVertSpeed, 0x800, fx);
                    this->SetAnim(0x24, 0, 0x1000, 0);
                    s32 fx2 = FX_MUL(fx, 0x500);
                    if (fx2 >= 0x4000) fx2 = 0x4000;
                    {
                        s32 modelIdx = GetBodyModelID((u8)param1, 0);
                        char *p2 = (char *)((long long)(int)&(
                                                dExtFrameCtrl_c &)*mModels.mBodyModels[modelIdx]);
                        ((dExtFrameCtrl_c *)p2)->speed = fx2;
                    }
                    mStateWork = 0;
                }
                goto end;
            } else if (v > 0x200) {
                func_ov002_020cb400(this);
                mStateTimer = 0x1e;
                {
                    u32 idx3 = (u32)gActivePlayerSlot * 0x18;
                    s32 mag;
                    if (*((u8*)data_0209f4ac + idx3) == 0) {
                        mag = (*(u16*)((char*)data_0209f49c + idx3) & 0x800) ? 0x1e000 : 0xfff0;
                    } else {
                        mag = 0x1e000;
                    }
                    s16 val = *(s16*)((char*)data_0209f4a4 + idx3);
                    s32 fx = FX_MUL(mag, val);
                    func_ov002_020bf340(this, &this->mVertSpeed, -0x800, fx);
                }
                mAngleYSpeed -= 0x40;
                {
                    u32 idx4 = (u32)gActivePlayerSlot * 0x18;
                    s16 av = *(s16*)((char*)data_0209f4a4 + idx4);
                    if (av < 0) av = -av;
                    if (mAngleYSpeed <= av) av = mAngleYSpeed;
                    mAngleYSpeed = av;
                }
                if (!IsAnim(0x25)) {
                    this->SetAnim(0x25, 0, 0x1000, 0);
                }
                if (mAttachOffsetY < -0x64000) {
                    this->ChangeState(data_ov002_021101b4);
                    return 1;
                }
                goto end;
            } else {
                mStateTimer = 0x1e;
                mVertSpeed = 0;
                func_ov002_020cc05c(this, flagsC);
                if (!IsAnim(0x26)) {
                    this->SetAnim(0x26, 0, 0x1000, 0);
                }
                goto end;
            }
        }
    }

    default:
        goto tail;
    }

end:
    if (mIsAirborne == 0) {
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }
    if (*(u16*)((char*)data_0209f49e + (u32)gActivePlayerSlot * 0x18) & 2) {
        mPrevAngleY = mAngleY;
        this->ChangeState(data_ov002_0211070c);
        return 1;
    }

tail: {
    s32 y = mClimbClsn->GetPos().y;
    if ((mPosY - y) < -0x64000) {
        mPosY = y - 0x64000;
    }
}
    if (mAttachOffsetY < -0x64000) {
        mAttachOffsetY = -0x64000;
    }
    {
        s32 t2 = mClimbClsn->height;
        if (t2 <= mAttachOffsetY) {
            mAttachOffsetY = t2;
        }
    }
    if (mJumpComboTimer != 0 || (mVertSpeed != 0 && mStateStep == 1)) {
        func_ov002_020cb474(this);
    }
    {
        s16* p8e = &this->mAngleY;
        *p8e = (s16)(*p8e + mAngleYSpeed);
    }
    {
        s32* p688 = &this->mAttachOffsetY;
        *p688 += mVertSpeed;
    }
    if (func_ov002_020cbea8(this)) {
        return 1;
    }
    {
        mPosY = mAttachOffsetY + mClimbClsn->GetPos().y;
    }
    if (*(s16*)((char*)data_0209f4a4 + (u32)gActivePlayerSlot * 0x18) == 0) {
        mStateWork = 1;
    }
    Player_AdvanceAnims(this);
    return 1;
}
}

// @symbol _ZN6Player13St_Climb_InitEv
extern "C" {
int Player::St_Climb_Init() {
    extern int Player_ReleaseHeldActor(void*);
    extern int func_ov002_020e3078(void*, void*);
    extern char data_ov002_021106f4[];
    Player_ReleaseHeldActor(this);

    int *bitfield = (int *)&mdCcAcPos_c.flags;
    *bitfield |= 4;
    *bitfield &= ~8;

    if (mHorzSpeed <= 0xa000) {
        mAngleYSpeed = 0;
        this->SetAnim(0x26, 0, (Fix12i)0x1000, 0);
        mStateStep = 1;
    } else {
        int val = mHorzSpeed / 16;
        mAngleYSpeed = (short)val;
        if (mAngleYSpeed >= 0x3000)
            mAngleYSpeed = 0x3000;
        this->SetAnim(0x27, 0x40000000, (Fix12i)0x1000, 0);
        mStateStep = 0;
    }

    Vector3 &res = mClimbClsn->GetPos();

    mPosX = res.x;
    mPosZ = res.z;
    mAttachOffsetY = mPosY - res.y;
    mVertAccel = 0;
    mHorzSpeed = 0;
    mVertSpeed = 0;

    Sound::PlayCharVoice(mCharacter, 0xd, mCamSpacePos);

    mStateWork = 0;
    mWalkTimer = 3;
    this->mdCcAcPos_c.height = 0x5a000;
    mHideShadow = 1;

    {
        dCc_c *b = mClimbClsn;
        if (b->flags & 0x1000000) {
            if ((b->height >> 2) <= this->mAttachOffsetY) {
                if (func_ov002_020e3078(this, data_ov002_021106f4) == 0) {
                    mJumpComboTimer = 0xf;
                }
            }
        }
    }
    return 1;
}
}

// @symbol func_ov002_020cbea8
#define LA(p) ((int)(p))
extern "C" {
int func_ov002_020cbea8(char *c)
{
    Player *self = (Player *)c;
    extern int func_ov002_020cc01c(void*);
    char *o;
    int amt;
    int b;
    int ca8;
    int *p;
    int t;

    if (self->mdCcAcPos_c.otherOwner == 0) return 0;
    if ((self->mdCcAcPos_c.hitFlags & 0x400000) == 0)
        return 0;
    o = (char *)dActor_c::FindWithID(self->mdCcAcPos_c.otherOwner);
    if (o == 0 || o == c) return 0;
    b = (int)(((dActor_c *)o)->actorID == 0xbf);
    if (b == 0) return 0;
    if ((int)self->mClimbClsn == 0 || (int)((Player *)o)->mClimbClsn == 0)
        return 0;
    if (func_ov002_020cc01c(c) != 0 || func_ov002_020cc01c(o) != 0) return 1;
    ca8 = self->mVertSpeed;
    amt = self->mdCcAcPos_c.pushback.y;
    if (ca8 == 0 || ((Player *)o)->mVertSpeed == 0)
        amt = amt * 2;
    if (ca8 != 0) {
        p = &self->mAttachOffsetY;
        *p += amt * 2;
        self->mAngleYSpeed = 0;
        self->mVertSpeed = 0;
    }
    if (((Player *)o)->mVertSpeed != 0) {
        p = (int *)LA(&((Player *)o)->mAttachOffsetY);
        *p -= amt * 2;
        ((Player *)o)->mAngleYSpeed = 0;
        t = ((Player *)o)->mSpeedZ;
        ((Player *)o)->mVertSpeed = 0;
        ((Player *)o)->mSpeedZ = t;
    }
    return 0;
}
}
#undef LA

// @symbol func_ov002_020cc01c
extern "C" {
int func_ov002_020cc01c(char* c){
    Player *self = (Player *)c;

    extern Player::State data_ov002_021101b4;
  if (self->mWalkTimer == 0) return 0;
  self->ChangeState(data_ov002_021101b4);
  return 1;
}
}

// @symbol func_ov002_020cc05c
extern "C" {
void func_ov002_020cc05c(char *self)
{
    Player *player = (Player *)self;
    extern s16 data_0209f4a2[];
    extern void func_ov002_020cc05c(void*, unsigned short);
    int idx = gActivePlayerSlot * 0x18;
    /* Through (int): plain player->mAngleYSpeed reschedules the 0x69c add and moves bytes. */
    s16 curval = *(s16 *)((int)&player->mAngleYSpeed);
    *(s16 *)((int)&player->mAngleYSpeed) =
        curval + (s16)FX_MUL(*(s16 *)((char *)data_0209f4a2 + idx), 0x80);
    {
        s16 tv2 = *(s16*)((char*)data_0209f4a2 + idx);
    {
        s16 av = tv2;
        if (av < 0) av = (s16)(-av);
        {
            int mag32 = FX_MUL(av, 0x480);
            s16 mag = (s16)mag32;
            s16 nmag = (s16)(-mag);
            s16 cur2 = player->mAngleYSpeed;
            s16 result;
            if (cur2 < nmag) result = nmag;
            else {
                if (cur2 <= mag) mag = cur2;
                result = mag;
            }
            player->mAngleYSpeed = result;
        }
    }
}
}
}

// @symbol _ZN6Player9IsOnShellEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::IsOnShell()
{
    extern Player::State data_ov002_02110304;
  if(IsState(data_ov002_02110304)) return 1;
  *(int*)((char*)&mRidingShell)=0;
  return 0;
}
}

// @symbol func_ov002_020cc16c
extern "C" {
int func_ov002_020cc16c(Player *player, void *state) {
    extern void func_ov002_020d80d0(void *);
    extern Player::State data_ov002_02110304;
    if (player->mRidingShell != 0)
        return 0;
    if (player->mIsMetal != 0 || player->mIsVanish != 0) {
        player->func_ov002_020bdef0();
        player->func_ov002_020bdd9c();
    }
    if (player->mIsFire != 0)
        func_ov002_020d80d0(player);
    player->mRidingShell = (s32)state;
    player->ChangeState(data_ov002_02110304);
    return 1;
}
}

// @symbol func_ov002_020cc1f4
extern "C" {
void func_ov002_020cc1f4(Player *self) {
    extern Player::State data_ov002_0211013c;
    if (self->mRidingShell == 0) return;
    self->St_Shell_Cleanup();
    self->ChangeState(data_ov002_0211013c);
}
}

// @symbol _ZN6Player16St_Shell_CleanupEv
/* recovered: named members + shared header, real C++ method */
int Player::St_Shell_Cleanup()
{
    char *p = *(char **)((char *)&mRidingShell);
    if (p != 0) {
        ((daShl_c *)p)->mCarrier = 0;
        mRidingShell = 0;
        mPosY += 0x46000;
        mIsAirborne = 1;
        mLandSoundPlayed = 0;
        EndStateMusic(0x33);
    }
    return 1;
}

// @symbol _ZN6Player13St_Shell_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Shell_Main()
{
    extern void func_ov002_020eeca8(void*, int arg);
    extern int atan2_hold_Player_13(int a, int b);
    extern void ApproachAngle(void*, short target, int divisor, int band, int maxStep);
    extern void func_ov002_020e25f0(void*, int a);
    extern void func_ov002_020e28d4(void*, int a, int b);
    extern void Player_AdvanceAnims(void*);
    extern s16 data_0209f4a0[];
    extern u16 data_0209f49e[];
    extern u16 data_0209f49c[];
    extern Player::State data_ov002_0211004c;
    extern Player::State data_ov002_021104e4;
    extern int func_02035638(void*);
    extern int func_0203564c(int);
    extern int AngleDiff(int, int);
    extern void func_ov002_020cc660(void*, int);
    extern int func_ov002_020d674c(void*);
    int flag = 0;
    void* res;

    if (func_02035638((u8*)((char*)&mMeshClsn))) {
        flag = 1;
        res = (void*)func_0203564c((int)((char*)&mMeshClsn));
        if (((dBgPi *)res)->GetClsnID() != -1) {
            void *a = dActor_c::FindWithID((u32)((dBgPi *)res)->GetClsnID());
            if (a) {
                u16 type = ((dActor_c *)a)->actorID;
                if ((type == 0x14 ? flag : 0) || (type == 0x15 ? 1 : 0)) {
                    func_ov002_020eeca8((void*)((char*)&mMeshClsn), (int)((char*)this));
                    flag = 0;
                }
            }
        }
    }

    if (mMeshClsn.IsOnWall()) {
        int ang = cstd::atan2(mWallNormalX, mWallNormalZ);
        if (AngleDiff(ang, mPrevAngleY) > 0x6000) {
            if (mMeshClsn.GetWallResult()->surface.clps.GetFloorClass() != 1) {
                flag = 1;
            }
        }
    }

    if (flag) {
        func_ov002_020c1eb4((s16)(mAngleY + 0x8000));
        return 1;
    }

    if (mStateStep == 0) {
        s16 target;
        int accel;
        if (*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) != 0) {
            target = mDesiredAngleY;
            accel = 0x40000;
        } else {
            target = mAngleY;
            accel = 0x18000;
        }
        s16 angleSave = mPrevAngleY;
        ApproachAngle((short*)((char*)&mPrevAngleY), target, 8, 0x800, 0x100);
        mAngleY = mPrevAngleY;
        func_ov002_020cc660(((char*)this), angleSave);
        {
            int spd = mHorzSpeed;
            int step = 0x1100;
            if (spd > 0) step = 0x600;
            ApproachLinear(*((int *)((char *)&mHorzSpeed)), accel, step);
        }
        func_ov002_020c04e0();
        if (mIsAirborne != 0) {
            (*(u8*)((char*)&mStateStep))++;
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
        }
        {
            int off = gActivePlayerSlot * 0x18;
            if (*(u16*)((char*)data_0209f49e + off) & 2) {
                (*(u8*)((char*)&mStateStep))++;
                mVertSpeed = (mHorzSpeed >> 2) + 0x2a000;
                mIsAirborne = 1;
                mLandSoundPlayed = 0;
                func_ov002_020e25f0(((char*)this), 0);
                return 1;
            } else if (*(u16*)((char*)data_0209f49c + off) & 0x400) {
                if (func_ov002_020d674c(((char*)this))) {
                    this->ChangeState(data_ov002_0211004c);
                } else {
                    mStateStep = 0;
                    this->ChangeState(data_ov002_021104e4);
                }
                return 1;
            }
        }
    } else {
        if (mIsAirborne == 0) {
            mStateStep = 0;
            mHeadRot[2] = 0x1000;
            mSpineRot[2] = mHeadRot[2];
        } else {
            mHeadRot[2] = 0;
            mSpineRot[2] = mHeadRot[2];
            func_ov002_020e28d4(((char*)this), 0x1800, 0x800);
            mVertAccel = -0x4000;
            mTerminalVelocity = -0x4b000;
            if (mVertSpeed >= 0) {
                mVertAccel = -0x8000;
                if (*(u16*)((char*)data_0209f49c + gActivePlayerSlot * 0x18) & 2) {
                    mVertAccel = -0x3400;
                }
            } else {
                if ((*(u16*)((char*)data_0209f49c + gActivePlayerSlot * 0x18) & 2) && mHasWings != 0) {
                    mVertAccel = -0x2000;
                    mTerminalVelocity = -0x25800;
                }
            }
        }
    }

    if (this->FinishedAnim() && this->IsAnim(0x35)) {
        this->SetAnim(0x34, 0, 0x1000, 0);
    }
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol func_ov002_020cc660
extern "C" {
void func_ov002_020cc660(char *self, int angle){
    extern s32 ApproachAngle(void*, s16 from, s16 start, s16 speed, s16 max);
    extern void func_ov002_020cc660(void*, int);
    Player *p = (Player *)self;
    s16 v, w;
    int delta;
    int third;
    delta = angle - p->mPrevAngleY;
    third = p->mHorzSpeed / 12;
    v = (s16)FX_MUL(delta, third);
    if (v < -0x1800)
        v = -0x1800;
    else if (v > 0x1800)
        v = 0x1800;
    ApproachAngle(&p->mSpineRot[1], v, 8, 0x200, 0x20);
    p->mHeadRot[0] = -p->mSpineRot[1];
    p->mAngleZ = p->mSpineRot[1];
    w = (s16)FX_MUL(p->mHorzSpeed, 0xaa);
    if (w < 0)
        w = 0;
    else if (w > 0x1000)
        w = 0x1000;
    ApproachAngle(&p->mSpineRot[2], w, 8, 0x200, 0x20);
    p->mHeadRotMode = 1;
    p->mModelOffsetY = 0x46000;
}
}

// @symbol _ZN6Player13St_Shell_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Shell_Init()
{
    extern int Player_ReleaseHeldActor(void*);
  Player_ReleaseHeldActor(((char*)this));
  this->SetAnim(0x35, 0x40000000, 0x1000, 0);
  mStateStep=0;
  StartStateMusic(0x33);
  return 1;
}
}

// @symbol _ZN6Player17St_HurtWater_MainEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_HurtWater_Main() {
    extern int func_ov002_020cec2c(void *);
    extern int func_ov002_020ceaf4(void *);
    extern int func_ov002_020ceb7c(void *);
    extern int Player_AdvanceAnims(void *);
    extern Player::State data_ov002_0211067c;
    extern Player::State data_ov002_021106ac;
    extern int data_0209f32c[];
    if (func_ov002_020cec2c(((char *)this)))
        return 1;
    ApproachLinear(*((int *)((char *)&mHorzSpeed)), 0, 0x800);
    int t = func_ov002_020ceaf4(((char *)this));
    ApproachLinear(*((int *)((char *)&mVertSpeed)), t, 0x800);
    if (this->FinishedAnim()) {
        if ((mStateStep & 1) == 0) {
            mPrevAngleY = mPrevAngleY + 0x8000;
            mAngleY = mPrevAngleY;
        }
        if (this->GetHealth()) {
            if (mIsMetal == 0)
                this->ChangeState(data_ov002_0211067c);
            else
                this->ChangeState(data_ov002_021106ac);
            mIsTakingDamage = 0;
            if (mHurtDamage)
                mInvincibleTimer = 0x24;
        } else {
            func_ov002_020c5dec(8);
            return 1;
        }
    }
    int lim = *(int *)data_0209f32c - 0x50000;
    if (mPosY >= lim && (mClsnFlags & 1) == 0) {
        if (mVertSpeed >= 0) {
            mPosY = lim;
            mVertSpeed = 0;
        }
    }
    func_ov002_020ceb7c(((char *)this));
    Player_AdvanceAnims(((char *)this));
    return 1;
}
}

// @symbol _ZN6Player17St_HurtWater_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_HurtWater_Init()
{
    extern int data_0209f318[];
    extern int data_ov002_02109fe4[];
    extern void func_0200d89c(void*);
  unsigned a=data_ov002_02109fe4[mStateStep&1];
  this->SetAnim(a, 0x40000000, 0x1000, 0);
  mStateWork=0;
  mStateArg=0;
  mVertAccel=0;
  mTerminalVelocity=-0xc000;
  mVertSpeed=0x10000;
  mIsTakingDamage=1;
  if(mHurtDamage){
    int b=mStateStep&0xf0;
    if(b==0 || b==0x10){
      func_ov002_020d93ac();
      func_ov002_020d94cc();
    }
  }
  func_0200d89c(*(char**)data_0209f318);
  return 1;
}
}

// @symbol _ZN6Player23St_MetalWaterWater_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Metal Mario walking along the bottom underwater. Losing both the metal and
 * mega caps drops back to the swim state. While mStateWork is 0 the player
 * walks and sinks -- gravity and terminal velocity depend on whether the water
 * surface (data_0209f32c) is more than 0x46000 above (0xd2000 when mega).
 * Landing plays a thud, spawns a splash particle, and either returns to the
 * ground state or starts the landing animation.
 */
extern "C" {
int Player::St_MetalWaterWater_Main()
{
    extern int func_ov002_020cec2c(void*);
    extern int func_ov002_020eeca8(void*, void*);
    extern int func_ov002_020bf224(void*, int a, int b);
    extern void ApproachAngle(void*, int target, int a, int b, int d);
    extern void PlayerNewSimpleHold7(u32 id, int x, int y, int z);
    extern void Player_AdvanceAnims(void*);
    extern struct S18b2 data_0209f4a0[];
    extern int data_0209f32c;
    extern Player::State data_ov002_0211067c;
    extern Player::State data_ov002_021106ac;
    /* volatile: the original pins these stores to memory in this order. */
    volatile Vector3 pos;
    u32 f;
    int v;
    int lim;
    int py;
    int px, py2, pz;

    if (func_ov002_020cec2c(this) != 0) return 1;
    f = mIsMetal;
    if (f == 0 && mIsMega == 0) {
        this->ChangeState(data_ov002_0211067c);
        return 1;
    }
    if (f != 0) func_ov002_020eeca8(&mMeshClsn, this);

    if (mStateWork == 0) {
        v = 0;
        if (data_0209f4a0[gActivePlayerSlot].a != 0) {
            v = func_ov002_020bf224(this, 0x14000, 0x1000);
            ApproachAngle(&mPrevAngleY, mDesiredAngleY, 0x20, 0x1000, 0x10);
        }
        mAngleY = mPrevAngleY;
        ApproachLinear(mHorzSpeed, v, 0x400);
        py = mPosY;
    lim = py + 0x46000;
    if (mIsMega != 0) lim = py + 0xd2000;
        if (data_0209f32c > lim) {
            mVertAccel = -0x1800;
            mTerminalVelocity = -0x14000;
        } else {
            mVertAccel = -0x4000;
            mTerminalVelocity = -0x4b000;
        }
        if (mIsAirborne == 0) {
            Sound::PlayBank0(0xa9, mCamSpacePos);
            px = mPosX;
            pos.x = px;
            py2 = mPosY;
            pos.y = py2;
            pz = mPosZ;
            pos.z = pz;
            if (mIsMega != 0) {
                py2 += 0x19000;
                pos.y = py2;
                Particle::System::NewSimple(0x5c, px, py2, pz);
            } else {
                py2 += 0x32000;
                pos.y = py2;
                Particle::System::NewSimple(0xd7, px, py2, pz);
            }
            if (data_0209f4a0[gActivePlayerSlot].a != 0) {
                this->ChangeState(data_ov002_021106ac);
            } else {
                mHorzSpeed = 0;
                if (mStateStep != 0)
                    this->SetAnim(0x52, 0x40000000, 0x1000, 0);
                else
                    this->SetAnim(0x55, 0x40000000, 0x1000, 0);
                mStateWork = 1;
            }
        }
    } else {
        if (FinishedAnim() != 0) {
            this->ChangeState(data_ov002_021106ac);
            return 1;
        }
    }
    Player_AdvanceAnims(this);
    return 1;
}
}

// @symbol _ZN6Player23St_MetalWaterWater_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_MetalWaterWater_Init()
{
  mIsInAirState=1;
  mIsUnderwater=1;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  if(mStateStep==0){
    func_ov002_020bf2d8(0x20000);
    this->SetAnim(0x53, 0x40000000, 0x1000, 0);
  }else{
      this->SetAnim(0x54, 0x40000000, 0x1000, 0);
  }
  Sound::PlayBank0(0xa8, mCamSpacePos);
  mStateWork=0;
  return 1;
}
}

// @symbol _ZN6Player24St_MetalWaterGround_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_MetalWaterGround_Main()
{
    extern int func_ov002_020cec2c(void*);
    extern void ApproachAngle(void*, short, int, int, int);
    extern int func_ov002_020bf224(void*, int, int);
    extern void func_0201251c(int, int, void*, int);
    extern void Player_AdvanceAnims(void*);
    extern int data_0209f32c;
    extern unsigned char data_0209f49e;
    extern short data_0209f4a0;
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_0211067c;
    extern Player::State data_ov002_021106c4;
    extern void func_ov002_020cd190(void*);
    if(func_ov002_020cec2c(((char*)this))) return 1;
    int u703 = mIsMega;
    if((int)(data_0209f32c - 0x50000) < (int)(mPosY - 0xa000)){
        mIsUnderwater = 0;
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }
    if(*(unsigned short*)((char*)&data_0209f49e + gActivePlayerSlot * 0x18) & 2){
        mStateStep = 0;
        this->ChangeState(data_ov002_021106c4);
        return 1;
    }
    if(mIsAirborne != 0 && (mPosY - mGroundY) >= 0x32000){
        mStateStep = 1;
        this->ChangeState(data_ov002_021106c4);
        return 1;
    }
    if(mIsMetal == 0 && u703 == 0){
        this->ChangeState(data_ov002_0211067c);
        return 1;
    }
    func_ov002_020cd190(((char*)this));
    int approach = 0;
    if(*(short*)((char*)&data_0209f4a0 + gActivePlayerSlot * 0x18) != 0){
        ApproachAngle((short*)((char*)&mPrevAngleY), mDesiredAngleY, 8, 0x1000, 0x80);
        approach = func_ov002_020bf224(((char*)this), 0x14000, 0x1000);
    }
    ApproachLinear(*((int *)((char *)&mHorzSpeed)), approach, 0x1000);
    mAngleY = mPrevAngleY;
    if(mIsMega != 0){
        if(mHorzSpeed == 0){
            this->SetAnim(0xa0, 0, 0x1000, 0);
        }else if(this->IsAnim(0xa0) != 0){
            this->SetAnim(0x9f, 0x40000000, 0x1000, 0);
        }else if(this->IsAnim(0x9f) != 0){
            if(this->FinishedAnim() != 0){
                this->SetAnim(0x9e, 0,
                              *(short *)((char *)&data_0209f4a0 + gActivePlayerSlot * 0x18) * 2, 0);
            }
        }else if(this->IsAnim(0x9f) == 0){
            int v = *(short*)((char*)&data_0209f4a0 + gActivePlayerSlot * 0x18);
            if(v != 0){
                this->SetAnim(0x9e, 0, v * 2, 0);
                func_ov002_020bf5e0();
            }else{
                this->SetAnim(0x9d, 0x40000000, 0x1000, 0);
            }
        }
    }else if(mHorzSpeed == 0){
        this->SetAnim(0x47, 0, 0x1000, 0);
    }else{
        this->SetAnim(0x48, 0, 0x1000, 0);
        int r5 = (int)((((long long)mHorzSpeed << 9) + 0x800) >> 12);
        if(r5 < 0x400) r5 = 0x400;
        dExtFrameCtrl_c &anim =
            *mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)];
        anim.speed = r5;
        if (mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)]->WillHitFrame(4) !=
                0 ||
            mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)]->WillHitFrame(
                0x13) != 0) {
            func_0201251c(0, 0xaa, (void*)((char*)&mCamSpacePosX), mHorzSpeed);
        }
    }
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player24St_MetalWaterGround_InitEv
/* recovered: named members + shared header, real C++ method */
int Player::St_MetalWaterGround_Init()
{
    dActor_c *p = *(dActor_c **)((char *)&mHeldObj);
    int b0 = (p != 0);
    if (b0) {
        int h = p->actorID;
        int b1 = (h == 0x10c);
        if (!b1) {
            int b2 = (h == 0x10b);
            if (!b2) goto tail;
        }
        p->mFlags &= ~0x4000;
        ((dActor_c *)mHeldObj)->mFlags &= ~0x100;
        *(int**)((char*)&mHeldObj) = 0;
    }
tail:
    Sound::PlayCharVoice(mCharacter, 0x2e, mCamSpacePos);
    mVertAccel = -0x1800;
    mTerminalVelocity = -0x14000;
    if (mHorzSpeed >= 0x14000) mHorzSpeed = 0x14000;
    mIsUnderwater = 1;
    return 1;
}

// @symbol func_ov002_020cd190
extern "C" {
void func_ov002_020cd190(char* c){
    Player *self = (Player *)c;
    extern void func_ov002_020ce8bc(void*, int x);
    extern int data_0209f32c;
    extern void func_ov002_020cd190(void*);
  if(self->mIsMega==0) return;
  if(self->mPosY + 0x180000 <= data_0209f32c) return;
  func_ov002_020ce8bc(c, self->mHorzSpeed);
}
}

// @symbol _ZN6Player15St_Swim_CleanupEv
/* recovered: named members + shared header, real C++ method */
int Player::St_Swim_Cleanup()
{
  if(mSwimMusicPushed!=0){
    mSwimMusicPushed=0;
    EndStateMusic(0x33);
  }
  return 1;
}

// @symbol func_ov002_020cd218
extern "C" {
void func_ov002_020cd218(char* c, int v, int* out1, int* out2){
    extern short data_02082214[];
    extern void func_ov002_020cd218(void*, int, void*, void*);
    Player *player = (Player *)c;
    *out1 = FX_MUL(v, data_02082214[((unsigned short)player->mPrevAngleX >> 4) * 2 + 1]);
    *out2 = -FX_MUL(v, data_02082214[((unsigned short)player->mPrevAngleX >> 4) * 2]);
}
}

// @symbol func_ov002_020cd28c
extern "C" {
int func_ov002_020cd28c(Player *self) {
    self->SetAnim(0xaf, 0x40000000, 0x1000, 0);
    self->mStateStep = 7;
    return 7;
}
}

// @symbol func_ov002_020cd2c4
extern "C" {
void func_ov002_020cd2c4(Player *self) {
    extern void func_ov002_020cd2c4(void*);
    self->SetAnim(0xab, 0x40000000, 0x1000, 0);
    self->mStateStep = 1;
    self->mStateTimer = 0;
}
}

// @symbol func_ov002_020cd308
extern "C" {
void func_ov002_020cd308(Player *self) {
    extern void func_ov002_020cd308(void*);
    self->SetAnim(0xae, 0x40000000, 0x1000, 0);
    self->mStateStep = 6;
    self->mStateTimer = 0x10;
    self->mStateWork &= 0xa;
}
}

// @symbol func_ov002_020cd364
extern "C" {
void func_ov002_020cd364(char *c) {
    Player *self = (Player *)c;
    extern void func_ov002_020dc020(void*);
    extern void func_ov002_020cd364(void*);
    func_ov002_020dc020(c);
    self->mAttackClsn.Clear();
    self->mAttackClsn.Update();
    self->mStateTimer = 2;
    self->mStateStep = 5;
}
}

// @symbol func_ov002_020cd39c
extern "C" {
void func_ov002_020cd39c(Player *self) {
    extern void func_ov002_020cd39c(void*);
    self->SetAnim(0xaa, 0x40000000, 0x1000, 0);
    self->mStateStep = 4;
    self->mStateTimer = 7;
}
}

// @symbol func_ov002_020cd3e0
extern "C" {
void func_ov002_020cd3e0(Player *self) {
    extern void func_ov002_020cd3e0(void *);
    self->SetAnim(0xa9, 0x40000000, 0x1000, 0);
    self->mStateStep = 2;
    self->mStateTimer = 0xf;
    self->mStateWork &= 0xfe;
    Sound::PlayBank0(0x14, self->mCamSpacePos);
}
}

// @symbol func_ov002_020cd448
extern "C" {
void func_ov002_020cd448(char* self) {
    Player *player = (Player *)self;
    extern int func_ov002_020c18b0(void*, int x);
    extern short data_0209f4a4;
    extern void func_ov002_020cd448(void*);
    unsigned char fl;
    if (player->mStateStep < 2) goto tail;
    fl = player->mClsnFlags;
    if (fl & 1) {
        short before;
        player->mAngleX = player->mPrevAngleX;
        before = player->mAngleX;
        func_ov002_020c18b0(self, 0);
        if (before < player->mAngleX) goto tail;
        ApproachLinear2(player->mPrevAngleX, player->mAngleX, 0x200);
        goto tail;
    }
    if (fl & 2) {
        if (*(short*)((char*)&data_0209f4a4 + gActivePlayerSlot * 0x18) != 0) goto tail;
        if (player->mPrevAngleX > 0) {
            short* q = &player->mPrevAngleX;
            *q += 0x200;
            if (player->mPrevAngleX > 0x3f00) player->mPrevAngleX = 0x3f00;
        } else {
            short* q = &player->mPrevAngleX;
            *q -= 0x200;
            if (player->mPrevAngleX < -0x3f00) player->mPrevAngleX = -0x3f00;
        }
    }
tail:
    ApproachLinear2(player->mAngleXSpeed, player->mAngleZ, 0x200);
    player->mAngleZ = player->mAngleXSpeed;
}
}

// @symbol func_ov002_020cd550
/* the += / -= on 0x92 go through a materialized base (add r1,c,#0x92;
   ldrsh/strh [r1]); the (long long)-mask launder stops mwcc folding it. */
#define LS16(p) (*(short*)(int)(p))
extern "C" {
void func_ov002_020cd550(char* c)
{
    Player *self = (Player *)c;
    extern struct S18a data_0209f4a4[];
    extern short data_02082214[];
    extern void func_ov002_020cd448(void*);
    extern void func_ov002_020cd550(void*);
    short lim, v, w, a;
    int step, idx;

    lim = (short)FX_MUL(data_0209f4a4[gActivePlayerSlot].f0, -0x3f00);
    lim &= 0xfff0;

    step = self->mPrevAngleX > 0 ? 0x100 : 0x200;
    if (self->mPrevAngleX < lim) {
        LS16(&self->mPrevAngle) += step;
        if (self->mPrevAngleX > lim) self->mPrevAngleX = lim;
    } else if (self->mPrevAngleX > lim) {
        LS16(&self->mPrevAngle) -= step;
        if (self->mPrevAngleX < lim) self->mPrevAngleX = lim;
    }

    func_ov002_020cd448(c);

    v = self->mPrevAngleX;
    if (v < 0) self->mAngleX = v * 6 / 10;
    else self->mAngleX = v * 10 / 8;

    self->mModelOffsetY = 0;
    w = self->mAngleX;
    if (w >= 0)
        self->mModelOffsetY = FX_MUL(w, 0xc000);

    a = (short)(self->mAngleX - 0x2000);
    if (a < -0x2000) a = -0x2000;
    if (a > 0) a = 0;
    idx = (u16)(short)(a * 2) >> 4;
    self->mModelOffsetZ = data_02082214[idx * 2 + 1] * -0x30;
}
}
#undef LS16

// @symbol func_ov002_020cd71c
extern "C" {
void func_ov002_020cd71c(char *self)
{
    Player *player = (Player *)self;
    typedef short s16;
    extern int data_ov002_020ff1f0[];
    extern short data_0209f4a2[];
    extern void func_ov002_020cd71c(void*);
    int ip = data_ov002_020ff1f0[player->param1];
    int a = FX_MUL(ip, 0x320);
    int c = (int)((((long long)ip << 7) + 0x800) >> 12);
    s16 step = (s16)((((long long)ip << 5) + 0x800) >> 12);
    int b = FX_MUL(ip, 0x240);
    int e = *(short *)((char *)data_0209f4a2 + gActivePlayerSlot * 0x18);
    s16 target = (s16)FX_MUL(-b, e);

    if (player->mPrevVertSpeed == 0) {
        step = (s16)((((long long)ip << 6) + 0x800) >> 12);
        target = (s16)FX_MUL(-a, e);
    }

    if (target > 0) {
        if (player->mAngleYSpeed < 0) {
            s16 *q = &player->mAngleYSpeed;
            *q = *q + 0x40;
            if (player->mAngleYSpeed > c)
                player->mAngleYSpeed = c;
        } else {
            ApproachLinear2(player->mAngleYSpeed, target, step);
        }
    } else if (target < 0) {
        if (player->mAngleYSpeed > 0) {
            s16 *q = &player->mAngleYSpeed;
            *q = *q - 0x40;
            if (player->mAngleYSpeed < -c)
                player->mAngleYSpeed = -c;
        } else {
            ApproachLinear2(player->mAngleYSpeed, target, step);
        }
    } else {
        ApproachLinear2(player->mAngleYSpeed, 0, 0x40);
    }

    {
        s16 *q = &player->mAngleY;
        *q = *q + player->mAngleYSpeed;
    }
    player->mAngleZ = -(player->mAngleYSpeed << 3);
    player->mPrevAngleY = player->mAngleY;
}
}

// @symbol _ZN6Player12St_Swim_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Swimming, per frame. Metal cap diverts to the metal-underwater state and
 * surfacing to the walking one. mStateStep selects the stroke phase; each
 * phase drives the stroke animation, the body model's animation block at
 * +0x50, and the forward impulse, with a kick sound on the frame the
 * animation crosses its trigger.
 */
extern "C" {
int Player::St_Swim_Main()
{
    extern int func_ov002_020cede0(void*);
    extern int func_ov002_020cec2c(void*);
    extern void func_ov002_020cd71c(void*);
    extern void func_ov002_020cd550(void*);
    extern unsigned int func_02022d44(unsigned int, unsigned int, int, int, int, void*);
    extern void* func_02037dc4(void*);
    extern int atan2_hold_Player_12(int, int);
    extern void Vec3_RotateYAndTranslate(void*, void*, short, void*);
    extern int func_ov002_020ce324(void*);
    extern void func_ov002_020cd364(void*);
    extern void func_ov002_020cd3e0(void*);
    extern int func_ov002_020cedb0(void*, int);
    extern void func_ov002_020cd39c(void*);
    extern void func_ov002_020cd2c4(void*);
    extern void func_ov002_020cd308(void*);
    extern void func_ov002_020dbf4c(void*);
    extern void Player_ReleaseHeldActor(void*);
    extern void func_ov002_020cd218(void*, int, void*, void*);
    extern int func_ov002_020ceaf4(void*);
    extern int func_ov002_020ce5f8(void*);
    extern void Player_AdvanceAnims(void*);
    extern Player::State data_ov002_021106ac;
    extern Player::State data_ov002_021101b4;
    extern unsigned short data_0209f49c;
    extern unsigned short data_0209f49e;
    extern char data_0209ee90[];
    int t4;
    volatile int pos[3];
    int* fr;
    int ang;
    int out[3];
    int vec[3];
    short yaw;
    int var_r2;
    unsigned short a4;
    int var_r6;
    int var_r1;
    int has;
    int *p;
    unsigned int t5;

    if (mIsMetal != 0) {
        this->ChangeState(data_ov002_021106ac);
        return 1;
    }
    if (this->GetHealth() == 0) {
        func_ov002_020c5dec(8);
        return 1;
    }
    if (func_ov002_020cede0(this) == 0) {
        mIsUnderwater = 0;
        this->ChangeState(data_ov002_021101b4);
        return 1;
    }
    mPeakY = mPosY;
    if (func_ov002_020cec2c(this) != 0) return 1;

    t4 = (*(unsigned short*)((char*)&data_0209f49c + gActivePlayerSlot * 0x18)) & 0x802;
    mAngleXSpeed = mAngleZ;
    func_ov002_020cd71c(this);
    func_ov002_020cd550(this);

    if (mJumpComboTimer != 0) {
        int eid = 0xcd;
        int y, z;
        pos[0] = mPosX;
        y = mPosY;
        pos[1] = y;
        z = mPosZ;
        pos[2] = z;
        y = y + 0x32000;
        pos[1] = y;
        *(unsigned int*)&mParticle3 = func_02022d44(*(volatile unsigned int*)(&mParticle3), eid, pos[0], pos[1], z, 0);
    }

    if (mPrevVertSpeed == 0 && mMeshClsn.IsOnGround() != 0) {
        fr = (int *)func_02037dc4(&mMeshClsn.GetFloorResult()->surface);
        ang = fr[1];
        if (ang < 0x720 && ang > 0x400 && mPosY >= mWaterSurfaceY - 0xc8000) {
            int at;
            mPosY = mPreClsnPosY - 0x8000;
            at = cstd::atan2(fr[0], fr[2]);
            vec[0] = 0;
            vec[1] = 0;
            at = at + 0x8000;
            yaw = (short)at;
            vec[2] = (*(int*)((char*)data_0209ee90 + 0x244)) << 0xc;
            Vec3_RotateYAndTranslate(out, &mPosX, yaw, vec);
            mPosX = out[0];
            mPosY = out[1];
            mPosZ = out[2];
        }
    }

    switch (mStateStep) {
    case 0:
        mPrevAngleX = -0x4000;
        mPrevAngleZ = 0;
        mAngleX = 0;
        mAngleZ = 0;
        mAngleYSpeed = 0;
        ApproachLinear(*((int *)(&mPrevVertSpeed)), 0, 0x5000);
        if (this->IsAnim(0xa8) == 0 && this->FinishedAnim() != 0) {
            this->SetAnim(0xa8, 0, 0x1000, 0);
        }
        if (mPrevVertSpeed == 0) {
            mStateStep = 1;
            mPrevAngleX = 0;
            mStateTimer = 0x1e;
        }
        break;
    case 1:
        var_r2 = 0x800;
        if (mStateTimer != 0) var_r2 = 0x200;
        ApproachLinear(*((int *)(&mPrevVertSpeed)), 0, var_r2);
        if (this->IsAnim(0xa8) == 0) {
            if (this->FinishedAnim() != 0) {
                this->SetAnim(0xa8, 0, 0x1000, 0);
            }
        } else if (mAngleX >= 0x1000) {
            dExtFrameCtrl_c &anim =
                *mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)];
            if (anim.speed != 0x3000) {
                dExtFrameCtrl_c &again =
                    *mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)];
                again.speed = 0x3000;
            }
        } else if (mAngleYSpeed != 0) {
            dExtFrameCtrl_c &anim =
                *mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)];
            if (anim.speed != 0x2000) {
                dExtFrameCtrl_c &again =
                    *mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)];
                again.speed = 0x2000;
            }
        } else {
            dExtFrameCtrl_c &anim =
                *mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)];
            if (anim.speed != 0x1000) {
                dExtFrameCtrl_c &again =
                    *mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)];
                again.speed = 0x1000;
            }
        }
        if (func_ov002_020ce324(this) != 0) return 1;
        if ((*(unsigned short*)((char*)&data_0209f49e + gActivePlayerSlot * 0x18)) & 1) {
            func_ov002_020cd364(this);
        } else if (t4 != 0) {
            func_ov002_020cd3e0(this);
        }
        break;
    case 2:
        a4 = mStateTimer;
        if (a4 >= 0xa) var_r6 = 0x800;
        else if (a4 <= 4) var_r6 = 0x1800;
        else var_r6 = 0;
        ApproachLinear(*((int *)(&mPrevVertSpeed)), func_ov002_020cedb0(this, 0x10000), var_r6);
        if (mStateTimer < 0xa && t4 == 0) {
            mStateWork |= 1;
        }
        if (mStateTimer == 0 && this->IsAnim(0xa9) != 0 && this->FinishedAnim() != 0) {
            if (!(mStateWork & 1)) {
                mStateStep = 3;
            } else {
                func_ov002_020cd39c(this);
            }
        }
        break;
    case 3:
        ApproachLinear(*((int *)(&mPrevVertSpeed)), func_ov002_020cedb0(this, 0xc000), 0x200);
        if (func_ov002_020ce324(this) != 0)
            return 1;
        if (t4 != 0) {
            this->SetAnim(0xa6, 0, 0x1000, 0);
            if (mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)]->WillHitFrame(
                    0) != 0) {
                Sound::PlayBank0(0x15, mCamSpacePos);
            }
        } else {
            func_ov002_020cd39c(this);
        }
        break;
    case 4:
        ApproachLinear(*((int *)(&mPrevVertSpeed)), 0, 0x400);
        if (func_ov002_020ce324(this) != 0)
            return 1;
        if (mStateTimer == 0 && t4 != 0) {
            func_ov002_020cd3e0(this);
        }
        if (this->FinishedAnim() != 0 && this->IsAnim(0xaa) != 0) {
            func_ov002_020cd2c4(this);
        }
        break;
    case 5:
        if (mStateTimer == 0) {
            func_ov002_020cd308(this);
        }
        break;
    case 6:
        func_ov002_020dbf4c(this);
        var_r1 = func_ov002_020cedb0(this, 0x10000);
        a4 = mStateTimer;
        if (a4 >= 5) var_r2 = 0x1000;
        else if (a4 == 0) { var_r2 = 0x800; var_r1 = 0; }
        else var_r2 = 0;
        ApproachLinear(*((int *)(&mPrevVertSpeed)), var_r1, var_r2);
        if (this->FinishedAnim() != 0) {
            has = (mHeldObj != 0) ? 1 : 0;
            if (has != 0) {
                mStateStep = 8;
                mStateTimer = 0xf0;
                p = (int *)&((dActor_c *)mHeldObj)->mFlags;
                *p |= 0x4000;
                this->SetAnim(0xb0, 0, 0x1000, 0);
            } else {
                func_ov002_020cd2c4(this);
            }
        }
        mAttackClsn.Clear();
        {
            dExtFrameCtrl_c &anim =
                *mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)];
            t5 = ((unsigned int)anim.currFrame << 4) >> 0x10;
        }
        if ((int)t5 >= 5 && (int)t5 <= 7) {
            mAttackClsn.Update();
        }
        break;
    case 7:
        if (this->FinishedAnim() != 0) {
            mStateStep = 8;
            mStateTimer = 0xf0;
            this->SetAnim(0xb0, 0, 0x1000, 0);
            p = (int *)&((dActor_c *)mHeldObj)->mFlags;
            *p |= 0x4000;
            StartStateMusic(0x33);
            mSwimMusicPushed = 1;
        }
        break;
    case 8:
        ApproachLinear(*((int *)(&mPrevVertSpeed)), func_ov002_020cedb0(this, 0x1e000), 0x400);
        if ((*(unsigned short *)((char *)&data_0209f49e + gActivePlayerSlot * 0x18)) & 1) {
            this->SetAnim(0xb1, 0x40000000, 0x1000, 0);
            mStateStep = 9;
            EndStateMusic(0x33);
        }
        if (mStateTimer != 0) {
            has = (mHeldObj != 0) ? 1 : 0;
            if (has == 0) goto case8_block107;
        } else {
        case8_block107:
            Player_ReleaseHeldActor(this);
            func_ov002_020cd2c4(this);
            EndStateMusic(0x33);
            mSwimMusicPushed = 0;
        }
        if (mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)]->WillHitFrame(0) !=
            0) {
            Sound::PlayBank0(0x15, mCamSpacePos);
        }
        break;
    case 9:
        if (mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)]->WillHitFrame(
                0xd) != 0) {
            func_ov002_020daa74();
        }
        if (this->FinishedAnim() != 0) {
            func_ov002_020cd2c4(this);
        }
        break;
    }

    func_ov002_020cd218(this, mPrevVertSpeed, &mHorzSpeed, &mVertSpeed);
    {
        int* pa8 = (int*)((unsigned int)(&mVertSpeed));
        *pa8 = *pa8 + func_ov002_020ceaf4(this);
    }
    if (func_ov002_020ce5f8(this) != 0) return 1;
    Player_AdvanceAnims(this);
    return 1;
}
}

// @symbol func_ov002_020ce324
extern "C" {
int func_ov002_020ce324(char* c)
{
    Player *player = (Player *)c;
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef struct dActor_c dActor_c;
    extern struct S18b data_0209f49e[];
    extern struct S18a data_0209f4a4[];
    extern short data_02082214[];
    extern Player::State data_ov002_02110694;
    extern int func_ov002_020ceb54(void*);
    extern int atan2_hold_Player_11(int a, int b);
    extern int AngleDiff(int a, int b);
    extern int func_ov002_020ce324(void*);
    Vector3 a1, a2, b1, b2, nrm;
    int ok;

    if (!(data_0209f49e[gActivePlayerSlot].f0 & 2)) return 0;
    if (func_ov002_020ceb54(c)) return 0;

    dBgCh_Lin rl1;
    a1.x = player->mPosX;
    a1.y = player->mPosY;
    a1.z = player->mPosZ;
    a2.x = player->mPosX;
    a2.y = player->mPosY;
    a2.z = player->mPosZ;
    a1.y += 0x50000;
    a2.y += 0x96000;
    rl1.SetObjAndLine(a1, a2, (dActor_c *)c);
    if (rl1.DetectClsn()) {
        return 0;
    }

    ok = 0;
    if (data_0209f4a4[gActivePlayerSlot].f0 >= 0xa00) {
        ok = 1;
    } else {
        int ax, bz, bx, az, by, ay;
        u16 ang = player->mAngleY;

        dBgCh_Lin rl2;
        ax = player->mPosX;
        az = player->mPosZ;
        ay = player->mPosY + 0x32000;
        b1.x = ax;
        b1.y = ay;
        b1.z = az;
        bz = player->mPosZ + data_02082214[(ang >> 4) * 2 + 1] * 0x64;
        bx = player->mPosX + data_02082214[(ang >> 4) * 2] * 0x64;
        by = player->mPosY + 0x32000;
        b2.x = bx;
        b2.y = by;
        b2.z = bz;
        rl2.SetObjAndLine(b1, b2, (dActor_c *)c);
        if (rl2.DetectClsn()) {
            rl2.surface.CopyNormalTo(nrm);
            if (nrm.y == 0) {
                if (AngleDiff(cstd::atan2(nrm.x, nrm.z), player->mAngleY) >= 0x5000)
                    ok = 1;
            }
        }
    }

    if (ok == 1) {
        player->mIsUnderwater = 0;
        player->ChangeState(data_ov002_02110694);
        return 1;
    }
    return 0;
}
}

// @symbol _ZN6Player12St_Swim_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Swim_Init()
{
    extern void Player_ReleaseHeldActor(void*);
    extern int data_0209f318[];
    extern void func_0200d768(void*, unsigned char);
  Player_ReleaseHeldActor(((char*)this));
  Sound::PlayCharVoice(mCharacter, 0x2e, mCamSpacePos);
  this->SetAnim(0xa7, 0x40000000, 0x1000, 0);
  mStateStep=0;
  mVertAccel=0;
  mTerminalVelocity=-0xc000;
  mIsUnderwater=1;
  func_0200d768(*(void**)data_0209f318,mPlayerNo);
  mPrevVertSpeed=mVertSpeed;
  mHorzSpeed=0;
  mVertSpeed=0;
  mAngleYSpeed=0;
  mStateWork=0;
  mStateArg=0;
  mJumpComboTimer=0;
  return 1;
}
}

// @symbol func_ov002_020ce5f8
/* recovered: shared common types */
extern "C" {
int func_ov002_020ce5f8(char* c) {
    Player *self = (Player *)c;
    extern int func_ov002_020ceb54(void*);
    extern void func_ov002_020ce8bc(void*, int arg);
    extern void func_ov002_020ceb7c(void*);
    extern void MulVec3Mat4x3(void*, void*, void*);
    extern void* Vec3_LslInPlace(void*, int sh);
    extern u32 func_020229f0(int x, int y, int z);
    extern int data_0209f32c;
    extern Player::State data_ov002_0211013c;
    extern int func_ov002_020ce5f8(void*);
    int y, threshold;
    Vector3 pos;

    threshold = data_0209f32c - 0x50000;
    y = self->mPosY;
    if (y >= threshold) {
        if ((self->mClsnFlags & 1) == 0) {
            if (self->mVertSpeed >= 0) {
                self->mPosY = threshold;
                self->mVertSpeed = 0;
            }
        } else {
            if (threshold < y - 0xa000) {
                self->mIsUnderwater = 0;
                self->ChangeState(data_ov002_0211013c);
                return 1;
            }
        }
    }

    if (func_ov002_020ceb54(c) == 0) {
        if (self->mStateArg == 0) {
            self->mStateArg |= 1;
            Sound::PlayBank0(0x17, self->mCamSpacePos);
            self->mParticle1 = 0;
        }
        func_ov002_020ce8bc(c, self->mPrevVertSpeed);
    } else {
        self->mStateArg = 0;
    }
    func_ov002_020ceb7c(c);

    {
        u32 idx = self->GetBodyModelID(self->param1 & 0xff, 0);
        dExtAnmModel_c *m1 = self->mModels.mBodyModels[idx];
        Matrix4x3 *base;
        u32 idx2;
        dExtAnmModel_c *m2;
        base = &m1->data.transforms[15];
        idx2 = self->GetBodyModelID(self->param1 & 0xff, 0);
        m2 = self->mModels.mBodyModels[idx2];
        MulVec3Mat4x3(&base->m[9], &m2->mat4x3, &pos);
    }
    Vec3_LslInPlace(&pos, 3);

    if (pos.y < data_0209f32c - 0x1e000) {
        func_020229f0(pos.x, *(volatile int*)&pos.y, pos.z);
        if (self->mStateWaitTimer == 0) {
            Sound::PlayBank3(0xb, self->mCamSpacePos);
            self->mStateWaitTimer = 0xa;
        }
    }
    return 0;
}
}

// @symbol func_ov002_020ce798
/* the -= 0x4000 goes through a materialized base (add r1,c,#0x60; ldr/str [r1]);
   the (long long)-mask launder is what stops mwcc folding it to [c,#0x60]. */
#define LDR_I(p) (*(int*)(int)(p))
extern "C" {
void func_ov002_020ce798(Player *player) {
    typedef struct dActor_c dActor_c;
    extern int data_0209f32c;
    extern Player::State data_ov002_0211067c;
    Vector3 v1, v2;

    if (!player->IsState(data_ov002_0211067c))
        return;

    if (player->mPosY >= data_0209f32c - 0x50000) {
        if (!player->mMeshClsn.IsOnGround())
            player->mPosY = data_0209f32c - 0x50000;
    }

    dBgCh_Lin rl;
    v1.x = player->mPosX;
    v1.y = player->mPosY;
    v1.z = player->mPosZ;
    v2.x = player->mPosX;
    v2.y = player->mPosY;
    v2.z = player->mPosZ;
    v1.y += 0x50000;
    v2.y += 0x96000;
    rl.SetObjAndLine(v1, v2, (dActor_c *)player);
    if (rl.DetectClsn()) {
        ::Vector3 cp = rl.GetClsnPos();
        if (player->mPosY >= cp.y - 0x82000) {
            LDR_I(&player->mPosY) -= 0x4000;
            if (player->mPosY < cp.y - 0x82000)
                player->mPosY = cp.y - 0x82000;
        }
    }
}
}
#undef LDR_I

// @symbol func_ov002_020ce8bc
extern "C" {
void func_ov002_020ce8bc(char* self, int arg1)
{
    extern u32 func_02022cbc(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir);
    extern char* data_0209f32c;
    Player *p = (Player *)self;
    int z = p->mPosZ;
    volatile int x = p->mPosX;
    volatile int y = (int)(data_0209f32c + 0x3000);
    volatile int zshadow = z;

    if (p->mIsMega != 0) {
        if (arg1 != 0) {
            p->mWaterMoveParticle =
                func_02022cbc(*(volatile int *)&p->mWaterMoveParticle, 0x65, x, y, z, 0);
        } else {
            p->mWaterStillParticle =
                func_02022cbc(*(volatile int *)&p->mWaterStillParticle, 0x66, x, y, z, 0);
        }
    } else {
        if (arg1 != 0) {
            p->mWaterMoveParticle =
                func_02022cbc(*(volatile int *)&p->mWaterMoveParticle, 0xe8, x, y, z, 0);
        } else {
            p->mWaterStillParticle =
                func_02022cbc(*(volatile int *)&p->mWaterStillParticle, 0xe9, x, y, z, 0);
        }
    }
}
}

// @symbol func_ov002_020ce9c8
extern "C" {
void func_ov002_020ce9c8(char* self)
{
    Player *player = (Player *)self;
    extern u32 func_02022c80(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir);
    extern u32 func_02022cbc(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir);
    extern u32 func_02022d00(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void*);
    extern char* data_0209f32c;
    int v[3];
    int effectID;

    *(volatile int*)&v[0] = player->mPosX;
    *(volatile int*)&v[1] = player->mPosY;
    *(volatile int*)&v[2] = player->mPosZ;
    v[1] = (int)(data_0209f32c + 0x4b000);
    effectID = 0xce;
    if (player->mIsMega)
        effectID = 0x54;
    func_02022c80(0, effectID, v[0], v[1], v[2], 0);

    v[1] = (int)(data_0209f32c + 0x3000);
    effectID = 0xcf;
    if (player->mIsMega)
        effectID = 0x55;
    func_02022cbc(0, effectID, v[0], v[1], v[2], 0);

    if (player->mIsMega)
        player->mParticle1 = func_02022d00(player->mParticle1, 0x56, player->mPosX, (int)data_0209f32c, player->mPosZ, 0);
    else
        Particle::System::NewBigSplash(player->mPosX, (int)data_0209f32c, player->mPosZ);

    if (player->mIsMega)
        Sound::PlayBank0(0xd4, player->mCamSpacePos);
    else
        Sound::PlayBank0(0x16, player->mCamSpacePos);
}
}

// @symbol func_ov002_020ceaf4
extern "C" {
int func_ov002_020ceaf4(char* c){
    extern int data_0209f32c;
    extern int data_ov002_020ff1e0[];
    Player *player = (Player *)c;
    int v;
    if (data_0209f32c - 0x50000 - player->mPosY < 0x1f4000)
        return 0x1800;
    v = data_ov002_020ff1e0[player->param1];
    return -(int)((((long long)v << 13) + 0x800) >> 12);
}
}

// @symbol func_ov002_020ceb54
extern "C" {
int func_ov002_020ceb54(char *p)
{
    extern int data_0209f32c;
    extern int func_ov002_020ceb54(void*);
    Player *player = (Player *)p;
    return data_0209f32c - 0x50000 >= player->mPosY + 0x3c000;
}
}

// @symbol func_ov002_020ceb7c
/* The 0x6e5 read-modify-write goes through a materialized base
   (ldr rN,[pc]; add rN,c,rN) because 0x6e5 is not an ARM rotated immediate;
   the (long long)-mask launder is what stops mwcc folding it to [c,#0x6e5]. */
#define LAUNDER_U8(p)      ((unsigned char *)(int)(p))
extern "C" {
void func_ov002_020ceb7c(struct CameraRaw *c)
{
    Player *self = (Player *)c;
    extern struct CameraRaw *data_0209f318;
    extern int data_0209f32c;
    extern void func_0200d768(void*, unsigned char playerID);
    extern void func_0200d72c(void*, unsigned char playerID);
    struct CameraRaw *g = data_0209f318;

    if (self->mStateWork & 2) {
        if (data_0209f32c - 0x50000 - self->mPosY >= 0x64000)
            return;
        func_0200d768(g, self->mPlayerNo);
        *LAUNDER_U8(&self->mStateWork) ^= 2;
    } else {
        if (data_0209f32c - 0x50000 - self->mPosY <= 0x12c000)
            return;
        func_0200d72c(g, self->mPlayerNo);
        *LAUNDER_U8(&self->mStateWork) |= 2;
    }
}
}
#undef LAUNDER_U8

// @symbol func_ov002_020cec2c
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* func_ov002_020cec2c at 0x020cec2c (ov002), size 0x184
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
extern "C" {
int func_ov002_020cec2c(Player *player) {
    extern void PlayerPlayLongHold_cec2c(unsigned int a, unsigned int b, unsigned int c,
                                         const Vector3 *v, unsigned int d);
    extern int func_ov002_020d91e0(void*, int damage, int doPre);
    extern void func_ov002_020da9d4(void*);
    extern s8 data_0209f2f8;
    extern int func_ov002_020ceb54(void*);
    int flag;
    int thresh;

    if (player->mIsNoControl != 0 || player->mIsMetal != 0 || player->mIsMega != 0 ||
        player->mIsTakingDamage != 0)
        return 0;

    flag = 0;
    if (data_0209f2f8 == 0x13 || data_0209f2f8 == 0x14 || data_0209f2f8 == 0x30)
        flag = 1;

    if (func_ov002_020ceb54(player) == 0 && flag == 0) {
        player->mAirDrainCounter = 0;
        if (player->mWalkTimer == 0) {
            player->Heal(0x100);
            player->mWalkTimer = 0xf;
        }
        return 0;
    }

    thresh = 0xf0;
    if (flag != 0) thresh = 0x50;

    if (player->GetHealth() <= 2) {
        player->mSurfaceSoundHandle = Sound::PlayLong((unsigned int)player->mSurfaceSoundHandle, 2,
                                                      0x27, *(Vector3 *)&player->mCamSpacePos, 0);
    }

    {
        u16 *p = &player->mAirDrainCounter;
        *p = *p + 1;
    }

    if (player->mAirDrainCounter < thresh)
        goto ret0;

    player->mAirDrainCounter = 0;
    if (func_ov002_020d91e0(player, 0x100, 0) == 0)
        goto ret0;

    func_ov002_020da9d4(player);
    player->func_ov002_020c5dec(7);
    return 1;

ret0:
    return 0;
}
}

// @symbol func_ov002_020cedb0
extern "C" {
Fix12i func_ov002_020cedb0(Player *c, Fix12i a) {
    extern unsigned short data_ov002_020ff158[];
    extern int func_ov002_020cedb0(void*, int);
    unsigned int t = data_ov002_020ff158[c->param1];
    return FX_MUL(a, (int)t);
}
}

// @symbol func_ov002_020cede0
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
int func_ov002_020cede0(void *thiz_) {
    extern int SurfaceInfo_TestFlag0x20(void *);
    extern int data_0209f32c;
    extern signed char data_0209f2f8;
    extern int func_ov002_020cede0(void *);
    extern int data_0209212c;
    char *thiz = (char *)thiz_;
    Player *p = (Player *)thiz;
    Vector3 v;
    int y = p->mPosY;
    int g = data_0209f32c;
    if (g < y - 0x64000)
        return 0;
    {
        int d = g - y;
        if (d < 0)
            d = -d;
        if (d >= 0xc8000)
            return 1;
    }
    dBgCh_Gnd rg;
    {
        int z = p->mPosZ;
        int yy = data_0209f32c + 0xc8000;
        int x = p->mPosX;
        v.x = x;
        v.y = yy;
        v.z = z;
    }
    rg.StartDetectingWater();
    rg.StopDetectingOrdinary();
    rg.SetObjAndPos(v, (dActor_c *)thiz);
    if (rg.DetectClsn() != 0 && SurfaceInfo_TestFlag0x20((int *)&rg.surface) != 0) {
        return 1;
    }
    v.y = data_0209212c;
    rg.StartDetectingWater();
    rg.StopDetectingOrdinary();
    rg.SetObjAndPos(v, (dActor_c *)thiz);
    if (rg.DetectClsn() != 0 && SurfaceInfo_TestFlag0x20((int *)&rg.surface) != 0) {
        p->mWaterSurfaceY = rg.clsnY;
        data_0209f32c = p->mWaterSurfaceY;
        return 1;
    }
    if (data_0209f2f8 == 0x21) {
        p->mWaterSurfaceY = 0x80000000;
        data_0209f32c = p->mWaterSurfaceY;
        return 0;
    }
    return 1;
}
}

// @symbol func_ov002_020cef84
extern "C" {
int func_ov002_020cef84(Player *p) {
    typedef struct {
        void *tag;
        int f04, f08, f0c, f10, f14;
        unsigned short f18, f1a;
        int f1c, f20, f24;
    } ClsnResultTmp;
    extern Player::State data_ov002_0211001c;
    extern int data_02099368;
    Vector3 v1, v2;
    int lim;

    dBgCh_Lin line;
    if (!p->IsInAir() || p->mIsUnderwater != 0 || p->mVertSpeed <= 0 ||
        p->IsState(data_ov002_0211001c) || p->mIsMega != 0 || p->mIsTakingDamage != 0 ||
        p->mIsNoControl != 0) {
        return 0;
    }

    v1.x = p->mPosX;
    int y1 = p->mPosY;
    v1.y = y1;
    v1.z = p->mPosZ;
    v2.x = p->mPosX;
    int y2 = p->mPosY;
    v2.y = y2;
    v2.z = p->mPosZ;
    v1.y = y1 + 0x64000;
    v2.y = y2 + 0x96000;
    lim = 0x8c000;
    {
        int t = p->mHeldObj != 0;
        if (t != false) {
            if (p->param1 == 2) {
                v2.y += 0x32000;
                lim = 0xb4000;
            }
        }
    }
    line.StopIgnoringVanishSurfaces();
    if (p->mIsVanish != 0)
        line.StartIgnoringVanishSurfaces();
    line.SetObjAndLine(v1, v2, (dActor_c *)p);
    if (line.DetectClsn()) {
        ::Vector3 cp = line.GetClsnPos();
        if (line.surface.clps.GetSurfaceType() == 3) {
            int t2 = p->mHeldObj != 0;
            if (t2 == false) {
                p->ChangeState(data_ov002_0211001c);
                p->mPosY = cp.y - 0x78000;
                return 1;
            }
        }
        {
            int d = cp.y - p->mPosY;
            if (d < 0) d = -d;
            if (d < lim) {
                void *a;
                ClsnResultTmp tmp;
                int *dst = &tmp.f04;
                int w0, w1;
                p->mVertSpeed = -0x1000;
                p->mClsnFlags |= 8;
                w0 = line.surface.clps.w0;
                w1 = line.surface.clps.w1;
                dst[0] = w1 ? w0 : w0;
                dst[1] = w1;
                dst[2] = line.surface.normal.x;
                dst[3] = line.surface.normal.y;
                dst[4] = line.surface.normal.z;
                tmp.tag = &data_02099368;
                tmp.f18 = line.triangleID;
                tmp.f1a = line.colliderIdx;
                tmp.f1c = line.clsnID;
                tmp.f20 = (int)line.owner;
                tmp.f24 = (int)line.collider;
                a = dActor_c::FindWithID(((dBgPi *)&tmp)->GetClsnID());
                if (a)
                    ((dActor_c *)a)->OnHitFromUnderneath(*(dActor_c *)p);
                ((dBgPi *)&tmp)->dBgPi::~dBgPi();
            }
        }
    }
    return 0;
}
}

// @symbol func_ov002_020cf20c
/* recovered: shared common types */
// func_ov002_020cf20c at 0x020cf20c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
int func_ov002_020cf20c(char* c) {
    Player *self = (Player *)c;
    extern void func_0200ca14(void*, unsigned char b, int c);
    extern void* data_0209f318[];
    extern int func_ov002_020cf20c(void*);
    Vector3 v1;
    Vector3 v2;
    dBgCh_Lin line;
    v1.x = self->mPosX;
    int y1 = self->mPosY;
    v1.y = y1;
    v1.z = self->mPosZ;
    v2.x = self->mPosX;
    int y2 = self->mPosY;
    v2.y = y2;
    v2.z = self->mPosZ;
    v1.y = y1 + 0x64000;
    v2.y = y2 + 0xb4000;
    line.SetObjAndLine(v1, v2, (dActor_c*)c);
    if (line.DetectClsn()) {
        if (line.surface.clps.GetSurfaceType() == 3) {
            ::Vector3 clsnPos = line.GetClsnPos();
            void* cam = data_0209f318[0];
            int t = line.surface.clps.func_02037e48();
            func_0200ca14(cam, self->mPlayerNo, t);
            int ret = clsnPos.y;
            return ret;
        }
    }
    return (int)0x80000000;
}
}

// @symbol func_ov002_020cf2f8
extern "C" {
void func_ov002_020cf2f8(void *c) {
    extern int ApproachAngle(void *, int, int, int, int);
    extern int func_ov002_020bf224(void *, int, int);
    extern short data_0209f4a0[];
    extern void func_ov002_020cf2f8(void *);
    if (*(short *)((char *)data_0209f4a0 + gActivePlayerSlot * 0x18) == 0)
        return;
    Player *p = (Player *)c;
    ApproachLinear(p->mHorzSpeed, func_ov002_020bf224(c, 0x8000, 0x2000), 0x1000);
    ApproachAngle(&p->mPrevAngleY, p->mDesiredAngleY, 0x10, 0x1000, 0x200);
    p->mAngleY = p->mPrevAngleY;
}
}

// @symbol func_ov002_020cf384
extern "C" {
void func_ov002_020cf384(char *p)
{
    extern void func_ov002_020cf384(void*);
    Player *player = (Player *)p;
    player->mVertAccel = 0;
    player->mVertSpeed = 0;
    player->mHorzSpeed = 0;
}
}

// @symbol _ZN6Player20St_CeilingGrate_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_CeilingGrate_Main()
{
    extern void Player_AdvanceAnims(void*);
    extern u16 data_0209f49c[];
    extern u16 data_0209f49e[];
    extern s16 data_0209f4a0[];
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_0211004c;
    extern Player::State data_ov002_021105a4;
    extern int func_ov002_020cf20c(void*);
    extern int func_ov002_020d674c(void*);
    extern u8 data_0209f4ab[];
    extern void func_ov002_020cf384(void*);
    extern u32 data_ov002_0210a60c[];
    extern void func_ov002_020cf2f8(void*);
    int r4;
    int spd;

    r4 = func_ov002_020cf20c(((char*)this));
    if (r4 == 0x80000000) {
        mPosX = mPreClsnPosX;
        mPosY = mPreClsnPosY;
        mPosZ = mPreClsnPosZ;
        r4 = mPosY + 0x90000;
    }

    {
        int idx = gActivePlayerSlot * 0x18;
        if ((*(u16*)((char*)data_0209f49c + idx) & 2) == 0) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
        if (*(u16*)((char*)data_0209f49e + idx) & 0x400) {
            if (func_ov002_020d674c(((char*)this)) != 0)
                this->ChangeState(data_ov002_0211004c);
            else
                this->ChangeState(data_ov002_021105a4);
            return 1;
        }
        {
            u8* p = data_0209f4ab + idx;
            if (*p != 0)
                *p = 5;
        }
    }

    switch (mStateStep) {
    case 0:
        func_ov002_020cf384(((char*)this));
        if (this->FinishedAnim() != 0) {
            mStateStep = 1;
            this->SetAnim(data_ov002_0210a60c[mStateWork & 1], 0, 0x1000, 0);
        }
        {
            dExtFrameCtrl_c &anim = *mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)];
            if ((u32)(anim.currFrame << 4) >> 0x10 < 0x21)
                break;
        }
    case 1:
        func_ov002_020cf384(((char*)this));
        if (*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) != 0) {
            mStateWork ^= 1;
            mStateStep = 2;
            spd = mHorzSpeed >> 3;
            if (spd < 0x800)
                spd = 0x800;
            this->SetAnim(data_ov002_0210a60c[(mStateWork & 1) + 2], 0x40000000, spd, 0);
        }
        break;
    case 2:
        func_ov002_020cf2f8(((char*)this));
        spd = mHorzSpeed >> 3;
        if (spd < 0x800)
            spd = 0x800;
        {
            dExtFrameCtrl_c &anim = *mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)];
            anim.speed = spd;
        }
        if (this->FinishedAnim() != 0) {
            Sound::PlayBank0(0xb2, mCamSpacePos);
            if (*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) != 0) {
                mStateWork ^= 1;
                this->SetAnim(data_ov002_0210a60c[(mStateWork & 1) + 2], 0x40000000, 0x1000, 0);
            } else {
                mStateStep = 1;
                this->SetAnim(data_ov002_0210a60c[mStateWork & 1], 0, 0x1000, 0);
            }
        }
        break;
    }

    mPosY = r4 - 0x90000;
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player20St_CeilingGrate_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_CeilingGrate_Init()
{
    extern void func_ov002_020cf384(void*);
  func_ov002_020cf384(((char*)this));
  mStateStep=0;
  mStateWork=0;
  this->SetAnim(0x5b, 0x40000000, 0x1000, 0);
  return 1;
}
}

// @symbol func_ov002_020cf700
extern "C" {
int func_ov002_020cf700(Player *player) {
    extern Player::State data_ov002_0211001c;
    extern int func_ov002_020cf700(void*);
    return player->IsState(data_ov002_0211001c);
}
}

// @symbol _ZN6Player14St_OnWall_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_OnWall_Main()
{
    extern void func_ov002_020cabe0(void*);
    extern int func_ov002_020c5244();
    extern int func_ov002_020d36d8(void*, int a);
    extern int atan2_hold_Player_10(Fix12i a, int b);
    extern int func_ov002_020bf224(void*, int a, int b);
    extern int func_ov002_020d4d88(void*, int a, int b);
    extern void Player_AdvanceAnims(void*);
    extern s16 data_0209f4a0[];
    extern Player::State data_ov002_0211013c;
    extern int AngleDiff(int, int);
    extern void func_ov002_020eee3c(void*, void*);
    u16 f = mStateFlags;
    u16 b0 = f & 1;
    if (b0 != 0) {
        func_ov002_020c0364(3);
        return 1;
    }
    u16 b1 = f & 4;
    if (b1 != 0) {
        func_ov002_020cabe0(((char*)this));
        return 1;
    }
    if (func_ov002_020c5244() != 0)
        return 1;
    if (func_ov002_020d36d8(((char*)this), 0) != 0) {
        mPrevAngleY = mAngleY;
        return 1;
    }
    if ((mClsnFlags & 2) == 0) {
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }
    if (mStateStep == 2) {
        if (*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) == 0) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
        int a = cstd::atan2(mWallNormalX, mWallNormalZ) + 0x8000;
        int d = AngleDiff((s16)a, mDesiredAngleY);
        if (d >= 0x4000) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
        if (d >= 0x1555) {
            mStateStep = 0;
            if ((s16)a == (s16)(mDesiredAngleY + d))
                mStateStep = 1;
            this->St_OnWall_Init();
            return 1;
        }
        ApproachLinear(*(short *)((char*)&mAngleY), (s16)a, 0x800);
        mPrevAngleY = a;
        mHorzSpeed = func_ov002_020bf224(((char*)this), 0xa000, 0x2000);
        func_ov002_020eee3c(&mMeshClsn, ((char *)this));
    } else {
        if (*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) != 0) {
            int ang = cstd::atan2(mWallNormalX, mWallNormalZ);
            int d = AngleDiff((s16)(ang + 0x8000), mDesiredAngleY);
            if (d >= 0x4000) {
                this->ChangeState(data_ov002_0211013c);
                return 1;
            }
            if (d < 0x1555) {
                mStateStep = 2;
                this->St_OnWall_Init();
                return 1;
            }
            int a2 = ang + 0x8000;
            ApproachLinear(*(short *)((char*)&mAngleY), (s16)a2, 0x800);
            u32 anim;
            if ((s16)a2 == (s16)(mDesiredAngleY + d)) {
                mPrevAngleY = ang + 0x4000;
                anim = 0x5d;
            } else {
                mPrevAngleY = ang - 0x4000;
                anim = 0x5c;
            }
            Fix12i spd = FX_MUL(mHorzSpeed, 0x600);
            if (spd < 0x400)
                spd = 0x400;
            this->SetAnim(anim, 0, spd, 0);
            func_ov002_020d4d88(((char*)this), func_ov002_020bf224(((char*)this), 0x6000, 0x1000), 0x1000);
        } else {
            mHorzSpeed = 0;
            this->SetAnim(0x5e, 0, 0x1000, 0);
        }
    }
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player14St_OnWall_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_OnWall_Init()
{
    extern int data_ov002_020ff164[];
  int r3=0x1000;
  if(param1==2){
    if(mStateStep==2) r3=0x2000;
  }
  this->SetAnim(data_ov002_020ff164[mStateStep], 0, r3, 0);
  mHorzSpeed=0;
  return 1;
}
}

// @symbol func_ov002_020cfaf0
extern "C" {
int func_ov002_020cfaf0(char* c) {
    extern short data_02082214[];
    extern int func_ov002_020cfaf0(void*);
    Player *p = (Player *)c;
    dBgCh_Lin line;
    Vector3 v1;
    Vector3 v2;
    {
        int ang = p->mAngleY;
        int j = 2 * (((unsigned short)(short)(ang + 0x8000)) >> 4);
        int s = data_02082214[j] * 32;
        int t = data_02082214[j + 1] * 32;
        int ax = p->mPosX + s;
        int az = p->mPosZ + t;
        int ay = p->mPosY - 0x20000;
        v1.x = ax;
        v1.y = ay;
        v1.z = az;
        int bx = p->mPosX + s;
        int bz = p->mPosZ + t;
        int by = p->mPosY + 0x20000;
        v2.x = bx;
        v2.y = by;
        v2.z = bz;
    }
    line.SetObjAndLine(v1, v2, (dActor_c*)c);
    if (line.DetectClsn()) {
        return 1;
    }
    return 0;
}
}

// @symbol func_ov002_020cfbdc
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
int func_ov002_020cfbdc(char *self)
{
    Player *player = (Player *)self;
    extern void func_ov002_020d0948(void*);
    extern short data_02082214[];
    extern Player::State data_ov002_021101b4;
    extern int func_ov002_020cfbdc(void*);
    Vector3 pts[3];
    int idx = (unsigned short)(short)(player->mAngleY + 0x8000) >> 4;
    int r6 = data_02082214[idx * 2];
    int r5 = data_02082214[idx * 2 + 1];

    if (player->mStateWaitTimer == 0) {
        if ((player->mClsnFlags & 2) != 0) {
            if (player->mMeshClsn.IsOnGround() != 0) {
                dBgPi *fr = player->mMeshClsn.GetFloorResult();
                if (fr->GetClsnID() != -1) {
                    if (dActor_c::FindWithID((unsigned int)fr->GetClsnID()) != 0) {
                        func_ov002_020d0948(self);
                        return 1;
                    }
                }
            }
        }
    }

    {
        int az = player->mPosZ + (r5 << 5);
        int ay = player->mPosY - 0xa000;
        int ax = player->mPosX + (r6 << 5);
        pts[1].x = ax;
        pts[1].y = ay;
        pts[1].z = az;
    }
    {
        int bx = player->mPosX + r6 * 0x60;
        int bz = player->mPosZ + r5 * 0x60;
        int by = player->mPosY - 0xa000;
        pts[2].x = bx;
        pts[2].y = by;
        pts[2].z = bz;
    }

    dBgCh_Lin line;
    line.SetObjAndLine(pts[1], pts[2], (dActor_c*)self);
    if (line.DetectClsn()) {
        player->mPosX += r5 * 0x60;
        player->mPosY -= 0x80000;
        player->mPosZ += r5 * 0x60;
        player->ChangeState(data_ov002_021101b4);
        return 1;
    }
    return 0;
}
}

// @symbol func_ov002_020cfd84
// func_ov002_020cfd84 at 0x020cfd84
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
int func_ov002_020cfd84(void* actor, void* a, void* b) {
    dBgCh_Lin line;
    dBgPi result;
    line.SetObjAndLine(*(Vector3*)a, *(Vector3*)b, (dActor_c*)actor);
    if (line.DetectClsn()) {
        result.surface = line.surface;
        result.triangleID = line.triangleID;
        result.colliderIdx = line.colliderIdx;
        result.clsnID = line.clsnID;
        result.owner = line.owner;
        result.collider = line.collider;
        if (result.GetClsnID() != -1) {
            void *act = dActor_c::FindWithID((unsigned int)result.GetClsnID());
            if (act != 0) {
                int t = (((dActor_c *)act)->actorID == 0x12a);
                if (t == 0) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
}

// @symbol func_ov002_020cfea4
/* recovered: Player ground/edge probe used while carrying or standing on
 * another actor.
 *
 * With mStateWaitTimer at zero and bit 1 of mClsnFlags set, the player first
 * asks its dBgCh_Actr (mMeshClsn) whether it stands on, or presses against, a
 * collider that belongs to another actor: standing on one that faces more
 * than 90 degrees away, or touching one as a wall, ends the probe with 1.
 * Otherwise a dBgCh_Gnd is built 32 units ahead of the player along its
 * facing angle (sin/cos from data_02082214, 1.19.12 units shifted up by 5)
 * and 0x14000 above it; ground within 0x50000 below the player's own height
 * ends the probe with 1. The remaining two checks hand func_ov002_020cfd84 a
 * pair of points 0xa000 below that probe: the point itself, and the point
 * moved 60 units to the player's right (angle + 0x4000), then 60 units to
 * its left. pts[] holds the eight Vector3 slots of the ROM's frame: pts[1]
 * is the ground probe, pts[2]/pts[3] the base pair, pts[4]/pts[5] and
 * pts[6]/pts[7] the copies handed to each call.
 *
 * Two spellings are load-bearing for the register colouring of the tail
 * (the 6bs/6ct receiver lever): the sideways offsets are kept in `dx`/`dz`,
 * the locals that already cross the first func_ov002_020cfd84 guard, as the
 * finished products tbl[t] * 60 (the ROM keeps exactly those products live
 * across the call); and the lowered y is computed before pts[1].x/.z are
 * reloaded, with the second lowered y computed after the two products. With
 * both in place 2004/b56 gives pts[1].x ip, pts[1].z r3, the lowered y lr,
 * the two sums r8/r7 and the second lowered y sb, which is the ROM's tail.
 */
extern "C" {
int func_ov002_020cfea4(char *self)
{
    Player *p = (Player *)self;
    extern int AngleDiff(int a, int b);
    extern int func_ov002_020cfd84(void *actor, const Vector3 *a, const Vector3 *b);
    extern short data_02082214[];

    Vector3 pts[8];
    dBgPi *fr;
    void *other;
    int idx, dx, dz;
    if (p->mStateWaitTimer == 0 && (p->mClsnFlags & 2) != 0) {
        if (p->mMeshClsn.IsOnGround() != 0) {
            fr = p->mMeshClsn.GetFloorResult();
            if (fr->GetClsnID() != -1) {
                other = dActor_c::FindWithID((unsigned int)fr->GetClsnID());
                if (other != 0) {
                    if (AngleDiff(p->mAngleY, ((dActor_c *)other)->mAngleY) >= 0x4000)
                        return 1;
                }
            }
        }
        if (p->mMeshClsn.IsOnWall() != 0) {
            fr = p->mMeshClsn.GetWallResult();
            if (fr->GetClsnID() != -1) {
                other = dActor_c::FindWithID((unsigned int)fr->GetClsnID());
                if (other != 0)
                    return 1;
            }
        }
    }
    dBgCh_Gnd rg;
    {
        int y;
        short *tbl;
        int ang = p->mAngleY;
        tbl = data_02082214;
        y = p->mPosY;
        idx = (unsigned short)(short)(ang + 0x8000) >> 4;
        dz = tbl[(idx << 1) + 1];
        dx = tbl[idx << 1];
        {
            int z = p->mPosZ;
            int x = p->mPosX;
            int pz = z + (dz << 5);
            int px = x + (dx << 5);
            int py = y + 0x14000;
            pts[1].x = px;
            pts[1].y = py;
            pts[1].z = pz;
        }
    }
    rg.SetObjAndPos(pts[1], (dActor_c *)self);
    if (rg.DetectClsn() != 0) {
        if (rg.clsnY >= p->mPosY - 0x50000) {
            return 1;
        }
    }
    {
        int y = p->mPosY;
        int yl = y - 0xa000;
        int px = pts[1].x;
        int pz = pts[1].z;
        pts[2].x = px;
        pts[2].y = yl;
        pts[2].z = pz;
        {
            short *tbl;
            int y2;
            int ang = p->mAngleY;
            y2 = p->mPosY;
            tbl = data_02082214;
            idx = (unsigned short)(short)(ang + 0x4000) >> 4;
            {
                int t = idx << 1;
                int yl2;
                dx = tbl[t] * 60;
                dz = tbl[t + 1] * 60;
                yl2 = y2 - 0xa000;
                pts[3].x = px + dx;
                pts[3].y = yl2;
                pts[3].z = pz + dz;
            }
        }
    }
    pts[4] = pts[2];
    pts[5] = pts[3];
    if (func_ov002_020cfd84(self, &pts[4], &pts[5]) != 0) {
        return 1;
    }
    pts[3].x = pts[1].x - dx;
    pts[3].z = pts[1].z - dz;
    pts[6] = pts[2];
    pts[7] = pts[3];
    if (func_ov002_020cfd84(self, &pts[6], &pts[7]) != 0) {
        return 1;
    }
    return 0;
}
}

// @symbol func_ov002_020d0178
extern "C" {
int func_ov002_020d0178(char* c, int arg1, int flag)
{
    extern int atan2_hold_Player_9(int x, int z);
    extern int AngleDiff(int a, int b);
    extern s16 data_02082214[];
    Player *p = (Player *)c;

    Vector3 normal;
    Vector3 pos1;
    Vector3 lineStart;
    dBgCh_Gnd rg;
    int ang;
    int gy;
    int res;

    if (flag != 0)
        ang = (s16)(p->mAngleY + 0x8000);
    else
        ang = p->mAngleY;
    {
        u8 b7 = p->mIsMega;
        int d = 0x40;
        int i;
        int s, csn;
        int xoff, zoff;
        int h;
        if (b7 != 0)
            d = 0xd0;
        i = (((u16)ang) >> 4) * 2;
        d <<= 12;
        s = data_02082214[i];
        csn = data_02082214[i + 1];
        xoff = FX_MUL(d, s);
        zoff = FX_MUL(d, csn);
        h = 0xc8;
        if (b7 != 0)
            h = 0x1f4;
        {
            int tz = p->mPosZ + zoff;
            int ty = p->mPosY + (h << 12);
            int tx = p->mPosX + xoff;
            pos1.x = tx;
            pos1.y = ty;
            pos1.z = tz;
        }
    }
    rg.SetObjAndPos(pos1, (dActor_c *)c);
    if (!rg.DetectClsn()) {
        return 0;
    }
    gy = rg.clsnY;
    if (flag != 0) {
        if (gy > p->mPosY + 0x64000 || gy < p->mPosY) {
            return 0;
        }
    } else {
        int lo = 0x64;
        if (p->mIsMega != 0)
            lo = 0x190;
        if (gy > p->mPosY + ((lo + 0x64) << 12) || gy < p->mPosY + (lo << 12)) {
            return 0;
        }
    }
    rg.surface.CopyNormalTo(normal);
    if (normal.y < 0xf04) {
        return 0;
    }
    dBgCh_Lin rl;
    pos1.y = gy;
    {
        int tz = p->mPosZ;
        int tx = p->mPosX;
        lineStart.x = tx;
        lineStart.y = arg1;
        lineStart.z = tz;
    }
    rl.SetObjAndLine(lineStart, pos1, (dActor_c *)c);
    if (!rl.DetectClsn()) {
        return 0;
    }
    rl.surface.CopyNormalTo(normal);
    if (normal.y >= 0x2c7) {
        return 0;
    }
    res = cstd::atan2(normal.x, normal.z);
    {
        int diff = AngleDiff(res, p->mAngleY);
        if (flag != 0) {
            if (diff < 0)
                diff = -diff;
            if (diff > 0x4000) {
                return 0;
            }
        } else {
            if (diff < 0)
                diff = -diff;
            if (diff < 0x4000) {
                return 0;
            }
        }
    }
    ::Vector3 clsnPos = rl.GetClsnPos();
    lineStart.y += 0x64000;
    pos1.y += 0x64000;
    rl.SetObjAndLine(lineStart, pos1, (dActor_c *)c);
    if (rl.DetectClsn()) {
        return 0;
    }
    lineStart.y = p->mPosY;
    Vector3 line2;
    line2.x = p->mPosX;
    line2.y = p->mPosY;
    line2.z = p->mPosZ;
    line2.y = gy + 0xa000;
    rl.SetObjAndLine(lineStart, line2, (dActor_c *)c);
    if (rl.DetectClsn()) {
        return 0;
    }
    p->mPosX = clsnPos.x - normal.x;
    p->mPosZ = clsnPos.z - normal.z;
    p->mPosY = gy;
    p->mAngleY = (u16)(res + 0x8000);
    return 1;
}
}

// @symbol func_ov002_020d0580
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" {
int func_ov002_020d0580(Player *p)
{
    extern Player::State data_ov002_021105a4;
    extern Player::State data_ov002_0210ffec;
    extern int func_ov002_020d0d2c(void *c);

    int b354, b358;

    if (p->GetHealth() != 0 && p->mIsNoControl == 0 && p->mIsUnderwater == 0) {
        b354 = (p->mRidingShell != 0);
        if (b354 == 0 && p->mClimbClsn == 0) {
            b358 = (p->mHeldObj != 0);
            if (b358 == 0 && p->mIsTakingDamage == 0 && p->mIsBalloon == 0 && p->IsInAir() != 0 &&
                func_ov002_020d0d2c(p) == 0) {
                goto cont;
            }
        }
    }
    return 0;

cont:
    if (p->IsState(data_ov002_021105a4)) {
        if (p->mStateStep != 0)
            return 0;
    }
    if (p->mIsAirborne == 0 || p->mVertSpeed > 0)
        return 0;
    if (func_ov002_020d0178((char *)p, p->mPosY, 0) == 0)
        return 0;
    p->mStateStep = 1;
    p->ChangeState(data_ov002_0210ffec);
    return 1;
}
}

// @symbol func_ov002_020d06c0
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" {
int func_ov002_020d06c0(Player *p)
{
    extern Player::State data_ov002_021101b4;
    extern Player::State data_ov002_0211001c;
    extern Player::State data_ov002_0210ffec;

    int thresh, b354, b358;

    if (!p->IsState(data_ov002_021101b4))
        return 0;

    if (p->GetHealth() != 0 && p->mHoldHeavyTimer == 0 && p->mIsUnderwater == 0) {
        b354 = (p->mRidingShell != 0);
        if (b354 == 0 && p->mClimbClsn == 0) {
            b358 = (p->mHeldObj != 0);
            if (b358 == 0 && p->mIsTakingDamage == 0 && p->mIsBalloon == 0 &&
                p->IsState(data_ov002_0211001c) == 0 && p->mIsAirborne != 0) {
                goto cont;
            }
        }
    }
    return 0;

cont:
    if (p->mHorzSpeed > Player_ScaleByCharFactor((void *)p, 0x14000))
        return 0;

    thresh = 0xa0;
    if (p->mIsMega != 0)
        thresh = 0x1c2;

    if (p->mGroundY > p->mPosY - thresh * 0x1000)
        return 0;

    if (func_ov002_020d0178((char *)p, p->mPosY, 1) == 0)
        return 0;

    p->mStateStep = 0;
    p->ChangeState(data_ov002_0210ffec);
    return 1;
}
}

// @symbol _ZN6Player17St_LedgeGrab_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_LedgeGrab_Main()
{
    extern int Player_AdvanceAnims(void*);
    extern Player::State data_ov002_0211013c;

  if(this->FinishedAnim())
    this->ChangeState(data_ov002_0211013c);
  if(this->IsAnim(0x20)){
    int id=this->GetBodyModelID(param1&0xff, 0);
    dExtAnmModel_c *anim = mModels.mBodyModels[id];
    if (anim->WillHitFrame(0x1e))
        Sound::PlayBank0(mGroundSoundType + 0x40, mCamSpacePos);
  }
  Player_AdvanceAnims(((char*)this));
  return 1;
}
}

// @symbol _ZN6Player17St_LedgeGrab_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_LedgeGrab_Init()
{

  if(mStateStep){
      this->SetAnim(0x20, 0x40000000, 0x1000, 0);
      Sound::PlayCharVoice(mCharacter, 0x1a, mCamSpacePos);
  } else {
      this->SetAnim(0x23, 0x40000000, 0x1000, 0);
      Sound::PlayCharVoice(mCharacter, 0x17, mCamSpacePos);
  }
  return 1;
}
}

// @symbol _ZN6Player20St_LedgeHang_CleanupEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_LedgeHang_Cleanup()
{
    extern void func_ov002_020e63a4(void*);

    func_ov002_020e63a4(((void *)this));
    return 1;
}
}

// @symbol func_ov002_020d0948
extern "C" {
void func_ov002_020d0948(char* self)
{
    extern short data_02082214[];
    extern int data_ov002_021101b4[];
    Player *p = (Player *)self;

    /* The two header reads go through a byte pointer: with the int-typed
       self, mwcc materializes the +0x703 offset from the literal pool
       (ldr r1,[pc] / ldrb r2,[r0,r1]) instead of folding it into the ldrb. */
    char* s = (char*)self;
    Player *player = (Player *)s;
    int base = (short)(player->mAngleY + 0x8000);
    if (player->mIsMega != 0) {
        int idx = (unsigned short)base >> 4;
        int *p0 = (int *)(((long long)(int)(&p->mPosX)));
        int *p1 = (int *)(((long long)(int)(&p->mPosY)));
        int *p2 = (int *)(((long long)(int)(&p->mPosZ)));
        *p0 = data_02082214[idx * 2] * 0x110 + *p0;
        *p1 = *p1 - 0x12c000;
        *p2 = data_02082214[idx * 2 + 1] * 0x110 + *p2;
    } else {
        int idx = (unsigned short)base >> 4;
        int *p0 = (int *)(((long long)(int)(&p->mPosX)));
        int *p1 = (int *)(((long long)(int)(&p->mPosY)));
        int *p2 = (int *)(((long long)(int)(&p->mPosZ)));
        *p0 = *p0 + (data_02082214[idx * 2] << 6);
        *p1 = *p1 - 0x64000;
        *p2 = *p2 + (data_02082214[idx * 2 + 1] << 6);
    }
    p->mHorzSpeed = -0x8000;
    ((Player *)self)->ChangeState(*(Player::State *)((int)data_ov002_021101b4));
}
}

// @symbol _ZN6Player17St_LedgeHang_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_LedgeHang_Main()
{

    extern Player::State data_ov002_02110004;
    extern int AngleDiff(int, int);
    extern void Player_AdvanceAnims(void* c);
    extern u16 data_0209f49e[];
    extern s16 data_0209f4a0[];

    int v = mGroundY;
    if (v != (int)0x80000000) {
        int d = v - mPosY;
        if (d < 0) d = -d;
        if (d < 0x28000) {
            mPosY = v;
            mIsAirborne = 0;
            if (mFloorNormalY < 0xddb) {
                func_ov002_020d0948(((char*)this));
                return 1;
            }
        }
    }

    if (mIsAirborne != 0) {
        func_ov002_020d0948(((char*)this));
        return 1;
    }

    if (mStateStep == 0) {
        if (this->FinishedAnim()) {
            this->SetAnim(0x21, 0, 0x1000, 0);
            mStateStep++;
        }
    } else {
        if ((!func_ov002_020cfaf0(((char*)this))
             && (*(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18) & 2))
            || func_ov002_020cfea4(((char*)this))) {
            mStateStep = 1;
            this->ChangeState(data_ov002_02110004);
        } else {
            if (func_ov002_020cfbdc(((char*)this)))
                return 1;
            if (*(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18) & 0x400) {
                func_ov002_020d0948(((char*)this));
                return 1;
            }
            {
                if (mStateTimer == 0
                    && *(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) != 0) {
                    if (AngleDiff(mDesiredAngleY, mAngleY) >= 0x4000) {
                        func_ov002_020d0948(((char*)this));
                    } else if (!func_ov002_020cfaf0(((char*)this))) {
                        mStateStep = 0;
                        this->ChangeState(data_ov002_02110004);
                    }
                }
            }
        }
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player17St_LedgeHang_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_LedgeHang_Init() {
    /* Stays a POD twin of Vector3, measured: `Vec3 z3 = {0, 0, 0}` on a POD
       lowers to `add r1, sp, #4` plus three stores through that pointer and
       two reloads, which is what the ROM does. Vector3 has a constructor, so every
       non-POD spelling constant-folds the zeros into the later stores and the
       function loses 12 bytes. Tested as Vector3(0,0,0), default plus three
       stores, copy-assignment, member-wise construction, Vector3(), and stores
       through a plain and a volatile pointer -- none reproduce it. */
    struct Vec3 {
        int x, y, z;
    };

    mStateWaitTimer = 0;
    if (mStateStep == 0) {
        this->SetAnim(0x22, 0x40000000, 0x1000, 0);
    } else {
        this->SetAnim(0x21, 0, 0x1000, 0);
    }
    mStateWaitTimer = 2;
    mPrevAngleY = mAngleY;
    mVertSpeed = 0;
    mHorzSpeed = 0;
    mIsAirborne = 0;
    Sound::PlayCharVoice(mCharacter, 0x16, mCamSpacePos);
    {
        struct Vec3 z3 = {0, 0, 0};
        struct Vec3 off;
        z3.y = -0x64000;
        off.x = z3.x;
        off.y = z3.y;
        off.z = z3.z;
        mdCcAcPos_c.SetPosRelativeToActor(*(Vector3 *)&off);
    }
    mStatePhase = 0;
    return 1;
}
}

// @symbol func_ov002_020d0d2c
extern "C" {
int func_ov002_020d0d2c(Player *c) {
    extern Player::State data_ov002_0210ffec;
    extern Player::State data_ov002_02110004;

    if (c->IsState(data_ov002_0210ffec) || c->IsState(data_ov002_02110004))
        return 1;
    return 0;
}
}

// @symbol _ZN6Player17St_WallSlide_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_WallSlide_Main()
{
    extern void func_02022b04(Fix12i, Fix12i, Fix12i);
    extern Player::State data_ov002_021103dc;
    typedef int Fix12i;
    extern int func_0201226c(int a0, int a1, int a2, int a3, int a4, short a5);
    extern void Player_AdvanceAnims(void *p);
    extern unsigned char data_0209f49e[];
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_021101b4;

  *((int *) ((char *)&mPeakY)) = *((int *) ((char *)&mPosY));
  if ((*((unsigned char *) ((char *)&mIsAirborne))) == 0)
  {
    func_ov002_020c2f64();
    this->ChangeState(data_ov002_0211013c);
  }
  else
  {
    if ((*((unsigned char *) ((char *)&mIsMega))) != 0)
    {
      func_02022b04(*((int *) ((char *)&mPosX)), *((int *) ((char *)&mPosY)), *((int *) ((char *)&mPosZ)));
    }
    else
    {
        Particle::RunningSlidingDustAt(*((int *)((char *)&mPosX)), *((int *)((char *)&mPosY)),
                                       *((int *)((char *)&mPosZ)));
    }
    *((int *) ((char *)&mLoopingSoundHandle)) = func_0201226c(*((int *) ((char *)&mLoopingSoundHandle)), 0, (*((int *) ((char *)&mGroundSoundType))) + 0xe2, (int) ((char *)&mCamSpacePosX), *((int *) ((char *)&mHorzSpeed)), 0);
    if ((*((unsigned short *) (&data_0209f49e[gActivePlayerSlot * 0x18]))) & 2)
    {
        if (mStateTimer) {
            mAngleY = mAngleYSpeed;
        }
      this->ChangeState(data_ov002_021103dc);
    }
    else
    {
      if ((*((unsigned char *) ((char *)&mClsnFlags))) & 2)
      {
        goto end;
      }
      this->ChangeState(data_ov002_021101b4);
    }
  }
  end:
  Player_AdvanceAnims(((char *)this));
  return 1;
}
}

// @symbol _ZN6Player17St_WallSlide_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_WallSlide_Init()
{

    this->SetAnim(0x29, 0x40000000, 0x1000, 0);
    mHorzSpeed = 0;
    if (mVertSpeed >= 0)
        mVertSpeed = 0;
    mStateTimer = 7;
    return 1;
}
}

// @symbol _ZN6Player13St_Crawl_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Crawl_Main()
{
    extern int func_ov002_020d1164(void*);
    extern int func_ov002_020d36d8(void* c, int arg);
    extern int Player_ScaleByCharFactor(void* c, int a);
    extern int func_ov002_020bf224(void* c, int a, int b);
    extern void ApproachAngle(s16* cur, s16 target, int divisor, int band, int maxStep);
    extern void func_ov002_020c18b0(void* c, int a);
    extern int func_ov002_020d4d88(void* c, int a, int b);
    extern void Player_AdvanceAnims(void* c);
    extern s16 data_0209f4a0[];
    extern u16 data_0209f49c[];
    extern Player::State data_ov002_021104e4;

    int a = 0;
    int b = 0;

    switch (mStateStep) {
    case 3:
        if (this->FinishedAnim()) {
            mStateStep = 4;
            this->SetAnim(0x63, 0, 0x1000, 0);
        }
        break;
    case 4: {
        u8 idx = gActivePlayerSlot;
        int off = idx * 0x18;
        s16 val1 = *(s16*)((char*)data_0209f4a0 + off);
        if (val1 >= 0x200 && ((*(u16*)((char*)data_0209f49c + off) & 0x400) != 0 || func_ov002_020d1164(((char*)this)) != 0)) {
            int id = this->GetBodyModelID(param1 & 0xff, 0);
            dExtAnmModel_c *anim = mModels.mBodyModels[id];
            if (anim->WillHitFrame(0xe) ||
                mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(0x1c)) {
                Sound::PlayBank0(mGroundSoundType + 0xc0, mCamSpacePos);
            }
            int shifted = mHorzSpeed >> 2;
            int id3 = this->GetBodyModelID(param1 & 0xff, 0);
            /* Launder kept: storing through mBodyModels[id3] directly moves bytes. */
            dExtFrameCtrl_c *anim3 = reinterpret_cast<dExtFrameCtrl_c *>(
                (long long)(int)&static_cast<dExtFrameCtrl_c &>(*mModels.mBodyModels[id3]));
            anim3->speed = shifted;
            if (func_ov002_020d36d8(((char*)this), 1))
                return 1;
            int r4v = Player_ScaleByCharFactor(((char*)this), 0x8000);
            int r2v = Player_ScaleByCharFactor(((char*)this), 0x1000);
            a = func_ov002_020bf224(((char*)this), r4v, r2v);
            b = 0x1000;
            ApproachAngle((s16*)((char*)&mPrevAngleY), mDesiredAngleY, 0x10, 0x2000, 0x100);
            mAngleY = mPrevAngleY;
            func_ov002_020c18b0(((char*)this), 0);
        } else {
            mHorzSpeed = 0;
            mStateStep = 5;
            this->SetAnim(0x62, 0x40000000, 0x1000, 0);
        }
        break;
    }
    case 5:
        if (this->FinishedAnim()) {
            mStateStep = 1;
            this->ChangeState(data_ov002_021104e4);
        }
        break;
    }

    func_ov002_020d4d88(((char*)this), a, b);
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player13St_Crawl_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Crawl_Init()
{

  mStateStep=3;
  this->SetAnim(0x64, 0x40000000, 0x1000, 0);
  return 1;
}
}

// @symbol func_ov002_020d1164
// func_ov002_020d1164 at 0x020d1164
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
int func_ov002_020d1164(char* c)
{
    Player *player = (Player *)c;

    dBgCh_Lin line;
    Vector3 v1;
    Vector3 v2;
    int y;
    v1.x = player->mPosX;
    v1.y = player->mPosY;
    v1.z = player->mPosZ;
    v2.x = player->mPosX;
    y = player->mPosY;
    v2.y = y;
    v2.z = player->mPosZ;
    v2.y = y + 0x96000;
    line.SetObjAndLine(v1, v2, (dActor_c*)c);
    if (line.DetectClsn()) {
        return 1;
    }
    return 0;
}
}

// @symbol func_ov002_020d1204
extern "C" {
int func_ov002_020d1204(void* thisptr)
{
    extern unsigned char data_0209f49e[];
    extern Player::State data_ov002_0211052c;
    extern Player::State data_ov002_0211055c;
    extern Player::State data_ov002_0211019c;

    unsigned char* p = (unsigned char*)thisptr;
    Player *player = (Player *)p;
    if (*(unsigned short*)(data_0209f49e + gActivePlayerSlot * 0x18) & 2) {
        int v = player->mHorzSpeed;
        if (v == 0) {
            ((Player *)thisptr)->ChangeState(data_ov002_0211052c);
        } else if (player->mStateTimer != 0 && v > 0xa000) {
            ((Player *)thisptr)->ChangeState(data_ov002_0211055c);
            return 1;
        } else {
            ((Player *)thisptr)->ChangeState(data_ov002_0211019c);
        }
        return 1;
    }
    return 0;
}
}

// @symbol func_ov002_020d12b0
extern "C" {
int func_ov002_020d12b0(u8 *self)
{
    Player *player = (Player *)self;
    extern u8 data_0209f49e[];
    extern Player::State data_ov002_02110574;
    extern Player::State data_ov002_02110634;
    extern Player::State data_ov002_0211061c;
    extern int Player_ScaleByCharFactor(void *c, int a);

    if ((*(u16 *)(data_0209f49e + gActivePlayerSlot * 0x18) & 1) == 0) goto ret0;
    if (self[0x703] != 0) {
        self[0x6e2] = 2;
        ((Player *)self)->ChangeState(data_ov002_02110574);
    } else {
        if (player->mHorzSpeed >= Player_ScaleByCharFactor(self, 0xa000)) {
            ((Player *)self)->ChangeState(data_ov002_02110634);
        } else {
            ((Player *)self)->ChangeState(data_ov002_0211061c);
        }
    }
    return 1;
ret0:
    return 0;
}
}

// @symbol _ZN6Player14St_Crouch_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Crouch_Main()
{
    extern int func_ov002_020dd2f4(void* c);
    extern void Player_AdvanceAnims(void* c);
    extern u16 data_0209f49c[];
    extern s16 data_0209f4a0[];
    extern Player::State data_ov002_021101b4;
    extern Player::State data_ov002_02110514;
    extern Player::State data_ov002_0211013c;

    func_ov002_020bf90c();

    if (mIsAirborne != 0) {
        int d = mPosY - mGroundY;
        if (d >= 0x32000) {
            this->ChangeState(data_ov002_021101b4);
            return 1;
        }
    }

    switch (mStateStep) {
    case 0:
        if (this->IsAnim(0x2d) && this->FinishedAnim()) {
            this->SetAnim(0x2c, 0, 0x1000, 0);
            mStateStep = 1;
        }
        if (func_ov002_020d12b0(((u8*)this)))
            return 1;
        func_ov002_020d1204(((char*)this));
        break;
    case 1:
        if ((*(u16*)((char*)data_0209f49c + gActivePlayerSlot * 0x18) & 0x400) != 0
            || mHorzSpeed != 0
            || func_ov002_020d1164(((char*)this)) != 0) {
            if (mHorzSpeed != 0) {
                func_ov002_020c06fc(0x4000);
                func_ov002_020dd2f4(((char*)this));
            } else {
                if (mIsMega == 0) {
                    if (*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) >= 0x200) {
                        this->ChangeState(data_ov002_02110514);
                        return 1;
                    }
                }
            }
            u16 bit = mStateFlags & 1;
            if (bit != 0) {
                func_ov002_020c0364(1);
                return 1;
            }
        } else {
            this->SetAnim(0x2e, 0x40000000, 0x1000, 0);
            mStateStep = 2;
        }
        if (func_ov002_020d12b0(((u8*)this)))
            return 1;
        if (func_ov002_020d1204(((char*)this)) != 0)
            return 1;
        break;
    case 2:
        if (this->IsAnim(0x2e) && this->FinishedAnim()) {
            this->ChangeState(data_ov002_0211013c);
            mCrouchTimer = 0xa;
        } else {
            func_ov002_020d1204(((char*)this));
        }
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player14St_Crouch_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Crouch_Init() {
    extern unsigned char data_0209f49c[];

    if (mDashBoostTimer) {
        Sound::PlayCharVoice(mCharacter, 0x2e, mCamSpacePos);
    }
    mStateTimer = 0x1e;
    if (*(unsigned short *)(data_0209f49c + gActivePlayerSlot * 0x18) & 2) {
        if (func_ov002_020d1204(((char *)this)))
            return 1;
    }
    if (mStateStep == 0) {
        this->SetAnim(0x2d, 0x40000000, 0x1000, 0);
        mPrevAngleY = mAngleY;
    } else if (mStateStep == 1) {
        this->SetAnim(0x2c, 0, 0x1000, 0);
    }
    return 1;
}
}

// @symbol _ZN6Player17St_HoldHeavy_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_HoldHeavy_Main()
{
    extern void func_ov002_020d1f78(void*, u32);
    extern int data_ov002_020ff254[];
    extern int data_ov002_020ff1d0[];
    extern void func_ov002_020da9d4(void* c);
    extern void ApproachAngle(short* cur, short target, int divisor, int band, int maxStep);
    extern int func_ov002_020bf224(void* c, int a, int b);
    extern void func_ov002_020d4d88(void* c, int a, int b);
    extern void Player_AdvanceAnims(void* c);
    extern Player::State data_ov002_0211013c;
    extern u16 data_0209f49e[];
    extern Player::State data_ov002_021105d4;
    extern Player::State data_ov002_021101b4;
    extern s16 data_0209f4a0[];
    extern u8 data_0209f4ac[];
    extern u16 data_0209f49c[];

    int var_r1 = 0;

    if (mHeldObj == 0) {
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }

    if (mStateStep == 0) {
        u32 anim = data_ov002_020ff254[mStateWork];
        if (this->IsAnim(anim)) {
            if (this->FinishedAnim()) {
                mStateStep = 1;
            } else {
                u32 arg = (u8)param1;
                int modelIdx = this->GetBodyModelID(arg, 0);
                if (mModels.mBodyModels[modelIdx]->WillHitFrame(6)) {
                    dActor_c *heavy = *(dActor_c **)((char *)&mHeldObj);
                    if (heavy != 0) {
                        heavy->mFlags |= 0x4000;
                    }
                }
            }
        }
        ApproachLinear(*((int *)((char *)&mHorzSpeed)), 0, 0x1000);
        goto end;
    }

    {
        int off = gActivePlayerSlot * 0x18;
        u16 flags = *(u16*)((char*)data_0209f49e + off);
        if (flags & 1) {
            this->ChangeState(data_ov002_021105d4);
            return 1;
        }

        if (mIsAirborne != 0) {
            func_ov002_020da9d4(((char*)this));
            this->ChangeState(data_ov002_021101b4);
            mHoldHeavyTimer = 0xa;
            return 1;
        }

        {
            s16 t = *(s16*)((char*)data_0209f4a0 + off);
            if (t != 0) {
                ApproachAngle((short*)((char*)&mPrevAngleY), mDesiredAngleY, 8, 0x1000, 0x400);
                var_r1 = data_ov002_020ff1d0[param1];
            }
        }
    }

    {
        int off2 = gActivePlayerSlot * 0x18;
        if (*((u8*)data_0209f4ac + off2) == 0) {
            if (!(*(u16*)((char*)data_0209f49c + off2) & 0x800)) {
                var_r1 >>= 1;
            }
        } else {
            var_r1 = func_ov002_020bf224(((char*)this), var_r1, 0);
        }
        func_ov002_020d4d88(((char*)this), var_r1, 0x1000);
    }

    if (mHorzSpeed == 0) {
        mAngleY = mPrevAngleY;
    } else if (param1 == 1) {
        ApproachAngle((short*)((char*)&mAngleY), mPrevAngleY, 0x10, 0x1000, 0x100);
    } else {
        ApproachAngle((short*)((char*)&mAngleY), mPrevAngleY, 8, 0x2000, 0x400);
    }

    if (mHorzSpeed == 0) {
        if (this->FinishedAnim() ||
            !mModels.mBodyModels[this->GetBodyModelID((u8)param1, 0)]->GetFlags()) {
            this->SetAnim(data_ov002_020ff254[mStateWork + 2], 0, 0x1000, 0);
        }
    } else {
        func_ov002_020d1f78(((char*)this), data_ov002_020ff254[mStateWork + 4]);
    }

end:
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player17St_HoldHeavy_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_HoldHeavy_Init()
{
    extern int data_ov002_020ff254[];

  mStateWork=0;
  this->SetAnim(data_ov002_020ff254[mStateWork], 0x40000000, 0x1000, 0);
  Sound::PlayCharVoice(mCharacter, 0x12, mCamSpacePos);
  return 1;
}
}

// @symbol _ZN6Player20St_HoldLight_CleanupEv
/* recovered: named members + shared header, real C++ method */
int Player::St_HoldLight_Cleanup()
{

  char* p = *(char**)((char*)&mHeldObj);
  if (p) {
      ((dActor_c *)p)->mFlags |= 0x4000;
  }
  return 1;
}

// @symbol _ZN6Player17St_HoldLight_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_HoldLight_Main()
{
    extern int data_ov002_020ff1c0[];
    extern Player::State data_ov002_0211019c;
    extern int func_ov002_020d9dcc(void*);
    extern void func_ov002_020d1f78(void*, u32);
    extern int func_ov002_020e0ccc(void* c, short* st);
    extern int func_ov002_020c5244(void* c);
    extern int Player_ReleaseHeldActor(void* c);
    extern void ApproachAngle(short* cur, short target, int divisor, int band, int maxStep);
    extern int func_ov002_020bf224(void* c, int a, int b);
    extern void func_ov002_020d4d88(void* c, int a, int b);
    extern void Player_AdvanceAnims(void* c);
    extern s16 data_0209f4a0[];
    extern u8 data_0209f4ac[];
    extern u16 data_0209f49c[];
    extern u16 data_0209f49e[];
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_021105d4;
    extern Player::State data_ov002_021101b4;

    if (mHeldObj == 0) {
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }
    if (func_ov002_020c0434()) {
        func_ov002_020c0364(3);
        return 1;
    }

    if (mStateStep == 0) {
        ApproachLinear(*((int *)((char *)&mHorzSpeed)), 0, 0x800);
        if (this->IsAnim(0x2f) || this->IsAnim(0x86)) {
            if (this->FinishedAnim()) {
                if (func_ov002_020e0ccc(((char*)this), *(short**)((char*)&mHeldObj))) {
                    return 1;
                }
                mPrevAngleY = mAngleY;
                mHorzSpeed = 0;
                mStateStep = 1;
            } else {
                u32 arg = (u8)param1;
                int modelIdx = this->GetBodyModelID(arg, 0);
                if (mModels.mBodyModels[modelIdx]->WillHitFrame(6)) {
                    dActor_c *light = *(dActor_c **)((char *)&mHeldObj);
                    if (light != 0) {
                        u32 *p = &light->mFlags;
                        *p |= 0x4000;
                    }
                }
            }
        }
        goto end;
    }

    if (func_ov002_020c5244(((char*)this))) {
        return 1;
    }

    if (mSinkDepth > 0x19000) {
        Player_ReleaseHeldActor(((char*)this));
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }

    int var_r4 = 0;
    if (*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) != 0) {
        s16 t = mDesiredAngleY;
        if (mStateArg != 0) {
            mPrevAngleY = t;
            mStateArg = 0;
        }
        ApproachAngle((short*)((char*)&mPrevAngleY), t, 4, 0x2000, 0x800);
        var_r4 = data_ov002_020ff1c0[param1];
    }

    if (*((u8*)data_0209f4ac + gActivePlayerSlot * 0x18) == 0) {
        if (!(*(u16*)((char*)data_0209f49c + gActivePlayerSlot * 0x18) & 0x800)) {
            var_r4 = FX_MUL(var_r4, 0xa00);
        }
    } else {
        var_r4 = func_ov002_020bf224(((char*)this), var_r4, 0);
    }

    mStateArg = 0;
    if (mHorzSpeed == 0) {
        mStateArg = 1;
        mAngleY = mPrevAngleY;
    } else {
        ApproachAngle((short*)((char*)&mAngleY), mPrevAngleY, 8, 0x2000, 0x800);
    }

    func_ov002_020d4d88(((char*)this), var_r4, 0x1000);

    {
        u16 flags = *(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18);
        if (flags & 1) {
            this->ChangeState(data_ov002_021105d4);
            return 1;
        }
        if (flags & 2) {
            mJumpComboTimer = 0;
            this->ChangeState(data_ov002_0211019c);
            return 1;
        }
        if (flags & 0x400) {
            if (Player_ReleaseHeldActor(((char*)this))) {
                this->ChangeState(data_ov002_0211013c);
            }
            return 1;
        }
    }

    if (mIsAirborne != 0) {
        this->ChangeState(data_ov002_021101b4);
        return 1;
    }
    if (func_ov002_020d9dcc(((char*)this))) {
        return 1;
    }

    if (mHorzSpeed == 0) {
        if (this->FinishedAnim() ||
            !mModels.mBodyModels[this->GetBodyModelID((u8)param1, 0)]->GetFlags()) {
            dActor_c *light = *(dActor_c **)((char *)&mHeldObj);
            u32 animId = 0x33;
            if (light != 0) {
                int b = (light->mFlags & 0x8000) != 0;
                if (b) {
                    animId = 0x87;
                    if (param1 == 2) {
                        int b2 = (light->actorID == 0xce);
                        if (b2) {
                            animId = 0x33;
                        }
                    }
                }
            }
            this->SetAnim(animId, 0, 0x1000, 0);
        }
    } else {
        func_ov002_020d1f78(((char*)this), 0x48);
    }

end:
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player17St_HoldLight_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_HoldLight_Init() {
    extern int Player_ScaleByCharFactor(char *c, int a);

    if (mHorzSpeed >= Player_ScaleByCharFactor(((char *)this), 0x1c000)) {
        mHorzSpeed = Player_ScaleByCharFactor(((char *)this), 0x1c000);
    }
    if (mStateStep == 0) {
        char *o = *(char **)((char *)&mHeldObj);
        unsigned int anim = 0x2f;
        if (o != 0) {
            int b = (int)((((dActor_c *)o)->mFlags & 0x8000) != 0);
            if (b != 0) {
                anim = 0x86;
                if (param1 == 2) {
                    if ((int)(((dActor_c *)o)->actorID == 0xce) != 0)
                        anim = 0x2f;
                }
            }
        }
        this->SetAnim(anim, 0x40000000, 0x1000, 0);
        Sound::PlayCharVoice(mCharacter, 0x12, mCamSpacePos);
    }
    mStateWork = 0;
    mAngleYSpeed = 0;
    mStateArg = 0;
    return 1;
}
}

// @symbol func_ov002_020d1f78
extern "C" {
void func_ov002_020d1f78(void *selfPtr, u32 param)
{
    typedef struct { u8 pad[0x18]; } Rec18;
    extern Rec18 data_0209f4ac[];
    extern Rec18 data_0209f49c[];
    extern Rec18 data_0209f4a0[];
    extern int data_0209f32c;
    extern u32 func_02022d00(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void *dir);
    extern void func_0201251c(int a, int b, int c, int d);
    extern void PlayerNewSimpleHold8(unsigned int id, Fix12i a, Fix12i b, Fix12i c);

    Player *self = (Player *)selfPtr;
    Player *p = self;
    int idx;
    int flag;
    int id;
    char *anim;
    u32 w;
    int state;
    int target;

    idx = gActivePlayerSlot;
    flag = 0;
    if (*(u8 *)&data_0209f4ac[idx] == 0) {
        if ((*(u16 *)&data_0209f49c[idx] & 0x800) == 0)
            flag = 1;
    } else {
        if (*(s16 *)&data_0209f4a0[idx] < 0xb33)
            flag = 1;
    }

    if (p->param1 == 2 && flag == 0) {
        id = self->GetBodyModelID(p->param1 & 0xff, 0);
        anim = (char *)((long long)(int)&(dExtFrameCtrl_c &)*p->mModels.mBodyModels[id]);
        w = ((dExtFrameCtrl_c *)anim)->currFrame;
        /* The whole frame of the 20.12 counter; the u16 parameter's narrowing
           folds with the shift into the ROM's lsl #4 / lsr #16. */
        p->SetAnim(0x3f, 0, 0x2800, w >> 12);
    } else {
        int v = p->mHorzSpeed;
        int shift = v >> 3;
        if (shift < 0x400)
            shift = 0x400;
        self->SetAnim(param, 0, shift, 0);
    }

    id = self->GetBodyModelID(p->param1 & 0xff, 0);
    if (!p->mModels.mBodyModels[id]->WillHitFrame(4)) {
        id = self->GetBodyModelID(p->param1 & 0xff, 0);
        if (!p->mModels.mBodyModels[id]->WillHitFrame(0x13))
            return;
    }

    if (p->mIsInShallowWater != 0) {
        u32 newId = func_02022d00((u32)p->mParticle1, 0xde, p->mPosX, data_0209f32c, p->mPosZ, 0);
        p->mParticle1 = newId;
        return;
    }

    func_0201251c(0, p->mGroundSoundType + 0x20, (int)(&p->mCamSpacePosX), p->mHorzSpeed);

    state = p->mGroundSoundType;
    target = -1;
    if (state == 6)
        goto state_case6;
    if (state == 8)
        goto state_case8;
    if (state == 9) {
        target = 0xe6;
    }
    goto state_merge;
state_case8:
    target = 0xe4;
    goto state_merge;
state_case6:
    target = 0xe5;
state_merge:
    if (target == -1)
        return;

    Particle::System::NewSimple(target, p->mPosX, p->mPosY, p->mPosZ);
}
}

// @symbol _ZN6Player21St_WaitQuicksand_MainEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_WaitQuicksand_Main()
{
    extern int func_ov002_020d22ec(void*,int);
    extern int Player_AdvanceAnims(void*);

  int v=mSinkDepth;
  if(v<0x1e000){
    this->ChangeState(ST_WAIT);
    return 1;
  }
  if(v>=0x46000){
      this->SetAnim(0x82, 0, 0x1000, 0);
  }
  if(mSinkDepth>=0xb4000){
    if(func_ov002_020c5dec(3)) return 1;
  }
  func_ov002_020d22ec(((char*)this),0);
  Player_AdvanceAnims(((char*)this));
  return 1;
}
}

// @symbol _ZN6Player21St_WaitQuicksand_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_WaitQuicksand_Init()
{

    this->SetAnim(0x47, 0, 0x1000, 0);
    mStateWork = 0;
    return 1;
}
}

// @symbol func_ov002_020d225c
extern "C" {
int func_ov002_020d225c(char *o)
{
    Player *player = (Player *)o;

    if (player->param1 == 3 && player->mSleepStage == 2 && player->mStatePhase == 0)
        return 1;
    return 0;
}
}

// @symbol func_ov002_020d228c
extern "C" {
void func_ov002_020d228c(Player *player) {
    extern Player::State data_ov002_02110154;

    player->ChangeState(data_ov002_02110154);
    player->SetAnim(9, 0, 0x1000, 0);
    player->mStateStep = 4;
    player->mSleepBubbleScaleX = 0;
    player->mSleepBubbleScaleY = 0;
    player->mSleepBubbleScaleZ = 0;
    player->mSleepStage = 2;
    player->mStateArg = 0;
}
}

// @symbol func_ov002_020d22ec
extern "C" {
int func_ov002_020d22ec(char* c, int arg)
{
    extern void func_ov002_020cabe0(char* c);
    extern int func_ov002_020d36d8(char* c, int x);
    extern char data_0209f4a0[];
    extern Player::State data_ov002_0211013c;
    Player *player = (Player *)c;

    if (*(short*)(data_0209f4a0 + gActivePlayerSlot * 0x18) > arg) {
        ((Player *)c)->ChangeState(data_ov002_0211013c);
        return 1;
    }
    if ((unsigned short)(player->mStateFlags & 4)) {
        func_ov002_020cabe0(c);
        return 1;
    }
    return func_ov002_020d36d8(c, 0);
}
}

// @symbol _ZN6Player15St_Wait_CleanupEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Wait_Cleanup()
{
    extern unsigned char data_0209f2d8;
    extern void* data_0209f318;
    extern int data_0209caa0[];

    void* r4 = data_0209f318;
    ((dCamera_c *)r4)->mFlags &= ~0x2000;
    mSleepStage = 0;
    do {
        unsigned char v = data_0209f2d8;
        int b0 = (v == 1);
        if (b0 == 0) {
            if ((data_0209caa0[2] & 0x80) != 0) {
                int b1 = (v == 2);
                if (b1 == 0) break;
            }
            if (mAnimScriptId == 0xf)
                mRootMotionMode = 0;
            mAnimScriptId = 0;
            mAnimScriptStep = 0;
        }
    } while (0);
    return 1;
}
}

// @symbol _ZN6Player12St_Wait_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Wait_Main()
{

    extern Player::State data_ov002_0211049c;
    extern void func_ov002_020bcdf0(void*);
    extern int func_ov002_020d2fdc(void*);
    extern Player::State data_ov002_0211019c;
    extern int func_ov002_020d2da0(void*);
    extern void func_ov002_020d2f24(void*);
    extern void func_ov002_020d2e74(void*);
    extern int func_ov002_020c5244(void);
    extern int func_ov002_020d22ec(char* c, int arg);
    extern void func_ov002_020cabe0(void* c);
    extern void Player_AdvanceAnims(void* c);
    extern void* data_0209f318;
    extern unsigned char data_0209f49c[];
    extern char data_0209f4a0[];
    extern unsigned char data_0209f4ae[];
    extern unsigned char data_0209f2d8;
    extern int data_0209caa0[];
    extern unsigned char data_0209f49e[];
    extern Player::State data_ov002_0211013c;

    if ((unsigned short)(mStateFlags & 1)) {
        func_ov002_020c0364(3);
        return 1;
    }
    if (func_ov002_020c5244()) {
        return 1;
    }
    if (func_ov002_020c6adc()) {
        return 1;
    }
    if (mSinkDepth >= 0x1e000) {
        this->ChangeState(data_ov002_0211049c);
        return 1;
    }

    {
    unsigned char st = mStateStep;
    void* r4 = data_0209f318;
    switch (st) {
    default:
        break;

    case 8: {
        if (!(*(unsigned short *)(data_0209f49c + gActivePlayerSlot * 0x18) & 0x800) &&
            mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(0)) {
            Sound::PlayCharVoice(mCharacter, 0x2c, mCamSpacePos);
        }
    }
    case 0: {
        unsigned char f2d8_0 = data_0209f2d8;
        int b0 = (f2d8_0 == 1);
        if (b0 == 0) {
            if (data_0209caa0[2] & 0x80) {
                int b1 = (f2d8_0 == 2);
                if (b1) goto blk0_34;
                goto blk0_35;
            }
            blk0_34:
            func_ov002_020bcdf0(((char*)this));
            goto blk0_36;
        }
        blk0_35:
        if (func_ov002_020d2fdc(((char*)this)) == 0) {
            blk0_36:
            {
                int idx0 = gActivePlayerSlot * 0x18;
                short lvl0 = *(short*)(data_0209f4a0+idx0);
                if (lvl0 != 0) {
                    int thr0 = (*(unsigned char*)(data_0209f4ae+idx0)!=2) ? 0x471 : 0x555;
                    if (lvl0 < thr0) {
                        mPrevAngleY = mDesiredAngleY;
                        mAngleY = mPrevAngleY;
                    }
                }
                {
                    int thr0b = (*(unsigned char*)(data_0209f4ae+idx0)!=2) ? 0x471 : 0x555;
                    func_ov002_020d22ec(((char*)this), thr0b);
                }
            }
        }
        break;
    }

    case 10: {
        if (mIsMetal!=0 || mIsVanish!=0) {
            mStateStep = 0;
            this->SetAnim(0x47, 0, 0x1000, 0);
            mStateTimer = 0x384;
        }
        {
            int idx10 = gActivePlayerSlot * 0x18;
            short lvl10 = *(short*)(data_0209f4a0+idx10);
            if (lvl10 != 0) {
                int thr10 = (*(unsigned char*)(data_0209f4ae+idx10)!=2) ? 0x471 : 0x555;
                if (lvl10 < thr10) {
                    mPrevAngleY = mDesiredAngleY;
                    mAngleY = mPrevAngleY;
                }
            }
            {
                int thr10b = (*(unsigned char*)(data_0209f4ae+idx10)!=2) ? 0x471 : 0x555;
                func_ov002_020d22ec(((char*)this), thr10b);
            }
        }
        break;
    }

    case 1: {
        if (this->FinishedAnim()) {
            this->SetAnim(0xc, 0, 0x1000, 0);
            mStateStep = 2;
        }
        {
        unsigned int x1 = param1;
        switch (x1) {
        default:
            break;
        case 0: {
            int idA = this->GetBodyModelID(x1&0xff, 0);
            dExtAnmModel_c *animA = mModels.mBodyModels[idA];
            if (animA->WillHitFrame(0x5a)) {
                Sound::PlayCharVoice(mCharacter, 0x2d, mCamSpacePos);
            } else if (mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(
                           0x3a) ||
                       mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(
                           0x44)) {
                Sound::PlayBank0(4, mCamSpacePos);
            }
            break;
        }
        case 1: {
            int idD = this->GetBodyModelID(x1&0xff, 0);
            dExtAnmModel_c *animD = mModels.mBodyModels[idD];
            if (animD->WillHitFrame(0x6e)) {
                Sound::PlayCharVoice(mCharacter, 0x2d, mCamSpacePos);
            } else {
                int idE = this->GetBodyModelID(param1&0xff, 0);
                dExtAnmModel_c *animE = mModels.mBodyModels[idE];
                if (animE->WillHitFrame(0x32)) {
                    Sound::PlayBank0(0xb1, mCamSpacePos);
                }
            }
            break;
        }
        case 2: {
            int idF = this->GetBodyModelID(x1&0xff, 0);
            dExtAnmModel_c *animF = mModels.mBodyModels[idF];
            if (animF->WillHitFrame(0x20)) {
                Sound::PlayBank0(2, mCamSpacePos);
            } else if (mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(
                           0x47) ||
                       mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(
                           0x60)) {
                Sound::PlayBank0(3, mCamSpacePos);
            }
            break;
        }
        case 3: {
            int idI = this->GetBodyModelID(x1&0xff, 0);
            dExtAnmModel_c *animI = mModels.mBodyModels[idI];
            if (animI->WillHitFrame(0xad)) {
                Sound::PlayBank0(0x50, mCamSpacePos);
            }
            break;
        }
        }
        }
        if (func_ov002_020d22ec(((char*)this), 0)) {
            ((dCamera_c *)r4)->mFlags &= ~0x2000;
            Sound::PlayCharVoice(mCharacter, 0x2e, mCamSpacePos);
        }
        break;
    }

    case 2: {
        if (mMuteSleepVoice == 0 &&
            mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(1)) {
            Sound::PlayCharVoice(mCharacter, 0x2a, mCamSpacePos);
        }
        {
            unsigned char f2d8_2 = data_0209f2d8;
            int b2a = (f2d8_2 == 1);
            if (b2a == 0) {
                if ((data_0209caa0[2] & 0x80) == 0) break;
                int b2b = (f2d8_2 == 2);
                if (b2b != false) break;
            }
        }
        if (mStateTimer == 0) {
            Sound::PlayCharVoice(mCharacter, 0x2e, mCamSpacePos);
            this->SetAnim(0xa, 0x40000000, 0x1000, 0);
            mStateStep = 3;
            mSleepStage = 1;
        }
        if (*(unsigned short*)(data_0209f49e+gActivePlayerSlot*0x18) & 2) {
            this->ChangeState(data_ov002_0211019c);
            ((dCamera_c *)r4)->mFlags &= ~0x2000;
            Sound::PlayCharVoice(mCharacter, 0x2e, mCamSpacePos);
            return 1;
        }
        if (func_ov002_020d2da0(((char*)this))==1 || (unsigned short)(mStateFlags&4)!=0) {
            this->SetAnim(0xb, 0x40000000, 0x1000, 0);
            mStateStep = 9;
        }
        break;
    }

    case 3: {
        if (this->FinishedAnim()) {
            this->SetAnim(9, 0, 0x1000, 0);
            mStateStep = 4;
            mSleepBubbleScaleX = 0;
            mSleepBubbleScaleY = 0;
            mSleepBubbleScaleZ = 0;
            mSleepStage = 2;
            mStateArg = 1;
        }
        if ((unsigned short)(mStateFlags&4) != 0) {
            func_ov002_020cabe0(((char*)this));
            return 1;
        }
        break;
    }

    case 5: {
        if (this->FinishedAnim()) {
            mStateStep = 6;
            this->SetAnim(0x96, 0, 0x1000, 0);
        }
        func_ov002_020d22ec(((char*)this), 0);
        break;
    }

    case 4: {
        if (mStatePhase == 0) {
            func_ov002_020d2f24(((char*)this));
            if (mMuteSleepVoice == 0 &&
                mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(0xd)) {
                Sound::PlayCharVoice(mCharacter, 0x2b, mCamSpacePos);
            }
        }
        {
            unsigned char f2d8_4 = data_0209f2d8;
            int b4a = (f2d8_4 == 1);
            if (b4a == 0) {
                if ((data_0209caa0[2] & 0x80) == 0) break;
                int b4b = (f2d8_4 == 2);
                if (b4b != false) break;
            }
            if (func_ov002_020d2da0(((char *)this)) == 1 || mSleepBubbleScaleX >= 0x3000 ||
                mIsInShallowWater != 0 || (unsigned short)(mStateFlags & 4)) {
                func_ov002_020d2e74(((char*)this));
                mSleepStage = 0;
                this->SetAnim(8, 0x40000000, 0x1000, 0);
                mStateStep = 9;
            }
        }
        break;
    }

    case 6: {
        if (func_ov002_020d2da0(((char*)this)) == 1) {
            this->SetAnim(0x97, 0x40000000, 0x1000, 0);
            mStateStep = 9;
        }
        break;
    }

    case 7: {
        if (this->FinishedAnim()) {
            this->SetAnim(0xf, 0, 0x1000, 0);
            mStateStep = 8;
        }
        if ((unsigned short)(mStateFlags&0x800) == 0) {
            func_ov002_020d22ec(((char*)this), 0);
        }
        break;
    }

    case 9: {
        if (this->FinishedAnim()) {
            this->ChangeState(data_ov002_0211013c);
        }
        break;
    }
    }
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol func_ov002_020d2da0
extern "C" {
int func_ov002_020d2da0(Player *player) {
    struct E18 {
        short v;
        char pad[0x16];
    };
    extern struct E18 data_0209f4a0[];
    extern struct E18 data_0209f49e[];
    extern char *data_0209f318;
    extern Player::State data_ov002_021101b4;

    unsigned char idx = gActivePlayerSlot;
    char *p = data_0209f318;
    int cond = (int)(data_0209f4a0[idx].v >= 0x200);
    if (cond | (((unsigned short)data_0209f49e[idx].v) & 0xc03)) {
        ((dCamera_c *)p)->mFlags &= ~0x2000;
        Sound::PlayCharVoice(player->mCharacter, 0x2e, player->mCamSpacePos);
        return 1;
    }
    if (player->mIsAirborne == 0)
        return 0;
    ((dCamera_c *)p)->mFlags &= ~0x2000;
    player->ChangeState(data_ov002_021101b4);
    return 2;
}
}

// @symbol func_ov002_020d2e74
/* recovered: shared common types */
extern "C" {
void func_ov002_020d2e74(Player *p) {
    extern void Vec3_RotateYAndTranslate(Vector3 * out, const Vector3 *base, s16 ang,
                                         const Vector3 *off);
    extern void PlayerNewSimpleHold9(u32 a, int b, int c, int d);

    Vector3 v;
    Vector3 off;
    if (p->mIsInShallowWater != 0)
        return;
    if (p->param1 != 3)
        return;
    Sound::PlayBank0(0xbc, p->mCamSpacePos);
    Sound::PlayCharVoice(p->mCharacter, 0x2f, p->mCamSpacePos);
    off.x = 0;
    off.y = 0x1e000;
    off.z = 0x64000;
    Vec3_RotateYAndTranslate(&v, &p->mPos, p->mAngleY, &off);
    Particle::System::NewSimple(0xfe, v.x, v.y, v.z);
    Particle::System::NewSimple(0xff, v.x, v.y, v.z);
}
}

// @symbol func_ov002_020d2f24
extern "C" {
void func_ov002_020d2f24(Player *player) {

    int v;
    int r4;
    u32 id;

    id = player->param1;
    if (id != 3) return;

    r4 = player->mSleepBubbleScaleX;
    id = player->GetBodyModelID(id & 0xff, 0);
    {
        /* q is the frame controller; m->currFrame would fold the split +0x50. */
        int *q = (int *)((int)player->mModels.mBodyModels[id] + 0x50);
        v = (u32)(((dExtFrameCtrl_c *)q)->currFrame << 4) >> 0x10;
    }

    if (v >= 6) {
        if (v <= 0x1c) {
            r4 -= 0x180;
            goto store;
        }
    }
    if (v >= 0x24 && v <= 0x3a) {
        if (r4 < 0x2800) r4 += 0x180;
        else r4 += 0x300;
        goto store;
    }
    if (v == 0x3b) {
        if (player->mStateArg != 0)
            r4 += 0x10;
    }

store:
    if (r4 < 0x400) r4 = 0x400;
    if (r4 > 0x4000) r4 = 0x4000;
    player->mSleepBubbleScaleX = r4;
    player->mSleepBubbleScaleY = r4;
    player->mSleepBubbleScaleZ = r4;
}
}

// @symbol func_ov002_020d2fdc
extern "C" {
int func_ov002_020d2fdc(Player *p) {
    typedef struct { u16 flags; char pad[22]; } Entry24;
    extern Entry24 data_0209f49c[];
    extern u8 data_0209f2d8;
    extern char data_0209f2f8;
    extern void *data_0209f318;
    extern void func_ov002_020d4c30(void *p);

    int r5;
    int id;

    p->mDashBoostTimer = 0;
    if (p->mIsMega != 0) {
        return 0;
    }

    if ((unsigned short)(p->mStateFlags & 0x800)) {
        return 1;
    }

    if (data_0209f49c[gActivePlayerSlot].flags & 0x800) {
        if (p->mStateWork < 0x1e) {
            p->mStateWork += 1;
            r5 = p->mStateWork * 0x111 + 0x1000;
        } else {
            p->mDashBoostTimer = 0x1e;
            r5 = 0x3000;
            if (p->mIsInShallowWater == 0) {
                Particle::RunningSlidingDustAt(p->mPosX, p->mPosY, p->mPosZ);
            }
        }

        p->SetAnim(0x3f, 0, r5, 0);

        id = p->GetBodyModelID(p->param1 & 0xff, 0);
        if (p->mModels.mBodyModels[id]->WillHitFrame(4) ||
            (id = p->GetBodyModelID(p->param1 & 0xff, 0),
             p->mModels.mBodyModels[id]->WillHitFrame(0x13))) {
            func_ov002_020d4c30(p);
        }
        return 0;
    }

    p->mStateWork = 0;
    {
        int flag = 0;
        if (data_0209f2d8 == 1) flag = 1;
        if (flag != 0) {
            p->SetAnim(0x47, 0, 0x1000, 0);
            return 0;
        }
    }

    if (p->mSinkDepth > 0x32000) {
        p->SetAnim(0x7d, 0, 0x1000, 0);
        return 0;
    }

    if (p->mStateStep == 0) {
        p->SetAnim(0x47, 0, 0x1000, 0);
        if (p->mStateTimer == 0) {
            char v = data_0209f2f8;
            if (v == 0x13 || v == 0xa || (unsigned)(v - 0x30) <= 1) {
                p->SetAnim(0x95, 0x40000000, 0x1000, 0);
                p->mStateStep = 5;
            } else {
                p->mStateTimer = 0x708;
                p->SetAnim(0xd, 0x40000000, 0x1000, 0);
                p->mStateStep = 1;
            }
            ((dCamera_c *)data_0209f318)->mFlags |= 0x2000;
        }
    } else {
        p->SetAnim(0xf, 0, 0x1000, 0);
    }
    return 0;
}
}

// @symbol _ZN6Player12St_Wait_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Wait_Init()
{
    extern unsigned char data_0209f2d8;
    extern int data_0209caa0[];

    unsigned char f2d8;
    int b0;

    mStatePhase = 0;
    f2d8 = data_0209f2d8;
    b0 = (f2d8 == 1);
    if (b0 != 0) goto L80;
    if ((data_0209caa0[2] & 0x80) == 0) goto L50;
    {
        int b1 = (f2d8 == 2);
        if (b1 == 0) goto L80;
    }
L50:
    this->SetAnim(0xb3, 0, 0x1000, 0);
    mStateStep = 0;
    return 1;

L80:
    if (mIsMega == 0) goto Ld8;
    if (b0 != 0) goto Lbc;
    if (data_0209caa0[2] & 0x80) goto Lbc;
    this->SetAnim(0xa0, 0, 0x1000, 0);
Lbc:
    mStateTimer = 0x384;
    return 1;

Ld8:
    if (mToxicSurfaceY == 0x80000000)
        goto L13c;
    if (mPosY >= mToxicSurfaceY - 0x64000)
        goto L13c;
    if (mIsMetal != 0) goto L13c;
    if (mIsVanish != 0) goto L13c;
    mStateStep = 0xa;
    this->SetAnim(5, 0, 0x1000, 0);
    return 1;

L13c:
    mStateStep = 0;
    if (mSinkDepth > 0x32000) {
        this->SetAnim(0x7d, 0, 0x1000, 0);
    } else if (this->GetHealth() <= 2) {
        mStateStep = 7;
        this->SetAnim(0xe, 0x40000000, 0x1000, 0);
    } else {
        this->SetAnim(0x47, 0, 0x1000, 0);
    }
    mStateTimer = 0x384;
    return 1;
}
}

// @symbol func_ov002_020d3498
extern "C" {
void func_ov002_020d3498(char* c)
{
    Player *player = (Player *)c;

    player->mHorzSpeed = -player->mHorzSpeed;
    player->mAngleY = player->mAngleY + 0x8000;
    player->mPrevAngleY = player->mAngleY;
}
}

// @symbol _ZN6Player18St_TurnAround_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_TurnAround_Main()
{

    extern Player::State data_ov002_021101e4;
    extern void ApproachAngle(s16* cur, s16 target, int divisor, int band, int maxStep);
    extern Fix12i Player_ScaleByCharFactor(void* c, Fix12i a);
    extern int func_0201226c(int a0, int a1, int a2, int a3, int a4, s16 a5);
    extern void Player_AdvanceAnims(void* c);
    extern u16 data_0209f49e[];
    extern s16 data_0209f4a0[];
    extern Player::State data_ov002_021101b4;
    extern Player::State data_ov002_0211013c;

    int r5v;
    s16 angle;
    int sel;
    int raw;
    int tmp;

    raw = mDesiredAngleY;
    if (*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) != 0) {
        angle = (s16)(raw + 0x8000);
        sel = (mHorzSpeed < 0) ? 0x1800 : 0x8000;
        r5v = func_ov002_020bf56c(((char*)this), sel);
        ApproachAngle((s16*)((char*)&mPrevAngleY), angle, 0x10, 0x1000, 0x200);
        tmp = Player_ScaleByCharFactor(((char*)this), 0x28000);
        ApproachLinear(*((int *)((char *)&mHorzSpeed)), -tmp, r5v);
    } else {
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }

    ApproachAngle((s16*)((char*)&mAngleY), mPrevAngleY, 8, 0x2000, 0x800);

    if ((*(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18) & 2) != 0) {
        this->ChangeState(data_ov002_021101e4);
    } else if (mIsAirborne != 0) {
        this->ChangeState(data_ov002_021101b4);
    } else if (mHorzSpeed <= 0x12000) {
        if (!this->IsAnim(0x45))
            this->SetAnim(0x45, 0x40000000, 0x1000, 0);
        if (this->FinishedAnim()) {
            func_ov002_020d3498(((char*)this));
            this->ChangeState(data_ov002_0211013c);
        }
    } else {
        func_ov002_020bf88c();
        mLoopingSoundHandle = func_0201226c(mLoopingSoundHandle, 0, mGroundSoundType + 0xe2, (int)((char*)&mCamSpacePosX), mHorzSpeed, 0);
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player18St_TurnAround_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_TurnAround_Init()
{

    this->SetAnim(0x46, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    return 1;
}
}

// @symbol func_ov002_020d36d8
extern "C" {
int func_ov002_020d36d8(char* c, int arg)
{
    extern int func_ov002_020dd824(char* c);
    extern int func_ov002_020d5c6c(char* c);
    extern int func_ov002_020dde74(char* c);
    extern int func_ov002_020d674c(char* c);
    extern int func_ov002_020d1204(char* c);
    extern unsigned char data_0209f49e[];
    extern unsigned char data_0209f49c[];
    extern Player::State data_ov002_0211004c;
    extern Player::State data_ov002_021104e4;
    extern Player::State data_ov002_0211019c;
    extern Player::State data_ov002_021101b4;
    Player *player = (Player *)c;

    if ((unsigned short)(player->mStateFlags & 0x40))
        return 0;

    if (((Player *)(c))->func_ov002_020c0434() != 0) {
        ((Player *)(c))->func_ov002_020c0364(3);
        return 1;
    }
    if (func_ov002_020dd824(c) != 0) return 1;
    if (func_ov002_020d5c6c(c) != 0) return 1;

    {
        int idx = gActivePlayerSlot * 0x18;
        unsigned short f49e = *(unsigned short*)(data_0209f49e + idx);
        if (f49e & 1) {
            return func_ov002_020dde74(c) != 0;
        }
        if ((*(unsigned short*)(data_0209f49c + idx) & 0x400) && (arg & 1) == 0) {
            if (func_ov002_020d674c(c) != 0) {
                ((Player *)c)->ChangeState(data_ov002_0211004c);
            } else {
                player->mStateStep = 0;
                ((Player *)c)->ChangeState(data_ov002_021104e4);
            }
            return 1;
        }
        if (f49e & 2) {
            if (player->mCrouchTimer == 0) {
                ((Player *)c)->ChangeState(data_ov002_0211019c);
                return 1;
            }
            if (func_ov002_020d1204(c) != 0) return 1;
        }
    }

    if (player->mIsAirborne != 0) {
        if (player->mPosY - player->mGroundY >= 0x28000) {
            ((Player *)c)->ChangeState(data_ov002_021101b4);
            return 1;
        }
    }
    return 0;
}
}

// @symbol _ZN6Player12St_Walk_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Walk_Main()
{
    extern void func_ov002_020d45c0(void*);
    extern int func_ov002_020d3b9c(void*);
    extern void func_ov002_020d413c(void*, short);
    extern int func_ov002_020c5244(void);
    extern int func_ov002_020d36d8(void* c, int a);
    extern void ApproachAngle(short* cur, short target, int divisor, int band, int maxStep);
    extern int func_0201226c(int a0, int a1, int a2, int a3, int a4, short a5);
    extern void func_ov002_020d4540(void* c);
    extern void func_ov002_020cabe0(void* c);
    extern void Player_AdvanceAnims(void* c);
    extern u8 data_0209f4ae[];
    extern s16 data_0209f4a0[];
    extern Player::State data_ov002_02110154;

    if ((u16)(mStateFlags & 0x800)) {
        this->ChangeState(data_ov002_02110154);
        return 1;
    }
    if (func_ov002_020c5244()) return 1;
    if (func_ov002_020d36d8(((char*)this), 0)) return 1;

    func_ov002_020d45c0(((char*)this));
    short r5 = mPrevAngleY;
    if (func_ov002_020d3b9c(((char*)this))) return 1;

    func_ov002_020d413c(((char*)this), r5);

    if (mHorzSpeed == 0) {
        mAngleY = mPrevAngleY;
    } else {
        ApproachAngle((short*)((char*)&mAngleY), mPrevAngleY, 4, 0x2000, 0x800);
    }

    if (mIsBraking != 0) {
        if (!this->IsAnim(0x37)) {
            int result = func_0201226c(mLoopingSoundHandle, 0, mGroundSoundType + 0xe2, (int)((char*)&mCamSpacePosX), mHorzSpeed, 0);
            mLoopingSoundHandle = result;
        }
        if (this->FinishedAnim()) {
            if (this->IsAnim(0x36)) {
                if (mHorzSpeed == 0) {
                    this->SetAnim(0x37, 0x40000000, 0x1000, 0);
                }
            } else {
                if (this->IsAnim(0x37)) {
                    func_ov002_020d4540(((char*)this));
                }
            }
        }
        goto end;
    }

    if (mHorzSpeed != 0) goto label_29c;

    {
        u8 idx = gActivePlayerSlot;
        int off = idx * 0x18;
        u8 b = data_0209f4ae[off];
        int thresh = (b != 2) ? 0x471 : 0x555;
        u8 v710 = mIsWalkPace;
        int rhs = thresh - (v710 << 7);
        short lhs = *(s16*)((char*)data_0209f4a0 + off);
        if (lhs < rhs) goto label_1f0;
        if ((u16)(mStateFlags & 0xc) == 0) goto label_29c;
    }

label_1f0:
    if (mIsMega != 0) {
        if (this->FinishedAnim() && this->IsAnim(0x9d)) {
            this->ChangeState(data_ov002_02110154);
            mStateWork = 1;
            func_ov002_020cabe0(((char*)this));
        }
        goto end;
    }

    if (this->FinishedAnim()) goto label_27c;

    {
        u32 arg = (u8)param1;
        int modelIdx = this->GetBodyModelID(arg, 0);
        if (mModels.mBodyModels[modelIdx]->GetFlags())
            goto end;
    }

label_27c:
    this->ChangeState(data_ov002_02110154);
    mStateWork = 1;
    func_ov002_020cabe0(((char*)this));
    goto end;

label_29c:
    func_ov002_020d4748();
    mStateWork = 1;

end:
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol func_ov002_020d3b9c
/* recovered: Player walk state helper (between St_Walk_Main and St_Walk_Init): steering and turn approach. */
extern "C" {
static inline s16 GetStick(void)
{
    extern short data_0209f4a0[];
    return *((s16 *) (((char *) data_0209f4a0) + gActivePlayerSlot * 0x18));
}
static inline u8 GetMode(void)
{
    extern unsigned char data_0209f4ae[];
    return *((u8 *) (data_0209f4ae + gActivePlayerSlot * 0x18));
}
static inline u8 GetB14(void)
{
    extern unsigned char data_0209f4ac[];
    return *((u8 *) (data_0209f4ac + gActivePlayerSlot * 0x18));
}
static inline int StickLo(void) { return (GetMode() != 2) ? 0x471 : 0x555; }
static inline int StickHi(void) { return (GetMode() != 2) ? 0x955 : 0xAAA; }
static inline int StickScale(void) { return (GetMode() != 2) ? 0x24 : 0x30; }
static inline int StickLoSpd(void) { return (GetMode() != 2) ? 0xa : 0x10; }
static inline int StickHiSpd(void) { return (GetMode() != 2) ? 0x15 : 0x20; }
int func_ov002_020d3b9c(char *c)
{
    extern int Player_ScaleByCharFactor(void *c, int a);
    extern int func_ov002_020bf224(int c, int a, int b);
    extern int func_ov002_020bf56c(void *c, int b);
    extern int func_ov002_020f035c(unsigned int sel, int r1);
    extern int AngleDiff(int a, int b);
    extern int __aeabi_idiv(int a, int b);
    extern int ApproachAngle(short *cur, short target, int divisor, int band, int maxStep);
    extern void Player_AdvanceAnims(void *c);
    extern void func_ov002_020d4540(void *c);
    extern void func_ov002_020d454c(void *c);
    extern int func_ov002_020d4d88(void *c, int a, int b);
    extern void func_ov002_020c18b0(void *c, int a);
    extern short data_0209f4a0[];
    extern char data_0209f49c[];
    extern unsigned char data_0209f4ac[];
    extern unsigned char data_0209f4ae[];
    extern Player::State data_ov002_02110184;
    Player *player = (Player *)c;
    int spd;
    int acc;
    s16 stick;
    s16 targ;
    int diff;
    spd = 0;
    stick = *((s16 *)(((char *)data_0209f4a0) + (gActivePlayerSlot * 0x18)));
    acc = spd;
    if ((stick != 0) && (((u16)(player->mStateFlags & 0xc)) == 0)) {
        player->mStickGraceTimer = 4;
        if ((*((u8 *)(data_0209f4ac + (gActivePlayerSlot * 0x18)))) == 0) {
            spd = Player_ScaleByCharFactor(c, 0x14000);
            acc = 0x1000;
            if (((*((u16 *)(data_0209f49c + (gActivePlayerSlot * 0x18)))) & 0x800) == 0) {
                if (player->mStateTimer != 0) {
                    spd = Player_ScaleByCharFactor(c, 0xa000);
                }
            } else {
                spd = Player_ScaleByCharFactor(c, 0x28000);
            }
        } else {
            if (GetStick() <= StickLo()) {
                spd = 0;
            } else if (GetStick() <= StickHi()) {
                int t = (GetStick() * StickScale()) >> 12;
                spd = (t - StickLoSpd()) << 15;
                spd = spd / (StickHiSpd() - StickLoSpd());
            } else {
                int a = Player_ScaleByCharFactor(c, 0x28000);
                int b = Player_ScaleByCharFactor(c, 0xa000);
                spd = func_ov002_020bf224((int)c, a, b);
            }
            acc = 0x1000;
            if ((*((s16 *)(((char *)data_0209f4a0) + (gActivePlayerSlot * 0x18)))) >= 0xf00) {
                acc = 0x1000;
            }
        }
        if (player->mDashBoostTimer != 0) {
            if (((*((u16 *)(data_0209f49c + (gActivePlayerSlot * 0x18)))) & 0x800) != 0) {
                u8 *p = &player->mDashBoostTimer;
                *p = (u8)((*p) - 1);
                spd = FX_MUL(spd, 0x1333);
                acc = 0x1e000;
                ((Player *)c)->func_ov002_020bf88c();
            } else {
                player->mDashBoostTimer = 0;
            }
        }
        {
            int v = player->mSinkDepth;
            if (v > 0xa000) {
                int q = 0x6400 / (v >> 12);
                spd = FX_MUL(spd, q);
            }
        }
        player->mModelOffsetY = 0;
        if ((player->param1) == 1) {
            u16 h = player->mWaterRunTimer;
            if (((h != 0) && (player->mWaterRunLatch != 0)) && (h < 0x1e)) {
                int a = Player_ScaleByCharFactor(c, 0x28000);
                int prod = h * a;
                {
                    int cap = prod / 30;
                    if (spd > cap) {
                        spd = cap;
                    }
                }
                {
                    int tmp = -((0x1e - ((int)h)) << 12);
                    tmp = tmp * 2;
                    player->mModelOffsetY = tmp;
                }
            }
        }
        if (func_ov002_020f035c(((Player *)c)->func_ov002_020c031c(), player->mFloorNormalY) != 0) {
            acc = 0x1000;
        }
        targ = player->mDesiredAngleY;
        diff = AngleDiff(player->mPrevAngleY, targ);
        if (diff < 0) {
            diff = -diff;
        }
        if (diff > 0x6000) {
            int cur = player->mHorzSpeed;
            if ((cur >= Player_ScaleByCharFactor(c, 0x1c000)) && (player->mIsMega == 0)) {
                ((Player *)c)->ChangeState(data_ov002_02110184);
                Player_AdvanceAnims(c);
                return 1;
            }
            if (cur == 0) {
                player->mPrevAngleY = targ;
            } else {
                ApproachAngle(&player->mPrevAngleY, targ, 4, 0x2000, 0x800);
            }
        } else {
            int dv;
            int step;
            int idx2;
            step = 0x800;
            idx2 = gActivePlayerSlot * 0x18;
            dv = 4;
            if ((*((u8 *)(data_0209f4ac + idx2))) == 0) {
                if ((*((s16 *)(((char *)data_0209f4a0) + idx2))) >= 0xa00) {
                    if (AngleDiff(player->mPrevAngleY, targ) >= 0x1000) {
                        if (player->mTurnRamp < 0x18) {
                            player->mTurnRamp++;
                            dv = (s16)(0x10 - (player->mTurnRamp >> 1));
                        }
                    } else {
                        dv = 0x10;
                        if (player->mTurnRamp >= 2) {
                            player->mTurnRamp -= 2;
                        }
                    }
                    step = (s16)((0x10 - (dv - 4)) << 3);
                } else if (player->mTurnRamp >= 2) {
                    player->mTurnRamp -= 2;
                }
            }
            ApproachAngle(&player->mPrevAngleY, targ, dv, 0x2000, step);
        }
        if (player->mIsBraking != 0) {
            func_ov002_020d4540(c);
        }
    } else {
        player->mDashBoostTimer = 0;
        if (player->mIsBraking != 0) {
            acc = func_ov002_020bf56c(c, 0x2000);
            ((Player *)c)->func_ov002_020bf88c();
        } else if (player->mStickGraceTimer == 0) {
            if (player->mIsMega == 0)
                acc = 0x1000;
            else
                acc = 0x4000;
            if (player->mHorzSpeed < (-0xa000)) {
                player->mHorzSpeed = -0xa000;
            }
            if (player->mIsMega == 0) {
                if (player->mHorzSpeed > Player_ScaleByCharFactor(c, 0x10000)) {
                    func_ov002_020d454c(c);
                }
            }
        }
        if (player->mHorzSpeed < Player_ScaleByCharFactor(c, 0xa000)) {
            player->mStateTimer = 0x3c;
        }
    }
  func_ov002_020d4d88(c, spd, acc);
  func_ov002_020c18b0(c, 1);
  player->mAngleZ = 0;
  return 0;
}
}

// @symbol func_ov002_020d413c
extern "C" {
void func_ov002_020d413c(char *self, int arg)
{
    extern int Player_ScaleByCharFactor(void *c, int a);
    extern int ApproachAngle(short *angle, int targetAngle, int invFactor, int maxDelta, int minDelta);
    Player *p = (Player *)self;
    int v;
    int diff;
    short s;
    if (p->mIsMega)
        return;
    v = Player_ScaleByCharFactor(self, 0x1c000);
    diff = p->mHorzSpeed;
    if (diff < v) {
        p->mSpineRot[1] = 0;
        p->mSpineRot[2] = p->mSpineRot[1];
        return;
    }
    s = (short)(FX_MUL(((diff - v) / 12), 0x1c71));
    {
        int t = s;
        if (t >= 0x2000) t = 0x2000;
        ApproachAngle(&p->mSpineRot[2], t, 4, 0x2000, 0x100);
    }
    {
        int t = (short)((arg - p->mPrevAngleY) * 6);
        if (t < -0x1555) t = -0x1555;
        else if (t > 0x1555) t = 0x1555;
        ApproachAngle(&p->mSpineRot[1], t, 8, 0x100, 0x10);
    }
}
}

// @symbol _ZN6Player12St_Walk_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Walk_Init()
{
    extern int Player_ScaleByCharFactor(void *c, int a);
    extern void func_ov002_020d4540(void *p);
    extern void func_ov002_020caf68(void *self);
    extern int func_ov002_020e3078(void *self, void *s);
    extern int RandomIntInternal(int *seed);
    extern short data_0209f4a0;
    extern unsigned char data_0209f2d8;
    extern int data_0209caa0[3];
    extern unsigned char data_0209f4ac;
    extern Player::State data_ov002_0211010c;
    extern int data_ov002_02110154[];
    extern int data_ov002_0210e160[];
    extern int data_ov002_020ff1b0[4];
    if (this->GetHealth() == 0) {
        mStateStep = 1;
        this->ChangeState(data_ov002_0211010c);
        return 1;
    }

    if (mHorzSpeed > Player_ScaleByCharFactor(((char *)this), 0x28000)) {
        mHorzSpeed = Player_ScaleByCharFactor(((char *)this), 0x28000);
    }
    if (mHorzSpeed < -0xe000) {
        mHorzSpeed = -0xe000;
    }

    if (*(short *)((char *)&data_0209f4a0 + gActivePlayerSlot * 0x18) == 0) {
        int byte1 = data_0209f2d8;
        int cond = (byte1 == 1);
        if (cond) goto do_anim;
        if ((data_0209caa0[2] & 0x80) == 0) goto merge;
        cond = (byte1 == 2);
        if (cond) goto merge;
do_anim:
        if (mIsMega != 0) {
            this->SetAnim(0xa0, 0, 0x1000, 0);
        } else {
            this->SetAnim(0x47, 0, 0x1000, 0);
        }
    } else {
        func_ov002_020d4748();
    }
merge:
    mStateTimer = 0;
    if (*(unsigned char *)((char *)&data_0209f4ac + gActivePlayerSlot * 0x18) == 0) {
        if (mHorzSpeed == 0) {
            mStateTimer = 0x3c;
        }
    }

    mPeakY = mPosY;
    mStateWork = 0;
    mStateArg = 0;
    if (mHorzSpeed > Player_ScaleByCharFactor(((char *)this), 0x24000)) {
        mStateArg = 1;
    }
    *(unsigned short *)((char *)&mStateFlags) &= ~0x100;
    mIsUnderwater = 0;
    func_ov002_020d4540(((char *)this));

    func_ov002_020caf68(((char *)this));

    mRidingShell = 0;
    mHeldObj = 0;
    mGrabbedByActor = 0;
    mStateStep = 0;
    mEatingPlayer = 0;
    if (func_ov002_020e3078(((char *)this), data_ov002_02110154) != 0) {
        mWalkTimer = 0x10;
        if (*(short *)((char *)&data_0209f4a0 + gActivePlayerSlot * 0x18) != 0) {
            mPrevAngleY = mDesiredAngleY;
            mAngleY = mPrevAngleY;
        }
        if (mDashBoostTimer != 0) {
            unsigned int rnd = (unsigned int)RandomIntInternal(data_ov002_0210e160);
            int idx = (rnd >> 6) & 3;
            int voiceId = data_ov002_020ff1b0[idx];
            Sound::PlayCharVoice(mCharacter, voiceId, mCamSpacePos);
        }
    }

    if (param1 == 1) {
        if (mIsInShallowWater == 0) {
            mDetectWater = 1;
        }
    }

    if (mIsMega != 0 && mScaleX != 0x3000) {
        this->SetNoControlState(0xd, -1, 0);
    }
    return 1;
}
}

// @symbol func_ov002_020d4540
extern "C" {
void func_ov002_020d4540(Player *p) {
    p->mIsBraking = 0;
}
}

// @symbol func_ov002_020d454c
extern "C" {
void func_ov002_020d454c(Player *player) {
    extern int Player_ScaleByCharFactor(void *player, int a);
    if (player->mIsAirborne != 0)
        return;
    if (player->mHorzSpeed > Player_ScaleByCharFactor(player, 0x10000))
        player->mHorzSpeed = Player_ScaleByCharFactor(player, 0x10000);
    player->mIsBraking = 1;
    player->SetAnim(0x36, 0x40000000, 0x1000, 0);
}
}

// @symbol func_ov002_020d45c0
extern "C" {
void func_ov002_020d45c0(char* self)
{
    extern int Player_ScaleByCharFactor(void* c, int a);
    extern void func_02022b04(int x, int y, int z);
    extern int data_0208e430;
    extern s16 data_0209f4a0[];
    extern u8 data_0209f4ac[];
    extern u16 data_0209f49c[];
    Player *p = (Player *)self;
    if (p->mIsInShallowWater != 0)
        return;
    if (data_0208e430 == 0x2e) return;

    if (p->mHorzSpeed < Player_ScaleByCharFactor(self, 0x24000)) {
        if (*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) >= 0xe00) {
            if (*(u8*)((char*)data_0209f4ac + gActivePlayerSlot * 0x18) != 0
                || (*(u16*)((char*)data_0209f49c + gActivePlayerSlot * 0x18) & 0x800)) {
                if (p->mIsMega != 0)
                    func_02022b04(p->mPosX, p->mPosY, p->mPosZ);
                else
                    Particle::RunningSlidingDustAt(p->mPosX, p->mPosY, p->mPosZ);
            }
        }
    }

    if (*(u8*)((char*)data_0209f4ac + gActivePlayerSlot * 0x18) == 0) return;

    if (p->mStateArg != 0) {
        if (p->mStateWaitTimer != 0) {
            if (p->mIsMega != 0)
                func_02022b04(p->mPosX, p->mPosY, p->mPosZ);
            else
                Particle::RunningSlidingDustAt(p->mPosX, p->mPosY, p->mPosZ);
        }
        if (p->mHorzSpeed < Player_ScaleByCharFactor(self, 0x1c000))
            p->mStateArg = 0;
    } else {
        if (p->mHorzSpeed > Player_ScaleByCharFactor(self, 0x26000)) {
            p->mStateWaitTimer = 0x10;
            p->mStateArg = 1;
        }
    }
}
}

// @symbol _ZN6Player19func_ov002_020d4748Ev
/* local extern: the call passes an untyped this, so it cannot use the header method. */
extern "C" {
static inline int* GetAnim(char* c)
{
    Player *player = (Player *)c;
    return (
        int *)((char *)player->mModels
                   .mBodyModels[((Player *)c)->GetBodyModelID((unsigned char)player->param1, 0)] +
               0x50);
}
static inline u16 AnimSpeed(char *c) {
    return GetAnim(c)[2] >> 12;
}
void Player::func_ov002_020d4748()
{
    int Player_ScaleByCharFactor(void*, int);
    void func_ov002_020d4c30(void*);
    extern unsigned char data_0209f4ac;
    extern unsigned short data_0209f49c;
    extern unsigned char data_0209f4ae;
    extern short data_0209f4a0;
    extern unsigned char data_0209f2d8;
    extern int data_0209caa0[];
    char* c = (char*)this;
    int i = gActivePlayerSlot * 0x18;
    int flag = 0;

    if (*(unsigned char*)((char*)&data_0209f4ac + i) == 0) {
        if (!(*(unsigned short*)((char*)&data_0209f49c + i) & 0x800))
            flag = 1;
    } else {
        int k = (*(unsigned char*)((char*)&data_0209f4ae + i) != 2) ? 0xdc7 : 0xe00;
        if (*(short *)((char *)&data_0209f4a0 + i) < k + (this->mIsWalkPace << 7))
            flag = 1;
    }
    this->mIsWalkPace = flag;

    if (this->mIsMega != 0) {
        int i2;
        short v;
        this->func_ov002_020bf5e0();
        if (this->IsAnim(0x9f) != 0)
            return;
        i2 = gActivePlayerSlot * 0x18;
        v = *(short*)((char*)&data_0209f4a0 + i2);
        if (v != 0) {
            if (flag != 0) {
                int spd;
                if (*(unsigned char*)((char*)&data_0209f4ac + i2) == 0) {
                    spd = (this->mStateTimer != 0) ? 0x1000 : 0x2000;
                } else {
                    spd = v * 3;
                }
                ((Player *)c)->SetAnim(0x9e, 0, spd, 0);
                return;
            } else {
                ((Player *)c)->SetAnim(0xa4, 0, 0x2000, 0);
                return;
            }
        } else {
            ((Player *)c)->SetAnim(0x9d, 0x40000000, 0x1000, 0);
            return;
        }
    } else {
        int i3 = gActivePlayerSlot * 0x18;
        if (*(unsigned char*)((char*)&data_0209f4ac + i3) != 0) {
            unsigned char* p = &this->mStateStep;
            unsigned char d;
            int b;
            int i4;
            int thr;
            *p = *p + 1;
            i4 = gActivePlayerSlot * 0x18;
            thr = (*(unsigned char*)((char*)&data_0209f4ae + i4) != 2) ? 0x955 : 0xaaa;
            if (*(short*)((char*)&data_0209f4a0 + i4) > thr)
                this->mStateStep = 0;
            d = data_0209f2d8;
            b = (d == 1);
            if (b == false) {
                if (data_0209caa0[2] & 0x80) {
                    int b2 = (d == 2);
                    if (b2 == false) goto after6e3;
                }
                this->mStateStep = 0;
            }
        after6e3:;
        } else {
            this->mStateStep = 0;
        }

        if (this->mSinkDepth > 0x32000) {
            ((Player *)c)
                ->SetAnim(0x7e, 0, *(short *)((char *)&data_0209f4a0 + gActivePlayerSlot * 0x18),
                          AnimSpeed(c));
        } else if (this->mStateStep >= 0x17) {
            this->mStateStep = 0x17;
            ((Player *)c)->SetAnim(0x61, 0, 0x1000, AnimSpeed(c));
        } else if (this->mFloorClass == 4 && *(short*)((char*)&data_0209f4a0 + gActivePlayerSlot * 0x18) == 0) {
            Particle::RunningSlidingDustAt(this->mPosX, this->mPosY, this->mPosZ);
            ((Player *)c)->SetAnim(0x47, 0, 0x1000, 0);
        } else if (this->mIsWalkPace != 0) {
            u16 s5 = AnimSpeed(c);
            int spd = this->mHorzSpeed >> 3;
            if (spd < 0x400)
                spd = 0x400;
            ((Player *)c)->SetAnim(0x48, 0, spd, s5);
        } else {
            int t = 0x1000 - this->mFloorNormalY;
            int f = (int)((((long long)t << 10) + 0x800) >> 12);
            int v;
            int diff = Player_ScaleByCharFactor(c, 0x1c000) - this->mHorzSpeed;
            v = FX_MUL(diff, f);
            if (v < 0)
                v = 0;
            ((Player *)c)->SetAnim(0x3f, 0, v + 0x2800, AnimSpeed(c));
        }

        if (((dExtFrameCtrl_c *)(GetAnim(c)))->WillHitFrame(4) != 0 ||
            ((dExtFrameCtrl_c *)(GetAnim(c)))->WillHitFrame(0x13) != 0) {
            func_ov002_020d4c30(c);
        }
    }
}
}

// @symbol func_ov002_020d4c30
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" {
extern "C" void func_ov002_020d4c30(Player *self) {
    extern void func_0201251c(int a, int b, int c, int d);
    extern int data_0209f32c;
    extern void PlayerNewSimpleHold10(unsigned int id, Fix12i a, Fix12i b, Fix12i c);
    extern int data_0208e430;
    extern unsigned int func_02022d00(unsigned int, unsigned int, Fix12i, Fix12i, Fix12i, void*);
    int id;
    if (self->mIsInShallowWater) {
        self->mParticle1 =
            func_02022d00(self->mParticle1, 0xde, self->mPosX, data_0209f32c, self->mPosZ, 0);
    }
    if (self->mSinkDepth > 0x32000) {
        Sound::PlayBank0(0xb3, self->mCamSpacePos);
    } else if (self->mStateStep >= 0x1e) {
        if (self->mIsMetal == 0) {
            Sound::PlayBank0(self->mGroundSoundType + 0x70, self->mCamSpacePos);
        } else {
            Sound::PlayBank0(0xa4, self->mCamSpacePos);
        }
    } else {
        if (self->mIsMetal == 0) {
            func_0201251c(0, self->mGroundSoundType + 0x20, (int)&self->mCamSpacePos,
                          self->mHorzSpeed);
        } else {
            func_0201251c(0, 0xaa, (int)&self->mCamSpacePos, self->mHorzSpeed);
        }
    }
    if (data_0208e430 == 0x2e) return;
    id = -1;
    if (self->mGroundSoundType != 6) {
        if (self->mGroundSoundType != 8) {
            if (self->mGroundSoundType == 9)
                id = 0xe6;
        } else
            id = 0xe4;
    } else
        id = 0xe5;
    if (id == -1) return;
    Particle::System::NewSimple(id, self->mPosX, self->mPosY, self->mPosZ);
}
}

// @symbol func_ov002_020d4d88
/* func_ov002_020d4d88 at 0x020d4d88
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
extern "C" {
int func_ov002_020d4d88(Player *player, int a, int b, int c) {
    ApproachLinear(player->mHorzSpeed, a, b);
    if (a != 0 || (unsigned short)(player->mStateFlags & 1)) {
        player->func_ov002_020c04e0();
        return 1;
    }
    return 0;
}
}

// @symbol _ZN6Player12St_Bonk_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Bonk_Main()
{
    struct dCamera_c;
    extern void Player_AdvanceAnims(void* c);
    extern void func_0200d8c8(struct dCamera_c * cam, const Vector3 *v, int strength);
    extern struct dCamera_c* data_0209f318;
    extern Player::State data_ov002_0211013c;
    int rate;

    if (mStateTimer != 0) {
        mHorzSpeed = 0x40000;
        mVertSpeed = 0x8000;
        goto ret1;
    }

    rate = 0x1000;
    if (mIsAirborne == 0) {
        int v = mPrevVertSpeed;
        if (v < 0) {
            mVertSpeed = (-v) / 3;
            if (mVertSpeed <= 0x1000) {
                mVertSpeed = 0;
            }
        }

        if (mStateArg == 0) {
            Sound::PlayBank0(mGroundSoundType + 0x50, mCamSpacePos);
            Sound::PlayCharVoice(mCharacter, 0x11, mCamSpacePos);
            mStateArg = 1;
        }

        if (mPeakY - mPosY > 0xbb8000) {
            func_0200d8c8(data_0209f318, &mPos, 0x7d0000);
        }

        rate = 0x1800;
    }

    ApproachLinear(*((int *)((char *)&mHorzSpeed)), 0, rate);

    if (mHorzSpeed != 0) {
        goto willhit;
    }
    if (mIsAirborne == 0) {
        goto finishedanim;
    }
willhit:
    {
        u32 arg = (u8)param1;
        int modelIdx = this->GetBodyModelID(arg, 0);
        if (mModels.mBodyModels[modelIdx]->WillHitFrame(0x2e) != 0) {
            mStateWork = 1;
        }
    }
    goto tail;
finishedanim:
    mStateWork = 0;
    if (this->FinishedAnim() != 0) {
        mPrevAngleY = mPrevAngleY + 0x8000;
        mAngleY = mPrevAngleY;
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }
tail:
    if (mStateWork == 0) {
        Player_AdvanceAnims(((char*)this));
    }
    mPrevVertSpeed = mVertSpeed;
ret1:
    return 1;
}
}

// @symbol _ZN6Player12St_Bonk_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Bonk_Init()
{
    this->SetAnim(0x13, 0x40000000, 0x1000, 0);
    mStateTimer = 0x10;
    mHorzSpeed = 0x40;
    mVertSpeed = 0xa000;
    mStateWork = 0;
    return 1;
}
}

// @symbol _ZN6Player8BlowAwayEs
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
void Player::BlowAway(short v)
{

    extern Player::State data_ov002_0211037c;
  if(mIsMetal) return;
  if(mIsBalloon) return;
  mPrevAngleY=v;
  mAngleY=(short)(v+0x8000);
  this->ChangeState(data_ov002_0211037c);
}
}

// @symbol _ZN6Player16St_BurnLava_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Burn-in-lava, per frame: kills the player once outside battle levels, spawns
 * smoke/steam particles with the burn SFX, then runs the three-hop bounce
 * (halving horizontal speed each hop, hop height 0x14000 then 0xa000) until
 * the burnt animation finishes, and leaves via state change or death.
 */
/* current level id */
/* sin/cos pair table, indexed by angle >> 4 */
/* the walk state */
/* Particle::System::New takes Fix12<int> by value -- the
   mwccarm 6az wall, runbook section 7 -- so it stays extern "C" with scalars
   in those slots. */
extern "C" {
int Player::St_BurnLava_Main()
{
    extern u8 data_ov002_0211117c;
    extern s8 data_0209f2f8;
    extern s16 data_02082214[];
    extern Player::State data_ov002_0211013c;
    extern void KillPlayer(void);
    extern u32 PlayerNewHold7(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                              void *callback);

    extern void func_ov002_020e28d4(void *, int, int);
    extern void Player_AdvanceAnims(void *);
    if (data_ov002_0211117c == 0 && mStateArg == 0) {
        KillPlayer();
        mStateArg = 1;
        mIsBodyClsnEnabled = 0;
    }

    {
        s8 lv = data_0209f2f8;
        if (lv != 0x13 && lv != 0x31 && mVertSpeed >= 0) {
            /* Field order is the stack layout the original built, not a choice. */
            struct Info {
                s16 vec[3];
                int pos[3];
            } info;
            int zz = mPosZ;
            int yy = mPosY;
            int xx = mPosX;
            info.pos[0] = xx;
            info.pos[2] = zz - 0x1e000;
            info.pos[1] = yy;
            /* The u16 cast is the semantics, not a missing type fix. mAngleY
               really is signed -- AngleDiff takes it signed and mPrevAngleY is
               assigned from it directly -- but indexing this table wants the
               angle to WRAP, so the shift must be logical. Spelling it
               `mAngleY >> 4` costs 2 words and changes the meaning for
               negative angles. */
            info.vec[0] = data_02082214[(*(u16 *)&mAngleY >> 4) * 2];
            info.vec[1] = 0;
            info.vec[2] = data_02082214[(*(u16 *)&mAngleY >> 4) * 2 + 1];
            mParticle1 = Particle::System::New(mParticle1, 0xbd, info.pos[0], info.pos[1],
                                               info.pos[2], (const Vector3_16f *)(info.vec), 0);
            mParticle2 = Particle::System::New(mParticle2, 0xbe, info.pos[0], info.pos[1],
                                               info.pos[2], (const Vector3_16f *)(info.vec), 0);
            mLoopingSoundHandle =
                Sound::PlayLong(mLoopingSoundHandle, 0, 0x106, *(Vector3 *)&mCamSpacePosX, 0);
        }
    }

    if (mStateStep == 0) {
        func_ov002_020e28d4(this, 0x1000, 0x1000);
        if (mIsAirborne == 0) {
            mStateWork++;
            if (mStateWork >= 3) {
                this->SetAnim(0x17, 0x40000000, 0x1000, 0);
                mHorzSpeed = 0;
                mStateStep++;
            } else {
                mHorzSpeed >>= 1;
                mIsAirborne = 1;
                mLandSoundPlayed = 0;
                if (mStateWork == 1)
                    mVertSpeed = 0x14000;
                else
                    mVertSpeed = 0xa000;
            }
        }
    } else {
        if (FinishedAnim() != 0) {
            mPrevAngleY = mAngleY;
            if (GetHealth() == 0)
                func_ov002_020c5dec(2);
            else
                this->ChangeState(data_ov002_0211013c);
        }
    }

    Player_AdvanceAnims(this);
    return 1;
}
}

// @symbol _ZN6Player16St_BurnLava_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_BurnLava_Init()
{
    extern int func_ov002_020d91e0(void*,int,int);
    extern int func_ov002_020e3078(void*,int*);
    extern int data_ov002_021100f4[];
  mIsInAirState=1;
  mJumpComboStage=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  mIsTakingDamage=1;
  this->SetAnim(0x16, 0, 0x1000, 0);
  mHorzSpeed=0;
  mVertSpeed=0x64000;
  mStateWork=0;
  mStateStep=0;
  if(func_ov002_020e3078(((char*)this),data_ov002_021100f4)==0){
    mStateArg=0;
  }
  func_ov002_020d91e0(((char*)this),0x300,1);
  Sound::PlayCharVoice(mCharacter, 0x23, mCamSpacePos);
  return 1;
}
}

// @symbol _ZN6Player19func_ov002_020d5338Ev
extern "C" {
int Player::func_ov002_020d5338(){

    extern Player::State data_ov002_02110304;
    extern Player::State data_ov002_02110484;
    extern Player::State data_ov002_021100f4;

  if (this->IsState(data_ov002_02110304)
      || this->IsState(data_ov002_02110484)
      || this->mIsMetal != 0
      || this->mIsNoControl != 0) return 0;
  this->ChangeState(data_ov002_021100f4);
  return 1;
}
}

// @symbol _ZN6Player16St_BurnFire_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Player burn-in-fire state: early-exits via ChangeState on a couple of
 * flag checks, then (unless blocked by a hazard-level check) spawns two
 * fire/smoke particles plus a burn sound at the player's position offset
 * upward, then runs a small state machine (0/1) driving the burn anim and
 * a homing turn+speed approach before handing off to the common per-frame
 * tail function.
 */
extern "C" {
int Player::St_BurnFire_Main()
{

    extern Player::State data_ov002_0211013c;
    extern s16 data_02082214[];
    extern u16 data_0209f49e[];
    extern s16 data_0209f4a0[];
    extern void func_02012694(unsigned int id, const void *v);
    extern int func_ov002_020d91e0(void *thiz, int damage, int doPre);
    extern u32 PlayerNewHold8(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                              void *callback);
    extern void PlayerPlayLongHold_burnfire(unsigned int handle, unsigned int id, unsigned int vol,
                                            const void *vec, unsigned int flags);
    extern void ApproachAngle(short *cur, short target, int divisor, int band, int maxStep);
    extern void Player_AdvanceAnims(void *c);
    int speed = 0;

    if (mStateWork >= 0xa0 || mIsMetal != 0) {
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }

    if (mIsInShallowWater != 0) {
        func_02012694(3, &mCamSpacePosX);
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }

    {
        u16 lvl = mStateTimer;
        if (lvl == 0x50 || lvl == 0x32 || lvl == 0x14) {
            if (func_ov002_020d91e0(this, 0x100, 1) != 0) {
                func_ov002_020c5dec(2);
                return 1;
            }
        }
    }

    *(u8 *)(&mStateWork) += 2;

    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        int zz = mPosZ - 0x1e000;
        int yy = mPosY + 0x32000;
        int xx = mPosX;

        info.pos[0] = xx;
        info.pos[1] = yy;
        info.pos[2] = zz;
        info.vec[0] = data_02082214[(*(u16 *)&mAngleY >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)&mAngleY >> 4) * 2 + 1];

        mParticle1 = Particle::System::New(mParticle1, 0xbd, info.pos[0], info.pos[1], info.pos[2],
                                           (const Vector3_16f *)(info.vec), 0);
        mParticle2 = Particle::System::New(mParticle2, 0xbe, info.pos[0], info.pos[1], info.pos[2],
                                           (const Vector3_16f *)(info.vec), 0);
        mLoopingSoundHandle =
            Sound::PlayLong(mLoopingSoundHandle, 0, 0x106, *(Vector3 *)&mCamSpacePosX, 0);
    }

    switch (mStateStep) {
    case 0:
        if (this->IsAnim(0x53) != 0) {
            this->mStateWork += 1;
        }
        if (mIsAirborne == 0) {
            mStateStep = 1;
            this->SetAnim(0x3f, 0, 0x1000, 0);
            dExtFrameCtrl_c &anim = *mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)];
            anim.speed = 0x4000;
        }
        break;
    case 1:
        {
            int idx = gActivePlayerSlot * 0x18;
            if ((*(u16 *)((char *)data_0209f49e + idx) & 2) != 0) {
                mStateStep = 0;
                this->SetAnim(0x53, 0, 0x1000, 0);
                mIsAirborne = 1;
                mLandSoundPlayed = 0;
                mVertSpeed = 0x1e000;
                break;
            }
        }
        if (mIsAirborne != 0) {
            mStateStep = 0;
            this->SetAnim(0x54, 0, 0x1000, 0);
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
            break;
        }
        this->SetAnim(0x3f, 0, 0x1000, 0);
        {
            dExtFrameCtrl_c &anim = *mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)];
            anim.speed = 0x4000;
        }
        {
            int idx = gActivePlayerSlot * 0x18;
            speed = 0x28000;
            if (*(s16 *)((char *)data_0209f4a0 + idx) != 0) {
                ApproachAngle(&mAngleY, mDesiredAngleY, 8, 0x4000, 0x10);
            }
            mPrevAngleY = mAngleY;
        }
        break;
    }

    ApproachLinear(mHorzSpeed, speed, 0x1000);
    Player_AdvanceAnims(this);
    return 1;
}
}

// @symbol _ZN6Player16St_BurnFire_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_BurnFire_Init()
{
    mIsTakingDamage = 1;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    this->SetAnim(0x16, 0, 0x1000, 0);
    mStateStep = 0;
    mStateTimer = 0x51;
    /* mFloorClass == 1 is the class SetFloorSurfaceInfo latches on (see the
       CLPS block in Player.h): on it the player is planted on the ground
       instead of being popped upward by the burn. */
    if (mFloorClass != 1) {
        mVertSpeed = 0x1e000;
    } else {
        mPosY = mGroundY;
    }
    mHorzSpeed = 0;
    mStateWork = 0;
    Sound::PlayCharVoice(mCharacter, 0x23, mCamSpacePos);
    return 1;
}
}

// @symbol _ZN6Player4BurnEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::Burn()
{

    extern Player::State data_ov002_021100dc;
    extern int func_ov002_020d82f0(void*);
  if(func_ov002_020d82f0(((char*)this))==0) return 0;
  if(mIsMetal || mIsUnderwater) return 0;
  this->ChangeState(data_ov002_021100dc);
  return 1;
}
}

// @symbol _ZN6Player19St_Electrocute_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Electrocution, per frame: a looping shock sound and three stacked particle
 * systems at the player's position raised 0x32000, then when the state timer
 * expires the two low bits of mStateWork decide where to go -- bit 0 selects
 * the "took damage" path over a plain exit, bit 1 selects the swimming exit
 * over the walking one.
 *
 * The volatile locals and the volatile reads of the particle handles are
 * load-bearing: they pin the order and the memory round-trips the original
 * compiled. They are kept verbatim.
 */
/* Particle::System::New takes Fix12<int> by value -- the mwccarm 6az wall,
   runbook section 7 -- so it stays extern "C" with scalars in those slots. */
/* the walk state */
/* the swim state */
extern "C" {
int Player::St_Electrocute_Main()
{
    extern void PlayerPlayLongHold_electro(u32 a, u32 b, u32 c, Vector3 * v, u32 d);
    extern u32 PlayerNewHold9(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                              void *callback);
    extern void Player_AdvanceAnims(void* self);
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_0211067c;
    Vector3 *pos = &mCamSpacePos;
    volatile s32 x;
    volatile s32 y;
    volatile s32 zvar;
    s32 z;
    s32 yraw;

    mLoopingSoundHandle = Sound::PlayLong(mLoopingSoundHandle, 0, 0x10b, *pos, 0);

    x = mPosX;
    yraw = mPosY;
    y = yraw;
    {
        s32 zraw = mPosZ;
        s32 yadjraw = yraw + 0x32000;
        z = zraw;
        zvar = zraw;
        y = yadjraw;
    }

    mParticle1 = Particle::System::New(*(volatile u32 *)&mParticle1, 0xc1, x, y, z, 0, 0);
    mParticle2 = Particle::System::New(*(volatile u32 *)&mParticle2, 0xc2, x, y, zvar, 0, 0);
    mParticle3 = Particle::System::New(mParticle3, 0xc3, x, y, zvar, 0, 0);

    if (mStateTimer == 0) {
        u8 flags = mStateWork;
        if ((flags & 1) == 0) {
            mInvincibleTimer = 0x24;
            flags = mStateWork;
            if ((flags & 2) == 0) {
                this->ChangeState(data_ov002_0211013c);
            } else {
                this->ChangeState(data_ov002_0211067c);
            }
        } else {
            if ((flags & 2) != 0) {
                func_ov002_020c5dec(8);
            } else {
                mInvincibleTimer = 0x24;
                if (func_ov002_020c5dec(4) == 0) {
                    this->ChangeState(data_ov002_0211013c);
                }
            }
        }
    }

    Player_AdvanceAnims(this);
    return 1;
}
}

// @symbol _ZN6Player19St_Electrocute_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Electrocute_Init()
{
    extern int func_ov002_020da9d4(void*);
  func_ov002_020da9d4(((void*)this));
  *(char*)((char*)&mIsTakingDamage)=1;
  *(short*)((char*)&mStateTimer)=0x1e;
  *(short*)((char*)&mInvincibleTimer)=0x1e;
  *(int*)((char*)&mHorzSpeed)=0;
  *(int*)((char*)&mVertSpeed)=0;
  *(int*)((char*)&mVertAccel)=0;
  this->SetAnim(0x11, 0, 0x1000, 0);
  Sound::PlayCharVoice(*(unsigned char *)((char *)&mCharacter), 7, mCamSpacePos);
  return 1;
}
}

// @symbol _ZN6Player5ShockEj
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::Shock(unsigned int j)
{

    extern Player::State data_ov002_021100c4;
    extern int func_ov002_020d91e0(void* c, int a, int b, int d);
    extern int func_ov002_020d82f0(void*);
  if (func_ov002_020d82f0(((char*)this)) == 0) return 0;
  if (mIsMetal) j = 0;
  mStateWork = 0;
  if (func_ov002_020d91e0(((char*)this), j<<8, 1, 0)) mStateWork = 1;
  if (mIsUnderwater) {
    unsigned char* p = &this->mStateWork;
    *p |= 2;
  }
  this->ChangeState(data_ov002_021100c4);
  return 1;
}
}

// @symbol func_ov002_020d5ab4
#pragma push
#pragma opt_strength_reduction off
extern "C" {
int func_ov002_020d5ab4(Player *p) {
    typedef long long s64;
    extern Player::State data_ov002_02110064;
    extern short data_02082214[];
    Vector3 pos;
    void *spawned;
    short *rp;
    int *pp;
    int a;

    if (p->IsState(data_ov002_02110064) == 0) {
        return 0;
    }

    rp = &((Player *)p->mEatingPlayer)->mAngleX;
    p->mAngleX = rp[0];
    p->mAngleY = rp[1];
    p->mAngleZ = rp[2];
    p->mPrevAngleX = p->mAngleX;
    p->mPrevAngleY = p->mAngleY;
    p->mPrevAngleZ = p->mAngleZ;

    pp = &((Player *)p->mEatingPlayer)->mPosX;
    pos.x = pp[0];
    pos.y = pp[1];
    pos.z = pp[2];

    a = (unsigned short)(short)(*(volatile short *)(&p->mAngleY) + 0x8000) >> 4;
    pos.x = pos.x + FX_MUL(data_02082214[a * 2], 0x64000);
    a = (unsigned short)(short)(*(volatile short *)(&p->mAngleY) + 0x8000) >> 4;
    pos.z = pos.z + FX_MUL(data_02082214[a * 2 + 1], 0x64000);

    p->mPosX = pos.x;
    p->mPosY = pos.y;
    p->mPosZ = pos.z;

    spawned = dActor_c::Spawn(9, 2, pos, 0, p->mAreaId, -1);
    ((daYegg_c *)spawned)->mPlayer = p;
    p->mOpacity = 0;
    p->mStateTimer = 0xd2;
    p->mdCcAcPos_c.flags &= ~2;
    p->mStateStep = 3;
    p->mPrevVertSpeed = 0;
    p->mEatingPlayer = 0;
    return 1;
}
}
#pragma pop

// @symbol func_ov002_020d5c6c
extern "C" {
int func_ov002_020d5c6c(Player *player) {
    extern Player::State data_ov002_02110034;
    extern int func_ov002_020d600c(void* p);
    void* p;
    u16 f = player->mStateFlags & 2;
    if (f == 0)
        return 0;
    p = (void *)player->mObjInMouth;
    if (p != 0) {
        if (func_ov002_020d600c(p))
            goto change;
    }
    *(u16 *)(void *)(int)(&player->mStateFlags) &= ~2;
    return 0;
change:
    player->ChangeState(data_ov002_02110034);
    return 1;
}
}

// @symbol func_ov002_020d5cec
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
extern "C" int func_ov002_020d5cec(Player *player) {
    extern short data_02082214[];
    extern Player::State data_ov002_02110064;
    Vector3 v1;
    Vector3 v2;
    Player *d0;
    short* m;
    Vector3 *p;
    int y1;
    int shifted;

    if (player->IsState(data_ov002_02110064) == 0)
        return 0;
    player->SetAnim(0x56, 0x40000000, 0x1000, 0);

    d0 = (Player *)player->mEatingPlayer;
    m = &d0->mAngleX;
    player->mPrevAngleX = m[0];
    player->mPrevAngleY = m[1];
    player->mPrevAngleZ = m[2];
    player->mAngleY = player->mPrevAngleY;
    player->mHorzSpeed = 0x18000;
    player->mVertSpeed = 0x18000;

    p = &((Player *)player->mEatingPlayer)->mPos;
    v1.x = p->x;
    y1 = p->y;
    v1.y = y1;
    v1.z = p->z;
    v1.y = y1 + 0x32000;

    shifted = (unsigned short)player->mAngleY >> 4;
    v2.x = v1.x + FX_MUL(data_02082214[shifted * 2], 0x50000);
    v2.y = v1.y;
    v2.z = v1.z + FX_MUL(data_02082214[shifted * 2 + 1], 0x50000);

    dBgCh_Lin line;
    line.SetObjAndLine(v1, v2, (dActor_c *)player);
    if (line.DetectClsn()) {
        player->mPosX = v1.x;
        player->mPosY = v1.y;
        player->mPosZ = v1.z;
        player->mHorzSpeed = 0x8000;
    } else {
        player->mPosX = v2.x;
        player->mPosY = v2.y;
        player->mPosZ = v2.z;
    }
    player->mOpacity = 0x1f;
    player->mdCcAcPos_c.flags &= ~2;
    player->mStateStep = 2;
    player->mStateTimer = 4;
    player->mIsTakingDamage = 1;
    player->mEatingPlayer = 0;
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020d5ed0Ev
extern "C" {
int Player::func_ov002_020d5ed0(){

    extern Player::State data_ov002_02110064;
    extern Player::State data_ov002_0211013c;
    if (!this->IsState(data_ov002_02110064)) {
        this->mEatingPlayer = 0;
        this->ChangeState(data_ov002_0211013c);
        return 0;
    }
    this->mStateStep = 1;
    this->mStateTimer = 0x5a;
    this->mOpacity = 0;
    return 1;
}
}

// @symbol func_ov002_020d5f34
extern "C" {
int func_ov002_020d5f34(Player *c, void *st) {
    extern Player::State data_ov002_02110064;
    Player *player = (Player *)c;
    if (player->mEatingPlayer != 0)
        return 0;
    player->mEatingPlayer = (s32)st;
    ((Player *)c)->SetAnim(0x3f, 0, 0x3000, 0);
    player->mStateStep = 0;
    ((Player *)c)->ChangeState(data_ov002_02110064);
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020d5f98EPhS0_
extern "C" {
int Player::func_ov002_020d5f98(unsigned char* a, unsigned char* b){
    extern int RandomIntInternal(int* seed);
    extern Player::State data_ov002_02110064;
    extern int data_ov002_0210e160[];

  if (this->IsState(data_ov002_02110064)
      && this->mStateStep == 3
      && this->mStateWork != 0) {
    *a = ((unsigned)RandomIntInternal(data_ov002_0210e160) >> 4) & 3;
    *b = this->mStateWork;
    return 1;
  }
  return 0;
}
}

// @symbol func_ov002_020d600c
extern "C" {
int func_ov002_020d600c(Player *player) {
    extern Player::State data_ov002_02110064;
    if (player->IsState(data_ov002_02110064)) {
        if (player->mStateStep == 1)
            return 1;
    }
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020d6048Ev
extern "C" {
int Player::func_ov002_020d6048(){

    extern Player::State data_ov002_02110064;

  if(this->IsState(data_ov002_02110064)){
    if(this->mStateStep==3) return 1;
  }
  return 0;
}
}

// @symbol _ZN6Player23St_InYoshiMouth_CleanupEv
/* recovered: named members + shared header, real C++ method
 *
 * ov002, 0x020d6084, size 0x34. Leaving the state Yoshi has you in: two body
 * collision bits are cleared and two markers set, then it returns 1.
 *
 * `return 1` is not decoration here. ChangeState calls mCleanup FIRST, on the
 * state being left, and TESTS the result -- a 0 vetoes the transition. So this
 * handler's return value is what lets the player leave Yoshi's mouth at all.
 *
 * The word at 0x2ec used to be a Player field called mBodyClsnFlags. It is
 * not one: it is the body collider's own `flags` -- dCc_c's, at +0x18
 * -- reached through mdCcAcPos_c now that the member has its
 * real type. Clearing 0x2000 and 0x2 in it is what a cleanup of a carried
 * state looks like. The other two writes are still unnamed offsets -- 0x713
 * and 0x6f5 -- and are left as such rather than guessed at.
 *
 * THE TWO CLEARS DO NOT MERGE. `*p &= ~0x2002` is one instruction shorter and
 * changes the size, so the ROM really does mask twice; measured, not assumed.
 * The `(long long)` round-trip the placeholder body carried was free, and is
 * gone.
 *
 * This file carried the placeholder name func_ov002_020d6084 until the symbol
 * was moved here from ov006. See the commit and include/Player.h.
 */
int Player::St_InYoshiMouth_Cleanup()
{
    unsigned int *p = (unsigned int *)&mdCcAcPos_c.flags;
    *p &= ~0x2000;
    *p &= ~2;
    mIsBodyClsnEnabled = 1;
    mOpacity = 0x1f;
    return 1;
}

// @symbol _ZN6Player20St_InYoshiMouth_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_InYoshiMouth_Main()
{
    extern int func_ov002_020d5cec(char *c);
    extern void Vec3_RotateYAndTranslate(Vector3 *res, Vector3 *trans, short angY, Vector3 *add);
    extern void Player_AdvanceAnims(char *c);
    extern Player::State data_ov002_02110094;
    extern Player::State data_ov002_0211007c;
    extern Player::State data_ov002_0211019c;
    switch (mStateStep) {
    case 0:
        break;
    case 1:
        {
        Player *r4 = (Player *)mEatingPlayer;
        Player *eater = r4;
        u32 t;
        int d;
        if (r4 != 0 && (char *)eater->mObjInMouth != ((char *)this)) {
            if (func_ov002_020d5cec(((char *)this)))
                return 1;
        }
            {
                int *p = &eater->mPosX;
                mPosX = p[0];
                mPosY = p[1];
                mPosZ = p[2];
            }
            d = func_ov002_020beb38();
            t = eater->mMouthHoldTimer;
            if (eater->mIsNoControl == 0 && eater->mIsTakingDamage == 0) {
                t = (u16)(t - d);
                *(u8 *)((char *)&mStatePhase) += d;
                if ((s16)t < 0)
                    t = 0;
            }
            if (t <= 1 && mStatePhase >= 0x1e) {
                Vector3 res;
                Vector3 addv;
                Vector3 hv;
                addv.x = 0;
                addv.y = 0x64000;
                addv.z = 0x64000;
                Vec3_RotateYAndTranslate(&res, &eater->mPos, eater->mAngleY, &addv);
                hv.x = res.x;
                hv.y = res.y;
                hv.z = res.z;
                r4->Hurt(hv, 1, 0, 0, (eater->param1 + 1) & 0xff, 1);
                this->ChangeState(data_ov002_0211019c);
                eater->mObjInMouth = 0;
                mEatingPlayer = 0;
                return 1;
            }
            eater->mMouthHoldTimer = (s16)t;
            break;
        }
    case 2:
        if (mStateTimer == 0)
            *(int *)((char *)&mdCcAcPos_c.flags) |= 0x2000;
        if (this->FinishedAnim()) {
            mStateStep = 3;
            mHurtDamage = 1;
            this->ChangeState(data_ov002_02110094);
        }
        break;
    case 3:
        {
            int d = func_ov002_020beb38();
            *(u16 *)((char *)&mStateTimer) -= (d << 2);
            mStateWork = d;
            if ((s16)mStateTimer <= 0) {
                mStateTimer = 0;
                mOpacity = 0x1f;
                this->ChangeState(data_ov002_0211007c);
            }
        }
        break;
    }
    Player_AdvanceAnims(((char *)this));
    return 1;
}
}

// @symbol _ZN6Player20St_InYoshiMouth_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int Player::St_InYoshiMouth_Init()
{
    unsigned int r3;
    char *slot;
    unsigned int r1;

    func_ov002_020bdb50(0);
    r3 = 0;
    mStateWork = (unsigned char)r3;
    mStatePhase = (unsigned char)r3;
    slot = (char *)((char *)&mdCcAcPos_c.flags);
    r1 = *(unsigned int *)slot;
    r1 |= 2u;
    *(unsigned int *)slot = r1;
    mIsBodyClsnEnabled = (unsigned char)r3;
    return 1;
}

// @symbol func_ov002_020d6334
extern "C" {
int func_ov002_020d6334(Player *o, int val) {
    int i;
    int *arr;
    arr = (int *)o->mHeldObjQueue;
    for (i = 0; i < 5; i++) {
        if (arr[i] == 0) {
            arr[i] = val;
            return 1;
        }
    }
    return 0;
}
}

// @symbol func_ov002_020d6368
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
#pragma push
#pragma opt_strength_reduction off
extern "C" {
void func_ov002_020d6368(char *self)
{
    typedef long long s64;
    extern short data_02082214[];
    extern int func_ov002_020d6334(void*, int);
    extern void func_ov002_020d6368(char*);
    Player *p = (Player *)self;
    volatile Vector3_16 rot;
    Vector3 pos;
    void *spawned;
    int a;
    int zero = 0;
    short rotY;

    pos.x = p->mPosX;
    pos.y = p->mPosY;
    pos.z = p->mPosZ;

    a = *(volatile unsigned short *)(&p->mAngleY) >> 4;
    pos.x = pos.x - FX_MUL(data_02082214[a * 2], 0x14000);

    a = *(volatile unsigned short *)(&p->mAngleY) >> 4;
    *(volatile int *)&pos.y = pos.y - FX_MUL(data_02082214[a * 2 + 1], 0x14000);

    rotY = p->mAngleY;
    rot.x = zero;
    rot.z = zero;
    rot.y = rotY;

    spawned = dActor_c::Spawn(9, p->mEggParams << 4, pos, (const Vector3_16 *)&rot, p->mAreaId, -1);
    ((daYegg_c *)spawned)->mPlayer = (Player *)self;
    func_ov002_020d6334(self, (int)spawned);
    p->mEggParams = zero;
}
}
#pragma pop

// @symbol _ZN6Player15St_Swallow_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Swallow_Main()
{
    extern int func_ov002_020ceaf4(char* c);
    extern void Player_AdvanceAnims(char* c);
    extern int data_0209ee90[];
    extern Player::State data_ov002_0211067c;
    extern Player::State data_ov002_0211013c;
    extern int func_ov002_020d5ab4(void*);
    extern void func_ov002_020d6368(char*);
    dActor_c *p360;

    if (mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(3)) {
        func_ov002_020d71a0();
        goto L65ec;
    }
    if (mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(7) == 0) {
        goto L65ec;
    }

    p360 = *(dActor_c **)((char *)&mObjInMouth);
    if (p360 == 0) goto L65e4;
    {
        int b = (p360->actorID == 0xbf);
        if (b == 0) goto L653c;
    }
    p360->mAngleX = mAngleX;
    p360->mAngleY = mAngleY;
    p360->mAngleZ = mAngleZ;
    func_ov002_020d5ab4(*(void**)((char*)&mObjInMouth));
    Sound::PlayCharVoice(0, 0x100, mCamSpacePos);
    goto L65e4;

L653c:
    p360->OnTurnIntoEgg(*this);
    {
        void* q = *(void**)((char*)&mObjInMouth);
        if (((dActor_c *)q)->OnYoshiTryEat() == 1)
            goto L65e4;
    }
    if ((unsigned short)(mStateFlags & 0x1000)) goto L65e4;
    {
        unsigned char flag = 0;
        int b_c2 = (((dActor_c *)mObjInMouth)->actorID == 0xc2);
        if (b_c2 || *(int*)((char*)data_0209ee90 + 0x238) != 0) {
            flag |= 1;
        }
        *(unsigned char*)((char*)&mEggParams) |= (flag + 1);
    }
    func_ov002_020d6368(((char*)this));
    Sound::PlayCharVoice(0, 0x100, mCamSpacePos);

L65e4:
    *(void**)((char*)&mObjInMouth) = 0;
L65ec:
    if (mIsUnderwater != 0) {
        *(int*)((char*)&mVertSpeed) += func_ov002_020ceaf4(((char*)this));
    }
    if (this->FinishedAnim()) {
        if (mIsUnderwater != 0) {
            this->ChangeState(data_ov002_0211067c);
        } else {
            this->ChangeState(data_ov002_0211013c);
        }
    }
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player15St_Swallow_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Swallow_Init() {
    Player::State;

    extern Player::State data_ov002_02110034;
    extern int Player_DisableInteraction(void *);
    if (*(void **)((char *)&mObjInMouth)) {
        void *o = *(void **)((char *)&mObjInMouth);
        if (((dActor_c *)o)->OnYoshiTryEat() == 4) {
            Player_DisableInteraction(((char *)this));
        }
        int eq = ((dActor_c *)mObjInMouth)->actorID == 0xbf;
        if (eq != 0) {
            this->ChangeState(data_ov002_02110034);
            return 1;
        }
    }
    this->SetAnim(0x70, 0x40000000, 0x1000, 0);
    return 1;
}
}

// @symbol _ZN6Player20RegisterEggCoinCountEjbb
/* recovered: named members + shared header, real C++ method */
void Player::RegisterEggCoinCount(unsigned int count, bool b2, bool b3)
{
	mEggParams = (count & 0xf) << 2;
	if (b3)
		*(unsigned char *)((char*)&mEggParams) |= 0x40;
	if (b2)
		*(unsigned char *)((char*)&mEggParams) |= 0x80;
}

// @symbol func_ov002_020d674c
extern "C" {
int func_ov002_020d674c(char* c){
    Player *player = (Player *)c;
    dActor_c *o = (dActor_c *)player->mObjInMouth;
    if (o == 0)
        return 0;
    return o->OnYoshiTryEat() == 6;
}
}

// @symbol func_ov002_020d6790
extern "C" {
int func_ov002_020d6790(char* self)
{
    Player *p = (Player *)self;
    extern void Player_DisableInteraction(char* c);
    extern void func_ov002_020d7430(Player& player);
    extern void func_ov002_020d8118(Player * c);
    extern Player::State data_ov002_02110034;
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_02110154;
    extern Player::State data_ov002_0211067c;
    extern Player::State data_ov002_0211004c;
    if (p->mMouthHoldTimer != 0)
        return 0;

    dActor_c *o = (dActor_c *)p->mObjInMouth;
    if (o == 0) return 0;

    int isbf = (o->actorID == 0xbf);
    if (isbf) {
        if (p->mIsNoControl != 0)
            goto isbf_ret0;
        if (p->mIsTakingDamage != 0)
            goto isbf_ret0;
        ((Player *)self)->ChangeState(data_ov002_02110034);
        Player_DisableInteraction(self);
        return 1;
isbf_ret0:
        return 0;
    }

    if (!((Player *)self)->IsState(data_ov002_0211013c)) {
        if (!((Player *)self)->IsState(data_ov002_02110154)) {
            if (!((Player *)self)->IsState(data_ov002_0211067c))
                return 0;
        }
    }

    o = (dActor_c *)p->mObjInMouth;
    switch (o->OnYoshiTryEat()) {
    case 2:
    case 4:
        func_ov002_020d7430(*reinterpret_cast<Player*>(self));
        goto ret0;
    case 5: {
        int eq132 = (((dActor_c *)p->mObjInMouth)->actorID == 0x132);
        if (eq132) {
            func_ov002_020d8118(p);
        }
        ((dActor_c *)p->mObjInMouth)->OnTurnIntoEgg(*(Player *)self);
        p->mObjInMouth = 0;
        p->mFireBreaths = 1;
        goto ret0;
    }
    case 3:
        ((Player *)self)->ChangeState(data_ov002_02110034);
        return 1;
    case 1:
    case 6:
        ((Player *)self)->ChangeState(data_ov002_0211004c);
        return 1;
    case 7:
        if (p->mUseAltBodyModel != 0)
            goto case7_ret1;
        ((dActor_c *)p->mObjInMouth)->mFlags |= 0x40000;
        ((dActor_c *)p->mObjInMouth)->mFlags &= ~0x20000;
        p->mUseAltBodyModel = 1;
case7_ret1:
        return 1;
    default:
ret0:
        return 0;
    }
}
}

// @symbol _ZN6Player19func_ov002_020d6998Ev
/* recovered: shared common types */
extern "C" {
int Player::func_ov002_020d6998()
{
    typedef int (*VFunc)(void*);
    extern int func_ov002_020d6dac(Player * self);
    extern void func_ov002_020d71ec(char* self, int arg1);
    extern int func_ov002_020d6c60(char* self, void* other);
    extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
    extern void Vec3_RotateYAndTranslate(int* out, int* in, s16 angle, int* src);
    extern u32 PlayerNewHold10(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                               void *callback);
    extern int func_ov002_020d7030(char* self, void* other);
    extern void Player_DisableInteraction(char* self);
    extern s16 data_02082214[];
    char* self = (char*)this;
    s16 saved;
    void* other;
    u32 id;

    if (func_ov002_020bea7c()) {
        if (IsCollectingCap() != 0) return 0;
        if (this->mHatCharacter != 3) return 0;
    }
    {
        u16 flag = (u16)(this->mStateFlags & 0x1000);
        if (flag) return 0;
    }
    {
        s16* p = &this->mAngleY;
        s16 diff = *p - mHeadRot[1];
        saved = this->mAngleY;
        *p = diff;
    }
    if (func_ov002_020d6dac(this) != 0) {
        func_ov002_020d71ec(self, 2);
        this->mAngleY = saved;
        return 1;
    }
    this->mAngleY = saved;

    if (mObjInMouth != 0)
        goto fail;
    id = this->mAttackClsn.otherOwner;
    if (id == 0) goto fail;
    other = dActor_c::FindWithID(id);
    if (other == 0) goto fail;
    if (other == this) goto fail;

    if (IsCollectingCap() != 0) goto react;
    if (((dActor_c *)other)->OnYoshiTryEat() == 0)
        goto react;

    {
        u8 isType = (((dActor_c *)other)->actorID == 0xbf);
        if (isType) {
            if (((dActor_c *)other)->param1 == 3) {
                if (func_ov002_020d6c60(self, other) == 0) goto react;
                return 1;
            } else {
                Vector3 out;
                Vector3_16 pos16;
                Vector3 rotSrc;
                s16 angle;
                int idx;

                angle = Vec3_HorzAngle(&((dActor_c *)other)->mPos, &mPos);
                idx = ((u16)angle >> 4) * 2;

                pos16.x = data_02082214[idx];
                pos16.y = 0;
                pos16.z = data_02082214[idx + 1];

                rotSrc.x = 0;
                rotSrc.y = 0x32000;
                rotSrc.z = 0x32000;

                Vec3_RotateYAndTranslate(&out.x, &((dActor_c *)other)->mPosX, angle, &rotSrc.x);

                Particle::System::New(0, 0x13e, out.x, out.y, out.z, (const Vector3_16f *)(&pos16),
                                      0);
                Particle::System::New(0, 0x13f, out.x, out.y, out.z, (const Vector3_16f *)(&pos16),
                                      0);
                goto react;
            }
        } else {
            int r;
            if (func_ov002_020d7030(self, other) == 0) goto react;
            r = ((dActor_c *)other)->OnYoshiTryEat();
            if (r == 4) Player_DisableInteraction(self);
            return 1;
        }
    }

react:
    if (this->mAttackClsn.hitFlags & 2) goto fail;
    func_ov002_020d71ec(self, 2);
    Sound::PlayCharVoice(0, 0xfe, mCamSpacePos);
    return 1;

fail:
    func_ov002_020db8d8();
    mAttackClsn.Clear();
    mAttackClsn.Update();
    return 0;
}
}

// @symbol func_ov002_020d6c60
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* Second parameter is void* to agree with decl_common.h:1443. It was char*, which in a
 * C++ TU is a different overload rather than a redeclaration, so this file has not
 * compiled since #866 -- silently, because a file that fails to build is graded NO-SYM,
 * and NO-SYM could not fail the PR gate. */
extern "C" {
extern "C" int func_ov002_020d6c60(char *p0, void *p1v) {
    extern void func_ov002_020d7030(void *c, void *d);
    extern void Player_DisableInteraction(void *c);
    extern void func_ov002_020d71ec(void *c, int x);
    extern int func_ov002_020d5f34(Player *, void *);
    extern int func_ov002_020d6c60(char *, void *);
    extern int func_ov002_020d82f0(void *);
    char *p1 = (char *)p1v;
    Vector3 v;
    int b;
    int y;
    int *q;
    void *sp;
    if (func_ov002_020d82f0(p1) == 0)
        goto ret0a;
    b = (((Player *)p1)->mObjInMouth != 0);
    if (b == 0)
        goto l48;
ret0a:
    return 0;
l48:
    b = (((Player *)p1)->mCharacter != ((Player *)p1)->param1);
    if (b == 0)
        goto leq;
    v.x = ((Player *)p0)->mPosX;
    y = ((Player *)p0)->mPosY;
    v.y = y;
    v.z = ((Player *)p0)->mPosZ;
    v.y = y + 0x64000;
    sp = dActor_c::Spawn(0x10d, (((Player *)p1)->param1 << 8) | 5, v, &((Player *)p0)->mAngle,
                         ((Player *)p0)->mAreaId, -1);
    if (sp == 0)
        goto ret0b;
    func_ov002_020d7030(p0, sp);
    ((Player *)p1)->SetNewHatCharacter(((Player *)p0)->mCharacter, 0, 0);
    Player_DisableInteraction(p0);
    return 1;
leq:
    if (func_ov002_020d5f34((Player *)p1, p0) == 0)
        goto ret0b;
    ((Player *)p0)->mObjInMouth = (s32)p1;
    q = (int *)&((dActor_c *)p1)->mFlags;
    *q = *q | 0x20000;
    func_ov002_020d71ec(p0, 1);
    Player_DisableInteraction(p0);
    return 1;
ret0b:
    return 0;
}
}

// @symbol func_ov002_020d6dac
/* The first two words of the SurfaceInfo rl.surface are copied as one
 * two-int struct. Spelled `{ int a, b; }`, C++ scalarizes that copy into
 * load/store/load/store, 10 words off the ROM's load/load/store/store;
 * the array-wrapper view restores it, as in func_ov002_020c14b8. */
extern "C" {
int func_ov002_020d6dac(Player *self) {
    Player *player = (Player *)self;
    typedef struct P2 { int w[2]; } P2;
    typedef struct dBgPiCopy {
        void *vtb;
        int s0, s1, s2, s3, s4;
        u16 f, g;
        int h, i, j;
    } dBgPiCopy;
    extern void MulVec3Mat4x3(void *v, void *m, void *dst);
    extern void Vec3_MulScalarInPlace(int *v, int s);
    extern int func_02053274(int *a, int *b);
    extern short atan2_hold_Player_8(int a, int b);
    extern u32 PlayerNewHold11(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                               void *callback);
    extern short data_02082214[];
    extern void *data_02099368[];
    Vector3_16f dir;
    Vector3 p1;
    Vector3 p2;
    dBgPiCopy res;
    dBgCh_Lin rl;
    int *d;
    char *actor;
    s16 ang;
    int t;
    int idx;

    {
        u8 b = player->mUseAltBodyModel;
        int model;
        Matrix4x3 *m1 = player->mModels.mHeadModels[(b << 2) + 3]->data.transforms;
        model = ((Player *)self)->GetBodyModelID(player->param1 & 0xff, 0);
        MulVec3Mat4x3(&m1->m[9], &player->mModels.mBodyModels[model]->mat4x3, &p1);
        Vec3_MulScalarInPlace((int *)&p1, 0x8000);
    }

    {
        u8 b2 = player->mUseAltBodyModel;
        Matrix4x3 *m2i = &player->mModels.mHeadModels[(b2 << 2) + 3]->data.transforms[3];
        int model2 = ((Player *)self)->GetBodyModelID(player->param1 & 0xff, 0);
        void *a0, *a1, *a2;
        a1 = player->mModels.mBodyModels[model2];
        a0 = &m2i->m[9];
        a1 = &((dExtAnmModel_c *)a1)->mat4x3;
        a2 = &p2;
        MulVec3Mat4x3(a0, a1, a2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

    rl.SetObjAndLine(p1, p2, (dActor_c *)self);

    if (rl.DetectClsn() == 0) {
        return 0;
    }

    d = &res.s0;
    *(P2 *)d = *(P2 *)&rl.surface.clps;
    d[2] = rl.surface.normal.x;
    d[3] = rl.surface.normal.y;
    d[4] = rl.surface.normal.z;
    res.vtb = data_02099368;
    res.f = rl.triangleID;
    res.g = rl.colliderIdx;
    res.h = rl.clsnID;
    res.i = (int)rl.owner;
    res.j = (int)rl.collider;

    actor = (char *)dActor_c::FindWithID(((dBgPi *)&res)->GetClsnID());
    if (actor != 0 && (t = (int)(((dActor_c *)actor)->actorID == 0xc2)) != 0) {
        ((dBgPi *)&res)->dBgPi::~dBgPi();
        return 0;
    }

    ::Vector3 clsnPos = rl.GetClsnPos();
    player->mTongueHitDist = func_02053274((int *)&p1, (int *)&clsnPos);
    Vector3 normal;
    rl.surface.CopyNormalTo(normal);
    ang = cstd::atan2(normal.x, normal.z);
    idx = (u16)ang >> 4;
    dir.x = data_02082214[idx * 2];
    dir.y = 0;
    dir.z = data_02082214[idx * 2 + 1];
    Particle::System::New(0, 0x13e, clsnPos.x, clsnPos.y, clsnPos.z, (const Vector3_16f *)(&dir),
                          0);
    Particle::System::New(0, 0x13f, clsnPos.x, clsnPos.y, clsnPos.z, (const Vector3_16f *)(&dir),
                          0);

    ((dBgPi *)&res)->dBgPi::~dBgPi();
    return 1;
}
}

// @symbol func_ov002_020d7030
extern "C" {
int func_ov002_020d7030(Player *a, char *b) {
    extern void func_ov002_020d71ec(void*, int);
    int flag = (((dActor_c *)b)->mFlags & 0x60000) != 0;
    if (flag)
        return 0;
    ((dActor_c *)b)->mFlags |= 0x20000;
    *(void **)(b + 0xd0) = a;
    a->mObjInMouth = (s32)b;
    func_ov002_020d71ec(a, 1);
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020d708cEv
extern "C" {
char* Player::func_ov002_020d708c() {
    extern int Vec3_Dist(const void* a, const void* b);
    extern short Vec3_HorzAngle(const void* a, const void* b);
    extern short Vec3_VertAngle(const void* a, const void* b);
    char* self = (char*)this;
    char* a;
    char* best = 0;
    int bestAng = 0x238e;
    a = (char *)Next((dActor_c *)best);
    if (a) {
        int thresh = bestAng;
        do {
            if (a != self) {
                if (((dActor_c *)a)->OnYoshiTryEat()) {
                    if (Vec3_Dist(&mPos, &((dActor_c *)a)->mPos) < 0x15e000) {
                        int diff =
                            (short)(this->mAngleY - Vec3_HorzAngle(&mPos, &((dActor_c *)a)->mPos));
                        int ad = diff < 0 ? -diff : diff;
                        if (ad <= thresh) {
                            int ad2 = diff < 0 ? (short)(-diff) : diff;
                            if (ad2 < bestAng) {
                                bestAng = diff;
                                best = a;
                            }
                        }
                    }
                }
            }
            a = (char *)Next((dActor_c *)a);
        } while (a);
    }
    if (best) {
        short v = Vec3_VertAngle(&mPos, &((dActor_c *)best)->mPos);
        this->mHeadRot[0] = 0;
        this->mHeadRot[1] = (short)bestAng;
        this->mHeadRot[2] = v;
        this->mHeadRotMode = 1;
    }
    return best;
}
}

// @symbol _ZN6Player19func_ov002_020d718cEv
void Player::func_ov002_020d718c() {

    this->mObjInMouth = 0;
    func_ov002_020d71a0();
}

// @symbol _ZN6Player19func_ov002_020d71a0Ev
extern "C" {
void Player::func_ov002_020d71a0() {
    struct G {
        int w[2];
    };
    extern struct G data_ov002_0210eb88;
    char *p = (char*)this;
    this->mUseAltBodyModel = 0;
    ((dExtAnmModel_c *)mModels.mHeadModels[3])
        ->SetAnim((BCA_File *)((void *)data_ov002_0210eb88.w[1]), 0x40000000, 0x1000, 0);
    *(int *)((char *)mModels.mHeadModels[3] + 0x58) = 0;
}
}

// @symbol func_ov002_020d71ec
#pragma push
#pragma opt_propagation off
/* P2's only member is an array. With named members, C++ copies each
 * table into arr[] member by member, and the arr base and the zero for
 * the stack argument take their registers in the other order (5 words
 * differ, measured); a copy of an array member is one block move, as C
 * made it. */
extern "C" {
void func_ov002_020d71ec(Player *player, int arg1) {
    struct P2 { int w[2]; };
    extern struct P2 data_ov002_0210a34c;
    extern struct P2 data_ov002_0210a084;
    extern struct P2 data_ov002_0210eb88;
    extern u8 data_ov002_020ff0f8[];
    int arr[4];
    u32 shifted;
    int val;

    if (arg1 != 2) {
        dExtFrameCtrl_c &anim =
            *player->mModels.mBodyModels[player->GetBodyModelID(player->param1 & 0xff, 0)];
        shifted = ((u32)anim.currFrame << 4) >> 0x10;
        *(struct P2*)&arr[0] = data_ov002_0210a34c;
        *(struct P2*)&arr[2] = data_ov002_0210a084;
        player->SetAnim(arr[arg1], 0x40000000, 0x1000, 0);

        if (player->mUseAltBodyModel == 0) {
            ((dExtAnmModel_c *)player->mModels.mHeadModels[3])
                ->SetAnim((BCA_File *)(*(void **)((char *)(&arr[2])[arg1] + 4)), 0x40000000, 0x1000,
                          0);
        }

        if (arg1 == 1) {
            val = data_ov002_020ff0f8[shifted];
            dExtFrameCtrl_c &body =
                *player->mModels.mBodyModels[player->GetBodyModelID(player->param1 & 0xff, 0)];
            val = ((u32)val << 0x10) >> 4;
            body.currFrame = val;
            ((dExtAnmModel_c *)player->mModels.mHeadModels[3])->currFrame = val;
        }

        player->mStateStep = 1;
        return;
    }

    player->SetAnim(0x6c, 0x40000000, -0x1000, 0);
    ((dExtAnmModel_c *)player->mModels.mHeadModels[3])
        ->SetAnim((BCA_File *)(*(void **)((char *)&data_ov002_0210eb88 + 4)), 0x40000000, -0x1000,
                  0);
    player->mStateStep = 2;
    player->mStateTimer = 0xa;
}
}
#pragma pop

// @symbol _ZN6Player21St_YoshiPower_CleanupEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_YoshiPower_Cleanup()
{
    extern void func_ov002_020d6790(char*);
    char* r2 = *(char**)((char*)&mObjInMouth);
    if (r2 != 0) {
        int t = (int)((((dActor_c *)r2)->mFlags & 0x20000) != 0);
        if (t != 0) {
            ((dActor_c *)r2)->mFlags &= ~0xe0000;
            func_ov002_020d718c();
            return 1;
        }
        ((dActor_c *)r2)->mFlags &= ~0x20000;
    }
    if (mUseAltBodyModel == 0) {
        char* q = *(char**)((char*)&mObjInMouth);
        if (q != 0) {
            int eq = (int)(((dActor_c *)q)->actorID == 0xbf);
            if (eq == 0) {
                func_ov002_020d6790(((char*)this));
                goto end;
            }
        }
        func_ov002_020d718c();
    } else {
        if (*(char **)((char *)&mObjInMouth) == 0 && mFireBreaths == 0)
            func_ov002_020d71a0();
    }
end:
    return 1;
}
}

// @symbol func_ov002_020d7430
// State globals still have incompatible declarations across the legacy callers.
extern "C" {
void func_ov002_020d7430(Player& player)
{
    extern Player::State data_ov002_02110034;
    extern Player::State data_ov002_0211013c;
    dActor_c* mouthActor = reinterpret_cast<dActor_c*>(player.mObjInMouth);
    if (mouthActor->OnYoshiTryEat() == 2) {
        Vector3 pos;
        pos.x = player.mPosX;
        pos.y = player.mPosY;
        pos.z = player.mPosZ;
        player.Hurt(pos, 1, 0xc000, 1, 0, 0);
        return;
    }
    player.func_ov002_020c9e18();
    mouthActor = reinterpret_cast<dActor_c*>(player.mObjInMouth);
    mouthActor->OnTurnIntoEgg(player);
    if (player.IsState(data_ov002_02110034)) {
        player.ChangeState(data_ov002_0211013c);
    }
    player.func_ov002_020d718c();
    Sound::PlayCharVoice(0, 0xfd, player.mCamSpacePos);
}
}

// @symbol _ZN6Player18St_YoshiPower_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Yoshi swallowing and spitting, per frame. mStateStep walks the sequence, and
 * most transitions are gated on the body model's dExtFrameCtrl_c reaching a specific
 * frame -- which is why GetBodyModelID is re-fetched at each one rather than
 * hoisted; the original does that too.
 *
 * Player::St_YoshiPower_Main at 0x020d7504 (ov002), size 0x9cc
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (also base/sp2)
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
/* local extern: the call passes an untyped this, so it cannot use the header method. */
/* local extern: the call passes an untyped this, so it cannot use the header method. */
struct PVec { int x, y, z; PVec() {} ~PVec() {} };
extern "C" {
int Player::St_YoshiPower_Main()
{
    extern void func_ov002_020d71ec(char*, int);
    extern void MulVec3Mat4x3(void*, void*, void*);
    extern void Vec3_MulScalarInPlace(void*, int);
    extern void func_ov002_020d8118(Player *);
    extern int func_ov002_020e0ccc(char*, void*);
    extern void func_ov002_020d7430(Player & player);
    extern void func_ov002_020d5cec(void*);
    extern u32 PlayerNewHold12(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                               void *callback);
    extern void func_ov002_020dc09c(char*);
    extern void ApproachAngle(void*, short, int, int, int);
    extern void Player_AdvanceAnims(char *);
    extern short data_02082214[];
    extern int data_ov002_0210e3d8[];
    extern Player::State data_ov002_02110034;
    extern Player::State data_ov002_021101b4;
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_0211067c;
    /* The helpers this calls are all declared taking char*, and there are
       thirty-odd call sites. One alias keeps them untouched; the field
       accesses below are the part that needed recovering. */
    char* c = (char*)this;
    Vector3_16f dir;
    Vector3 tmp;
    PVec pos;
    dActor_c *obj0;
    char* dp;

    if (mIsAirborne == 0) {
        ApproachLinear(*((int *)(&mHorzSpeed)), 0, 0x1000);
        this->func_ov002_020c2f64();
    }

    switch (mStateStep) {
    case 0: {
        if (mStateArg == 0) {
            int t4 = (((dExtAnmModel_c *)mModels.mHeadModels[3])->currFrame >> 0xc) << 0x10;
            int id0 = this->GetBodyModelID(this->param1 & 0xff, 0);
            /* Split: a plain ->currFrame folds +0x50 and +8 into one +0x58 and moves bytes. */
            dp = (char *)mModels.mBodyModels[id0];
            dp = (char *)&(dExtFrameCtrl_c &)*(dExtAnmModel_c *)dp;
            ((dExtFrameCtrl_c *)dp)->currFrame = (unsigned int)t4 >> 4;
            mStateArg = 1;
        }
        if (this->func_ov002_020d6998() != 0) {
            return 1;
        }
        if (this->FinishedAnim() != 0) {
            func_ov002_020d71ec(c, 0);
        }
        goto common_tail;
    }

    case 1: {
        ApproachLinear2(mHeadRot[1], 0, 0x200);
        ApproachLinear2(mHeadRot[2], 0, 0x200);
        int flag = 1;
        mHeadRotMode = flag;
        if (*(void**)(&mObjInMouth) != 0) {
            int b = mUseAltBodyModel;
            Matrix4x3 *t4 = &mModels.mHeadModels[(b << 2) + 3]->data.transforms[3];
            if (*(void**)(&mObjInMouth) != 0) {
                if (!(((dActor_c *)*(void **)&mObjInMouth)->mFlags & 0x20000))
                    flag = 0;
                if (flag != 0) {
                    int id1 = this->GetBodyModelID(this->param1 & 0xff, 0);
                    MulVec3Mat4x3(&t4->m[9], &mModels.mBodyModels[id1]->mat4x3, &tmp);
                    Vec3_MulScalarInPlace(&tmp, 0x8000);
                    dp = (char*)*(void**)(&mObjInMouth);
                    ((dActor_c *)dp)->mPosX = tmp.x;
                    ((dActor_c *)dp)->mPosY = tmp.y;
                    ((dActor_c *)dp)->mPosZ = tmp.z;
                }
            }
            int id2 = this->GetBodyModelID(this->param1 & 0xff, 0);
            if (mModels.mBodyModels[id2]->WillHitFrame(0xa) != 0) {
                Vector3 *src = &mPos;
                dp = (char*)*(void**)(&mObjInMouth);
                ((dActor_c *)dp)->mPosX = src->x;
                ((dActor_c *)dp)->mPosY = src->y;
                ((dActor_c *)dp)->mPosZ = src->z;
                dActor_c *o = *(dActor_c **)(&mObjInMouth);
                int cond = (o->actorID == 0xbf);
                if (cond != 0) {
                    o->mFlags &= ~0x20000;
                    if (((Player*)(*(void**)(&mObjInMouth)))->func_ov002_020d5ed0() == 0) {
                        func_ov002_020d718c();
                    }
                } else {
                    ((dActor_c *)o)->mFlags |= 0x40000;
                    ((dActor_c *)mObjInMouth)->mFlags &= ~0x20000;
                    if ((*(dActor_c **)(&mObjInMouth))->OnYoshiTryEat() == 4 ||
                        (*(dActor_c **)(&mObjInMouth))->OnYoshiTryEat() == 2) {
                        mStateStep = 3;
                        int f2 = 1;
                        mUseAltBodyModel = f2;
                        mStateTimer = 8;
                        if (((dActor_c *)mObjInMouth)->actorID != 0x10d)
                            f2 = 0;
                        if (f2 != 0) mStateTimer = 0xc;
                        unk_6ee = 1;
                        goto common_tail;
                    }
                }
            } else {
                int id3 = this->GetBodyModelID(this->param1 & 0xff, 0);
                if (mModels.mBodyModels[id3]->WillHitFrame(0xc) != 0) {
                    mUseAltBodyModel = 1;
                    if ((*(dActor_c **)(&mObjInMouth))->OnYoshiTryEat() == 5) {
                        int cond2 = (((dActor_c *)mObjInMouth)->actorID == 0x132);
                        if (cond2 != 0) {
                            func_ov002_020d8118(this);
                        }
                        (*(dActor_c **)(&mObjInMouth))->OnTurnIntoEgg(*(Player *)c);
                        *(void**)(&mObjInMouth) = 0;
                        mFireBreaths = 1;
                    }
                }
            }
        }
        if (this->FinishedAnim() != 0) {
            obj0 = *(dActor_c **)(&mObjInMouth);
            if (obj0 != 0) {
                if (obj0->OnYoshiTryEat() == 3) {
                    this->ChangeState(data_ov002_02110034);
                    return 1;
                }
                if (func_ov002_020e0ccc(c, *(void**)(&mObjInMouth)) != 0) {
                    return 1;
                }
                mMouthHoldTimer = 0x5a;
            }
            if (mIsAirborne != 0) {
                this->ChangeState(data_ov002_021101b4);
            } else {
                this->ChangeState(data_ov002_0211013c);
            }
            return 1;
        }
        goto common_tail;
    }

    case 2: {
        ApproachLinear2(mHeadRot[1], 0, 0x200);
        ApproachLinear2(mHeadRot[2], 0, 0x200);
        mHeadRotMode = 1;
        if (mStateTimer != 0) {
            return 1;
        }
        int id4 = this->GetBodyModelID(this->param1 & 0xff, 0);
        int *a50b =
            (int *)((long long)(int)((char *)&(dExtFrameCtrl_c &)*mModels.mBodyModels[id4]));
        if ((unsigned short)(((dExtFrameCtrl_c *)a50b)->currFrame >> 0xc) == 0) {
            if (mIsAirborne != 0) {
                this->ChangeState(data_ov002_021101b4);
            } else {
                this->ChangeState(data_ov002_0211013c);
            }
            return 1;
        }
        goto common_tail;
    }

    case 3:
        if (mStateTimer == 0) {
            if (*(void**)(&mObjInMouth) == 0) {
                if (mIsUnderwater != 0) {
                    this->ChangeState(data_ov002_0211067c);
                } else {
                    this->ChangeState(data_ov002_0211013c);
                }
            } else {
                func_ov002_020d7430(*this);
            }
            return 1;
        }
        goto common_tail;

    case 4: {
        int id5 = this->GetBodyModelID(this->param1 & 0xff, 0);
        if (mModels.mBodyModels[id5]->WillHitFrame(3) != 0) {
            int z = 0;
            mUseAltBodyModel = z;
            if (mUseAltBodyModel == 0) {
                int id6 = this->GetBodyModelID(this->param1 & 0xff, z);
                int *a50c = (int *)((
                    long long)(int)((char *)&(dExtFrameCtrl_c &)*mModels.mBodyModels[id6]));
                /* The u16 startFrame takes the whole frame, currFrame >> 12;
                   its narrowing folds into the ROM's lsl #4 / lsr #16. */
                ((dExtAnmModel_c *)mModels.mHeadModels[3])
                    ->SetAnim((BCA_File *)data_ov002_0210e3d8[1], 0x40000000, 0x1000,
                              ((dExtFrameCtrl_c *)a50c)->currFrame >> 12);
            }
        }
        int id7 = this->GetBodyModelID(this->param1 & 0xff, 0);
        if (mModels.mBodyModels[id7]->WillHitFrame(4) != 0) {
            obj0 = *(dActor_c **)(&mObjInMouth);
            if (obj0 != 0) {
                int cond3 = (obj0->actorID == 0xbf);
                if (cond3 != 0) {
                    func_ov002_020d5cec(obj0);
                    mStateFlags &= ~2;
                } else {
                    ((dActor_c *)obj0)->mFlags |= 0x80000;
                    ((dActor_c *)mObjInMouth)->mFlags &= ~0x40000;
                }
            }
            *(void**)(&mObjInMouth) = 0;
            Sound::PlayCharVoice(0, 0xff, mCamSpacePos);
        }
        if (this->FinishedAnim() != 0) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
        goto common_tail;
    }

    case 5: {
        if (mStateTimer != 0) {
            int id8 = this->GetBodyModelID(this->param1 & 0xff, 0);
            int *a50d =
                (int *)((long long)(int)((char *)&(dExtFrameCtrl_c &)*mModels.mBodyModels[id8]));
            if (((unsigned int)(((dExtFrameCtrl_c *)a50d)->currFrame << 4) >> 0x10) >= 4) {
                int i2 = (*(unsigned short*)&mAngleY >> 4) * 2;
                short* tbl = data_02082214;
                int zv = mPosZ + (int)(((((long long)tbl[i2+1]) << 18) + 0x800) >> 12);
                int yv = mPosY + 0x37000;
                pos.x = mPosX + (int)(((((long long)tbl[i2]) << 18) + 0x800) >> 12);
                pos.z = zv;
                pos.y = yv;
                dir.x = data_02082214[(*(unsigned short*)&mAngleY >> 4) * 2];
                dir.y = 0;
                dir.z = data_02082214[(*(unsigned short*)&mAngleY >> 4) * 2 + 1];
                mParticle1 = Particle::System::New(mParticle1, 0x13c, pos.x, pos.y, pos.z,
                                                   (const Vector3_16f *)(&dir), 0);
                mParticle2 = Particle::System::New(mParticle2, 0x13d, pos.x, pos.y, pos.z,
                                                   (const Vector3_16f *)(&dir), 0);
                func_ov002_020dc09c(c);
                mAttackClsn.Clear();
                mAttackClsn.Update();
                if (mStateTimer == 1 && mFireBreaths == 0) {
                    int z2 = 0;
                    mUseAltBodyModel = z2;
                    int id9 = this->GetBodyModelID(this->param1 & 0xff, z2);
                    int *a50e = (int *)((
                        long long)(int)((char *)&(dExtFrameCtrl_c &)*mModels.mBodyModels[id9]));
                    ((dExtAnmModel_c *)mModels.mHeadModels[3])
                        ->SetAnim((BCA_File *)data_ov002_0210e3d8[1], 0x40000000, 0x1000,
                                  ((dExtFrameCtrl_c *)a50e)->currFrame >> 12);
                }
                return 1;
            }
        }
        if (this->FinishedAnim() != 0) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
        goto common_tail;
    }

    case 6: {
        dActor_c *p = *(dActor_c **)(&mHeldObj);
        if (p != 0) {
            ApproachAngle(&mAngleY, p->mAngleY, 4, 0x2000, 0x10);
        }
        mPrevAngleY = mAngleY;
        int id10 = this->GetBodyModelID(this->param1 & 0xff, 0);
        if (mModels.mBodyModels[id10]->WillHitFrame(6) != 0) {
            this->func_ov002_020daa74();
            Sound::PlayCharVoice(0, 0x102, mCamSpacePos);
            goto common_tail;
        }
        if (this->FinishedAnim() != 0) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
        goto common_tail;
    }

    default:
    common_tail:
        Player_AdvanceAnims(c);
        if (mUseAltBodyModel == 0 && mStateStep != 6) {
            ((dExtAnmModel_c *)mModels.mHeadModels[3])->Advance();
        }
        return 1;
    }
}
}

// @symbol _ZN6Player18St_YoshiPower_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* Player::St_YoshiPower_Init at 0x020d7ed0 (ov002), size 0x200
 * Matched byte-for-byte with mwccarm 1.2/sp2p3
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
extern "C" {
int Player::St_YoshiPower_Init()
{
    extern void func_ov002_020ed63c(void*, int);
    mStateFlags |= 0x200;
    unk_6ee = 0;
    mEggParams = 0;
    if (mFireTimer != 0) {
        mFireBreaths = 2;
    }
    if (mHorzSpeed >= 0x1c000) {
        mHorzSpeed = 0x1c000;
    }
    if (**(int**)((char*)&mHeldObjQueue) == 0) {
        if (mFireBreaths != 0) {
            this->SetAnim(0x6d, 0x40000000, 0x1000, 0);
            mStateStep = 5;
            mStateTimer = 0x1e;
            mFireBreaths -= 1;
            Sound::PlayCharVoice(0, 0x101, mCamSpacePos);
        } else if (*(void**)((char*)&mObjInMouth) == 0) {
            this->SetAnim(0x6c, 0x40000000, 0x1000, 0);
            if (mUseAltBodyModel != 0) {
                func_ov002_020d71a0();
            }
            mStateStep = 0;
            mStateArg = 0;
            mAttackClsn.Clear();
            func_ov002_020d708c();
            Sound::PlayCharVoice(0, 0xfc, mCamSpacePos);
        } else {
            this->SetAnim(0x6d, 0x40000000, 0x1000, 0);
            mStateStep = 4;
        }
    } else {
        int i;
        this->SetAnim(0x30, 0x40000000, 0x1000, 0);
        mStateStep = 6;
        *(void**)((char*)&mHeldObj) = **(void***)((char*)&mHeldObjQueue);
        ((dActor_c *)mHeldObj)->mFlags |= 0x100;
        func_ov002_020ed63c(*(void**)((char*)&mHeldObj), 1);
        (*(int**)((char*)&mHeldObjQueue))[0] = 0;
        for (i = 1; i < 5; i++) {
            int* p = *(int**)((char*)&mHeldObjQueue);
            int v = p[i];
            if (v != 0) {
                (p + i)[-1] = v;
                (*(int**)((char*)&mHeldObjQueue))[i] = 0;
            }
        }
    }
    return 1;
}
}

// @symbol func_ov002_020d80d0
extern "C" {
void func_ov002_020d80d0(char* c){
    Player *player = (Player *)c;
    if (player->mIsFire != 1)
        return;
    player->mFireTimer = 0;
    player->mIsFire = 0;
    player->mFireBreaths = 0;
    player->mUseAltBodyModel = 0;
    player->EndPowerupMusic(0x32);
}
}

// @symbol func_ov002_020d8118
extern "C" {
void func_ov002_020d8118(Player *c) {
    Player *player = (Player *)c;
    ((Player *)c)->func_ov002_020bda48();
    player->mFireTimer = 0x258;
    player->StartPowerupMusic(0x32);
    ((Player *)c)->func_ov002_020c43c4(5);
    player->mIsFire = 1;
}
}

// @symbol func_ov002_020d8158
extern "C" {
void func_ov002_020d8158(char* c)
{
    extern void func_ov002_020d8118(Player * c);
    extern Player::State data_ov002_02110034;
    extern Player::State data_ov002_0211004c;
    Player *p = (Player *)c;
    if (p->param1 != 3) {
        *(u16 *)(int)(&p->mStateFlags) &= ~0x1000;
        return;
    }
    if (((Player *)c)->IsState(data_ov002_02110034))
        return;
    if (((Player *)c)->IsState(data_ov002_0211004c))
        return;
    if (p->mIsNoControl != 0)
        return;
    if ((u16)(p->mStateFlags & 0x1000) == 0)
        return;
    func_ov002_020d8118(p);
    p->mFireBreaths = 1;
    ((Player *)c)->func_ov002_020bdc18();
    *(u16 *)(int)(&p->mStateFlags) &= ~0x1000;
    p->mUseAltBodyModel = 1;
}
}

// @symbol _ZN6Player13InitFireYoshiEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
void Player::InitFireYoshi()
{

    extern Player::State data_ov002_0211004c;
    u16 *p = &this->mStateFlags;
    *p |= 0x1000;
    if (mObjInMouth == 0)
        return;
    this->ChangeState(data_ov002_0211004c);
}
}

// @symbol _ZN6Player21St_DizzyStars_CleanupEv
int Player::St_DizzyStars_Cleanup()
{

    mFlags &= ~0x80u;
    return 1;
}

// @symbol _ZN6Player18St_DizzyStars_MainEv
/* recovered: named members + shared header, real C++ method */
/* the walk state */
extern "C" {
int Player::St_DizzyStars_Main()
{

    extern Player::State data_ov002_0211013c;
    func_ov002_020d98b4();
    /* Read SIGNED on purpose. mStateTimer is u16 and every other user treats
       it that way, but this test is `> 0` against a signed short, which is a
       bgt rather than a bhi. Spelling it unsigned changes the instruction. */
    if (*(s16*)&mStateTimer > 0) return 1;
    this->ChangeState(data_ov002_0211013c);
    return 1;
}
}

// @symbol _ZN6Player18St_DizzyStars_InitEv
/* recovered: named members + shared header, real C++ method */
int Player::St_DizzyStars_Init()
{
    mStateTimer = 0x12c;
    mFlags |= 0x80;
    return 1;
}

// @symbol func_ov002_020d82f0
extern "C" {
int func_ov002_020d82f0(void *c) {
    Player *p = (Player *)c;
    if (p->mInvincibleTimer != 0)
        return 0;
    if (p->mIsMega != 0)
        return 0;
    if (p->mCapFlags != 0)
        return 0;
    if (p->mIsVanish != 0)
        return 0;
    if (p->mIsTakingDamage != 0)
        goto ret0;
    if (p->mIsNoControl == 0)
        goto ret1;
ret0:
    return 0;
ret1:
    return 1;
}
}

// @symbol func_ov002_020d8360
/* func_ov002_020d8360 at 0x020d8360
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
extern "C" {
int func_ov002_020d8360(void *self, void *other, unsigned int flags)
{
    extern int func_ov002_020d82f0(void *c);
    extern int func_ov002_020d853c(char *c, char *o);
    extern int func_ov002_020d8944(void *self, char *enemy, char *v1);
    extern void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
    extern void func_ov002_020d8d10(void *self, Vector3 *pos);
    extern void func_020105cc(void *thiz, unsigned int flags);
    extern unsigned short data_ov002_020ff29c[];
    char *c = (char *)self;
    Player *player = (Player *)c;
    char *o = (char *)other;

    {
        int r = func_ov002_020d82f0(c);
        if (!r) return r;
    }

    {
        unsigned char t = ((Player *)o)->mAttackClashed;
        if (t) return t;
    }

    if (func_ov002_020d853c(c, o) == 0) {
        unsigned int m = player->mdCcAcPos_c.hitFlags & 0x3c0;
        if (m == 0) return m;

        {
            int r = func_ov002_020d8944(c, o, c);
            if (r != 0) {
                r = 1;
                ((Player *)o)->mAttackClashed = r;
                player->mAttackClashed = r;
            }
            return r;
        }
    }

    {
        int flag = flags & 0x380;
        if (flag != 0) {
            player->mIsAirborne = 1;
            player->mLandSoundPlayed = 0;
        }
        {
            Vector3 out, tr, v, cp;
            int *p = &((dActor_c *)o)->mPosX;
            v.x = p[0];
            v.y = p[1];
            v.z = p[2];
            {
                int hr = ((Player *)c)->Hurt(v, 1, 0, 0, (((dActor_c *)o)->param1 + 1) & 0xff, 1);
                if (!hr) return hr;
            }
            tr.x = 0;
            tr.y = 0x50000;
            tr.z = 0x64000;
            Vec3_RotateYAndTranslate((int *)&out, &player->mPosX, player->mAngleY, (int *)&tr);
            cp.x = out.x;
            cp.y = out.y;
            cp.z = out.z;
            func_ov002_020d8d10(c, &cp);
        }
        func_020105cc(c, flags);
        {
            unsigned char b6f9 = player->mIsMetal;
            unsigned short idx = (unsigned short)(player->param1 + (((dActor_c *)o)->param1 << 2));
            int t98;
            if (b6f9 != 0) idx = 0x11;
            t98 = data_ov002_020ff29c[idx] << 3;
            player->mHorzSpeed = t98;
            player->mIsAirborne = 1;
            player->mLandSoundPlayed = 0;
            if (flag != 0) {
                player->mVertSpeed = t98 + 0x20000;
                {
                    int *hp = (int *)(int)(&player->mHorzSpeed);
                    int half = *hp >> 1;
                    *hp = half;
                    return half;
                }
            }
            player->mVertSpeed = 0x18000;
            return 0x18000;
        }
    }
}
}

// @symbol func_ov002_020d853c
extern "C" {
int func_ov002_020d853c(Player *player, char *o) {
    extern short Vec3_HorzAngle(void *v0, void *v1);
    extern int AngleDiff(int a, int b);
    extern Player::State data_ov002_02110574;
    extern unsigned short data_ov002_0210a5dc[];
    if (player->IsState(data_ov002_02110574)) {
        int j = (player->mPunchKickStep << 1) & 0xff;
        unsigned idx = player->GetBodyModelID(player->param1 & 0xff, 0);
        dExtAnmModel_c *mp = player->mModels.mBodyModels[idx];
        int v = ((dExtFrameCtrl_c *)((long long)(int)&(dExtFrameCtrl_c &)*mp))->currFrame;
        int k = (unsigned short)(v >> 12);
        if (k < data_ov002_0210a5dc[j] || k > data_ov002_0210a5dc[j + 1])
            return 1;
        if (AngleDiff(Vec3_HorzAngle(&player->mPos, &((dActor_c *)o)->mPos), player->mAngleY) <
            0x4000)
            return 0;
    }
    return 1;
}
}

// @symbol func_ov002_020d85fc
extern "C" {
extern "C" int func_ov002_020d85fc(Player *player, Player *arg, int flags) {
    extern int func_ov002_020db5f4(Player * player, dActor_c * arg);
    extern Player::State data_ov002_0211034c;
    extern Player::State data_ov002_02110094;
    if ((flags & 0x1000) && arg->param1 == 2 &&
        (player->IsState(data_ov002_0211034c) ||
         (player->IsState(data_ov002_02110094) && player->mStateTimer != 0)) &&
        arg->TryGrab(*(dActor_c *)player)) {
        func_ov002_020db5f4(player, arg);
        return 1;
    }
    return 0;
}
}

// @symbol func_ov002_020d869c
struct PVec869c { s32 x, y, z; ~PVec869c() {} };
extern "C" {
extern "C" int func_ov002_020d869c(char *c)
{
    extern int func_ov002_020d85fc(char *self, char *arg, int flags);
    extern int func_ov002_020d8360(void *self, void *other, u32 flags);
    extern int func_ov002_020d9298(char *c);
    extern void func_ov002_020db8bc(u8 *p, u8 val);
    extern void PlayerNewSimpleHold11(u32 id, Fix12i x, Fix12i y, Fix12i z);
    Player *pl = (Player *)c;
    char *r6;
    u32 flags;
    u32 f2;
    int t;
    PVec869c p;
    s32 x;
    s32 y;
    s32 z;

    if (pl->mEatingPlayer != 0 || pl->mIsBodyClsnEnabled == 0)
        return 0;
    if (pl->mdCcAcPos_c.otherOwner == 0)
        return 0;
    r6 = (char *)dActor_c::FindWithID(pl->mdCcAcPos_c.otherOwner);
    if (r6 == 0 || r6 == c)
        return 0;
    t = (int)(((dActor_c *)r6)->actorID == 0xbf);
    if (t == 0)
        return 0;

    flags = pl->mdCcAcPos_c.hitFlags & 0x27fe0;
    if (flags != 0) {
        if (func_ov002_020d85fc(c, r6, flags) == 1)
            return 0;
        f2 = flags & ~0x1000u;
        if (f2 != 0)
            func_ov002_020d8360(c, r6, f2);
    } else {
        if (pl->mIsNoControl != 0)
            return 0;
        if (pl->JumpedOnByPlayer(pl->mdCcAcPos_c, *(Player *)r6) != 0) {
            if (((Player *)r6)->IsInAir() == 0)
                return 0;
            if (func_ov002_020d9298(r6) != 0) {
                func_ov002_020db8bc((u8 *)c, 0);
                x = pl->mPosX;
                y = pl->mPosY + 0x82000;
                z = pl->mPosZ;
                p.x = x;
                p.y = y;
                p.z = z;
                Particle::System::NewSimple(0xb2, p.x, p.y, p.z);
                Sound::PlayBank0(0x12, pl->mCamSpacePos);
                Sound::PlayCharVoice(pl->mCharacter, 0x19, pl->mCamSpacePos);
            }
        }
    }
    return 1;
}
}

// @symbol func_ov002_020d8838
// Set bit 0x20 in the halfword flag at self+0x6ce and return. The u64-mask launder
// forces the base+offset to materialize into a register (pool-loaded 0x6ce + add) so
// the ldrh/strh reuse it, matching the ROM instead of folding the address.
extern "C" {
void func_ov002_020d8838(char *self)
{
    Player *player = (Player *)self;
    player->mStateFlags |= 0x20;
}
}

// @symbol func_ov002_020d8854
/* recovered: shared common types */
extern "C" {
void func_ov002_020d8854(char *self) {
    extern void Vec3_RotateYAndTranslate(Vector3 * out, Vector3 * in, int ang, Vector3 *tr);
    extern int func_ov002_020d8944(char *a, char *b, char *other);
    extern void func_ov002_020d8d10(char *self, Vector3 *v);
    Player *p = (Player *)self;
    char *other = (char *)dActor_c::FindWithID(p->mAttackClsn.otherOwner);
    if (other == 0)
        return;
    {
        int b = (int)(((dActor_c *)other)->actorID == 0xbf);
        if (b != 0) {
            if (((Player *)other)->mPlayerNo <= p->mPlayerNo)
                return;
            if ((p->mAttackClsn.hitFlags & 0x3c0) == 0)
                return;
            if (func_ov002_020d8944(self, self, other) != 0) {
                ((Player *)other)->mAttackClashed = 1;
                p->mAttackClashed = 1;
            }
            return;
        }
    }
    {
        Vector3 out;
        Vector3 tr;
        Vector3 cp;
        tr.x = 0;
        tr.y = 0x50000;
        tr.z = 0x64000;
        Vec3_RotateYAndTranslate(&out, (Vector3 *)(&p->mPosX), p->mAngleY, &tr);
        cp = out;
        func_ov002_020d8d10(self, &cp);
    }
}
}

// @symbol func_ov002_020d8944
/* recovered: shared common types */
// non-trivial destructor keeps the particle-spawn vector on the stack (no SROA)
struct PVec8944 {
    int x, y, z;
    ~PVec8944() {}
};
extern "C" {
int func_ov002_020d8944(void *self, Player *enemy, Player *v1) {
    s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
    int AngleDiff(int a, int b);
    void Vec3_Add(Vector3 * out, Vector3 * a, Vector3 * b);
    void Vec3_Asr(Vector3 * d, Vector3 * s, int sh);
    void PlayerNewSimpleHold12(unsigned int id, int x, int y, int z);
    extern int func_ov002_020d8944(void *self, char *enemy, char *v1);
    PVec8944 p;
    Vector3 sum;
    Vector3 asr;
    int b0 = ((unsigned short)((dActor_c *)v1)->actorID == 0xbf);
    if (b0) {
        s16 ang = Vec3_HorzAngle(&((dActor_c *)enemy)->mPos, &((dActor_c *)v1)->mPos);
        if (AngleDiff(ang, ((dActor_c *)enemy)->mAngleY) >= 0x4000)
            return 0;
        ((dActor_c *)enemy)->mHorzSpeed = 0xa000;
        ((dActor_c *)v1)->mHorzSpeed = 0xa000;
        ((dActor_c *)enemy)->mPrevAngleX = 0;
        ((dActor_c *)enemy)->mPrevAngleY = ang + 0x8000;
        ((dActor_c *)enemy)->mPrevAngleZ = 0;
        ((dActor_c *)v1)->mPrevAngleX = 0;
        ((dActor_c *)v1)->mPrevAngleY = ang;
        ((dActor_c *)v1)->mPrevAngleZ = 0;
        Vec3_Add(&sum, &((dActor_c *)enemy)->mPos, &((dActor_c *)v1)->mPos);
        Vec3_Asr(&asr, &sum, 1);
        p.x = asr.x;
        p.y = asr.y;
        p.z = asr.z;
        p.y += 0x50000;
        Particle::System::NewSimple(0x12a, p.x, p.y, p.z);
        Particle::System::NewSimple(0x12b, p.x, p.y, p.z);
        enemy->mAttackClashed = 1;
        v1->mAttackClashed = 1;
        return 1;
    }
    return 0;
}
}

// @symbol func_ov002_020d8a50
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
void func_ov002_020d8a50(char* self, int which)
{
    Player *player = (Player *)self;
    extern void func_ov002_020dbf4c(char* c);
    extern s16 data_02082214[];
    extern u16 data_ov002_0210a5cc[];
    extern void func_ov002_020d8854(char*);
    extern void func_ov002_020d8d10(char *, Vector3 *);
    extern void func_ov002_020dbbc0(char*);
    extern void func_ov002_020dbd94(char*);
    extern void func_ov002_020dbe70(char*);
    extern int func_ov002_020eeeb8(void*, char*);
    extern int func_ov002_020ef070(void*, char*);
    int flag = 1;
    u32 i2 = (u8)(which << 1);
    dExtAnmModel_c *model;
    dExtFrameCtrl_c *anim;
    int frame;

    model = player->mModels.mBodyModels[((Player *)self)->GetBodyModelID(player->param1 & 0xff, 0)];
    /* Launder kept: reading model->currFrame directly moves bytes. */
    anim = reinterpret_cast<dExtFrameCtrl_c *>(
        (long long)(int)&static_cast<dExtFrameCtrl_c &>(*model));
    frame = (u16)(anim->currFrame >> 12);
    if (frame < data_ov002_0210a5cc[i2] || frame > data_ov002_0210a5cc[i2 + 1]) {
        flag = 0;
        player->mAttackClsn.Clear();
    }

    if ((u8)player->mAttackClashed != 0)
        return;

    switch (which) {
    case 0: func_ov002_020dbf4c(self); break;
    case 1: func_ov002_020dbe70(self); break;
    case 2: func_ov002_020dbd94(self); break;
    case 3: func_ov002_020dbbc0(self); break;
    }

    if (player->mAttackClsn.otherOwner != 0) {
        player->mHorzSpeed = 0xa000;
        player->mPrevAngleY = player->mAngleY + 0x8000;
        func_ov002_020d8854(self);
    }

    player->mAttackClsn.Clear();
    if (flag == 1)
        player->mAttackClsn.Update();

    model = player->mModels.mBodyModels[((Player *)self)->GetBodyModelID(player->param1 & 0xff, 0)];
    if (!model->WillHitFrame(data_ov002_0210a5cc[i2]))
        return;

    switch (which) {
    case 0:
    case 1:
        func_ov002_020ef070(&player->mMeshClsn, self);
        break;
    case 2:
    case 3:
        func_ov002_020eeeb8(&player->mMeshClsn, self);
        break;
    }

    {
        u16 t = player->mStateFlags & 0x20;
        if (t == 0)
            return;
    }

    player->mStateFlags &= ~0x20;
    Sound::PlayBank0(0xb5, player->mCamSpacePos);

    {
        u16 ang = (u16)player->mAngleY;
        dBgCh_Lin ray;
        Vector3 v1;
        Vector3 v2;
        Vector3 normal;
        int idx;

        idx = ang >> 4;
        v1.x = player->mPosX;
        v1.y = player->mPosY;
        v1.z = player->mPosZ;
        v1.y += 0x32000;
        v2.x = v1.x;
        v2.y = v1.y;
        v2.z = v1.z;
        v2.x += data_02082214[idx * 2] * 0x64;
        v2.z += data_02082214[idx * 2 + 1] * 0x64;
        ray.SetObjAndLine(v1, v2, (dActor_c *)self);
        if (ray.DetectClsn()) {
            ray.surface.CopyNormalTo(normal);
            if (normal.y == 0) {
                player->mHorzSpeed = 0xa000;
                player->mPrevAngleY = player->mAngleY + 0x8000;
                ::Vector3 clsnPos = ray.GetClsnPos();
                func_ov002_020d8d10(self, &clsnPos);
            }
        }
    }
}
}

// @symbol func_ov002_020d8d10
/* recovered: shared common types */
/* func_ov002_020d8d10 at 0x020d8d10
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
struct Self { char pad0[8]; int state; char pad2[0x82]; s16 angle; };
extern "C" {
void func_ov002_020d8d10(struct Self *self, Vector3 *pos) {
    struct Callback;
    extern s16 data_02082214[];
    extern u32 PlayerNewHold13(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                               void *callback);
    Vector3_16 vec;

    vec.x = data_02082214[((unsigned short)(short)(self->angle + 0x8000) >> 4) * 2];
    vec.y = 0;
    vec.z = data_02082214[(((unsigned short)(short)(self->angle + 0x8000) >> 4) * 2) + 1];

    if (self->state != 2) {
        Particle::System::New(0, 0xbf, pos->x, pos->y, pos->z, (const Vector3_16f *)(&vec), 0);
        Particle::System::New(0, 0xc0, pos->x, pos->y, pos->z, 0, 0);
    } else {
        Particle::System::New(0, 0x12f, pos->x, pos->y, pos->z, (const Vector3_16f *)(&vec), 0);
        Particle::System::New(0, 0x130, pos->x, pos->y, pos->z, (const Vector3_16f *)(&vec), 0);
        Particle::System::New(0, 0x131, pos->x, pos->y, pos->z, 0, 0);
    }
}
}

// @symbol _ZN6Player4HurtERK7Vector3jihhh
/* recovered: named members + shared header, declarations from a shared header */
/* recovered: named members + shared header */
extern "C" {
int Player::Hurt(const Vector3 &a, u32 b, Fix12i dmg, u8 arg4, u8 arg5, u8 arg6) {
    extern void func_ov002_020d5cec(char* c);
    extern s16 Vec3_HorzAngle(const void* v0, const void* v1);
    extern void PlayerNewSimpleHold13(u32 id, int a, int b, int cc);
    extern int func_ov002_020d91e0(char* thiz, int damage, int doPre);
    extern Player::State data_ov002_02110094;
    extern Player::State data_ov002_0211010c;
    extern int AngleDiff(int, int);
    extern u16 data_ov002_020ff128[];
    extern Player::State data_ov002_021100ac;
    extern int func_ov002_020d82f0(void*);
    extern void func_ov002_020d91b8(char*, int);
    s16 ang;
    void* p360;
    int hasCap;

    if (func_ov002_020d82f0(((char *)this)) == 0) {
        return 0;
    }

    if (arg5 != 0) {
        if ((int)b < 2) {
            b += 2;
        }
    }

    hasCap = this->HasNoCap();
    if (hasCap != 0) goto L88;
    if (this->mCharacter == this->param1)
        goto L88;
    if (this->mIsMetal != 0)
        goto L88;
    if (b == 0) goto L88;

    Sound::PlayCharVoice(this->mCharacter, 0x22, this->mCamSpacePos);
    goto L9c;

L88:
    {
        int t = 0;
        if ((int)b > 1) t = 1;
        func_ov002_020d91b8(((char *)this), t);
    }

L9c:
    if (this->mIsMetal != 0) {
        b = 0;
    }
    p360 = *(void **)((char *)&this->mObjInMouth);
    if (p360 != 0) {
        int match = (((dActor_c *)p360)->actorID == 0xbf);
        if (match) {
            func_ov002_020d5cec((char*)p360);
            *(u16 *)((char *)&this->mStateFlags) &= ~2;
        }
        this->mUseAltBodyModel = 0;
        {
            int *p = (int *)&((dActor_c *)this->mObjInMouth)->mFlags;
            *p |= 0x80000;
        }
        {
            int *p = (int *)&((dActor_c *)this->mObjInMouth)->mFlags;
            *p &= ~0x40000;
        }
        {
            int *p = (int *)&((dActor_c *)this->mObjInMouth)->mFlags;
            *p &= ~0x20000;
        }
        *(void **)((char *)&this->mObjInMouth) = 0;
    }

    this->mStateStep = 0;

    if (b != 0) {
        int neq = 0;
        if (this->mCharacter != this->param1)
            neq = 1;
        if (neq)
            this->mStateStep = 2;
    }
    if ((int)b > 1) {
        this->mStateStep = 2;
    }
    if (this->mIsAirborne != 0) {
        this->mStateStep = 4;
    }

    ang = Vec3_HorzAngle(&this->mPos, &a);
    if (AngleDiff(this->mAngleY, ang) > 0x4000) {
        *(u8 *)((char *)&this->mStateStep) += 1;
    }
    this->mPrevAngleY = ang + 0x8000;
    if (this->mStateStep & 1) {
        this->mAngleY = this->mPrevAngleY;
    } else {
        this->mAngleY = ang;
    }

    if (arg4 == 1) {
        u16 tv = data_ov002_020ff128[this->param1];
        dmg = (dmg * tv) / 100;
    }
    this->mHorzSpeed = dmg;

    if (arg5 != 0) {
        u8 t = arg5 - 1;
        *(u8 *)((char *)&this->mStateStep) |= 0x10;
        this->mStateWork = t;
    }

    this->mAttackClsn.Unlink();
    this->mHurtDamage = b;

    if (this->mIsUnderwater == 0) {
        if (this->mHurtDamage != 0) {
            if (arg6 != 0) {
                volatile struct { int x, y, z; } v;
                int y = this->mPosY;
                int z = this->mPosZ;
                int x = this->mPosX;
                y = y + 0x50000;
                v.x = x;
                v.y = y;
                v.z = z;
                if (*(volatile int *)((char *)&this->mHurtDamage) == 1) {
                    Particle::System::NewSimple(0xcb, x, y, z);
                    Particle::System::NewSimple(0xcc, v.x, v.y, v.z);
                } else {
                    Particle::System::NewSimple(0xc9, x, y, z);
                    Particle::System::NewSimple(0xca, v.x, v.y, v.z);
                }
            }
        }

        this->ChangeState(data_ov002_02110094);
        if (func_ov002_020d91e0(((char *)this), b << 8, 1) != 0) {
            *(u8 *)((char *)&this->mStateStep) &= 1;
            this->ChangeState(data_ov002_0211010c);
        }
    } else {
        this->ChangeState(data_ov002_021100ac);
        func_ov002_020d91e0(((char *)this), b << 8, 1);
    }

    return 1;
}
}

// @symbol func_ov002_020d91b8
extern "C" {
void func_ov002_020d91b8(unsigned char *self, int a) {
    extern int data_ov002_020ff120[];
    Player *player = (Player *)self;
    Sound::PlayCharVoice(self[0x6d9], data_ov002_020ff120[a & 1], player->mCamSpacePos);
}
}

// @symbol func_ov002_020d91e0
extern "C" {
int func_ov002_020d91e0(Player *player, int damage, int doPre) {
    extern void func_ov002_020da9d4(void* p);
    extern u8 data_0209f2d8;
    extern char data_0209ee90[];
    if (doPre != 0)
        func_ov002_020da9d4(player);

    if (SaveData::HasPlayerLostCap()) {
        if (player->mHasWings == 0)
            damage = damage + (damage >> 1);
    }

    {
        int b = (int)(data_0209f2d8 == 1);
        if (b != 0) return 0;
    }

    if (*(int*)(data_0209ee90 + 0x248) == 0) {
        player->Heal(-damage);
    }
    if (player->GetHealth() != 0)
        return 0;

    player->Heal(-0x100);
    return 1;
}
}

// @symbol func_ov002_020d9298
extern "C" {
int func_ov002_020d9298(Player *player) {
    extern Player::State data_ov002_021105a4;
    extern Player::State data_ov002_021100f4;
    extern Player::State data_ov002_0211019c;
    if (player->IsState(data_ov002_021105a4))
        return 0;
    Sound::PlayCharVoice(player->mCharacter, 0x14, player->mCamSpacePos);
    if (player->IsState(data_ov002_021100f4)) {
        player->mVertSpeed = 0x40000;
        return 1;
    }
    if (player->IsState(data_ov002_0211019c)) {
        player->mJumpComboTimer = 0xa;
    }
    player->ChangeState(data_ov002_0211019c);
    return 1;
}
}

// @symbol _ZN6Player6BounceEi
/* recovered: named members + shared header, declarations from a shared header */
/* recovered: named members + shared header */
extern "C" {
void Player::Bounce(Fix12i f) {
    extern Player::State data_ov002_021105a4;
    extern Player::State data_ov002_0211019c;
    if (this->IsState(data_ov002_021105a4))
        return;
    if (this->GetHealth() == 0)
        return;
    this->ChangeState(data_ov002_0211019c);
    this->mVertSpeed = f;
    Sound::PlayCharVoice(this->mCharacter, 0x14, this->mCamSpacePos);
    Sound::PlayBank0(0xb6, this->mCamSpacePos);
}
}

// @symbol _ZN6Player19func_ov002_020d93acEv
/* recovered: shared common types */
extern "C" {
int Player::func_ov002_020d93ac()
{
    short GetAngleToCamera(int i);
    extern u8 data_0209f2d8;
    char *self = (char*)this;
    Vector3 pos;
    char *spawned;
    u32 chr;
    int xv, yv, sum, zv;
    int b;
    int ang, xx, zz;

    if (this->HasNoCap()) goto fail;

    chr = this->param1 & 0xff;
    if (this->mCharacter == chr) goto fail;
    if (chr == 3) goto fail;

    this->SetNewHatCharacter(this->mCharacter, 0, false);
    this->mHasNoCap = 1;

    yv = this->mPosY;
    zv = this->mPosZ;
    xv = this->mPosX;
    sum = yv + 0x82000;
    pos.x = xv;
    pos.y = sum;
    pos.z = zv;
    spawned = (char *)Spawn(0x10d, (chr << 8) | 5, pos, 0, this->mAreaId, -1);
    if (spawned == 0) goto fail;

    b = (data_0209f2d8 == 1);
    if (!b) {
        ang = GetAngleToCamera(this->mPlayerNo);
        zz = this->mPrevAngleZ;
        xx = this->mPrevAngleX;
        ang += 0x8000;
        ((dActor_c *)spawned)->mPrevAngleX = xx;
        ((dActor_c *)spawned)->mPrevAngleY = ang;
        ((dActor_c *)spawned)->mPrevAngleZ = zz;
    } else {
        ((dActor_c *)spawned)->mPrevAngleX = this->mPrevAngleX;
        ((dActor_c *)spawned)->mPrevAngleY = this->mPrevAngleY;
        ((dActor_c *)spawned)->mPrevAngleZ = this->mPrevAngleZ;
    }
    ((dActor_c *)spawned)->mHorzSpeed = 0xb000;
    return 1;
fail:
    return 0;
}
}

// @symbol _ZN6Player19func_ov002_020d94ccEv
/* recovered: shared common types */
extern "C" {
int Player::func_ov002_020d94cc()
{
    short GetAngleToCamera(int i);
    void GiveVsStars(int idx, int delta);
    extern u8 data_0209f2d8;
    extern s8 data_0209f310;
    char *self = (char*)this;
    Vector3 pos;
    dActor_c *sndobj;
    char *spawned;
    int ang, xx, zz;
    int xv, yv, sum, zv;
    int b;

    if ((&data_0209f310)[this->mPlayerNo] <= 0) goto fail;

    sndobj = SpawnSoundObj(3);
    b = (data_0209f2d8 == 1);
    if (b) {
        if (sndobj != 0)
            ((daSoundObj_c *)sndobj)->mSoundID = 0x50;
    }

    GiveVsStars(this->mPlayerNo, -1);

    yv = this->mPosY;
    zv = this->mPosZ;
    xv = this->mPosX;
    sum = yv + 0x82000;
    pos.x = xv;
    pos.y = sum;
    pos.z = zv;

    b = (data_0209f2d8 == 1);
    if (b)
        spawned = (char *)Spawn(0xb2, 0x10, pos, 0, this->mAreaId, -1);
    else
        spawned = (char *)Spawn(0xb3, 0x10, pos, 0, this->mAreaId, -1);

    if (spawned == 0) goto fail;

    ((daStar_c *)spawned)->func_ov002_020e7218(self, 0);

    b = (data_0209f2d8 == 1);
    if (!b) {
        ang = GetAngleToCamera(this->mPlayerNo);
        zz = this->mPrevAngleZ;
        xx = this->mPrevAngleX;
        ang += 0x8000;
        ((dActor_c *)spawned)->mPrevAngleX = xx;
        ((dActor_c *)spawned)->mPrevAngleY = ang;
        ((dActor_c *)spawned)->mPrevAngleZ = zz;
    } else {
        ((dActor_c *)spawned)->mPrevAngleX = this->mPrevAngleX;
        ((dActor_c *)spawned)->mPrevAngleY = this->mPrevAngleY;
        ((dActor_c *)spawned)->mPrevAngleZ = this->mPrevAngleZ;
    }
    ((dActor_c *)spawned)->mHorzSpeed = this->mHorzSpeed;
    return 1;
fail:
    return 0;
}
}

// @symbol _ZN6Player15St_Hurt_CleanupEv
/* recovered: named members + shared header, real C++ method */
// Player::St_Hurt_Cleanup - clear flag bit 0x80 in the u32 at this+0xb0, return 1.
// The u64-mask launder forces the base to materialize (add r2,this,#0xb0) so the
// load/store reuse it, matching the ROM instead of folding the offset into ldr/str.
int Player::St_Hurt_Cleanup()
{
    *(unsigned int *)((char *)&mFlags) &= ~0x80;
    return 1;
}

// @symbol _ZN6Player12St_Hurt_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Three-phase hurt state, sequenced by mStatePhase: 0 lands the hit (sound, voice,
 * knockback), 1 waits for horizontal speed to bleed off, 2 runs out the timer
 * and then hands back to the swimming or walking state depending on
 * mIsUnderwater. mStateWork is used as two nibbles here -- the high one
 * selects the hurt variant, the low one gates animation advance.
 */
/* the swim state */
/* the walk state */
extern "C" {
int Player::St_Hurt_Main()
{
    extern void func_ov002_020d9a4c(Player * c);
    extern void func_ov002_020d99a4(Player * self);
    extern void func_ov002_020d9aac(void* c);
    extern void Player_AdvanceAnims(void* c);
    extern int func_ov002_020e2c84(void *self);
    extern Player::State data_ov002_0211067c;
    extern Player::State data_ov002_0211013c;
    mIsBodyClsnEnabled = 1;
    if (mJumpComboTimer != 0) {
        mIsBodyClsnEnabled = 0;
    }

    switch (mStatePhase) {
    case 0:
        func_ov002_020d9a4c(this);
        func_ov002_020d99a4(this);
        if (mIsAirborne != 0) break;
        func_ov002_020d9aac(this);
        mStateTimer = 0;
        if ((mStateStep & 7) >= 2) {
            Sound::PlayBank0((u32)mGroundSoundType + 0x50, mCamSpacePos);
            Sound::PlayCharVoice(mCharacter, 0x11, mCamSpacePos);
        }
        if (mHurtDamage != 0) {
            u32 masked = mStateStep & 0xf0;
            if (masked == 0 || masked == 0x10) {
                func_ov002_020d94cc();
            }
            mHurtDamage = 0;
        }
        if (mIsInShallowWater == 0) {
            func_ov002_020bf9d4();
        }
        mStatePhase = 1;
        break;

    case 1:
        func_ov002_020d9a4c(this);
        func_ov002_020d99a4(this);
        if (mIsAirborne != 0) break;
        func_ov002_020d9aac(this);
        if (mHorzSpeed != 0) break;
        mStatePhase = 2;
        if ((mStateWork >> 4) == 2) {
            mStateTimer = 0x5a;
        }
        break;

    case 2:
        if (mStateTimer != 0) {
            func_ov002_020d98b4();
            return 1;
        }
        mFlags &= ~0x80;
        mStateWork &= 0xf0;
        if (FinishedAnim()) {
            if ((mStateStep & 1) == 0) {
                mPrevAngleY = mPrevAngleY + 0x8000;
                mAngleY = mPrevAngleY;
            }
            mIsTakingDamage = 0;
            if (mStateArg & 2) {
                mInvincibleTimer = 0x24;
            }
            if (mIsUnderwater != 0) {
                this->ChangeState(data_ov002_0211067c);
            } else {
                this->ChangeState(data_ov002_0211013c);
            }
        }
        break;

    default:
        break;
    }

    if ((mStateWork & 0xf) == 0) {
        Player_AdvanceAnims(this);
    }
    mPrevVertSpeed = mVertSpeed;
    if (mIsAirborne == 0) {
        func_ov002_020e2c84(this);
    }
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020d98b4Ev
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
void Player::func_ov002_020d98b4() {
    extern void MulVec3Mat4x3(void *a, void *b, Vector3 *out);
    extern void Vec3_MulScalar(Vector3 * out, Vector3 * in, int s);
    extern u32 PlayerNewHold14(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                               void *callback);
    int t = this->func_ov002_020beb38();
    this->mStateTimer -= t;
    {
        if ((s16)mStateTimer < 0)
            mStateTimer = 0;
    }
    {
        unsigned int id = this->GetBodyModelID(param1 & 0xff, 0);
        Matrix4x3 *bone = &mModels.mBodyModels[id]->data.transforms[15];
        unsigned int id2 = this->GetBodyModelID(param1 & 0xff, 0);
        Vector3 v;
        Vector3 s;
        MulVec3Mat4x3(&bone->m[9], &mModels.mBodyModels[id2]->mat4x3, &v);
        Vec3_MulScalar(&s, &v, 0x8000);
        s.y += 0x50000;
        mParticle1 = (s32)Particle::System::New((u32)mParticle1, 0xd6, s.x, s.y, s.z, 0, 0);
    }
    mFlags |= 0x80;
}
}

// @symbol func_ov002_020d99a4
extern "C" {
void func_ov002_020d99a4(Player *self) {
    extern u8 data_ov002_02109dcc[];
    extern int data_ov002_0210a6ec[];
    Player *player = (Player *)self;
    u8 idx;
    if (player->mIsAirborne == 0)
        return;
    idx = player->mStateStep;
    if (idx > 1) return;
    ((Player *)self)
        ->SetAnim(data_ov002_0210a6ec[idx & 7], 0x40000000, 0x1000,
                  (u16)(data_ov002_02109dcc[idx] + 1));
    player->mStateStep += 2;
    player->mStateWork |= 1;
}
}

// @symbol func_ov002_020d9a4c
extern "C" {
void func_ov002_020d9a4c(Player *c) {
    extern u8 data_ov002_02109dc4[];
    Player *player = (Player *)c;
    int idx = ((Player *)c)->GetBodyModelID(player->param1 & 0xff, 0);
    dExtAnmModel_c *anim = player->mModels.mBodyModels[idx];
    if (anim->WillHitFrame(data_ov002_02109dc4[player->mStateStep & 7]) != 0)
        player->mStateWork |= 1;
}
}

// @symbol func_ov002_020d9aac
extern "C" {
void func_ov002_020d9aac(char* c)
{
    Player *p = (Player *)c;
    int v;

    if ((p->mStateStep & 7) >= 2) {
        if (p->mPrevVertSpeed < 0) {
            p->mVertSpeed = -(p->mPrevVertSpeed >> 2);
            v = p->mVertSpeed;
            if (v <= 0x1000) {
                p->mVertSpeed = 0;
            } else if (v >= 0xa000) {
                p->mVertSpeed = 0xa000;
            }
            if (p->mHorzSpeed >= 0xa000)
                p->mHorzSpeed = 0xa000;
        }
    } else {
        if (p->mHorzSpeed >= 0x12000)
            p->mHorzSpeed = 0x12000;
    }

    if ((p->mStateStep & 7) < 2) {
        ApproachLinear(p->mHorzSpeed, 0, 0x800);
    } else {
        ApproachLinear(p->mHorzSpeed, 0, 0x1800);
    }
}
}

// @symbol _ZN6Player12St_Hurt_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Hurt_Init()
{
    extern char* data_0209f318;
    extern u32 data_ov002_0210a6d4[];
    extern void func_0200d89c(char*);
    int mid;
    char* m;
    u8 old;
    this->SetAnim(data_ov002_0210a6d4[mStateStep & 7], 0x40000000, 0x1000, 0);
    mid = this->GetBodyModelID(param1 & 0xff, 0);
    m = (char *)mModels.mBodyModels[mid];
    m = (char *)((unsigned int)&(dExtFrameCtrl_c &)*(dExtAnmModel_c *)m & 0xFFFFFFFFU);
    ((dExtFrameCtrl_c *)m)->currFrame = 0;
    mStatePhase = 0;
    old = mStateWork;
    mStateWork = 0;
    if ((mStateStep & 0xf0) == 0x10) mStateWork = old << 4;
    mStateArg = 0;
    if (mHurtDamage != 0) {
        mStateArg = 2;
        func_ov002_020d93ac();
    }
    mIsTakingDamage = 1;
    mPrevVertSpeed = mVertSpeed;
    func_0200d89c(data_0209f318);
    mJumpComboTimer = 5;
    return 1;
}
}

// @symbol _ZN6Player12GetHurtStateEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::GetHurtState()
{

    extern Player::State data_ov002_02110094;
  if(this->IsState(data_ov002_02110094))
    return mStateStep & 7;
  return -1;
}
}

// @symbol func_ov002_020d9c70
#pragma push
#pragma opt_common_subs off
#pragma opt_strength_reduction off
extern "C" {
void func_ov002_020d9c70(char *o)
{
    extern void func_ov002_020db54c(unsigned char *self, int a, int b, short c);
    Player *player = (Player *)o;
    short v = player->mAngleYSpeed;
    int absv;
    int A, Braw;
    short Bnarrow, Bresult;
    char *p;

    absv = (v < 0) ? (short)(-v) : v;
    A = FX_MUL(absv, 0x28000);

    absv = (v < 0) ? (short)(-v) : v;
    Braw = FX_MUL(absv, 0x1c71);
    Bnarrow = (short)Braw;

    Bresult = (v < 0) ? (short)(player->mAngleY - Bnarrow) : (short)(player->mAngleY + Bnarrow);

    p = (char *)player->mHeldObj;
    if (p == 0)
        return;

    {
        int t = ((dActor_c *)p)->actorID;
        t = t == 0xbf;
        if (t != false) {
            func_ov002_020db54c((unsigned char *)p, A, 0x20000, Bresult);
            return;
        }
    }

    ((dActor_c *)p)->mPrevAngleX = 0;
    ((dActor_c *)p)->mPrevAngleY = Bresult;
    ((dActor_c *)p)->mPrevAngleZ = 0;
    {
        char *q = (char *)player->mHeldObj;
        short *r = (short *)(int)(&((dActor_c *)q)->mPrevAngle);
        short first = r[0];
        ((dActor_c *)q)->mAngleX = first;
        ((dActor_c *)q)->mAngleY = r[1];
        ((dActor_c *)q)->mAngleZ = r[2];
    }
    ((dActor_c *)player->mHeldObj)->mHorzSpeed = A;
    {
        char *q = (char *)player->mHeldObj;
        ((dActor_c *)q)->mSpeedX = 0;
        ((dActor_c *)q)->mVertSpeed = 0x20000;
        ((dActor_c *)q)->mSpeedZ = 0;
    }
}
}
#pragma pop

// @symbol func_ov002_020d9dcc
/* The state test compares the comparison as an int,
 * `(int)(x == 0xbf) == 0`.  In C++ the comparison is a bool: tested
 * directly, the 0/1 is folded into the branch (12 bytes short), and
 * widened to long long, as the C shard spelled it, the early return's 0
 * is loaded ahead of the branch and reused below it (29 words differ).
 * The int view keeps the 0/1 and gives the return 0 its own path. */
extern "C" {
int func_ov002_020d9dcc(Player *player) {
    extern int AngleDiff(int a, int b);
    extern s16 data_0209f4a0[];
    extern Player::State data_ov002_02110604;
    char* p;

    if (player->param1 != 2)
        return 0;

    p = (char *)player->mHeldObj;
    if (p == 0 || (int)(((dActor_c *)p)->actorID == 0xbf) == 0)
        return 0;

    if (*(s16 *)((char *)data_0209f4a0 + gActivePlayerSlot * 0x18) == 0 ||
        player->mIsAirborne != 0) {
        player->mStateTimer = 0;
        return 0;
    }

    if (player->mStateTimer == 0) {
        player->mStateTimer = 0x3c;
        player->mAngleYSpeed = player->mAngleY;
        player->mStateWork = 0;
        return 0;
    }

    if (player->mStateWork == 0) {
        if (player->mAngleY < player->mAngleYSpeed)
            player->mStateWork = 0xff;
        else
            player->mStateWork = 1;
    }

    s16 diff = player->mAngleY - player->mPreClsnAngleY;
    if ((s8)player->mStateWork >= 0) {
        if (diff < 0) {
            player->mStateTimer = 0;
            return 0;
        }
        if (AngleDiff(player->mAngleY, player->mAngleYSpeed) >= 0x4000) {
            player->mStateWork += 1;
            player->mAngleYSpeed += 0x4000;
        }
    } else {
        if (diff > 0) {
            player->mStateTimer = 0;
            return 0;
        }
        if (AngleDiff(player->mAngleY, player->mAngleYSpeed) >= 0x4000) {
            player->mStateWork -= 1;
            player->mAngleYSpeed -= 0x4000;
        }
    }

    s8 v = (s8)player->mStateWork;
    if (v < 0)
        v = -v;
    if (v < 5)
        return 0;

    player->ChangeState(data_ov002_02110604);
    return 1;
}
}

// @symbol _ZN6Player22St_SwingPlayer_CleanupEv
/* recovered: named members + shared header, real C++ method */
int Player::St_SwingPlayer_Cleanup()
{
  char* p = *(char**)((char*)&mHeldObj);
  if (p) {
      ((dActor_c *)p)->mFlags &= ~0x800;
  }
  mPrevAngleY = mAngleY;
  return 1;
}


/* recovered: Player::St_SwingPlayer_Main, the per-frame body of the swing state (animation, particles, the approach helpers). */
// Provenance: first drafted from nearmiss/db.jsonl (divergence 8) and kept on main as a
// decompiled-not-matched draft; matched 2026-10-12 under mwccarm 2004/b56. The swing step
// is the s16 angle difference shifted IN PLACE (`diff >>= 6` / `diff >>= 7`), which keeps
// the difference and the step in one register across both if-converted arms, and the
// angular speed is declared before the difference, which gives it ip and the step r3.
extern "C" {
// @symbol _ZN6Player19St_SwingPlayer_MainEv
int Player::St_SwingPlayer_Main()
{
    char* c = (char*)this;
    typedef s32 Fix12;
    extern void func_ov002_020d9c70(char* c);
    extern int func_ov002_020da95c(char* c);
    extern int func_ov002_020da9d4(char* c);
    extern u32 PlayerNewHold15(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                               void *callback);
    extern void PlayerPlayLongHold_swing(u32 h, u32 a, u32 b, void *v, u32 d);
    extern void func_ov002_020dc174(char *c, const Vector3 *pos, Fix12i reach, Fix12i height,
                                    u32 flags, u32 a6);
    extern void Player_AdvanceAnims(char* c);
    extern u16 data_0209f49e[];
    extern s16 data_0209f4a0[];
    extern Player::State data_ov002_0211013c;
    switch (this->mStateStep) {
    case 0:
        if (this->FinishedAnim()) {
            ((Player *)c)->SetAnim(0x6d, 0x40000000, 0x1000, 0);
            this->mStateStep = 1;
        }
        this->mAngleY += this->mAngleYSpeed;
        break;
    case 1: {
        Vector3 offset;
        Vector3 v;
        if ((*(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18) & 1) != 0) {
            char *p = (char *)mHeldObj;
            if (p != 0) {
                int b = (((dActor_c *)p)->mFlags & 0x200) != 0;
                if (!b) {
                    func_ov002_020d9c70(c);
                    func_ov002_020da95c(c);
                    ((Player *)c)->SetAnim(0x6e, 0x40000000, 0x1000, 0);
                    this->mStateStep = 3;
                } else {
                    func_ov002_020da9d4(c);
                }
            }
            return 1;
        }
        if (*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) != 0) {
            s16 av = this->mAngleYSpeed;
            s16 diff = this->mDesiredAngleY - this->mPrevDesiredAngleY;
            if (av < 0) av = -av;
            if (av < 0x800) diff >>= 6; else diff >>= 7;
            if (diff != 0) {
                this->mAngleYSpeed += diff;
                if (this->mAngleYSpeed >= 0x1800)
                    this->mAngleYSpeed = 0x1800;
                else if (this->mAngleYSpeed <= -0x1800)
                    this->mAngleYSpeed = -0x1800;
            } else {
                ApproachLinear2(this->mAngleYSpeed, 0, 0x20);
            }
        } else {
            ApproachLinear2(this->mAngleYSpeed, 0, 0x80);
        }
        {
            s16 a2 = this->mAngleYSpeed;
            if (a2 < 0) a2 = -a2;
            if (a2 >= 0xf99) {
                u32 id = 0x132;
                v.x = this->mPosX;
                v.y = this->mPosY;
                v.z = this->mPosZ;
                v.y = this->mPosY + 0x3c000;
                if (this->mAngleYSpeed > 0)
                    id = 0x133;
                mParticle1 = Particle::System::New((u32)mParticle1, id, *(int *)&v.x, *(int *)&v.y,
                                                   *(int *)&v.z, 0, 0);
                mLoopingSoundHandle = Sound::PlayLong((u32)mLoopingSoundHandle, 0, 0x1c,
                                                      *(Vector3 *)&mCamSpacePosX, 0);
            }
            if (this->mAngleYSpeed != 0)
                this->mStateTimer = 0xa;
        }
        if (this->mStateTimer == 0) {
            ((Player *)c)->SetAnim(0x6f, 0x40000000, 0x1000, 0);
            this->mStateStep = 2;
        }
        {
            s16 before = this->mAngleY;
            this->mAngleY += this->mAngleYSpeed;
            if ((this->mAngleYSpeed <= -0x100 && before < this->mAngleY) ||
                (this->mAngleYSpeed >= 0x100 && before > this->mAngleY))
                Sound::PlayBank0(0x1b, mCamSpacePos);
        }
        offset.x = 0;
        offset.y = 0x32000;
        offset.z = 0x64000;
        func_ov002_020dc174(c, &offset, 0x32000, 0x32000, 0x80, 0);
        mAttackClsn.Clear();
        mAttackClsn.Update();
        break;
    }
    case 2:
        if (this->FinishedAnim())
            this->ChangeState(data_ov002_0211013c);
        break;
    case 3:
        if (this->FinishedAnim())
            this->ChangeState(data_ov002_0211013c);
        break;
    }

    Player_AdvanceAnims(c);
    return 1;
}
}

// @symbol _ZN6Player19St_SwingPlayer_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_SwingPlayer_Init()
{
  mHorzSpeed = 0;
  this->SetAnim(0x6c, 0x40000000, 0x1000, 0);
  mStateStep = 0;
  mAngleYSpeed = mAngleY - mPreClsnAngleY;
  {
      dActor_c *obj = *(dActor_c **)((char *)&mHeldObj);
      if (obj != 0)
          obj->mFlags |= 0x800;
  }
  return 1;
}
}

// @symbol _ZN6Player25St_GrabBowserTail_CleanupEv
/* recovered: named members + shared header, real C++ method */
int Player::St_GrabBowserTail_Cleanup()
{
    func_ov002_020daa74();
    return 1;
}

// @symbol _ZN6Player22St_GrabBowserTail_MainEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_GrabBowserTail_Main()
{
    extern void func_02012694(u32 id, void* v);
    extern void Player_AdvanceAnims(char* c);
    extern u16 data_0209f49e[];
    extern s16 data_0209f4a0[];
    extern char data_0209ee90[];
    extern Player::State data_ov002_02110154;
    switch (mStateStep) {
    case 0:
        if (this->FinishedAnim()) {
            mStateStep = 1;
            this->SetAnim(0x80, 0, 0x1000, 0);
        }
        break;
    case 1:
        if ((*(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18) & 1) != 0) {
            s16 x = mAngleYSpeed;
            if (x < 0) x = -x;
            if (x > 0xe00)
                Sound::PlayCharVoice(mCharacter, 0x32, mCamSpacePos);
            else
                Sound::PlayCharVoice(mCharacter, 0x33, mCamSpacePos);
            mStateStep = 2;
            this->SetAnim(0x7f, 0x40000000, 0x1000, 0);
            func_ov002_020daa74();
            return 1;
        }
        if (*(int*)(data_0209ee90 + 0x24c) != 0)
            mStateWork = 0;
        if (*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) == 0) {
            u8 v = mStateWork;
            (this->mStateWork)++;
            if (v > 0x78) {
                mStateStep = 2;
                this->SetAnim(0x7f, 0x40000000, 0x1000, 0);
                func_ov002_020daa74();
                return 1;
            }
        } else {
            mStateWork = 0;
        }
        if (*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) > 0x200) {
            if (mStateArg == 0) {
                mStateArg = 1;
                mPrevDesiredAngleY = mDesiredAngleY;
            } else {
                s16 xv = mAngleYSpeed;
                if (xv < 0) xv = -xv;
                s32 r3;
                if (xv < 0x800)
                    r3 = ((mDesiredAngleY - mPrevDesiredAngleY) << 10) >> 16;
                else
                    r3 = ((mDesiredAngleY - mPrevDesiredAngleY) << 9) >> 16;
                if (r3 < -0x80) r3 = -0x80;
                if (r3 > 0x80) r3 = 0x80;
                mAngleYSpeed += r3;
                if (mAngleYSpeed > 0x1000) mAngleYSpeed = 0x1000;
                if (mAngleYSpeed < -0x1000) mAngleYSpeed = -0x1000;
            }
        } else {
            mStateArg = 0;
            ApproachLinear2(this->mAngleYSpeed, 0, 0x40);
        }
        {
            s16 before = mAngleY;
            mAngleY += mAngleYSpeed;
            if ((mAngleYSpeed <= -0x100 && before < mAngleY) ||
                (mAngleYSpeed >= 0x100 && before > mAngleY))
                func_02012694(0xb4, &mCamSpacePos);
            s16 sp = mAngleYSpeed;
            if (sp >= 0)
                mAngleX = -sp;
            else
                mAngleX = sp;
        }
        break;
    case 2:
        if (this->FinishedAnim())
            this->ChangeState(data_ov002_02110154);
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player22St_GrabBowserTail_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_GrabBowserTail_Init()
{
    extern int* data_0209f318;
    extern void func_0200d10c(void*, unsigned char);
    this->SetAnim(0x81, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    mAngleYSpeed = 0;
    mStateWork = 0;
    mStateArg = 0;
    func_0200d10c((void *)data_0209f318, mPlayerNo);
    return 1;
}
}

// @symbol _ZN6Player9DropActorEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::DropActor()
{

    extern Player::State data_ov002_02110364;
    extern Player::State data_ov002_02110394;
    extern Player::State data_ov002_02110604;
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_021105d4;
    extern Player::State data_ov002_02110034;
    u32* s = *(u32**)((char*)&mHeldObj);
    int present = (s != 0);
    if (present) {
        int held = ((((dActor_c *)s)->mFlags & 0x200) != 0);
        if (!held) {
            u32 *s_ptr1 = &((dActor_c *)s)->mFlags;
            *s_ptr1 &= ~0x4000u;

            u32* s_ptr2 = *(u32**)((char*)&mHeldObj);
            u32 *field_ptr2 = &((dActor_c *)s_ptr2)->mFlags;
            *field_ptr2 &= ~0x100u;

            u32* s_ptr3 = *(u32**)((char*)&mHeldObj);
            u32 *field_ptr3 = &((dActor_c *)s_ptr3)->mFlags;
            *field_ptr3 &= ~0x400u;

            u32* s_ptr4 = *(u32**)((char*)&mHeldObj);
            u32 *field_ptr4 = &((dActor_c *)s_ptr4)->mFlags;
            *field_ptr4 &= ~0x2000u;

            u32* s_ptr5 = *(u32**)((char*)&mHeldObj);
            u32 *field_ptr5 = &((dActor_c *)s_ptr5)->mFlags;
            *field_ptr5 &= ~0x800u;

            u32* s_ptr6 = *(u32**)((char*)&mHeldObj);
            u32 *field_ptr6 = &((dActor_c *)s_ptr6)->mFlags;
            *field_ptr6 &= ~0x1000u;

            *(u32**)((char*)&mHeldObj) = 0;

            if (this->IsState(data_ov002_02110364) ||
                this->IsState(data_ov002_02110394) ||
                this->IsState(data_ov002_02110604))
            {
                this->ChangeState(data_ov002_0211013c);
            }
        } else {
            this->ChangeState(data_ov002_021105d4);
        }
        return 1;
    }
    {
        int b = (mObjInMouth != 0);
        if (!b) {
            return 0;
        }
        this->ChangeState(data_ov002_02110034);
        return 1;
    }
}
}

// @symbol func_ov002_020da95c
#pragma push
#pragma opt_propagation off
extern "C" {
int func_ov002_020da95c(char *c)
{
    Player *player = (Player *)c;
    int p = player->mHeldObj;
    int t = (p != 0);
    if (t == 0) return 0;
    ((dActor_c *)p)->mFlags &= ~0x800;
    ((dActor_c *)*(volatile int *)&player->mHeldObj)->mFlags &= ~0x4000;
    ((dActor_c *)*(volatile int *)&player->mHeldObj)->mFlags |= 0x1000;
    ((dActor_c *)*(volatile int *)&player->mHeldObj)->mFlags &= ~0x100;
    player->mHeldObj = 0;
    return 1;
}
}
#pragma pop

// @symbol func_ov002_020da9d4
extern "C" {
extern "C" int func_ov002_020da9d4(Player *self) {
    extern void func_ov002_020db54c(void* s, int a, int b, int c);
    char *s = (char *)self->mHeldObj;
    int b = (s != 0);
    int* p;
    if (!b)
        return 0;
    {
        int e = (((dActor_c *)s)->actorID == 0xbf);
        if (e)
            func_ov002_020db54c(s, 0x10000, 0x10000, self->mAngleY);
    }
    p = (int *)&((dActor_c *)self->mHeldObj)->mFlags;
    *p &= ~0x4000;
    p = (int *)&((dActor_c *)self->mHeldObj)->mFlags;
    *p |= 0x2000;
    p = (int *)&((dActor_c *)self->mHeldObj)->mFlags;
    *p &= ~0x100;
    self->mHeldObj = 0;
    return 1;
}
}

// @symbol _ZN6Player19func_ov002_020daa74Ev
extern "C" {
int Player::func_ov002_020daa74(){
    extern void func_ov002_020db54c(void* s, int a, int b, int c);
    char* self = (char*)this;
    char *s = (char *)mHeldObj;
    int b = (s != 0);
    if (!b)
        return 0;
    ((dActor_c *)s)->mFlags &= ~0x4000;
    ((dActor_c *)mHeldObj)->mFlags |= 0x400;
    ((dActor_c *)mHeldObj)->mFlags &= ~0x100;
    {
        char *s2 = (char *)mHeldObj;
        int e = (((dActor_c *)s2)->actorID == 0xbf);
        if (e) {
            int c_val = this->mAngleY;
            func_ov002_020db54c(s2, 0x30000, 0x10000, c_val);
        }
    }
    mHeldObj = 0;
    return 1;
}
}

// @symbol Player_ReleaseHeldActor
extern "C" {
int Player_ReleaseHeldActor(Player *c) {
    extern void func_ov002_020db54c(char* p, int a, int b, int d);
    char *p = (char *)c->mHeldObj;
    int ok = (int)(p != 0);
    int is_bf;
    if (ok == 0) return 0;
    is_bf = (int)(((dActor_c *)p)->actorID == 0xbf);
    if (is_bf != 0) {
        func_ov002_020db54c(p, 0x10000, 0x10000, c->mAngleY);
    }
    ((dActor_c *)c->mHeldObj)->mFlags &= ~0x4000;
    ((dActor_c *)c->mHeldObj)->mFlags &= ~0x100;
    c->mHeldObj = 0;
    return 1;
}
}

// @symbol _ZN6Player7TryGrabER8dActor_c
/* recovered: named members + shared header, real C++ method
 *
 * Refuses if already holding something or if the target does not carry the
 * grabbable flag (0x80). Grabbing is allowed from the punch state past step 2,
 * from swimming, or -- for actor IDs other than 0xbd/0xbe -- from the crouch
 * state.
 */
/* punch */
/* swim */
/* crouch */
extern "C" {
int Player::TryGrab(dActor_c & actor_)
{
    extern int func_ov002_020dacb4(void* p, void* actor);
    extern Player::State data_ov002_02110574;
    extern Player::State data_ov002_0211067c;
    extern Player::State data_ov002_021105bc;
    dActor_c *actor = &actor_;
    int b = (mHeldObj != 0);
    if (b) return 0;
    if ((actor->mFlags & 0x80) == 0) return 0;
    if (!(IsState(data_ov002_02110574) && mPunchKickStep != 2)) {
        if (!IsState(data_ov002_0211067c)) goto bd;
    }
    func_ov002_020dacb4(this, actor);
    return 1;
bd:
    {
        unsigned short t = actor->actorID;
        int b0 = (t == 0xbd);
        if (!b0) {
            int b1 = (t == 0xbe);
            if (!b1) {
                if (IsState(data_ov002_021105bc)) {
                    func_ov002_020dacb4(this, actor);
                    return 1;
                }
            }
        }
    }
    return 0;
}
}

// @symbol func_ov002_020dacb4
extern "C" {
int func_ov002_020dacb4(Player *p, char *actor) {
    extern int func_ov002_020cd28c(void *c);
    extern Player::State data_ov002_021105ec;
    extern Player::State data_ov002_021105bc;
    extern Player::State data_ov002_02110394;
    extern Player::State data_ov002_02110364;
    extern SharedFilePtr *data_ov002_020ff480[];
    int t;

    p->mHeldObj = (s32)actor;
    ((dActor_c *)p->mHeldObj)->mFlags &= ~0x400;
    ((dActor_c *)p->mHeldObj)->mFlags &= ~0x2000;
    ((dActor_c *)p->mHeldObj)->mFlags |= 0x100;
    p->mHorzSpeed = 0;

    if (p->mIsUnderwater != 0) {
        func_ov002_020cd28c(p);
    } else {
        t = 0;
        if (((dActor_c *)p->mHeldObj)->actorID == 0x116)
            t = 1;
        if (t != 0) {
            p->mStateStep = 0;
            p->ChangeState(data_ov002_021105ec);
            return 1;
        }
        if (!p->IsState(data_ov002_021105bc)) {
            unsigned short fc;
            int u;
            p->mStateStep = 0;
            fc = ((dActor_c *)p->mHeldObj)->actorID;
            t = 0;
            if (fc == 0xbd) t = 1;
            if (t != 0) goto chg394;
            u = fc;
            u = u == 0xbe;
            if (u == false) goto chg364;
        chg394:
            p->ChangeState(data_ov002_02110394);
            goto after;
        chg364:
            p->ChangeState(data_ov002_02110364);
        after: ;
        }
    }

    {
        char *a2 = (char *)p->mHeldObj;
        int offset = 0xc4;
        int s = ((dActor_c *)a2)->mFlags;
        s = (s & 0x8000) != false;
        if (s != false) {
            offset = 0x224;
            if (p->param1 == 2) {
                int c = ((dActor_c *)a2)->actorID;
                c = c == 0xce;
                if (c != false) offset = 0xc4;
            }
        }
        {
            int player_var = p->param1;
            int model_idx = p->GetBodyModelID(player_var & 0xff, 0);
            dExtAnm2Model_c *model = p->mModels.mBodyModels[model_idx];
            int j = (int)data_ov002_020ff480[offset + player_var]->filePtr;
            model->Func_020162C4(j, 0, 0x1000, 0);
        }
    }
    return 1;
}
}

// @symbol _ZN6Player13St_Throw_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
int Player::St_Throw_Main() {
    extern void func_ov002_020da9d4(void *);
    extern void Player_AdvanceAnims(void *);
    extern Player::State data_ov002_0211013c;
    if (mIsAirborne == 0) {
        ApproachLinear(mHorzSpeed, 0, 0x1000);
    }
    if (*(char **)((char *)&mHeldObj)) {
        int r4 = 5;
        int id = this->GetBodyModelID(param1 & 0xff, 0);
        dExtAnmModel_c *anim = mModels.mBodyModels[id];
        if (anim->WillHitFrame(0)) {
            if (func_ov002_020c19d0(0x40, 0x32))
                r4 = 0;
        }
        char *q = *(char **)((char *)&mHeldObj);
        unsigned short a = ((dActor_c *)q)->actorID;
        int t1 = (a == 0xbd);
        if (!t1) {
            int t2 = (a == 0xbe);
            if (!t2)
                goto Lb8;
        }
        r4 = 7;
        goto Ld0;
    Lb8: {
        int b = (((dActor_c *)q)->mFlags & 0x200) ? 1 : 0;
        if (b)
            r4 = 0xd;
    }
    Ld0:
        int id2 = this->GetBodyModelID(param1 & 0xff, 0);
        dExtAnmModel_c *anim2 = mModels.mBodyModels[id2];
        if (anim2->WillHitFrame(r4)) {
            int b2 = (((dActor_c *)mHeldObj)->mFlags & 0x200) ? 1 : 0;
            if (b2 == 0)
                func_ov002_020daa74();
            else
                func_ov002_020da9d4(((char *)this));
        }
    }
    if (this->FinishedAnim())
        this->ChangeState(data_ov002_0211013c);
    Player_AdvanceAnims(((char *)this));
    return 1;
}
}

// @symbol _ZN6Player13St_Throw_InitEv
/* recovered: named members + shared header, real C++ method
 *
 * mHeldObj is declared s32 in Player.h but is an actor pointer: this reads
 * fBase_c's mFlags (+0xb0) and actorID (+0xc) through it. dActor_c IDs 0xbd
 * and 0xbe get the alternate throw animation.
 */
extern "C" {
int Player::St_Throw_Init() {
    dActor_c *held = (dActor_c *)mHeldObj;
    int b = (held->mFlags & 0x200) ? 1 : 0;
    if (!b) {
        unsigned short a = held->actorID;
        int t1 = (a == 0xbd);
        if (!t1) {
            int t2 = (a == 0xbe);
            if (!t2)
                goto anim30;
        }
        this->SetAnim(0x91, 0x40000000, 0x1000, 0);
        goto voice;
    anim30:
        this->SetAnim(0x30, 0x40000000, 0x1000, 0);
    voice:
        Sound::PlayCharVoice(mCharacter, 0x13, mCamSpacePos);
    } else {
        this->SetAnim(0x8a, 0x40000000, 0x1000, 0);
    }
    return 1;
}
}

// @symbol _ZN6Player17St_Thrown_CleanupEv
/* recovered: named members + shared header, real C++ method */
// Player::St_Thrown_Cleanup - clear flag bit 0x2000 in the u32 at this+0x2ec, return 1.
// The u64-mask launder forces the base to materialize (add r2,this,#0x2ec) so the
// load/store reuse it, matching the ROM instead of folding the offset into ldr/str.
int Player::St_Thrown_Cleanup()
{
    *(unsigned int *)((char *)&mdCcAcPos_c.flags) &= ~0x2000;
    return 1;
}

// @symbol _ZN6Player14St_Thrown_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Thrown_Main()
{
    extern int atan2_hold_Player_7(Fix12i a, int b);
    extern void Player_AdvanceAnims(char* c);
    extern Player::State data_ov002_0211031c;
    extern Player::State data_ov002_0211010c;
    extern Player::State data_ov002_0211013c;
    extern int func_ov002_020e2c84(char*);
    if (mStateTimer == 1) {
        mdCcAcPos_c.flags |= 0x2000;
    }
    func_ov002_020bf90c();

    u8 state = mStateStep;
    switch (state) {
    case 0:
        if (mIsAirborne == 0) {
            mAngleX = 0;
            mdCcAcPos_c.flags &= ~0x2000;
            func_ov002_020c06fc(0x8000);
            if (mStateArg == 0) {
                mStateArg = 1;
                Sound::PlayCharVoice(mCharacter, 6, mCamSpacePos);
            }
            int r5 = func_ov002_020e2c84(((char*)this));
            if (r5 != 2 && mIsInShallowWater == 0) {
                func_ov002_020bf9d4();
            }
            if (r5 != 0) {
                return 1;
            }
            if (mFloorNormalY == 0x1000) {
                if (mHorzSpeed == 0) {
                    mStateStep = 1;
                    goto L1f8;
                }
            } else {
                this->ChangeState(data_ov002_0211031c);
                return 1;
            }
        }
        mAngleY = mPrevAngleY;
        mHorzSpeed = FX_MUL(mHorzSpeed, 0xfae);
        if (mIsAirborne != 0) {
            int a = cstd::atan2(mHorzSpeed, mVertSpeed);
            s16 b = a - 0x4000;
            if (b > 0x3000) b = 0x3000;
            mAngleX = b;
        } else {
            ApproachLinear2(*((s16 *)((char *)&mAngleX)), 0, 0x400);
        }
        break;
    case 1:
        if (this->GetHealth() == 0) {
            mStateStep = 1;
            this->ChangeState(data_ov002_0211010c);
            return 1;
        }
        if (this->FinishedAnim() != 0) {
            this->ChangeState(data_ov002_0211013c);
            mPrevAngleY = mAngleY;
        }
        Player_AdvanceAnims(((char*)this));
        break;
    }

L1f8:
    if (mIsAirborne == 0) {
        func_ov002_020e2c84(((char*)this));
    }
    return 1;
}
}

// @symbol _ZN6Player14St_Thrown_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Thrown_Init()
{
    int b = (mFlags & 0x400) ? 1 : 0;
    if (b)
        mStateTimer = 4;
    Sound::PlayCharVoice(mCharacter, 0x1d, mCamSpacePos);
    this->SetAnim(0x15, 0x40000000, 0x1000, 0x12);
    mStateStep = 0;
    mStateWork = 0;
    mAngleY = mPrevAngleY;
    mStateArg = 0;
    return 1;
}
}

// @symbol _ZN6Player18St_Grabbed_CleanupEv
/* recovered: named members + shared header, real C++ method
 *
 * Clears bit 1 of the body-collision flags, then if something is holding the
 * player at +0x35c and its actorID is 0xbf, tells it to drop and clears the
 * slot. Note the callee is Player::DropActor() --
 * invoked with THAT actor as its this, not the player's: whatever occupies
 * 0x35c is itself a Player. Kept as an extern "C" call rather than a method
 * call so the source does not assert a type this function cannot evidence.
 *
 * 0x35c is mGrabbedByActor, NOT mHeldObj at 0x358 -- those are different slots; the
 * object the player holds and the actor holding the player.
 */
extern "C" {
int Player::St_Grabbed_Cleanup()
{
    Player *a;
    *(u32*)&mdCcAcPos_c.flags &= ~2;
    a = (Player *)mGrabbedByActor;
    if (a) {
        int isBob = ((dActor_c *)a)->actorID == 0xbf;
        if (isBob) {
            a->DropActor();
            mGrabbedByActor = 0;
        }
    }
    return 1;
}
}

// @symbol _ZN6Player15St_Grabbed_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Grabbed_Main()
{
    extern void Player_AdvanceAnims(char* self);
    extern void func_ov002_020db54c(char*, int, int, int);
    char* grab;
    u8* p;
    unsigned int id;
    int ret;
    volatile int* pRate;
    char* model;

    mPeakY = mPosY;
    grab = *(char**)((char*)&mGrabbedByActor);
    if (grab != 0) {
        int isBob = (((dActor_c *)grab)->actorID == 0xbf);
        if (isBob != 0) {
            ret = func_ov002_020beb38();
            if (ret != 0) {
                p = (u8*)(int)((char*)&mStateStep);
                *p = (u8)(*p + ret);
                mStateTimer = 6;
                if ((s16)(mStateStep - ret) < 0) {
                    ((Player *)*(char **)((char *)&mGrabbedByActor))->DropActor();
                    func_ov002_020db54c(((char*)this), 0x10000, 0x10000, mAngleY);
                    return 1;
                }
            }
        }
    }

    if (mStateTimer != 0) {
        id = this->GetBodyModelID(param1 & 0xff, 0);
        model = (char *)mModels.mBodyModels[id];
        pRate = (int *)&(dExtFrameCtrl_c &)*(dExtAnmModel_c *)model;
        pRate = (int *)&((dExtFrameCtrl_c *)pRate)->speed;
        *pRate = 0x4000;
    } else {
        id = this->GetBodyModelID(param1 & 0xff, 0);
        model = (char *)mModels.mBodyModels[id];
        pRate = (int *)&(dExtFrameCtrl_c &)*(dExtAnmModel_c *)model;
        pRate = (int *)&((dExtFrameCtrl_c *)pRate)->speed;
        *pRate = 0x1000;
    }
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player15St_Grabbed_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Grabbed_Init()
{
    extern void func_ov002_020da9d4(void* c);
  func_ov002_020da9d4(((char*)this));
  *(int*)(int)((char*)&mdCcAcPos_c.flags) |= 2;
  mIsBodyClsnEnabled = 0;
  mSkipGroundUpdate = 1;
  Sound::PlayCharVoice(mCharacter, 6, mCamSpacePos);
  this->SetAnim(0x71, 0, 0x1000, 0);
  mHideShadow = 1;
  mStateStep = 0;
  return 1;
}
}

// @symbol func_ov002_020db54c
/* recovered: shared common types */
extern "C" {
extern "C" void func_ov002_020db54c(Player *p, int a, int b, short c) {
    extern void func_02035860(void *a, Vector3 *v);
    extern Player::State data_ov002_021101cc;
    void *node;
    p->mPrevAngleY = c;
    p->mHorzSpeed = a;
    p->mVertSpeed = b;
    node = (void *)p->mGrabbedByActor;
    if (node != 0) {
        int bb = (int)(((dActor_c *)node)->actorID == 0xbf);
        if (bb != 0) {
            Vector3 *src = (Vector3 *)((int)&((dActor_c *)node)->mPos);
            Vector3 v;
            int ty;
            v.x = src->x;
            ty = src->y;
            v.y = ty;
            v.z = src->z;
            v.y = ty + 0x64000;
            func_02035860(&p->mMeshClsn, &v);
            p->mPrevPosX = v.x;
            p->mPrevPosY = v.y;
            p->mPrevPosZ = v.z;
        }
    }
    p->mGrabbedByActor = 0;
    p->ChangeState(data_ov002_021101cc);
}
}

// @symbol func_ov002_020db5f4
extern "C" {
int func_ov002_020db5f4(Player *c, dActor_c *arg) {
    extern Player::State data_ov002_0211016c;
    extern int func_ov002_020d82f0(void *c);
    Player *player = (Player *)c;
    int b = (((dActor_c *)arg)->actorID == 0xbf);
    if (b == 0) {
        if (func_ov002_020d82f0(c) == 0)
            return 0;
    }
    if (player->mGrabbedByActor != 0)
        return 0;
    player->mGrabbedByActor = (int)arg;
    ((Player *)c)->ChangeState(data_ov002_0211016c);
    return 1;
}
}

// @symbol func_ov002_020db674
extern "C" {
int func_ov002_020db674(char* c, int a1, int a2, int a3)
{
    extern int func_ov002_020d82f0(void* c);
    extern Player::State data_ov002_02110094;
    Player *p = (Player *)c;
    if (func_ov002_020d82f0(c) == 0) return 0;
    Sound::PlayCharVoice(p->mCharacter, 0x1d, p->mCamSpacePos);
    p->mPrevAngleY = a3;
    p->mAngleY = a3 + 0x8000;
    p->mHorzSpeed = a1;
    p->mVertSpeed = a2;
    p->mHurtDamage = 0;
    c[0x6e3] = 4;
    c[0x6de] = 1;
    c[0x6df] = 0;
    ((Player *)c)->ChangeState(data_ov002_02110094);
    return 1;
}
}

// @symbol func_ov002_020db704
extern "C" {
void func_ov002_020db704(Player *self) {
    extern int data_ov002_020ff230[];
    extern int data_ov002_020ff2c0[];
    int s;
    u8 i;
    int j;

    s = self->mScaleAnimStep;
    if (s == 0)
        return;

    i = 0;
    j = 0;
    switch (self->mScaleAnimKind) {
    case 1:
        i = 2;
        j = 6;
    case 0:
        if (self->mScaleAnimTimer == 0) {
            if (s == 1)
                i++;
            if (ApproachLinear(self->mScaleY, data_ov002_020ff2c0[s - 1 + j],
                               data_ov002_020ff230[i])) {
                if (self->mScaleAnimStep == 1 || self->mScaleAnimStep == 6) {
                    self->mScaleAnimTimer = 10;
                } else {
                    self->mScaleAnimStep++;
                }
            }
        } else if (self->mScaleAnimTimer == 1) {
            if (s == 1) {
                self->mScaleAnimStep++;
            } else {
                self->mScaleAnimStep = 0;
                self->mScaleAnimTimer = 0;
                if (self->mIsMega) {
                    self->mScaleX = 0x3000;
                    self->mScaleY = 0x3000;
                    self->mScaleZ = 0x3000;
                    return;
                }
            }
        }
        self->mScaleZ = (0x1000 - self->mScaleY) / 2 + 0x1000;
        self->mScaleX = self->mScaleZ;
        return;
    case 2:
        if (ApproachLinear(self->mScaleY, 0x3000, 0x200)) {
            self->mScaleAnimStep = 0;
        }
        self->mScaleZ = self->mScaleY;
        self->mScaleX = self->mScaleZ;
        return;
    case 3:
        if (ApproachLinear(self->mScaleY, 0x1000, 0x200)) {
            self->mScaleAnimStep = 0;
        }
        self->mScaleZ = self->mScaleY;
        self->mScaleX = self->mScaleZ;
        return;
    }
}
}

// @symbol func_ov002_020db8bc
extern "C" {
void func_ov002_020db8bc(Player *p, unsigned char kind) {
    p->mScaleAnimStep = 1;
    p->mScaleAnimTimer = 0;
    p->mScaleAnimKind = kind;
}
}

// @symbol _ZN6Player19func_ov002_020db8d8Ev
/* recovered: shared common types */
extern "C" {
void Player::func_ov002_020db8d8()
{
    extern unsigned char data_0209f2d8;
    extern void MulVec3Mat4x3(void *out, void *mtx, void *vec);
    extern void Vec3_MulScalar(void *out, void *vec, int s);
    extern void func_ov002_020dc174(char *c, const Vector3 *pos, Fix12i reach, Fix12i height,
                                    u32 flags, u32 a6);
    char *self = (char*)this;
    volatile Vector3 backup;
    Vector3 tmp;
    Vector3 dir;
    Vector3 scaled;
    int model;
    Matrix4x3 *m;
    int b;
    int cond;
    backup.x = this->mPosX;
    backup.y = this->mPosY;
    backup.z = this->mPosZ;
    b = this->mUseAltBodyModel;
    m = &mModels.mHeadModels[(b << 2) + 3]->data.transforms[3];
    model = this->GetBodyModelID(param1 & 0xff, 0);
    MulVec3Mat4x3(&m->m[9], &mModels.mBodyModels[model]->mat4x3, &tmp);
    Vec3_MulScalar(&scaled, &tmp, 0x8000);
    this->mPosX = scaled.x;
    this->mPosY = scaled.y;
    this->mPosZ = scaled.z;
    dir.x = 0;
    dir.z = 0;
    dir.y = -0x32000;
    cond = (data_0209f2d8 == 1);
    if (cond != 0) {
        func_ov002_020dc174(self, &dir, 0x46000, 0x96000, 0x8000, 0);
    } else {
        func_ov002_020dc174(self, &dir, 0x32000, 0x64000, 0x8000, 0);
    }
    this->mPosX = backup.x;
    this->mPosY = backup.y;
    this->mPosZ = backup.z;
}
}

// @symbol func_ov002_020dba0c
extern "C" {
void func_ov002_020dba0c(char *c) {
    Player *player = (Player *)c;
    extern unsigned char data_0209f2d8;
    extern void func_ov002_020dc174(char *c, const Vector3 *pos, Fix12i reach, Fix12i height,
                                    u32 flags, u32 a6);
    Vector3 v;
    int r2, r3;
    unsigned int a5;
    if ((int)(data_0209f2d8 == 1) != 0) {
        v.z = 0xa0000;
        v.x = 0;
        r2 = 0x78000;
        v.y = 0;
        v.z = 0x78000;
        r3 = 0x50000;
    } else if (player->mIsMega == 0) {
        r2 = 0x78000;
        v.z = 0x78000;
        r3 = 0x50000;
        v.x = 0;
        v.y = 0;
        r2 = r3;
        if (player->param1 == 2) {
            v.z = 0xa0000;
            r2 = 0x78000;
        }
    } else {
        v.x = 0;
        v.y = -0x40000;
        v.z = 0x80000;
        r3 = 0x100000;
        r2 = 0xc0000;
    }
    a5 = 0x1400;
    if (player->param1 == 3)
        a5 = 0x400;
    func_ov002_020dc174(c, &v, r2, r3, a5, 0);
}
}

// @symbol func_ov002_020dbaec
extern "C" {
void func_ov002_020dbaec(char *c) {
    Player *player = (Player *)c;
    extern unsigned char data_0209f2d8;
    extern void func_ov002_020dc174(char *c, const Vector3 *pos, Fix12i reach, Fix12i height,
                                    u32 flags, u32 a6);
    Vector3 v;
    int r2, r3;
    if ((int)(data_0209f2d8 == 1) != 0) {
        v.z = 0xa0000;
        v.x = 0;
        r2 = 0x78000;
        v.y = 0;
        v.z = 0x78000;
        r3 = 0x50000;
    } else if (player->mIsMega == 0) {
        r2 = 0x78000;
        v.z = 0x78000;
        r3 = 0x50000;
        v.x = 0;
        v.y = 0;
        r2 = r3;
        if (player->param1 == 2) {
            v.z = 0xa0000;
            r2 = 0x78000;
        }
    } else {
        v.x = 0;
        v.y = -0x40000;
        v.z = 0x80000;
        r3 = 0x100000;
        r2 = 0xc0000;
    }
    func_ov002_020dc174(c, &v, r2, r3, 0x200, 0);
}
}

// @symbol func_ov002_020dbbc0
extern "C" {
void func_ov002_020dbbc0(char *c) {
    Player *player = (Player *)c;
    extern unsigned char data_0209f2d8;
    extern void func_ov002_020dc174(char *c, const Vector3 *pos, Fix12i reach, Fix12i height,
                                    u32 flags, u32 a6);
    Vector3 v;
    int r2, r3;
    if ((int)(data_0209f2d8 == 1) != 0) {
        v.z = 0xa0000;
        v.x = 0;
        r2 = 0x78000;
        v.y = 0;
        v.z = 0x78000;
        r3 = 0x50000;
    } else if (player->mIsMega == 0) {
        r2 = 0x78000;
        v.z = 0x78000;
        r3 = 0x50000;
        v.x = 0;
        v.y = 0;
        r2 = r3;
        if (player->param1 == 2) {
            v.z = 0xa0000;
            r2 = 0x78000;
        }
    } else {
        v.x = 0;
        v.y = -0x40000;
        v.z = 0x80000;
        r3 = 0x100000;
        r2 = 0xc0000;
    }
    func_ov002_020dc174(c, &v, r2, r3, 0x100, 0);
}
}

// @symbol func_ov002_020dbc94
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
void func_ov002_020dbc94(char* c) {
    extern unsigned char data_0209f2d8;
    extern void func_ov002_020dc174(char *c, const Vector3 *pos, Fix12i reach, Fix12i height,
                                    u32 flags, u32 a6);
    Player *player = (Player *)c;
    Vector3 v;
    int b = (int)(data_0209f2d8 == 1);
    if (b) {
        int r2 = 0x82000;
        v.x = 0;
        v.y = -0x32000;
        v.z = 0;
        if (player->param1 == 2)
            r2 = 0xc8000;
        func_ov002_020dc174(c, &v, r2, 0x5a000, 0x20, 0);
    } else if (player->mIsMega == 0) {
        v.x = 0;
        v.y = -0x32000;
        v.z = 0;
        func_ov002_020dc174(c, &v, 0x32000, 0x5a000, 0x20, 0);
    } else {
        v.x = 0;
        v.y = -0x40000;
        v.z = 0;
        func_ov002_020dc174(c, &v, 0xc0000, 0x80000, 0x20, 0);
    }
}
}

// @symbol func_ov002_020dbd94
extern "C" {
void func_ov002_020dbd94(char *c) {
    Player *player = (Player *)c;
    extern unsigned char data_0209f2d8;
    extern void func_ov002_020dc174(char *c, const Vector3 *pos, Fix12i reach, Fix12i height,
                                    u32 flags, u32 a6);
    Vector3 v;
    int r2, r3;
    if ((int)(data_0209f2d8 == 1) != 0) {
        v.z = 0x8c000;
        v.x = 0;
        v.y = 0xa000;
        v.z = 0x64000;
        r3 = 0xbe000;
        r2 = 0x78000;
    } else if (player->mIsMega == 0) {
        v.x = 0;
        v.y = 0xa000;
        v.z = 0x64000;
        r2 = 0x50000;
        r3 = 0xbe000;
        if (player->param1 == 2) {
            v.z = 0x8c000;
            r2 = 0x78000;
        }
    } else {
        v.x = 0;
        v.y = 0x40000;
        v.z = 0xd0000;
        r3 = 0x200000;
        r2 = 0x90000;
    }
    func_ov002_020dc174(c, &v, r2, r3, 0x80, 0xc0);
}
}

// @symbol func_ov002_020dbe70
extern "C" {
void func_ov002_020dbe70(char *c) {
    Player *player = (Player *)c;
    extern unsigned char data_0209f2d8;
    extern void func_ov002_020dc174(char *c, const Vector3 *pos, Fix12i reach, Fix12i height,
                                    u32 flags, u32 a6);
    Vector3 v;
    int r2, r3;
    if ((int)(data_0209f2d8 == 1) != 0) {
        v.z = 0x78000;
        v.x = 0;
        v.y = 0xa000;
        v.z = 0x5a000;
        r3 = 0x82000;
        r2 = 0x64000;
    } else if (player->mIsMega == 0) {
        v.x = 0;
        v.y = 0xa000;
        v.z = 0x5a000;
        r2 = 0x46000;
        r3 = 0x82000;
        if (player->param1 == 2) {
            v.z = 0x78000;
            r2 = 0x64000;
        }
    } else {
        v.x = 0;
        v.y = 0x40000;
        v.z = 0xe0000;
        r3 = 0x140000;
        r2 = 0xa0000;
    }
    func_ov002_020dc174(c, &v, r2, r3, 0x40, 0xc0);
}
}

// @symbol func_ov002_020dbf4c
extern "C" {
void func_ov002_020dbf4c(char *c) {
    Player *player = (Player *)c;
    extern unsigned char data_0209f2d8;
    extern void func_ov002_020dc174(char *c, const Vector3 *pos, Fix12i reach, Fix12i height,
                                    u32 flags, u32 a6);
    Vector3 v;
    int r2, r3;
    if ((int)(data_0209f2d8 == 1) != 0) {
        v.x = 0;
        v.y = 0xa000;
        v.z = 0x50000;
        r3 = 0x6e000;
        r2 = 0x5a000;
    } else if (player->mIsMega == 0) {
        v.x = 0;
        v.y = 0xa000;
        v.z = 0x50000;
        r2 = 0x3c000;
        r3 = 0x46000;
        if (player->param1 == 2) {
            v.z = 0x6e000;
            r2 = 0x5a000;
        }
    } else {
        v.x = 0;
        v.y = 0x40000;
        v.z = 0xc0000;
        r3 = 0x100000;
        r2 = 0x80000;
    }
    func_ov002_020dc174(c, &v, r2, r3, 0x40, 0xc0);
}
}

// @symbol func_ov002_020dc020
extern "C" {
void func_ov002_020dc020(char *c) {
    Player *player = (Player *)c;
    extern void func_ov002_020dc174(char *c, const Vector3 *pos, Fix12i reach, Fix12i height,
                                    u32 flags, u32 a6);
    Vector3 v;
    int r2, r3;
    if (player->mIsMega == 0) {
        v.x = 0;
        v.y = 0xa000;
        v.z = 0x3c000;
        r3 = 0x46000;
        r2 = 0x28000;
    } else {
        v.x = 0;
        v.y = 0x40000;
        v.z = 0xc0000;
        r3 = 0x100000;
        r2 = 0x80000;
    }
    func_ov002_020dc174(c, &v, r2, r3, 0x1000, 0);
}
}

// @symbol func_ov002_020dc09c
/* func_ov002_020dc09c at 0x020dc09c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
extern "C" {
void func_ov002_020dc09c(char *c) {
    Player *player = (Player *)c;
    extern void func_ov002_020dc174(char *c, const Vector3 *pos, Fix12i reach, Fix12i height,
                                    u32 flags, u32 a6);
    Vector3 v;
    int r2, r3;
    if (player->mIsMega == 0) {
        if (player->mStateTimer & 1) {
            r3 = 0x28000;
            v.x = 0;
            v.y = 0x28000;
            v.z = 0x87000;
            r2 = 0x3c000;
        } else {
            r3 = 0x50000;
            v.x = 0;
            v.y = 0x28000;
            v.z = 0x10e000;
            r2 = 0x50000;
        }
    } else {
        r2 = 0x80000;
        if (player->mStateTimer & 1) {
            v.x = 0;
            v.y = 0x80000;
            v.z = 0xc0000;
            r3 = 0x80000;
        } else {
            v.x = 0;
            v.y = 0x80000;
            v.z = 0x1c0000;
            r3 = 0x100000;
        }
    }
    func_ov002_020dc174(c, &v, r2, r3, 0x40000, 0);
}
}

// @symbol func_ov002_020dc174
extern "C" {
/* The attack collision's offset, reach and height. pos is a const Vector3 *: it
   goes straight to dCcAcPos_c::Init, whose ROM symbol mangles that
   parameter as RK7Vector3, a const Vector3 &. Eleven callers in this file each used to spell it void *. */
void func_ov002_020dc174(char *c, const Vector3 *pos, Fix12i reach, Fix12i height, u32 flags,
                         u32 a6) {
    Player *player = (Player *)c;
    int f = player->mIsMega ? 0x10 : 0;
    player->mAttackClsn.Init((dActor_c *)c, *pos, reach, height, flags | 2 | f, a6);
}
}

// @symbol _ZN6Player17St_SlideKick_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_SlideKick_Main()
{
    extern int func_ov002_020dd2f4(void* c);
    extern void Player_AdvanceAnims(void* c);
    extern Player::State data_ov002_021104e4;
    extern u16 data_0209f49e[];
    extern Player::State data_ov002_021101b4;
    extern Player::State data_ov002_021101fc;
    extern void func_ov002_020dbaec(void*);
    func_ov002_020bf90c();
    if (mIsAirborne == 0) {
        mIsSlidingOnGround = 1;
        func_ov002_020c06fc(0x4000);
        func_ov002_020dd2f4(((char*)this));
    }

    if (mHorzSpeed != 0)
        goto La8;

    if (this->FinishedAnim() == 0)
        goto L1d8;

    if (this->IsAnim(0x67) == 0)
        goto L88;

    this->SetAnim(0x66, 0x40000000, 0x1000, 0);
    goto L1d8;

L88:
    mPrevAngleY = mAngleY;
    mStateStep = 1;
    this->ChangeState(data_ov002_021104e4);
    goto L1d8;

La8:
    if (func_ov002_020c0688() == 0)
        goto L14c;

    if (mStateWork == 0) {
        if (mPrevVertSpeed < 0) {
            u16 bit = mStateFlags & 1;
            if (bit == 0) {
                int t = -mPrevVertSpeed;
                if (t > 0x14000) t = 0x14000;
                mVertSpeed = t;
                mStateWork = 1;
            }
        }
    }

    {
        u16 r1 = *(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18);
        if ((r1 & 1) || (r1 & 2)) {
            if (mSlideType == 0) {
                this->ChangeState(data_ov002_021101fc);
            }
        }
    }
    mStateStep = 0;
    goto L18c;

L14c:
    mStateStep++;
    if (mStateStep <= 0x1e)
        goto L18c;
    if (mPosY - mGroundY <= 0x64000)
        goto L18c;
    this->ChangeState(data_ov002_021101b4);

L18c:
    {
        int id = this->GetBodyModelID(param1 & 0xff, 0);
        dExtAnmModel_c *anim = mModels.mBodyModels[id];
        u32 w = ((dExtFrameCtrl_c *)((long long)(int)&(dExtFrameCtrl_c &)*anim))->currFrame;
        if ((u16)(w >> 12) < 4)
            goto L1d8;
        func_ov002_020dbaec(((char*)this));
        mAttackClsn.Clear();
        mAttackClsn.Update();
    }

L1d8:
    Player_AdvanceAnims(((char*)this));
    mPrevVertSpeed = mVertSpeed;
    return 1;
}
}

// @symbol _ZN6Player17St_SlideKick_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_SlideKick_Init()
{
    extern int RandomIntInternal(int* seed);
    extern int data_ov002_0210e160;
    extern int data_ov002_020ff130[];
    Sound::PlayBank0(0x11, mCamSpacePos);
    unsigned int r = (unsigned int)RandomIntInternal(&data_ov002_0210e160);
    Sound::PlayCharVoice(mCharacter, data_ov002_020ff130[(r >> 4) & 1], mCamSpacePos);
    this->SetAnim(0x67, 0x40000000, 0x1000, 0);
    int *yv = &mHorzSpeed;
    mVertSpeed = 0x14000;
    yv[0] += 0xf000;
    if (mHorzSpeed > 0x28000) mHorzSpeed = 0x28000;
    mStateWork = 0;
    mIsSlidingOnGround = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    return 1;
}
}

// @symbol _ZN6Player17St_SweepKick_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_SweepKick_Main()
{
    extern int func_ov002_020d8a50(void*,int);
    extern int Player_AdvanceAnims(void*);
    extern unsigned short data_0209f49e[];
    extern Player::State data_ov002_021104e4;
    extern Player::State data_ov002_0211019c;
  if(*(unsigned short*)((char*)data_0209f49e + gActivePlayerSlot*0x18) & 2)
    this->ChangeState(data_ov002_0211019c);
  func_ov002_020d8a50(((void*)this),3);
  if(this->FinishedAnim()){
    *(char*)((char*)&mStateStep)=1;
    this->ChangeState(data_ov002_021104e4);
  }
  Player_AdvanceAnims(((void*)this));
  return 1;
}
}

// @symbol _ZN6Player17St_SweepKick_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_SweepKick_Init()
{
    this->SetAnim(0x68, 0x40000000, 0x1000, 0);
    mHorzSpeed = 0;
    mAttackClashed = 0;
    /* mCamSpacePosX/Y/Z are three consecutive words; dActor_c.h declares them
       individually rather than as one Vector3. */
    Sound::PlayCharVoice(mCharacter, 0xa, mCamSpacePos);
    return 1;
}
}

// @symbol func_ov002_020dc560
#define ADDR(p) ((int *)(((long long)(int)(p))))
extern "C" {
void func_ov002_020dc560(char *self)
{
    extern int func_01ffa344(int a, int b);
    extern int AngleDiff(int a, int b);
    extern void ApproachAngle(short *cur, short target, int divisor, int band, int maxStep);
    extern int func_01ffa4bc(int a);
    extern int func_01ff99a4(int a, int b);
    extern int func_01ff98f4(int a, int b);
    extern char data_0209f4a0[];
    extern short data_02082214[];
    Player *player = (Player *)self;
    ApproachLinear(player->mHorzSpeed, 0, 0x599);

    {
        int tblval = *(short *)(data_0209f4a0 + gActivePlayerSlot * 0x18);
        if (tblval != 0) {
            short diff = (short)(player->mDesiredAngleY - player->mPrevAngleY);
            int idx2 = ((unsigned short)diff >> 4) * 2;
            int r1 = func_01ffa344(0x48400000, (unsigned short)diff >> 4);
            int sinv = data_02082214[idx2 + 1];
            int scaled = FX_MUL(sinv, 0x1800);
            int combined = FX_MUL(scaled, tblval);
            ApproachLinear(player->mHorzSpeed, r1, combined);

            {
                short *p94 = (short *)ADDR(&player->mPrevAngleY);
                int cosv = data_02082214[idx2];
                int tblval2 = *(short *)(data_0209f4a0 + gActivePlayerSlot * 0x18);
                int scaled2 = FX_MUL(cosv, 0x200);
                int combined2 = FX_MUL(scaled2, tblval2);
                *p94 = (short)(*p94 + combined2);
            }
        }
    }

    {
        short cur = player->mPrevAngleY;
        int ad = AngleDiff(cur, player->mAngleY);
        short target = cur;
        if (ad >= 0x4000)
            target = (short)(player->mPrevAngleY + 0x8000);
        ApproachAngle(&player->mAngleY, target, 8, 0x4000, 0x10);
    }

    if (func_01ff99a4(func_01ffa4bc(player->mHorzSpeed), 0x48400000) != 0) {
        *ADDR(&player->mHorzSpeed) -= 0x1000;
    }

    if (func_01ff98f4(func_01ffa4bc(player->mHorzSpeed), -0x3e800000) != 0) {
        *ADDR(&player->mHorzSpeed) += 0x2000;
    }
}
}
#undef ADDR

// @symbol _ZN6Player17St_ButtSlide_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_ButtSlide_Main()
{
    extern int func_ov002_020dd2f4(char* c);
    extern int func_0201226c(int a0, int a1, int a2, char* a3, int a4, int a5);
    extern void func_ov002_020e25f0(char* c, int a);
    extern void func_ov002_020c18b0(char* c, int a);
    extern void func_ov002_020dc560(char* c);
    extern void Player_AdvanceAnims(char* c);
    extern u16 data_0209f49e[];
    extern Player::State data_ov002_021101b4;
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_0211019c;
    extern void func_ov002_020dcafc(char*);
    switch (mStatePhase) {
    case 0:
        func_ov002_020bf90c();
        if (mIsAirborne == 0) {
            mIsSlidingOnGround = 1;
            func_ov002_020c06fc(0x4000);
            func_ov002_020dd2f4(((char*)this));
        }
        if (mHorzSpeed == 0) {
            if (mSlideType == 0) {
                mStatePhase = 2;
                mStateWork = 0;
                break;
            }
            (this->mSlideStoppedTimer)++;
            if (mSlideStoppedTimer >= 0x1e) {
                mStatePhase = 2;
                mStateWork = 0;
                break;
            }
        } else {
            mSlideStoppedTimer = 0;
        }
        if (func_ov002_020c0688() != 0) {
            if ((*(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18) & 2) != 0
                && mStateWaitTimer == 0) {
                mJumpComboStage = 0;
                this->ChangeState(data_ov002_0211019c);
                return 1;
            }
            mLoopingSoundHandle = func_0201226c(mLoopingSoundHandle, 0, mGroundSoundType + 0xe2,
                                                (char *)&mCamSpacePos, mHorzSpeed, 0);
        } else {
            mStateStep = 0;
            mStatePhase = 1;
            mStateWork = 0;
            if ((mStateArg | mSlideType) != 0)
                mVertSpeed = 0x15000;
            func_ov002_020e25f0(((char*)this), 0);
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
            break;
        }
        func_ov002_020c18b0(((char*)this), 0);
        func_ov002_020dcafc(((char*)this));
        break;
    case 1:
        mAngleZ = 0;
        mAngleX = mAngleZ;
        if (mIsAirborne == 0) {
            if (mLandSoundPlayed == 0) {
                mLandSoundPlayed = 1;
                Sound::PlayBank0(mGroundSoundType + 0x50, mCamSpacePos);
            }
            if (mStateWork == 0 && mFloorNormalY >= 0xfc1) {
                mVertSpeed = -mPrevVertSpeed / 2;
                (this->mStateWork)++;
            } else {
                mStatePhase = 0;
                mVertSpeed = 0;
                break;
            }
        } else {
            func_ov002_020dc560(((char*)this));
        }
        (this->mStateStep)++;
        if (mStateStep > 0x1e
            && mPosY - mGroundY > 0x1f4000) {
            this->ChangeState(data_ov002_021101b4);
            return 1;
        }
        break;
    case 2:
        mAngleZ = 0;
        mAngleX = mAngleZ;
        if (mStateWork == 0) {
            this->SetAnim(0x42, 0x40000000, 0x1000, 0);
            mPrevAngleY = mAngleY;
            mStateWork = 1;
        } else if (this->FinishedAnim() != 0) {
            this->ChangeState(data_ov002_0211013c);
        }
        break;
    }

    mPrevVertSpeed = mVertSpeed;
    Player_AdvanceAnims(((char*)this));
    mStateArg = mSlideType;
    return 1;
}
}

// @symbol _ZN6Player17St_ButtSlide_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_ButtSlide_Init() {
    struct dCamera_c;
    extern struct dCamera_c *data_0209f318;
    extern void func_0200d544(struct dCamera_c * thiz, unsigned char playerID);
    this->SetAnim(0x41, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    if (mSlideType) {
        func_0200d544(data_0209f318, mPlayerNo);
    }
    mStatePhase = 0;
    mStateArg = mSlideType;
    mStateWaitTimer = 6;
    mIsSlidingOnGround = 0;
    return 1;
}
}

// @symbol func_ov002_020dcafc
/* func_ov002_020dcafc at 0x020dcafc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
extern "C" {
void func_ov002_020dcafc(char* c) {
    extern int AngleDiff(int a, int b);
    extern int ApproachAngle(void* p, int a, int b, int c, int d);
    extern short data_0209f4a2[];
    extern short data_0209f4a4[];
    Player *player = (Player *)c;
    s16 v5 = 0, v4 = 0;
    int idx;
    s16 a, b;

    if (player->mHorzSpeed != 0) {
        idx = gActivePlayerSlot * 0x18;
        a = *(short*)((char*)data_0209f4a2 + idx);
        b = *(short*)((char*)data_0209f4a4 + idx);
        v5 = (s16)FX_MUL(a, 0xaab);
        v4 = (s16)FX_MUL(-b, 0x1555);
    }

    if (AngleDiff(player->mAngleY, player->mPrevAngleY) >= 0x4000) {
        v5 = (s16)(-v5);
        v4 = (s16)(-v4);
    }

    ApproachAngle(&player->mSpineRot[1], v5, 8, 0x200, 0x20);
    ApproachAngle(&player->mSpineRot[2], v4, 8, 0x200, 0x20);
}
}

// @symbol _ZN6Player20St_StomachSlide_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_StomachSlide_Main()
{
    extern int func_ov002_020dd2f4(void* c);
    extern int func_ov002_020e2ea0(void* c);
    extern void func_ov002_020c18b0(void* c, u32 a);
    extern void func_ov002_020e25f0(void* c, int a);
    extern void func_ov002_020dc560(void* c);
    extern int func_ov002_020e0ccc(void* c, short* st);
    extern Fix12i atan2_hold_Player_6(Fix12i a, Fix12i b);
    extern void Player_AdvanceAnims(void* c);
    extern u16 data_0209f49e[];
    extern u8 data_0209ee90;
    extern Player::State data_ov002_021105bc;
    extern Player::State data_ov002_0211031c;
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_021101b4;
    extern Player::State data_ov002_021101fc;
    extern void func_ov002_020dba0c(void*);
    dActor_c *light0 = *(dActor_c **)((char *)&mHeldObj);
    if (light0 != 0) {
        u32 *p = &light0->mFlags;
        *p |= 0x4000;
    }

    switch (mStatePhase) {
    case 0:
        func_ov002_020bf90c();
        if (mIsAirborne == 0) {
            mIsSlidingOnGround = 1;
            if (this->IsState(data_ov002_021105bc)) {
                if (mHeldObj != 0) {
                    mHorzSpeed = 0;
                    goto case0_e0;
                }
            }
            {
                u32 arg = 0x4000;
                if (mSlideType == 0) arg = 0x8000;
                func_ov002_020c06fc(arg);
            }
            func_ov002_020dd2f4(((char*)this));
            if (this->IsState(data_ov002_021105bc) &&
                (u16)(mStateFlags & 1)) {
                func_ov002_020c0364(2);
            }
        }
    case0_e0:
        if (mHorzSpeed == 0) {
            if (mSlideType == 0) {
                mStateStep = 0;
                mStatePhase = 2;
                goto end;
            }
            {
                u8* p = (u8*)((char*)&mSlideStoppedTimer);
                *p = (u8)(*p + 1);
            }
            if (mSlideStoppedTimer >= 0x1e) {
                mStateStep = 0;
                mStatePhase = 2;
                goto end;
            }
        } else {
            mSlideStoppedTimer = 0;
        }

        if (func_ov002_020c0688()) {
            if (func_ov002_020e2ea0(((char*)this))) {
                return 1;
            }
            if (this->IsAnim(0x40) && this->FinishedAnim()) {
                this->SetAnim(0x43, 0x40000000, 0x1000, 0);
            }
            {
                u16 flags = *(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18);
                if ((flags & 1) || (flags & 2)) {
                    if (!(u16)(mStateFlags & 1)) {
                        this->ChangeState(data_ov002_021101fc);
                    }
                }
            }
            func_ov002_020c18b0(((char*)this), 0);
        } else {
            if (this->IsState(data_ov002_0211031c)) {
                mStatePhase = 1;
                mStateStep = 0;
                if (*(int*)((char*)&data_0209ee90 + 0x244) != 0) {
                    if ((mStateArg | mSlideType) != 0) {
                        mVertSpeed = 0x15000;
                    }
                }
                func_ov002_020e25f0(((char*)this), 0);
                this->SetAnim(0x54, 0x40000000, 0x1000, 0);
            }
        }
        func_ov002_020dba0c(((char*)this));
        mAttackClsn.Clear();
        mAttackClsn.Update();
        goto end;

    case 1:
        func_ov002_020dba0c(((char*)this));
        mAttackClsn.Clear();
        mAttackClsn.Update();
        if (mIsAirborne == 0) {
            mStatePhase = 0;
            mVertSpeed = 0;
            mStateStep = 0;
            this->SetAnim(0x43, 0x40000000, 0x1000, 0);
            goto end;
        }
        func_ov002_020dc560(((char*)this));
        {
            u8* p = (u8*)((char*)&mStateStep);
            *p = (u8)(*p + 1);
        }
        if (mStateStep <= 0x1e) goto end;
        if (mPosY - mGroundY <= 0x1f4000) goto end;
        this->ChangeState(data_ov002_021101b4);
        return 1;

    case 2:
        mAngleZ = 0;
        mAngleX = mAngleZ;
        if (mStateStep == 0) {
            dActor_c *light = *(dActor_c **)((char *)&mHeldObj);
            int haveLight = (light != 0);
            if (haveLight) {
                u32 animId = 0x18;
                int b1 = (light->mFlags & 0x8000) != 0;
                if (b1) {
                    animId = 0x8b;
                    if (param1 == 2) {
                        int b2 = (light->actorID == 0xce);
                        if (b2) {
                            animId = 0x18;
                        }
                    }
                }
                this->SetAnim(animId, 0x40000000, 0x1000, 0);
            } else {
                this->SetAnim(0x3e, 0x40000000, 0x1000, 0);
            }
            mPrevAngleY = mAngleY;
            {
                u8* p = (u8*)((char*)&mStateStep);
                *p = (u8)(*p + 1);
            }
            goto end;
        }
        if (this->FinishedAnim()) {
            if (func_ov002_020e0ccc(((char*)this), *(short**)((char*)&mHeldObj))) {
                return 1;
            }
            this->ChangeState(data_ov002_0211013c);
        }
        goto end;
    }

end:
    if (mIsAirborne != 0) {
        s16 v = cstd::atan2(mHorzSpeed >> 8, mVertSpeed >> 8) - 0x4000;
        if (v < 0) v = 0;
        if (v >= 0x2aaa) v = 0x2aaa;
        ApproachLinear2(*((short *)((char *)&mAngleX)), v, 0x200);
    }
    Player_AdvanceAnims(((char*)this));
    mStateArg = mSlideType;
    return 1;
}
}

// @symbol _ZN6Player20St_StomachSlide_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_StomachSlide_Init() {
    struct dCamera_c;
    extern struct dCamera_c *data_0209f318;
    extern void func_0200d544(struct dCamera_c * thiz, unsigned char playerID);
    this->SetAnim(0x40, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    if (mSlideType) {
        func_0200d544(data_0209f318, mPlayerNo);
    }
    mStateWork = 0;
    mStatePhase = 0;
    mSlideStoppedTimer = 0;
    mStateArg = mSlideType;
    mIsSlidingOnGround = 0;
    return 1;
}
}

// @symbol _ZN6Player12St_Dive_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Dive_Init()
{
    extern int RandomIntInternal(int* seed);
    extern int data_ov002_0210e160;
    extern int data_ov002_020ff100[];
    Sound::PlayBank0(0x11, mCamSpacePos);
    unsigned int r = (unsigned int)RandomIntInternal(&data_ov002_0210e160);
    Sound::PlayCharVoice(mCharacter, data_ov002_020ff100[(r >> 4) & 1], mCamSpacePos);
    this->SetAnim(0x40, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    int *yv = &mHorzSpeed;
    mVertSpeed = 0x1e000;
    yv[0] += 0xf000;
    if (mHorzSpeed > 0x28000) mHorzSpeed = 0x28000;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    mStateWork = 0;
    mStatePhase = 0;
    mSlideStoppedTimer = 0;
    mIsSlidingOnGround = 0;
    mStateArg = mSlideType;
    return 1;
}
}

// @symbol _ZN6Player8IsDivingEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::IsDiving()
{

    extern Player::State data_ov002_021105bc;
  return this->IsState(data_ov002_021105bc) != 0;
}
}

// @symbol _ZN6Player17LostGrabbedObjectEv
/* recovered: named members + shared header, real C++ method */
int Player::LostGrabbedObject()
{
  return ((Player *)(((void*)this)))->IsAnim(0x18) || ((Player *)(((void*)this)))->IsAnim(0x8b);
}

// @symbol _ZN6Player14IsFrontSlidingEv
/* recovered: named members + shared header, real C++ method */
int Player::IsFrontSliding()
{
  return ((Player *)(((void*)this)))->IsAnim(0x43) || ((Player *)(((void*)this)))->IsAnim(0x40);
}

// @symbol func_ov002_020dd2f4
extern "C" {
int func_ov002_020dd2f4(Player *p) {
    extern int atan2_hold_Player_5(Fix12i y, Fix12i x);
    extern int AngleDiff(int a, int b);
    int atan;

    if (!(p->mClsnFlags & 2))
        return 0;

    atan = cstd::atan2(p->mWallNormalX, p->mWallNormalZ);
    if (AngleDiff(atan, p->mPrevAngleY) < 0x6000)
        return 0;

    if (p->func_ov002_020c0cbc() != 0) {
        p->mHorzSpeed = FX_MUL(p->mHorzSpeed, 0xe66);
        if (p->mHorzSpeed < 0x4000)
            p->mHorzSpeed = 0x4000;
        p->mPrevAngleY = atan - (s16)(p->mPrevAngleY - atan) + 0x8000;
    }
    return 0;
}
}

// @symbol _ZN6Player17St_PunchKick_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_PunchKick_Main()
{
    extern void func_ov002_020d8a50(void* c, u32 a);
    extern void Player_AdvanceAnims(char* self);
    extern u16 data_0209f49e[];
    extern s16 data_0209f4a0[];
    extern Player::State data_ov002_0211013c;
    extern void func_ov002_020dd5ec(void*);
    extern int func_ov002_020e2c84(char*);
    u16 st = mStateTimer;

    if (st != 0) {
        if (st == 1) {
            func_ov002_020dd5ec(((char*)this));
        }
    } else {
        if (mPunchKickCooldown == 0) {
            u8 idx = gActivePlayerSlot;
            u16 flags = *(u16*)((char*)data_0209f49e + idx * 0x18);
            if ((flags & 1) != 0 && mPunchKickStep < 2) {
                u8* p = (u8*)((char*)&mPunchKickStep);
                (*p)++;
                this->St_PunchKick_Init();
                return 1;
            }
        }

        func_ov002_020d8a50(((char*)this), mPunchKickStep);

        if (mIsAirborne != 0)
            goto end;

        if (mPunchKickStep == 2 && mStatePhase == 0) {
            mStatePhase = 1;
            if (func_ov002_020e2c84(((char*)this)) != 0)
                return 1;
        }

        ApproachLinear(*((int *)((char *)&mHorzSpeed)), 0, 0x800);
        func_ov002_020c2f64();

        if (func_ov002_020c0434() != 0) {
            func_ov002_020c0364(3);
            return 1;
        }

        if (this->FinishedAnim() || mPunchKickStep == 2) {
            this->ChangeState(data_ov002_0211013c);
            mPunchKickCooldown = 0x10;

            if (mPunchKickStep != 2)
                mHorzSpeed = 0;

            if (mPrevAngleY != mAngleY) {
                mHorzSpeed = 0;
                mPrevAngleY = mAngleY;
            }

            {
                u8 idx2 = gActivePlayerSlot;
                s16 val = *(s16*)((char*)data_0209f4a0 + idx2 * 0x18);
                if (val == 0 && mPunchKickStep == 2) {
                    this->SetAnim(0x52, 0x40000000, 0x1000, 0);
                    mPrevAngleY = mAngleY;
                    mHorzSpeed = 0;
                }
            }

            if (mPunchKickStep != 2)
                goto end;
            if (mIsInShallowWater != 0)
                goto end;

            func_ov002_020bf9d4();
        }
    }

end:
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol func_ov002_020dd5ec
extern "C" {
void func_ov002_020dd5ec(Player *player) {
    extern int RandomIntInternal(int *seed);
    extern u16 data_0209f49c[];
    extern int data_ov002_0210a5c0[];
    extern u16 data_ov002_02109dbc[];
    extern int data_ov002_0210e160;
    extern int data_ov002_020ff194[];
    int model;
    int *new_var3;
    int *p;
    int r;
    int idx;
    int *new_var;
    int new_var2;
    if ((*((u16 *)(((char *)data_0209f49c) + (gActivePlayerSlot * 0x18)))) & 2) {
        player->mPunchKickStep = 2;
    }
    player->SetAnim(data_ov002_0210a5c0[player->mPunchKickStep], 0x40000000, 0x1000, 0);
    model = player->GetBodyModelID(player->param1 & 0xff, 0);
    new_var2 = -0x4000;
    new_var = (int *)&player->mModels.mBodyModels[model];
    new_var3 = (int *)&(dExtFrameCtrl_c &)*(dExtAnmModel_c *)*new_var;
    if ((player && player) && player) {
    }
    p = new_var3;
    ((dExtFrameCtrl_c *)p)->currFrame = 0;
    player->mPunchKickCooldown = data_ov002_02109dbc[player->mPunchKickStep];
    if ((player->mPunchKickStep) == 2) {
        if ((((*((u16 *)(((char *)data_0209f49c) + (gActivePlayerSlot * 0x18)))) & 2) != 0) ||
            ((player->mIsAirborne) != 0)) {
            player->mVertSpeed = 0x20000;
            player->mVertAccel = new_var2;
        } else {
            player->mVertSpeed = 0xc000;
            player->mVertAccel = -0x1800;
        }
        player->mIsAirborne = 1;
        player->mLandSoundPlayed = 0;
    }
    idx = player->mPunchKickStep;
    if (idx != 2) {
        r = RandomIntInternal(&data_ov002_0210e160);
        idx = (idx + (((unsigned int)r) >> 4)) & 1;
    }
    Sound::PlayCharVoice(player->mCharacter, data_ov002_020ff194[idx], player->mCamSpacePos);
    if ((player->mIsMega) != 0) {
        Sound::PlayBank0(0xd5, player->mCamSpacePos);
        return;
    }
    if ((player->mPunchKickStep) != 2) {
        Sound::PlayBank0(7, player->mCamSpacePos);
    } else {
        Sound::PlayBank0(8, player->mCamSpacePos);
    }
}
}

// @symbol _ZN6Player17St_PunchKick_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_PunchKick_Init()
{
    extern void func_ov002_020dc020(void*);
  *(int*)((char*)&mPeakY)=*(int*)((char*)&mPosY);
  mAttackClashed = 0;
  *(short*)((char*)&mStateTimer)=2;
  *(char*)((char*)&mStatePhase)=0;
  if (*(unsigned char *)((char *)&mPunchKickStep) == 2 || this->IsCollectingCap())
      return 1;
  if(*(unsigned char*)((char*)&mIsMega)==0){
    func_ov002_020dc020(((void*)this));
    mAttackClsn.Clear();
    mAttackClsn.Update();
  } else {
      mAttackClsn.Clear();
  }
  return 1;
}
}

// @symbol func_ov002_020dd824
extern "C" {
int func_ov002_020dd824(Player *player) {
    extern Player::State data_ov002_0211034c;
    extern int func_ov002_020d82f0(char *player);
    if (player->mIsAirborne)
        return 0;
    if (func_ov002_020d82f0(player) == 0)
        return 0;
    if (player->mIsMetal)
        return 0;
    if (player->mdCcAcPos_c.otherOwner == 0)
        return 0;
    if (player->mdCcAcPos_c.hitFlags & 0x10000) {
        return player->ChangeState(data_ov002_0211034c) != 0;
    }
    return 0;
}
}

// @symbol func_ov002_020dd8b8
extern "C" {
int func_ov002_020dd8b8(Player *player) {
    extern Player::State data_ov002_021105a4;
    if (!player->IsState(data_ov002_021105a4))
        return 0;
    return player->mStateStep == 0 ? 1 : 0;
}
}

// @symbol func_ov002_020dd8f4
extern "C" {
void func_ov002_020dd8f4(Player *player) {
    extern Player::State data_ov002_021101b4;
    ((Player *)player)->ChangeState(data_ov002_021101b4);
}
}

// @symbol func_ov002_020dd908
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
void func_ov002_020dd908(char *sb) {
    extern int Vec3_Dist(const Vector3 *a, const Vector3 *b);
    extern unsigned char data_0209f2d8;
    extern unsigned char data_0209f21c;
    extern char *data_0209f394[];
    extern void func_ov002_020dd8f4(Player *);
    int i;
    Vector3 v;
    if ((int)(data_0209f2d8 == 1) == 0)
        return;
    for (i = 0; i < data_0209f21c; i++) {
        char *o = data_0209f394[i];
        Player *player = (Player *)o;
        int *p;
        if (o == 0)
            continue;
        if ((int)player->mClimbClsn == 0)
            continue;
        p = &player->mPosX;
        v.x = p[0];
        v.y = p[1];
        v.z = p[2];
        v.y = ((dActor_c *)sb)->mPosY;
        if (Vec3_Dist(&((dActor_c *)sb)->mPos, &v) >= 0x12c000)
            continue;
        func_ov002_020dd8f4(player);
    }
}
}

// @symbol _ZN6Player22St_GroundPound_CleanupEv
/* recovered: named members + shared header, real C++ method */
// Player::St_GroundPound_Cleanup - clear flag bit 0x20 in the u32 at this+0x2ec, return 1.
// The u64-mask launder forces the base to materialize (add r2,this,#0x2ec) so the
// load/store reuse it, matching the ROM instead of folding the offset into ldr/str.
int Player::St_GroundPound_Cleanup()
{
    *(unsigned int *)((char *)&mdCcAcPos_c.flags) &= ~0x20;
    return 1;
}

// @symbol _ZN6Player19St_GroundPound_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Ground pound, per frame. mStateStep is read SIGNED and acts as a three-way
 * selector: positive counts down the spin-up animations, 0 is the falling and
 * impact phase, and negative (0xff, set on impact) is the recovery. The impact
 * branch picks one of three particle sets by cap state and character, shakes
 * the camera, and skips the upper particles in shallow water.
 */
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern "C" {
int Player::St_GroundPound_Main()
{
    void func_ov002_020ef2a4(void* c, void* arg);
    int func_ov002_020e2c84(void* self);
    void func_ov002_020dd908(void* sb);
    void PlayerNewSimpleHold14(unsigned int id, int x, int y, int z);
    void func_0200d8c8(void* cam, const void* v, int strength);
    void func_ov002_020dbc94(void* c);
    int func_ov002_020d36d8(void* c, int arg);
    void Player_AdvanceAnims(void* self);
    extern void* data_0209f318;
    extern char data_0209f4a0[];
    extern Player::State data_ov002_0211013c;
    s8 timer = *(s8*)&mStateStep;
    if (timer > 0) {
        mVertSpeed = 0;
        if (mStateStep == 2)
            mVertSpeed = 0xa000;
        if (FinishedAnim() != 0) {
            mStateStep -= 1;
            if (mStateStep != 0) {
                u32 anim = 0x3c;
                if (mIsMega != 0) anim = 0xa2;
                this->SetAnim(anim, 0x40000000, 0x1000, 0);
            } else {
                u32 anim = 0x3d;
                if (mIsMega != 0) anim = 0xa1;
                this->SetAnim(anim, 0x40000000, 0x1000, 0);
                mVertSpeed = -0x32000;
                Sound::PlayCharVoice(mCharacter, 0x15, mCamSpacePos);
                mPeakY = mPosY;
            }
        }
    } else if (timer == 0) {
        u16 st;
        func_ov002_020ef2a4(&mMeshClsn, this);
        st = mStateTimer;
        if (st != 0) {
            if (st == 1) {
                func_ov002_020c2f64();
                this->SetAnim(0x42, 0x40000000, 0x1000, 0);
                mStateStep = 0xff;
                *(u32*)&mdCcAcPos_c.flags &= ~0x20;
            }
            {
                u16 f = mStateFlags & 1;
                if (f != 0) {
                    func_ov002_020c0364(1);
                    return 1;
                }
            }
        } else {
            if (mIsAirborne == 0) {
                if (func_ov002_020e2c84(this) == 2)
                    return 1;
                func_ov002_020dd908(this);
                if (mIsMega != 0) {
                    Sound::PlayBank0(0xd3, mCamSpacePos);
                } else {
                    Sound::PlayBank0((u32)mGroundSoundType + 0x90, mCamSpacePos);
                }
                {
                    Vector3 v;
                    int x = mPosX;
                    v.x = x;
                    v.y = mPosY;
                    int z = mPosZ;
                    v.z = z;
                    if (mIsMega != 0) {
                        int yy = mPosY + 0x14000;
                        v.y = yy;
                        Particle::System::NewSimple(0x57, x, yy, z);
                        Particle::System::NewSimple(0x58, v.x, v.y, v.z);
                        if (mIsInShallowWater == 0) {
                            Particle::System::NewSimple(0x5a, v.x, v.y, v.z);
                            int y2 = mPosY + 0x3c000;
                            int xx = v.x;
                            int zz = v.z;
                            *(volatile int*)&v.y = y2;
                            Particle::System::NewSimple(0x59, xx, y2, zz);
                        }
                    } else if (param1 != 2) {
                        int yy = mPosY + 0xa000;
                        v.y = yy;
                        Particle::System::NewSimple(0xd3, x, yy, z);
                        if (mIsInShallowWater == 0) {
                            int y2 = mPosY + 0x3c000;
                            int xx = v.x;
                            int zz = v.z;
                            v.y = y2;
                            Particle::System::NewSimple(0xd4, xx, y2, zz);
                            Particle::System::NewSimple(0xd5, v.x, v.y, v.z);
                        }
                    } else {
                        int yy = mPosY + 0xa000;
                        v.y = yy;
                        Particle::System::NewSimple(0x134, x, yy, z);
                        Particle::System::NewSimple(0x135, v.x, v.y, v.z);
                        if (mIsInShallowWater == 0) {
                            int y2 = mPosY + 0x32000;
                            int xx = v.x;
                            int zz = v.z;
                            v.y = y2;
                            Particle::System::NewSimple(0x136, xx, y2, zz);
                            Particle::System::NewSimple(0x137, v.x, v.y, v.z);
                        }
                    }
                }
                func_0200d8c8(data_0209f318, &mPosX, 0x578000);
                mStateTimer = 6;
            } else {
                func_ov002_020dbc94(this);
                mAttackClsn.Clear();
                mAttackClsn.Update();
            }
        }
    } else {
        if (*(short*)(data_0209f4a0 + gActivePlayerSlot * 0x18) != 0 || FinishedAnim() != 0) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
        func_ov002_020d36d8(this, 1);
    }
    Player_AdvanceAnims(this);
    return 1;
}
}
#pragma pop

// @symbol _ZN6Player19St_GroundPound_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_GroundPound_Init()
{
    extern int Player_ReleaseHeldActor(void*);
  unsigned int anim;
  Player_ReleaseHeldActor(((void*)this));
  *(char*)((char*)&mJumpComboStage)=0;
  *(int*)((char*)&mVertSpeed)=0;
  *(int*)((char*)&mHorzSpeed)=0;
  *(short*)((char*)&mPrevAngleY)=*(short*)((char*)&mAngleY);
  anim=0x3b;
  if (*(unsigned char*)((char*)&mIsMega)) anim=0xa3;
  this->SetAnim(anim, 0x40000000, 0x1000, 0);
  *(char*)((char*)&mStateStep)=2;
  Sound::PlayBank0(6, mCamSpacePos);
  mdCcAcPos_c.flags |= 0x20;
  return 1;
}
}

// @symbol func_ov002_020dde74
extern "C" {
extern "C" int func_ov002_020dde74(Player *p) {
    extern int Player_ScaleByCharFactor(Player * c, int a);
    extern Player::State data_ov002_021105d4;
    extern Player::State data_ov002_02110034;
    extern Player::State data_ov002_021101e4;
    extern Player::State data_ov002_0211019c;
    extern Player::State data_ov002_021105bc;
    extern Player::State data_ov002_02110574;
    extern char data_0209f49c[];
    int b = (p->mHeldObj != 0);
    if (b) {
        p->ChangeState(data_ov002_021105d4);
        return 1;
    }
    if (p->param1 == 3 && p->mIsMega == 0) {
        p->ChangeState(data_ov002_02110034);
        return 1;
    }
    if ((*(u16 *)(data_0209f49c + (u8)gActivePlayerSlot * 0x18) & 2) || p->mIsMega != 0)
        goto change574;

    if (p->mHorzSpeed < Player_ScaleByCharFactor(p, 0x20000)) {
        if (p->IsState(data_ov002_021101e4))
            goto change5bc;
        if (!p->IsState(data_ov002_0211019c))
            goto change574;
        if (p->mJumpComboStage != 2)
            goto change574;
    }
change5bc:
    if (p->mHorzSpeed < Player_ScaleByCharFactor(p, 0x24000))
        p->mHorzSpeed = Player_ScaleByCharFactor(p, 0x24000);
    return p->ChangeState(data_ov002_021105bc);

change574:
    if (p->mIsAirborne != 0)
        p->mPunchKickStep = 2;
    else
        p->mPunchKickStep = 0;
    return p->ChangeState(data_ov002_02110574);
}
}

// @symbol _ZN6Player19St_TornadoSpin_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Being carried up a tornado. The player orbits mAttachedActor: the horizontal
 * offset is rotated by an ever-increasing mAngleYSpeed while mAttachOffsetY
 * climbs at mVertSpeed. Leaving happens at the tornado's own height limit
 * (+0xdc on it) or on a collision bit. A dBgCh_Gnd from 0x64000 above the
 * candidate position decides whether the player is placed there or clamped to
 * mGroundY.
 */
extern "C" {
int Player::St_TornadoSpin_Main()
{
    extern void Player_AdvanceAnims(void* self);
    extern Player::State data_ov002_02110454;
    extern short data_02082214[];
    Vector3 pos;
    Vector3 clsnPos;
    int dx = mPosX - ((dActor_c*)mAttachedActor)->mPosX;
    int dz = mPosZ - ((dActor_c*)mAttachedActor)->mPosZ;
    int sx = FX_MUL(dx, 0xf33);
    int sz = FX_MUL(dz, 0xf33);
    int spd = mVertSpeed;
    if (spd < 0) {
        mVertSpeed = 0x1000;
    } else if (spd < 0x3c000) {
        mVertSpeed += 0x1000;
    }
    mAttachOffsetY += mVertSpeed;
    if (mAttachOffsetY < 0)
        mAttachOffsetY = 0;
    /* The attached actor is the tornado: func_ov002_020de33c, the only way
       into this state, is called only by daTor_c with itself. */
    if (mAttachOffsetY > ((daTor_c *)mAttachedActor)->mdCcAc_c.height || (mClsnFlags & 4) != 0) {
        mStatePhase = 0;
        this->ChangeState(data_ov002_02110454);
        if (mVertSpeed < 0x14000)
            mVertSpeed = 0x14000;
        return 1;
    }
    if (mAngleYSpeed < 0x3000)
        mAngleYSpeed += 0x100;
    mAngleY += mAngleYSpeed;
    {
        int v = mAngleYSpeed;
        if (v >= 0x1000) v = 0x1000;
        {
            int k = (u16)v >> 4;
            int sinv = data_02082214[k * 2 + 1];
            int cosv = data_02082214[k * 2];
            ((int *)&pos)[0] =
                FX_MUL(sz, cosv) + (((dActor_c *)mAttachedActor)->mPosX + FX_MUL(sx, sinv));
            ((int *)&pos)[2] =
                FX_MUL(sz, sinv) + (((dActor_c *)mAttachedActor)->mPosZ - FX_MUL(sx, cosv));
            ((int*)&pos)[1] = mAttachOffsetY + ((dActor_c*)mAttachedActor)->mPosY;
        }
    }
    dBgCh_Gnd rc;
    clsnPos = pos;
    clsnPos.y += 0x64000;
    rc.SetObjAndPos(clsnPos, this);
    if (rc.DetectClsn() != 0) {
        mPosX = pos.x;
        mPosY = pos.y;
        mPosZ = pos.z;
    } else {
        if (mPosY >= mGroundY)
            mPosY = pos.y;
        else
            mPosY = mGroundY;
    }
    Player_AdvanceAnims(this);
    return 1;
}
}

// @symbol _ZN6Player19St_TornadoSpin_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_TornadoSpin_Init() {
    extern int Player_ReleaseHeldActor(void *);
    extern int data_0209f318[];
    extern void func_0200d6b4(void *, unsigned char);
    Player_ReleaseHeldActor(((char *)this));
    this->SetAnim(0x5f, 0, 0x1000, 0);
    mVertSpeed = 0;
    mVertAccel = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    func_0200d6b4(*(void **)data_0209f318, mPlayerNo);
    mAttachOffsetY = mPosY - ((dActor_c *)mAttachedActor)->mPosY;
    Sound::PlayCharVoice(mCharacter, 0x26, mCamSpacePos);
    return 1;
}
}

// @symbol func_ov002_020de328
extern "C" {
int func_ov002_020de328(Player *player) {
    extern Player::State data_ov002_021102bc;
    return player->IsState(data_ov002_021102bc);
}
}

// @symbol func_ov002_020de33c
extern "C" {
int func_ov002_020de33c(Player *c, int a) {
    extern Player::State data_ov002_021102bc;
    extern Player::State data_ov002_02110484;
    Player *player = (Player *)c;
    if (player->mIsNoControl == 0 && !((Player *)c)->IsState(data_ov002_021102bc) &&
        !((Player *)c)->IsState(data_ov002_02110484)) {
        goto ok;
    }
    return 0;
ok:
    player->mAttachedActor = a;
    ((Player *)c)->ChangeState(data_ov002_021102bc);
    return 1;
}
}

// @symbol _ZN6Player18St_Balloon_CleanupEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Balloon_Cleanup()
{
    extern void func_ov002_020de968(void*);
    func_ov002_020de968(((void *)this));
    return 1;
}
}

// @symbol func_ov002_020de3d0
extern "C" {
void func_ov002_020de3d0(int a0, short* r1, short* r2){
  if(*r2 >= 0x100) *r2 -= 0x20;
  if(*r2 <= -0x100) *r2 += 0x20;
  if(*r1 < 0) *r2 += 0x10;
  else *r2 -= 0x10;
  *r1 = *r1 + *r2;
}
}

// @symbol func_ov002_020de428
/* recovered: shared common types */
struct dActor_ce428 {
    char pad74[0x74];
    Vector3 vec; /* 0x74 */
    char pad[0x70c - 0x80];
    unsigned char flag; /* 0x70c */
};
extern "C" {
void func_ov002_020de428(struct dActor_ce428 *thiz)
{
    if (thiz->flag)
        return;
    Sound::PlayBank0(0xbd, thiz->vec);
    thiz->flag = 0x1e;
}
}

// @symbol _ZN6Player15St_Balloon_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Balloon (wing-cap style) flight, per frame. Bails out to one of two states
 * when the balloon timer runs out or the global flag says so; otherwise the
 * A-button bit adds lift in one of two bands, a collision bit bounces the
 * player off a wall by reflecting mPrevAngleY, and the pitch/roll pair is
 * driven from the turn rate.
 */
/* local extern: the call passes an untyped this, so it cannot use the header method. */
extern "C" {
s32 Player::St_Balloon_Main()
{
    extern void func_ov002_020de968(void *);
    extern s16 atan2_hold_Player_4(s32, s32);
    extern int AngleDiff(s32, s16);
    extern void func_ov002_020de428(void *);
    extern void ApproachAngle(void *, s16, s16, s16, s32);
    extern s32 func_ov002_020bf224(void *, s32, s32);
    extern void func_ov002_020de3d0(void *, s16 *, s16 *);
    extern s16 data_0209f49e[];
    extern s16 data_0209f4a0[];
    extern s16 data_02082214[];
    extern Player::State data_ov002_021101b4;
    extern Player::State data_ov002_021105a4;
    if (mStateArg != 0) {
        mStateArg--;
    }
    mPeakY = mPosY;
    mVertAccel = 0;
    mTerminalVelocity = -0xc000;
    ApproachLinear(mVertSpeed, -0x6000, 0x800);

    if (mBalloonTimer == 0) {
        func_ov002_020de968(this);
        this->ChangeState(data_ov002_021101b4);
        return 1;
    }

    u16 flags = *(u16 *)((char *)&data_0209f49e + gActivePlayerSlot * 0x18);
    if (flags & 0x400) {
        func_ov002_020de968(this);
        this->ChangeState(data_ov002_021105a4);
        return 1;
    }

    if ((flags & 2) && mStateWaitTimer == 0) {
        if (mStateTimer == 0) {
            mVertSpeed += 0xc000;
            if (mVertSpeed < 0xc000)
                mVertSpeed = 0xc000;
            if (mVertSpeed > 0x10000)
                mVertSpeed = 0x10000;
        } else {
            mVertSpeed += 0x6000;
            if (mVertSpeed < 0x6000)
                mVertSpeed = 0x6000;
            if (mVertSpeed > 0xc000)
                mVertSpeed = 0xc000;
        }
        Sound::PlayBank0(0xbe, mCamSpacePos);
        mStateTimer = 8;
    }

    if (mClsnFlags & 2) {
        s16 ang = cstd::atan2(mWallNormalX, mWallNormalZ);
        s16 diff = AngleDiff(ang, mPrevAngleY);
        if ((s32)diff > 0x4000) {
            func_ov002_020de428(this);
            mPrevAngleY = (s16)((ang * 2 + 0x8000) - mPrevAngleY);
            mHorzSpeed = FX_MUL(mHorzSpeed, 0x1400);
            if (mHorzSpeed > 0x14000)
                mHorzSpeed = 0x14000;
            if (mHorzSpeed < 0x4000)
                mHorzSpeed = 0x4000;
            s32 idx = ((s32)(u16)diff >> 4) * 2;
            s16 t0 = data_02082214[idx + 1];
            s32 v1 = FX_MUL(mHorzSpeed, t0);
            s32 v = FX_MUL(v1, 0xa0);
            s16 t1 = data_02082214[idx];
            mAngleYSpeed = (s16)FX_MUL(t1, v);
            mAngleXSpeed = (s16)FX_MUL(t0, v);
        }
    }

    u8 mode = mClsnFlags;
    if (mode & 1) {
        s32 g = mPrevVertSpeed;
        if (g < 0) {
            mVertSpeed = -FX_MUL(g, 0x2800);
            func_ov002_020de428(this);
        }
    } else if (mode & 4) {
        s32 g = mPrevVertSpeed;
        if (g > 0) {
            mVertSpeed = -(s32)((((long long)g << 14) + 0x800) >> 12);
            mStateWaitTimer = 0x1e;
            func_ov002_020de428(this);
        }
    }

    if (mVertSpeed >= 0xa000)
        mVertSpeed = 0xa000;

    s32 r4 = 0;
    if (*(s16 *)((char *)&data_0209f4a0 + gActivePlayerSlot * 0x18) != 0) {
        ApproachAngle(&mPrevAngleY, mDesiredAngleY, 0x20, 0x2000, 0x10);
        r4 = 0x10000;
    }
    ApproachLinear2(mAngleY, mPrevAngleY, 0x200);
    ApproachLinear(mHorzSpeed, func_ov002_020bf224(this, r4, 0), 0x100);
    func_ov002_020de3d0(this, &mAngleZ, &mAngleYSpeed);
    func_ov002_020de3d0(this, &mAngleX, &mAngleXSpeed);
    mPrevVertSpeed = mVertSpeed;
    mModels.mAnmModel1.Advance();
    return 1;
}
}

// @symbol _ZN6Player15St_Balloon_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Balloon_Init() {
    extern int Player_ReleaseHeldActor(void *);
    extern int data_0209f318[];
    extern int data_ov002_0210e750[];
    extern void func_0200d63c(void *, unsigned char);
    Player_ReleaseHeldActor(((char *)this));
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    if (mVertSpeed >= 0x10000)
        mVertSpeed = 0x10000;
    if (mVertSpeed <= -0x10000)
        mVertSpeed = -0x10000;
    mHorzSpeed = 0;
    mModelOffsetY = 0x64000;
    func_0200d63c((void *)data_0209f318[0], mPlayerNo);
    mAngleYSpeed = 0x100;
    mAngleXSpeed = 0x100;
    mPrevVertSpeed = mVertSpeed;
    ((dExtAnmModel_c *)((char *)&mModels.mAnmModel1))
        ->SetAnim((BCA_File *)((void *)data_ov002_0210e750[1]), 0x40000000, 0x1000, 0);
    mModels.mAnmModel1.currFrame = 0;
    return 1;
}
}

// @symbol func_ov002_020de968
// local extern: this file's local Vector3/Fix12i shadow the shared headers, so it cannot include Player.h and use the member
extern "C" {
struct Vector3e968 { int x, y, z; ~Vector3e968(){} };
}
extern "C" {
void func_ov002_020de968(char* c){
    extern void PlayerNewSimpleHold15(unsigned int id, Fix12i a, Fix12i b, Fix12i d);
    Player *p = (Player *)c;
    p->mBalloonTimer = 0;
    if (p->mIsBalloon != 1)
        return;
    p->mIsBalloon = 0;
    p->mOpacity = 0x1f;
    p->EndPowerupMusic(0x30);
    Vector3e968 v;
    v.x = p->mPosX;
    v.y = p->mPosY;
    v.z = p->mPosZ;
    v.y = p->mPosY + 0x50000;
    Particle::System::NewSimple(0xc5, v.x, v.y, v.z);
    Particle::System::NewSimple(0xc6, v.x, v.y, v.z);
    Sound::PlayBank0(0xd8, p->mCamSpacePos);
}
}

// @symbol _ZN6Player16InitBalloonMarioEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
void Player::InitBalloonMario()
{
    extern void PlayerNewSimpleHold16(unsigned int id, int x, int y, int z);
    extern Player::State data_ov002_0211028c;
    Vector3 v;

    if (mIsBalloon != 0) return;

    func_ov002_020bda48();
    mIsBalloon = 1;
    mBalloonTimer = 0x258;
    this->ChangeState(data_ov002_0211028c);
    StartPowerupMusic(0x30);
    func_ov002_020c43c4(2);

    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y = mPosY + 0x50000;
    Particle::System::NewSimple(0xc5, v.x, v.y, v.z);
    Particle::System::NewSimple(0xc6, v.x, v.y, *(volatile int *)&v.z);

    Sound::PlayBank0(0xd7, mCamSpacePos);
}
}

// @symbol _ZN6Player17St_WindCarry_MainEv
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" {
int Player::St_WindCarry_Main()
{
    extern void func_ov002_020e28d4(void* c, int a, int b);
    extern void Player_AdvanceAnims(char* self);
    extern int __aeabi_idiv(int a, int b);
    extern Player::State data_ov002_021105bc;
    char* c = (char*)this;

    if (mIsAirborne == 0) {
        this->func_ov002_020c2f64();
        this->ChangeState(data_ov002_021105bc);
        this->SetAnim(0x43, 0x40000000, 0x1000, 0);
        mVertSpeed = 0;
        return 1;
    }

    func_ov002_020e28d4(c, 0x1800, 0x800);
    {
        int val = mPosY + 0x514000;
        if (mSurfaceType == 0x13 && val > (int)0xfdecc000 && val < 0x898000) {
            int speed;
            if (val < 0)
                speed = 0x32000;
            else
                speed = __aeabi_idiv(0x2710000, (val + 0xc8000) >> 12);
            if (mVertSpeed < speed) {
                int* p = &this->mVertSpeed;
                *p = *p + (speed >> 3);
                if (mVertSpeed > speed) mVertSpeed = speed;
            }
        }
    }

    if (mStateStep == 0 && FinishedAnim() != 0) {
        this->SetAnim(0x73, 0, 0x1000, 0);
        {
            u8* q = &this->mStateStep;
            *q = *q + 1;
        }
    }
    Player_AdvanceAnims(c);
    return 1;
}
}

// @symbol _ZN6Player17St_WindCarry_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_WindCarry_Init()
{
    extern int func_ov002_020da9d4(void*);
    extern int data_0209f318[];
    extern void func_0200d63c(void*, unsigned char);
  func_ov002_020da9d4(((char*)this));
  this->SetAnim(0x4a, 0x40000000, 0x1000, 0);
  mVertSpeed=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  func_0200d63c((void*)data_0209f318[0], mPlayerNo);
  return 1;
}
}

// @symbol func_ov002_020dec70
extern "C" {
extern "C" int func_ov002_020dec70(char *c)
{
    Player *p = (Player *)c;
    void func_ov002_020bf800(char *c, Vector3_16f v);
    void PlayerPlayLongHold_dec70(u32 a, u32 b, u32 c, Vector3 * v, u32 d);
    int func_ov002_020e0478(void *c);
    extern Player::State data_ov002_02110274;
    extern Player::State data_ov002_021102d4;
    extern Player::State data_ov002_02110244;
    int t = p->mPosY + 0x514000;
    if (t < (int)0xfdecc000) return 0;
    {
        Vector3_16f v;
        v.z = 0;
        v.y = 0x1000;
        v.x = v.z;
        func_ov002_020bf800(c, v);
    }
    p->mSurfaceSoundHandle =
        Sound::PlayLong((u32)p->mSurfaceSoundHandle, 3, 0x90, *(Vector3 *)&p->mCamSpacePos, 0);
    if (((Player *)c)->IsState(data_ov002_02110274) ||
        ((Player *)c)->IsState(data_ov002_021102d4) ||
        ((Player *)c)->IsState(data_ov002_02110244) || p->mIsMetal || p->mIsMega ||
        p->mIsNoControl || p->mIsAirborne == 0)
        return 0;
    if (func_ov002_020e0478(c) != 0 || p->mIsBalloon != 0) {
        if (t < 0x898000)
            p->mPosY += 0x8000;
        return 0;
    }
    ((Player *)c)->ChangeState(data_ov002_02110274);
    return 1;
}
}

// @symbol _ZN6Player21St_JumpQuicksand_MainEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_JumpQuicksand_Main()
{

    extern Player::State data_ov002_02110424;
    extern void Player_AdvanceAnims(char *);
    u8 t = mStateWork;
    (*(u8 *)(int)((char *)&mStateWork))++;
    if (t < 6) {
        (*(int *)(int)((char *)&mSinkDepth)) -= FX_MUL((7 - mStateWork) << 12, 0xccc);
        if (mSinkDepth < 0x1000)
            mSinkDepth = 0x1100;
    } else {
        this->ChangeState(data_ov002_02110424);
    }
    Player_AdvanceAnims(((char *)this));
    return 1;
}
}

// @symbol _ZN6Player21St_JumpQuicksand_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_JumpQuicksand_Init()
{
  mJumpedFromQuicksand=1;
  this->SetAnim(0x53, 0x40000000, 0x1000, 0);
  mStateWork=0;
  mVertSpeed=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  return 1;
}
}

// @symbol _ZN6Player17St_Cannon_CleanupEv
/* recovered: named members + shared header, real C++ method */
// Player::St_Cannon_Cleanup - clear flag bit 0x20 in the u32 at this+0x2ec, return 1.
// The u64-mask launder forces the base to materialize (add r2,this,#0x2ec) so the
// load/store reuse it, matching the ROM instead of folding the offset into ldr/str.
int Player::St_Cannon_Cleanup()
{
    *(unsigned int *)((char *)&mdCcAcPos_c.flags) &= ~0x20;
    return 1;
}

// @symbol _ZN6Player14St_Cannon_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Cannon_Main()
{
    extern short GetAngleToCamera(int i);
    extern void func_0200d6f0(void* thiz, u8 pid);
    extern void func_02012694(u32 id, void* v);
    extern int atan2_hold_Player_3(int a, int b);
    extern void Player_AdvanceAnims(void* c);
    extern u16 data_0209f49e[];
    extern char* data_0209f318;
    extern short data_02082214[];
    extern Player::State data_ov002_0211031c;
    extern Player::State data_ov002_02110214;
    switch (mStateStep) {
    case 0:
        break;
    case 1: {
        if ((*(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18) & 3) == 0)
            break;

        char* p = data_0209f318;
        mPrevAngleY = GetAngleToCamera(mPlayerNo) + 0x8000;
        mAngleY = mPrevAngleY;

        int ang = (u16)((dCamera_c *)p)->mAngleX;
        int j = ang >> 4;
        mHorzSpeed = FX_MUL(data_02082214[j * 2 + 1], 0x64000);
        mVertSpeed = FX_MUL(data_02082214[j * 2], 0x64000);
        mAngleX = 0;
        mStateStep = 2;
        mOpacity = 0x1f;
        mIsBodyClsnEnabled = 1;
        *(int*)((char*)&mdCcAcPos_c.flags) |= 0x20;
        func_0200d6f0(p, mPlayerNo);
        func_02012694(0x14f, &mCamSpacePos);
        Sound::PlayBank0(0xb9, mCamSpacePos);
        Sound::PlayCharVoice(mCharacter, 0xc, mCamSpacePos);
        func_ov002_020c9e18();
        mIsControlDisabled = 0;
        break;
    }
    case 2:
        if ((mClsnFlags & 6) != 0) {
            func_ov002_020c1eb4((short)(mAngleY + 0x8000));
            return 1;
        }
        if (mIsAirborne == 0) {
            if (mStateWork != 0) {
                this->ChangeState(data_ov002_0211031c);
                return 1;
            }
            this->ChangeState(data_ov002_0211031c);
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
            mVertSpeed = 0xa000;
            return 1;
        }
        if (mHasWings != 0 && mVertSpeed < 0) {
            this->SetAnim(0x72, 0x40000000, 0x1000, 0);
            mStateStep = 3;
            return 1;
        }
        *(int*)((char*)&mHorzSpeed) -= 0x80;
        if (mHorzSpeed < 0xa000)
            mHorzSpeed = 0xa000;
        if (mVertSpeed >= 0)
            Particle::RunningSlidingDustAt(mPosX, mPosY, mPosZ);
        if (mStateWork == 0)
            mAngleX = cstd::atan2(mHorzSpeed >> 8, mVertSpeed >> 8) - 0x4000;
        Player_AdvanceAnims(((char*)this));
        // fall through
    case 3:
        if (this->FinishedAnim()) {
            mStateStep = 1;
            this->ChangeState(data_ov002_02110214);
        }
        Player_AdvanceAnims(((char*)this));
        break;
    }
    return 1;
}
}

// @symbol _ZN6Player14St_Cannon_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Cannon_Init()
{
    extern int Player_DisableInteraction(void*);
    extern int Player_ReleaseHeldActor(void*);
  Player_DisableInteraction(((char*)this));
  mIsUnderwater=0;
  mIsInAirState=1;
  mIsFallScreaming=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  Player_ReleaseHeldActor(((char*)this));
  mStateStep=0;
  this->SetAnim(0x73, 0, 0x1000, 0);
  mVertAccel=-0x1200;
  mOpacity=0;
  mIsBodyClsnEnabled=0;
  mStateWork=0;
  mIsControlDisabled=1;
  return 1;
}
}

// @symbol _ZN6Player16IsInsideOfCannonEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::IsInsideOfCannon()
{

    extern Player::State data_ov002_021102d4;
  if(!this->IsState(data_ov002_021102d4)) return 0;
  return mStateStep!=2 ? 1 : 0;
}
}

// @symbol _ZN6Player22IsBeingShotOutOfCannonEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::IsBeingShotOutOfCannon()
{

    extern Player::State data_ov002_021102d4;
  if(!this->IsState(data_ov002_021102d4)) return 0;
  return mStateStep==2 ? 1 : 0;
}
}

// @symbol func_ov002_020df300
extern "C" {
int func_ov002_020df300(char* p) {
    extern void func_0200d438(int a, unsigned char b);
    extern int data_0209f318;
    Player *player = (Player *)p;
    if (player->mStateStep)
        return 0;
    player->mStateStep = 1;
    func_0200d438(data_0209f318, player->mPlayerNo);
    return 1;
}
}

// @symbol func_ov002_020df34c
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" {
extern "C" int func_ov002_020df34c(Player* self) {
    struct dCamera_c {};
    extern int func_ov002_020e0478(void* c);
    extern void func_0200d474(dCamera_c* thiz, unsigned char playerID);
    extern Player::State data_ov002_0210ffec;
    extern Player::State data_ov002_021102d4;
    extern dCamera_c* data_0209f318;
    unsigned char* p = (unsigned char*)self;
    if (p[0x709] != 0 || func_ov002_020e0478(self) != 0 ||
        self->IsState(data_ov002_0210ffec) != 0) {
        return 0;
    }
    p[0x6e3] = 0;
    self->ChangeState(data_ov002_021102d4);
    func_0200d474(data_0209f318, p[0x6d8]);
    return 1;
}
}

// @symbol _ZN6Player14St_Owl_CleanupEv
/* recovered: named members + shared header, real C++ method */
int Player::St_Owl_Cleanup()
{
    EndStateMusic(0x2f);
    mHeldObj = 0;
    return 1;
}

// @symbol _ZN6Player11St_Owl_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Owl_Main()
{
    extern int ApproachAngle(short* cur, short target, int divisor, int band, int maxStep);
    extern void func_0201f32c(int a);
    extern void func_0200d3f8(void* thiz, unsigned char playerID, void* ptr);
    extern void Player_DisableInteraction(char* c);
    extern void Player_AdvanceAnims(void* c);
    extern unsigned char data_0209f49c[];
    extern char data_0209f4a0[];
    extern Player::State data_ov002_021101b4;
    extern short data_02082214[];
    extern char* data_0209f318;
    extern unsigned char data_0209d660;
    extern void func_0200d6b4(void*, unsigned char);
    unsigned char st;

    mPeakY = mPosY;
    st = mStateStep;
    if (st != 2 && st != 3) {
        if (mHeldObj == 0 ||
            (*(unsigned short*)(data_0209f49c + gActivePlayerSlot*0x18) & 2) == 0 ||
            (mClsnFlags & 2)) {
            this->ChangeState(data_ov002_021101b4);
            return 1;
        }
    }

    switch (st) {
    case 0:
        if (mAttachOffsetY <= mPosY) {
            (this->mStateStep)++;
            mVertAccel = -0x2c00;
        }
        if (mIsAirborne != 0) {
            if (mVertSpeed < 0) {
                ApproachLinear(*((int *)((char *)&mVertSpeed)), 0x3c000, 0x8000);
            } else {
                ApproachLinear(*((int *)((char *)&mVertSpeed)), 0x3c000, 0x2000);
            }
        } else {
            mVertSpeed = 0x8000;
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
        }
        ApproachAngle((short*)((char*)&mAngleY), mAngleYSpeed, 0x10, 0x100, 0);
        break;

    case 1:
    case 4: {
        int x = mHeldObj;
        short ang;
        int i;
        short mag = 10;
        x = ((daOwl_c *)x)->mAnmModel.currFrame;
        x = (unsigned short)((unsigned int)x >> 12);
        x += 0xb;
        ang = x * 0xccc;
        i = (unsigned short)ang >> 4;
        mVertSpeed = data_02082214[i*2+1] * mag;
        mHorzSpeed = 0x14000;
        if (*(short*)(data_0209f4a0 + gActivePlayerSlot*0x18) != 0) {
            ApproachAngle((short*)((char*)&mAngleY), mDesiredAngleY, 0x20, 0x100, 0);
        }
        if (mStateStep == 1) {
            if (mAttachOffsetY - 0xed8000 > mPosY) {
                mOwlFarCamera = 1;
                func_0201f32c(0xa3);
                func_0200d3f8(data_0209f318, mPlayerNo, 0);
                mVertAccel = 0;
                Player_DisableInteraction(((char*)this));
                mStateStep = 2;
            }
        } else {
            if (mStateTimer == 0) {
                this->ChangeState(data_ov002_021101b4);
                return 1;
            }
        }
        break;
    }

    case 2:
        mHorzSpeed = 0;
        mVertSpeed = mHorzSpeed;
        if (data_0209d660 == 0) {
            mStateStep = 3;
            func_0200d6b4(data_0209f318, mPlayerNo);
        }
        break;

    case 3:
        if ((((dCamera_c *)data_0209f318)->mFlags & 0x8000) == 0) {
            mStateStep = 4;
            mStateTimer = 0x3c;
            func_ov002_020c9e18();
            mOwlFarCamera = 0;
        }
        break;
    }

    if (this->FinishedAnim()) {
        this->SetAnim(0x58, 0, 0x1000, 0);
    }
    mPrevAngleY = mAngleY;
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player11St_Owl_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Owl_Init()
{
    extern int data_0209f318[];
    extern void func_0200d6b4(void*, unsigned char);
  mTerminalVelocity=-0x4b000;
  mVertAccel=0;
  mStateStep=0;
  mHorzSpeed=0;
  this->SetAnim(0x5b, 0x40000000, 0x1000, 0);
  func_0200d6b4((void*)data_0209f318[0], mPlayerNo);
  StartStateMusic(0x2f);
  return 1;
}
}

// @symbol func_ov002_020df7ac
extern "C" {
int func_ov002_020df7ac(Player *thiz) {
    extern Player::State data_ov002_02110244;
    extern Player::State data_ov002_021101b4;
    if (thiz->IsState(data_ov002_02110244) == 0)
        return 0;
    thiz->St_Owl_Cleanup();
    thiz->ChangeState(data_ov002_021101b4);
    return 1;
}
}

// @symbol func_ov002_020df7f4
extern "C" {
int func_ov002_020df7f4(Player *player) {
    extern Player::State data_ov002_02110244;
    if (!player->IsState(data_ov002_02110244) || player->mHeldObj == 0)
        return -1;
    return player->mStateStep != 0;
}
}

// @symbol func_ov002_020df840
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
int func_ov002_020df840(Player *thiz, void *arg1, Vector3 *arg2) {
    extern u16 data_0209f49c[];
    extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
    extern Player::State data_ov002_02110244;
    if (thiz->mIsMega != 0 || thiz->mIsBalloon != 0)
        return 0;
    if ((*(u16*)((char*)data_0209f49c + gActivePlayerSlot * 0x18) & 2) == 0)
        return 0;
    if (thiz->mHeldObj != 0)
        return 0;
    thiz->ChangeState(data_ov002_02110244);
    thiz->mHeldObj = (int)arg1;
    thiz->mAttachOffsetY = arg2->y;
    thiz->mAngleYSpeed = Vec3_HorzAngle(&thiz->mPos, arg2);
    return 1;
}
}

// @symbol func_ov002_020df8f0
#pragma push
#pragma opt_common_subs off
extern "C" {
void func_ov002_020df8f0(char* c)
{
    typedef unsigned long long u64;
    extern s16 data_0209f4a2[];
    extern s16 data_0209f4a4[];
    extern s16 data_02082214[];
    Player *player = (Player *)c;
    int vb = player->mVertSpeed;
    int va = player->mHorzSpeed;
    int b = vb >> 8;
    int sq = b * b;
    int a = va >> 8;
    sq += a * a;
    player->mPrevVertSpeed = cstd::sqrt((u64)sq) << 8;

    {
        int idx = gActivePlayerSlot * 0x18;
        s16 tval = *(s16*)((char*)data_0209f4a4 + idx);
        int div5 = player->mPrevVertSpeed / 5;
        int v = FX_MUL((tval >> 6), div5);
        if (v < -0x8000) v = -0x8000;
        if (v >= 0x8000) v = 0x7fff;

        if (v > 0) {
            if (player->mAngleXSpeed < 0) {
                player->mAngleXSpeed += 0x40;
                if (player->mAngleXSpeed > 0x20)
                    player->mAngleXSpeed = 0x20;
            } else {
                s16 step;
                if (player->mAngleXSpeed < v)
                    step = 0x20;
                else
                    step = 0x40;
                ApproachLinear2(player->mAngleXSpeed, (s16)v, step);
            }
        } else if (v < 0) {
            if (player->mAngleXSpeed > 0) {
                player->mAngleXSpeed -= 0x40;
                if (player->mAngleXSpeed < -0x20)
                    player->mAngleXSpeed = -0x20;
            } else {
                s16 step;
                if (player->mAngleXSpeed < v)
                    step = 0x40;
                else
                    step = 0x20;
                ApproachLinear2(player->mAngleXSpeed, (s16)v, step);
            }
        } else {
            ApproachLinear2(player->mAngleXSpeed, 0, 0x40);
        }
    }

    {
        int idx = gActivePlayerSlot * 0x18;
        int u2 = player->mPrevVertSpeed;
        s16 tval2 = *(s16*)((char*)data_0209f4a2 + idx);
        int quarter = u2 / 4;
        int v2 = 0 - FX_MUL((tval2 >> 6), quarter);
        if (v2 < -0x8000) v2 = -0x8000;
        if (v2 >= 0x8000) v2 = 0x7fff;

        if (v2 > 0) {
            if (player->mAngleYSpeed < 0) {
                player->mAngleYSpeed += 0x40;
                if (player->mAngleYSpeed > 0x10)
                    player->mAngleYSpeed = 0x10;
            } else {
                s16 step;
                if (player->mAngleYSpeed < v2)
                    step = 0x10;
                else
                    step = 0x20;
                ApproachLinear2(player->mAngleYSpeed, (s16)v2, step);
            }
        } else if (v2 < 0) {
            if (player->mAngleYSpeed > 0) {
                player->mAngleYSpeed -= 0x40;
                if (player->mAngleYSpeed < -0x10)
                    player->mAngleYSpeed = -0x10;
            } else {
                s16 step;
                if (player->mAngleYSpeed < v2)
                    step = 0x20;
                else
                    step = 0x10;
                ApproachLinear2(player->mAngleYSpeed, (s16)v2, step);
            }
        } else {
            ApproachLinear2(player->mAngleYSpeed, 0, 0x40);
        }
    }

    player->mPrevAngleY += player->mAngleYSpeed;
    player->mPrevAngleZ = (s16)((0 - player->mAngleYSpeed) * 0x14);

    player->mPrevVertSpeed -= ((player->mPrevAngleX >> 1) + 0x100);
    if (player->mPrevVertSpeed < 0)
        player->mPrevVertSpeed = -player->mPrevVertSpeed;

    {
        int *p = &player->mPrevVertSpeed;
        *p -= (0x1000 - data_02082214[((u16)(player->mAngleYSpeed) >> 4) * 2 + 1]) >> 1;
    }
    if (player->mPrevVertSpeed < 0)
        player->mPrevVertSpeed = 0;

    if (player->mPrevVertSpeed > 0x10000) {
        player->mPrevAngleX += ((player->mPrevVertSpeed - 0x20000) * 6) >> 12;
    } else if (player->mPrevVertSpeed <= 0x4000) {
        player->mPrevAngleX -= 0x600;
    } else {
        player->mPrevAngleX += ((player->mPrevVertSpeed - 0x20000) * 0xa) >> 12;
    }

    player->mPrevAngleX += player->mAngleXSpeed;
    if (player->mPrevAngleX > 0x2aaa)
        player->mPrevAngleX = 0x2aaa;
    if (player->mPrevAngleX < (s16)0xd556)
        player->mPrevAngleX = (s16)0xd556;

    player->mVertSpeed =
        FX_MUL(player->mPrevVertSpeed, data_02082214[((u16)player->mPrevAngleX >> 4) * 2]);
    player->mHorzSpeed =
        FX_MUL(player->mPrevVertSpeed, data_02082214[((u16)player->mPrevAngleX >> 4) * 2 + 1]);

    {
        player->mAngleX = player->mPrevAngleX;
        player->mAngleY = player->mPrevAngleY;
        player->mAngleZ = player->mPrevAngleZ;
        s16 r = (s16)(0 - player->mPrevAngleX);
        int spd = player->mPrevVertSpeed;
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        s16 P = (s16)FX_MUL(t, 0x2aaaU);
        if (r > P) r = P;
        if (r < -P) r = (s16)(-P);
        player->mAngleX = r;
    }
}
}
#pragma pop

// @symbol _ZN6Player11St_Fly_MainEv
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
#define c ((char *)this)
extern "C" {
int Player::St_Fly_Main()
{
    extern int func_ov002_020d674c(char *);
    extern void func_ov002_020df8f0(void *);
    extern u32 PlayerNewHold16(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                               void *callback);
    extern void func_ov002_020e25f0(char *, int a);
    extern void MulVec3Mat4x3(void *dst, void *src, void *out);
    extern void Vec3_MulScalarInPlace(s32 *v, s32 s);
    extern int func_02012194(char *, int a1, int a2, int a3, int s0, void *s1, int s2);
    extern void Player_AdvanceAnims(char *self);
    extern Player::State data_ov002_0211004c;
    extern Player::State data_ov002_021105a4;
    extern Player::State data_ov002_021101b4;
    extern Player::State data_ov002_021105bc;
    extern u16 data_0209f49e;
    extern u16 data_0209f49c;
    extern s16 data_02082214[];
    mPeakY = mPosY;

    if ((*(u16 *)((char *)&data_0209f49e + gActivePlayerSlot * 0x18) & 0x400) == 0) {
        goto skip;
    }
    if (func_ov002_020d674c(c) != 0) {
        this->ChangeState(data_ov002_0211004c);
    } else {
        this->ChangeState(data_ov002_021105a4);
    }
    return 1;

skip:
    if (mHasWings == 0) {
        this->ChangeState(data_ov002_021101b4);
        return 1;
    }
    if (mIsAirborne == 0) {
        this->func_ov002_020c2f64();
        this->ChangeState(data_ov002_021105bc);
        this->SetAnim(0x43, 0x40000000, 0x1000, 0);
        mVertSpeed = 0;
        return 1;
    }

    if (this->IsAnim(0x4a) != 0) {
        if (mVertSpeed > 0x20000) {
            mVertAccel = -0x3400;
            if (mVertSpeed >= 0) {
                if ((*(u16 *)((char *)&data_0209f49c + gActivePlayerSlot * 0x18) & 2) == 0) {
                    mVertAccel = -0x8000;
                }
            } else {
                mVertAccel = -0x2000;
                this->mHorzSpeed = this->mHorzSpeed + 0xe00;
            }
            if (this->FinishedAnim() != 0) {
                mVertAccel = 0;
                mTerminalVelocity = -0x4b000;
                this->SetAnim(0x49, 0, 0x1000, 0);
            }
        } else {
            mVertAccel = 0;
            mTerminalVelocity = -0x4b000;
            func_ov002_020df8f0(c);
        }
    } else {
        func_ov002_020df8f0(c);
    }

    Vector3_16f vec16;
    Vector3 vec32;
#define spC vec16.x
#define spE vec16.y
#define sp10 vec16.z
#define sp14 vec32.x
#define sp18 vec32.y
#define sp1C vec32.z
    {
        s16 n = data_02082214[((u16)mPrevAngleX >> 4) * 2 + 1];
        s32 r0 = (s16)-n;
        s16 t = data_02082214[((u16)mAngleY >> 4) * 2];
        spC = (s16)FX_MUL(r0, t);
        spE = data_02082214[((u16)mAngleX >> 4) * 2];
        s16 t2 = data_02082214[((u16)mAngleY >> 4) * 2 + 1];
        sp10 = (s16)FX_MUL(r0, t2);
    }

    if (mPrevVertSpeed >= 0x28000) {
        mParticle1 = Particle::System::New(mParticle1, 0xd1, mPosX, mPosY, mPosZ,
                                           (const Vector3_16f *)(&vec16), 0);
    }

    if (mPrevVertSpeed >= 0x30000) {
        if (mPrevAngleX > 0x800) {
            if (mStateArg == 0) {
                Sound::PlayBank0(0xb9, mCamSpacePos);
                func_ov002_020e25f0(c, 2);
                mStateArg++;
            }
            {
                Matrix4x3 *bone =
                    &mModels.mBodyModels[this->GetBodyModelID(this->param1 & 0xff, 0)]
                         ->data.transforms[14];
                dExtAnmModel_c *m2 =
                    mModels.mBodyModels[this->GetBodyModelID(this->param1 & 0xff, 0)];
                MulVec3Mat4x3(&bone->m[9], &m2->mat4x3, &vec32);
                Vec3_MulScalarInPlace(&vec32.x, 0x8000);
                mParticle2 = Particle::System::New(mParticle2, 0xd0, sp14, sp18, sp1C,
                                                   (const Vector3_16f *)(&vec16), 0);
            }
            {
                Matrix4x3 *bone =
                    &mModels.mBodyModels[this->GetBodyModelID(this->param1 & 0xff, 0)]
                         ->data.transforms[11];
                dExtAnmModel_c *m2 =
                    mModels.mBodyModels[this->GetBodyModelID(this->param1 & 0xff, 0)];
                MulVec3Mat4x3(&bone->m[9], &m2->mat4x3, &vec32);
                Vec3_MulScalarInPlace(&vec32.x, 0x8000);
                mParticle3 = Particle::System::New(mParticle3, 0xd0, sp14, sp18, sp1C,
                                                   (const Vector3_16f *)(&vec16), 0);
            }
        }
    } else {
        mStateArg = 0;
    }

    {
        s32 v = mPrevVertSpeed;
        s32 r1 = (v / 0x1000) * 3;
        if (r1 >= 0x100) {
            r1 = 0x100;
        }
        mLoopingSoundHandle =
            func_02012194((char *)mLoopingSoundHandle, 0, 0x104, 3, r1, &mCamSpacePos, 0);
    }
    Player_AdvanceAnims(c);
    return 1;
}
}
#undef spC
#undef spE
#undef sp10
#undef sp14
#undef sp18
#undef sp1C
#undef c

// @symbol _ZN6Player11St_Fly_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Fly_Init() {
    extern int data_0209f318[];
    extern void Player_ReleaseHeldActor(char *);
    extern void func_0200d6f0(void *, unsigned char);
    mAngleXSpeed = 0;
    mAngleYSpeed = 0;
    mPrevAngleX = 0;
    mPrevAngleZ = 0;
    mPrevAngleY = mAngleY;
    Player_ReleaseHeldActor(((char *)this));
    if (mStateStep == 0) {
        mVertSpeed = 0x5d000;
        this->SetAnim(0x4a, 0x40000000, 0x1000, 0);
        mTerminalVelocity = -0x4000;
    } else {
        mVertAccel = 0;
        this->SetAnim(0x49, 0, 0x1000, 0);
    }
    func_0200d6f0((void *)data_0209f318[0], mPlayerNo);
    return 1;
}
}

// @symbol func_ov002_020e032c
extern "C" {
void func_ov002_020e032c(unsigned char* c)
{
    extern unsigned char data_0209f2d8;
    extern int data_0209caa0;
    unsigned char st;
    if (c[0x6ff] == 0) return;
    c[0x6ff] = 0;
    st = data_0209f2d8;
    if ((int)(st == 1) == 0) {
        if (((&data_0209caa0)[2] & 0x80) == 0) return;
        if ((int)(st == 2) != 0) return;
    }
    ((Player *)c)->EndPowerupMusic(0x33);
}
}

// @symbol _ZN6Player16InitWingFeathersEb
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
void Player::InitWingFeathers(bool b_)
{
    extern unsigned char data_0209f2d8[];
    int b = (int)b_;

    int t0 = (data_0209f2d8[0] == 1);
    if (!t0) {
        if (param1 != 0) return;
    }
    func_ov002_020bdd2c();
    mHasWings = 1;
    mPowerupTimer = 0x708;
    {
        int t1 = (data_0209f2d8[0] == 1);
        if (!t1)
            StartPowerupMusic(0x33);
        else
            StartPowerupMusic(0x4e);
    }
    if (b == 0) return;
    func_ov002_020c43c4(1);
}
}

// @symbol func_ov002_020e0478
extern "C" {
int func_ov002_020e0478(Player *c) {
    extern Player::State data_ov002_02110214;
    return c->IsState(data_ov002_02110214) != 0;
}
}

// @symbol func_ov002_020e04a4
/* func_ov002_020e04a4 at 0x020e04a4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
extern "C" {
void func_ov002_020e04a4(Player *c) {
    extern short data_0209f4a0[];
  if (*(short*)((char*)data_0209f4a0 + gActivePlayerSlot*0x18) < 0x600) return;
  c->func_ov002_020bf88c();
}
}

// @symbol _ZN6Player12St_Land_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Land_Main()
{
    extern int func_ov002_020e3078(void* c, void* s);
    extern int Player_ScaleByCharFactor(void* c, int a);
    extern int func_ov002_020bf224(void* c, int a, int b);
    extern void Player_AdvanceAnims(void* c);
    extern u16 data_0209f49e[];
    extern u16 data_0209f49c[];
    extern s16 data_0209f4a0[];
    extern int data_ov002_0211052c[];
    extern Player::State data_ov002_0211013c;
    extern int AngleDiff(int, int);
    extern int data_ov002_0211019c[];
    extern int data_ov002_0211055c[];
    extern int func_ov002_020d5c6c(void*);
    extern int func_ov002_020dde74(void*);
    extern void func_ov002_020e04a4(void*);
    if (func_ov002_020c0434()) {
        func_ov002_020c0364(3);
        func_ov002_020c06fc(0x4000);
        return 1;
    }

    func_ov002_020e04a4(((char*)this));
    u16 temp_r3 = mStateFlags;

    if ((u16)(temp_r3 & 0x40) == 0) {
        if ((*(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18) & 2)
            || (u16)(temp_r3 & 0x100) != 0) {
            *(u16*)((char*)&mStateFlags) &= ~0x100;
            if (func_ov002_020e3078(((char*)this), data_ov002_0211055c) != 0
                && (*(u16*)((char*)data_0209f49c + gActivePlayerSlot * 0x18) & 0x400)
                && mHorzSpeed >= 0) {
                this->ChangeState(*reinterpret_cast<Player::State *>(data_ov002_0211055c));
                return 1;
            }
            if (this->IsAnim(0x4e) != 0) {
                mAngleY = mPrevAngleY;
            }
            int v = (mHeldObj != 0);
            if (v != 0) {
                mJumpComboTimer = 0;
            }
            this->ChangeState(*reinterpret_cast<Player::State *>(data_ov002_0211019c));
            return 1;
        }
        if (func_ov002_020d5c6c(((char*)this)) != 0) {
            return 1;
        }
        if (*(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18) & 1) {
            return func_ov002_020dde74(((char*)this));
        }
    } else {
        *(u16*)((char*)&mStateFlags) &= ~0x100;
    }

    ApproachLinear(
        *((int *)((char *)&mHorzSpeed)),
        func_ov002_020bf224(((char *)this), Player_ScaleByCharFactor(((char *)this), 0x20000), 0),
        0x4000);

    if (func_ov002_020e3078(((char*)this), data_ov002_0211052c) != 0) {
        mHorzSpeed = 0;
    }

    if (mStateTimer != 0) {
        goto tail;
    }

    if ((*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) != 0)
        || (this->FinishedAnim() != 0)) {
        if (this->IsAnim(0x1b) != 0) {
            this->SetAnim(0x2e, 0x40000000, 0x1000, 0);
            goto tail;
        }
        if (this->IsAnim(0x4e) != 0) {
            mAngleY = (s16)(mAngleY + 0x8000);
        }
        if ((*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) != 0)
            && AngleDiff(mAngleY, mPrevAngleY) >= 0x4000) {
            if (AngleDiff(mDesiredAngleY, mAngleY) >= 0x4000) {
                mAngleY = mPrevAngleY;
            } else {
                mHorzSpeed = 0 - mHorzSpeed;
            }
        }
        mPrevAngleY = mAngleY;
        this->ChangeState(data_ov002_0211013c);
        if (this->IsState(data_ov002_0211013c) != 0
            && mStateStep != 0
            && mHorzSpeed >= Player_ScaleByCharFactor(((char*)this), 0x1c000)) {
            mSpineRot[2] = 0x4000;
        }
        return 1;
    }

    if (func_ov002_020e3078(((char*)this), data_ov002_0211019c) == 0) {
        mHorzSpeed = 0;
    }

tail:
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player12St_Land_InitEv
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
#pragma push
#pragma opt_common_subs off
extern "C" {
int Player::St_Land_Init()
{
    int func_ov002_020e0a64(char* c);
    void func_ov002_020e25d4(char* c);
    int func_ov002_020e2c84(char* c);
    extern int data_ov002_0211019c;
    extern int data_ov002_021101b4;
    extern int data_ov002_021101e4;
    extern int data_ov002_0211052c;
    extern int data_ov002_02110724;
    extern int data_ov002_0211007c;
    extern int data_ov002_0211055c;
    extern int data_ov002_02110454;
    extern char data_0209f4a0[];
    char* c = (char*)this;
    int r4;

    this->func_ov002_020c2f64();
    r4 = (int)mPrevState;

    if (mJumpedFromQuicksand == 0) {
        int v = mSurfaceType;
        if (v >= 6 && v <= 9) {
            int s = mSinkDepth;
            if (s < 0x1e000) {
                mSinkDepth = 0x1e000;
            }
        }
    }

    int r1;
    if (r4 == (int)&data_ov002_0211019c) {
        r1 = func_ov002_020e0a64(c);
    } else if (r4 == (int)&data_ov002_021101b4) {
        r1 = 0x55;
    } else if (r4 == (int)&data_ov002_021101e4) {
        r1 = 0x4e;
    } else if (r4 == (int)&data_ov002_0211052c) {
        mPrevAngleY = mAngleY;
        mHorzSpeed = 0;
        r1 = 0x2b;
    } else if (r4 == (int)&data_ov002_02110724 || r4 == (int)&data_ov002_0211007c) {
        r1 = 0x51;
    } else if (r4 == (int)&data_ov002_0211055c) {
        r1 = 0x1b;
    } else if (r4 == (int)&data_ov002_02110454) {
        r1 = 0x60;
    } else {
        r1 = 0x52;
    }

    this->SetAnim(r1, 0x40000000, 0x1000, 0);

    int idx = gActivePlayerSlot;
    short sval = *(short*)((char*)&data_0209f4a0 + idx * 0x18);
    if (sval == 0 && (IsAnim(0x2b) || IsAnim(0x4c))) {
        func_ov002_020e25d4(c);
    }
    mStateStep = 0;
    if (r4 == (int)&data_ov002_0211019c && mJumpComboStage == 2) {
        mStateStep = 1;
    }

    mStateTimer = 2;
    if (func_ov002_020e2c84(c) != 2 && mIsInShallowWater == 0) {
        this->func_ov002_020bf9d4();
    }

    return 1;
}
}
#pragma pop

// @symbol func_ov002_020e0a64
extern "C" {
int func_ov002_020e0a64(Player *player) {
    extern int data_ov002_0210a5b4[];
    if (player->IsAnim(0x54) == 0)
        player->mJumpComboTimer = 8;
    if (player->mIsMega != 0)
        return 0x9b;
    return data_ov002_0210a5b4[player->mJumpComboStage];
}
}

// @symbol _ZN6Player21St_SmallLaunchUp_MainEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_SmallLaunchUp_Main()
{

    extern Player::State data_ov002_02110424;
    extern void Player_AdvanceAnims(void*);
  if(mIsAirborne==0){
    this->ChangeState(data_ov002_02110424);
  }
  Player_AdvanceAnims(((char*)this));
  return 1;
}
}

// @symbol _ZN6Player21St_SmallLaunchUp_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_SmallLaunchUp_Init()
{
  mJumpedFromQuicksand=0;
  mIsInAirState=1;
  mIsFallScreaming=0;
  mJumpComboStage=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  this->SetAnim(0x56, 0, 0x1000, 0);
  mVertSpeed=0x2a000;
  mHorzSpeed=0;
  mAngleX=mPrevAngleX;
  mAngleY=mPrevAngleY;
  mAngleZ=mPrevAngleZ;
  return 1;
}
}

// @symbol _ZN6Player17St_SlopeJump_MainEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_SlopeJump_Main() {
    extern int Player_AdvanceAnims(void *);
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_02110034;
    extern Player::State data_ov002_021105bc;
    extern unsigned short data_0209f49e[];
    ApproachLinear(*((int *)((char *)&mHorzSpeed)), 0, 0x800);
    if (mIsAirborne == 0) {
        this->ChangeState(data_ov002_0211013c);
    }
    if (*(unsigned short *)((char *)data_0209f49e + gActivePlayerSlot * 0x18) & 1) {
        mPrevAngleY = mAngleY;
        if (mIsMega == 0) {
            if (param1 == 3) {
                this->ChangeState(data_ov002_02110034);
            } else {
                this->ChangeState(data_ov002_021105bc);
            }
        }
    }
    Player_AdvanceAnims(((char *)this));
    return 1;
}
}

// @symbol _ZN6Player17St_SlopeJump_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_SlopeJump_Init()
{
    extern int func_ov002_020e2b6c(void*);
    extern int func_ov002_020e2ad0(void*);
    extern int func_ov002_020e25f0(void*,int);
  mJumpedFromQuicksand=0;
  if(func_ov002_020e2b6c(((char*)this))) return 1;
  mIsInAirState=1;
  mIsFallScreaming=0;
  mJumpComboStage=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  this->SetAnim(0x53, 0x40000000, 0x1000, 0);
  mVertSpeed=0x2a000;
  func_ov002_020e2ad0(((char*)this));
  if(mHorzSpeed>0){
      mHorzSpeed = FX_MUL(mHorzSpeed, 0xc00);
  }
  func_ov002_020e25f0(((char*)this),0);
  return 1;
}
}

// @symbol func_ov002_020e0ccc
extern "C" {
int func_ov002_020e0ccc(Player *c, short *st) {
    extern Player::State data_ov002_02110484;
    int b;
    if (st == 0)
        return 0;
    b = (int)(((dActor_c *)st)->actorID == 0xc1);
    if (b == 0)
        return 0;
    c->ChangeState(data_ov002_02110484);
    return 1;
}
}

// @symbol _ZN6Player22St_CrazedCrate_CleanupEv
int Player::St_CrazedCrate_Cleanup() {
    mdCcAcPos_c.flags &= ~0x20;
    mJumpComboStage = 0;
    return 1;
}

// @symbol _ZN6Player19St_CrazedCrate_MainEv
extern "C" {
int Player::St_CrazedCrate_Main() {
    typedef long long s64;
    extern short data_02082214[];
    extern void func_ov002_020e28d4(void*, int, int);
    extern int atan2_hold_Player_2(int, int);
    extern int AngleDiff(int, int);
    extern void func_ov002_020e0f38(void*, int);
    extern void Player_ReleaseHeldActor(void*);
    extern Player::State data_ov002_0211031c;
    func_ov002_020e28d4(this, 0x1000, 0x1800);

    if (mClsnFlags & 2) {
        int atan = cstd::atan2(mWallNormalX, this->mWallNormalZ);
        int diff = AngleDiff(atan, this->mAngleY);
        if (diff >= 0x6000) {
            int t = (s16)(2 * atan - this->mAngleY);
            t += 0x8000;
            mAngleY = t;
            int half = 0x800;
            int *xpos = (int *)(int)(&mPosX);
            int* zpos;
            mPrevAngleY = mAngleY;
            *xpos +=
                (int)(((s64)mHorzSpeed * data_02082214[(((int)(u16)mPrevAngleY >> 4) << 1) + 1] +
                       half) >>
                      12);
            zpos = (int *)(int)(&mPosZ);
            *zpos +=
                (int)(((s64)mHorzSpeed * data_02082214[((int)(u16)mPrevAngleY >> 4) << 1] + half) >>
                      12);
        }
    }

    mAngleY = mPrevAngleY;

    if (mIsAirborne == 0) {
        if (mJumpComboStage < 2) {
            (*(u8 *)(int)(&mJumpComboStage))++;
            func_ov002_020e0f38(this, this->mJumpComboStage);
        } else {
            if (param1 == 3) {
                if ((void *)mObjInMouth != 0) {
                    ((fBase_c *)mObjInMouth)->MarkForDestruction();
                }
                Vector3 pos;
                pos.x = mPosX;
                pos.y = mPosY;
                pos.z = mPosZ;
                this->SpawnCoins(pos, 5, 0xf000, 0);
                func_ov002_020d718c();
            } else {
                Player_ReleaseHeldActor(this);
            }
            this->ChangeState(data_ov002_0211031c);
            this->SetAnim(0x43, 0x40000000, 0x1000, 0);
        }
    }

    int v98 = mHorzSpeed;
    int va8 = mVertSpeed;
    int atan2 = cstd::atan2(v98 >> 8, va8 >> 8);
    mAngleX = (s16)(atan2 - 0x4000);

    return 1;
}
}

// @symbol func_ov002_020e0f38
extern "C" {
void func_ov002_020e0f38(char* c, int a){
    extern int data_ov002_020ff17c[];
    extern int data_ov002_020ff188[];
    Player *player = (Player *)c;
    player->mVertSpeed = data_ov002_020ff17c[a];
    player->mHorzSpeed = data_ov002_020ff188[a];
    if (a != 0)
        player->LandingDust(true);
    if (a != 2)
        func_0201267c(0x6c, &player->mCamSpacePos);
    else
        func_0201267c(0x6d, &player->mCamSpacePos);
}
}

// @symbol _ZN6Player19St_CrazedCrate_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* Player::St_CrazedCrate_Init() at 0x020e0fac (ov002), size 0x74
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
extern "C" {
int Player::St_CrazedCrate_Init()
{
    extern void func_ov002_020e0f38(void*, unsigned char);
  mJumpedFromQuicksand = 0;
  mIsInAirState = 1;
  mIsFallScreaming = 0;
  mJumpComboStage = 0;
  mIsAirborne = 1;
  mLandSoundPlayed = 0;
  this->SetAnim(0x43, 0x40000000, 0x1000, 0);
  func_ov002_020e0f38(((char*)this), mJumpComboStage);
  mPrevAngleY = mAngleY;
  *(int*)((char*)&mdCcAcPos_c.flags) |= 0x20;
  return 1;
}
}

// @symbol _ZN6Player10SpinBounceEi
/* recovered: named members + shared header */
extern "C" {
void Player::SpinBounce(Fix12i f) {
    extern Player::State data_ov002_021105a4;
    extern Player::State data_ov002_02110454;
    if (this->IsState(data_ov002_021105a4))
        return;
    if (this->GetHealth() == 0)
        return;
    this->mVertSpeed = f;
    Sound::PlayCharVoice(this->mCharacter, 0x28, this->mCamSpacePos);
    Sound::PlayBank0(0xb6, this->mCamSpacePos);
    this->mStatePhase = 1;
    this->ChangeState(data_ov002_02110454);
}
}

// @symbol func_ov002_020e10a8
extern "C" {
int func_ov002_020e10a8(Player *c) {
    extern Player::State data_ov002_02110454;
    return c->IsState(data_ov002_02110454) != 0;
}
}

// @symbol _ZN6Player15St_Spin_CleanupEv
/* recovered: named members + shared header, real C++ method */
// Player::St_Spin_Cleanup - clear flag bit 0x20 in the u32 at this+0x2ec, return 1.
// The u64-mask launder forces the base to materialize (add r2,this,#0x2ec) so the
// load/store reuse it, matching the ROM instead of folding the offset into ldr/str.
int Player::St_Spin_Cleanup()
{
    *(unsigned int *)((char *)&mdCcAcPos_c.flags) &= ~0x20;
    return 1;
}

// @symbol _ZN6Player12St_Spin_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Spin-jump, per frame. Landing hands off to another state after snapping the
 * facing angle back to mPrevAngleY. The global bit at data_0209f49c selects
 * between two spin rates and two terminal velocities.
 */
extern "C" {
int Player::St_Spin_Main()
{
    extern u16 data_0209f49c[];
    extern Player::State data_ov002_02110424;
    extern void func_ov002_020e28d4(void *thiz, int a, int b);
    extern void Player_AdvanceAnims(void *thiz);
    func_ov002_020e28d4(this, 0x1000, 0x1000);
    if (mIsAirborne == 0) {
        mAngleY = mPrevAngleY;
        this->ChangeState(data_ov002_02110424);
    }
    if (*(u16*)((char*)data_0209f49c + gActivePlayerSlot * 0x18) & 2) {
        mTerminalVelocity = -0x9000;
        mAngleY += 0x2000;
    } else {
        mTerminalVelocity = -0xc000;
        mAngleY += 0x1800;
    }
    Player_AdvanceAnims(this);
    return 1;
}
}

// @symbol _ZN6Player12St_Spin_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Spin_Init()
{
    struct dCamera_c;
    extern int* data_0209f318;
    extern void func_ov002_020e25f0(void* c, int i);
    extern void func_0200d678(dCamera_c* thiz, unsigned char pid);
  mJumpedFromQuicksand = 0;
  mIsInAirState = 1;
  mIsFallScreaming = 0;
  mJumpComboStage = 0;
  mIsAirborne = 1;
  mLandSoundPlayed = 0;
  this->SetAnim(0x5f, 0, 0x1000, 0);
  mTerminalVelocity = -0x10000;
  if (mStatePhase) {
    mVertSpeed = 0x50000;
    func_ov002_020e25f0(((char*)this), 2);
  }
  int *p = (int *)&mdCcAcPos_c.flags;
  int old = *p;
  int** cam_ptr_ptr = &data_0209f318;
  *p = old | 0x20;
  unsigned char pid = mPlayerNo;
  func_0200d678((dCamera_c*)*cam_ptr_ptr, pid);
  return 1;
}
}

// @symbol _ZN6Player16St_LongJump_MainEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_LongJump_Main()
{
    extern int func_ov002_020e28d4(void*,int,int);
    extern int Player_AdvanceAnims(void*);
    extern Player::State data_ov002_02110424;
  func_ov002_020e28d4(((char*)this),0x1800,0x800);
  if(mIsAirborne==0){
    this->ChangeState(data_ov002_02110424);
  }
  Player_AdvanceAnims(((char*)this));
  return 1;
}
}

// @symbol _ZN6Player16St_LongJump_InitEv
/* recovered: named members + shared header, real C++ method
 *
 * Three guards get first refusal on the long jump; if any takes it, this
 * returns without entering. Otherwise: airborne flags, the jump animation,
 * an upward kick, then horizontal speed scaled by 1.5 (0x1800 in 12.4 fixed
 * point, with the usual +0x800 round-to-nearest) and clamped to 0x3c000.
 */
extern "C" {
int Player::St_LongJump_Init()
{
    extern int func_ov002_020e2be4(void* self);
    extern int func_ov002_020e2ba8(void* c);
    extern int func_ov002_020e2b6c(void* c);
    extern void func_ov002_020e2ad0(void* c);
    mJumpedFromQuicksand = 0;
    if (func_ov002_020e2be4(this)) {
        return 1;
    }
    if (func_ov002_020e2ba8(this)) {
        return 1;
    }
    if (func_ov002_020e2b6c(this)) {
        return 1;
    }
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    mJumpComboStage = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    this->SetAnim(0x1a, 0x40000000, 0x1000, 0);
    mVertSpeed = 0x1e000;
    func_ov002_020e2ad0(this);
    mVertAccel = -0x2000;
    mHorzSpeed = FX_MUL(mHorzSpeed, 0x1800);
    if (mHorzSpeed >= 0x3c000) {
        mHorzSpeed = 0x3c000;
    }
    /* mCamSpacePosX/Y/Z are three consecutive words; dActor_c.h declares them
       individually rather than as one Vector3. */
    if (mIsMetal == 0) {
        Sound::PlayBank0((u32)mGroundSoundType + 0x30, mCamSpacePos);
    } else {
        Sound::PlayBank0(0xa0, mCamSpacePos);
    }
    Sound::PlayCharVoice(mCharacter, 4, mCamSpacePos);
    return 1;
}
}

// @symbol _ZN6Player16St_SideFlip_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_SideFlip_Main()
{
    extern void func_ov002_020eeca8(void* a, void* b);
    extern void func_ov002_020e28d4(void* c, u32 a, u32 b);
    extern void Player_AdvanceAnims(void* c);
    extern Player::State data_ov002_02110424;
    extern u16 data_0209f49e[];
    extern int data_ov002_0211052c[];
    extern u16 data_0209f49c[];
    extern Player::State data_ov002_02110454;
    extern int data_ov002_021101e4[];
    extern int data_ov002_021101fc[];
    extern int data_ov002_02110694[];
    extern int func_ov002_020e2664(void*);
    func_ov002_020eeca8(&mMeshClsn, ((char *)this));

    if (IsState(*(Player::State *)data_ov002_02110694)) {
        if (mVertSpeed < 0) {
            mHorzSpeed = 0x12000;
        }
        func_ov002_020e28d4(((char*)this), 0, 0x800);
    } else {
        func_ov002_020e28d4(((char*)this), 0x1800, 0x800);
    }

    if (mIsAirborne == 0) {
        this->ChangeState(data_ov002_02110424);
    } else {
        if (!IsState(*(Player::State *)data_ov002_021101fc)) {
            u16 r0v = *(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18);
            if (r0v & 0x400) {
                if (IsState(*(Player::State *)data_ov002_0211052c)) {
                    mPrevAngleY = mAngleY;
                }
            }
            if (func_ov002_020e2664(((char*)this))) {
                return 1;
            }
        } else {
            if (this->FinishedAnim()) {
                this->SetAnim(0x54, 0x40000000, 0x1000, 0);
            }
        }

        mVertAccel = -0x4000;

        if (mVertSpeed >= 0) {
            u16 r1v = *(u16*)((char*)data_0209f49c + gActivePlayerSlot * 0x18);
            if ((r1v & 2) != 0
                || IsState(*(Player::State *)data_ov002_021101e4)
                || IsState(*(Player::State *)data_ov002_021101fc)
                || IsState(*(Player::State *)data_ov002_0211052c)) {
                mVertAccel = -0x3400;
            }
        } else {
            if (param1 == 1
                && IsState(*(Player::State *)data_ov002_0211052c)) {
                mStatePhase = 0;
                this->ChangeState(data_ov002_02110454);
                return 1;
            }
        }

        if (IsState(*(Player::State *)data_ov002_021101e4)) {
            int id = this->GetBodyModelID(param1 & 0xff, 0);
            dExtAnmModel_c *anim = mModels.mBodyModels[id];
            u32 w = ((dExtFrameCtrl_c *)((long long)(int)&(dExtFrameCtrl_c &)*anim))->currFrame;
            if ((u16)(w >> 12) == 0x10) {
                Sound::PlayBank0(0xf, mCamSpacePos);
            }
        }
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

// @symbol _ZN6Player17St_WaterJump_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_WaterJump_Init()
{
    extern int func_ov002_020e25f0(void*,int);
  mJumpedFromQuicksand=0;
  mIsInAirState=1;
  mIsFallScreaming=0;
  mJumpComboStage=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  this->SetAnim(0x53, 0x40000000, 0x1000, 0);
  mVertSpeed=0x2a000;
  mHorzSpeed=0x12000;
  Sound::PlayBank0(0x18, mCamSpacePos);
  func_ov002_020e25f0(((char*)this),0);
  return 1;
}
}

// @symbol _ZN6Player21St_HeadstandJump_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_HeadstandJump_Init()
{
  mJumpedFromQuicksand=0;
  mIsInAirState=1;
  mIsFallScreaming=0;
  mJumpComboStage=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  this->SetAnim(0x1c, 0x40000000, 0x1000, 0);
  mVertSpeed=0x3e000;
  mHorzSpeed=0x18000;
  Sound::PlayCharVoice(mCharacter, 0xc, mCamSpacePos);
  return 1;
}
}

// @symbol _ZN6Player16St_WallJump_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_WallJump_Main()
{
    extern int func_ov002_020eeca8(void*, void*);
    extern int func_ov002_020e28d4(void*, int, int);
    extern int Player_AdvanceAnims(void*);
    extern Player::State data_ov002_02110424;
    extern unsigned short data_0209f49e[];
    extern Player::State data_ov002_0211052c;
    extern void (Player::*data_ov002_0211073c[])();
    extern int func_ov002_020e2664(void*);
    func_ov002_020eeca8(&mMeshClsn, ((void *)this));
    func_ov002_020e28d4(((void *)this), 0x1800, 0x800);
    if (*(unsigned char *)((char *)&mIsAirborne) == 0) {
        this->ChangeState(data_ov002_02110424);
    } else {
        if (*(unsigned short *)((char *)data_0209f49e + gActivePlayerSlot * 0x18) & 0x400) {
            if (this->IsState(data_ov002_0211052c)) {
                *(short *)((char *)&mPrevAngleY) = *(short *)((char *)&mAngleY);
            }
        }
        if (func_ov002_020e2664(((void *)this)))
            return 1;
        (this->*data_ov002_0211073c[param1])();
    }
  Player_AdvanceAnims(((void*)this));
  return 1;
}
}

// @symbol _ZN6Player16St_WallJump_InitEv
/* recovered: named members + shared header, real C++ method
 *
 * ov002, 0x020e17f8, size 0xb0. Entering the wall jump: clear the airborne
 * bookkeeping, start anim 0x28, set the launch speeds, TURN AROUND, and shout.
 *
 * The turn is the whole move. mAngleY += 0x8000 is exactly half a revolution
 * on a 16-bit angle, and mPrevAngleY is snapped to the new value in the same
 * breath so nothing interpolates through the 180 degrees -- the player faces
 * away from the wall instantly rather than spinning.
 *
 * Every offset the placeholder body carried is a named field of Player or of
 * its dActor_c base, so the shadow `struct Player` it used to declare is gone.
 * The vertical speed is chosen by param1: character 2 gets a fixed 0x2aa00,
 * everyone else goes through the ov002 worker with 0x3e000.
 *
 * `x = x + k` AND `x += k` ARE NOT THE SAME FUNCTION HERE, and that one line
 * is the whole difference between this file matching and not. On the s16
 * member the compound form makes mwcc materialise &mAngleY into a register --
 * `add r,r,#0x8e` then `ldrsh r,[r]` and later `strh r,[r]` -- one instruction
 * more than the ROM, which addresses the field directly off the object both
 * times (`ldrsh r,[r,#0x8e]` / `strh r,[r,#0x8e]`). Written out longhand it
 * matches. Nothing else in the function was ever wrong: the draft was 44 of 45
 * instructions correct and had been sitting unenrolled for it.
 */
extern "C" {
int Player::St_WallJump_Init()
{
    mJumpedFromQuicksand = 0;
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    mJumpComboStage = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    this->SetAnim(0x28, 0x40000000, 0x1000, 0);
    if (param1 == 2) {
        mVertSpeed = 0x2aa00;
    } else {
        func_ov002_020bf2d8(0x3e000);
    }
    mHorzSpeed = 0x18000;
    mAngleY = mAngleY + 0x8000;
    mPrevAngleY = mAngleY;
    mStateWork = 1;
    mStateStep = 0;
    Sound::PlayCharVoice(mCharacter, 0xc, mCamSpacePos);
    return 1;
}
}

// @symbol _ZN6Player16St_BackFlip_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_BackFlip_Init()
{
    extern int func_ov002_020e2be4(void*);
    extern int func_ov002_020e2ba8(void*);
    extern int func_ov002_020e2b6c(void*);
    extern int func_ov002_020e2ad0(void*);
    extern int func_ov002_020e25f0(void*, int);
  *(char*)((char*)&mJumpedFromQuicksand)=0;
  if (func_ov002_020e2be4(((void*)this))) return 1;
  if (func_ov002_020e2ba8(((void*)this))) return 1;
  if (func_ov002_020e2b6c(((void*)this))) return 1;
  *(char*)((char*)&mIsInAirState)=1;
  *(char*)((char*)&mIsFallScreaming)=0;
  *(char*)((char*)&mJumpComboStage)=0;
  *(char*)((char*)&mIsAirborne)=1;
  *(char*)((char*)&mLandSoundPlayed)=0;
  this->SetAnim(0x2a, 0x40000000, 0x1000, 0);
  *(int*)((char*)&mVertSpeed)=0x3e000;
  if (*(int*)((char*)&param1)==1) *(int*)((char*)&mVertSpeed)=0x4c000;
  func_ov002_020e2ad0(((void*)this));
  *(int*)((char*)&mHorzSpeed)=0x10000;
  *(short*)((char*)&mPrevAngleY) = *(short*)((char*)&mAngleY) + 0x8000;
  func_ov002_020e25f0(((void*)this), 0);
  return 1;
}
}

// @symbol _ZN6Player24St_SlideKickRecover_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_SlideKickRecover_Init()
{
    extern int Player_ReleaseHeldActor(void*);
    extern int func_ov002_020e25f0(void*, int);
  *(char*)((char*)&mJumpedFromQuicksand)=0;
  *(char*)((char*)&mIsInAirState)=1;
  *(char*)((char*)&mIsFallScreaming)=0;
  Player_ReleaseHeldActor(((void*)this));
  *(char*)((char*)&mJumpComboStage)=0;
  *(char*)((char*)&mIsAirborne)=1;
  *(char*)((char*)&mLandSoundPlayed)=0;
  *(int*)((char*)&mVertSpeed)=0x20000;
  this->SetAnim(0x56, 0x40000000, 0x1000, 0);
  func_ov002_020e25f0(((void*)this), 0);
  Sound::PlayBank0(0xf, mCamSpacePos);
  return 1;
}
}

// @symbol _ZN6Player16St_SideFlip_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_SideFlip_Init()
{
    extern int func_ov002_020e2be4(void*);
    extern int func_ov002_020e2ba8(void*);
    extern int func_ov002_020e2b6c(void*);
    extern int func_ov002_020e2ad0(void*);
    extern int func_ov002_020e25f0(void*, int);
  *(char*)((char*)&mJumpedFromQuicksand)=0;
  if (func_ov002_020e2be4(((void*)this))) return 1;
  if (func_ov002_020e2ba8(((void*)this))) return 1;
  if (func_ov002_020e2b6c(((void*)this))) return 1;
  *(char*)((char*)&mIsInAirState)=1;
  *(char*)((char*)&mIsFallScreaming)=0;
  *(char*)((char*)&mJumpComboStage)=0;
  *(char*)((char*)&mIsAirborne)=1;
  *(char*)((char*)&mLandSoundPlayed)=0;
  *(int*)((char*)&mHorzSpeed)=0x8000;
  *(int*)((char*)&mVertSpeed)=0x3e000;
  func_ov002_020e2ad0(((void*)this));
  this->SetAnim(0x4d, 0x40000000, 0x1000, 0);
  *(short*)((char*)&mPrevAngleY) = *(short*)((char*)&mDesiredAngleY);
  *(short*)((char*)&mAngleY) = *(short*)((char*)&mDesiredAngleY) + 0x8000;
  func_ov002_020e25f0(((void*)this), 0);
  return 1;
}
}

// @symbol _ZN6Player12St_Fall_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Fall_Main()
{
    extern int func_ov002_020e28d4(void*, int, int);
    extern int Player_AdvanceAnims(void*);
    extern Fix12i Player_ScaleByCharFactor(void* c, Fix12i a);
    extern Player::State data_ov002_02110424;
    extern int func_ov002_020e2664(void*);
  func_ov002_020e28d4(((void*)this), 0x1800, 0x800);
  if (*(unsigned char*)((char*)&mIsAirborne) == 0) {
    this->ChangeState(data_ov002_02110424);
  } else if (func_ov002_020e2664(((void*)this))) {
    return 1;
  }
  Player_AdvanceAnims(((void*)this));
  if (*(int*)((char*)&mHorzSpeed) >= Player_ScaleByCharFactor(((void*)this), 0x28000))
    *(int*)((char*)&mHorzSpeed) = Player_ScaleByCharFactor(((void*)this), 0x28000);
  return 1;
}
}

// @symbol _ZN6Player12St_Fall_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Fall_Init()
{
  *(char*)((char*)&mJumpedFromQuicksand)=0;
  *(char*)((char*)&mIsInAirState)=1;
  *(char*)((char*)&mIsFallScreaming)=0;
  *(char*)((char*)&mJumpComboStage)=0;
  *(char*)((char*)&mIsAirborne)=1;
  *(char*)((char*)&mLandSoundPlayed)=0;
  if (this->IsAnim(0x41)) {
      this->SetAnim(0x44, 0x40000000, 0x1000, 0);
      *(char *)((char *)&mStateStep) = 1;
  } else {
      this->SetAnim(0x54, 0x40000000, 0x1000, 0);
      *(char *)((char *)&mStateStep) = 0;
  }
  *(int*)((char*)&mPeakY) = *(int*)((char*)&mPosY);
  return 1;
}
}

// @symbol func_ov002_020e1c20
extern "C" {
void func_ov002_020e1c20(char *c) {
    extern int func_02012120(unsigned int p0, unsigned int p1, unsigned int p2, const Vector3 *p3,
                             short p4);
    extern int func_ov002_020bf27c(char *c, int r1);
    extern unsigned char data_0209f49c[];
    extern Player::State data_ov002_021101b4;
    Player *player = (Player *)c;
    if ((player->mClsnFlags) & 0xc) {
        ((Player *)c)->ChangeState(data_ov002_021101b4);
        return;
    }

    if ((player->mStateStep) != 0) {
        if ((*((unsigned short *)(&data_0209f49c[gActivePlayerSlot * 0x18]))) & 2) {
            if ((player->mClsnFlags) & 4) {
                player->mVertSpeed = -0x20000;
            }
            player->mLoopingSoundHandle =
                func_02012120(player->mLoopingSoundHandle, player->mCharacter, 0x18,
                              (const Vector3 *)(&player->mCamSpacePosX), 0);
            player->mHorzSpeed = FX_MUL(player->mHorzSpeed, 0xf00);
            if ((player->mVertSpeed) < 0) {
                int *p = (int *)(((long long)(&player->mVertSpeed)));
                *p = *p + 0xc00;
            } else {
                int *p = (int *)(((long long)(&player->mVertSpeed)));
                *p = *p + 0x1000;
            }
            if ((player->mVertSpeed) < 0x11000) {
                return;
            }
        }
        player->mStateStep = 0;
        player->mStateWork = 0;
        ((Player *)c)->SetAnim(0x54, 0x40000000, 0x1000, 0);
        return;
    }

    player->mVertAccel = func_ov002_020bf27c(c, -0x4000);
    if ((player->mVertSpeed) >= 0) {
        player->mVertAccel = -0x8000;
        if ((*((unsigned short *)(&data_0209f49c[gActivePlayerSlot * 0x18]))) & 2) {
            player->mVertAccel = func_ov002_020bf27c(c, -0x3000);
        }
        return;
    }

    if ((player->mVertSpeed) >= -0x8000) {
        return;
    }

    if ((*((unsigned short *)(&data_0209f49c[gActivePlayerSlot * 0x18]))) & 2) {
        if ((player->mStateWork) == 1) {
            player->mStateStep = 1;
            player->mVertAccel = 0;
            ((Player *)c)->SetAnim(0x3f, 0, 0x1000, 0);
            dExtFrameCtrl_c &anim =
                *player->mModels
                     .mBodyModels[((Player *)c)->GetBodyModelID(player->param1 & 0xff, 0)];
            anim.speed = 0x4000;
        }
    }
}
}

// @symbol func_ov002_020e1e70
extern "C" {
void func_ov002_020e1e70(char *c) {
    extern int func_02012120(unsigned int p0, unsigned int p1, unsigned int p2, const Vector3 *p3,
                             short p4);
    extern int func_ov002_020bf27c(char *c, int r1);
    extern unsigned char data_0209f49c[];
    Player *player = (Player *)c;
    player->mVertAccel = -0x4000;
    player->mTerminalVelocity = -0x4b000;
    if ((player->mVertSpeed) >= 0) {
        player->mVertAccel = -0x8000;
        if ((*((unsigned short *)(&data_0209f49c[gActivePlayerSlot * 0x18]))) & 2) {
            player->mVertAccel = -0x3400;
        }
    }
    if ((player->mVertSpeed) >= 0) {
        return;
    }
    if ((*((unsigned short *)(&data_0209f49c[gActivePlayerSlot * 0x18]))) & 2) {
        ((Player *)c)->SetAnim(0x3f, 0, 0x1000, 0);
        dExtFrameCtrl_c &anim =
            *player->mModels.mBodyModels[((Player *)c)->GetBodyModelID(player->param1 & 0xff, 0)];
        anim.speed = 0x4000;
        player->mLoopingSoundHandle =
            func_02012120(player->mLoopingSoundHandle, player->mCharacter, 0x18,
                          (const Vector3 *)(&player->mCamSpacePosX), 0);
        if ((player->mHasWings) != 0) {
            player->mVertAccel = func_ov002_020bf27c(c, -0x4000) >> 1;
            player->mTerminalVelocity = -0x25800;
        } else {
            player->mVertAccel = func_ov002_020bf27c(c, -0x4000);
        }
    } else if (((Player *)c)->IsAnim(0x3f) != 0) {
        ((Player *)c)->SetAnim(0x54, 0, 0x1000, 0);
    }
}
}

// @symbol func_ov002_020e200c
extern "C" {
void func_ov002_020e200c(char* c){
    extern int func_ov002_020bf27c(char* c, int r1);
    extern unsigned char data_0209f49c[];
    Player *player = (Player *)c;
    player->mVertAccel = func_ov002_020bf27c(c, -0x4000);
    player->mTerminalVelocity = -0x4b000;
    if (player->mVertSpeed >= 0) {
        player->mVertAccel = -0x8000;
        if (*(unsigned short *)&data_0209f49c[gActivePlayerSlot * 0x18] & 2)
            player->mVertAccel = -0x3400;
    }
    if (player->mVertSpeed >= 0)
        return;
    if (player->mJumpComboStage == 1)
        ((Player *)c)->SetAnim(0x50, 0x40000000, 0x1000, 0);
    if ((*(unsigned short *)&data_0209f49c[gActivePlayerSlot * 0x18] & 2) == 0)
        return;
    if (player->mHasWings == 0)
        return;
    player->mVertAccel = -0x2000;
    player->mTerminalVelocity = -0x25800;
}
}

// @symbol _ZN6Player12St_Jump_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Jump_Main()
{
    extern int func_ov002_020eeca8(void*, void*);
    extern int func_ov002_020e28d4(void*, int, int);
    extern int Player_AdvanceAnims(void*);
    extern Player::State data_ov002_02110424;
    extern u16 data_0209f49e[];
    extern u8 data_0209f4ab[];
    extern void (Player::*data_ov002_0211073c[])();
    extern int func_ov002_020e2664(void*);
    func_ov002_020eeca8(&mMeshClsn, ((void *)this));
    func_ov002_020e28d4(((void *)this), 0xd00, 0x800);

    if (*(u8 *)((char *)&mIsAirborne) == 0) {
        if (*(u16 *)((char *)&mStateTimer) != 0) {
            u16 *q = (u16 *)((char *)&mStateFlags);
            *q |= 0x100;
        }
        this->ChangeState(data_ov002_02110424);
    } else {
        {
            int off = gActivePlayerSlot * 0x18;
            u8 *p = (u8 *)((char *)data_0209f4ab + off);
            if (*p != 0) {
                *p = 5;
            }
            if (*(u16 *)((char *)data_0209f49e + off) & 2) {
                *(u16 *)((char *)&mStateTimer) = 5;
            }
        }

        if (func_ov002_020e2664(((void *)this)))
            return 1;

        if (*(u8 *)((char *)&mJumpComboStage) == 2) {
            *(s32 *)((char *)&mVertAccel) = -0x4000;
            *(s32 *)((char *)&mTerminalVelocity) = -0x4b000;
            if (*(s32 *)((char *)&mVertSpeed) >= 0) {
                *(s32 *)((char *)&mVertAccel) = -0x3400;
            }

            {
                u32 id = this->GetBodyModelID(*(u32 *)((char *)&param1) & 0xff, 0);
                dExtAnmModel_c *anim = mModels.mBodyModels[id];
                u32 w = ((dExtFrameCtrl_c *)((long long)(int)&(dExtFrameCtrl_c &)*anim))->currFrame;
                u16 t = (u16)(w >> 12);
                if (t == 4 || t == 0x18 || t == 0x2c) {
                    Sound::PlayBank0(0xf, mCamSpacePos);
                }
            }
        } else {
            int idx = *(int *)((char *)&param1);
            if (*(u8 *)((char *)&mIsMega) != 0) {
                idx = 0;
            }
            (this->*data_ov002_0211073c[idx])();
        }
    }

  Player_AdvanceAnims(((void*)this));
  return 1;
}
}

// @symbol _ZN6Player12St_Jump_InitEv
/* recovered: named members + shared header, real C++ method
 *
 * Entering the jump state. Two guards get first refusal. Then the triple-jump
 * combo stage advances (mod 3) if the combo timer is still running, and is
 * reset by any of: sinking deeper than 0x1000, carrying something, being mega,
 * or -- at stage 2 without wings -- not running fast enough. Wings at stage 2
 * divert to a different state entirely; otherwise the per-stage launch speed
 * and animation are looked up by stage, speed is scaled by 0.8, quartered on a
 * sharp turn, and zeroed if a ledge check fires.
 */
/* launch speed, by combo stage */
/* jump animation, by combo stage */
/* the winged-jump state */
extern "C" {
int Player::St_Jump_Init()
{
    extern int func_ov002_020e2be4(void *c);
    extern int func_ov002_020e2b6c(void *c);
    extern int Player_ScaleByCharFactor(void *c, int a);
    extern void func_ov002_020e2ad0(void *c);
    extern int AngleDiff(int a, int b);
    extern void func_ov002_020e25f0(void *c, int a);
    extern int data_ov002_0210a59c[];
    extern int data_ov002_0210a5a8[];
    extern Player::State data_ov002_02110214;
    int t;

    mJumpedFromQuicksand = 0;
    if (func_ov002_020e2be4(this) != 0)
        return 1;
    if (func_ov002_020e2b6c(this) != 0)
        return 1;

    mIsInAirState = 1;
    mIsFallScreaming = 0;
    if (mJumpComboTimer == 0) {
        mJumpComboStage = 0;
    } else {
        mJumpComboStage = (mJumpComboStage + 1) % 3;
    }

    if (mSinkDepth > 0x1000)
        goto clear;
    t = (mHeldObj != 0);
    if (t == false)
        goto skipclear;
clear:
    mJumpComboStage = 0;
skipclear:

    if (mIsMega != 0)
        mJumpComboStage = 0;

    if (mJumpComboStage == 2 && mHasWings == 0) {
        if (mHorzSpeed <= Player_ScaleByCharFactor(this, 0x14000))
            mJumpComboStage = 0;
    }

    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    if (mHasWings == 1 && mJumpComboStage == 2) {
        mStateStep = 0;
        this->ChangeState(data_ov002_02110214);
    } else {
        int v = data_ov002_0210a59c[mJumpComboStage] + (mHorzSpeed >> 2);
        if (mIsMega != 0) {
            func_ov002_020bf2d8(FX_MUL(v, 0x1400));
        } else {
            func_ov002_020bf2d8(v);
        }
        func_ov002_020e2ad0(this);
        if (mIsMega == 0) {
            this->SetAnim(data_ov002_0210a5a8[mJumpComboStage], 0x40000000, 0x1000, 0);
        } else {
            this->SetAnim(0x9c, 0x40000000, 0x1000, 0);
        }
        if (mHorzSpeed > Player_ScaleByCharFactor(this, 0x64000)) {
            mHorzSpeed = Player_ScaleByCharFactor(this, 0x64000);
        }
    }

    mStateWork = 0;
    if (param1 == 3) {
        if (mJumpComboStage != 2)
            mStateWork = 1;
    }

    mStateStep = 0;
    mStateArg = 0;
    unk_6cc = 0;
    func_ov002_020e25f0(this, mJumpComboStage);

    mHorzSpeed = FX_MUL(mHorzSpeed, 0xccc);

    if (AngleDiff(mPrevAngleY, mAngleY) >= 0x4000) {
        mHorzSpeed >>= 2;
    }

    if (func_ov002_020c19d0(0x64, 0x32) != 0) {
        mStateFlags |= 0x200;
        mHorzSpeed = 0;
    }
    return 1;
}
}

// @symbol func_ov002_020e25d4
/* recovered: shared common types */
extern "C" {
extern "C" void func_ov002_020e25d4(char *c)
{
    Player *player = (Player *)c;
    Sound::PlayCharVoice(player->mCharacter, 0x10, player->mCamSpacePos);
}
}

// @symbol func_ov002_020e25f0
extern "C" {
inline int inline_fn(int *arg0) {
    extern int RandomIntInternal(int *seed);
    return RandomIntInternal(arg0);
}
void func_ov002_020e25f0(char *c, int a) {
    extern int RandomIntInternal(int *seed);
    extern int data_ov002_0210e160[];
    extern unsigned int data_ov002_020ff284[];
    Player *p = (Player *)c;
    if ((p->mIsMetal) == 0) {
        Sound::PlayBank0((p->mGroundSoundType) + 0x30, p->mCamSpacePos);
    } else {
        Sound::PlayBank0(0xa0, p->mCamSpacePos);
    }
    int idx = (a * 2) + (inline_fn(data_ov002_0210e160) & 1);
    Sound::PlayCharVoice(p->mCharacter, data_ov002_020ff284[idx], p->mCamSpacePos);
}
}

// @symbol func_ov002_020e2664
extern "C" {
int func_ov002_020e2664(char* self)
{
    extern int func_ov002_020d5c6c(char* c);
    extern int func_ov002_020d674c(char* c);
    extern int func_ov002_020dde74(char* thiz);
    extern u16 data_0209f49e[];
    extern Player::State data_ov002_021101e4;
    extern Player::State data_ov002_0211004c;
    extern Player::State data_ov002_021105a4;
    extern Player::State data_ov002_021105d4;
    extern Player::State data_ov002_0211019c;
    extern Player::State data_ov002_021101b4;
    extern Player::State data_ov002_02110724;
    extern Player::State data_ov002_021101fc;
    extern Player::State data_ov002_0211052c;
    extern Player::State data_ov002_0211055c;
    extern Player::State data_ov002_02110694;
    extern Player::State data_ov002_02110034;
    extern Player::State data_ov002_021105bc;
    extern Player::State data_ov002_02110574;
    Player *p = (Player *)self;
    u16 flags;
    int ptr;
    int b1;
    int b2;

    if (func_ov002_020d5c6c(self)) return 1;

    flags = *(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18);
    if (flags & 0x400) {
        if (((Player *)self)->IsState(data_ov002_021101e4)) {
            p->mAngleY = p->mAngleY + 0x8000;
        }
        if (func_ov002_020d674c(self)) {
            ((Player *)self)->ChangeState(data_ov002_0211004c);
        } else {
            ((Player *)self)->ChangeState(data_ov002_021105a4);
        }
        return 1;
    }

    if (!(flags & 1)) goto ret0;

    ptr = p->mHeldObj;
    b1 = !!ptr;
    if (b1 != false) {
        b2 = ((dActor_c *)ptr)->mFlags;
        b2 = ((b2 & 0x200) != 0);
        if (b2 == false) {
            ((Player *)self)->ChangeState(data_ov002_021105d4);
        }
        return 1;
    }

    if (((Player *)self)->IsState(data_ov002_0211019c) && p->mJumpComboStage != 2) {
        if (func_ov002_020dde74(self)) return 1;
        goto ret0;
    }

    if (((Player *)self)->IsState(data_ov002_021101b4) ||
        ((Player *)self)->IsState(data_ov002_02110724) ||
        ((Player *)self)->IsState(data_ov002_021101fc) ||
        ((Player *)self)->IsState(data_ov002_0211052c) ||
        ((Player *)self)->IsState(data_ov002_0211055c) ||
        ((Player *)self)->IsState(data_ov002_02110694))
        return 0;

    if (((Player *)self)->IsState(data_ov002_021101e4)) {
        p->mAngleY = p->mAngleY + 0x8000;
    }
    p->mPrevAngleY = p->mAngleY;

    if (p->mIsMega == 0) {
        if (p->param1 == 3) {
            ((Player *)self)->ChangeState(data_ov002_02110034);
        } else {
            ((Player *)self)->ChangeState(data_ov002_021105bc);
        }
    } else {
        p->mPunchKickStep = 2;
        ((Player *)self)->ChangeState(data_ov002_02110574);
    }

    p->mAngleY = p->mPrevAngleY;
    return 1;

ret0:
    return 0;
}
}

// @symbol func_ov002_020e28d4
extern "C" {
void func_ov002_020e28d4(Player *p, int a, int b) {
    extern int atan2_hold_Player_1(Fix12i a, int b);
    extern int AngleDiff(int a, int b);
    extern void ApproachAngle(s16* cur, s16 target, int divisor, int band, int maxStep);
    extern int Player_ScaleByCharFactor(void *p, int a);
    extern int func_ov002_020bf224(void *p, int a, int b);
    extern s16 data_0209f4a0[];
    extern s16 data_02082214[];
    extern Player::State data_ov002_0211055c;
    int r4;

    if (p->param1 == 1)
        a <<= 1;

    if (*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) != 0) {
        if (p->mClsnFlags & 2) {
            int t = cstd::atan2(p->mWallNormalX, p->mWallNormalZ);
            if (AngleDiff((s16)(t + 0x8000), p->mPrevAngleY) < 0x2000) {
                if (p->mHorzSpeed >= 0x3000)
                    p->mHorzSpeed = 0x3000;
            }
        }
        if (p->IsState(data_ov002_0211055c)) {
            r4 = Player_ScaleByCharFactor(p, 0x3c000);
            s16 e = AngleDiff(p->mDesiredAngleY, p->mAngleY);
            e = e + 0x4000;
            s16 tv = data_02082214[((u16)e >> 4) * 2];
            r4 = FX_MUL(r4, tv);
            if (r4 < 0)
                r4 = -r4;
            if (r4 < 0xa000)
                r4 = 0xa000;
        } else {
            r4 = func_ov002_020bf224(p, Player_ScaleByCharFactor(p, 0x21000),
                                     Player_ScaleByCharFactor(p, 0xa000));
        }

        s16 r7 = p->mDesiredAngleY;
        if (p->mHorzSpeed != 0) {
            if (AngleDiff(p->mPrevAngleY, r7) > 0x6000) {
                r7 = r7 + 0x8000;
                r4 = -r4;
            }
            ApproachAngle(&p->mPrevAngleY, r7, 0x10, 0x100, 0);
        } else {
            p->mPrevAngleY = r7;
        }
    } else {
        r4 = 0;
        a = b;
    }

    ApproachLinear(p->mHorzSpeed, r4, a);

    if (AngleDiff(p->mPrevAngleY, p->mAngleY) < 0x4000)
        return;

    if (p->mHorzSpeed >= 0x10000)
        p->mHorzSpeed -= 0x2000;
}
}

// @symbol func_ov002_020e2ad0
extern "C" {
void func_ov002_020e2ad0(char* p)
{
    Player *player = (Player *)p;
    if (player->mSinkDepth <= 0x1000)
        return;
    if (player->mQuicksandJumpTimer == 0) {
        player->mVertSpeed = FX_MUL(player->mVertSpeed, 0x900);
        player->mHorzSpeed >>= 1;
        player->mJumpedFromQuicksand = 1;
    } else {
        if (player->mSinkDepth < 0xb000)
            player->mSinkDepth = 0xb000;
    }
}
}

// @symbol func_ov002_020e2b6c
extern "C" {
int func_ov002_020e2b6c(Player *player) {
    extern Player::State data_ov002_021104cc;
    if (player->mSinkDepth < 0xb000)
        return 0;
    player->ChangeState(data_ov002_021104cc);
    return 1;
}
}

// @symbol func_ov002_020e2ba8
extern "C" {
int func_ov002_020e2ba8(Player *player) {
    extern Player::State data_ov002_0211019c;
    if (player->mIsMega == 0)
        return 0;
    player->ChangeState(data_ov002_0211019c);
    return 1;
}
}

// @symbol func_ov002_020e2be4
extern "C" {
extern "C" int func_ov002_020e2be4(Player *player) {
    extern Player::State data_ov002_021103f4;
    int thr;
    if (player->func_ov002_020c04ac())
        return 0;
    switch (player->func_ov002_020c031c()) {
    case 4:
    case 5:
        thr = 0xf74;
        break;
    case 3:
        thr = 0xf04;
        break;
    default:
        thr = 0xddb;
        break;
    case 1:
    case 2:
        thr = 0xddb;
        break;
    }
    if (player->mFloorNormalY > thr)
        return 0;
    player->ChangeState(data_ov002_021103f4);
    return 1;
}
}

// @symbol func_ov002_020e2c84
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
int func_ov002_020e2c84(char *self)
{
    Player *player = (Player *)self;
    struct dCamera_c;
    extern int func_ov002_020e3078(char *self, void *s);
    extern int func_ov002_020e2ea0(char *self);
    extern int func_ov002_020d91e0(char *thiz, int damage, int doPre);
    extern void func_0200d8c8(struct dCamera_c * cam, const Vector3 *v, int strength);
    extern void func_ov002_020db8bc(unsigned char *p, unsigned char val);
    extern char data_ov002_02110454;
    extern Player::State data_ov002_02110094;
    extern Player::State data_ov002_0211010c;
    extern struct dCamera_c *data_0209f318;
    extern char data_ov002_02110484;
    extern int func_ov002_020e2c84(char*);
    extern void func_ov002_020e301c(char*);
    int diff;

    diff = player->mPeakY - player->mPosY;
    func_ov002_020e301c(self);

    if (func_ov002_020e3078(self, &data_ov002_02110454) != 0 ||
        func_ov002_020e3078(self, &data_ov002_02110484) != 0 || player->mIsMega != 0)
        return 0;

    if (func_ov002_020e2ea0(self) != 0)
        return 2;

    if (diff > 0xbb8000) {
        player->mStateStep = 2;
        player->mHurtDamage = 0;
        ((Player *)self)->ChangeState(data_ov002_02110094);

        if (func_ov002_020d91e0(self, 0x400, 1) != 0) {
            {
                u8 *p = &player->mStateStep;
                *p &= 1;
            }
            ((Player *)self)->ChangeState(data_ov002_0211010c);
            return 1;
        }

        func_0200d8c8(data_0209f318, (Vector3 *)(&player->mPosX), 0x7d0000);
        player->mIsFallScreaming = 0;
        Sound::PlayCharVoice(player->mCharacter, 7, player->mCamSpacePos);
    } else if (diff > 0x47e000) {
        if (((Player *)self)->func_ov002_020c0cbc() == 0) {
            if (func_ov002_020d91e0(self, 0x200, 1) != 0) {
                if (((Player *)self)->IsState(data_ov002_02110094) != 0) {
                    {
                        u8 *p = &player->mStateStep;
                        *p &= 1;
                    }
                    ((Player *)self)->ChangeState(data_ov002_0211010c);
                } else {
                    ((Player *)self)->func_ov002_020c5dec(2);
                }
                return 1;
            }

            func_ov002_020db8bc((unsigned char *)self, 1);
            player->mVertSpeed = 0;
            func_0200d8c8(data_0209f318, (Vector3 *)(&player->mPosX), 0x7d0000);
            player->mIsFallScreaming = 0;
            Sound::PlayCharVoice(player->mCharacter, 7, player->mCamSpacePos);
        }
    }

    player->mPeakY = player->mPosY;
    return 0;
}
}

// @symbol func_ov002_020e2ea0
extern "C" {
extern "C" int func_ov002_020e2ea0(Player *self) {
    int func_ov002_020e3078(Player *self, Player::State *s);
    extern signed char data_0209f2f8;
    extern Player::State data_ov002_021105bc;
    extern Player::State data_ov002_021105a4;
    extern Player::State data_ov002_021101e4;
    extern Player::State data_ov002_0211040c;
    char *base = (char *)self;
    Player *p = (Player *)base;
    signed char c = data_0209f2f8;

    if (c == 0x13 || c == 0xa || c == 0x10) {
        /* proceed */
    } else if (c != 0x22) {
        goto ret0;
    }

    int val = p->mSurfaceType;
    if (val == 1 || val == 0xe) {
        return 0;
    }

    if (!(&p->mMeshClsn)->IsOnGround()) {
        return 0;
    }

    dBgPi *cr = p->mMeshClsn.GetFloorResult();
    if (cr->GetClsnID() != -1) {
        if (dActor_c::FindWithID((unsigned int)cr->GetClsnID()) != 0) {
            return 0;
        }
    }

    int diff = p->mPeakY - p->mPosY;
    if (diff <= 0x3e8000) {
        goto ret0;
    }
    if (p->mFloorNormalY < 0xddb) {
        goto ret0;
    }

    p->mStateStep = 0;
    if (self->IsState(data_ov002_021105bc)) {
        p->mStateStep = 1;
    } else if (self->IsState(data_ov002_021105a4)) {
        p->mStateStep = 2;
    } else {
        if (func_ov002_020e3078(self, &data_ov002_021101e4)) {
            p->mAngleY = p->mPrevAngleY;
        }
    }

    p->mStateArg = 0;
    if (c == 0x10) {
        p->mStateArg = 1;
    } else if (c == 0x22) {
        p->mStateArg = 2;
    }

    self->ChangeState(data_ov002_0211040c);
    return 1;

ret0:
    return 0;
}
}

// @symbol func_ov002_020e301c
extern "C" {
void func_ov002_020e301c(char* c){
    extern void func_0200d8c8(void* cam, void* v, int strength);
    extern void* data_0209f318;
    Player *player = (Player *)c;
    if (player->param1 != 2)
        return;
    if (player->mPeakY - player->mPosY < 0x190000)
        return;
    func_0200d8c8(data_0209f318, &player->mPos, 0x320000);
}
}

// @symbol func_ov002_020e3078
struct Player3078 { char p[0x400]; };
extern "C" {
int func_ov002_020e3078(struct Player3078* self, void* s) {
    return ((Player *)self)->mPrevState == s;
}
}

// @symbol _ZN6Player7IsStateERNS_5StateE
/* recovered: real C++ method, named members + shared header */
/* Five instructions, and every one of them is in the source now:
     ldr r0, [r0, #0x370]   -> mState
     cmp r0, r1             -> == &state
     moveq r0, #1 / movne r0, #0
     bx lr
   The moveq/movne pair is the compiler normalising to a truth value, which is
   what makes bool the honest return type rather than int -- contrast the
   dBgCh_Actr accessors, which return a raw mask straight out of an `and`.

   The parameter is a reference, so the caller passes an address and this
   compares pointer identity: two distinct State objects with identical
   contents are NOT the same state. That is the whole semantic. */
bool Player::IsState(Player::State &state)
{
    return mState == &state;
}

// @symbol _ZN6Player11ChangeStateERNS_5StateE
/* recovered: named members + shared header, real C++ method,
 * declarations from a shared header
 *
 * The parameter is a REFERENCE -- the ROM name says RNS_5StateE -- so it is
 * taken as one and bound to a local pointer, leaving every expression in the
 * body exactly as it was.
 */
/* recovered: named members + shared header */
extern "C" {
int Player::ChangeState(Player::State &state) {
    extern void func_ov002_020d4540(void *p);
    extern void func_0200d81c(void *thiz, int playerID);
    extern void *data_0209f318;
    extern Player::State data_ov002_0211022c;
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_0211067c;
    extern Player::State data_ov002_021106ac;
    extern Player::State data_ov002_02110364;
    extern void func_ov002_020e6780(char*);
    Player::State *newState = &state;
    mRequestedState = newState;

    {
        Player::State *cur;
        int ok;
        /* The null-checks below stay as raw offset arithmetic on purpose.
           Spelling either as `*(void**)&cur->mCleanup == 0` costs 4 words:
           taking the ADDRESS of a pointer-to-member makes mwcc materialise
           the whole 8-byte pmf first. Reading one to CALL it is free -- the
           calls below use the named members. Measured both ways. */
        cur = mState;
        if (cur == 0) { ok = 1; goto check_ok; }
        if (*(int*)((char*)cur+0x10) == 0) { ok = 1; goto check_ok; }
        ok = (this->*(cur->mCleanup))();
    check_ok:
        if (!ok) return 0;
    }

    if (mState == &data_ov002_0211022c
        && (unsigned short)(mStateFlags & 0x400) == 0
        && mIsNoControl != 0
        && newState != &data_ov002_0211013c
        && newState != &data_ov002_0211067c
        && newState != &data_ov002_021106ac
        && newState != &data_ov002_0211022c)
    {
        return 0;
    }

    {
        int b = (mHeldObj != 0);
        if (b) {
        if (newState == &data_ov002_0211013c) {
            mStateStep = 1;
            newState = &data_ov002_02110364;
        }
        }
    }

    mLoopingSoundHandle = 0;
    mParticle3 = 0;
    mParticle2 = mParticle3;
    mParticle1 = mParticle2;

    mPrevState = mState;
    mState = newState;

    mHideShadow = 0;
    mIsBodyClsnEnabled = 1;
    mSkipGroundUpdate = 0;

    mModelOffsetZ = 0;
    mModelOffsetY = mModelOffsetZ;

    mIsInAirState = 0;
    mTongueHitDist = 0;

    mAngleZ = 0;
    mAngleX = mAngleZ;

    mIsControlDisabled = 0;
    mDetectWater = 0;
    mAttackClashed = 0;

    mStateFlags &= ~0x200;

    {
        int v1 = 0x4000;
        int v2 = 0x4b000;
        v1 = -v1;
        v2 = -v2;
        mVertAccel = v1;
        mTerminalVelocity = v2;
    }

    mStateTimer = 0;

    func_ov002_020d4540(this);

    mIsTakingDamage = 0;
    func_ov002_020c9e18();

    func_ov002_020e6780((char *)this);

    if (data_0209f318 != 0) {
        func_0200d81c(data_0209f318, mPlayerNo);
    }

    {
        Player::State *s = mState;
        if (*(int*)s == 0) return 1;
        return (this->*(s->mInit))();
    }
}
}

// @symbol _ZN6Player16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* _020bdef0, _020bdd9c, _020e032c and func_02073244 come from decl_common.h
   above; redeclaring one here with `char *` where that header says `void *`
   is not a redeclaration but an attempt to overload a C-linkage name, which
   mwccarm rejects outright. func_ov002_020bdd2c is a Player member now and
   comes from Player.h. */
#pragma push
#pragma opt_common_subs off
#pragma opt_strength_reduction off
extern "C" {
int Player::CleanupResources()
{
    enum { false_, true_ };
    struct VB { virtual void v0(); virtual void v1(); };
    void func_0203cbc0(void *p);
    /* local extern: the registrar takes this D1 by address, which C++ cannot spell. */
    void *_ZN7Vector3D1Ev(void *self);
    void func_ov002_020bebd4(char *c);
    void UnloadSilverStarAndNumber(void);
    void UnloadKeyModels(int n);
    extern void *data_ov002_020ff480[];
    extern u8 data_0209f2d8;
    extern char data_ov002_0210d9a0[];
    extern char data_ov002_0210d9a8[];
    extern char data_ov002_0210d9c0[];
    extern char data_ov002_0210da38[];
    extern char data_ov002_0210da40[];
    extern char data_ov002_0210e3d8[];
    extern char data_ov002_0210e750[];
    extern char data_ov002_0210e8d0[];
    extern char data_ov002_0210eb20[];
    extern char data_ov002_0210ebb8[];
    extern char data_ov002_0210ebd8[];
    extern char data_ov002_02110aa4[];
    extern char data_ov002_0210e1c8[];
    extern char data_ov002_0210e1d0[];
    extern char data_ov002_0210e1e0[];
    extern char data_ov002_0210e1e8[];
    extern char data_ov002_0210e230[];
    extern char data_ov002_0210e250[];
    extern char data_ov002_0210e270[];
    extern char data_ov002_0210e288[];
    extern char data_ov002_0210e2f0[];
    extern char data_ov002_0210e3b0[];
    extern char data_ov002_0210e3c8[];
    extern char data_ov002_0210e3e0[];
    extern char data_ov002_0210e3e8[];
    extern char data_ov002_0210e400[];
    extern char data_ov002_0210e408[];
    extern char data_ov002_0210e430[];
    extern char data_ov002_0210e438[];
    extern char data_ov002_0210e450[];
    extern char data_ov002_0210e458[];
    extern char data_ov002_0210e460[];
    extern char data_ov002_0210e478[];
    extern char data_ov002_0210e4b8[];
    extern char data_ov002_0210e4c0[];
    extern char data_ov002_0210e4d0[];
    extern char data_ov002_0210e4e8[];
    extern char data_ov002_0210e4f0[];
    extern char data_ov002_0210e500[];
    extern char data_ov002_0210e538[];
    extern char data_ov002_0210e540[];
    extern char data_ov002_0210e588[];
    extern char data_ov002_0210e600[];
    extern char data_ov002_0210e620[];
    extern char data_ov002_0210e640[];
    extern char data_ov002_0210e660[];
    extern char data_ov002_0210e670[];
    extern char data_ov002_0210e680[];
    extern char data_ov002_0210e690[];
    extern char data_ov002_0210e6a0[];
    extern char data_ov002_0210e6b8[];
    extern char data_ov002_0210e6c0[];
    extern char data_ov002_0210e6e0[];
    extern char data_ov002_0210e6f8[];
    extern char data_ov002_0210e708[];
    extern char data_ov002_0210e728[];
    extern char data_ov002_0210e738[];
    extern char data_ov002_0210e758[];
    extern char data_ov002_0210e770[];
    extern char data_ov002_0210e780[];
    extern char data_ov002_0210e788[];
    extern char data_ov002_0210e790[];
    extern char data_ov002_0210e798[];
    extern char data_ov002_0210e7a8[];
    extern char data_ov002_0210e7b0[];
    extern char data_ov002_0210e7e8[];
    extern char data_ov002_0210e7f0[];
    extern char data_ov002_0210e800[];
    extern char data_ov002_0210e818[];
    extern char data_ov002_0210e898[];
    extern char data_ov002_0210e8e8[];
    extern char data_ov002_0210e910[];
    extern char data_ov002_0210e958[];
    extern char data_ov002_0210e9d0[];
    extern char data_ov002_0210e9e8[];
    extern char data_ov002_0210ea10[];
    extern char data_ov002_0210ea30[];
    extern char data_ov002_0210ea70[];
    extern char data_ov002_0210ea88[];
    extern char data_ov002_0210eac8[];
    extern char data_ov002_0210eaf0[];
    extern char data_ov002_0210eb00[];
    extern char data_ov002_0210eb10[];
    extern char data_ov002_0210eb38[];
    extern char data_ov002_0210eb58[];
    extern char data_ov002_0210eb70[];
    extern char data_ov002_0210eb88[];
    extern char data_ov002_0210eb90[];
    extern char data_ov002_0210eb98[];
    extern char data_ov002_0210eba8[];
    extern char data_ov002_0210ebc8[];
    extern char data_ov002_0210ec00[];
    extern char data_ov002_0210ec10[];
    extern char data_ov002_0210ec38[];
    extern char data_ov002_0210ec40[];
    extern char data_ov002_0210ec50[];
    extern char data_ov002_0210ec60[];
    extern char data_ov002_0210ec98[];
    extern char data_ov002_0210eca8[];
    extern char data_ov002_0210ecb8[];
    extern void func_02073244(void*, int, int, void (*)(void*));
    extern void func_ov002_020e032c(void*);
    int i;
    u32 b;

    func_ov002_020bdd2c();
    func_ov002_020bdef0();
    func_ov002_020bdd9c();
    func_ov002_020e032c(((char *)this));
    for (i = 0; i < 4; i++) {
        int j;
        VB *p = (VB *)mModels.mBodyModels[i];
        if (p != 0) {
            if (p != 0)
                p->v1();
        }
        p = (VB *)mModels.mHeadModels[i];
        if (p != 0) {
            if (p != 0)
                p->v1();
        }
        j = i + 4;
        p = (VB *)mModels.mHeadModels[j];
        if (p != 0) {
            if (p != 0)
                p->v1();
        }
        {
            int q = mModels.mBodyTexParams[i];
            if (q != 0)
                func_0203cbc0((void *)q);
            q = mModels.mHeadTexParams[i];
            if (q != 0)
                func_0203cbc0((void *)q);
            q = mModels.mHeadTexParams[j];
            if (q != 0)
                func_0203cbc0((void *)q);
        }
    }
    {
        VB *p = (VB *)mModels.mBodyModels[4];
        if (p != 0) {
            if (p != 0)
                p->v1();
        }
        p = (VB *)mModels.mSleepBubbleModel;
        if (p != 0) {
            if (p != 0)
                p->v1();
        }
    }
    {
        int q = (int)mTrailPos;
        if (q != 0)
            /* Array destruction discards the receiver result. */
            func_02073244((void *)q, 0xc, 8, (void (*)(void *))_ZN7Vector3D1Ev);
        q = (int)mTrailFlags;
        if (q != 0)
            func_0203cbc0((void *)q);
        q = mHeldObjQueue;
        if (q != 0)
            func_0203cbc0((void *)q);
    }
    ((SharedFilePtr *)(data_ov002_020ff480[mCharFileBase + (param1 & 3)]))->Release();
    b = data_0209f2d8;
    b = b == 1;
    if (b != false_)
        func_ov002_020bebd4(((char *)this));
    ((SharedFilePtr *)(data_ov002_0210ec50))->Release();
    ((SharedFilePtr *)(data_ov002_0210ebb8))->Release();
    ((SharedFilePtr *)(data_ov002_0210ec60))->Release();
    ((SharedFilePtr *)(data_ov002_0210e818))->Release();
    ((SharedFilePtr *)(data_ov002_0210e690))->Release();
    ((SharedFilePtr *)(data_ov002_0210eb20))->Release();
    ((SharedFilePtr *)(data_ov002_0210e758))->Release();
    ((SharedFilePtr *)(data_ov002_0210ec38))->Release();
    ((SharedFilePtr *)(data_ov002_0210eb00))->Release();
    ((SharedFilePtr *)(data_ov002_0210e790))->Release();
    ((SharedFilePtr *)(data_ov002_0210e7a8))->Release();
    ((SharedFilePtr *)(data_ov002_0210e7f0))->Release();
    ((SharedFilePtr *)(data_ov002_0210e798))->Release();
    ((SharedFilePtr *)(data_ov002_0210e4f0))->Release();
    ((SharedFilePtr *)(data_ov002_0210e3d8))->Release();
    ((SharedFilePtr *)(data_ov002_0210eb88))->Release();
    ((SharedFilePtr *)(data_ov002_0210e6c0))->Release();
    ((SharedFilePtr *)(data_ov002_0210eb98))->Release();
    ((SharedFilePtr *)(data_ov002_0210e4c0))->Release();
    ((SharedFilePtr *)(data_ov002_0210e500))->Release();
    ((SharedFilePtr *)(data_ov002_0210e1c8))->Release();
    ((SharedFilePtr *)(data_ov002_0210e770))->Release();
    ((SharedFilePtr *)(data_ov002_0210e230))->Release();
    ((SharedFilePtr *)(data_ov002_0210e288))->Release();
    ((SharedFilePtr *)(data_ov002_0210e478))->Release();
    ((SharedFilePtr *)(data_ov002_0210e9d0))->Release();
    ((SharedFilePtr *)(data_ov002_0210ea10))->Release();
    ((SharedFilePtr *)(data_ov002_0210e538))->Release();
    ((SharedFilePtr *)(data_ov002_0210eac8))->Release();
    ((SharedFilePtr *)(data_ov002_0210e898))->Release();
    ((SharedFilePtr *)(data_ov002_0210e9e8))->Release();
    ((SharedFilePtr *)(data_ov002_0210eba8))->Release();
    ((SharedFilePtr *)(data_ov002_0210eb70))->Release();
    ((SharedFilePtr *)(data_ov002_0210eaf0))->Release();
    ((SharedFilePtr *)(data_ov002_0210e640))->Release();
    ((SharedFilePtr *)(data_ov002_0210e670))->Release();
    ((SharedFilePtr *)(data_ov002_0210e680))->Release();
    ((SharedFilePtr *)(data_ov002_0210e438))->Release();
    ((SharedFilePtr *)(data_ov002_0210e3b0))->Release();
    ((SharedFilePtr *)(data_ov002_0210e3c8))->Release();
    ((SharedFilePtr *)(data_ov002_0210e408))->Release();
    ((SharedFilePtr *)(data_ov002_0210e400))->Release();
    ((SharedFilePtr *)(data_ov002_0210ecb8))->Release();
    ((SharedFilePtr *)(data_ov002_0210e8d0))->Release();
    ((SharedFilePtr *)(data_ov002_0210ebd8))->Release();
    b = data_0209f2d8;
    b = b == 1;
    if (b == false_) {
        ((SharedFilePtr *)(data_ov002_0210d9a8))->Release();
        ((SharedFilePtr *)(data_ov002_02110aa4))->Release();
        ((SharedFilePtr *)(data_ov002_0210da38))->Release();
        ((SharedFilePtr *)(data_ov002_0210e750))->Release();
        ((SharedFilePtr *)(data_ov002_0210e588))->Release();
        ((SharedFilePtr *)(data_ov002_0210e7e8))->Release();
        ((SharedFilePtr *)(data_ov002_0210e450))->Release();
        ((SharedFilePtr *)(data_ov002_0210e788))->Release();
        ((SharedFilePtr *)(data_ov002_0210e1e8))->Release();
        ((SharedFilePtr *)(data_ov002_0210e1e0))->Release();
        ((SharedFilePtr *)(data_ov002_0210e4d0))->Release();
        ((SharedFilePtr *)(data_ov002_0210e4e8))->Release();
        ((SharedFilePtr *)(data_ov002_0210e1d0))->Release();
        ((SharedFilePtr *)(data_ov002_0210e800))->Release();
        ((SharedFilePtr *)(data_ov002_0210e4b8))->Release();
        ((SharedFilePtr *)(data_ov002_0210e270))->Release();
        ((SharedFilePtr *)(data_ov002_0210e458))->Release();
        ((SharedFilePtr *)(data_ov002_0210ea70))->Release();
        ((SharedFilePtr *)(data_ov002_0210e540))->Release();
        ((SharedFilePtr *)(data_ov002_0210e8e8))->Release();
        ((SharedFilePtr *)(data_ov002_0210eca8))->Release();
        ((SharedFilePtr *)(data_ov002_0210ec98))->Release();
        ((SharedFilePtr *)(data_ov002_0210e600))->Release();
        ((SharedFilePtr *)(data_ov002_0210ea88))->Release();
        ((SharedFilePtr *)(data_ov002_0210e910))->Release();
        ((SharedFilePtr *)(data_ov002_0210e460))->Release();
        ((SharedFilePtr *)(data_ov002_0210e2f0))->Release();
        ((SharedFilePtr *)(data_ov002_0210e958))->Release();
        ((SharedFilePtr *)(data_ov002_0210eb38))->Release();
        ((SharedFilePtr *)(data_ov002_0210e660))->Release();
        ((SharedFilePtr *)(data_ov002_0210ea30))->Release();
        ((SharedFilePtr *)(data_ov002_0210e3e8))->Release();
        ((SharedFilePtr *)(data_ov002_0210eb90))->Release();
        ((SharedFilePtr *)(data_ov002_0210e6b8))->Release();
        ((SharedFilePtr *)(data_ov002_0210e620))->Release();
        ((SharedFilePtr *)(data_ov002_0210e6a0))->Release();
        ((SharedFilePtr *)(data_ov002_0210eb58))->Release();
        ((SharedFilePtr *)(data_ov002_0210e3e0))->Release();
        ((SharedFilePtr *)(data_ov002_0210eb10))->Release();
        ((SharedFilePtr *)(data_ov002_0210e708))->Release();
        ((SharedFilePtr *)(data_ov002_0210ebc8))->Release();
        ((SharedFilePtr *)(data_ov002_0210e6e0))->Release();
        ((SharedFilePtr *)(data_ov002_0210e430))->Release();
        ((SharedFilePtr *)(data_ov002_0210ec40))->Release();
        ((SharedFilePtr *)(data_ov002_0210e738))->Release();
        ((SharedFilePtr *)(data_ov002_0210e250))->Release();
        ((SharedFilePtr *)(data_ov002_0210ec10))->Release();
        ((SharedFilePtr *)(data_ov002_0210ec00))->Release();
        ((SharedFilePtr *)(data_ov002_0210e728))->Release();
    }
    if (mLoadedResourceFlags & 1)
        UnloadSilverStarAndNumber();
    if (mLoadedResourceFlags & 2)
        ((SharedFilePtr *)(data_ov002_0210da40))->Release();
    if (mLoadedResourceFlags & 4)
        ((SharedFilePtr *)(data_ov002_0210d9a0))->Release();
    if (mLoadedResourceFlags & 8)
        ((SharedFilePtr *)(data_ov002_0210d9c0))->Release();
    if (mLoadedResourceFlags & 0x10)
        UnloadKeyModels(mKeyModelId);
    b = data_0209f2d8;
    b = b == 1;
    if (b == false_) {
        if (mLoadedResourceFlags & 0x20) {
            ((SharedFilePtr *)(data_ov002_0210e6f8))->Release();
            ((SharedFilePtr *)(data_ov002_0210e7b0))->Release();
            ((SharedFilePtr *)(data_ov002_0210e780))->Release();
        }
    } else {
        ((SharedFilePtr *)(data_ov002_0210e7b0))->Release();
    }
    if (mLoadedResourceFlags & 0x40) {
        u32 idx = mHatCharacter;
        if (idx == (u32)param1)
            idx = mPrevCharacter;
        ((SharedFilePtr *)(data_ov002_020ff480[mCharFileBase + idx]))->Release();
    }
    return 1;
}
}
#pragma pop

// @symbol _ZN6Player16OnPendingDestroyEv
/* recovered: named members + shared header, real C++ method */
/* Player::OnPendingDestroy() at 0x020e3a04 (ov002) -- vtable slot 12.
 * Empty override; Player does nothing when marked for destruction.
 */
void Player::OnPendingDestroy()
{
    (void)this;
}

// @symbol _ZN6Player6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
#pragma push
#pragma opt_common_subs off
#define AT(p, off) ((void*)(int)((char*)(p) + (off)))
extern "C" {
int Player::Render()
{
    struct VObj {
        virtual void d00();
        virtual void d04();
        virtual void d08();
        virtual void d0c();
        virtual void d10();
        virtual void m14(void* a);
        virtual void m18(void* a, void* b);
    };
    typedef struct M34 { int m[12]; } M34;
    extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
    extern void Matrix4x3_ApplyInPlaceToRotationZ(void* m, int ang);
    extern void MulMat4x3Mat4x3(void* a, void* b, void* c);
    extern int func_ov002_020d225c(char* self);
    extern u8 data_0209f2d8;
    extern u8 data_0209fc5c[];
    extern Player::State data_ov002_021100c4;
    extern int data_020a0e68;
    extern void func_ov002_020e3e00(void*, void*, u32);
    int t = (data_0209f2d8 == 1);
    if (t != false) {
        if (data_0209fc5c[mPlayerNo] == 0)
            return 1;
    }
    if (this->IsState(data_ov002_021100c4)) {
        if (mInvincibleTimer & 2)
            return 1;
    } else {
        if (mInvincibleTimer & 1)
            return 1;
    }
    if (mOpacity == 0)
        return 1;
    if (!(mFlags & 0x10)) {
        dExtAnm2Model_c *bodyMdl;
        int i;
        bodyMdl = mModels.mBodyModels[this->GetBodyModelID(mBodyModelId, 1)];
        if (mIsBalloon == 0) {
            bodyMdl->ApplyOpacity(mOpacity, 0);
            {
                int t2 = (data_0209f2d8 == 1);
                if (t2 != false) {
                    if (param1 == 3) {
                        int* p = (int*)AT(bodyMdl, 8);
                        BMD_File *hdr = (BMD_File *)p[0];
                        char* rec = (char*)p[1];
                        u32 i2;
                        for (i2 = 0; i2 < hdr->numMaterials; i2++) {
                            *(int *)(rec + 0x20) = mVsYoshiMaterialParam;
                            rec += 0x30;
                        }
                    }
                }
            }
            bodyMdl->dExtSimpleModel_c::Render(&mScale);
            if (param1 == 1) {
                mModels.mTexSeqPlayer[0].Update(mModels.mBodyModels[1]->data);
                mModels.mTexSeqPlayer[0].currFrame = mPlayerTexFrame << 12;
            }
            func_ov002_020e3e00(bodyMdl, &mScale, mOpacity);
            if (mDrawnHasWings != 0) {
                mModels.mAnmModel2.ApplyOpacity(mOpacity, 0);
                if (mBodyModelId == 3) {
                    Matrix4x3_FromTranslation(&data_020a0e68, -0x1b33, -0x666, 0);
                    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, 0x4000);
                    MulMat4x3Mat4x3(&data_020a0e68, &bodyMdl->data.transforms[8], &data_020a0e68);
                    ((VObj *)((char *)&mModels.mAnmModel2))->m18(&data_020a0e68, &mScale);
                } else {
                    ((VObj *)((char *)&mModels.mAnmModel2))
                        ->m18(&bodyMdl->data.transforms[15], &mScale);
                }
            }
        } else {
            mModels.mAnmModel1.ApplyOpacity(mOpacity, 0);
            ((VObj *)((char *)&mModels.mAnmModel1))->m14((char *)&mScaleX);
        }
        i = func_ov002_020becf4(mBodyModelId, 1);
        {
            dExtSimpleModel_c *mdl4 = mModels.mHeadModels[i];
            if (mdl4 != 0 && i != 9 && i != 8) {
                mdl4->ApplyOpacity(mOpacity, 0);
                {
                    int t3 = (data_0209f2d8 == 1);
                    if (t3 != false) {
                        if (param1 == 3) {
                            int *p = (int *)AT((char *)mModels.mHeadModels[i], 8);
                            BMD_File *hdr = (BMD_File *)p[0];
                            char* rec = (char*)p[1];
                            u32 i2;
                            for (i2 = 0; i2 < hdr->numMaterials; i2++) {
                                *(int *)(rec + 0x20) = mVsYoshiMaterialParam;
                                rec += 0x30;
                            }
                        }
                    }
                }
                if (i == 3) {
                    mModels.mHeadModels[i]->dExtSimpleModel_c::Render(&mScale);
                } else {
                    Matrix4x3 *dst = mModels.mHeadModels[i]->data.transforms;
                    Matrix4x3 *src = &bodyMdl->data.transforms[15];
                    *(M34*)dst = *(M34*)src;
                    ((VObj *)(char *)mModels.mHeadModels[i])->m14((char *)&mScaleX);
                }
                if (param1 == 1) {
                    mModels.mTexSeqPlayer[1].Update(mModels.mHeadModels[1]->data);
                    mModels.mTexSeqPlayer[1].currFrame = mPlayerTexFrame << 12;
                }
                if (func_ov002_020bea7c() == 0) {
                    mModels.mTexSeqBody[mBodyModelId].Update(mModels.mHeadModels[i]->data);
                }
                func_ov002_020e3e00((char *)mModels.mHeadModels[i], &mScale, mOpacity);
                if (func_ov002_020d225c(((char*)this))) {
                    ((VObj *)mModels.mSleepBubbleModel)->m14((char *)&mSleepBubbleScaleX);
                }
            }
        }
    }
    return 1;
}
}
#undef AT
#pragma pop

// @symbol func_ov002_020e3e00
/* recovered: shared common types */
//
/* decl_common.h is deliberately NOT included. This file declares its callees
   concretely -- `const int *`, `unsigned int`, `dExtSimpleModel_c *` -- while the header
   declares the same symbols with generic `void*`/`int`. Under `extern "C"`
   those are not compatible redeclarations but illegal overloads, so the file
   did not compile AT ALL (errors on the MulMat3x3Mat3x3, func_02016acc and
   func_ov002_020e3e00 lines) for as long as the include was here -- which is
   why no gate ever noticed: an uncompilable file is dropped by all of them.
   Only two declarations were ever needed from the header; they are restated
   below under their real names. */
/* Were spelled `func_02111b64` / `func_02111b6c` -- invented names that
       exist in no symbols.txt anywhere in config/. ov002's own relocs record
       both sites as `kind:load` to an address claimed by 17 and 13 modules
       respectively (config/arm9/overlays/ov002/relocs.txt:6676-6677). A `load`
       cannot answer a function, which rules out ov012/ov025/ov045/ov052; of
       the data claimants that remain, ov055 alone declares BOTH addresses
       unambiguous `kind:bss`. ov017 and ov033 are clean at 0x02111b6c but
       ambiguous at 0x02111b64; every other candidate is ambiguous at both. */
extern "C" {
extern "C" void func_ov002_020e3e00(dExtSimpleModel_c *obj, Vector3 *vec, unsigned int opacity) {
    typedef struct { int m[12]; } Mat;
    extern signed char data_0209f2f8;
    extern signed char data_02092120;
    extern int data_020a0e68[12];
    extern int data_ov055_02111b64;
    extern int data_ov055_02111b6c;
    void func_0203c178(int *dst, int a, int b, int c);
    void MulMat3x3Mat3x3(int *dst, const int *a, const int *b);
    void func_02016acc(void *obj, unsigned int mask);
    void func_02016b24(void *obj, unsigned int mask);
    int ip = (data_0209f2f8 == 0x2f) ? 1 : 0;

    if (!ip) {
        if (data_0209f2f8 != 5) return;
        if (data_02092120 != 3) return;
    }

    if (ip) {
        int ok;
        if (data_ov055_02111b6c == 0 || (data_ov055_02111b64 & 0x20000) != 0)
            ok = 1;
        else
            ok = 0;
        if (!ok) return;
    }

    Mat tmp;
    Mat *src = (Mat *)&obj->mat4x3;
    tmp = *src;

    int r0;
    if (ip)
        r0 = 0;
    else
        r0 = 0x1086000;
    int t = r0 >> 3;
    t <<= 1;
    int diff = t - src->m[9];
    src->m[9] = diff;

    func_0203c178(data_020a0e68, -0x1000, 0x1000, 0x1000);
    MulMat3x3Mat3x3((int*)src, data_020a0e68, (int*)src);

    func_02016acc(obj, 0x80);
    func_02016b24(obj, 0x40);
    obj->dExtSimpleModel_c::Render(vec);
    obj->ApplyOpacity(opacity, 0);
    func_02016b24(obj, 0x80);
    func_02016acc(obj, 0x40);

    *src = tmp;
}
}

// @symbol func_ov002_020e3f90
extern "C" {
int func_ov002_020e3f90(char* self) {
    Player *player = (Player *)self;
    struct M48 { int w[12]; };
    extern void ApproachAngle(short* v, int target, int a, int b, int c);
    extern void Matrix4x3_ApplyInPlaceToTranslation(struct M48* m, int x, int y, int z);
    extern void Matrix4x3_ApplyInPlaceToRotationX(struct M48* m, int a);
    extern void Matrix4x3_ApplyInPlaceToRotationY(struct M48* m, int a);
    extern void Matrix4x3_ApplyInPlaceToRotationZ(struct M48* m, int a);
    extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(struct M48* m, int x, int y, int z);
    extern struct M48 data_020a0e68;
    struct S { short angs[3]; volatile int v[3]; int tgt[3]; } s;
    int b;
    int t;
    int t1;
    int t2;
    Player *p;
    unsigned int n;
    char* m;

    if ((char *)player->mGrabbedByActor == 0)
        return 0;
    if (((dActor_c *)player->mGrabbedByActor)->mCarryMatrix == 0)
        return 0;

    b = 0;
    s.v[0] = 0;
    s.v[1] = 0;
    s.v[2] = 0;
    data_020a0e68 =
        *(struct M48 *)*(char *volatile *)&((dActor_c *)*(char *volatile *)&player->mGrabbedByActor)
             ->mCarryMatrix;
    p = (Player *)player->mGrabbedByActor;
    if (((dActor_c *)p)->actorID == 0xbf)
        b = 1;
    if (b != 0) {
        t = (unsigned int)(p->mHeldObj != 0);
        if (t == 0) return 0;
        t = (unsigned int)(((int)player->mFlags & 0x4000) != 0);
        if (t == 0) return 0;
        if ((unsigned int)(((int)p->mCharFileBase >> 2) - 0x6c) <= 1) {
            s.tgt[0] = -0x3000;
            s.tgt[1] = -0x1000;
            s.tgt[2] = -0x4000;
            s.angs[0] = -0x2000;
            s.angs[1] = -0x1000;
            s.angs[2] = -0x1000;
        } else {
            s.tgt[0] = 0;
            s.tgt[1] = -0x3000;
            s.tgt[2] = -0x4000;
            s.angs[0] = 0x1000;
            s.angs[1] = 0x7000;
            s.angs[2] = 0;
        }
        ApproachLinear(((Player *)self)->mStateTargetX, s.tgt[0], 0x400);
        ApproachLinear(((Player *)self)->mStateTargetY, s.tgt[1], 0x400);
        ApproachLinear(((Player *)self)->mStateTargetZ, s.tgt[2], 0x400);
        ApproachAngle(&player->mAngleX, s.angs[0], 0x10, 0x1000, 0x100);
        ApproachAngle(&player->mAngleY, s.angs[1], 0x10, 0x1000, 0x100);
        ApproachAngle(&player->mAngleZ, s.angs[2], 0x10, 0x1000, 0x100);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, ((Player *)self)->mStateTargetX,
                                            ((Player *)self)->mStateTargetY,
                                            ((Player *)self)->mStateTargetZ);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, player->mAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, player->mAngleX);
        Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, player->mAngleZ);
    } else {
        t = (unsigned int)(((dActor_c *)p)->actorID == 0xbd);
        if (t != 0) {
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0x5800, -0x6800, -0x2000);
            Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, 0x3c71, 0x38e, -0x4e38);
        } else {
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0x5800, 0x5000, 0xb000);
            Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, -0x600, -0x2600, 0);
        }
    }
    *(struct M48*)(self + 0x5bc) = data_020a0e68;
    t = ((volatile struct M48*)&data_020a0e68)->w[9];
    t1 = ((volatile struct M48*)&data_020a0e68)->w[10];
    t2 = ((volatile struct M48*)&data_020a0e68)->w[11];
    s.v[1] = t1;
    s.v[2] = t2;
    s.v[0] = t;
    player->mPosX = t << 3;
    player->mPosY = s.v[1] << 3;
    player->mPosZ = s.v[2] << 3;
    player->mPrevAngleY = ((dActor_c *)player->mGrabbedByActor)->mAngleY;
    n = ((Player *)self)->GetBodyModelID(player->param1 & 0xff, 0);
    m = (char *)player->mModels.mBodyModels[n];
    *(struct M48 *)&((dExtSimpleModel_c *)m)->mat4x3 = data_020a0e68;
    *(struct M48 *)&player->mModels.mAnmModel2.mat4x3 = data_020a0e68;
    n = ((Player *)self)->func_ov002_020becf4(player->param1 & 0xff, 0);
    if (n != 9 && n != 8) {
        m = (char *)player->mModels.mHeadModels[n];
        if (m != 0) {
            *(struct M48 *)&((dExtSimpleModel_c *)m)->mat4x3 = data_020a0e68;
        }
    }
    return 1;
}
}

// @symbol func_ov002_020e4374
extern "C" {
void func_ov002_020e4374(char* c, int* p1, int* p2)
{
    Player *p = (Player *)c;
    int v;
    int m;
    *p1 = (p->mModelOffsetY + (p->mPosY - p->mSinkDepth) + 0xf000) - p->mGroundY;
    if (p->mIsBalloon != 0)
        *p1 -= 0x70000;
    if (*p1 <= 0x1000) *p1 = 0x1000;
    m = 0x64000;
    if (p->mIsMega != 0) {
        m = p->mScaleY * 0x64;
    } else if (p->mIsBalloon != 0) {
        m = 0xc8000;
    }
    v = FX_MUL(*p1, 0x180);
    *p2 = m - v;
    if (*p2 < 0xa000) *p2 = 0xa000;
    *p1 += 0x28000;
}
}

// @symbol func_ov002_020e444c
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
void func_ov002_020e444c(char *c)
{
    extern void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
    extern void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
    extern void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3 *mF, s16 angY);
    extern void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3 *mF, s16 angX);
    extern void Matrix4x3_ApplyInPlaceToRotationZ(struct Matrix4x3 *mF, s16 angZ);
    extern int func_ov002_020d225c(void *o);
    extern void Matrix4x3_ApplyInPlaceToTranslation(struct Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z);
    extern void func_ov002_020e4374(void *c, int *p1, int *p2);
    extern struct Matrix4x3 data_020a0e68;
    extern int func_ov002_020e3f90(char*);
    Player *player = (Player *)c;
    int sp8, spC;
    int b;

    if (func_ov002_020e3f90(c) == 0) {
        int y = player->mModelOffsetY + (player->mPosY - player->mSinkDepth) + 0xf000;
        if (player->mIsBalloon != 0)
            y -= 0x70000;
        Matrix4x3_FromTranslation(&data_020a0e68, player->mPosX >> 3, y >> 3, player->mPosZ >> 3);
        *(struct Matrix4x3 *)(c + 0x5bc) = data_020a0e68;

        {
            int out[3];
            int src[3];
            int offsetZ = player->mModelOffsetZ;
            src[1] = player->mModelOffsetY - player->mSinkDepth;
            src[2] = offsetZ;
            src[0] = 0;
            Vec3_RotateYAndTranslate(out, &player->mPosX, player->mAngleY, src);
            Matrix4x3_FromTranslation(&data_020a0e68, out[0] >> 3, out[1] >> 3, out[2] >> 3);
        }

        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, player->mAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, player->mAngleX);
        Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, player->mAngleZ);

        if (player->mIsBalloon == 0) {
            int bodyID = ((Player *)c)->GetBodyModelID(player->param1 & 0xff, 0);
            player->mModels.mBodyModels[bodyID]->mat4x3 = data_020a0e68;
            player->mModels.mAnmModel2.mat4x3 = data_020a0e68;
        } else {
            player->mModels.mAnmModel1.mat4x3 = data_020a0e68;
        }

        {
            int r0 = ((Player *)c)->func_ov002_020becf4(player->param1 & 0xff, 0);
            if (r0 != 9 && r0 != 8) {
                dExtSimpleModel_c *q = player->mModels.mHeadModels[r0];
                if (q != 0) {
                    q->mat4x3 = data_020a0e68;
                }
            }
        }

        if (func_ov002_020d225c(c) != 0) {
            int z = FX_MUL(player->mSleepBubbleScaleZ, 0x2400) + 0xc000;
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0x7000, z);
            player->mModels.mSleepBubbleModel->mat4x3 = data_020a0e68;
        }
    }

    func_ov002_020e4374(c, &sp8, &spC);

    if ((player->mOpacity | player->mIsVanish) == 0)
        player->mHideShadow = 1;

    if (player->mMeshClsn.IsOnGround() != 0) {
        dBgPi *fr = player->mMeshClsn.GetFloorResult();
        if (fr->GetClsnID() != -1) {
            void *a = dActor_c::FindWithID(fr->GetClsnID());
            if (a != 0) {
                b = (int)(((dActor_c *)a)->actorID == 0xa1);
                if (b != 0) {
                    sp8 += 0x3c000;
                }
            }
        }
    }

    if (player->mHideShadow != 0)
        return;

    ((dActor_c *)c)
        ->DropShadowRadHeight(player->mShadowModel, *(Matrix4x3 *)(c + 0x5bc), spC, sp8, 0xf);
}
}

// @symbol func_ov002_020e4768
// local extern: include-free shard, the call passes the object as an untyped pointer so it cannot use the header member
extern "C" {
extern "C" void func_ov002_020e4768(char* self)
{
    Player *player = (Player *)self;
    struct VTable4 {
        virtual void v0();
        virtual void v1();
        virtual void v2();
        virtual void v3();
        virtual void DoThing(int arg);
    };
    typedef struct { int w[12]; } Mat43;
    extern void func_020167a4(void* p);
    extern void func_ov002_020e640c(void* self);
    extern void MulMat4x3Mat4x3(void* dst, void* a, void* b);
    extern int data_020a0e68[12];
    unsigned int mid;
    dExtAnm2Model_c *comp;
    int idx;

    mid = ((Player *)self)->GetBodyModelID(player->param1 & 0xff, 0);
    comp = player->mModels.mBodyModels[mid];
    func_020167a4(comp);

    func_ov002_020e640c(self);

    mid = ((Player *)self)->GetBodyModelID(player->param1 & 0xff, 0);
    comp = player->mModels.mBodyModels[mid];
    comp->data.UpdateVertsUsingBones();

    idx = ((Player *)self)->func_ov002_020becf4(player->param1 & 0xff, 0);

    if (idx == 3) {
        unsigned int m2 = ((Player *)self)->GetBodyModelID(player->param1 & 0xff, 0);
        int a1 = (int)&player->mModels.mBodyModels[m2]->data.transforms[15];
        a1 = idx ? a1 : a1;
        ((VTable4 **)player->mModels.mHeadModels)[idx]->DoThing(a1);
    }

    {
        int hasFlag = (player->mHeldObj != 0);
        if (hasFlag) {
            mid = ((Player *)self)->GetBodyModelID(player->param1 & 0xff, 0);
            comp = player->mModels.mBodyModels[mid];
            *(Mat43 *)data_020a0e68 = *(Mat43 *)&comp->mat4x3;

            mid = ((Player *)self)->GetBodyModelID(player->param1 & 0xff, 0);
            comp = player->mModels.mBodyModels[mid];
            MulMat4x3Mat4x3(&comp->data.transforms[14], data_020a0e68, data_020a0e68);

            *(Mat43 *)player->pad_58c = *(Mat43 *)data_020a0e68;
            player->mCarryMatrix = (Matrix4x3 *)player->pad_58c;
        }
    }

    mid = ((Player *)self)->GetBodyModelID(player->param1 & 0xff, 0);
    comp = player->mModels.mBodyModels[mid];
    *(Mat43 *)data_020a0e68 = *(Mat43 *)&comp->mat4x3;

    mid = ((Player *)self)->GetBodyModelID(player->param1 & 0xff, 0);
    comp = player->mModels.mBodyModels[mid];
    MulMat4x3Mat4x3(&comp->data.transforms[15], data_020a0e68, data_020a0e68);

    *(Mat43*)(self + 0x5ec) = *(Mat43*)data_020a0e68;
}
}

// @symbol func_ov002_020e496c
extern "C" {
int func_ov002_020e496c(Player *c) {
    Player *player = (Player *)c;
    int id = ((Player *)c)->GetBodyModelID(player->mBodyModelId, 1);
    return ((int *)player->mModels.mBodyModels)[id];
}
}

// @symbol func_ov002_020e4990
extern "C" {
int func_ov002_020e4990(Player *player) {
    extern void func_0201628c(int a, int b);
    extern void func_020162a4(int a, int b);
    int id;
    int b;
    id = player->GetBodyModelID(player->param1 & 0xff, 0);
    func_0201628c(((int *)player->mModels.mBodyModels)[id], 8);
    b = player->mHeldObj ? 1 : 0;
    if (b == 0)
        return 0;
    if (player->IsAnim(0x30) || player->IsAnim(0x2f) || player->IsAnim(0x33) ||
        player->IsAnim(0x18) || player->IsAnim(0x8a) || player->IsAnim(0x87) ||
        player->IsAnim(0x8b) || player->IsAnim(0x6d) || player->IsAnim(0x6f) ||
        player->IsAnim(0x6c) || player->IsAnim(0x91) || player->IsAnim(0x8e) ||
        player->IsAnim(0x90) || player->IsAnim(0x8f) || player->IsAnim(0x43) ||
        player->IsAnim(0x5b) || player->IsAnim(0x58) || player->IsAnim(0x81) ||
        player->IsAnim(0x80) || player->IsAnim(0xaf) || player->IsAnim(0xb0))
        return 0;
    id = player->GetBodyModelID(player->param1 & 0xff, 0);
    func_020162a4(((int *)player->mModels.mBodyModels)[id], 8);
    return 1;
}
}

// @symbol func_ov002_020e4bb8
extern "C" {
void func_ov002_020e4bb8(char* self)
{
    extern void func_ov002_020e444c(char* c);
    extern int func_ov002_020e4990(char* c);
    extern void func_ov002_020e4768(char* c);
    extern SharedFilePtr *data_ov002_020ff480[];
    Player *player = (Player *)self;
    u8 save_6fd;
    u8 save_6f9;
    u8 save_6fb;
    u8 save_6ff;

    ((Player *)self)->func_ov002_020be3b0();

    save_6fd = player->mIsBalloon;
    save_6f9 = player->mIsMetal;
    save_6fb = player->mIsVanish;
    save_6ff = player->mHasWings;

    player->mPowerupBlinkOn = 0;
    if (player->mPowerupTimer < 0x3f) {
        player->mPowerupBlinkOn = (u8)(player->mPowerupTimer & 1);
        if (player->mPowerupBlinkOn == 0) {
            if (player->mHasWings == 1 || player->mIsVanish != 0) {
                player->mIsVanish = 0;
                player->mHasWings = player->mIsVanish;
            }
            if (player->mIsMetal != 0) {
                u32 v;
                u32 id;
                dExtAnm2Model_c *dst;
                dExtAnm2Model_c *src;
                SharedFilePtr *poolEntry;
                char* p;

                player->mIsMetal = 0;
                v = player->param1;
                id = ((Player *)self)->GetBodyModelID((u8)v, 0);
                dst = player->mModels.mBodyModels[id];
                src = player->mModels.mBodyModels[4];
                poolEntry = data_ov002_020ff480[player->mCharFileBase + v];
                p = (char *)poolEntry->filePtr;
                dst->Copy(*src, p, 0);
            }
        }
    }

    if (player->mBalloonTimer < 0x3f) {
        if (player->mIsBalloon != 0) {
            player->mIsBalloon = (u8)(player->mBalloonTimer & 1);
            player->mHideShadow = 0;
            if (player->mIsBalloon == 0) {
                player->mOpacity = 0;
            } else {
                player->mOpacity = 0x1f;
            }
        }
    }

    func_ov002_020e444c(self);
    func_ov002_020e4990(self);
    func_ov002_020e4768(self);

    player->mBodyModelId = (u8)player->param1;
    player->mDrawnIsBalloon = player->mIsBalloon;
    player->mDrawnIsMetal = player->mIsMetal;
    player->mPlayerTexFrame = player->mIsVanish;
    player->mDrawnHasWings = player->mHasWings;
    player->mDrawnHasNoCap = (u8)((Player *)self)->HasNoCap();

    player->mIsBalloon = save_6fd;
    player->mIsMetal = save_6f9;
    player->mIsVanish = save_6fb;
    player->mHasWings = save_6ff;
}
}

// @symbol _ZN6Player8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* local extern: the call passes an untyped this, so it cannot use the header method. */
#define LAU(p) (p)
extern "C" {
int Player::Behavior()
{
    struct C3;
    typedef int (C3::*PMF)();
    struct G_ee90 {
        char pad[0x26c];
        s32 flag26c;
    };
    extern u16 DecIfAbove0_Short(u16 *p);
    extern s16 GetAngleToCamera(u8 playerID);
    extern void func_02035684(void *p, s32 v);
    extern void func_0203568c(void *p, s32 v);
    extern void func_ov002_020ce8bc(void *self, s32 arg);
    extern void func_ov002_020d6790(void *self);
    extern void func_ov002_020d80d0(void *self);
    extern u8 data_0209f250;
    extern s8 data_0209f284;
    extern u8 data_0209f2d8;
    extern s8 data_0209f2f8;
    extern s32 gRunningKuppaScript;
    extern u8 data_0209fc5c[];
    extern s32 data_0209fc68;
    extern G_ee90 data_0209ee90;
    extern Player::State data_ov002_0210ffec;
    extern Player::State data_ov002_021101b4;
    extern Player::State data_ov002_0211031c;
    extern Player::State data_ov002_02110334;
    extern Player::State data_ov002_02110454;
    extern Player::State data_ov002_021104b4;
    extern Player::State data_ov002_021104e4;
    extern Player::State data_ov002_021104fc;
    extern Player::State data_ov002_02110514;
    extern Player::State data_ov002_021105bc;
    extern Player::State data_ov002_0211061c;
    extern Player::State data_ov002_02110634;
    extern Player::State data_ov002_021106f4;
    extern u8 data_0209f254;
    extern s16 data_0209f4a6;
    extern void func_ov002_020ca940(void*);
    extern void func_ov002_020d8158(void*);
    extern int func_ov002_020d869c(void*);
    extern void func_ov002_020db704(void*);
    extern void func_ov002_020e032c(void*);
    extern void func_ov002_020e4bb8(void*);
    s32 r0;
    s32 scale;
    s32 r2;
    s32 temp;
    s32 mul;
    s32 fallFlag;
    volatile Vector3 v0;
    Vector3 v1;

    if (data_0209f2d8 == 1)
        r0 = 1;
    else
        r0 = 0;
    if (r0 != 0 && data_0209fc5c[mPlayerNo] == 0)
        return 1;

    scale = 0x1000;
    r2 = scale;
    if (mIsMega != 0) {
        scale = 0x3000;
        r2 = 0x4200;
    } else if (mIsBalloon != 0) {
        scale = 0x1400;
        r2 = 0x2000;
    }
    temp = FX_MUL(r2, 0x32000);
    func_0203568c(&mMeshClsn, temp);
    func_02035684(&mMeshClsn, temp);
    mdCcAcPos_c.radius = scale * 0x28;
    mul = 0x96;
    if (IsState(data_ov002_021104e4)
        || IsState(data_ov002_02110514)
        || IsState(data_ov002_02110334)
        || IsState(data_ov002_0211031c)
        || IsState(data_ov002_021105bc)
        || IsState(data_ov002_0211061c)
        || IsState(data_ov002_02110634)) {
        mul = 0x5a;
    }
    mdCcAcPos_c.height = mul * scale;

    if (mIsMega != 0)
        *(u32 *)LAU((char *)&mdCcAcPos_c.flags) |= 0x10;
    else
        *(u32 *)LAU((char *)&mdCcAcPos_c.flags) &= ~0x10;

    if (data_0209fc68 == 0) {
        if (gRunningKuppaScript != 0)
            r0 = 1;
        else
            r0 = 0;
        if (r0 == 0) {
            gActivePlayerSlot = 0;
            goto after_player_slot;
        }
    }
    gActivePlayerSlot = mPlayerNo;
after_player_slot:

    if (func_ov002_020c4188() != 0)
        return 1;

    DecIfAbove0_Short(&mStateTimer);
    DecIfAbove0_Short(&mStateWaitTimer);
    DecIfAbove0_Short(&mInvincibleTimer);
    DecIfAbove0_Short(&mJumpComboTimer);
    DecIfAbove0_Short(&mPunchKickCooldown);
    DecIfAbove0_Short(&mStickGraceTimer);
    DecIfAbove0_Short(&mCrouchTimer);
    DecIfAbove0_Short(&mScaleAnimTimer);
    DecIfAbove0_Short(&mHoldHeavyTimer);
    DecIfAbove0_Short(&mClimbCooldown);
    DecIfAbove0_Short(&mWalkTimer);
    DecIfAbove0_Short(&mWaterRunTimer);
    DecIfAbove0_Short(&mQuicksandRunTimer);
    DecIfAbove0_Short(&mQuicksandJumpTimer);
    DecIfAbove0_Short(&mTeleportTimer);

    if (mTeleportTimer == 1)
        data_0209f284 = 0;

    if (mIsControlDisabled == 0) {
        DecIfAbove0_Short(&mBalloonTimer);
        if (DecIfAbove0_Short(&mFireTimer) == 0)
            func_ov002_020d80d0(((char *)this));
        if (DecIfAbove0_Short(&mPowerupTimer) == 0) {
            func_ov002_020e032c(((char *)this));
            func_ov002_020bdef0();
            func_ov002_020bdd9c();
        }
        if (DecIfAbove0_Short(&mMegaTimer) == 0)
            func_ov002_020bdd2c();

        if (mPrevAreaId != mAreaId) {
            mPrevAreaId = mAreaId;
            mMouthHoldTimer = 0;
        }
        if (DecIfAbove0_Short(&mMouthHoldTimer) == 0 && mObjInMouth != 0)
            func_ov002_020d6790(((char *)this));
    }

    {
        s16 ang = mDesiredAngleY;
        s32 stride = 0x18;
        mPrevDesiredAngleY = ang;
        mDesiredAngleY = *(s16 *)((char *)&data_0209f4a6 + gActivePlayerSlot * stride);
        if (data_0209fc68 == 0) {
            s16 add = GetAngleToCamera(0);
            s16 *p = (s16 *)LAU((char *)&mDesiredAngleY);
            *p = (s16)(*p + add);
        } else {
            s16 add = GetAngleToCamera(mPlayerNo);
            s16 *p = (s16 *)LAU((char *)&mDesiredAngleY);
            *p = (s16)(*p + add);
        }
    }

    func_ov002_020c2e78();
    func_ov002_020ca940(((char *)this));
    func_ov002_020d8158(((char *)this));

    {
        Player::State *st = mState;
        if (*(void **)&st->mMain != 0)
            (this->*(st->mMain))();
    }

    if ((u16)(mStateFlags & 0x80) != 0) {
        this->ChangeState(data_ov002_021104fc);
        *(u16 *)LAU((char *)&mStateFlags) &= ~0x80;
    }

    if (mIsNoControl != 0)
        fallFlag = 1;
    else
        fallFlag = 0;
    if (data_0209f2f8 == 0x1f
        && IsState(data_ov002_021104b4) != 0)
        fallFlag = 0;

    if (mIsAirborne == 0 || fallFlag != 0 || mPeakY < mPosY)
        mPeakY = mPosY;

    if (mIsAirborne == 0 && mIsFallScreaming != 0) {
        Sound::PlayCharVoice(mCharacter, 0x2e, mCamSpacePos);
        mIsFallScreaming = 0;
    }
    if (mIsAirborne != 0 && mIsFallScreaming == 0
        && IsState(data_ov002_02110454) == 0
        && (mPeakY - mPosY) > 0x47e000) {
        mIsFallScreaming = 1;
        Sound::PlayCharVoice(mCharacter, 0x1d, mCamSpacePos);
    }

    if (mIsInShallowWater != 0 && mIsUnderwater == 0 && mIsAirborne == 0)
        func_ov002_020ce8bc(((char *)this), mHorzSpeed);

    mPreClsnAngleY = mAngleY;
    *(u16 *)LAU((char *)&mStateFlags) &= ~0x440;

    if (mHasWings != 0)
        mModels.mAnmModel2.Advance();

    mPreClsnPosX = mPosX;
    mPreClsnPosY = mPosY;
    mPreClsnPosZ = mPosZ;
    func_ov002_020bf36c(&mdCcAcPos_c);
    func_ov002_020bf13c();

    {
        s32 *p = (s32 *)LAU((int)&v0);
        s32 z = 0;
        p[0] = z;
        p[1] = z;
        {
            Player::State *st = &data_ov002_0210ffec;
            p[2] = z;
            if (IsState(*st) != 0) {
                if (mIsMega != 0) {
                    s32 t = 0x12c000;
                    v0.y = -t;
                } else {
                    s32 t = 0x64000;
                    v0.y = -t;
                }
            } else if (IsState(data_ov002_021106f4) != 0) {
                v0.y = 0x64000;
            }
        }
    }
    v1.x = v0.x;
    v1.y = v0.y;
    v1.z = v0.z;
    mdCcAcPos_c.SetPosRelativeToActor(v1);
    func_ov002_020c2db8();

    if (mIsInShallowWater != 0)
        mGroundSoundType = 3;

    if (data_0209ee90.flag26c != 0) {
        data_0209ee90.flag26c = 0;
        this->ChangeState(data_ov002_021101b4);
        mStateStep = 0;
        this->ChangeState(*reinterpret_cast<Player::State *>(&data_ov002_0210ffec));
    }

    func_ov002_020db704(((char *)this));
    func_ov002_020d869c(((char *)this));

    {
        u16 t = mInvincibleTimer;
        if (t != 0) {
            if (t == 1)
                *(u32 *)LAU((char *)&mdCcAcPos_c.vulnFlags) |= 0x10000000;
            else
                *(u32 *)LAU((char *)&mdCcAcPos_c.vulnFlags) &= ~0x10000000;
        }
    }

    mdCcAcPos_c.Clear();
    if (mIsBodyClsnEnabled != 0)
        mdCcAcPos_c.Update();

    if (data_0209fc68 == 0)
        gActivePlayerSlot = 0;
    else
        gActivePlayerSlot = data_0209f250;

    func_ov002_020e4bb8(((char *)this));

    {
        s32 r2 = mTongueHitDist;
        if (r2 != 0) {
            u8 ch = mUseAltBodyModel;
            Matrix4x3 *t4 = &mModels.mHeadModels[(ch << 2) + 3]->data.transforms[3];
            if (r2 < 0)
                r2 = -r2;
            t4->m[11] = r2 >> 3;
        }
    }

    data_0209f254 = 0;
    if (param1 == 3) {
        void *p = *(void **)(*(void **)((char *)&mHeldObjQueue));
        if (p != 0)
            data_0209f254 = ((daYegg_c *)p)->mParamHigh;
    }

    return 1;
}
}
#undef LAU

// @symbol _ZN6Player13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
// Match: Player::InitResources (ov002:0x020e558c).
// The 0x14-byte block allocated for mHeldObjQueue is zeroed in place with
// mwccarm's __builtin__clear intrinsic: that is the compiler's inline CLEAR
// (5 x 4 strb post-inc, subs/bne) aimed at the heap pointer itself. Value-
// init, placement new, struct assignment and memset all go through a stack
// temporary or an out-of-line __clear/memset call instead.
// Also: pin data_0209f2d8 in local `d` so ==2 keeps r1 bool (preserves r0);
// decl order td,tx,tz + load order for dBgCh_Gnd pos regs.
extern "C" {
s32 Player::InitResources()
{
    struct CAA0 { char pad[8]; int unk8; };
    int SublevelToLevel(int s);
    void func_ov002_020e6330(void* p);
    void func_ov002_020e6350(void* p);
    void func_ov002_020e5948(void* p);
    void* func_02073470(int a, int b, int c, void* d, void* e);
    void func_ov002_020e63a4(void* p);
    void func_02035644(void* p, int a);
    void func_ov002_020d6368(void* p);
    void LoadSilverStarAndNumber(void);
    void func_02013a00(void);
    int StartWithFarCamera(void);
    /* local extern: the registrar takes this D1 by address, which C++ cannot spell. */
    void *_ZN7Vector3D1Ev(void *object);
    void func_0203d384(void);
    extern u8 data_0209f2d8;
    extern s8 data_0209f2f8;
    extern u8 data_0209f254;
    extern s8 data_02092114;
    extern u8 data_02092128[];
    extern CAA0 data_0209caa0;
    extern int gRunningKuppaScript;
    extern u8 data_0209f250;
    extern u8 data_0209f2fc;
    extern int data_0209212c;
    extern u8 data_0209211c;
    extern u8 data_0209f200;
    /* The helpers below take void or char pointers, and this file's match
       depends on declaration and load order documented in the header comment
       above. One alias keeps every one of those decisions untouched. */
    void* self = (void*)this;
    char* c = (char*)self;
    Vector3 pos;
    unsigned int a, b;
    u8 n8;
    u8 sub;
    int t;
    int changed;
    int i; unsigned int u;
    u8 f254;
    u8 cur;
    int td, tx, tz;

    a = param1;
    mPlayerNo = (a >> 6) & 3;
    n8 = a >> 8;
    b = param1;
    sub = (b >> 3) & 7;
    mCharacter = b & 7;
    *(u8*)(c + 0x6da) = sub;
    changed = 0;
    t = (unsigned int)(data_0209f2d8 == 1);
    if (t == 0) {
        if (SublevelToLevel(data_0209f2f8) == 0x1d) {
            data_0209f254 = 0;
            cur = mCharacter;
            if (sub != cur) {
                data_02092114 = -1;
                sub = cur;
                data_02092128[mPlayerNo] = *(volatile u8*)(&mCharacter);
                changed = 1;
            }
        }
        if (data_02092114 >= 0) sub = mCharacter;
    }
    param1 = sub;
    {
      unsigned int d = data_0209f2d8;
      t = (unsigned int)(d == 2);
      if (t != 0) goto Ld0;
      t = (unsigned int)(d == 1);
    }
    if (t != 0) goto Lac;
    if ((data_0209caa0.unk8 & 0x80) != 0) goto Lac;
    t = (unsigned int)(gRunningKuppaScript != 0);
    if (t == 0) goto Ld0;
Lac:
    if (mPlayerNo == data_0209f250) {
        func_ov002_020e6330(c);
        func_ov002_020e6350(c);
    }
Ld0:
    func_ov002_020e5948(c);
    if (changed != 0) ((Player *)(c))->func_ov002_020beabc();
    mTrailPos =
        (Vector3 *)func_02073470(0x32, 0xc, 8, (void *)func_0203d384, (void *)_ZN7Vector3D1Ev);
    mTrailFlags = (u8 *)Memory::operator_new2(0x32);
    *(void **)(&mHeldObjQueue) = Memory::operator_new2(0x14);
    __builtin__clear((void*)mHeldObjQueue, 0x14);
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    func_ov002_020e63a4(c);
    mMeshClsn.Init((dActor_c *)c, 0x32000, 0x32000, &mPrevAngle, &mAngle);
    func_02035644(&mMeshClsn, 0x28000);
    mTerminalVelocity = -0x4b000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mSpawnAngleY = mAngleY;
    mGroundY = 0x80000000;
    mWaterSurfaceY = 0x80000000;
    mToxicSurfaceY = 0x80000000;
    mOpacity = 0x1f;
    mClpsWord1Lo = 0xff;
    *(s16*)&mPrevAreaId = mAreaId;
    f254 = data_0209f254;
    if (f254 != 0) {
        if (param1 == 3) {
            mEggParams = f254;
            func_ov002_020d6368(c);
            if ((f254 & 0x80) != 0) {
                if ((mLoadedResourceFlags & 1) == 0) {
                    LoadSilverStarAndNumber();
                    *(u8*)((int)(&mLoadedResourceFlags)) |= 1;
                }
            }
        }
    }
    data_0209f254 = 0;
    if (data_0209f2fc == 1) {
        if (SaveData::HasPlayerLostCap() != 0)
            func_02013a00();
    }
    ((Player *)(c))->func_ov002_020c7dd0(n8);
    dBgCh_Gnd rc;
    tz = mPosZ;
    tx = mPosX;
    td = data_0209212c;
    pos.x = tx;
    pos.y = td;
    pos.z = tz;
    rc.mProbeHeight = td * 2;
    rc.SetObjAndPos(pos, (dActor_c *)c);
    rc.StartDetectingWater();
    if (data_0209f2f8 == 0x1d) {
        data_0209211c = 2;
        data_0209f200 = 0xe;
    }
    if (StartWithFarCamera() != 0) mUseFarCamera = 1;
    mPeakY = mPosY;
    return 1;
}
}

// @symbol func_ov002_020e5948
/* defined in src/game/actors/d_a_obj1up_logo.cpp */
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
#pragma opt_propagation off
#pragma opt_loop_invariants off
extern "C" {
void func_ov002_020e5948(void* arg0)
{
    extern void LoadSilverStarAndNumber(void);
    extern void func_ov002_020bec2c(void* p);
    extern int *func_ov002_020e6bb0(void *);
    extern void func_ov002_020e6780(void* p);
    extern s8 data_0209f310[];
    extern u8 data_0209f2d8;
    extern s8 data_0209f2f8;
    extern SharedFilePtr data_ov002_0210e798;
    extern SharedFilePtr data_ov002_0210e4f0;
    extern SharedFilePtr data_ov002_0210e3d8;
    extern SharedFilePtr data_ov002_0210e6c0;
    extern SharedFilePtr data_ov002_0210eb98;
    extern SharedFilePtr data_ov002_0210eb88;
    extern SharedFilePtr data_ov002_0210e4c0;
    extern SharedFilePtr data_ov002_0210da40;
    extern SharedFilePtr data_ov002_0210d9a0;
    extern SharedFilePtr data_ov002_0210d9c0;
    extern SharedFilePtr data_ov002_0210e500;
    extern SharedFilePtr data_ov002_0210e1c8;
    extern SharedFilePtr data_ov002_0210e770;
    extern SharedFilePtr data_ov002_0210e230;
    extern SharedFilePtr data_ov002_0210e288;
    extern SharedFilePtr data_ov002_0210e478;
    extern SharedFilePtr data_ov002_0210e9d0;
    extern SharedFilePtr data_ov002_0210ea10;
    extern SharedFilePtr data_ov002_0210e538;
    extern SharedFilePtr data_ov002_0210eac8;
    extern SharedFilePtr data_ov002_0210e898;
    extern SharedFilePtr data_ov002_0210e9e8;
    extern SharedFilePtr data_ov002_0210eba8;
    extern SharedFilePtr data_ov002_0210eb70;
    extern SharedFilePtr data_ov002_0210eaf0;
    extern SharedFilePtr data_ov002_0210e640;
    extern SharedFilePtr data_ov002_0210e670;
    extern SharedFilePtr data_ov002_0210e680;
    extern SharedFilePtr data_ov002_0210e438;
    extern SharedFilePtr data_ov002_0210e3b0;
    extern SharedFilePtr data_ov002_0210e3c8;
    extern SharedFilePtr data_ov002_0210e408;
    extern SharedFilePtr data_ov002_0210e400;
    extern SharedFilePtr data_ov002_0210ecb8;
    extern SharedFilePtr data_ov002_0210e8d0;
    extern SharedFilePtr data_ov002_0210ebd8;
    extern SharedFilePtr data_ov002_0210e750;
    extern SharedFilePtr data_ov002_0210d9a8;
    extern SharedFilePtr data_ov002_02110aa4;
    extern SharedFilePtr data_ov002_0210e588;
    extern SharedFilePtr data_ov002_0210e7e8;
    extern SharedFilePtr data_ov002_0210e450;
    extern SharedFilePtr data_ov002_0210e788;
    extern SharedFilePtr data_ov002_0210e1e8;
    extern SharedFilePtr data_ov002_0210e1e0;
    extern SharedFilePtr data_ov002_0210e4d0;
    extern SharedFilePtr data_ov002_0210e4e8;
    extern SharedFilePtr data_ov002_0210e1d0;
    extern SharedFilePtr data_ov002_0210e800;
    extern SharedFilePtr data_ov002_0210e4b8;
    extern SharedFilePtr data_ov002_0210e270;
    extern SharedFilePtr data_ov002_0210e458;
    extern SharedFilePtr data_ov002_0210ea70;
    extern SharedFilePtr data_ov002_0210e540;
    extern SharedFilePtr data_ov002_0210e8e8;
    extern SharedFilePtr data_ov002_0210eca8;
    extern SharedFilePtr data_ov002_0210ec98;
    extern SharedFilePtr data_ov002_0210e600;
    extern SharedFilePtr data_ov002_0210ea88;
    extern SharedFilePtr data_ov002_0210e910;
    extern SharedFilePtr data_ov002_0210e460;
    extern SharedFilePtr data_ov002_0210e2f0;
    extern SharedFilePtr data_ov002_0210e958;
    extern SharedFilePtr data_ov002_0210eb38;
    extern SharedFilePtr data_ov002_0210e660;
    extern SharedFilePtr data_ov002_0210ea30;
    extern SharedFilePtr data_ov002_0210e3e8;
    extern SharedFilePtr data_ov002_0210eb90;
    extern SharedFilePtr data_ov002_0210e6b8;
    extern SharedFilePtr data_ov002_0210e620;
    extern SharedFilePtr data_ov002_0210e6a0;
    extern SharedFilePtr data_ov002_0210eb58;
    extern SharedFilePtr data_ov002_0210e3e0;
    extern SharedFilePtr data_ov002_0210eb10;
    extern SharedFilePtr data_ov002_0210e708;
    extern SharedFilePtr data_ov002_0210ebc8;
    extern SharedFilePtr data_ov002_0210e6e0;
    extern SharedFilePtr data_ov002_0210e430;
    extern SharedFilePtr data_ov002_0210ec40;
    extern SharedFilePtr data_ov002_0210e738;
    extern SharedFilePtr data_ov002_0210e250;
    extern SharedFilePtr data_ov002_0210ec10;
    extern SharedFilePtr data_ov002_0210ec00;
    extern SharedFilePtr data_ov002_0210e728;
    extern SharedFilePtr *data_ov002_020ff480[];
    extern SharedFilePtr *data_ov002_0210a68c[];
    extern SharedFilePtr *data_ov002_0210a69c[];
    extern SharedFilePtr *data_ov002_0210a6ac[];
    extern SharedFilePtr *data_ov002_020ff2f0[];
    extern SharedFilePtr data_ov002_0210da38;
    extern SharedFilePtr data_ov002_0210e780;
    extern SharedFilePtr data_ov002_0210e6f8;
    extern SharedFilePtr data_ov002_0210e7b0;
    extern SharedFilePtr data_ov002_0210ebb8;
    extern SharedFilePtr data_ov002_0210eb20;
    Player *c = (Player *)arg0;
    Player *player = c;
    s32 i;
    s32 j;
    s32 bodyId;
    void* ma;
    void* t;
    s32 t2;
    void* p;
    s32 n;
    s32 k;
    s32 outer;
    dExtAnmTexPat_c *texseq;
    SharedFilePtr *entry;

    dExtFrameCtrl_c::LoadFile(data_ov002_0210e798);
    dExtFrameCtrl_c::LoadFile(data_ov002_0210e4f0);
    dExtFrameCtrl_c::LoadFile(data_ov002_0210e3d8);
    dExtFrameCtrl_c::LoadFile(data_ov002_0210e6c0);
    dExtFrameCtrl_c::LoadFile(data_ov002_0210eb98);
    dExtFrameCtrl_c::LoadFile(data_ov002_0210eb88);
    dExtFrameCtrl_c::LoadFile(data_ov002_0210e4c0);

    if (data_0209f310[player->mPlayerNo] != 0) {
        LoadSilverStarAndNumber();
        player->mLoadedResourceFlags |= 1;
    }

    {
        s32 b = player->param1;
        s32 a = player->mCharacter;
        if (a != b) {
        switch (b) {
        case 0:
            dExtSimpleModel_c::LoadFile(data_ov002_0210da40);
            player->mLoadedResourceFlags |= 2;
            break;
        case 1:
            dExtSimpleModel_c::LoadFile(data_ov002_0210d9a0);
            player->mLoadedResourceFlags |= 4;
            break;
        case 2:
            dExtSimpleModel_c::LoadFile(data_ov002_0210d9c0);
            player->mLoadedResourceFlags |= 8;
            break;
        }
    }
    }

    dExtAnmTexPat_c::LoadFile(data_ov002_0210e500);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e1c8);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e770);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e230);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e288);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e478);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e9d0);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210ea10);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e538);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210eac8);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e898);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e9e8);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210eba8);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210eb70);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210eaf0);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e640);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e670);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e680);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e438);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e3b0);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e3c8);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e408);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e400);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210ecb8);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e8d0);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210ebd8);

    {
        s32 mode;
        mode = data_0209f2d8;
        mode = (mode == 1);
        if (mode == 0) {
            dExtFrameCtrl_c::LoadFile(data_ov002_0210e750);
            dExtSimpleModel_c::LoadFile(data_ov002_0210d9a8);
            dExtSimpleModel_c::LoadFile(data_ov002_02110aa4);
            dExtFrameCtrl_c::LoadFile(data_ov002_0210e588);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e7e8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e450);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e788);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e1e8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e1e0);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e4d0);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e4e8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e1d0);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e800);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e4b8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e270);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e458);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210ea70);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e540);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e8e8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210eca8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210ec98);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e600);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210ea88);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e910);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e460);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e2f0);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e958);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210eb38);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e660);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210ea30);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e3e8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210eb90);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e6b8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e620);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e6a0);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210eb58);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e3e0);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210eb10);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e708);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210ebc8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e6e0);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e430);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210ec40);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e738);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e250);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210ec10);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210ec00);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e728);
        }
    }

    {
        s32 mode;
        mode = data_0209f2d8;
        mode = (mode == 1);
        if (mode == 0) {
            player->mCharFileBase = 0;
        } else {
            player->mCharFileBase = 0x44;
            func_ov002_020bec2c(c);
        }
    }

    dExtFrameCtrl_c::LoadFile(*data_ov002_020ff480[(s32)player->mCharFileBase]);

    i = 0;
    do {
        player->mModels.mBodyModels[i] = new dExtAnm2Model_c;
        player->mModels.mBodyModels[i]->SetFile(
            (BMD_File *)dExtSimpleModel_c::LoadFile(*data_ov002_0210a68c[i]), 1, 1);
        player->mModels.mBodyModels[i]->Func_020162C4((u32)data_ov002_020ff480[i + 0xC4]->filePtr,
                                                      0, 0x1000, 0);
        player->mModels.mBodyTexParams[i] =
            (s32)func_ov002_020e6bb0(player->mModels.mBodyModels[i]);
        i++;
    } while (i < 4);

    func_ov002_020e6780(c);

    i = 0;
    do {
        if (i == 3) {
            player->mModels.mHeadModels[i] = new dExtAnmModel_c;
            player->mModels.mHeadModels[i]->SetFile(
                (BMD_File *)dExtSimpleModel_c::LoadFile(*data_ov002_0210a69c[i]), 1, 1);
            ((dExtAnmModel_c *)player->mModels.mHeadModels[i])
                ->SetAnim((BCA_File *)data_ov002_0210eb88.filePtr, 0, 0x1000, 0);
        } else {
            player->mModels.mHeadModels[i] = new dExtSimpleModel_c;
            player->mModels.mHeadModels[i]->SetFile(
                (BMD_File *)dExtSimpleModel_c::LoadFile(*data_ov002_0210a69c[i]), 1, 1);
        }
        player->mModels.mHeadTexParams[i] =
            (s32)func_ov002_020e6bb0(player->mModels.mHeadModels[i]);
        i++;
    } while (i < 4);

    j = 0;
    do {
        p = new dExtSimpleModel_c;
        {
            s32 idx = j + 4;
            player->mModels.mHeadModels[idx] = (dExtSimpleModel_c *)p;
            player->mModels.mHeadModels[idx]->SetFile(
                (BMD_File *)dExtSimpleModel_c::LoadFile(*data_ov002_0210a6ac[j]), 1, 1);
            player->mModels.mHeadTexParams[idx] =
                (s32)func_ov002_020e6bb0(player->mModels.mHeadModels[idx]);
        }
        j++;
    } while (j < 4);

    {
        s32 mode;
        mode = data_0209f2d8;
        mode = (mode == 1);
        if (mode == 0) {
            player->mModels.mSleepBubbleModel = new dExtSimpleModel_c;
            player->mModels.mSleepBubbleModel->SetFile(
                (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov002_0210da38), 1, -1);

            if (data_0209f2f8 != 2 && data_0209f2f8 != 5 && data_0209f2f8 != 4) {
                player->mLoadedResourceFlags |= 0x20;
                player->mModels.mBodyModels[4] = new dExtAnm2Model_c;
                player->mModels.mBodyModels[4]->SetFile(
                    (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov002_0210e780), 1, -1);
                player->mModels.mBodyModels[4]->Func_020162C4(
                    (u32)data_ov002_020ff480[0xC6]->filePtr, 0, 0x1000, 0);
                player->mModels.mAnmModel1.SetFile(
                    (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov002_0210e6f8), 1, 1);
                player->mModels.mAnmModel1.SetAnim((BCA_File *)data_ov002_0210e750.filePtr, 0,
                                                   0x1000, 0);
                player->mModels.mAnmModel2.SetFile(
                    (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov002_0210e7b0), 1, 1);
                player->mModels.mAnmModel2.SetAnim((BCA_File *)data_ov002_0210e4c0.filePtr, 0,
                                                   0x1000, 0);
            }
        } else {
            player->mModels.mAnmModel2.SetFile(
                (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov002_0210e7b0), 1, 1);
            player->mModels.mAnmModel2.SetAnim((BCA_File *)data_ov002_0210e4c0.filePtr, 0, 0x1000,
                                               0);
        }
    }

    player->mShadowModel.InitCylinder();

    bodyId = c->GetBodyModelID(3, 0);
    ma = player->mModels.mBodyModels[bodyId];
    {
        u8 pid = player->mPlayerNo;
        t = ((dExtAnmModel_c *)ma)->data.materials;
        t2 = *(s32*)((char*)t + 0x20);
        player->mVsYoshiMaterialParam = t2 + (pid << 1);
    }

    n = 0;
    {
        s32 mode;
        mode = data_0209f2d8;
        mode = (mode == 1);
        if (mode != 0)
            n = 0x11;
    }

    if (n < 0x19) {
        outer = n * 4;
        do {
            k = 0;
            texseq = player->mModels.mTexSeqBody;
            do {
                entry = data_ov002_020ff2f0[outer + k];
                dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov002_0210a69c[k]->filePtr,
                                         *(BTP_File *)entry->filePtr);
                if (n == 0x18) {
                    texseq->SetFile(*(BTP_File *)entry->filePtr, 0, 0x1000, 0);
                }
                k++;
                texseq++;
            } while (k < 4);
            n++;
            outer += 4;
        } while (n < 0x19);
    }

    dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov002_0210ebb8.filePtr,
                             *(BTP_File *)data_ov002_0210e8d0.filePtr);
    player->mModels.mTexSeqPlayer[0].SetFile(*(BTP_File *)data_ov002_0210e8d0.filePtr, 0, 0x1000,
                                             0);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov002_0210eb20.filePtr,
                             *(BTP_File *)data_ov002_0210ebd8.filePtr);
    player->mModels.mTexSeqPlayer[1].SetFile(*(BTP_File *)data_ov002_0210ebd8.filePtr, 0, 0x1000,
                                             0);
}
}
#pragma pop

// @symbol func_ov002_020e6330
extern "C" {
void func_ov002_020e6330(Player *self) {
    extern unsigned char data_ov002_020ff0f0[];
    extern void func_02011f7c(int a);
    func_02011f7c(data_ov002_020ff0f0[self->mCharacter & 3]);
}
}

// @symbol func_ov002_020e6350
extern "C" {
void func_ov002_020e6350(Player *c) {
    extern int func_02011ee4(int x);
    extern unsigned char data_0209f2d8;
    extern unsigned char data_ov002_020ff0f4[];
  int b = (int)(data_0209f2d8 == 1);
  if(b != 0) return;
  func_02011ee4(data_ov002_020ff0f4[c->param1 & 3]);
}
}

// @symbol func_ov002_020e63a4
extern "C" {
void func_ov002_020e63a4(char* c){
    extern int data_ov002_0210f92c[];
    Player *player = (Player *)c;
    int v[3];
    v[0] = data_ov002_0210f92c[0];
    v[1] = data_ov002_0210f92c[1];
    v[2] = data_ov002_0210f92c[2];
    player->mdCcAcPos_c.Init((dActor_c *)c, *(Vector3 *)v, 0x28000, 0x96000, 0x400008, 0x4f3ffe0);
}
}

/* One runtime bone of a body model: ModelComponents::bones points at
   0x34-byte records, not the file's 0x40-byte BMD_Bone. Same shape as
   d_a_cnn.cpp's CnnBone, except that the rotations are unsigned: the ROM
   reads them with ldrh. func_ov002_020e640c writes the head (bone 15) and
   spine (bone 8) rotations, reads bone 1's translation as root motion, and
   resets every other bone's translation from the file bone's +0x24..+0x2c. */
struct PlayerBone {
    u8 pad_00[0x1a];
    u16 rotX;   /* 0x1a */
    u16 rotY;   /* 0x1c */
    u16 rotZ;   /* 0x1e */
    s32 transX; /* 0x20 */
    s32 transY; /* 0x24 */
    s32 transZ; /* 0x28 */
    u8 pad_2c[0x8];
};
#ifndef SM64DS_PLATFORM_PC
typedef char PlayerBone_size_must_be_0x34[sizeof(PlayerBone) == 0x34 ? 1 : -1];
#endif

/* One of the file's 0x40-byte bone records (BMD_File::bones). Its rest-pose
   translation sits at +0x24; func_ov002_020e640c copies it into the runtime
   PlayerBone. d_ent_obj.cpp's VsBoneFileRecord reads the same three words in
   UnknownVsPlayer::func_ov075_02114b60. */
struct PlayerFileBone {
    u8 pad_00[0x24];
    s32 transX; /* 0x24 */
    s32 transY; /* 0x28 */
    s32 transZ; /* 0x2c */
    u8 pad_30[0x10];
};
typedef char PlayerFileBone_size_must_be_0x40[sizeof(PlayerFileBone) == 0x40 ? 1 : -1];

// @symbol func_ov002_020e640c
extern "C" {
void func_ov002_020e640c(char *c)
{
    extern void Vec3_RotateYAndTranslate(Vector3 *dst, const Vector3 *src, s16 ang, const Vector3 *t);
    Player *player = (Player *)c;
    char *md;
    char *mp;
    char *r4p;
    int id;
    Vector3 d;

    id = ((Player *)c)->GetBodyModelID(player->param1 & 0xff, 0);
    {
        int *p = (int *)(int)&player->mModels.mBodyModels[id]->data;
        md = (char *)p[2];
        mp = (char *)p[0];
    }
    r4p = (char *)((BMD_File *)mp)->bones;

    ((PlayerBone *)md)[8].rotY += player->mSpineRot[1];
    ((PlayerBone *)md)[8].rotZ += player->mSpineRot[2];

    {
        int b = player->mHeadRotMode;
        if (b == 0) {
        } else if (b == 1) {
            ((PlayerBone *)md)[15].rotX += *(u16 *)player->mHeadRot;
            ((PlayerBone *)md)[15].rotY += (u16)player->mHeadRot[1];
            ((PlayerBone *)md)[15].rotZ += (u16)player->mHeadRot[2];
        } else if (b == 2) {
            ((PlayerBone *)md)[15].rotX = *player->mHeadRot;
            ((PlayerBone *)md)[15].rotY = player->mHeadRot[1];
            ((PlayerBone *)md)[15].rotZ = player->mHeadRot[2];
        } else if (b == 3) {
            ((PlayerBone *)md)[15].rotZ = (s16)(*player->mHeadRot + 0x4000);
            ((PlayerBone *)md)[15].rotY = player->mHeadRot[1];
            ((PlayerBone *)md)[15].rotX = player->mHeadRot[2];
            ((PlayerBone *)md)[8].rotZ = *player->mSpineRot;
            ((PlayerBone *)md)[8].rotX = player->mSpineRot[1];
            ((PlayerBone *)md)[8].rotY = player->mSpineRot[2];
        } else if (b == 4) {
            ApproachLinear2(player->mHeadRot[0], (s16)(((PlayerBone *)md)[15].rotZ - 0x4000),
                            0x400);
            ApproachLinear2(player->mHeadRot[1], ((PlayerBone *)md)[15].rotY, 0x400);
            ApproachLinear2(player->mHeadRot[2], ((PlayerBone *)md)[15].rotX, 0x400);
            ((PlayerBone *)md)[15].rotZ = (s16)(*player->mHeadRot + 0x4000);
            ((PlayerBone *)md)[15].rotY = player->mHeadRot[1];
            ((PlayerBone *)md)[15].rotX = player->mHeadRot[2];
        }
    }

    *(u16 *)(c + 0x580) = ((PlayerBone *)md)[15].rotX;
    *(u16 *)(c + 0x582) = ((PlayerBone *)md)[15].rotY;
    *(u16 *)(c + 0x584) = ((PlayerBone *)md)[15].rotZ;

    ((PlayerBone *)md)->transX = ((PlayerFileBone *)r4p)->transX;
    ((PlayerBone *)md)->transY = ((PlayerFileBone *)r4p)->transY;
    ((PlayerBone *)md)->transZ = ((PlayerFileBone *)r4p)->transZ;

    {
        int b = ((Player *)c)->mRootMotionMode;
        if (b != 0) {
            if ((b & 1) == 1) {
                ((Player *)c)->mStateTargetX = ((PlayerBone *)md)[1].transX;
                if (((Player *)c)->mRootMotionMode < 3)
                    ((Player *)c)->mStateTargetY = 0;
                else
                    ((Player *)c)->mStateTargetY = ((PlayerBone *)md)[1].transY;
                ((Player *)c)->mStateTargetZ = ((PlayerBone *)md)[1].transZ;
                ((Player *)c)->mStateAnchorX = ((Player *)c)->mStateTargetX;
                ((Player *)c)->mStateAnchorY = ((Player *)c)->mStateTargetY;
                ((Player *)c)->mStateAnchorZ = ((Player *)c)->mStateTargetZ;
                ((Player *)c)->mRootMotionMode++;
            } else {
                d.x = (((PlayerBone *)md)[1].transX - ((Player *)c)->mStateTargetX) << 3;
                if (((Player *)c)->mRootMotionMode < 3)
                    d.y = 0;
                else
                    d.y = (((PlayerBone *)md)[1].transY - ((Player *)c)->mStateTargetY) << 3;
                d.z = (((PlayerBone *)md)[1].transZ - ((Player *)c)->mStateTargetZ) << 3;
                Vec3_RotateYAndTranslate(&player->mPos, &player->mPos, player->mAngleY, &d);
                ((Player *)c)->mStateTargetX = ((PlayerBone *)md)[1].transX;
                if (((Player *)c)->mRootMotionMode >= 3)
                    ((Player *)c)->mStateTargetY = ((PlayerBone *)md)[1].transY;
                ((Player *)c)->mStateTargetZ = ((PlayerBone *)md)[1].transZ;
            }
            ((PlayerBone *)md)[1].transX = 0;
            if (((Player *)c)->mRootMotionMode >= 3)
                ((PlayerBone *)md)[1].transY = 0;
            ((PlayerBone *)md)[1].transZ = 0;
        }
    }

    md += 0x68;
    r4p += 0x80;
    {
        s16 i = 2;
        if (((BMD_File *)mp)->numBones > 2) {
            do {
                ((PlayerBone *)md)->transX = ((PlayerFileBone *)r4p)->transX;
                ((PlayerBone *)md)->transY = ((PlayerFileBone *)r4p)->transY;
                ((PlayerBone *)md)->transZ = ((PlayerFileBone *)r4p)->transZ;
                i = (s16)(i + 1);
                md += 0x34;
                r4p += 0x40;
            } while ((u32)i < ((BMD_File *)mp)->numBones);
        }
    }
}
}

// @symbol func_ov002_020e6780
extern "C" {
void func_ov002_020e6780(char *raw)
{
    Player *player = (Player *)raw;
    player->mSpineRot[0] = 0;
    player->mSpineRot[1] = 0;
    player->mSpineRot[2] = 0;
    player->mHeadRot[0] = 0;
    player->mHeadRot[1] = 0;
    player->mHeadRot[2] = 0;
    player->mHeadRotMode = 0;
}
}

/* Out-of-line ~Player is the key function. defer_codegen on for this
 * definition only: under defer_codegen off the variants come out D1, D0, D2,
 * and the cartridge order is D0 then D1. With deferred codegen they come out
 * D2, D0, D1, and the following function stays after them. RTTI stays off
 * from here on, which suppresses _ZTI6Player and _ZTS6Player and writes a
 * zero typeinfo word in the vtable preamble. Retail relocates 0x0210a838 to
 * _ZTI7daPly_c. The 124 slot bytes reproduce. D2 is not in the cartridge. */
#pragma defer_codegen on
#pragma RTTI off

// @symbol _ZN6PlayerD0Ev
// @symbol _ZN6PlayerD1Ev
Player::~Player()
{
}

#pragma defer_codegen off

/* Every member is built by the compiler. mModels' implicit constructor is
 * inlined with its own `this` (Player + 0xdc), which is the second base
 * register the retail body carries. C2 is not in the cartridge. */
// @symbol _ZN6PlayerC1Ev
Player::Player() {}

// @symbol _ZN6Player13OnYoshiTryEatEv
int Player::OnYoshiTryEat()
{
    (void)this;
    return 1;
}

// @symbol func_ov002_020e69c0
extern "C" {
extern unsigned short data_ov002_0210a760[];
void func_ov002_020e69c0(void *unused)
{
    (void)unused;
    unsigned short *dst = data_ov002_0210a760;
    unsigned short a = *dst;
    unsigned short b = dst[1];
    /* byte +4 kept: &dst[2] adds a word (measured). */
    unsigned short *src = (unsigned short *)((char *)dst + 4);
    int i;
    for (i = 2; i < 0x20; i++) {
        *dst++ = *src++;
    }
    dst[0] = a;
    dst[1] = b;
}

/* Blends one RGB555 channel: to * t plus the already-scaled from term. */
static inline int Color_Lerp(int to, int t, int fromScaled) {
    return (to * t) + fromScaled;
}

/* Blends RGB555 colour from toward to by the fx12 weight t (0..0x1000).
 * The first argument is not read. */
// @symbol Color_Interp
int Color_Interp(int unused, int from, int to, int t) {
    int r;
    int g;
    int b;
    int s;
    unsigned int out;
    if (t >= 0x1000) {
        return to;
    }
    if (t <= 0) {
        return from;
    }
    s = 0x1000 - t;
    r = (from & 0x1f) * s;
    g = (from & 0x3e0) * s;
    b = (from & 0x7c00) * s;
    r = Color_Lerp(to & 0x1f, t, r);
    g = Color_Lerp(to & 0x3e0, t, g);
    b = Color_Lerp(to & 0x7c00, t, b);
    out = ((unsigned int)((r | g) | b)) << 4;
    return out >> 0x10;
}

extern void func_020555a4(void *table);

/* A 32-entry RGB555 ramp and the blended copy func_ov002_020e6a78 builds
 * behind it. */
struct ColorRamp {
    unsigned short base[0x20];
    unsigned short blended[0x20]; /* 0x40 */
};

/* Blends the 32-entry ramp toward data_ov002_0210a760 (which
 * func_ov002_020e69c0 rotates) by blend, into the ramp's second half,
 * and hands that half to func_020555a4. */
// @symbol func_ov002_020e6a78
void func_ov002_020e6a78(void *ramp, int blend) {
    unsigned short *src = ((ColorRamp *)ramp)->base;
    unsigned short *target = data_ov002_0210a760;
    unsigned short *dst = ((ColorRamp *)ramp)->blended;
    int i;
    for (i = 0; i < 0x20; i++) {
        *dst++ = Color_Interp((int)ramp, *src++, *target++, blend);
    }
    func_020555a4(((ColorRamp *)ramp)->blended);
}

/* Fills both halves of the ramp with the grey ramp (i, i, i) and clears
 * DISP3DCNT bits 1, 12 and 13. */
// @symbol func_ov002_020e6ad4
unsigned short *func_ov002_020e6ad4(unsigned short *ramp) {
    unsigned short *dst = ramp;
    unsigned short *copy = ((ColorRamp *)ramp)->blended;
    int i = 0;
    do {
        unsigned short grey = (i * 0x400) | (i | (i * 0x20));
        *dst++ = grey;
        *copy++ = grey;
        i++;
    } while (i < 0x20);
    *(volatile unsigned short *)0x4000060 &= ~0x3002;
    return ramp;
}

/* What the three material helpers below read from a model: its resource
 * at +0x08 (material count at +0x24) and its runtime materials at +0x0c,
 * 0x30 bytes each with TEXIMAGE_PARAM at +0x1c. */
struct ModelMaterialRes {
    char pad[0x24];
    unsigned int numMaterials; /* 0x24 */
};

struct ModelMaterial {
    char pad[0x1c];
    unsigned int texImageParam; /* 0x1c */
    char pad2[0x10];
};

/* Zeroes every material's TEXIMAGE_PARAM (the untextured cap flash). */
// @symbol func_ov002_020e6b3c
void func_ov002_020e6b3c(void *model) {
    void **p = (void **)((char *)model + 8);
    ModelMaterialRes *res = (ModelMaterialRes *)p[0];
    unsigned int n = res->numMaterials;
    ModelMaterial *mat = (ModelMaterial *)p[1];
    unsigned int i;
    for (i = 0; i < n; i++, mat++)
        mat->texImageParam = 0;
}

/* Writes the words func_ov002_020e6bb0 saved back into the materials. */
// @symbol func_ov002_020e6b74
void func_ov002_020e6b74(void *model, int *saved) {
    void **p = (void **)((char *)model + 8);
    ModelMaterialRes *res = (ModelMaterialRes *)p[0];
    unsigned int n = res->numMaterials;
    ModelMaterial *dst = (ModelMaterial *)p[1];
    unsigned int i;
    for (i = 0; i < n; i++, dst++, saved++)
        dst->texImageParam = *saved;
}

/* Allocates one word per material and saves each TEXIMAGE_PARAM into it. */
// @symbol func_ov002_020e6bb0
int *func_ov002_020e6bb0(void *model) {
    void **pp;
    ModelMaterialRes *res;
    unsigned int count;
    int *out;
    int *p;
    ModelMaterial *src;
    unsigned int i;

    pp = (void **)(((long long)(int)((char *)model + 8)));
    res = (ModelMaterialRes *)pp[0];
    count = res->numMaterials;
    out = (int *)Memory::operator_new2(count << 2);

    p = out;
    src = (ModelMaterial *)pp[1];
    for (i = 0; i < count; i++, src++)
        *p++ = src->texImageParam;
    return out;
}
}

/* Static initializer __sinit_ov002_021019d0, placed in .init by define_section.
 * Still written by hand: the objects it builds keep their ROM-owned .bss
 * homes, so a compiler-emitted initializer has nothing to define. */
#pragma define_section initsec ".init" ".init" ".init" far_absolute RX
#pragma section initsec begin
/* Runtime asset handles resolved through the overlay 0 file table. */
enum sinit_ov002_021019d0_AssetId {
    ASSET_B_MARIO_ALL_BMD = 0x04be,
    ASSET_B_MARIO_BODY_BMD = 0x04bf,
    ASSET_B_MARIO_HEAD_CAP_BMD = 0x04c0,
    ASSET_B_MARIO_START_BCA = 0x04c1,
    ASSET_COLD_WAIT_BCA = 0x04c2,
    ASSET_COLD_WAIT_END_BCA = 0x04c3,
    ASSET_COLD_WAIT_START_BCA = 0x04c4,
    ASSET_DAMAGE_START_BCA = 0x04c5,
    ASSET_DAMAGE_WAIT_BCA = 0x04c6,
    ASSET_DOOR_PULL_BCA = 0x04c7,
    ASSET_DOOR_PUSH_BCA = 0x04c8,
    ASSET_DROWN_DIE_BCA = 0x04c9,
    ASSET_ED_CAP_OFF_BCA = 0x04ca,
    ASSET_ED_CAP_OFF_WAIT_BCA = 0x04cb,
    ASSET_ED_FLY_BCA = 0x04cc,
    ASSET_ED_FLY_START_BCA = 0x04cd,
    ASSET_ED_KISS_TO_POSE_BCA = 0x04ce,
    ASSET_ED_KISS_WAIT_BCA = 0x04cf,
    ASSET_ED_LOOK_LEFT_BCA = 0x04d0,
    ASSET_ED_LOOK_LEFT_END_BCA = 0x04d1,
    ASSET_ED_LOOK_LEFT_WAIT_BCA = 0x04d2,
    ASSET_ED_NOD_BCA = 0x04d3,
    ASSET_ED_PUTUP_WAIT_BCA = 0x04d4,
    ASSET_ED_WAIT_BCA = 0x04d5,
    ASSET_ED_WAVE_BCA = 0x04d6,
    ASSET_E_DOWN_BCA = 0x04d7,
    ASSET_FALL_DIE_BCA = 0x04d8,
    ASSET_FALL_SAND_BCA = 0x04d9,
    ASSET_FIRST_CHANGE_BCA = 0x04da,
    ASSET_GAS_DIE_BCA = 0x04db,
    ASSET_GAS_WAIT_BCA = 0x04dc,
    ASSET_H_CARRY_START_BCA = 0x04dd,
    ASSET_H_CARRY_THROW_BCA = 0x04de,
    ASSET_H_CARRY_WAIT_BCA = 0x04df,
    ASSET_H_CARRY_WALK_BCA = 0x04e0,
    ASSET_KEY_GET_BCA = 0x04e1,
    ASSET_KEY_OPEN_BCA = 0x04e2,
    ASSET_LIE_END_BCA = 0x04e3,
    ASSET_LIE_WAIT_BCA = 0x04e4,
    ASSET_L_ED_AMAZED_BCA = 0x04e5,
    ASSET_L_ED_CAP_OFF_BCA = 0x04e6,
    ASSET_L_ED_CAP_OFF_WAIT_BCA = 0x04e7,
    ASSET_L_ED_LOOKUP_WAIT_BCA = 0x04e8,
    ASSET_L_ED_LOOK_MARIO_BCA = 0x04e9,
    ASSET_L_ED_LOOK_M_WAIT_BCA = 0x04ea,
    ASSET_L_ED_WAIT_BCA = 0x04eb,
    ASSET_L_ENTRY_WAIT_BCA = 0x04ec,
    ASSET_L_ENTRY_WAIT_END_BCA = 0x04ed,
    ASSET_L_FIRST_CHANGE_BCA = 0x04ee,
    ASSET_L_LIE_END_BCA = 0x04ef,
    ASSET_L_LIE_WAIT_BCA = 0x04f0,
    ASSET_L_OP_LOOK_WARIO_BCA = 0x04f1,
    ASSET_L_OP_NOD_BCA = 0x04f2,
    ASSET_L_OP_TURN_BCA = 0x04f3,
    ASSET_L_SLEEP_END_BCA = 0x04f4,
    ASSET_L_SLEEP_START_BCA = 0x04f5,
    ASSET_L_SLEEP_TO_LIE_BCA = 0x04f6,
    ASSET_L_SLEEP_WAIT_BCA = 0x04f7,
    ASSET_L_TX_DAMAGE_START_BTP = 0x04f8,
    ASSET_L_TX_DAMAGE_WAIT_BTP = 0x04f9,
    ASSET_L_TX_ED_AMAZED_BTP = 0x04fa,
    ASSET_L_TX_GAS_WAIT_BTP = 0x04fb,
    ASSET_L_TX_LIE_END_BTP = 0x04fc,
    ASSET_L_TX_LIE_WAIT_BTP = 0x04fd,
    ASSET_L_TX_RETURN_DOWN_BTP = 0x04fe,
    ASSET_L_TX_SLEEP_END_BTP = 0x04ff,
    ASSET_L_TX_SLEEP_START_BTP = 0x0500,
    ASSET_L_TX_SLEEP_TO_LIE_BTP = 0x0501,
    ASSET_L_TX_SLEEP_WAIT_BTP = 0x0502,
    ASSET_M_ENTRY_WAIT_BCA = 0x0503,
    ASSET_M_ENTRY_WAIT_END_BCA = 0x0504,
    ASSET_N_CARRY_LOST_BCA = 0x0505,
    ASSET_N_CARRY_POSE_BCA = 0x0506,
    ASSET_N_CARRY_PUT_BCA = 0x0507,
    ASSET_N_CARRY_START_BCA = 0x0508,
    ASSET_N_CARRY_WAIT_BCA = 0x0509,
    ASSET_N_CARRY_WALK_BCA = 0x050a,
    ASSET_OP_GO_BCA = 0x050b,
    ASSET_OP_WAIT_BCA = 0x050c,
    ASSET_PADDLE_1_BCA = 0x050d,
    ASSET_PADDLE_2_BCA = 0x050e,
    ASSET_PADDLE_CARRY_BCA = 0x050f,
    ASSET_PADDLE_END_BCA = 0x0510,
    ASSET_PUTON_T_CAP_BCA = 0x0511,
    ASSET_P_KOOPA_HOLD_BCA = 0x0512,
    ASSET_P_KOOPA_H_WAIT_BCA = 0x0513,
    ASSET_P_KOOPA_THROW_BCA = 0x0514,
    ASSET_RETURN_DOWN_BCA = 0x0515,
    ASSET_RETURN_NOCAP_BCA = 0x0516,
    ASSET_RETURN_STAR_BCA = 0x0517,
    ASSET_SCALE_UP_BCA = 0x0518,
    ASSET_SDROP_HEAD_BCA = 0x0519,
    ASSET_SDROP_HEAD_END_BCA = 0x051a,
    ASSET_SDROP_HEAD_WAIT_BCA = 0x051b,
    ASSET_SDROP_HIP_BCA = 0x051c,
    ASSET_SDROP_HIP_END_BCA = 0x051d,
    ASSET_SDROP_HIP_WAIT_BCA = 0x051e,
    ASSET_SDROP_LEG_BCA = 0x051f,
    ASSET_SDROP_LEG_END_BCA = 0x0520,
    ASSET_SDROP_LEG_WAIT_BCA = 0x0521,
    ASSET_SDROP_WAIT_BCA = 0x0522,
    ASSET_SDROP_WALK_BCA = 0x0523,
    ASSET_SHF_DIE_BCA = 0x0524,
    ASSET_SHOOT_BCA = 0x0525,
    ASSET_SHOOT_TO_FLY_BCA = 0x0526,
    ASSET_SLEEP_END_BCA = 0x0527,
    ASSET_SLEEP_START_BCA = 0x0528,
    ASSET_SLEEP_TO_LIE_BCA = 0x0529,
    ASSET_SLEEP_WAIT_BCA = 0x052a,
    ASSET_STAR_OPEN_BCA = 0x052b,
    ASSET_SU_HIPAT_BCA = 0x052c,
    ASSET_SU_HIPED_BCA = 0x052d,
    ASSET_SU_HIPSR_BCA = 0x052e,
    ASSET_SU_JUMP_BCA = 0x052f,
    ASSET_SU_JUMP_END_BCA = 0x0530,
    ASSET_SU_RUN_BCA = 0x0531,
    ASSET_SU_WAIT_BCA = 0x0532,
    ASSET_SU_WALK_BCA = 0x0533,
    ASSET_SU_WALK_END_BCA = 0x0534,
    ASSET_SU_WALK_START_BCA = 0x0535,
    ASSET_SWIM_BCA = 0x0536,
    ASSET_SWIM_ATTACK_BCA = 0x0537,
    ASSET_SWIM_CARRY_BCA = 0x0538,
    ASSET_SWIM_DAMAGE_BCA = 0x0539,
    ASSET_SWIM_DIE_BCA = 0x053a,
    ASSET_SWIM_IN_BCA = 0x053b,
    ASSET_SWIM_STAR_GET_BCA = 0x053c,
    ASSET_SWIM_THROW_BCA = 0x053d,
    ASSET_SWIM_WAIT_BCA = 0x053e,
    ASSET_S_DIE_BCA = 0x053f,
    ASSET_TX_DAMAGE_START_BTP = 0x0540,
    ASSET_TX_DAMAGE_WAIT_BTP = 0x0541,
    ASSET_TX_ED_KISS_TO_POSE_BTP = 0x0542,
    ASSET_TX_ED_KISS_WAIT_BTP = 0x0543,
    ASSET_TX_GAS_WAIT_BTP = 0x0544,
    ASSET_TX_LIE_END_BTP = 0x0545,
    ASSET_TX_LIE_WAIT_BTP = 0x0546,
    ASSET_TX_RETURN_DOWN_BTP = 0x0547,
    ASSET_TX_SLEEP_END_BTP = 0x0548,
    ASSET_TX_SLEEP_START_BTP = 0x0549,
    ASSET_TX_SLEEP_TO_LIE_BTP = 0x054a,
    ASSET_TX_SLEEP_WAIT_BTP = 0x054b,
    ASSET_WARIO_METAL_MODEL_BMD = 0x054c,
    ASSET_W_ED_AMAZED_WAIR_BCA = 0x054d,
    ASSET_W_ED_CAP_OFF_BCA = 0x054e,
    ASSET_W_ED_CAP_OFF_WAIT_BCA = 0x054f,
    ASSET_W_ED_LOOKUP_WAIT_BCA = 0x0550,
    ASSET_W_ED_WAIT_BCA = 0x0551,
    ASSET_W_ED_WAVE_BCA = 0x0552,
    ASSET_W_ENTRY_WAIT_BCA = 0x0553,
    ASSET_W_ENTRY_WAIT_END_BCA = 0x0554,
    ASSET_W_FIRST_CHANGE_BCA = 0x0555,
    ASSET_W_KEY_GET_BCA = 0x0556,
    ASSET_W_LIE_END_BCA = 0x0557,
    ASSET_W_LIE_WAIT_BCA = 0x0558,
    ASSET_W_OP_LOOK_LUIGI_BCA = 0x0559,
    ASSET_W_OP_NOD_BCA = 0x055a,
    ASSET_W_OP_POINT_BCA = 0x055b,
    ASSET_W_OP_TURN_BCA = 0x055c,
    ASSET_W_SLEEP_END_BCA = 0x055d,
    ASSET_W_SLEEP_START_BCA = 0x055e,
    ASSET_W_SLEEP_TO_LIE_BCA = 0x055f,
    ASSET_W_SLEEP_WAIT_BCA = 0x0560,
    ASSET_W_TX_DAMAGE_START_BTP = 0x0561,
    ASSET_W_TX_DAMAGE_WAIT_BTP = 0x0562,
    ASSET_W_TX_ED_AMAZED_BTP = 0x0563,
    ASSET_W_TX_GAS_WAIT_BTP = 0x0564,
    ASSET_W_TX_LIE_END_BTP = 0x0565,
    ASSET_W_TX_LIE_WAIT_BTP = 0x0566,
    ASSET_W_TX_RETURN_DOWN_BTP = 0x0567,
    ASSET_W_TX_SLEEP_END_BTP = 0x0568,
    ASSET_W_TX_SLEEP_START_BTP = 0x0569,
    ASSET_W_TX_SLEEP_TO_LIE_BTP = 0x056a,
    ASSET_W_TX_SLEEP_WAIT_BTP = 0x056b,
    ASSET_Y_ED_LOOKUP_WAIT_BCA = 0x056c,
    ASSET_Y_ED_LOOK_CAM_BCA = 0x056d,
    ASSET_Y_ED_SHY_TO_WAIT_BCA = 0x056e,
    ASSET_Y_ED_SHY_WAIT_BCA = 0x056f,
    ASSET_Y_ED_TURN_BCA = 0x0570,
    ASSET_Y_ED_WAIT_BCA = 0x0571,
    ASSET_Y_EGG_RUN_BCA = 0x0572,
    ASSET_Y_EGG_WAIT_BCA = 0x0573,
    ASSET_Y_LIE_END_BCA = 0x0574,
    ASSET_Y_LIE_WAIT_BCA = 0x0575,
    ASSET_Y_OP_LIE_TO_SIT_BCA = 0x0576,
    ASSET_Y_OP_SIT_TO_STAND_BCA = 0x0577,
    ASSET_Y_OP_SIT_WAIT_BCA = 0x0578,
    ASSET_Y_OP_STAND_WAIT_BCA = 0x0579,
    ASSET_Y_SLEEP_END_BCA = 0x057a,
    ASSET_Y_SLEEP_START_BCA = 0x057b,
    ASSET_Y_SLEEP_TO_LIE_BCA = 0x057c,
    ASSET_Y_SLEEP_WAIT_BCA = 0x057d,
    ASSET_Y_TX_DAMAGE_START_BTP = 0x057e,
    ASSET_Y_TX_DAMAGE_WAIT_BTP = 0x057f,
    ASSET_Y_TX_GAS_WAIT_BTP = 0x0580,
    ASSET_Y_TX_LIE_END_BTP = 0x0581,
    ASSET_Y_TX_LIE_WAIT_BTP = 0x0582,
    ASSET_Y_TX_RETURN_DOWN_BTP = 0x0583,
    ASSET_Y_TX_SLEEP_END_BTP = 0x0584,
    ASSET_Y_TX_SLEEP_START_BTP = 0x0585,
    ASSET_Y_TX_SLEEP_TO_LIE_BTP = 0x0586
};

// @symbol __sinit_ov002_021019d0
extern "C" {
void func_0203b9b4(int *p, int v);
/* __register_global_object */
void func_020731dc(void *object, void *destructor, void **node);
void *func_02017acc(void *self, unsigned int fileID);
int func_02017ab4(int self);
int SharedFilePtr_Destruct_Anim(int self);
void *SharedFilePtr_Construct_TexSeq(void *self, unsigned int fileID);
int SharedFilePtr_Destruct_TexSeq(int self);
void func_0201973c(void);
unsigned short *func_ov002_020e6ad4(unsigned short *);

/* The random-number seed RandomIntInternal draws from. */
extern int data_ov002_0210e160;

/* Model, animation and texture-sequence file handles, by address. */
extern SharedFilePtr data_ov002_0210e168;
extern SharedFilePtr data_ov002_0210e170;
extern SharedFilePtr data_ov002_0210e178;
extern SharedFilePtr data_ov002_0210e180;
extern SharedFilePtr data_ov002_0210e188;
extern SharedFilePtr data_ov002_0210e190;
extern SharedFilePtr data_ov002_0210e198;
extern SharedFilePtr data_ov002_0210e1a0;
extern SharedFilePtr data_ov002_0210e1a8;
extern SharedFilePtr data_ov002_0210e1b0;
extern SharedFilePtr data_ov002_0210e1b8;
extern SharedFilePtr data_ov002_0210e1c0;
extern SharedFilePtr data_ov002_0210e1c8;
extern SharedFilePtr data_ov002_0210e1d0;
extern SharedFilePtr data_ov002_0210e1d8;
extern SharedFilePtr data_ov002_0210e1e0;
extern SharedFilePtr data_ov002_0210e1e8;
extern SharedFilePtr data_ov002_0210e1f0;
extern SharedFilePtr data_ov002_0210e1f8;
extern SharedFilePtr data_ov002_0210e200;
extern SharedFilePtr data_ov002_0210e208;
extern SharedFilePtr data_ov002_0210e210;
extern SharedFilePtr data_ov002_0210e218;
extern SharedFilePtr data_ov002_0210e220;
extern SharedFilePtr data_ov002_0210e228;
extern SharedFilePtr data_ov002_0210e230;
extern SharedFilePtr data_ov002_0210e238;
extern SharedFilePtr data_ov002_0210e240;
extern SharedFilePtr data_ov002_0210e248;
extern SharedFilePtr data_ov002_0210e250;
extern SharedFilePtr data_ov002_0210e258;
extern SharedFilePtr data_ov002_0210e260;
extern SharedFilePtr data_ov002_0210e268;
extern SharedFilePtr data_ov002_0210e270;
extern SharedFilePtr data_ov002_0210e278;
extern SharedFilePtr data_ov002_0210e280;
extern SharedFilePtr data_ov002_0210e288;
extern SharedFilePtr data_ov002_0210e290;
extern SharedFilePtr data_ov002_0210e298;
extern SharedFilePtr data_ov002_0210e2a0;
extern SharedFilePtr data_ov002_0210e2a8;
extern SharedFilePtr data_ov002_0210e2b0;
extern SharedFilePtr data_ov002_0210e2b8;
extern SharedFilePtr data_ov002_0210e2c0;
extern SharedFilePtr data_ov002_0210e2c8;
extern SharedFilePtr data_ov002_0210e2d0;
extern SharedFilePtr data_ov002_0210e2d8;
extern SharedFilePtr data_ov002_0210e2e0;
extern SharedFilePtr data_ov002_0210e2e8;
extern SharedFilePtr data_ov002_0210e2f0;
extern SharedFilePtr data_ov002_0210e2f8;
extern SharedFilePtr data_ov002_0210e300;
extern SharedFilePtr data_ov002_0210e308;
extern SharedFilePtr data_ov002_0210e310;
extern SharedFilePtr data_ov002_0210e318;
extern SharedFilePtr data_ov002_0210e320;
extern SharedFilePtr data_ov002_0210e328;
extern SharedFilePtr data_ov002_0210e330;
extern SharedFilePtr data_ov002_0210e338;
extern SharedFilePtr data_ov002_0210e340;
extern SharedFilePtr data_ov002_0210e348;
extern SharedFilePtr data_ov002_0210e350;
extern SharedFilePtr data_ov002_0210e358;
extern SharedFilePtr data_ov002_0210e360;
extern SharedFilePtr data_ov002_0210e368;
extern SharedFilePtr data_ov002_0210e370;
extern SharedFilePtr data_ov002_0210e378;
extern SharedFilePtr data_ov002_0210e380;
extern SharedFilePtr data_ov002_0210e388;
extern SharedFilePtr data_ov002_0210e390;
extern SharedFilePtr data_ov002_0210e398;
extern SharedFilePtr data_ov002_0210e3a0;
extern SharedFilePtr data_ov002_0210e3a8;
extern SharedFilePtr data_ov002_0210e3b0;
extern SharedFilePtr data_ov002_0210e3b8;
extern SharedFilePtr data_ov002_0210e3c0;
extern SharedFilePtr data_ov002_0210e3c8;
extern SharedFilePtr data_ov002_0210e3d0;
extern SharedFilePtr data_ov002_0210e3d8;
extern SharedFilePtr data_ov002_0210e3e0;
extern SharedFilePtr data_ov002_0210e3e8;
extern SharedFilePtr data_ov002_0210e3f0;
extern SharedFilePtr data_ov002_0210e3f8;
extern SharedFilePtr data_ov002_0210e400;
extern SharedFilePtr data_ov002_0210e408;
extern SharedFilePtr data_ov002_0210e410;
extern SharedFilePtr data_ov002_0210e418;
extern SharedFilePtr data_ov002_0210e420;
extern SharedFilePtr data_ov002_0210e428;
extern SharedFilePtr data_ov002_0210e430;
extern SharedFilePtr data_ov002_0210e438;
extern SharedFilePtr data_ov002_0210e440;
extern SharedFilePtr data_ov002_0210e448;
extern SharedFilePtr data_ov002_0210e450;
extern SharedFilePtr data_ov002_0210e458;
extern SharedFilePtr data_ov002_0210e460;
extern SharedFilePtr data_ov002_0210e468;
extern SharedFilePtr data_ov002_0210e470;
extern SharedFilePtr data_ov002_0210e478;
extern SharedFilePtr data_ov002_0210e480;
extern SharedFilePtr data_ov002_0210e488;
extern SharedFilePtr data_ov002_0210e490;
extern SharedFilePtr data_ov002_0210e498;
extern SharedFilePtr data_ov002_0210e4a0;
extern SharedFilePtr data_ov002_0210e4a8;
extern SharedFilePtr data_ov002_0210e4b0;
extern SharedFilePtr data_ov002_0210e4b8;
extern SharedFilePtr data_ov002_0210e4c0;
extern SharedFilePtr data_ov002_0210e4c8;
extern SharedFilePtr data_ov002_0210e4d0;
extern SharedFilePtr data_ov002_0210e4d8;
extern SharedFilePtr data_ov002_0210e4e0;
extern SharedFilePtr data_ov002_0210e4e8;
extern SharedFilePtr data_ov002_0210e4f0;
extern SharedFilePtr data_ov002_0210e4f8;
extern SharedFilePtr data_ov002_0210e500;
extern SharedFilePtr data_ov002_0210e508;
extern SharedFilePtr data_ov002_0210e510;
extern SharedFilePtr data_ov002_0210e518;
extern SharedFilePtr data_ov002_0210e520;
extern SharedFilePtr data_ov002_0210e528;
extern SharedFilePtr data_ov002_0210e530;
extern SharedFilePtr data_ov002_0210e538;
extern SharedFilePtr data_ov002_0210e540;
extern SharedFilePtr data_ov002_0210e548;
extern SharedFilePtr data_ov002_0210e550;
extern SharedFilePtr data_ov002_0210e558;
extern SharedFilePtr data_ov002_0210e560;
extern SharedFilePtr data_ov002_0210e568;
extern SharedFilePtr data_ov002_0210e570;
extern SharedFilePtr data_ov002_0210e578;
extern SharedFilePtr data_ov002_0210e580;
extern SharedFilePtr data_ov002_0210e588;
extern SharedFilePtr data_ov002_0210e590;
extern SharedFilePtr data_ov002_0210e598;
extern SharedFilePtr data_ov002_0210e5a0;
extern SharedFilePtr data_ov002_0210e5a8;
extern SharedFilePtr data_ov002_0210e5b0;
extern SharedFilePtr data_ov002_0210e5b8;
extern SharedFilePtr data_ov002_0210e5c0;
extern SharedFilePtr data_ov002_0210e5c8;
extern SharedFilePtr data_ov002_0210e5d0;
extern SharedFilePtr data_ov002_0210e5d8;
extern SharedFilePtr data_ov002_0210e5e0;
extern SharedFilePtr data_ov002_0210e5e8;
extern SharedFilePtr data_ov002_0210e5f0;
extern SharedFilePtr data_ov002_0210e5f8;
extern SharedFilePtr data_ov002_0210e600;
extern SharedFilePtr data_ov002_0210e608;
extern SharedFilePtr data_ov002_0210e610;
extern SharedFilePtr data_ov002_0210e618;
extern SharedFilePtr data_ov002_0210e620;
extern SharedFilePtr data_ov002_0210e628;
extern SharedFilePtr data_ov002_0210e630;
extern SharedFilePtr data_ov002_0210e638;
extern SharedFilePtr data_ov002_0210e640;
extern SharedFilePtr data_ov002_0210e648;
extern SharedFilePtr data_ov002_0210e650;
extern SharedFilePtr data_ov002_0210e658;
extern SharedFilePtr data_ov002_0210e660;
extern SharedFilePtr data_ov002_0210e668;
extern SharedFilePtr data_ov002_0210e670;
extern SharedFilePtr data_ov002_0210e678;
extern SharedFilePtr data_ov002_0210e680;
extern SharedFilePtr data_ov002_0210e688;
extern SharedFilePtr data_ov002_0210e690;
extern SharedFilePtr data_ov002_0210e698;
extern SharedFilePtr data_ov002_0210e6a0;
extern SharedFilePtr data_ov002_0210e6a8;
extern SharedFilePtr data_ov002_0210e6b0;
extern SharedFilePtr data_ov002_0210e6b8;
extern SharedFilePtr data_ov002_0210e6c0;
extern SharedFilePtr data_ov002_0210e6c8;
extern SharedFilePtr data_ov002_0210e6d0;
extern SharedFilePtr data_ov002_0210e6d8;
extern SharedFilePtr data_ov002_0210e6e0;
extern SharedFilePtr data_ov002_0210e6e8;
extern SharedFilePtr data_ov002_0210e6f0;
extern SharedFilePtr data_ov002_0210e6f8;
extern SharedFilePtr data_ov002_0210e700;
extern SharedFilePtr data_ov002_0210e708;
extern SharedFilePtr data_ov002_0210e710;
extern SharedFilePtr data_ov002_0210e718;
extern SharedFilePtr data_ov002_0210e720;
extern SharedFilePtr data_ov002_0210e728;
extern SharedFilePtr data_ov002_0210e730;
extern SharedFilePtr data_ov002_0210e738;
extern SharedFilePtr data_ov002_0210e740;
extern SharedFilePtr data_ov002_0210e748;
extern SharedFilePtr data_ov002_0210e750;
extern SharedFilePtr data_ov002_0210e758;
extern SharedFilePtr data_ov002_0210e760;
extern SharedFilePtr data_ov002_0210e768;
extern SharedFilePtr data_ov002_0210e770;
extern SharedFilePtr data_ov002_0210e778;
extern SharedFilePtr data_ov002_0210e780;
extern SharedFilePtr data_ov002_0210e788;
extern SharedFilePtr data_ov002_0210e790;
extern SharedFilePtr data_ov002_0210e798;
extern SharedFilePtr data_ov002_0210e7a0;
extern SharedFilePtr data_ov002_0210e7a8;
extern SharedFilePtr data_ov002_0210e7b0;
extern SharedFilePtr data_ov002_0210e7c0;
extern SharedFilePtr data_ov002_0210e7c8;
extern SharedFilePtr data_ov002_0210e7d0;
extern SharedFilePtr data_ov002_0210e7d8;
extern SharedFilePtr data_ov002_0210e7e0;
extern SharedFilePtr data_ov002_0210e7e8;
extern SharedFilePtr data_ov002_0210e7f0;
extern SharedFilePtr data_ov002_0210e7f8;
extern SharedFilePtr data_ov002_0210e800;
extern SharedFilePtr data_ov002_0210e808;
extern SharedFilePtr data_ov002_0210e810;
extern SharedFilePtr data_ov002_0210e818;
extern SharedFilePtr data_ov002_0210e820;
extern SharedFilePtr data_ov002_0210e828;
extern SharedFilePtr data_ov002_0210e830;
extern SharedFilePtr data_ov002_0210e838;
extern SharedFilePtr data_ov002_0210e840;
extern SharedFilePtr data_ov002_0210e848;
extern SharedFilePtr data_ov002_0210e850;
extern SharedFilePtr data_ov002_0210e858;
extern SharedFilePtr data_ov002_0210e860;
extern SharedFilePtr data_ov002_0210e868;
extern SharedFilePtr data_ov002_0210e870;
extern SharedFilePtr data_ov002_0210e878;
extern SharedFilePtr data_ov002_0210e880;
extern SharedFilePtr data_ov002_0210e888;
extern SharedFilePtr data_ov002_0210e890;
extern SharedFilePtr data_ov002_0210e898;
extern SharedFilePtr data_ov002_0210e8a0;
extern SharedFilePtr data_ov002_0210e8a8;
extern SharedFilePtr data_ov002_0210e8b0;
extern SharedFilePtr data_ov002_0210e8b8;
extern SharedFilePtr data_ov002_0210e8c0;
extern SharedFilePtr data_ov002_0210e8c8;
extern SharedFilePtr data_ov002_0210e8d0;
extern SharedFilePtr data_ov002_0210e8d8;
extern SharedFilePtr data_ov002_0210e8e0;
extern SharedFilePtr data_ov002_0210e8e8;
extern SharedFilePtr data_ov002_0210e8f0;
extern SharedFilePtr data_ov002_0210e8f8;
extern SharedFilePtr data_ov002_0210e900;
extern SharedFilePtr data_ov002_0210e908;
extern SharedFilePtr data_ov002_0210e910;
extern SharedFilePtr data_ov002_0210e918;
extern SharedFilePtr data_ov002_0210e920;
extern SharedFilePtr data_ov002_0210e928;
extern SharedFilePtr data_ov002_0210e930;
extern SharedFilePtr data_ov002_0210e938;
extern SharedFilePtr data_ov002_0210e940;
extern SharedFilePtr data_ov002_0210e948;
extern SharedFilePtr data_ov002_0210e950;
extern SharedFilePtr data_ov002_0210e958;
extern SharedFilePtr data_ov002_0210e960;
extern SharedFilePtr data_ov002_0210e968;
extern SharedFilePtr data_ov002_0210e970;
extern SharedFilePtr data_ov002_0210e978;
extern SharedFilePtr data_ov002_0210e980;
extern SharedFilePtr data_ov002_0210e988;
extern SharedFilePtr data_ov002_0210e990;
extern SharedFilePtr data_ov002_0210e998;
extern SharedFilePtr data_ov002_0210e9a0;
extern SharedFilePtr data_ov002_0210e9a8;
extern SharedFilePtr data_ov002_0210e9b0;
extern SharedFilePtr data_ov002_0210e9b8;
extern SharedFilePtr data_ov002_0210e9c0;
extern SharedFilePtr data_ov002_0210e9c8;
extern SharedFilePtr data_ov002_0210e9d0;
extern SharedFilePtr data_ov002_0210e9d8;
extern SharedFilePtr data_ov002_0210e9e0;
extern SharedFilePtr data_ov002_0210e9e8;
extern SharedFilePtr data_ov002_0210e9f0;
extern SharedFilePtr data_ov002_0210e9f8;
extern SharedFilePtr data_ov002_0210ea00;
extern SharedFilePtr data_ov002_0210ea08;
extern SharedFilePtr data_ov002_0210ea10;
extern SharedFilePtr data_ov002_0210ea18;
extern SharedFilePtr data_ov002_0210ea20;
extern SharedFilePtr data_ov002_0210ea28;
extern SharedFilePtr data_ov002_0210ea30;
extern SharedFilePtr data_ov002_0210ea38;
extern SharedFilePtr data_ov002_0210ea40;
extern SharedFilePtr data_ov002_0210ea48;
extern SharedFilePtr data_ov002_0210ea50;
extern SharedFilePtr data_ov002_0210ea58;
extern SharedFilePtr data_ov002_0210ea60;
extern SharedFilePtr data_ov002_0210ea68;
extern SharedFilePtr data_ov002_0210ea70;
extern SharedFilePtr data_ov002_0210ea78;
extern SharedFilePtr data_ov002_0210ea80;
extern SharedFilePtr data_ov002_0210ea88;
extern SharedFilePtr data_ov002_0210ea90;
extern SharedFilePtr data_ov002_0210ea98;
extern SharedFilePtr data_ov002_0210eaa0;
extern SharedFilePtr data_ov002_0210eaa8;
extern SharedFilePtr data_ov002_0210eab0;
extern SharedFilePtr data_ov002_0210eab8;
extern SharedFilePtr data_ov002_0210eac0;
extern SharedFilePtr data_ov002_0210eac8;
extern SharedFilePtr data_ov002_0210ead0;
extern SharedFilePtr data_ov002_0210ead8;
extern SharedFilePtr data_ov002_0210eae0;
extern SharedFilePtr data_ov002_0210eae8;
extern SharedFilePtr data_ov002_0210eaf0;
extern SharedFilePtr data_ov002_0210eaf8;
extern SharedFilePtr data_ov002_0210eb00;
extern SharedFilePtr data_ov002_0210eb08;
extern SharedFilePtr data_ov002_0210eb10;
extern SharedFilePtr data_ov002_0210eb18;
extern SharedFilePtr data_ov002_0210eb20;
extern SharedFilePtr data_ov002_0210eb28;
extern SharedFilePtr data_ov002_0210eb30;
extern SharedFilePtr data_ov002_0210eb38;
extern SharedFilePtr data_ov002_0210eb40;
extern SharedFilePtr data_ov002_0210eb48;
extern SharedFilePtr data_ov002_0210eb50;
extern SharedFilePtr data_ov002_0210eb58;
extern SharedFilePtr data_ov002_0210eb60;
extern SharedFilePtr data_ov002_0210eb68;
extern SharedFilePtr data_ov002_0210eb70;
extern SharedFilePtr data_ov002_0210eb78;
extern SharedFilePtr data_ov002_0210eb80;
extern SharedFilePtr data_ov002_0210eb88;
extern SharedFilePtr data_ov002_0210eb90;
extern SharedFilePtr data_ov002_0210eb98;
extern SharedFilePtr data_ov002_0210eba0;
extern SharedFilePtr data_ov002_0210eba8;
extern SharedFilePtr data_ov002_0210ebb0;
extern SharedFilePtr data_ov002_0210ebb8;
extern SharedFilePtr data_ov002_0210ebc0;
extern SharedFilePtr data_ov002_0210ebc8;
extern SharedFilePtr data_ov002_0210ebd0;
extern SharedFilePtr data_ov002_0210ebd8;
extern SharedFilePtr data_ov002_0210ebe0;
extern SharedFilePtr data_ov002_0210ebe8;
extern SharedFilePtr data_ov002_0210ebf0;
extern SharedFilePtr data_ov002_0210ebf8;
extern SharedFilePtr data_ov002_0210ec00;
extern SharedFilePtr data_ov002_0210ec08;
extern SharedFilePtr data_ov002_0210ec10;
extern SharedFilePtr data_ov002_0210ec18;
extern SharedFilePtr data_ov002_0210ec20;
extern SharedFilePtr data_ov002_0210ec28;
extern SharedFilePtr data_ov002_0210ec30;
extern SharedFilePtr data_ov002_0210ec38;
extern SharedFilePtr data_ov002_0210ec40;
extern SharedFilePtr data_ov002_0210ec48;
extern SharedFilePtr data_ov002_0210ec50;
extern SharedFilePtr data_ov002_0210ec58;
extern SharedFilePtr data_ov002_0210ec60;
extern SharedFilePtr data_ov002_0210ec68;
extern SharedFilePtr data_ov002_0210ec70;
extern SharedFilePtr data_ov002_0210ec78;
extern SharedFilePtr data_ov002_0210ec80;
extern SharedFilePtr data_ov002_0210ec88;
extern SharedFilePtr data_ov002_0210ec90;
extern SharedFilePtr data_ov002_0210ec98;
extern SharedFilePtr data_ov002_0210eca0;
extern SharedFilePtr data_ov002_0210eca8;
extern SharedFilePtr data_ov002_0210ecb0;
extern SharedFilePtr data_ov002_0210ecb8;
extern SharedFilePtr data_ov002_0210ecc0;
extern SharedFilePtr data_ov002_0210ecc8;
extern SharedFilePtr data_ov002_0210ecd0;
extern SharedFilePtr data_ov002_0210ecd8;
extern SharedFilePtr data_ov002_0210ece0;
extern SharedFilePtr data_ov002_0210ece8;
extern SharedFilePtr data_ov002_0210ecf0;
extern SharedFilePtr data_ov002_0210ecf8;
extern SharedFilePtr data_ov002_0210ed00;
extern SharedFilePtr data_ov002_0210ed08;
extern SharedFilePtr data_ov002_0210ed10;
extern SharedFilePtr data_ov002_0210ed18;
extern SharedFilePtr data_ov002_0210ed20;
extern SharedFilePtr data_ov002_0210ed28;
extern SharedFilePtr data_ov002_0210ed30;
extern SharedFilePtr data_ov002_0210ed38;
extern SharedFilePtr data_ov002_0210ed40;
extern SharedFilePtr data_ov002_0210ed48;
extern SharedFilePtr data_ov002_0210ed50;
extern SharedFilePtr data_ov002_0210ed58;
extern SharedFilePtr data_ov002_0210ed60;

/* __register_global_object chain nodes, one per handle. */
extern void *data_ov002_0210ed68[3];
extern void *data_ov002_0210ed74[3];
extern void *data_ov002_0210ed80[3];
extern void *data_ov002_0210ed8c[3];
extern void *data_ov002_0210ed98[3];
extern void *data_ov002_0210eda4[3];
extern void *data_ov002_0210edb0[3];
extern void *data_ov002_0210edbc[3];
extern void *data_ov002_0210edc8[3];
extern void *data_ov002_0210edd4[3];
extern void *data_ov002_0210ede0[3];
extern void *data_ov002_0210edec[3];
extern void *data_ov002_0210edf8[3];
extern void *data_ov002_0210ee04[3];
extern void *data_ov002_0210ee10[3];
extern void *data_ov002_0210ee1c[3];
extern void *data_ov002_0210ee28[3];
extern void *data_ov002_0210ee34[3];
extern void *data_ov002_0210ee40[3];
extern void *data_ov002_0210ee4c[3];
extern void *data_ov002_0210ee58[3];
extern void *data_ov002_0210ee64[3];
extern void *data_ov002_0210ee70[3];
extern void *data_ov002_0210ee7c[3];
extern void *data_ov002_0210ee88[3];
extern void *data_ov002_0210ee94[3];
extern void *data_ov002_0210eea0[3];
extern void *data_ov002_0210eeac[3];
extern void *data_ov002_0210eeb8[3];
extern void *data_ov002_0210eec4[3];
extern void *data_ov002_0210eed0[3];
extern void *data_ov002_0210eedc[3];
extern void *data_ov002_0210eee8[3];
extern void *data_ov002_0210eef4[3];
extern void *data_ov002_0210ef00[3];
extern void *data_ov002_0210ef0c[3];
extern void *data_ov002_0210ef18[3];
extern void *data_ov002_0210ef24[3];
extern void *data_ov002_0210ef30[3];
extern void *data_ov002_0210ef3c[3];
extern void *data_ov002_0210ef48[3];
extern void *data_ov002_0210ef54[3];
extern void *data_ov002_0210ef60[3];
extern void *data_ov002_0210ef6c[3];
extern void *data_ov002_0210ef78[3];
extern void *data_ov002_0210ef84[3];
extern void *data_ov002_0210ef90[3];
extern void *data_ov002_0210ef9c[3];
extern void *data_ov002_0210efa8[3];
extern void *data_ov002_0210efb4[3];
extern void *data_ov002_0210efc0[3];
extern void *data_ov002_0210efcc[3];
extern void *data_ov002_0210efd8[3];
extern void *data_ov002_0210efe4[3];
extern void *data_ov002_0210eff0[3];
extern void *data_ov002_0210effc[3];
extern void *data_ov002_0210f008[3];
extern void *data_ov002_0210f014[3];
extern void *data_ov002_0210f020[3];
extern void *data_ov002_0210f02c[3];
extern void *data_ov002_0210f038[3];
extern void *data_ov002_0210f044[3];
extern void *data_ov002_0210f050[3];
extern void *data_ov002_0210f05c[3];
extern void *data_ov002_0210f068[3];
extern void *data_ov002_0210f074[3];
extern void *data_ov002_0210f080[3];
extern void *data_ov002_0210f08c[3];
extern void *data_ov002_0210f098[3];
extern void *data_ov002_0210f0a4[3];
extern void *data_ov002_0210f0b0[3];
extern void *data_ov002_0210f0bc[3];
extern void *data_ov002_0210f0c8[3];
extern void *data_ov002_0210f0d4[3];
extern void *data_ov002_0210f0e0[3];
extern void *data_ov002_0210f0ec[3];
extern void *data_ov002_0210f0f8[3];
extern void *data_ov002_0210f104[3];
extern void *data_ov002_0210f110[3];
extern void *data_ov002_0210f11c[3];
extern void *data_ov002_0210f128[3];
extern void *data_ov002_0210f134[3];
extern void *data_ov002_0210f140[3];
extern void *data_ov002_0210f14c[3];
extern void *data_ov002_0210f158[3];
extern void *data_ov002_0210f164[3];
extern void *data_ov002_0210f170[3];
extern void *data_ov002_0210f17c[3];
extern void *data_ov002_0210f188[3];
extern void *data_ov002_0210f194[3];
extern void *data_ov002_0210f1a0[3];
extern void *data_ov002_0210f1b8[3];
extern void *data_ov002_0210f1c4[3];
extern void *data_ov002_0210f1d0[3];
extern void *data_ov002_0210f1dc[3];
extern void *data_ov002_0210f1e8[3];
extern void *data_ov002_0210f1f4[3];
extern void *data_ov002_0210f200[3];
extern void *data_ov002_0210f20c[3];
extern void *data_ov002_0210f218[3];
extern void *data_ov002_0210f230[3];
extern void *data_ov002_0210f23c[3];
extern void *data_ov002_0210f248[3];
extern void *data_ov002_0210f254[3];
extern void *data_ov002_0210f260[3];
extern void *data_ov002_0210f26c[3];
extern void *data_ov002_0210f278[3];
extern void *data_ov002_0210f284[3];
extern void *data_ov002_0210f290[3];
extern void *data_ov002_0210f29c[3];
extern void *data_ov002_0210f2a8[3];
extern void *data_ov002_0210f2b4[3];
extern void *data_ov002_0210f2c0[3];
extern void *data_ov002_0210f2cc[3];
extern void *data_ov002_0210f2d8[3];
extern void *data_ov002_0210f2e4[3];
extern void *data_ov002_0210f2f0[3];
extern void *data_ov002_0210f2fc[3];
extern void *data_ov002_0210f314[3];
extern void *data_ov002_0210f320[3];
extern void *data_ov002_0210f32c[3];
extern void *data_ov002_0210f338[3];
extern void *data_ov002_0210f35c[3];
extern void *data_ov002_0210f368[3];
extern void *data_ov002_0210f374[3];
extern void *data_ov002_0210f380[3];
extern void *data_ov002_0210f38c[3];
extern void *data_ov002_0210f398[3];
extern void *data_ov002_0210f3a4[3];
extern void *data_ov002_0210f3bc[3];
extern void *data_ov002_0210f3c8[3];
extern void *data_ov002_0210f3d4[3];
extern void *data_ov002_0210f3e0[3];
extern void *data_ov002_0210f3ec[3];
extern void *data_ov002_0210f3f8[3];
extern void *data_ov002_0210f404[3];
extern void *data_ov002_0210f410[3];
extern void *data_ov002_0210f41c[3];
extern void *data_ov002_0210f428[3];
extern void *data_ov002_0210f434[3];
extern void *data_ov002_0210f440[3];
extern void *data_ov002_0210f44c[3];
extern void *data_ov002_0210f458[3];
extern void *data_ov002_0210f464[3];
extern void *data_ov002_0210f470[3];
extern void *data_ov002_0210f47c[3];
extern void *data_ov002_0210f488[3];
extern void *data_ov002_0210f494[3];
extern void *data_ov002_0210f4a0[3];
extern void *data_ov002_0210f4ac[3];
extern void *data_ov002_0210f4b8[3];
extern void *data_ov002_0210f4c4[3];
extern void *data_ov002_0210f4d0[3];
extern void *data_ov002_0210f4dc[3];
extern void *data_ov002_0210f4e8[3];
extern void *data_ov002_0210f4f4[3];
extern void *data_ov002_0210f500[3];
extern void *data_ov002_0210f50c[3];
extern void *data_ov002_0210f518[3];
extern void *data_ov002_0210f524[3];
extern void *data_ov002_0210f530[3];
extern void *data_ov002_0210f53c[3];
extern void *data_ov002_0210f548[3];
extern void *data_ov002_0210f554[3];
extern void *data_ov002_0210f560[3];
extern void *data_ov002_0210f56c[3];
extern void *data_ov002_0210f578[3];
extern void *data_ov002_0210f584[3];
extern void *data_ov002_0210f590[3];
extern void *data_ov002_0210f59c[3];
extern void *data_ov002_0210f5a8[3];
extern void *data_ov002_0210f5b4[3];
extern void *data_ov002_0210f5c0[3];
extern void *data_ov002_0210f5cc[3];
extern void *data_ov002_0210f5d8[3];
extern void *data_ov002_0210f5e4[3];
extern void *data_ov002_0210f5f0[3];
extern void *data_ov002_0210f5fc[3];
extern void *data_ov002_0210f608[3];
extern void *data_ov002_0210f614[3];
extern void *data_ov002_0210f620[3];
extern void *data_ov002_0210f62c[3];
extern void *data_ov002_0210f638[3];
extern void *data_ov002_0210f644[3];
extern void *data_ov002_0210f650[3];
extern void *data_ov002_0210f65c[3];
extern void *data_ov002_0210f668[3];
extern void *data_ov002_0210f674[3];
extern void *data_ov002_0210f680[3];
extern void *data_ov002_0210f68c[3];
extern void *data_ov002_0210f698[3];
extern void *data_ov002_0210f6a4[3];
extern void *data_ov002_0210f6b0[3];
extern void *data_ov002_0210f6bc[3];
extern void *data_ov002_0210f6c8[3];
extern void *data_ov002_0210f6d4[3];
extern void *data_ov002_0210f6e0[3];
extern void *data_ov002_0210f6ec[3];
extern void *data_ov002_0210f6f8[3];
extern void *data_ov002_0210f704[3];
extern void *data_ov002_0210f710[3];
extern void *data_ov002_0210f71c[3];
extern void *data_ov002_0210f728[3];
extern void *data_ov002_0210f734[3];
extern void *data_ov002_0210f740[3];
extern void *data_ov002_0210f74c[3];
extern void *data_ov002_0210f758[3];
extern void *data_ov002_0210f764[3];
extern void *data_ov002_0210f770[3];
extern void *data_ov002_0210f77c[3];
extern void *data_ov002_0210f788[3];
extern void *data_ov002_0210f794[3];
extern void *data_ov002_0210f7a0[3];
extern void *data_ov002_0210f7ac[3];
extern void *data_ov002_0210f7b8[3];
extern void *data_ov002_0210f7c4[3];
extern void *data_ov002_0210f7d0[3];
extern void *data_ov002_0210f7dc[3];
extern void *data_ov002_0210f7e8[3];
extern void *data_ov002_0210f800[3];
extern void *data_ov002_0210f80c[3];
extern void *data_ov002_0210f818[3];
extern void *data_ov002_0210f830[3];
extern void *data_ov002_0210f83c[3];
extern void *data_ov002_0210f848[3];
extern void *data_ov002_0210f854[3];
extern void *data_ov002_0210f860[3];
extern void *data_ov002_0210f86c[3];
extern void *data_ov002_0210f878[3];
extern void *data_ov002_0210f884[3];
extern void *data_ov002_0210f890[3];
extern void *data_ov002_0210f8a8[3];
extern void *data_ov002_0210f8b4[3];
extern void *data_ov002_0210f8c0[3];
extern void *data_ov002_0210f8d8[3];
extern void *data_ov002_0210f8e4[3];
extern void *data_ov002_0210f8f0[3];
extern void *data_ov002_0210f8fc[3];
extern void *data_ov002_0210f908[3];
extern void *data_ov002_0210f914[3];
extern void *data_ov002_0210f920[3];
extern void *data_ov002_0210f938[3];
extern void *data_ov002_0210f944[3];
extern void *data_ov002_0210f950[3];
extern void *data_ov002_0210f95c[3];
extern void *data_ov002_0210f968[3];
extern void *data_ov002_0210f974[3];
extern void *data_ov002_0210f980[3];
extern void *data_ov002_0210f98c[3];
extern void *data_ov002_0210f998[3];
extern void *data_ov002_0210f9a4[3];
extern void *data_ov002_0210f9b0[3];
extern void *data_ov002_0210f9bc[3];
extern void *data_ov002_0210f9c8[3];
extern void *data_ov002_0210f9d4[3];
extern void *data_ov002_0210f9e0[3];
extern void *data_ov002_0210f9ec[3];
extern void *data_ov002_0210f9f8[3];
extern void *data_ov002_0210fa04[3];
extern void *data_ov002_0210fa10[3];
extern void *data_ov002_0210fa1c[3];
extern void *data_ov002_0210fa28[3];
extern void *data_ov002_0210fa34[3];
extern void *data_ov002_0210fa40[3];
extern void *data_ov002_0210fa4c[3];
extern void *data_ov002_0210fa58[3];
extern void *data_ov002_0210fa64[3];
extern void *data_ov002_0210fa70[3];
extern void *data_ov002_0210fa7c[3];
extern void *data_ov002_0210fa88[3];
extern void *data_ov002_0210fa94[3];
extern void *data_ov002_0210faa0[3];
extern void *data_ov002_0210faac[3];
extern void *data_ov002_0210fab8[3];
extern void *data_ov002_0210fac4[3];
extern void *data_ov002_0210fad0[3];
extern void *data_ov002_0210fadc[3];
extern void *data_ov002_0210fae8[3];
extern void *data_ov002_0210faf4[3];
extern void *data_ov002_0210fb00[3];
extern void *data_ov002_0210fb0c[3];
extern void *data_ov002_0210fb18[3];
extern void *data_ov002_0210fb24[3];
extern void *data_ov002_0210fb30[3];
extern void *data_ov002_0210fb3c[3];
extern void *data_ov002_0210fb48[3];
extern void *data_ov002_0210fb54[3];
extern void *data_ov002_0210fb60[3];
extern void *data_ov002_0210fb6c[3];
extern void *data_ov002_0210fb78[3];
extern void *data_ov002_0210fb84[3];
extern void *data_ov002_0210fb90[3];
extern void *data_ov002_0210fb9c[3];
extern void *data_ov002_0210fba8[3];
extern void *data_ov002_0210fbb4[3];
extern void *data_ov002_0210fbc0[3];
extern void *data_ov002_0210fbcc[3];
extern void *data_ov002_0210fbd8[3];
extern void *data_ov002_0210fbe4[3];
extern void *data_ov002_0210fbf0[3];
extern void *data_ov002_0210fbfc[3];
extern void *data_ov002_0210fc08[3];
extern void *data_ov002_0210fc14[3];
extern void *data_ov002_0210fc20[3];
extern void *data_ov002_0210fc2c[3];
extern void *data_ov002_0210fc38[3];
extern void *data_ov002_0210fc44[3];
extern void *data_ov002_0210fc50[3];
extern void *data_ov002_0210fc5c[3];
extern void *data_ov002_0210fc68[3];
extern void *data_ov002_0210fc74[3];
extern void *data_ov002_0210fc80[3];
extern void *data_ov002_0210fc8c[3];
extern void *data_ov002_0210fc98[3];
extern void *data_ov002_0210fca4[3];
extern void *data_ov002_0210fcb0[3];
extern void *data_ov002_0210fcbc[3];
extern void *data_ov002_0210fcc8[3];
extern void *data_ov002_0210fcd4[3];
extern void *data_ov002_0210fce0[3];
extern void *data_ov002_0210fcec[3];
extern void *data_ov002_0210fcf8[3];
extern void *data_ov002_0210fd04[3];
extern void *data_ov002_0210fd10[3];
extern void *data_ov002_0210fd1c[3];
extern void *data_ov002_0210fd28[3];
extern void *data_ov002_0210fd34[3];
extern void *data_ov002_0210fd40[3];
extern void *data_ov002_0210fd4c[3];
extern void *data_ov002_0210fd58[3];
extern void *data_ov002_0210fd64[3];
extern void *data_ov002_0210fd70[3];
extern void *data_ov002_0210fd7c[3];
extern void *data_ov002_0210fd88[3];
extern void *data_ov002_0210fd94[3];
extern void *data_ov002_0210fda0[3];
extern void *data_ov002_0210fdac[3];
extern void *data_ov002_0210fdb8[3];
extern void *data_ov002_0210fdc4[3];
extern void *data_ov002_0210fdd0[3];
extern void *data_ov002_0210fddc[3];
extern void *data_ov002_0210fde8[3];
extern void *data_ov002_0210fdf4[3];
extern void *data_ov002_0210fe00[3];
extern void *data_ov002_0210fe0c[3];
extern void *data_ov002_0210fe18[3];
extern void *data_ov002_0210fe24[3];
extern void *data_ov002_0210fe30[3];
extern void *data_ov002_0210fe3c[3];
extern void *data_ov002_0210fe48[3];
extern void *data_ov002_0210fe54[3];
extern void *data_ov002_0210fe60[3];
extern void *data_ov002_0210fe6c[3];
extern void *data_ov002_0210fe78[3];
extern void *data_ov002_0210fe84[3];
extern void *data_ov002_0210fe90[3];
extern void *data_ov002_0210fe9c[3];
extern void *data_ov002_0210fea8[3];
extern void *data_ov002_0210feb4[3];
extern void *data_ov002_0210fec0[3];
extern void *data_ov002_0210fecc[3];
extern void *data_ov002_0210fed8[3];
extern void *data_ov002_0210fee4[3];
extern void *data_ov002_0210fef0[3];
extern void *data_ov002_0210fefc[3];
extern void *data_ov002_0210ff08[3];
extern void *data_ov002_0210ff14[3];
extern void *data_ov002_0210ff20[3];
extern void *data_ov002_0210ff2c[3];
extern void *data_ov002_0210ff38[3];
extern void *data_ov002_0210ff44[3];
extern void *data_ov002_0210ff50[3];
extern void *data_ov002_0210ff5c[3];
extern void *data_ov002_0210ff68[3];
extern void *data_ov002_0210ff74[3];
extern void *data_ov002_0210ff80[3];
extern void *data_ov002_0210ff8c[3];
extern void *data_ov002_0210ff98[3];
extern void *data_ov002_0210ffa4[3];
extern void *data_ov002_0210ffb0[3];
extern void *data_ov002_0210ffbc[3];
extern void *data_ov002_0210ffc8[3];
extern void *data_ov002_0210ffd4[3];
extern void *data_ov002_0210ffe0[3];

/* Pointer-to-member constants in ov002 .data, copied into the states below. */
extern int (Player::*data_ov002_02109dd4)();
extern int (Player::*data_ov002_02109ddc)();
extern int (Player::*data_ov002_02109de4)();
extern int (Player::*data_ov002_02109dec)();
extern int (Player::*data_ov002_02109df4)();
extern int (Player::*data_ov002_02109e04)();
extern int (Player::*data_ov002_02109e0c)();
extern int (Player::*data_ov002_02109e14)();
extern int (Player::*data_ov002_02109e1c)();
extern int (Player::*data_ov002_02109e24)();
extern int (Player::*data_ov002_02109e34)();
extern int (Player::*data_ov002_02109e3c)();
extern int (Player::*data_ov002_02109e44)();
extern int (Player::*data_ov002_02109e4c)();
extern int (Player::*data_ov002_02109e54)();
extern int (Player::*data_ov002_02109e5c)();
extern int (Player::*data_ov002_02109e64)();
extern int (Player::*data_ov002_02109e6c)();
extern int (Player::*data_ov002_02109e74)();
extern int (Player::*data_ov002_02109e7c)();
extern int (Player::*data_ov002_02109e84)();
extern int (Player::*data_ov002_02109e8c)();
extern int (Player::*data_ov002_02109e94)();
extern int (Player::*data_ov002_02109e9c)();
extern int (Player::*data_ov002_02109ea4)();
extern int (Player::*data_ov002_02109eac)();
extern int (Player::*data_ov002_02109eb4)();
extern int (Player::*data_ov002_02109ebc)();
extern int (Player::*data_ov002_02109ec4)();
extern int (Player::*data_ov002_02109ecc)();
extern int (Player::*data_ov002_02109ed4)();
extern int (Player::*data_ov002_02109edc)();
extern int (Player::*data_ov002_02109ee4)();
extern int (Player::*data_ov002_02109eec)();
extern int (Player::*data_ov002_02109ef4)();
extern int (Player::*data_ov002_02109efc)();
extern int (Player::*data_ov002_02109f04)();
extern int (Player::*data_ov002_02109f14)();
extern int (Player::*data_ov002_02109f1c)();
extern int (Player::*data_ov002_02109f24)();
extern int (Player::*data_ov002_02109f2c)();
extern int (Player::*data_ov002_02109f34)();
extern int (Player::*data_ov002_02109f3c)();
extern int (Player::*data_ov002_02109f4c)();
extern int (Player::*data_ov002_02109f5c)();
extern int (Player::*data_ov002_02109f64)();
extern int (Player::*data_ov002_02109f6c)();
extern int (Player::*data_ov002_02109f74)();
extern int (Player::*data_ov002_02109f7c)();
extern int (Player::*data_ov002_02109f84)();
extern int (Player::*data_ov002_02109f8c)();
extern int (Player::*data_ov002_02109f94)();
extern int (Player::*data_ov002_02109f9c)();
extern int (Player::*data_ov002_02109fa4)();
extern int (Player::*data_ov002_02109fac)();
extern int (Player::*data_ov002_02109fb4)();
extern int (Player::*data_ov002_02109fbc)();
extern int (Player::*data_ov002_02109fc4)();
extern int (Player::*data_ov002_02109fcc)();
extern int (Player::*data_ov002_02109fd4)();
extern int (Player::*data_ov002_02109fdc)();
extern int (Player::*data_ov002_02109fec)();
extern int (Player::*data_ov002_02109ffc)();
extern int (Player::*data_ov002_0210a004)();
extern int (Player::*data_ov002_0210a00c)();
extern int (Player::*data_ov002_0210a014)();
extern int (Player::*data_ov002_0210a01c)();
extern int (Player::*data_ov002_0210a024)();
extern int (Player::*data_ov002_0210a02c)();
extern int (Player::*data_ov002_0210a034)();
extern int (Player::*data_ov002_0210a03c)();
extern int (Player::*data_ov002_0210a044)();
extern int (Player::*data_ov002_0210a04c)();
extern int (Player::*data_ov002_0210a05c)();
extern int (Player::*data_ov002_0210a06c)();
extern int (Player::*data_ov002_0210a08c)();
extern int (Player::*data_ov002_0210a09c)();
extern int (Player::*data_ov002_0210a0a4)();
extern int (Player::*data_ov002_0210a0ac)();
extern int (Player::*data_ov002_0210a0bc)();
extern int (Player::*data_ov002_0210a0c4)();
extern int (Player::*data_ov002_0210a0cc)();
extern int (Player::*data_ov002_0210a0d4)();
extern int (Player::*data_ov002_0210a0e4)();
extern int (Player::*data_ov002_0210a0ec)();
extern int (Player::*data_ov002_0210a0f4)();
extern int (Player::*data_ov002_0210a0fc)();
extern int (Player::*data_ov002_0210a104)();
extern int (Player::*data_ov002_0210a114)();
extern int (Player::*data_ov002_0210a11c)();
extern int (Player::*data_ov002_0210a13c)();
extern int (Player::*data_ov002_0210a15c)();
extern int (Player::*data_ov002_0210a164)();
extern int (Player::*data_ov002_0210a16c)();
extern int (Player::*data_ov002_0210a17c)();
extern int (Player::*data_ov002_0210a184)();
extern int (Player::*data_ov002_0210a18c)();
extern int (Player::*data_ov002_0210a194)();
extern int (Player::*data_ov002_0210a19c)();
extern int (Player::*data_ov002_0210a1a4)();
extern int (Player::*data_ov002_0210a1ac)();
extern int (Player::*data_ov002_0210a1bc)();
extern int (Player::*data_ov002_0210a1c4)();
extern int (Player::*data_ov002_0210a1dc)();
extern int (Player::*data_ov002_0210a1e4)();
extern int (Player::*data_ov002_0210a1f4)();
extern int (Player::*data_ov002_0210a204)();
extern int (Player::*data_ov002_0210a20c)();
extern int (Player::*data_ov002_0210a214)();
extern int (Player::*data_ov002_0210a21c)();
extern int (Player::*data_ov002_0210a224)();
extern int (Player::*data_ov002_0210a234)();
extern int (Player::*data_ov002_0210a24c)();
extern int (Player::*data_ov002_0210a254)();
extern int (Player::*data_ov002_0210a25c)();
extern int (Player::*data_ov002_0210a26c)();
extern int (Player::*data_ov002_0210a27c)();
extern int (Player::*data_ov002_0210a284)();
extern int (Player::*data_ov002_0210a28c)();
extern int (Player::*data_ov002_0210a294)();
extern int (Player::*data_ov002_0210a2a4)();
extern int (Player::*data_ov002_0210a2ac)();
extern int (Player::*data_ov002_0210a2b4)();
extern int (Player::*data_ov002_0210a2bc)();
extern int (Player::*data_ov002_0210a2c4)();
extern int (Player::*data_ov002_0210a2cc)();
extern int (Player::*data_ov002_0210a2d4)();
extern int (Player::*data_ov002_0210a2dc)();
extern int (Player::*data_ov002_0210a2e4)();
extern int (Player::*data_ov002_0210a2ec)();
extern int (Player::*data_ov002_0210a2f4)();
extern int (Player::*data_ov002_0210a2fc)();
extern int (Player::*data_ov002_0210a304)();
extern int (Player::*data_ov002_0210a314)();
extern int (Player::*data_ov002_0210a31c)();
extern int (Player::*data_ov002_0210a324)();
extern int (Player::*data_ov002_0210a32c)();
extern int (Player::*data_ov002_0210a33c)();
extern int (Player::*data_ov002_0210a344)();
extern int (Player::*data_ov002_0210a354)();
extern int (Player::*data_ov002_0210a35c)();
extern int (Player::*data_ov002_0210a364)();
extern int (Player::*data_ov002_0210a374)();
extern int (Player::*data_ov002_0210a37c)();
extern int (Player::*data_ov002_0210a384)();
extern int (Player::*data_ov002_0210a38c)();
extern int (Player::*data_ov002_0210a394)();
extern int (Player::*data_ov002_0210a39c)();
extern int (Player::*data_ov002_0210a3a4)();
extern int (Player::*data_ov002_0210a3b4)();
extern int (Player::*data_ov002_0210a3bc)();
extern int (Player::*data_ov002_0210a3cc)();
extern int (Player::*data_ov002_0210a3d4)();
extern int (Player::*data_ov002_0210a3e4)();
extern int (Player::*data_ov002_0210a3ec)();
extern int (Player::*data_ov002_0210a414)();
extern int (Player::*data_ov002_0210a41c)();
extern int (Player::*data_ov002_0210a42c)();
extern int (Player::*data_ov002_0210a43c)();
extern int (Player::*data_ov002_0210a444)();
extern int (Player::*data_ov002_0210a454)();
extern int (Player::*data_ov002_0210a464)();
extern int (Player::*data_ov002_0210a484)();
extern int (Player::*data_ov002_0210a494)();
extern int (Player::*data_ov002_0210a49c)();
extern int (Player::*data_ov002_0210a4a4)();
extern int (Player::*data_ov002_0210a4ac)();
extern int (Player::*data_ov002_0210a4b4)();
extern int (Player::*data_ov002_0210a4c4)();
extern int (Player::*data_ov002_0210a4cc)();
extern int (Player::*data_ov002_0210a4d4)();
extern int (Player::*data_ov002_0210a4dc)();
extern int (Player::*data_ov002_0210a4e4)();
extern int (Player::*data_ov002_0210a4ec)();
extern int (Player::*data_ov002_0210a4f4)();
extern int (Player::*data_ov002_0210a4fc)();
extern int (Player::*data_ov002_0210a504)();
extern int (Player::*data_ov002_0210a50c)();
extern int (Player::*data_ov002_0210a514)();
extern int (Player::*data_ov002_0210a51c)();
extern int (Player::*data_ov002_0210a524)();
extern int (Player::*data_ov002_0210a52c)();
extern int (Player::*data_ov002_0210a53c)();
extern int (Player::*data_ov002_0210a544)();
extern int (Player::*data_ov002_0210a54c)();
extern void (Player::*data_ov002_02109dfc)();
extern void (Player::*data_ov002_02109e2c)();
extern void (Player::*data_ov002_02109f0c)();
extern void (Player::*data_ov002_02109f44)();
extern void (Player::*data_ov002_02109f54)();
extern void (Player::*data_ov002_0210a074)();
extern void (Player::*data_ov002_0210a10c)();
extern void (Player::*data_ov002_0210a12c)();
extern void (Player::*data_ov002_0210a134)();
extern void (Player::*data_ov002_0210a144)();
extern void (Player::*data_ov002_0210a154)();
extern void (Player::*data_ov002_0210a174)();
extern void (Player::*data_ov002_0210a1b4)();
extern void (Player::*data_ov002_0210a1cc)();
extern void (Player::*data_ov002_0210a1ec)();
extern void (Player::*data_ov002_0210a1fc)();
extern void (Player::*data_ov002_0210a22c)();
extern void (Player::*data_ov002_0210a23c)();
extern void (Player::*data_ov002_0210a244)();
extern void (Player::*data_ov002_0210a264)();
extern void (Player::*data_ov002_0210a274)();
extern void (Player::*data_ov002_0210a29c)();
extern void (Player::*data_ov002_0210a30c)();
extern void (Player::*data_ov002_0210a334)();
extern void (Player::*data_ov002_0210a3ac)();
extern void (Player::*data_ov002_0210a3f4)();
extern void (Player::*data_ov002_0210a404)();
extern void (Player::*data_ov002_0210a45c)();
extern void (Player::*data_ov002_0210a46c)();
extern void (Player::*data_ov002_0210a47c)();
extern void (Player::*data_ov002_0210a48c)();

/* The player states: init, main and cleanup handlers. */
extern Player::State data_ov002_0210ffec;
extern Player::State data_ov002_02110004;
extern Player::State data_ov002_0211001c;
extern Player::State data_ov002_02110034;
extern Player::State data_ov002_0211004c;
extern Player::State data_ov002_02110064;
extern Player::State data_ov002_0211007c;
extern Player::State data_ov002_02110094;
extern Player::State data_ov002_021100ac;
extern Player::State data_ov002_021100c4;
extern Player::State data_ov002_021100dc;
extern Player::State data_ov002_021100f4;
extern Player::State data_ov002_0211010c;
extern Player::State data_ov002_02110124;
extern Player::State data_ov002_0211013c;
extern Player::State data_ov002_0211016c;
extern Player::State data_ov002_02110184;
extern Player::State data_ov002_0211019c;
extern Player::State data_ov002_021101b4;
extern Player::State data_ov002_021101cc;
extern Player::State data_ov002_021101e4;
extern Player::State data_ov002_021101fc;
extern Player::State data_ov002_02110214;
extern Player::State data_ov002_0211022c;
extern Player::State data_ov002_0211025c;
extern Player::State data_ov002_02110274;
extern Player::State data_ov002_0211028c;
extern Player::State data_ov002_021102a4;
extern Player::State data_ov002_021102bc;
extern Player::State data_ov002_021102d4;
extern Player::State data_ov002_021102ec;
extern Player::State data_ov002_02110304;
extern Player::State data_ov002_0211031c;
extern Player::State data_ov002_02110334;
extern Player::State data_ov002_0211034c;
extern Player::State data_ov002_02110364;
extern Player::State data_ov002_0211037c;
extern Player::State data_ov002_02110394;
extern Player::State data_ov002_021103ac;
extern Player::State data_ov002_021103c4;
extern Player::State data_ov002_021103dc;
extern Player::State data_ov002_021103f4;
extern Player::State data_ov002_0211040c;
extern Player::State data_ov002_02110424;
extern Player::State data_ov002_0211043c;
extern Player::State data_ov002_02110454;
extern Player::State data_ov002_0211046c;
extern Player::State data_ov002_02110484;
extern Player::State data_ov002_0211049c;
extern Player::State data_ov002_021104b4;
extern Player::State data_ov002_021104cc;
extern Player::State data_ov002_021104e4;
extern Player::State data_ov002_021104fc;
extern Player::State data_ov002_02110514;
extern Player::State data_ov002_0211052c;
extern Player::State data_ov002_02110544;
extern Player::State data_ov002_0211055c;
extern Player::State data_ov002_02110574;
extern Player::State data_ov002_0211058c;
extern Player::State data_ov002_021105a4;
extern Player::State data_ov002_021105bc;
extern Player::State data_ov002_021105d4;
extern Player::State data_ov002_021105ec;
extern Player::State data_ov002_02110604;
extern Player::State data_ov002_0211061c;
extern Player::State data_ov002_02110634;
extern Player::State data_ov002_0211064c;
extern Player::State data_ov002_02110664;
extern Player::State data_ov002_0211067c;
extern Player::State data_ov002_02110694;
extern Player::State data_ov002_021106ac;
extern Player::State data_ov002_021106c4;
extern Player::State data_ov002_021106dc;
extern Player::State data_ov002_021106f4;
extern Player::State data_ov002_0211070c;
extern Player::State data_ov002_02110724;

/* Player handler tables; the second is indexed by mNoCtrlKind, the third by mStateWork. */
extern void (Player::*data_ov002_0211073c[4])();
extern void (Player::*data_ov002_02110884[20])();
extern void (Player::*data_ov002_0211075c[7])();

/* The two 32-entry palette ramps func_ov002_020e69c0 and func_ov002_020e6a78 work on. */
extern unsigned short data_ov002_02110804[];

void __sinit_ov002_021019d0(void)
{
    func_0203b9b4(&data_ov002_0210e160, 0x4d2);
    func_020731dc(&data_ov002_0210e160, (void *)func_0201973c, data_ov002_0210f9b0);
    func_02017acc(&data_ov002_0210e6f8, ASSET_B_MARIO_ALL_BMD);
    func_020731dc(&data_ov002_0210e6f8, (void *)func_02017ab4, data_ov002_0210f9c8);
    func_02017acc(&data_ov002_0210e648, ASSET_B_MARIO_BODY_BMD);
    func_020731dc(&data_ov002_0210e648, (void *)func_02017ab4, data_ov002_0210f9d4);
    func_02017acc(&data_ov002_0210eae8, ASSET_B_MARIO_HEAD_CAP_BMD);
    func_020731dc(&data_ov002_0210eae8, (void *)func_02017ab4, data_ov002_0210f9e0);
    func_02017acc(&data_ov002_0210eb20, 0x807e);
    func_020731dc(&data_ov002_0210eb20, (void *)func_02017ab4, data_ov002_0210f9f8);
    func_02017acc(&data_ov002_0210eb00, 0x807f);
    func_020731dc(&data_ov002_0210eb00, (void *)func_02017ab4, data_ov002_0210fa04);
    func_02017acc(&data_ov002_0210ebb8, 0x8080);
    func_020731dc(&data_ov002_0210ebb8, (void *)func_02017ab4, data_ov002_0210fa1c);
    func_02017acc(&data_ov002_0210e690, 0x8081);
    func_020731dc(&data_ov002_0210e690, (void *)func_02017ab4, data_ov002_0210fa28);
    func_02017acc(&data_ov002_0210ec38, 0x8082);
    func_020731dc(&data_ov002_0210ec38, (void *)func_02017ab4, data_ov002_0210fa40);
    func_02017acc(&data_ov002_0210ec50, 0x8083);
    func_020731dc(&data_ov002_0210ec50, (void *)func_02017ab4, data_ov002_0210fa4c);
    func_02017acc(&data_ov002_0210e758, 0x80b8);
    func_020731dc(&data_ov002_0210e758, (void *)func_02017ab4, data_ov002_0210fa64);
    func_02017acc(&data_ov002_0210e790, 0x80b9);
    func_020731dc(&data_ov002_0210e790, (void *)func_02017ab4, data_ov002_0210fa70);
    func_02017acc(&data_ov002_0210e780, ASSET_WARIO_METAL_MODEL_BMD);
    func_020731dc(&data_ov002_0210e780, (void *)func_02017ab4, data_ov002_0210fa94);
    func_02017acc(&data_ov002_0210ec60, 0x80ba);
    func_020731dc(&data_ov002_0210ec60, (void *)func_02017ab4, data_ov002_0210faa0);
    func_02017acc(&data_ov002_0210e7b0, 0x80bd);
    func_020731dc(&data_ov002_0210e7b0, (void *)func_02017ab4, data_ov002_0210fab8);
    func_02017acc(&data_ov002_0210e3a0, 0x80c0);
    func_020731dc(&data_ov002_0210e3a0, (void *)func_02017ab4, data_ov002_0210fac4);
    func_02017acc(&data_ov002_0210e7d8, 0x80c1);
    func_020731dc(&data_ov002_0210e7d8, (void *)func_02017ab4, data_ov002_0210fadc);
    func_02017acc(&data_ov002_0210e7a8, 0x80c2);
    func_020731dc(&data_ov002_0210e7a8, (void *)func_02017ab4, data_ov002_0210fae8);
    func_02017acc(&data_ov002_0210e7f0, 0x80c3);
    func_020731dc(&data_ov002_0210e7f0, (void *)func_02017ab4, data_ov002_0210fb00);
    func_02017acc(&data_ov002_0210e818, 0x80c4);
    func_020731dc(&data_ov002_0210e818, (void *)func_02017ab4, data_ov002_0210fb0c);
    data_ov002_0210e490.Construct(0x801d);
    func_020731dc(&data_ov002_0210e490, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fb24);
    data_ov002_0210e838.Construct(0x801e);
    func_020731dc(&data_ov002_0210e838, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fb30);
    data_ov002_0210e848.Construct(0x801f);
    func_020731dc(&data_ov002_0210e848, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fb48);
    data_ov002_0210e2c0.Construct(ASSET_L_ED_AMAZED_BCA);
    func_020731dc(&data_ov002_0210e2c0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fb54);
    data_ov002_0210e860.Construct(ASSET_L_ED_CAP_OFF_BCA);
    func_020731dc(&data_ov002_0210e860, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fb6c);
    data_ov002_0210e4f8.Construct(ASSET_L_ED_CAP_OFF_WAIT_BCA);
    func_020731dc(&data_ov002_0210e4f8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fb78);
    data_ov002_0210e888.Construct(ASSET_L_ED_LOOK_M_WAIT_BCA);
    func_020731dc(&data_ov002_0210e888, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fb90);
    data_ov002_0210e868.Construct(ASSET_L_ED_LOOK_MARIO_BCA);
    func_020731dc(&data_ov002_0210e868, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fb9c);
    data_ov002_0210e7c0.Construct(ASSET_L_ED_LOOKUP_WAIT_BCA);
    func_020731dc(&data_ov002_0210e7c0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fbb4);
    data_ov002_0210e8b0.Construct(ASSET_L_ED_WAIT_BCA);
    func_020731dc(&data_ov002_0210e8b0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fbc0);
    data_ov002_0210e8b8.Construct(ASSET_L_ENTRY_WAIT_BCA);
    func_020731dc(&data_ov002_0210e8b8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fbe4);
    data_ov002_0210e518.Construct(ASSET_L_ENTRY_WAIT_END_BCA);
    func_020731dc(&data_ov002_0210e518, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fbf0);
    data_ov002_0210e8d8.Construct(ASSET_L_FIRST_CHANGE_BCA);
    func_020731dc(&data_ov002_0210e8d8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fc08);
    data_ov002_0210e8f0.Construct(ASSET_L_LIE_END_BCA);
    func_020731dc(&data_ov002_0210e8f0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fc14);
    data_ov002_0210e900.Construct(ASSET_L_LIE_WAIT_BCA);
    func_020731dc(&data_ov002_0210e900, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fc2c);
    data_ov002_0210e548.Construct(ASSET_L_OP_LOOK_WARIO_BCA);
    func_020731dc(&data_ov002_0210e548, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fc38);
    data_ov002_0210e530.Construct(ASSET_L_OP_NOD_BCA);
    func_020731dc(&data_ov002_0210e530, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fc50);
    data_ov002_0210e528.Construct(ASSET_L_OP_TURN_BCA);
    func_020731dc(&data_ov002_0210e528, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f380);
    data_ov002_0210e4b0.Construct(0x8020);
    func_020731dc(&data_ov002_0210e4b0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fc68);
    data_ov002_0210e550.Construct(ASSET_L_SLEEP_END_BCA);
    func_020731dc(&data_ov002_0210e550, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fc74);
    data_ov002_0210e748.Construct(ASSET_L_SLEEP_START_BCA);
    func_020731dc(&data_ov002_0210e748, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fc8c);
    data_ov002_0210e5c0.Construct(ASSET_L_SLEEP_TO_LIE_BCA);
    func_020731dc(&data_ov002_0210e5c0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fc98);
    data_ov002_0210e998.Construct(ASSET_L_SLEEP_WAIT_BCA);
    func_020731dc(&data_ov002_0210e998, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fcb0);
    data_ov002_0210eaa0.Construct(0x8029);
    func_020731dc(&data_ov002_0210eaa0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fcbc);
    data_ov002_0210ea50.Construct(0x802a);
    func_020731dc(&data_ov002_0210ea50, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fcd4);
    data_ov002_0210e8c8.Construct(ASSET_M_ENTRY_WAIT_BCA);
    func_020731dc(&data_ov002_0210e8c8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fce0);
    data_ov002_0210e778.Construct(ASSET_M_ENTRY_WAIT_END_BCA);
    func_020731dc(&data_ov002_0210e778, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fcf8);
    data_ov002_0210ebe0.Construct(0x802b);
    func_020731dc(&data_ov002_0210ebe0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fd04);
    data_ov002_0210e4f0.Construct(0x802c);
    func_020731dc(&data_ov002_0210e4f0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fd1c);
    data_ov002_0210e4c8.Construct(0x802d);
    func_020731dc(&data_ov002_0210e4c8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fd28);
    data_ov002_0210eb30.Construct(0x802e);
    func_020731dc(&data_ov002_0210eb30, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fd40);
    data_ov002_0210e510.Construct(0x802f);
    func_020731dc(&data_ov002_0210e510, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fd4c);
    data_ov002_0210e970.Construct(ASSET_W_ED_AMAZED_WAIR_BCA);
    func_020731dc(&data_ov002_0210e970, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fd64);
    data_ov002_0210e558.Construct(ASSET_W_ED_CAP_OFF_BCA);
    func_020731dc(&data_ov002_0210e558, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fd70);
    data_ov002_0210e9c0.Construct(ASSET_W_ED_CAP_OFF_WAIT_BCA);
    func_020731dc(&data_ov002_0210e9c0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fd88);
    data_ov002_0210ea20.Construct(ASSET_W_ED_LOOKUP_WAIT_BCA);
    func_020731dc(&data_ov002_0210ea20, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fd94);
    data_ov002_0210e5b0.Construct(ASSET_W_ED_WAIT_BCA);
    func_020731dc(&data_ov002_0210e5b0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fdac);
    data_ov002_0210e5d0.Construct(ASSET_W_ED_WAVE_BCA);
    func_020731dc(&data_ov002_0210e5d0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fdb8);
    data_ov002_0210e5f0.Construct(ASSET_W_ENTRY_WAIT_BCA);
    func_020731dc(&data_ov002_0210e5f0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fdd0);
    data_ov002_0210e9a0.Construct(ASSET_W_ENTRY_WAIT_END_BCA);
    func_020731dc(&data_ov002_0210e9a0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fddc);
    data_ov002_0210e520.Construct(ASSET_W_FIRST_CHANGE_BCA);
    func_020731dc(&data_ov002_0210e520, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fdf4);
    data_ov002_0210eab8.Construct(0x8030);
    func_020731dc(&data_ov002_0210eab8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fe00);
    data_ov002_0210ead0.Construct(0x8031);
    func_020731dc(&data_ov002_0210ead0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fe18);
    data_ov002_0210eac0.Construct(0x8032);
    func_020731dc(&data_ov002_0210eac0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fe24);
    data_ov002_0210e9f0.Construct(0x8033);
    func_020731dc(&data_ov002_0210e9f0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fe3c);
    data_ov002_0210eae0.Construct(ASSET_W_KEY_GET_BCA);
    func_020731dc(&data_ov002_0210eae0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fe48);
    data_ov002_0210ea08.Construct(ASSET_W_LIE_END_BCA);
    func_020731dc(&data_ov002_0210ea08, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fe60);
    data_ov002_0210e990.Construct(ASSET_W_LIE_WAIT_BCA);
    func_020731dc(&data_ov002_0210e990, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fe6c);
    data_ov002_0210eaf8.Construct(ASSET_W_OP_LOOK_LUIGI_BCA);
    func_020731dc(&data_ov002_0210eaf8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fe84);
    data_ov002_0210e9b8.Construct(ASSET_W_OP_NOD_BCA);
    func_020731dc(&data_ov002_0210e9b8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fe90);
    data_ov002_0210ecb0.Construct(ASSET_W_OP_POINT_BCA);
    func_020731dc(&data_ov002_0210ecb0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fea8);
    data_ov002_0210eb08.Construct(ASSET_W_OP_TURN_BCA);
    func_020731dc(&data_ov002_0210eb08, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210feb4);
    data_ov002_0210e668.Construct(0x8034);
    func_020731dc(&data_ov002_0210e668, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fecc);
    data_ov002_0210e650.Construct(ASSET_W_SLEEP_END_BCA);
    func_020731dc(&data_ov002_0210e650, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fed8);
    data_ov002_0210eb18.Construct(ASSET_W_SLEEP_START_BCA);
    func_020731dc(&data_ov002_0210eb18, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fef0);
    data_ov002_0210e658.Construct(ASSET_W_SLEEP_TO_LIE_BCA);
    func_020731dc(&data_ov002_0210e658, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fefc);
    data_ov002_0210e678.Construct(ASSET_W_SLEEP_WAIT_BCA);
    func_020731dc(&data_ov002_0210e678, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ff14);
    data_ov002_0210eb28.Construct(0x803b);
    func_020731dc(&data_ov002_0210eb28, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ff20);
    data_ov002_0210e638.Construct(0x803c);
    func_020731dc(&data_ov002_0210e638, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ff38);
    data_ov002_0210e688.Construct(0x803d);
    func_020731dc(&data_ov002_0210e688, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ff44);
    data_ov002_0210eb40.Construct(0x803e);
    func_020731dc(&data_ov002_0210eb40, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ff5c);
    data_ov002_0210e630.Construct(0x803f);
    func_020731dc(&data_ov002_0210e630, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ff68);
    data_ov002_0210e930.Construct(0x8040);
    func_020731dc(&data_ov002_0210e930, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ff80);
    data_ov002_0210eb50.Construct(ASSET_Y_ED_LOOK_CAM_BCA);
    func_020731dc(&data_ov002_0210eb50, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ff8c);
    data_ov002_0210e698.Construct(ASSET_Y_ED_LOOKUP_WAIT_BCA);
    func_020731dc(&data_ov002_0210e698, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ffa4);
    data_ov002_0210eb60.Construct(ASSET_Y_ED_SHY_TO_WAIT_BCA);
    func_020731dc(&data_ov002_0210eb60, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ffb0);
    data_ov002_0210eb68.Construct(ASSET_Y_ED_SHY_WAIT_BCA);
    func_020731dc(&data_ov002_0210eb68, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ffc8);
    data_ov002_0210e6a8.Construct(ASSET_Y_ED_TURN_BCA);
    func_020731dc(&data_ov002_0210e6a8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ffd4);
    data_ov002_0210e618.Construct(ASSET_Y_ED_WAIT_BCA);
    func_020731dc(&data_ov002_0210e618, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ed68);
    data_ov002_0210e6b0.Construct(ASSET_Y_EGG_RUN_BCA);
    func_020731dc(&data_ov002_0210e6b0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ed74);
    data_ov002_0210eb78.Construct(ASSET_Y_EGG_WAIT_BCA);
    func_020731dc(&data_ov002_0210eb78, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ed8c);
    data_ov002_0210e3d0.Construct(0x8041);
    func_020731dc(&data_ov002_0210e3d0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ed98);
    data_ov002_0210eb80.Construct(0x8042);
    func_020731dc(&data_ov002_0210eb80, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210edb0);
    data_ov002_0210eb88.Construct(0x8043);
    func_020731dc(&data_ov002_0210eb88, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210edbc);
    data_ov002_0210e6c0.Construct(0x8044);
    func_020731dc(&data_ov002_0210e6c0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210edd4);
    data_ov002_0210e3d8.Construct(0x8045);
    func_020731dc(&data_ov002_0210e3d8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ede0);
    data_ov002_0210eb98.Construct(0x8046);
    func_020731dc(&data_ov002_0210eb98, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210edf8);
    data_ov002_0210e6c8.Construct(0x8047);
    func_020731dc(&data_ov002_0210e6c8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ee04);
    data_ov002_0210eba0.Construct(ASSET_Y_LIE_END_BCA);
    func_020731dc(&data_ov002_0210eba0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ee1c);
    data_ov002_0210e6d0.Construct(ASSET_Y_LIE_WAIT_BCA);
    func_020731dc(&data_ov002_0210e6d0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ee28);
    data_ov002_0210ebb0.Construct(ASSET_Y_OP_LIE_TO_SIT_BCA);
    func_020731dc(&data_ov002_0210ebb0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ee40);
    data_ov002_0210e3f0.Construct(ASSET_Y_OP_SIT_TO_STAND_BCA);
    func_020731dc(&data_ov002_0210e3f0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ee4c);
    data_ov002_0210e6d8.Construct(ASSET_Y_OP_SIT_WAIT_BCA);
    func_020731dc(&data_ov002_0210e6d8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ee64);
    data_ov002_0210ebc0.Construct(ASSET_Y_OP_STAND_WAIT_BCA);
    func_020731dc(&data_ov002_0210ebc0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ee70);
    data_ov002_0210e3f8.Construct(0x8048);
    func_020731dc(&data_ov002_0210e3f8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ee88);
    data_ov002_0210e6e8.Construct(ASSET_Y_SLEEP_END_BCA);
    func_020731dc(&data_ov002_0210e6e8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ee94);
    data_ov002_0210ebd0.Construct(ASSET_Y_SLEEP_START_BCA);
    func_020731dc(&data_ov002_0210ebd0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210eeac);
    data_ov002_0210e6f0.Construct(ASSET_Y_SLEEP_TO_LIE_BCA);
    func_020731dc(&data_ov002_0210e6f0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fbcc);
    data_ov002_0210e3c0.Construct(ASSET_Y_SLEEP_WAIT_BCA);
    func_020731dc(&data_ov002_0210e3c0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210eec4);
    data_ov002_0210e628.Construct(0x8419);
    func_020731dc(&data_ov002_0210e628, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210eed0);
    data_ov002_0210e700.Construct(0x841a);
    func_020731dc(&data_ov002_0210e700, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210eee8);
    data_ov002_0210ebe8.Construct(0x841b);
    func_020731dc(&data_ov002_0210ebe8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210eef4);
    data_ov002_0210ebf0.Construct(0x841c);
    func_020731dc(&data_ov002_0210ebf0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ef0c);
    data_ov002_0210e710.Construct(0x841d);
    func_020731dc(&data_ov002_0210e710, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ef18);
    data_ov002_0210e3a8.Construct(0x841e);
    func_020731dc(&data_ov002_0210e3a8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ef30);
    data_ov002_0210ebf8.Construct(0x841f);
    func_020731dc(&data_ov002_0210ebf8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ef3c);
    data_ov002_0210e718.Construct(0x8420);
    func_020731dc(&data_ov002_0210e718, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ef54);
    data_ov002_0210ec08.Construct(0x8421);
    func_020731dc(&data_ov002_0210ec08, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ef60);
    data_ov002_0210e720.Construct(0x8422);
    func_020731dc(&data_ov002_0210e720, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ef78);
    data_ov002_0210ec20.Construct(0x8423);
    func_020731dc(&data_ov002_0210ec20, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ef84);
    data_ov002_0210e410.Construct(0x8424);
    func_020731dc(&data_ov002_0210e410, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210ef9c);
    data_ov002_0210e260.Construct(0x8425);
    func_020731dc(&data_ov002_0210e260, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fa88);
    data_ov002_0210e730.Construct(0x8426);
    func_020731dc(&data_ov002_0210e730, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210efb4);
    data_ov002_0210ec30.Construct(0x8050);
    func_020731dc(&data_ov002_0210ec30, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210efc0);
    data_ov002_0210e418.Construct(0x8051);
    func_020731dc(&data_ov002_0210e418, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210efd8);
    data_ov002_0210e740.Construct(0x8052);
    func_020731dc(&data_ov002_0210e740, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210efe4);
    data_ov002_0210ec48.Construct(0x8053);
    func_020731dc(&data_ov002_0210ec48, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210effc);
    data_ov002_0210e420.Construct(0x8054);
    func_020731dc(&data_ov002_0210e420, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f008);
    data_ov002_0210e750.Construct(ASSET_B_MARIO_START_BCA);
    func_020731dc(&data_ov002_0210e750, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f020);
    data_ov002_0210e760.Construct(0x8055);
    func_020731dc(&data_ov002_0210e760, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f02c);
    data_ov002_0210e428.Construct(0x8056);
    func_020731dc(&data_ov002_0210e428, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f044);
    data_ov002_0210e258.Construct(0x8057);
    func_020731dc(&data_ov002_0210e258, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f050);
    data_ov002_0210e268.Construct(0x8058);
    func_020731dc(&data_ov002_0210e268, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f068);
    data_ov002_0210e440.Construct(0x8059);
    func_020731dc(&data_ov002_0210e440, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f074);
    data_ov002_0210e9e0.Construct(0x805a);
    func_020731dc(&data_ov002_0210e9e0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f08c);
    data_ov002_0210e240.Construct(0x805b);
    func_020731dc(&data_ov002_0210e240, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f098);
    data_ov002_0210e448.Construct(0x805c);
    func_020731dc(&data_ov002_0210e448, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f0b0);
    data_ov002_0210e798.Construct(0x805d);
    func_020731dc(&data_ov002_0210e798, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f0bc);
    data_ov002_0210e7a0.Construct(0x805e);
    func_020731dc(&data_ov002_0210e7a0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f0d4);
    data_ov002_0210e278.Construct(0x805f);
    func_020731dc(&data_ov002_0210e278, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f0e0);
    data_ov002_0210e238.Construct(0x8060);
    func_020731dc(&data_ov002_0210e238, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f0f8);
    data_ov002_0210e248.Construct(0x8061);
    func_020731dc(&data_ov002_0210e248, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f104);
    data_ov002_0210e468.Construct(ASSET_COLD_WAIT_BCA);
    func_020731dc(&data_ov002_0210e468, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f11c);
    data_ov002_0210e470.Construct(ASSET_COLD_WAIT_END_BCA);
    func_020731dc(&data_ov002_0210e470, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f128);
    data_ov002_0210e280.Construct(ASSET_COLD_WAIT_START_BCA);
    func_020731dc(&data_ov002_0210e280, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f140);
    data_ov002_0210e7d0.Construct(0x8062);
    func_020731dc(&data_ov002_0210e7d0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f14c);
    data_ov002_0210e290.Construct(0x8063);
    func_020731dc(&data_ov002_0210e290, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f164);
    data_ov002_0210e480.Construct(0x8064);
    func_020731dc(&data_ov002_0210e480, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f170);
    data_ov002_0210e488.Construct(ASSET_DAMAGE_START_BCA);
    func_020731dc(&data_ov002_0210e488, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f188);
    data_ov002_0210e228.Construct(ASSET_DAMAGE_WAIT_BCA);
    func_020731dc(&data_ov002_0210e228, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f194);
    data_ov002_0210e498.Construct(ASSET_DOOR_PULL_BCA);
    func_020731dc(&data_ov002_0210e498, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f1b8);
    data_ov002_0210e4a0.Construct(ASSET_DOOR_PUSH_BCA);
    func_020731dc(&data_ov002_0210e4a0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f1c4);
    data_ov002_0210e808.Construct(ASSET_DROWN_DIE_BCA);
    func_020731dc(&data_ov002_0210e808, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f1dc);
    data_ov002_0210e298.Construct(ASSET_E_DOWN_BCA);
    func_020731dc(&data_ov002_0210e298, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f1e8);
    data_ov002_0210e810.Construct(0x8065);
    func_020731dc(&data_ov002_0210e810, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f200);
    data_ov002_0210e2a0.Construct(ASSET_ED_CAP_OFF_BCA);
    func_020731dc(&data_ov002_0210e2a0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f20c);
    data_ov002_0210e820.Construct(ASSET_ED_CAP_OFF_WAIT_BCA);
    func_020731dc(&data_ov002_0210e820, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f230);
    data_ov002_0210e2a8.Construct(ASSET_ED_FLY_BCA);
    func_020731dc(&data_ov002_0210e2a8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f23c);
    data_ov002_0210e1d8.Construct(ASSET_ED_FLY_START_BCA);
    func_020731dc(&data_ov002_0210e1d8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f254);
    data_ov002_0210e2b0.Construct(ASSET_ED_KISS_TO_POSE_BCA);
    func_020731dc(&data_ov002_0210e2b0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f260);
    data_ov002_0210e840.Construct(ASSET_ED_KISS_WAIT_BCA);
    func_020731dc(&data_ov002_0210e840, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f278);
    data_ov002_0210e2b8.Construct(ASSET_ED_LOOK_LEFT_BCA);
    func_020731dc(&data_ov002_0210e2b8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f284);
    data_ov002_0210e850.Construct(ASSET_ED_LOOK_LEFT_END_BCA);
    func_020731dc(&data_ov002_0210e850, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f29c);
    data_ov002_0210e4e0.Construct(ASSET_ED_LOOK_LEFT_WAIT_BCA);
    func_020731dc(&data_ov002_0210e4e0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f2a8);
    data_ov002_0210e858.Construct(ASSET_ED_NOD_BCA);
    func_020731dc(&data_ov002_0210e858, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f2c0);
    data_ov002_0210e2c8.Construct(ASSET_ED_PUTUP_WAIT_BCA);
    func_020731dc(&data_ov002_0210e2c8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f2cc);
    data_ov002_0210e948.Construct(ASSET_ED_WAIT_BCA);
    func_020731dc(&data_ov002_0210e948, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f2e4);
    data_ov002_0210e2d0.Construct(ASSET_ED_WAVE_BCA);
    func_020731dc(&data_ov002_0210e2d0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f2f0);
    data_ov002_0210e870.Construct(0x8066);
    func_020731dc(&data_ov002_0210e870, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f314);
    data_ov002_0210e2d8.Construct(0x8067);
    func_020731dc(&data_ov002_0210e2d8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f320);
    data_ov002_0210e880.Construct(ASSET_FALL_DIE_BCA);
    func_020731dc(&data_ov002_0210e880, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f32c);
    data_ov002_0210e508.Construct(ASSET_FALL_SAND_BCA);
    func_020731dc(&data_ov002_0210e508, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f338);
    data_ov002_0210e2e0.Construct(0x8068);
    func_020731dc(&data_ov002_0210e2e0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f5e4);
    data_ov002_0210e890.Construct(ASSET_FIRST_CHANGE_BCA);
    func_020731dc(&data_ov002_0210e890, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f35c);
    data_ov002_0210e2e8.Construct(0x8069);
    func_020731dc(&data_ov002_0210e2e8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f368);
    data_ov002_0210e1f0.Construct(0x806a);
    func_020731dc(&data_ov002_0210e1f0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f374);
    data_ov002_0210e1b8.Construct(ASSET_GAS_DIE_BCA);
    func_020731dc(&data_ov002_0210e1b8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f38c);
    data_ov002_0210e8a0.Construct(ASSET_GAS_WAIT_BCA);
    func_020731dc(&data_ov002_0210e8a0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f398);
    data_ov002_0210e2f8.Construct(ASSET_H_CARRY_START_BCA);
    func_020731dc(&data_ov002_0210e2f8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f3a4);
    data_ov002_0210e8a8.Construct(ASSET_H_CARRY_THROW_BCA);
    func_020731dc(&data_ov002_0210e8a8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f3bc);
    data_ov002_0210e300.Construct(ASSET_H_CARRY_WAIT_BCA);
    func_020731dc(&data_ov002_0210e300, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f3c8);
    data_ov002_0210e8c0.Construct(ASSET_H_CARRY_WALK_BCA);
    func_020731dc(&data_ov002_0210e8c0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f3d4);
    data_ov002_0210e308.Construct(0x806b);
    func_020731dc(&data_ov002_0210e308, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f3e0);
    data_ov002_0210e1a8.Construct(0x806c);
    func_020731dc(&data_ov002_0210e1a8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f3ec);
    data_ov002_0210e310.Construct(0x806d);
    func_020731dc(&data_ov002_0210e310, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f3f8);
    data_ov002_0210e8e0.Construct(0x806e);
    func_020731dc(&data_ov002_0210e8e0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f404);
    data_ov002_0210e318.Construct(0x806f);
    func_020731dc(&data_ov002_0210e318, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f410);
    data_ov002_0210e8f8.Construct(0x8070);
    func_020731dc(&data_ov002_0210e8f8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f41c);
    data_ov002_0210e320.Construct(0x8071);
    func_020731dc(&data_ov002_0210e320, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f428);
    data_ov002_0210e908.Construct(0x8072);
    func_020731dc(&data_ov002_0210e908, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f434);
    data_ov002_0210e328.Construct(0x8073);
    func_020731dc(&data_ov002_0210e328, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f440);
    data_ov002_0210e1a0.Construct(0x8074);
    func_020731dc(&data_ov002_0210e1a0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f44c);
    data_ov002_0210e330.Construct(0x8075);
    func_020731dc(&data_ov002_0210e330, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f458);
    data_ov002_0210e918.Construct(0x8076);
    func_020731dc(&data_ov002_0210e918, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f464);
    data_ov002_0210e560.Construct(0x8077);
    func_020731dc(&data_ov002_0210e560, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f470);
    data_ov002_0210e338.Construct(0x8078);
    func_020731dc(&data_ov002_0210e338, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f47c);
    data_ov002_0210e928.Construct(0x8079);
    func_020731dc(&data_ov002_0210e928, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f488);
    data_ov002_0210e568.Construct(ASSET_KEY_GET_BCA);
    func_020731dc(&data_ov002_0210e568, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f494);
    data_ov002_0210e938.Construct(ASSET_KEY_OPEN_BCA);
    func_020731dc(&data_ov002_0210e938, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f4a0);
    data_ov002_0210e940.Construct(0x807a);
    func_020731dc(&data_ov002_0210e940, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f4ac);
    data_ov002_0210e340.Construct(0x807b);
    func_020731dc(&data_ov002_0210e340, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f4b8);
    data_ov002_0210e1f8.Construct(ASSET_LIE_END_BCA);
    func_020731dc(&data_ov002_0210e1f8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f4c4);
    data_ov002_0210e950.Construct(ASSET_LIE_WAIT_BCA);
    func_020731dc(&data_ov002_0210e950, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f4d0);
    data_ov002_0210e570.Construct(0x807c);
    func_020731dc(&data_ov002_0210e570, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f4dc);
    data_ov002_0210e960.Construct(0x807d);
    func_020731dc(&data_ov002_0210e960, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f4e8);
    data_ov002_0210e968.Construct(0x8084);
    func_020731dc(&data_ov002_0210e968, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f4f4);
    data_ov002_0210e578.Construct(0x8085);
    func_020731dc(&data_ov002_0210e578, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f500);
    data_ov002_0210e208.Construct(0x8086);
    func_020731dc(&data_ov002_0210e208, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f50c);
    data_ov002_0210e978.Construct(0x8087);
    func_020731dc(&data_ov002_0210e978, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f518);
    data_ov002_0210e580.Construct(0x8088);
    func_020731dc(&data_ov002_0210e580, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f524);
    data_ov002_0210e980.Construct(ASSET_N_CARRY_LOST_BCA);
    func_020731dc(&data_ov002_0210e980, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f530);
    data_ov002_0210e588.Construct(ASSET_N_CARRY_POSE_BCA);
    func_020731dc(&data_ov002_0210e588, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f53c);
    data_ov002_0210e988.Construct(ASSET_N_CARRY_PUT_BCA);
    func_020731dc(&data_ov002_0210e988, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f548);
    data_ov002_0210e348.Construct(ASSET_N_CARRY_START_BCA);
    func_020731dc(&data_ov002_0210e348, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f554);
    data_ov002_0210e178.Construct(ASSET_N_CARRY_WAIT_BCA);
    func_020731dc(&data_ov002_0210e178, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f560);
    data_ov002_0210e200.Construct(ASSET_N_CARRY_WALK_BCA);
    func_020731dc(&data_ov002_0210e200, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f56c);
    data_ov002_0210e590.Construct(ASSET_OP_GO_BCA);
    func_020731dc(&data_ov002_0210e590, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f578);
    data_ov002_0210e9a8.Construct(ASSET_OP_WAIT_BCA);
    func_020731dc(&data_ov002_0210e9a8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f584);
    data_ov002_0210e9b0.Construct(ASSET_P_KOOPA_H_WAIT_BCA);
    func_020731dc(&data_ov002_0210e9b0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f590);
    data_ov002_0210e598.Construct(ASSET_P_KOOPA_HOLD_BCA);
    func_020731dc(&data_ov002_0210e598, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f59c);
    data_ov002_0210e188.Construct(ASSET_P_KOOPA_THROW_BCA);
    func_020731dc(&data_ov002_0210e188, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f5a8);
    data_ov002_0210e5a0.Construct(ASSET_PADDLE_1_BCA);
    func_020731dc(&data_ov002_0210e5a0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f5b4);
    data_ov002_0210e9c8.Construct(ASSET_PADDLE_2_BCA);
    func_020731dc(&data_ov002_0210e9c8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f5c0);
    data_ov002_0210e350.Construct(ASSET_PADDLE_CARRY_BCA);
    func_020731dc(&data_ov002_0210e350, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f5cc);
    data_ov002_0210e5a8.Construct(ASSET_PADDLE_END_BCA);
    func_020731dc(&data_ov002_0210e5a8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f5d8);
    data_ov002_0210e9d8.Construct(0x8089);
    func_020731dc(&data_ov002_0210e9d8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f5f0);
    data_ov002_0210e358.Construct(0x808a);
    func_020731dc(&data_ov002_0210e358, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f5fc);
    data_ov002_0210e170.Construct(ASSET_PUTON_T_CAP_BCA);
    func_020731dc(&data_ov002_0210e170, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f608);
    data_ov002_0210e9f8.Construct(ASSET_RETURN_DOWN_BCA);
    func_020731dc(&data_ov002_0210e9f8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f614);
    data_ov002_0210e360.Construct(ASSET_RETURN_NOCAP_BCA);
    func_020731dc(&data_ov002_0210e360, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f620);
    data_ov002_0210e5b8.Construct(ASSET_RETURN_STAR_BCA);
    func_020731dc(&data_ov002_0210e5b8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f62c);
    data_ov002_0210ea00.Construct(0x808b);
    func_020731dc(&data_ov002_0210ea00, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f638);
    data_ov002_0210e368.Construct(0x808c);
    func_020731dc(&data_ov002_0210e368, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f644);
    data_ov002_0210e370.Construct(0x808d);
    func_020731dc(&data_ov002_0210e370, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f650);
    data_ov002_0210ea18.Construct(0x808e);
    func_020731dc(&data_ov002_0210ea18, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f65c);
    data_ov002_0210e210.Construct(0x808f);
    func_020731dc(&data_ov002_0210e210, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f668);
    data_ov002_0210e5c8.Construct(0x8090);
    func_020731dc(&data_ov002_0210e5c8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f674);
    data_ov002_0210ea28.Construct(0x8091);
    func_020731dc(&data_ov002_0210ea28, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f680);
    data_ov002_0210e378.Construct(ASSET_S_DIE_BCA);
    func_020731dc(&data_ov002_0210e378, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f68c);
    data_ov002_0210e190.Construct(ASSET_SCALE_UP_BCA);
    func_020731dc(&data_ov002_0210e190, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f698);
    data_ov002_0210ea38.Construct(0x8092);
    func_020731dc(&data_ov002_0210ea38, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f6a4);
    data_ov002_0210e380.Construct(ASSET_SDROP_HEAD_BCA);
    func_020731dc(&data_ov002_0210e380, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f6b0);
    data_ov002_0210ea40.Construct(ASSET_SDROP_HEAD_END_BCA);
    func_020731dc(&data_ov002_0210ea40, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f6bc);
    data_ov002_0210ea48.Construct(ASSET_SDROP_HEAD_WAIT_BCA);
    func_020731dc(&data_ov002_0210ea48, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f6c8);
    data_ov002_0210e5d8.Construct(ASSET_SDROP_HIP_BCA);
    func_020731dc(&data_ov002_0210e5d8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f6d4);
    data_ov002_0210e218.Construct(ASSET_SDROP_HIP_END_BCA);
    func_020731dc(&data_ov002_0210e218, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f6e0);
    data_ov002_0210ea58.Construct(ASSET_SDROP_HIP_WAIT_BCA);
    func_020731dc(&data_ov002_0210ea58, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f6ec);
    data_ov002_0210e5e0.Construct(ASSET_SDROP_LEG_BCA);
    func_020731dc(&data_ov002_0210e5e0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f6f8);
    data_ov002_0210ea60.Construct(ASSET_SDROP_LEG_END_BCA);
    func_020731dc(&data_ov002_0210ea60, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f704);
    data_ov002_0210e5e8.Construct(ASSET_SDROP_LEG_WAIT_BCA);
    func_020731dc(&data_ov002_0210e5e8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f710);
    data_ov002_0210ea68.Construct(ASSET_SDROP_WAIT_BCA);
    func_020731dc(&data_ov002_0210ea68, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f71c);
    data_ov002_0210e388.Construct(ASSET_SDROP_WALK_BCA);
    func_020731dc(&data_ov002_0210e388, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f728);
    data_ov002_0210e198.Construct(0x8093);
    func_020731dc(&data_ov002_0210e198, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f734);
    data_ov002_0210ea78.Construct(0x8094);
    func_020731dc(&data_ov002_0210ea78, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f740);
    data_ov002_0210e390.Construct(ASSET_SHF_DIE_BCA);
    func_020731dc(&data_ov002_0210e390, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f74c);
    data_ov002_0210e5f8.Construct(0x8095);
    func_020731dc(&data_ov002_0210e5f8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f758);
    data_ov002_0210ea80.Construct(ASSET_SHOOT_BCA);
    func_020731dc(&data_ov002_0210ea80, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f764);
    data_ov002_0210e398.Construct(ASSET_SHOOT_TO_FLY_BCA);
    func_020731dc(&data_ov002_0210e398, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f770);
    data_ov002_0210ea90.Construct(0x8096);
    func_020731dc(&data_ov002_0210ea90, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f77c);
    data_ov002_0210ea98.Construct(ASSET_SLEEP_END_BCA);
    func_020731dc(&data_ov002_0210ea98, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f788);
    data_ov002_0210e608.Construct(ASSET_SLEEP_START_BCA);
    func_020731dc(&data_ov002_0210e608, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f794);
    data_ov002_0210eaa8.Construct(ASSET_SLEEP_TO_LIE_BCA);
    func_020731dc(&data_ov002_0210eaa8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f7a0);
    data_ov002_0210eab0.Construct(ASSET_SLEEP_WAIT_BCA);
    func_020731dc(&data_ov002_0210eab0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f7ac);
    data_ov002_0210e610.Construct(0x8097);
    func_020731dc(&data_ov002_0210e610, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f7b8);
    data_ov002_0210e220.Construct(0x8098);
    func_020731dc(&data_ov002_0210e220, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f7c4);
    data_ov002_0210e168.Construct(0x8099);
    func_020731dc(&data_ov002_0210e168, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f7d0);
    data_ov002_0210e180.Construct(0x809a);
    func_020731dc(&data_ov002_0210e180, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f7dc);
    data_ov002_0210e3b8.Construct(0x809b);
    func_020731dc(&data_ov002_0210e3b8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f7e8);
    data_ov002_0210e1b0.Construct(0x809c);
    func_020731dc(&data_ov002_0210e1b0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f800);
    data_ov002_0210e1c0.Construct(0x809d);
    func_020731dc(&data_ov002_0210e1c0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f80c);
    data_ov002_0210ed58.Construct(0x809e);
    func_020731dc(&data_ov002_0210ed58, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f818);
    data_ov002_0210ed60.Construct(0x809f);
    func_020731dc(&data_ov002_0210ed60, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f830);
    data_ov002_0210ed50.Construct(0x80a0);
    func_020731dc(&data_ov002_0210ed50, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f83c);
    data_ov002_0210ed48.Construct(0x80a1);
    func_020731dc(&data_ov002_0210ed48, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f848);
    data_ov002_0210ed40.Construct(0x80a2);
    func_020731dc(&data_ov002_0210ed40, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f854);
    data_ov002_0210ed38.Construct(0x80a3);
    func_020731dc(&data_ov002_0210ed38, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f860);
    data_ov002_0210ed30.Construct(ASSET_STAR_OPEN_BCA);
    func_020731dc(&data_ov002_0210ed30, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f86c);
    data_ov002_0210ed28.Construct(0x80a4);
    func_020731dc(&data_ov002_0210ed28, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f878);
    data_ov002_0210ed20.Construct(ASSET_SU_HIPAT_BCA);
    func_020731dc(&data_ov002_0210ed20, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f884);
    data_ov002_0210ed18.Construct(ASSET_SU_HIPED_BCA);
    func_020731dc(&data_ov002_0210ed18, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f890);
    data_ov002_0210ed10.Construct(ASSET_SU_HIPSR_BCA);
    func_020731dc(&data_ov002_0210ed10, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f8a8);
    data_ov002_0210ed08.Construct(ASSET_SU_JUMP_BCA);
    func_020731dc(&data_ov002_0210ed08, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f8b4);
    data_ov002_0210ecf8.Construct(ASSET_SU_JUMP_END_BCA);
    func_020731dc(&data_ov002_0210ecf8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f8c0);
    data_ov002_0210ecf0.Construct(ASSET_SU_RUN_BCA);
    func_020731dc(&data_ov002_0210ecf0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f8d8);
    data_ov002_0210ece8.Construct(ASSET_SU_WAIT_BCA);
    func_020731dc(&data_ov002_0210ece8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f8e4);
    data_ov002_0210ece0.Construct(ASSET_SU_WALK_BCA);
    func_020731dc(&data_ov002_0210ece0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f8f0);
    data_ov002_0210ecd8.Construct(ASSET_SU_WALK_END_BCA);
    func_020731dc(&data_ov002_0210ecd8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f8fc);
    data_ov002_0210ecd0.Construct(ASSET_SU_WALK_START_BCA);
    func_020731dc(&data_ov002_0210ecd0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f908);
    data_ov002_0210ecc8.Construct(ASSET_SWIM_BCA);
    func_020731dc(&data_ov002_0210ecc8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f914);
    data_ov002_0210ecc0.Construct(ASSET_SWIM_ATTACK_BCA);
    func_020731dc(&data_ov002_0210ecc0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f920);
    data_ov002_0210e768.Construct(ASSET_SWIM_CARRY_BCA);
    func_020731dc(&data_ov002_0210e768, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f938);
    data_ov002_0210eca0.Construct(ASSET_SWIM_DAMAGE_BCA);
    func_020731dc(&data_ov002_0210eca0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f944);
    data_ov002_0210ec90.Construct(ASSET_SWIM_DIE_BCA);
    func_020731dc(&data_ov002_0210ec90, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f950);
    data_ov002_0210ec88.Construct(ASSET_SWIM_IN_BCA);
    func_020731dc(&data_ov002_0210ec88, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f95c);
    data_ov002_0210ec80.Construct(ASSET_SWIM_STAR_GET_BCA);
    func_020731dc(&data_ov002_0210ec80, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f968);
    data_ov002_0210ec78.Construct(ASSET_SWIM_THROW_BCA);
    func_020731dc(&data_ov002_0210ec78, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f974);
    data_ov002_0210ec70.Construct(ASSET_SWIM_WAIT_BCA);
    func_020731dc(&data_ov002_0210ec70, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f980);
    data_ov002_0210ec68.Construct(0x80a5);
    func_020731dc(&data_ov002_0210ec68, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f98c);
    data_ov002_0210e4d8.Construct(0x80a6);
    func_020731dc(&data_ov002_0210e4d8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f998);
    data_ov002_0210e920.Construct(0x80a7);
    func_020731dc(&data_ov002_0210e920, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f9a4);
    data_ov002_0210e7c8.Construct(0x80a8);
    func_020731dc(&data_ov002_0210e7c8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f9bc);
    data_ov002_0210ead8.Construct(0x80a9);
    func_020731dc(&data_ov002_0210ead8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210f9ec);
    data_ov002_0210eb48.Construct(0x80aa);
    func_020731dc(&data_ov002_0210eb48, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fa10);
    data_ov002_0210ec18.Construct(0x80ab);
    func_020731dc(&data_ov002_0210ec18, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fa34);
    data_ov002_0210ec58.Construct(0x80ac);
    func_020731dc(&data_ov002_0210ec58, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fa58);
    data_ov002_0210ed00.Construct(0x80b3);
    func_020731dc(&data_ov002_0210ed00, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fa7c);
    data_ov002_0210ec28.Construct(0x80b4);
    func_020731dc(&data_ov002_0210ec28, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210faac);
    data_ov002_0210e7e0.Construct(0x80b5);
    func_020731dc(&data_ov002_0210e7e0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fad0);
    data_ov002_0210e7f8.Construct(0x80b6);
    func_020731dc(&data_ov002_0210e7f8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210faf4);
    data_ov002_0210e4a8.Construct(0x80b7);
    func_020731dc(&data_ov002_0210e4a8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fb18);
    data_ov002_0210e828.Construct(0x80bb);
    func_020731dc(&data_ov002_0210e828, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fb3c);
    data_ov002_0210e4c0.Construct(0x80bc);
    func_020731dc(&data_ov002_0210e4c0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fb60);
    data_ov002_0210e878.Construct(0x80be);
    func_020731dc(&data_ov002_0210e878, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fb84);
    data_ov002_0210e830.Construct(0x80bf);
    func_020731dc(&data_ov002_0210e830, (void *)SharedFilePtr_Destruct_Anim, data_ov002_0210fba8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e538, 0x8021);
    func_020731dc(&data_ov002_0210e538, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fbd8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e8d0, 0x8022);
    func_020731dc(&data_ov002_0210e8d0, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fbfc);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e8e8, ASSET_L_TX_DAMAGE_START_BTP);
    func_020731dc(&data_ov002_0210e8e8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fc20);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e540, ASSET_L_TX_DAMAGE_WAIT_BTP);
    func_020731dc(&data_ov002_0210e540, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fc44);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e958, ASSET_L_TX_ED_AMAZED_BTP);
    func_020731dc(&data_ov002_0210e958, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fc5c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e898, 0x8023);
    func_020731dc(&data_ov002_0210e898, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fc80);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e9e8, 0x8024);
    func_020731dc(&data_ov002_0210e9e8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fca4);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ea70, ASSET_L_TX_GAS_WAIT_BTP);
    func_020731dc(&data_ov002_0210ea70, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fcc8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ebd8, 0x8025);
    func_020731dc(&data_ov002_0210ebd8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fcec);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eac8, 0x8026);
    func_020731dc(&data_ov002_0210eac8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fd10);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e460, ASSET_L_TX_LIE_END_BTP);
    func_020731dc(&data_ov002_0210e460, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fd34);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e910, ASSET_L_TX_LIE_WAIT_BTP);
    func_020731dc(&data_ov002_0210e910, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fd58);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e2f0, ASSET_L_TX_RETURN_DOWN_BTP);
    func_020731dc(&data_ov002_0210e2f0, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fd7c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ea10, 0x8027);
    func_020731dc(&data_ov002_0210ea10, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fda0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e600, ASSET_L_TX_SLEEP_END_BTP);
    func_020731dc(&data_ov002_0210e600, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fdc4);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ec98, ASSET_L_TX_SLEEP_START_BTP);
    func_020731dc(&data_ov002_0210ec98, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fde8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ea88, ASSET_L_TX_SLEEP_TO_LIE_BTP);
    func_020731dc(&data_ov002_0210ea88, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fe0c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eca8, ASSET_L_TX_SLEEP_WAIT_BTP);
    func_020731dc(&data_ov002_0210eca8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fe30);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e9d0, 0x8028);
    func_020731dc(&data_ov002_0210e9d0, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fe54);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eaf0, 0x8035);
    func_020731dc(&data_ov002_0210eaf0, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fe78);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ea30, ASSET_W_TX_DAMAGE_START_BTP);
    func_020731dc(&data_ov002_0210ea30, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fe9c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e660, ASSET_W_TX_DAMAGE_WAIT_BTP);
    func_020731dc(&data_ov002_0210e660, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fec0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eb10, ASSET_W_TX_ED_AMAZED_BTP);
    func_020731dc(&data_ov002_0210eb10, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210fee4);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e670, 0x8036);
    func_020731dc(&data_ov002_0210e670, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ff08);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e680, 0x8037);
    func_020731dc(&data_ov002_0210e680, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ff2c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eb38, ASSET_W_TX_GAS_WAIT_BTP);
    func_020731dc(&data_ov002_0210eb38, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ff50);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e640, 0x8038);
    func_020731dc(&data_ov002_0210e640, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ff74);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eb58, ASSET_W_TX_LIE_END_BTP);
    func_020731dc(&data_ov002_0210eb58, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ff98);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e6a0, ASSET_W_TX_LIE_WAIT_BTP);
    func_020731dc(&data_ov002_0210e6a0, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ffbc);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e3e0, ASSET_W_TX_RETURN_DOWN_BTP);
    func_020731dc(&data_ov002_0210e3e0, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ffe0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eb70, 0x8039);
    func_020731dc(&data_ov002_0210eb70, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ed80);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e6b8, ASSET_W_TX_SLEEP_END_BTP);
    func_020731dc(&data_ov002_0210e6b8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210eda4);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eb90, ASSET_W_TX_SLEEP_START_BTP);
    func_020731dc(&data_ov002_0210eb90, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210edc8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e620, ASSET_W_TX_SLEEP_TO_LIE_BTP);
    func_020731dc(&data_ov002_0210e620, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210edec);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e3e8, ASSET_W_TX_SLEEP_WAIT_BTP);
    func_020731dc(&data_ov002_0210e3e8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ee10);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eba8, 0x803a);
    func_020731dc(&data_ov002_0210eba8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ee34);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e3c8, 0x8049);
    func_020731dc(&data_ov002_0210e3c8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ee58);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e6e0, ASSET_Y_TX_DAMAGE_START_BTP);
    func_020731dc(&data_ov002_0210e6e0, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ee7c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ebc8, ASSET_Y_TX_DAMAGE_WAIT_BTP);
    func_020731dc(&data_ov002_0210ebc8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210eea0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e400, 0x804a);
    func_020731dc(&data_ov002_0210e400, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210eeb8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ecb8, 0x804b);
    func_020731dc(&data_ov002_0210ecb8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210eedc);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e708, ASSET_Y_TX_GAS_WAIT_BTP);
    func_020731dc(&data_ov002_0210e708, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ef00);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e408, 0x804c);
    func_020731dc(&data_ov002_0210e408, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ef24);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ec00, ASSET_Y_TX_LIE_END_BTP);
    func_020731dc(&data_ov002_0210ec00, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ef48);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ec10, ASSET_Y_TX_LIE_WAIT_BTP);
    func_020731dc(&data_ov002_0210ec10, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ef6c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e728, ASSET_Y_TX_RETURN_DOWN_BTP);
    func_020731dc(&data_ov002_0210e728, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210ef90);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e3b0, 0x804d);
    func_020731dc(&data_ov002_0210e3b0, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210efa8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e738, ASSET_Y_TX_SLEEP_END_BTP);
    func_020731dc(&data_ov002_0210e738, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210efcc);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ec40, ASSET_Y_TX_SLEEP_START_BTP);
    func_020731dc(&data_ov002_0210ec40, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210eff0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e250, ASSET_Y_TX_SLEEP_TO_LIE_BTP);
    func_020731dc(&data_ov002_0210e250, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f014);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e430, 0x804e);
    func_020731dc(&data_ov002_0210e430, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f038);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e438, 0x804f);
    func_020731dc(&data_ov002_0210e438, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f05c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e770, 0x80ad);
    func_020731dc(&data_ov002_0210e770, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f080);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e788, ASSET_TX_DAMAGE_START_BTP);
    func_020731dc(&data_ov002_0210e788, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f0a4);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e450, ASSET_TX_DAMAGE_WAIT_BTP);
    func_020731dc(&data_ov002_0210e450, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f0c8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e270, ASSET_TX_ED_KISS_TO_POSE_BTP);
    func_020731dc(&data_ov002_0210e270, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f0ec);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e458, ASSET_TX_ED_KISS_WAIT_BTP);
    func_020731dc(&data_ov002_0210e458, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f110);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e288, 0x80ae);
    func_020731dc(&data_ov002_0210e288, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f134);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e478, 0x80af);
    func_020731dc(&data_ov002_0210e478, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f158);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e7e8, ASSET_TX_GAS_WAIT_BTP);
    func_020731dc(&data_ov002_0210e7e8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f17c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e230, 0x80b0);
    func_020731dc(&data_ov002_0210e230, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f1a0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e800, ASSET_TX_LIE_END_BTP);
    func_020731dc(&data_ov002_0210e800, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f1d0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e1d0, ASSET_TX_LIE_WAIT_BTP);
    func_020731dc(&data_ov002_0210e1d0, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f1f4);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e4b8, ASSET_TX_RETURN_DOWN_BTP);
    func_020731dc(&data_ov002_0210e4b8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f218);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e1c8, 0x80b1);
    func_020731dc(&data_ov002_0210e1c8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f248);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e4d0, ASSET_TX_SLEEP_END_BTP);
    func_020731dc(&data_ov002_0210e4d0, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f26c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e1e0, ASSET_TX_SLEEP_START_BTP);
    func_020731dc(&data_ov002_0210e1e0, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f290);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e4e8, ASSET_TX_SLEEP_TO_LIE_BTP);
    func_020731dc(&data_ov002_0210e4e8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f2b4);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e1e8, ASSET_TX_SLEEP_WAIT_BTP);
    func_020731dc(&data_ov002_0210e1e8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f2d8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e500, 0x80b2);
    func_020731dc(&data_ov002_0210e500, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_0210f2fc);
    data_ov002_0211013c.mInit = data_ov002_02109e0c;
    data_ov002_0211013c.mMain = data_ov002_02109f74;
    data_ov002_0211013c.mCleanup = 0;
    Player::ST_WAIT.mInit = data_ov002_0210a25c;
    Player::ST_WAIT.mMain = data_ov002_02109e14;
    Player::ST_WAIT.mCleanup = data_ov002_02109f84;
    data_ov002_02110184.mCleanup = 0;
    data_ov002_02110184.mInit = data_ov002_02109f94;
    data_ov002_02110184.mMain = data_ov002_02109e1c;
    data_ov002_0211019c.mInit = data_ov002_02109fa4;
    data_ov002_0211019c.mCleanup = 0;
    data_ov002_0211019c.mMain = data_ov002_02109fac;
    data_ov002_021101b4.mCleanup = 0;
    data_ov002_021101b4.mInit = data_ov002_02109dec;
    data_ov002_021101b4.mMain = data_ov002_0210a26c;
    data_ov002_021101e4.mInit = data_ov002_02109fb4;
    data_ov002_021101e4.mCleanup = 0;
    data_ov002_021101e4.mMain = data_ov002_0210a27c;
    data_ov002_021101fc.mCleanup = 0;
    data_ov002_021101fc.mInit = data_ov002_02109fbc;
    data_ov002_021101fc.mMain = data_ov002_02109fc4;
    data_ov002_02110214.mInit = data_ov002_0210a214;
    data_ov002_02110214.mCleanup = 0;
    data_ov002_02110214.mMain = data_ov002_02109fd4;
    Player::ST_OWL.mInit = data_ov002_02109e9c;
    Player::ST_OWL.mMain = data_ov002_02109fdc;
    Player::ST_OWL.mCleanup = data_ov002_0210a2a4;
    data_ov002_02110274.mCleanup = 0;
    data_ov002_02110274.mInit = data_ov002_02109eac;
    data_ov002_02110274.mMain = data_ov002_0210a2b4;
    data_ov002_0211028c.mInit = data_ov002_0210a2bc;
    data_ov002_0211028c.mMain = data_ov002_02109fec;
    data_ov002_0211028c.mCleanup = data_ov002_0210a1e4;
    data_ov002_021102bc.mCleanup = 0;
    data_ov002_021102bc.mInit = data_ov002_02109e94;
    data_ov002_021102bc.mMain = data_ov002_0210a2cc;
    data_ov002_021102d4.mInit = data_ov002_02109ffc;
    data_ov002_021102d4.mMain = data_ov002_0210a1c4;
    data_ov002_021102d4.mCleanup = data_ov002_02109ebc;
    data_ov002_02110304.mInit = data_ov002_0210a2e4;
    data_ov002_02110304.mMain = data_ov002_0210a004;
    data_ov002_02110304.mCleanup = data_ov002_0210a2f4;
    data_ov002_0211031c.mCleanup = 0;
    data_ov002_0211031c.mInit = data_ov002_0210a00c;
    data_ov002_0211031c.mMain = data_ov002_02109e3c;
    data_ov002_02110334.mInit = data_ov002_0210a18c;
    data_ov002_02110334.mCleanup = 0;
    data_ov002_02110334.mMain = data_ov002_0210a304;
    data_ov002_02110364.mInit = data_ov002_0210a01c;
    data_ov002_02110364.mMain = data_ov002_02109e44;
    data_ov002_02110364.mCleanup = data_ov002_0210a314;
    data_ov002_02110394.mCleanup = 0;
    data_ov002_02110394.mInit = data_ov002_0210a31c;
    data_ov002_02110394.mMain = data_ov002_0210a324;
    data_ov002_021103ac.mInit = data_ov002_0210a164;
    data_ov002_021103ac.mCleanup = 0;
    data_ov002_021103ac.mMain = data_ov002_02109dd4;
    data_ov002_021103dc.mCleanup = 0;
    data_ov002_021103dc.mInit = data_ov002_0210a33c;
    data_ov002_021103dc.mMain = data_ov002_0210a034;
    data_ov002_021103f4.mInit = data_ov002_02109ec4;
    data_ov002_021103f4.mCleanup = 0;
    data_ov002_021103f4.mMain = data_ov002_0210a03c;
    data_ov002_02110424.mCleanup = 0;
    data_ov002_02110424.mInit = data_ov002_02109ecc;
    data_ov002_02110424.mMain = data_ov002_0210a35c;
    data_ov002_0211043c.mInit = data_ov002_0210a044;
    data_ov002_0211043c.mCleanup = 0;
    data_ov002_0211043c.mMain = data_ov002_02109e4c;
    data_ov002_02110454.mInit = data_ov002_0210a04c;
    data_ov002_02110454.mMain = data_ov002_0210a37c;
    data_ov002_02110454.mCleanup = data_ov002_0210a384;
    data_ov002_02110484.mInit = data_ov002_02109e54;
    data_ov002_02110484.mMain = data_ov002_02109ed4;
    data_ov002_02110484.mCleanup = data_ov002_0210a38c;
    data_ov002_0211049c.mCleanup = 0;
    data_ov002_0211049c.mInit = data_ov002_0210a05c;
    data_ov002_0211049c.mMain = data_ov002_0210a394;
    data_ov002_021104cc.mInit = data_ov002_02109ddc;
    data_ov002_021104cc.mCleanup = 0;
    data_ov002_021104cc.mMain = data_ov002_0210a0f4;
    data_ov002_021104e4.mCleanup = 0;
    data_ov002_021104e4.mInit = data_ov002_02109de4;
    data_ov002_021104e4.mMain = data_ov002_0210a54c;
    data_ov002_02110514.mInit = data_ov002_02109df4;
    data_ov002_02110514.mCleanup = 0;
    data_ov002_02110514.mMain = data_ov002_0210a0d4;
    data_ov002_0211052c.mCleanup = 0;
    data_ov002_0211052c.mInit = data_ov002_0210a544;
    data_ov002_0211052c.mMain = data_ov002_0210a53c;
    data_ov002_0211055c.mInit = data_ov002_0210a52c;
    data_ov002_0211055c.mCleanup = 0;
    data_ov002_0211055c.mMain = data_ov002_0210a524;
    data_ov002_02110574.mCleanup = 0;
    data_ov002_02110574.mInit = data_ov002_0210a514;
    data_ov002_02110574.mMain = data_ov002_0210a50c;
    data_ov002_021105a4.mInit = data_ov002_0210a504;
    data_ov002_021105a4.mMain = data_ov002_0210a4fc;
    data_ov002_021105a4.mCleanup = data_ov002_0210a4f4;
    data_ov002_021105bc.mCleanup = 0;
    data_ov002_021105bc.mInit = data_ov002_0210a4ec;
    data_ov002_021105bc.mMain = data_ov002_0210a4e4;
    data_ov002_021105d4.mInit = data_ov002_0210a0ac;
    data_ov002_021105d4.mCleanup = 0;
    data_ov002_021105d4.mMain = data_ov002_02109f14;
    data_ov002_021105ec.mInit = data_ov002_0210a4d4;
    data_ov002_021105ec.mMain = data_ov002_0210a4cc;
    data_ov002_021105ec.mCleanup = data_ov002_0210a4c4;
    data_ov002_02110604.mInit = data_ov002_0210a4b4;
    data_ov002_02110604.mMain = data_ov002_0210a494;
    data_ov002_02110604.mCleanup = data_ov002_0210a414;
    data_ov002_0211061c.mCleanup = 0;
    data_ov002_0211061c.mInit = data_ov002_0210a1dc;
    data_ov002_0211061c.mMain = data_ov002_0210a3cc;
    data_ov002_02110634.mInit = data_ov002_0210a49c;
    data_ov002_02110634.mCleanup = 0;
    data_ov002_02110634.mMain = data_ov002_0210a09c;
    data_ov002_0211064c.mInit = data_ov002_02109e6c;
    data_ov002_0211064c.mMain = data_ov002_0210a0e4;
    data_ov002_0211064c.mCleanup = data_ov002_0210a13c;
    data_ov002_02110664.mInit = data_ov002_0210a17c;
    data_ov002_02110664.mMain = data_ov002_0210a1a4;
    data_ov002_02110664.mCleanup = data_ov002_02109f5c;
    data_ov002_0211067c.mInit = data_ov002_0210a204;
    data_ov002_0211067c.mMain = data_ov002_0210a224;
    data_ov002_0211067c.mCleanup = data_ov002_0210a24c;
    data_ov002_02110694.mCleanup = 0;
    data_ov002_02110694.mInit = data_ov002_0210a42c;
    data_ov002_02110694.mMain = data_ov002_0210a2fc;
    data_ov002_021106ac.mInit = data_ov002_0210a2ec;
    data_ov002_021106ac.mMain = data_ov002_0210a0a4;
    data_ov002_021106ac.mCleanup = data_ov002_0210a194;
    data_ov002_021106c4.mInit = data_ov002_0210a294;
    data_ov002_021106c4.mMain = data_ov002_02109eb4;
    data_ov002_021106c4.mCleanup = data_ov002_02109e84;
    data_ov002_021106dc.mInit = data_ov002_0210a3a4;
    data_ov002_021106dc.mMain = data_ov002_0210a39c;
    data_ov002_021106dc.mCleanup = data_ov002_0210a3bc;
    data_ov002_021106f4.mInit = data_ov002_0210a2c4;
    data_ov002_021106f4.mMain = data_ov002_0210a3d4;
    data_ov002_021106f4.mCleanup = data_ov002_0210a3e4;
    data_ov002_0211070c.mCleanup = 0;
    data_ov002_0211070c.mInit = data_ov002_0210a3ec;
    data_ov002_0211070c.mMain = data_ov002_0210a3b4;
    data_ov002_02110724.mInit = data_ov002_0210a2ac;
    data_ov002_02110724.mCleanup = 0;
    data_ov002_02110724.mMain = data_ov002_02109ef4;
    data_ov002_0210ffec.mInit = data_ov002_0210a41c;
    data_ov002_0210ffec.mMain = data_ov002_0210a06c;
    data_ov002_0210ffec.mCleanup = data_ov002_02109eec;
    data_ov002_02110004.mCleanup = 0;
    data_ov002_02110004.mInit = data_ov002_0210a43c;
    data_ov002_02110004.mMain = data_ov002_0210a444;
    data_ov002_0211001c.mInit = data_ov002_0210a454;
    data_ov002_0211001c.mCleanup = 0;
    data_ov002_0211001c.mMain = data_ov002_0210a464;
    data_ov002_02110034.mInit = data_ov002_02109ee4;
    data_ov002_02110034.mMain = data_ov002_02109efc;
    data_ov002_02110034.mCleanup = data_ov002_0210a484;
    data_ov002_0211004c.mCleanup = 0;
    data_ov002_0211004c.mInit = data_ov002_02109f04;
    data_ov002_0211004c.mMain = data_ov002_0210a32c;
    data_ov002_02110064.mInit = data_ov002_0210a4a4;
    data_ov002_02110064.mMain = data_ov002_0210a4ac;
    data_ov002_02110064.mCleanup = data_ov002_0210a08c;
    data_ov002_0211007c.mCleanup = 0;
    data_ov002_0211007c.mInit = data_ov002_02109e74;
    data_ov002_0211007c.mMain = data_ov002_02109e7c;
    data_ov002_02110094.mInit = data_ov002_02109f1c;
    data_ov002_02110094.mMain = data_ov002_02109f24;
    data_ov002_02110094.mCleanup = data_ov002_02109edc;
    data_ov002_021100ac.mCleanup = 0;
    data_ov002_021100ac.mInit = data_ov002_02109f2c;
    data_ov002_021100ac.mMain = data_ov002_0210a2d4;
    data_ov002_021100c4.mInit = data_ov002_02109e8c;
    data_ov002_021100c4.mCleanup = 0;
    data_ov002_021100c4.mMain = data_ov002_02109f34;
    data_ov002_021100dc.mCleanup = 0;
    data_ov002_021100dc.mInit = data_ov002_02109f3c;
    data_ov002_021100dc.mMain = data_ov002_02109e5c;
    data_ov002_021100f4.mInit = data_ov002_0210a16c;
    data_ov002_021100f4.mCleanup = 0;
    data_ov002_021100f4.mMain = data_ov002_02109f4c;
    data_ov002_0211010c.mCleanup = 0;
    data_ov002_0211010c.mInit = data_ov002_0210a28c;
    data_ov002_0211010c.mMain = data_ov002_0210a1bc;
    data_ov002_02110124.mInit = data_ov002_02109e04;
    data_ov002_02110124.mCleanup = 0;
    data_ov002_02110124.mMain = data_ov002_0210a4dc;
    data_ov002_0211016c.mInit = data_ov002_02109f7c;
    data_ov002_0211016c.mMain = data_ov002_0210a254;
    data_ov002_0211016c.mCleanup = data_ov002_02109f8c;
    data_ov002_021101cc.mInit = data_ov002_02109f9c;
    data_ov002_021101cc.mMain = data_ov002_02109e24;
    data_ov002_021101cc.mCleanup = data_ov002_0210a234;
    data_ov002_0211022c.mInit = data_ov002_0210a284;
    data_ov002_0211022c.mMain = data_ov002_0210a21c;
    data_ov002_0211022c.mCleanup = data_ov002_02109fcc;
    data_ov002_0211025c.mCleanup = 0;
    data_ov002_0211025c.mInit = data_ov002_0210a20c;
    data_ov002_0211025c.mMain = data_ov002_02109ea4;
    data_ov002_021102a4.mInit = data_ov002_0210a1f4;
    data_ov002_021102a4.mCleanup = 0;
    data_ov002_021102a4.mMain = data_ov002_02109f6c;
    data_ov002_021102ec.mInit = data_ov002_02109e34;
    data_ov002_021102ec.mMain = data_ov002_02109f64;
    data_ov002_021102ec.mCleanup = data_ov002_0210a2dc;
    data_ov002_0211034c.mInit = data_ov002_0210a1ac;
    data_ov002_0211034c.mMain = data_ov002_0210a19c;
    data_ov002_0211034c.mCleanup = data_ov002_0210a014;
    data_ov002_0211037c.mCleanup = 0;
    data_ov002_0211037c.mInit = data_ov002_0210a184;
    data_ov002_0211037c.mMain = data_ov002_0210a024;
    data_ov002_021103c4.mInit = data_ov002_0210a02c;
    data_ov002_021103c4.mCleanup = 0;
    data_ov002_021103c4.mMain = data_ov002_0210a15c;
    data_ov002_0211040c.mCleanup = 0;
    data_ov002_0211040c.mInit = data_ov002_0210a344;
    data_ov002_0211040c.mMain = data_ov002_0210a354;
    data_ov002_0211046c.mInit = data_ov002_0210a364;
    data_ov002_0211046c.mMain = data_ov002_0210a374;
    data_ov002_0211046c.mCleanup = data_ov002_0210a11c;
    data_ov002_021104b4.mInit = data_ov002_0210a114;
    data_ov002_021104b4.mMain = data_ov002_0210a104;
    data_ov002_021104b4.mCleanup = data_ov002_0210a0fc;
    data_ov002_021104fc.mCleanup = 0;
    data_ov002_021104fc.mInit = data_ov002_02109e64;
    data_ov002_021104fc.mMain = data_ov002_0210a0ec;
    data_ov002_02110544.mInit = data_ov002_0210a0cc;
    data_ov002_02110544.mCleanup = 0;
    data_ov002_02110544.mMain = data_ov002_0210a0c4;
    data_ov002_0211058c.mInit = data_ov002_0210a51c;
    data_ov002_0211058c.mCleanup = 0;
    data_ov002_0211058c.mMain = data_ov002_0210a0bc;
    func_ov002_020e6ad4(data_ov002_02110804);
    data_ov002_0211073c[0] = data_ov002_02109f0c;
    data_ov002_0211073c[1] = data_ov002_0210a47c;
    data_ov002_0211073c[2] = data_ov002_0210a10c;
    data_ov002_0211073c[3] = data_ov002_0210a45c;
    data_ov002_02110884[0] = data_ov002_02109dfc;
    data_ov002_02110884[1] = data_ov002_0210a264;
    data_ov002_02110884[2] = data_ov002_0210a244;
    data_ov002_02110884[3] = data_ov002_0210a23c;
    data_ov002_02110884[4] = data_ov002_0210a274;
    data_ov002_02110884[5] = data_ov002_0210a22c;
    data_ov002_02110884[6] = data_ov002_0210a29c;
    data_ov002_02110884[7] = data_ov002_0210a1fc;
    data_ov002_02110884[8] = data_ov002_0210a1ec;
    data_ov002_02110884[9] = data_ov002_02109e2c;
    data_ov002_02110884[10] = data_ov002_0210a1cc;
    data_ov002_02110884[11] = data_ov002_0210a1b4;
    data_ov002_02110884[12] = data_ov002_02109f54;
    data_ov002_02110884[13] = data_ov002_0210a30c;
    data_ov002_02110884[14] = data_ov002_0210a174;
    data_ov002_02110884[15] = data_ov002_02109f44;
    data_ov002_02110884[16] = data_ov002_0210a154;
    data_ov002_02110884[17] = data_ov002_0210a144;
    data_ov002_02110884[18] = data_ov002_0210a134;
    data_ov002_02110884[19] = data_ov002_0210a12c;
    data_ov002_0211075c[0] = data_ov002_0210a48c;
    data_ov002_0211075c[1] = data_ov002_0210a46c;
    data_ov002_0211075c[2] = data_ov002_0210a3ac;
    data_ov002_0211075c[3] = data_ov002_0210a074;
    data_ov002_0211075c[4] = data_ov002_0210a334;
    data_ov002_0211075c[5] = data_ov002_0210a404;
    data_ov002_0211075c[6] = data_ov002_0210a3f4;
}
}

#pragma section initsec end
