//cpp
/* Castle key and the last Power Star -- ov089/daObjKey_c.
 * OBJ_KEY is actor 0x11a (282); LAST_STAR is 0x11b (283). Both factories
 * install this vtable. param1 & 7 picks the kind: 7 uses the power-star
 * model (data_ov002_0211094c, file 0x8015) and func_ov089_0213162c; 3 uses
 * func_ov089_021311c0; the other six kinds are StateDrop. Those two bodies,
 * the destructors, and the factories sit outside this run and stay there.
 *
 * deslop leftovers:
 * - StateDrop: mWithMeshClsn.UpdateContinuous() is the same size (0x2b4) but
 *   the reloc target is _ZN10dBgCh_Actr16UpdateContinuousEv. The ROM calls the
 *   veneer at arm9:0x020383fc.
 * - StateDrop: Particle::System::New (Fix12<int> x/y/z) size 0x2b4 -> 0x2c8
 *   (+0x14). Behavior's two calls size 0x310 -> 0x344 (+0x34). The header
 *   does not declare New, and a bare int does not convert to Fix12<int>.
 * - UpdateModelTransform: dActor_c::DropShadowRadHeight with Fix12<int>
 *   radius/depth size 0x130 -> 0x140 (+0x10). A bare int does not compile.
 * - InitResources: ModelAnim::SetAnim, both calls, Fix12<int> speed, size
 *   0x32c -> 0x344 (+0x18). dCcAcPos_c::Init, both calls, Fix12<int>
 *   radius/height, size 0x32c -> 0x34c (+0x20).
 * - InitResources: mWithMeshClsn.Init(...) links to the undefined
 *   _ZN10dBgCh_Actr4InitEP8dActor_ciiP10Vector3_16S3_: dBgCh_Actr.h spells
 *   the radii Fix12i, a plain s32, so the ROM's Fix12<int> mangling is
 *   called by name.
 * - func_ov089_02131df4: one control arm (mState != 7) size 0x110 -> 0xe8
 *   (-0x28). The ROM has both copies.
 *   func_ov089_02131dcc / func_ov089_02131df4 stay C names: the shards call
 *   them that way. common.h's flat Matrix4x3 keeps the translation in m[9..11].
 *   The carry sparkle reads the first bone's word at +0xc; BMD_Bone does not
 *   name it. g_profile_OBJ_KEY / LAST_STAR stay outside this TU.
 *   data_0209f318 as Camera * matches, but the plurality is void * so the
 *   cast stays.
 */

#include "types.h"
#include "common.h"
#include "decl_common.h"
#include "daObjKey_c.h"
#include "Player.h"
#include "Camera.h"
#include "Sound.h"
#include "SharedFilePtr.h"

#pragma defer_codegen off

namespace Event { void SetBit(unsigned int bit); int ClearBit(unsigned int bit); }

namespace Sound {
void LoadAndSetMusic_Layer3(unsigned int musicId);
}

/* {file id, loaded file}. Same two words PowerStar reads as id/ptr, and the
 * words this TU passes to SetAnim / SetFile / compares with mModelAnim.file.
 * SharedFilePtr itself has no fields; Release and LoadFile go through it. */
struct ObjKeyFile {
    u32 id;
    void *ptr;
};

enum {
    ACTOR_OBJ_KEY = 0x11a,
    KEY_KIND_STAR = 7,
    ANIM_CARRY = 3,
    EVENT_KEY = 0x1d,
    FLAG_CAM_TAKEOVER = 0x4000000,
    FLAG_HIDE = 0x40000,
    HIT_PLAYER = 0x400000,
    CC_DISABLED = 1,
    FX_FALL = 0x81,
    FX_BURST_A = 0x82,
    FX_BURST_B = 0x83,
    SND_LAND = 0x36,
    SND_APPEAR = 0x57,
    MUSIC_KEY = 0x17,
    Y_LOOK = 0x64000,
    Y_SPARK = 0x1c000,
    Y_BONE = 0x48000,
    SPIN_FAST = 0x400,
    SPIN_STEP = 0x100,
    SCALE_STAR = 0x2000,
    SCALE_KEY = 0x3000,
    GRAVITY = -0x2000,
    TERMINAL_VY = -0x32000,
    POP_VY = 0x23000,
    STAR_CLSN_R = 0xa0000,
    KEY_CLSN_R = 0x48000,
    STAR_CC_H = 0xfa000,
    KEY_CC_H = 0x64000,
    CC_R = 0x50000,
    CC_FLAGS = 0x800003,
    CC_VULN = 0x8000,
    SHADOW_R = 0x96000,
    SHADOW_DEPTH = 0x3e8000,
    SHADOW_OPACITY = 0xf,
    BONE_Y_MUL = 0x23,
    ANIM_FLAGS = 0x40000000,
    ANIM_SPEED = 0x1000
};

/* Filled by __sinit_ov089_021328d4 and indexed by mState. */
typedef void (daObjKey_c::*StateFunc)();
extern StateFunc data_ov089_02132cec[];

/* local extern: Camera.h has no SetFlag_3. Particle::System::New,
 * DropShadowRadHeight, ModelAnim::SetAnim and dCcAcPos_c::Init take
 * Fix12<int> by value; the scalar call does not compile and the Fix12
 * temporary size-DIFFs (see the file comment). The ROM calls the
 * UpdateContinuous veneer, not the method. */
extern "C" {
extern void dBgCh_Actr_UpdateContinuous_Veneer(char *p);
extern void *data_0209f318;
extern int data_0209b454;
extern void _ZN6Camera9SetFlag_3Ev(Camera *cam);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, int c, int d, int e, void *f, void *g);
extern void func_02012694(unsigned int id, const Vector3 *pos);
extern void func_ov002_020c3dbc(void *player);
extern int data_0209caa0[];
void Matrix4x3_FromRotationY(void *m, short angle);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *a, void *sm, void *mtx, int rad, int h, unsigned int x);
extern Matrix4x3 IDENTITY_MATRIX4X3;
extern void *data_ov089_021328b4[];
extern ObjKeyFile data_ov002_02110964;
extern ObjKeyFile data_ov089_02132c60;
extern ObjKeyFile data_ov089_02132c40;
extern ObjKeyFile data_ov089_02132c70;
extern ObjKeyFile data_ov089_02132c48;
extern Vector3 data_ov089_02132b40;
extern Vector3 data_ov089_02132ca4;
extern Matrix4x3 data_020a0e68;
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, int ang);
extern void MulMat4x3Mat4x3(void *d, void *a, void *b);
extern void SubVec3(void *d, void *a, void *b);
extern void Vec3_LslInPlace(void *v, int sh);
extern void AddVec3(void *d, void *a, void *b);
extern void LoadKeyModels(int idx);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *bca, int a, int fx, unsigned int f);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *thiz, void *actor, void *pos, int r, int s, unsigned int a, unsigned int b);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *thiz, void *actor, int a, int b, void *v, void *w);
/* An ObjKeyFile in layout, but CutsceneObject::InitResources declares it `char`;
   keep that spelling so the declarations of this symbol stay in agreement. */
extern char data_ov002_0211094c;
extern int data_0209cef0;
void func_ov089_02131df4(char *c, char *p);
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c9StateDropEv, 0x02131b18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c9StateDropEv
void daObjKey_c::StateDrop()
{
    Camera *cam = (Camera *)data_0209f318;
    Vector3 v;

    dBgCh_Actr_UpdateContinuous_Veneer((char *)&mWithMeshClsn);

    switch (mStep) {
    case 0:
        {
            /* Pointer copies: indexing cam directly folds the offsets into the loads. */
            Vector3 *lookAt = &cam->lookAt;
            Vector3 *pos = &cam->pos;
            mFlags |= FLAG_CAM_TAKEOVER;
            data_0209b454 |= FLAG_CAM_TAKEOVER;
            mSavedCamLookAt.x = lookAt->x;
            mSavedCamLookAt.y = lookAt->y;
            mSavedCamLookAt.z = lookAt->z;
            mSavedCamPos.x = pos->x;
            mSavedCamPos.y = pos->y;
            mSavedCamPos.z = pos->z;
            _ZN6Camera9SetFlag_3Ev(cam);
            v.x = mPosX;
            v.y = mPosY;
            v.z = mPosZ;
            v.y = v.y + Y_LOOK;
            cam->SetLookAt(v);
            mStep++;
            return;
        }

    case 1:
        /* Offset after the copy: mPosY + Y_LOOK in the copy reorders the loads. */
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        v.y = v.y + Y_LOOK;
        cam->SetLookAt(v);
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        v.y = v.y + Y_SPARK;
        mParticleID[0] = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            mParticleID[0], FX_FALL, v.x, v.y, v.z, 0, 0);
        if (mWithMeshClsn.JustHitGround()) {
            mVertSpeed = -mVertSpeed >> 1;
            func_02012694(SND_LAND, (Vector3 *)&mCamSpacePosX);
            return;
        }
        if (mWithMeshClsn.IsOnGround() == 0)
            return;
        cam->SetLookAt(mSavedCamLookAt);
        cam->SetPos(mSavedCamPos);
        cam->mFlags &= ~8;
        mFlags &= ~FLAG_CAM_TAKEOVER;
        data_0209b454 &= ~FLAG_CAM_TAKEOVER;
        mStep++;
        mdCcAcPos_c.flags &= ~CC_DISABLED;
        mParticleID[0] = 0;
        return;

    case 2:
        {
            u32 id = mdCcAcPos_c.otherOwner;
            dActor_c *found;
            if (id == 0)
                return;
            found = dActor_c::FindWithID(id);
            if (found == 0)
                return;
            if ((mdCcAcPos_c.hitFlags & HIT_PLAYER) == 0)
                return;
            func_ov089_02131df4((char *)this, (char *)found);
            mStep++;
            return;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* func_ov089_02131dcc, 0x02131dcc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov089_02131dcc
/* Last-star path. (char *, char *) is the spelling the shards declare. */
extern "C" void func_ov089_02131dcc(char *c, char *p)
{
    daObjKey_c *key = (daObjKey_c *)c;

    func_ov002_020c3dbc(p);
    Event::SetBit(EVENT_KEY);
    key->MarkForDestruction();
}

/* -------------------------------------------------------------------------- */
/* func_ov089_02131df4, 0x02131df4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov089_02131df4
extern "C" void func_ov089_02131df4(char *c, char *p)
{
    daObjKey_c *key = (daObjKey_c *)c;
    Player *player = (Player *)p;

    /* Word 1, bit (2 << kind): already collected. 1 asks Stage/dMeter/Player
     * for the new-star fanfare; 0 suppresses it. Kind 7 takes neither arm.
     * One arm is shorter (0x110 -> 0xe8); the ROM has both. */
    if (data_0209caa0[1] & (2 << key->mState))
        data_0209f2ac = 0;
    else
        data_0209f2ac = 1;
    data_0209caa0[1] |= (2 << key->mState);

    if (key->mState <= 1) {
        player->SetNoControlState(3, -1, 0);
        Sound::LoadAndSetMusic_Layer3(MUSIC_KEY);
    } else if (key->mState != KEY_KIND_STAR) {
        player->SetNoControlState(3, -1, 0);
        Sound::LoadAndSetMusic_Layer3(MUSIC_KEY);
    }

    func_ov089_0213115c((char *)key, ANIM_CARRY);
    key->mPlayer = player;
    {
        /* Through a pointer: key->mPlayer->mPosX folds the offset into each load. */
        s32 *pos = &key->mPlayer->mPosX;
        key->mPosX = pos[0];
        key->mPosY = pos[1];
        key->mPosZ = pos[2];
        key->mAngleY = key->mPlayer->mAngleY;
        key->mFlags &= ~FLAG_HIDE;
        Event::SetBit(EVENT_KEY);
    }
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c13OnTurnIntoEggER6Player, 0x02131f04 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c13OnTurnIntoEggER6Player
void daObjKey_c::OnTurnIntoEgg(Player &player)
{
    /* The flag keeps the ROM's moveq and movne pair. */
    unsigned isKey = (actorID == ACTOR_OBJ_KEY);
    if (isKey)
        return func_ov089_02131df4((char *)this, (char *)&player);
    return func_ov089_02131dcc((char *)this, (char *)&player);
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c13OnYoshiTryEatEv, 0x02131f4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c13OnYoshiTryEatEv
s32 daObjKey_c::OnYoshiTryEat() {
    return 4;
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c20UpdateModelTransformEv, 0x02131f54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c20UpdateModelTransformEv
void daObjKey_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
    mModelAnim.mat4x3.m[9] = mPosX >> 3;
    mModelAnim.mat4x3.m[10] = mPosY >> 3;
    mModelAnim.mat4x3.m[11] = mPosZ >> 3;
    if (data_ov089_021328b4[mState] != 0 && mAnimID == 0) {
        Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
        mModel.mat4x3.m[9] = mPosX >> 3;
        mModel.mat4x3.m[10] = (mPosY + Y_LOOK) >> 3;
        mModel.mat4x3.m[11] = mPosZ >> 3;
    }
    mShadowMatrix = IDENTITY_MATRIX4X3;
    mShadowMatrix.m[9] = mPosX >> 3;
    mShadowMatrix.m[10] = mPosY >> 3;
    mShadowMatrix.m[11] = mPosZ >> 3;
    /* Shadow only while the idle anim (file 0x801b) is showing. */
    if (mModelAnim.file == (BCA_File *)data_ov002_02110964.ptr)
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(this, &mShadowModel, &mShadowMatrix, SHADOW_R, SHADOW_DEPTH, SHADOW_OPACITY);
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c16CleanupResourcesEv, 0x02132084 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c16CleanupResourcesEv
int daObjKey_c::CleanupResources()
{
    UnloadKeyModels(mState);
    ((SharedFilePtr *)&data_ov002_02110964)->Release();
    if (mState != KEY_KIND_STAR) {
        ((SharedFilePtr *)&data_ov089_02132c60)->Release();
        ((SharedFilePtr *)&data_ov089_02132c40)->Release();
        ((SharedFilePtr *)&data_ov089_02132c70)->Release();
        ((SharedFilePtr *)&data_ov089_02132c48)->Release();
    }
    Event::ClearBit(EVENT_KEY);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c6RenderEv, 0x021320f0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c6RenderEv
int daObjKey_c::Render()
{
    int hidden = (mFlags & FLAG_HIDE) != 0;
    if (hidden)
        return 1;
    if (mAnimID != 0) {
        mModelAnim.Render(0);
    } else {
        mModelAnim.Render((Vector3 *)&mScaleX);
        /* mAnimID == 0 is already the else; the second test is in the ROM. */
        if (data_ov089_021328b4[mState] != 0 && mAnimID == 0)
            mModel.Render(0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c8BehaviorEv, 0x02132194 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c8BehaviorEv
int daObjKey_c::Behavior()
{
    Vector3 vec;
    Vector3 offset7;
    Vector3 offset;
    int animID = mAnimID;

    if (animID != 0) {
        if (animID == ANIM_CARRY) {
            Player *player = mPlayer;
            if (player != 0) {
                /* Through a pointer: player->mPosX folds the offset into each load. */
                s32 *pos = &player->mPosX;
                mPosX = pos[0];
                mPosY = pos[1];
                mPosZ = pos[2];
                mAngleY = mPlayer->mAngleY;
            }
            if (mModelAnim.Finished() == 0) {
                Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
                Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
                MulMat4x3Mat4x3(mModelAnim.data.transforms, &data_020a0e68, &data_020a0e68);
                vec.x = data_020a0e68.m[9];
                vec.y = data_020a0e68.m[10];
                vec.z = data_020a0e68.m[11];
                SubVec3(&vec, &mPosX, &vec);
                Vec3_LslInPlace(&vec, 3);
                AddVec3(&vec, &mPosX, &vec);
                /* First bone, word at +0xc. BMD_Bone has no field there. */
                vec.y = *(int *)((char *)mModelAnim.data.bones + 0xc) * BONE_Y_MUL + vec.y;
                vec.y = vec.y - Y_BONE;
                mParticleID[0] = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    mParticleID[0], FX_BURST_A, vec.x, vec.y, vec.z, 0, 0);
                mParticleID[1] = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    mParticleID[1], FX_BURST_B, vec.x, vec.y, vec.z, 0, 0);
            }
        }

        mModelAnim.Advance();
        UpdateModelTransform();
        if (mModelAnim.Finished()) {
            /* The flag keeps the ROM's moveq and movne pair. */
            int isKey = (actorID == ACTOR_OBJ_KEY);
            if (isKey != 0) {
                /* Anim 3's file (data_ov089_02132c40, id 0x44f) stays; the others die. */
                if (mModelAnim.file != (BCA_File *)data_ov089_02132c40.ptr)
                    MarkForDestruction();
            }
        }
        return 1;
    }

    if (UpdateYoshiEat(mWithMeshClsn)) {
        UpdateModelTransform();
        mdCcAcPos_c.Clear();
        return 1;
    }
    mEatingPlayer = 0;
    if (mSpinSpeed > SPIN_FAST)
        mSpinSpeed -= SPIN_STEP;
    else if (mSpinSpeed == 0)
        mSpinSpeed = SPIN_FAST;
    mAngleY += mSpinSpeed;
    UpdatePos(0);
    (this->*data_ov089_02132cec[mState])();
    UpdateModelTransform();
    mdCcAcPos_c.Clear();
    if (mState == KEY_KIND_STAR) {
        offset7.x = data_ov089_02132b40.x;
        offset7.y = data_ov089_02132b40.y;
        offset7.z = data_ov089_02132b40.z;
        mdCcAcPos_c.SetPosRelativeToActor(offset7);
    } else {
        offset.x = data_ov089_02132ca4.x;
        offset.y = data_ov089_02132ca4.y;
        offset.z = data_ov089_02132ca4.z;
        mdCcAcPos_c.SetPosRelativeToActor(offset);
    }
    mdCcAcPos_c.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c13InitResourcesEv, 0x021324a4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c13InitResourcesEv
int daObjKey_c::InitResources()
{
    Vector3 offset7;
    Vector3 offset;
    int kind = param1 & 7;
    mState = kind;
    LoadKeyModels(mState);
    Animation::LoadFile(*(SharedFilePtr *)&data_ov002_02110964);

    if (mState == KEY_KIND_STAR) {
        if (mModelAnim.SetFile((BMD_File *)((ObjKeyFile *)&data_ov002_0211094c)->ptr, 1, 1) == 0)
            return 0;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov002_02110964.ptr, ANIM_FLAGS, ANIM_SPEED, 0);
        mScaleX = SCALE_STAR;
        mScaleY = SCALE_STAR;
        mScaleZ = SCALE_STAR;
        offset7.x = data_ov089_02132b40.x;
        offset7.y = data_ov089_02132b40.y;
        offset7.z = data_ov089_02132b40.z;
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &offset7, CC_R, STAR_CC_H, CC_FLAGS, CC_VULN);
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, STAR_CLSN_R, 0, 0, 0);
        mSpinSpeed = SPIN_FAST;
        mVertAccel = 0;
        Sound::PlayBank3(SND_APPEAR, *(Vector3 *)&mCamSpacePosX);
    } else {
        Animation::LoadFile(*(SharedFilePtr *)&data_ov089_02132c60);
        Animation::LoadFile(*(SharedFilePtr *)&data_ov089_02132c40);
        Animation::LoadFile(*(SharedFilePtr *)&data_ov089_02132c70);
        Animation::LoadFile(*(SharedFilePtr *)&data_ov089_02132c48);
        if (mModelAnim.SetFile((BMD_File *)((ObjKeyFile *)data_ov089_02132894[mState])->ptr, 1, 1) == 0)
            return 0;
        {
            void *extra = data_ov089_021328b4[mState];
            if (extra != 0) {
                if (mModel.SetFile((BMD_File *)((ObjKeyFile *)extra)->ptr, 1, -1) == 0)
                    return 0;
            }
        }
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov002_02110964.ptr, ANIM_FLAGS, ANIM_SPEED, 0);
        mScaleX = SCALE_KEY;
        mScaleY = SCALE_KEY;
        mScaleZ = SCALE_KEY;
        mVertSpeed = POP_VY;
        offset.x = data_ov089_02132ca4.x;
        offset.y = data_ov089_02132ca4.y;
        offset.z = data_ov089_02132ca4.z;
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &offset, CC_R, KEY_CC_H, CC_FLAGS, CC_VULN);
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, KEY_CLSN_R, 0, 0, 0);
        mVertAccel = GRAVITY;
        mSpinSpeed = 0;
    }

    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mWithMeshClsn.SetLimMovFlag();
    mHomePos.x = mPosX;
    mHomePos.y = mPosY;
    mHomePos.z = mPosZ;
    mStateTimer = 0;
    mStep = 0;
    mBounceCount = 0;
    mTerminalVelocity = TERMINAL_VY;
    mAnimID = 0;
    mPlayer = 0;
    mParticleID[0] = mParticleID[1] = mParticleID[2] = 0;
    if (data_0209cef0 == 0)
        Event::ClearBit(EVENT_KEY);
    return 1;
}
